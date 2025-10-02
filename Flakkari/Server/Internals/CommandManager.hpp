/**************************************************************************
 * Flakkari Library v0.6.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file CommandManager.hpp
 * @brief CommandManager is a class that handles commands from the admin user
 *        and executes them.
 *
 * @note Need to export `FLAKKARI_PASSWORD` environment variable to use some
 *       commands.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.6.0
 * @date 2024-01-05
 **************************************************************************/

#ifndef COMMANDMANAGER_HPP_
#define COMMANDMANAGER_HPP_

#include <iostream>
#include <regex>

#include "../Game/GameManager.hpp"
#include "flakkari_config.h"

namespace Flakkari::Internals {

class CommandManager {
public:
    static std::regex PASSWORD_REGEX;
    static std::regex ADD_GAME_REGEX;
    static std::regex UPDATE_GAME_REGEX;
    static std::regex REMOVE_GAME_REGEX;
    static bool _unlocked;

public:
    /**
     * @brief Handle a command from the admin user and execute it.
     *
     * @note Need to export `FLAKKARI_PASSWORD` environment variable to use some
     *       commands.
     */
    static void handleCommand();

private:
    /**
     * @brief Handle a command from the admin user and execute it.
     *
     * @note dont need admin permission to use this command.
     *
     * @param input  The command to execute.
     * @return true  If the command was executed.
     * @return false  If the command was not executed.
     *
     * @example version
     * @example help
     */
    [[nodiscard]] static bool handleOpenCommand(const std::string &input);

    /**
     * @brief Handle a command from the admin user and execute it.
     *
     * @note Need to export `FLAKKARI_PASSWORD` environment variable to use some
     *      commands.
     *
     * @param input  The command to execute.
     * @return true  If the command was executed.
     * @return false  If the command was not executed.
     *
     * @example unlock <password>
     * @example lock
     */
    [[nodiscard]] static bool handlePasswordCommand(const std::string &input);

    /**
     * @brief Handle a command from the admin user and execute it.
     *
     * @note Need to export `FLAKKARI_PASSWORD` environment variable to use some
     *       commands.
     *
     * @param input  The command to execute.
     * @return true  If the command was executed.
     * @return false  If the command was not executed.
     *
     * @example addGame <gameName>
     * @example updateGame <gameName>
     * @example removeGame <gameName>
     * @example listGames
     * @example exit
     */
    [[nodiscard]] static bool handleAdminCommand(const std::string &input);
};

} // namespace Flakkari::Internals

#endif /* !COMMANDMANAGER_HPP_ */
