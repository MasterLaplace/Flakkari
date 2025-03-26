#include "GameDownloader.hpp"

using namespace Flakkari::Internals;

namespace Flakkari::Internals {

const std::regex GameDownloader::JSON_FILE_NAME(".*\\.json");

const std::string FLAKKARI_GAME_URL = "https://api.github.com/repos/MasterLaplace/Flakkari/contents/Games";
const std::string FLAKKARI_FILE_URL = "https://raw.githubusercontent.com/MasterLaplace/Flakkari/main/Games/";

GameDownloader::GameDownloader(const std::string &gameDir, int timeout) : _gameDir(gameDir), _timeout(timeout)
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

        if (remoteGamesURL.empty() || localGames.empty())
            continue;

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
}

void GameDownloader::removeGames(const std::vector<std::string> &remoteGamesURL,
                                 const std::vector<std::string> &localGames)
{
}

void GameDownloader::removeGame(const std::string &gameName)
{
}

bool GameDownloader::downloadGame(const std::string &gameName, const std::string &jsonBuffer)
{
    return true;
}

std::vector<std::string> GameDownloader::listGames()
{
    FLAKKARI_LOG_INFO("Listing remote games");

    std::vector<std::string> games;

    return games;
}

std::vector<std::string> GameDownloader::listLocalGames()
{
    FLAKKARI_LOG_INFO("Listing local games");

    std::vector<std::string> games;

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
    return true;
}

bool GameDownloader::sendCommand(std::string &readBuffer, const std::string &url)
{
    return true;
}

bool GameDownloader::cloneOrUpdateRepository(const std::string &repoUrl, const std::string &localPath)
{
    return true;
}

bool GameDownloader::updateSubModuleRecursivelyFollowingGitSubmoduleFile(git_repository *repo)
{
    FLAKKARI_LOG_DEBUG("Submodules updated successfully.");

    return true;
}

} // namespace Flakkari::Internals
