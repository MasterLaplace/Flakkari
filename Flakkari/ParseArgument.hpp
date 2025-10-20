/**************************************************************************
 * Flakkari Library v0.7.1
 *
 * Flakkari Library is a C++ Library for Network.
 * @file ParseArgument.hpp
 * @brief This file contains the ParseArgument class. It is used to parse the
 *        arguments of the server. It is used to get the game directory, the ip
 *        and the port of the server.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.7.1
 * @date 2024-12-07
 **************************************************************************/

#ifndef ARGUMENTS_HPP_
#define ARGUMENTS_HPP_

#include "Server/UDPServer.hpp"

#if defined(FLAKKARI_SYSTEM_WINDOWS)
#    include <iphlpapi.h>
#else
#    include <ifaddrs.h>
#endif

/**
 * @namespace Flakkari
 * The Flakkari namespace contains classes and functions related to the Flakkari project.
 */
namespace Flakkari {

/**
 * @class ParseArgument
 * @brief A class to parse command-line arguments for the Flakkari application.
 *
 * This class is responsible for parsing command-line arguments to retrieve
 * the game directory, IP address, and port number.
 */
class ParseArgument {
public:
    /**
     * @brief Constructs a ParseArgument object.
     * @param ac The argument count.
     * @param av The argument vector.
     */
    ParseArgument(int ac, const char *av[]);

    /**
     * @brief Destructor for ParseArgument.
     */
    ~ParseArgument() = default;

    /**
     * @brief Gets the game directory.
     * @return A constant reference to the game directory string.
     */
    const std::string &getGameDir() const;

    /**
     * @brief Gets the IP address.
     * @return A constant reference to the IP address string.
     */
    const std::string &getIp() const;

    /**
     * @brief Gets the port number.
     * @return The port number as an unsigned short.
     */
    unsigned short getPort() const;

    /**
     * @brief Gets the dry run flag.
     * @return True if the dry run flag is set, false otherwise.
     */
    bool isDryRun() const { return _dryRun; }

private:
    /**
     * @brief Retrieves IPv4 addresses.
     * @return True if successful, false otherwise.
     */
    bool GetIPv4Addresses();

    /**
     * @brief Retrieves IPv6 addresses.
     * @return True if successful, false otherwise.
     */
    bool GetIPv6Addresses();

    /**
     * @brief Retrieves the game directory from the environment.
     */
    void GetGameDirEnv();

private:
    std::string _gameDir;     ///< The game directory.
    std::string _ip;          ///< The IP address, default is "localhost".
    unsigned short _port = 0; ///< The port number, default is 8081.
    bool _dryRun = false;     ///< Flag to indicate a dry run.

    static constexpr const char *HELP_MESSAGE =
        "Usage: ./r-type_server <gameDir> <ip> <port>\n"
        "  -g|--games <gameDir>  The directory of the games folder\n"
        "  -i|--ip <ip>          The ip to bind the server to (default: localhost)\n"
        "  -p|--port <port>      The port to bind the server to (default: 8081)\n"
        "  -dr|--dry-run         Run the server in dry-run mode\n"
        "  -d|--default          Use default values (Games, localhost, 8081)\n"
        "  -v|--version          Display the version of the Flakkari Library\n"
        "  -h|--help             Display this help message\n\n"
        "Example: ./r-type_server Games localhost 8081\n\n"
        "More information at https://github.com/MasterLaplace/Falkkari\n"
        "Flakkari Library v" FLAKKARI_VERSION_STRING " - © 2024 MasterLaplace";
};

} /* namespace Flakkari */

#endif /* !ARGUMENTS_HPP_ */
