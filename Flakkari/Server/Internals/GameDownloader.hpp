/**************************************************************************
 * Flakkari Library v0.5.0
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
 * @version 0.5.0
 * @date 2024-12-09
 **************************************************************************/

#ifndef GAMEDOWNLOADER_HPP_
#define GAMEDOWNLOADER_HPP_

#include <cstdio>
#include <cstdlib>
#include <regex>

#include "../Game/GameManager.hpp"
#include "config.h.in"


namespace Flakkari::Internals {

class GameDownloader {
public:
    /**
     * @brief Construct a new Game Downloader object
     *
     */
    GameDownloader(const std::string &gameDir) : _gameDir(gameDir) {}

    /**
     * @brief Destroy the Game Downloader object
     *
     */
    ~GameDownloader() = default;

private:
    std::string _gameDir;
};

} // namespace Flakkari::Internals

#endif /* !GAMEDOWNLOADER_HPP_ */
