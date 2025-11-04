#include "GameDownloader.hpp"
#include <iostream>

using namespace Flakkari::Internals;

namespace Flakkari::Internals {

const std::regex GameDownloader::JSON_FILE_NAME(".*\\.json");

const std::string FLAKKARI_GAME_URL = "https://api.github.com/repos/MasterLaplace/Flakkari/contents/Games";
const std::string FLAKKARI_FILE_URL = "https://raw.githubusercontent.com/MasterLaplace/Flakkari/main/Games/";

static int submoduleForeachCallback(git_submodule *submodule, const char *name, void *payload)
{
    FLAKKARI_LOG_DEBUG("Updating submodule: " + std::string(name));

    auto *update_options = static_cast<git_submodule_update_options *>(payload);

    if (git_submodule_update(submodule, 1, update_options) != 0)
    {
        FLAKKARI_LOG_ERROR("Failed to update submodule: " + std::string(name));
        return -1;
    }

    return 0;
}

static int cred_acquire_cb(git_cred **out, const char *url, const char *username_from_url, unsigned int allowed_types,
                           void *payload)
{
    return git_cred_userpass_plaintext_new(out, "username", "password");
}

GameDownloader::GameDownloader(const std::string &gameDir, uint32_t timeout) : _gameDir(gameDir), _timeout(timeout)
{
    _running.store(true, std::memory_order_release);
    _thread = std::thread(&GameDownloader::start, this);
}

GameDownloader::~GameDownloader() { _thread.join(); }

void GameDownloader::start()
{
    while (_running)
    {
        std::unique_lock<std::mutex> lock(_mutex);

        if (_cv.wait_for(lock, std::chrono::seconds(_timeout),
                         [this] { return !_running.load(std::memory_order_acquire); }))
            break;

        auto remoteGamesURL = listGames();
        auto localGames = listLocalGames();

        downloadGames(remoteGamesURL, localGames);
        removeGames(remoteGamesURL, localGames);
    }
}

void Flakkari::Internals::GameDownloader::stop()
{
    _running.store(false, std::memory_order_release);

    _cv.notify_one();
}

void GameDownloader::downloadGames(const std::vector<std::string> &remoteGamesURL,
                                   const std::vector<std::string> &localGames)
{
    for (const auto &game : remoteGamesURL)
    {
        if (std::find(localGames.begin(), localGames.end(), game) != localGames.end())
            continue;

        if (!downloadFile(FLAKKARI_FILE_URL + game, _readBuffer, _gameDir + "/" + game))
            continue;

        FLAKKARI_LOG_INFO("Game downloaded: " + game);

        std::string gameBaseName = game;
        if (gameBaseName.ends_with(".json"))
            gameBaseName = gameBaseName.substr(0, gameBaseName.length() - 5);

        if (!downloadGame(gameBaseName, _readBuffer))
            continue;

        auto instance = GameManager::GetInstance();
        if (instance.addGame(gameBaseName) == 1)
            instance.updateGame(gameBaseName);
        GameManager::UnlockInstance();
    }
}

void GameDownloader::removeGames(const std::vector<std::string> &remoteGamesURL,
                                 const std::vector<std::string> &localGames)
{
    for (const auto &game : localGames)
    {
        if (std::find(remoteGamesURL.begin(), remoteGamesURL.end(), game) != remoteGamesURL.end())
            continue;

        FLAKKARI_LOG_INFO("Game not found in remote repository: " + game + " removing it");

        removeGame(game);
    }
}

void GameDownloader::removeGame(const std::string &gameName)
{
    FLAKKARI_LOG_INFO("Removing game: " + gameName);

    std::string fileName = _gameDir + "/" + gameName + ".json";

    if (!std::filesystem::exists(fileName))
    {
        FLAKKARI_LOG_WARNING("Game does not exist: " + gameName);
        return;
    }

    if (std::filesystem::remove(fileName))
    {
        FLAKKARI_LOG_ERROR("Failed to remove game: " + gameName);
        return;
    }

    GameManager::GetInstance().removeGame(gameName);
    GameManager::UnlockInstance();

    FLAKKARI_LOG_INFO("Game removed: " + gameName);
}

bool GameDownloader::downloadGame(const std::string &gameName, const std::string &jsonBuffer)
{
    nlohmann::json jsonObject = nlohmann::json::parse(jsonBuffer);
    std::string repoPath = _gameDir + "/" + gameName;

    if (!cloneOrUpdateRepository(jsonObject["url"], repoPath))
        return false;

    FLAKKARI_LOG_INFO("Game cloned or updated: " + gameName);
    return true;
}

std::vector<std::string> GameDownloader::listGames()
{
    FLAKKARI_LOG_INFO("Listing remote games");

    std::string result;
    if (!sendCommand(result, FLAKKARI_GAME_URL))
        return FLAKKARI_LOG_ERROR("Failed to list remote games"), std::vector<std::string>();

    std::vector<std::string> games;

    try
    {
        nlohmann::json jsonResponse = nlohmann::json::parse(result);

        for (const auto &item : jsonResponse)
        {
            if (item.contains("path") && item.contains("type"))
            {
                std::string path = item["path"];
                std::string type = item["type"];

                if (type == "file" && path.starts_with("Games/"))
                {
                    std::string filename = path.substr(6);

                    if (filename.ends_with(".json"))
                    {
                        games.push_back(filename);
                        std::cout << "Found game: " << filename << std::endl;
                    }
                }
            }
        }
    }
    catch (const nlohmann::json::exception &e)
    {
        FLAKKARI_LOG_ERROR("Failed to parse JSON response: " + std::string(e.what()));
        FLAKKARI_LOG_DEBUG("Raw response: " + result);
        return games;
    }

    return games;
}

std::vector<std::string> GameDownloader::listLocalGames()
{
    FLAKKARI_LOG_INFO("Listing local games");

    std::vector<std::string> games;

    for (const auto &entry : std::filesystem::directory_iterator(_gameDir))
    {
        if (std::regex_match(entry.path().string(), JSON_FILE_NAME))
        {
            games.push_back(entry.path().filename().string());
            FLAKKARI_LOG_INFO(entry.path().filename().string());
        }
        else if (entry.is_directory())
        {
            games.push_back(entry.path().filename().string());
            FLAKKARI_LOG_INFO(entry.path().filename().string());
        }
    }

    if (games.empty())
        FLAKKARI_LOG_WARNING("Failed to list local games");

    return games;
}

size_t GameDownloader::writeCallback(void *contents, size_t size, size_t nmemb, std::string *outBuffer)
{
    size_t totalSize = size * nmemb;

    if (outBuffer)
    {
        outBuffer->append((char *) contents, totalSize);
        return totalSize;
    }

    return FLAKKARI_LOG_ERROR("Failed to write to buffer"), 0;
}

bool GameDownloader::downloadFile(const std::string &url, std::string &readBuffer, const std::string &outputPath)
{
    std::ofstream outFile(outputPath, std::ios::binary | std::ios::trunc);
    readBuffer.clear();

    if (!outFile.is_open())
        return FLAKKARI_LOG_ERROR("Failed to open file: " + outputPath), false;

    if (!(_curl = curl_easy_init()))
        return FLAKKARI_LOG_ERROR("Failed to initialize curl"), false;

    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(_curl, CURLOPT_USERAGENT, "curl/8.2.1");

    if ((_ret = curl_easy_perform(_curl)) != CURLE_OK)
        FLAKKARI_LOG_ERROR("curl_easy_perform() failed: " + std::string(curl_easy_strerror(_ret)));
    else
    {
        outFile.write(readBuffer.c_str(), readBuffer.size());
        FLAKKARI_LOG_DEBUG("File downloaded: " + outputPath);
    }

    curl_easy_cleanup(_curl);
    outFile.close();
    return true;
}

bool GameDownloader::sendCommand(std::string &readBuffer, const std::string &url)
{
    struct curl_slist *headers = NULL;
    readBuffer.clear();

    if ((headers = curl_slist_append(headers, "Accept: application/vnd.github.v3+json")) == NULL)
        return FLAKKARI_LOG_ERROR("Failed to append headers"), false;

    if ((_curl = curl_easy_init()))
    {
        curl_easy_setopt(_curl, CURLOPT_BUFFERSIZE, 102400L);
        curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(_curl, CURLOPT_USERAGENT, "curl/8.2.1");
        curl_easy_setopt(_curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(_curl, CURLOPT_HTTP_VERSION, (long) CURL_HTTP_VERSION_2TLS);
        curl_easy_setopt(_curl, CURLOPT_FTP_SKIP_PASV_IP, 1L);
        curl_easy_setopt(_curl, CURLOPT_TCP_KEEPALIVE, 1L);
        curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, GameDownloader::writeCallback);
        curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(_curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        if ((_ret = curl_easy_perform(_curl)) != CURLE_OK)
            FLAKKARI_LOG_ERROR("curl_easy_perform() failed: " + std::string(curl_easy_strerror(_ret)));
        else
            FLAKKARI_LOG_DEBUG("Response: " + readBuffer);

        curl_easy_cleanup(_curl);
    }
    else
        FLAKKARI_LOG_ERROR("Failed to initialize curl");

    curl_slist_free_all(headers);

    return true;
}

bool GameDownloader::cloneOrUpdateRepository(const std::string &repoUrl, const std::string &localPath)
{
    git_libgit2_init();

    if (std::filesystem::exists(localPath))
    {
        git_repository *repo = nullptr;

        if (git_repository_open(&repo, localPath.c_str()) != 0)
        {
            FLAKKARI_LOG_WARNING("Directory exists but is not a valid Git repository: " + localPath);
            FLAKKARI_LOG_INFO("Removing directory and cloning fresh repository");

            try
            {
                std::filesystem::remove_all(localPath);
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                FLAKKARI_LOG_ERROR("Failed to remove directory: " + std::string(e.what()));
                git_libgit2_shutdown();
                return false;
            }
        }
        else
        {
            git_remote *remote = nullptr;

            if (git_remote_lookup(&remote, repo, "origin") == 0)
            {
                FLAKKARI_LOG_DEBUG("Pulling updates from remote: " + repoUrl);

                git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;

                if (git_remote_fetch(remote, nullptr, &fetch_opts, nullptr) != 0)
                {
                    FLAKKARI_LOG_ERROR("Failed to fetch updates from remote.");
                    git_remote_free(remote);
                    git_repository_free(repo);
                    git_libgit2_shutdown();
                    return false;
                }

                git_remote_free(remote);

                if (repo && !updateSubModuleRecursivelyFollowingGitSubmoduleFile(repo))
                {
                    git_repository_free(repo);
                    git_libgit2_shutdown();
                    return false;
                }

                FLAKKARI_LOG_DEBUG("Repository updated successfully.");
                git_repository_free(repo);
                git_libgit2_shutdown();
                return true;
            }
            else
            {
                FLAKKARI_LOG_ERROR("Failed to find remote 'origin'.");
                git_repository_free(repo);
            }
        }
    }

    git_repository *repo = nullptr;

    FLAKKARI_LOG_DEBUG("Cloning repository: " + repoUrl + " to: " + localPath);

    if (git_clone(&repo, repoUrl.c_str(), localPath.c_str(), nullptr) != 0)
    {
        FLAKKARI_LOG_ERROR("Failed to clone repository: " + repoUrl);
        git_libgit2_shutdown();
        return false;
    }

    FLAKKARI_LOG_DEBUG("Repository cloned successfully to: " + localPath);

    if (repo && !updateSubModuleRecursivelyFollowingGitSubmoduleFile(repo))
    {
        git_repository_free(repo);
        git_libgit2_shutdown();
        return false;
    }

    git_repository_free(repo);
    git_libgit2_shutdown();
    return true;
}

bool GameDownloader::updateSubModuleRecursivelyFollowingGitSubmoduleFile(git_repository *repo)
{
    git_submodule *submodule = nullptr;
    git_submodule_update_options submodule_update_options = GIT_SUBMODULE_UPDATE_OPTIONS_INIT;
    submodule_update_options.fetch_opts.callbacks.credentials = cred_acquire_cb;
    submodule_update_options.fetch_opts.callbacks.payload = nullptr;

    git_submodule_foreach(repo, submoduleForeachCallback, &submodule_update_options);

    FLAKKARI_LOG_DEBUG("Submodules updated successfully.");

    return true;
}

} // namespace Flakkari::Internals
