/**************************************************************************
 * Flakkari Library v0.7.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file GameDownloader.hpp
 * @brief GameDownloader is a class that downloads games from the remote
 *        repository and stores them in the Games folder.
 *        It also updates the games if they are already present and removes them
 *        if they are not present in the remote repository.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.7.0
 * @date 2024-12-09
 **************************************************************************/

#ifndef GAMEDOWNLOADER_HPP_
#define GAMEDOWNLOADER_HPP_

#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <regex>

#include "../Game/GameManager.hpp"
#include "flakkari_config.h"

#include <curl/curl.h>
#include <git2.h>

namespace Flakkari::Internals {

class GameDownloader {
public:
    /**
     * @brief Construct a new Game Downloader object
     *
     */
    GameDownloader(const std::string &gameDir, uint32_t timeout = 5);

    /**
     * @brief Destroy the Game Downloader object
     *
     */
    ~GameDownloader();

    /**
     * @brief Start the Game Downloader
     *
     */
    void start();

    /**
     * @brief Stop the Game Downloader
     *
     */
    void stop();

private:
    /**
     * @brief Download the games from the remote repository
     *
     * @param gameName Game to download
     */
    void downloadGames(const std::vector<std::string> &remoteGamesURL, const std::vector<std::string> &localGames);

    /**
     * @brief Remove the games from the Games folder
     *
     * @param gameName Game to remove
     */
    void removeGames(const std::vector<std::string> &remoteGamesURL, const std::vector<std::string> &localGames);

    /**
     * @brief Download a game from the remote repository
     *
     * @param gameName Game to download
     * @param jsonBuffer Buffer to store the response
     */
    [[nodiscard]] bool downloadGame(const std::string &gameName, const std::string &jsonBuffer);

    /**
     * @brief Remove a game from the Games folder and the GameManager
     *
     * @param gameName  Game to remove
     */
    void removeGame(const std::string &gameName);

    /**
     * @brief List all games present in the remote repository
     *
     * @return std::vector<std::string> List of games present in the remote repository
     */
    std::vector<std::string /*url*/> listGames();

    /**
     * @brief List all games present in the Games folder
     *
     * @return std::vector<std::string> List of games present in the Games folder
     */
    std::vector<std::string> listLocalGames();

    /**
     * @brief Send a command to the remote repository
     *
     * @param readBuffer Buffer to store the response
     * @param url URL to send the command to
     * @return true Command sent
     * @return false Command not sent
     */
    [[nodiscard]] bool sendCommand(std::string &readBuffer, const std::string &url);

    /**
     * @brief Download a file from the remote repository
     *
     * @param url URL to download the file from
     * @param outputPath Path to store the file
     * @return true File downloaded
     * @return false File not downloaded
     */
    [[nodiscard]] bool downloadFile(const std::string &url, std::string &readBuffer, const std::string &outputPath);

    /**
     * @brief Write the response to a buffer
     *
     * @param contents Response content
     * @param size Size of the content
     * @param nmemb Number of members
     * @param outBuffer Buffer to store the response
     * @return size_t Size of the response
     */
    static size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *outBuffer);

    /**
     * @brief Clone or update a repository
     *
     * @param repoUrl URL of the repository
     * @param localPath Path to store the repository
     * @return true Repository cloned or updated
     * @return false Repository not cloned or updated
     */
    [[nodiscard]] bool cloneOrUpdateRepository(const std::string &repoUrl, const std::string &localPath);

    /**
     * @brief Update a submodule recursively
     *
     * @param repo Repository to update
     * @return true Submodule updated
     * @return false Submodule not updated
     */
    [[nodiscard]] bool updateSubModuleRecursivelyFollowingGitSubmoduleFile(git_repository *repo);

private:
    static const std::regex JSON_FILE_NAME;
    std::string _gameDir;
    std::thread _thread;
    uint32_t _timeout;
    std::atomic<bool> _running = false;
    std::condition_variable _cv;
    std::mutex _mutex;
    std::string _readBuffer;
    CURL *_curl;
    CURLcode _ret;
};

} // namespace Flakkari::Internals

#endif /* !GAMEDOWNLOADER_HPP_ */
