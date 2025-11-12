/**************************************************************************
 * Flakkari Library v0.9.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Logger.hpp
 * @brief Logger class header. Can be used to log messages.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.9.0
 * @date 2023-12-19
 **************************************************************************/

#ifndef FLAKKARI_LOGGER_HPP_
#define FLAKKARI_LOGGER_HPP_

#include "flakkari_config.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#define LOG_INFO    0
#define LOG_LOG     1
#define LOG_DEBUG   2
#define LOG_WARNING 3
#define LOG_ERROR   4
#define LOG_FATAL   5

#define FLAKKARI_LOG(level, message)  Flakkari::Logger::log(level, message, __LINE__, __FILE__)
#define FLAKKARI_LOG_INFO(message)    FLAKKARI_LOG(LOG_INFO, message)
#define FLAKKARI_LOG_LOG(message)     FLAKKARI_LOG(LOG_LOG, message)
#define FLAKKARI_LOG_DEBUG(message)   FLAKKARI_LOG(LOG_DEBUG, message)
#define FLAKKARI_LOG_WARNING(message) FLAKKARI_LOG(LOG_WARNING, message)
#define FLAKKARI_LOG_ERROR(message)   FLAKKARI_LOG(LOG_ERROR, message)
#define FLAKKARI_LOG_FATAL(message)   FLAKKARI_LOG(LOG_FATAL, message)

#ifdef FLAKKARI_SYSTEM_WINDOWS
#    define STD_ERROR                                                                                                  \
        []() -> std::string {                                                                                          \
            char buffer[256];                                                                                          \
            strerror_s(buffer, sizeof(buffer), errno);                                                                 \
            return std::string(buffer);                                                                                \
        }()

#    define SPECIAL_ERROR std::to_string(WSAGetLastError())
#else
#    define STD_ERROR     std::string(::strerror(errno))
#    define SPECIAL_ERROR STD_ERROR
#endif

#ifdef FLAKKARI_SYSTEM_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    define _WINSOCK_DEPRECATED_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#    include <windows.h>

#    define COLOR_RESET          7
#    define COLOR_RED            12
#    define COLOR_GREEN          10
#    define COLOR_YELLOW         14
#    define COLOR_BLUE           9
#    define COLOR_MAGENTA        13
#    define COLOR_CYAN           11
#    define COLOR_WHITE          15
#    define COLOR_ORANGE         208
#    define COLOR_BRIGHT_RED     FOREGROUND_RED | FOREGROUND_INTENSITY
#    define COLOR_BRIGHT_GREEN   FOREGROUND_GREEN | FOREGROUND_INTENSITY
#    define COLOR_BRIGHT_YELLOW  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#    define COLOR_BRIGHT_BLUE    FOREGROUND_BLUE | FOREGROUND_INTENSITY
#    define COLOR_BRIGHT_MAGENTA FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY
#    define COLOR_BRIGHT_CYAN    FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
#    define COLOR_BRIGHT_WHITE   FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY

#else

#    define COLOR_RESET          "\033[0m"
#    define COLOR_RED            "\033[31m"
#    define COLOR_GREEN          "\033[32m"
#    define COLOR_YELLOW         "\033[33m"
#    define COLOR_BLUE           "\033[34m"
#    define COLOR_MAGENTA        "\033[35m"
#    define COLOR_CYAN           "\033[36m"
#    define COLOR_WHITE          "\033[37m"
#    define COLOR_ORANGE         "\033[38;5;208m"
#    define COLOR_BRIGHT_RED     "\033[91m"
#    define COLOR_BRIGHT_GREEN   "\033[92m"
#    define COLOR_BRIGHT_YELLOW  "\033[93m"
#    define COLOR_BRIGHT_BLUE    "\033[94m"
#    define COLOR_BRIGHT_MAGENTA "\033[95m"
#    define COLOR_BRIGHT_CYAN    "\033[96m"
#    define COLOR_BRIGHT_WHITE   "\033[97m"
#endif

namespace Flakkari {

class Logger {
public:
    enum class Mode {
        SILENT,
        NORMAL,
        DEBUG
    };

public:
    static void setMode(Mode mode) noexcept;
    static const std::string get_current_time() noexcept;
    static const std::string fatal_error_message() noexcept;
    static void log(int level, std::string message, int line, std::string file = "") noexcept;
    static void log(int level, std::string message) noexcept;
};

} /* namespace Flakkari */

#endif /* !FLAKKARI_LOGGER_HPP_ */
