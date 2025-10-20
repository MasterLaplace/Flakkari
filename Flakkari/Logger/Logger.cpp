/*
** EPITECH PROJECT, 2023
** Flakkari
** File description:
** Logger
*/

#include "Logger.hpp"

namespace Flakkari {

#if defined(DEBUG) || defined(_DEBUG)
static Logger::Mode _mode = Logger::Mode::DEBUG;
#else
static Logger::Mode _mode = Logger::Mode::NORMAL;
#endif

void Logger::setMode(Logger::Mode mode) noexcept { _mode = mode; }

const std::string Logger::get_current_time() noexcept
{
    char buffer[80];
    struct tm timeInfo;

#if defined(_WIN32) || defined(_WIN64)
    auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    if (localtime_s(&timeInfo, &currentTime) != 0)
        return "localtime_s failed";
#else
    auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    timeInfo = *std::localtime(&currentTime);
#endif

    std::strftime(buffer, sizeof(buffer), "[ %Y-%m-%d %H:%M:%S ]", &timeInfo);

    return std::string(buffer);
}

#ifdef _WIN32
void setColor(WORD color) noexcept
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
#endif

const std::string Logger::fatal_error_message() noexcept
{
#if !defined(_WIN32) || !defined(_WIN64)
    return std::string(
        "\nThis is a fatal error, the program will now exit.\n"
        "Please check the following issues:\n" COLOR_BRIGHT_CYAN
        "\thttps://github.com/masterlaplace/Flakkari/issues?q=is%3Aissue+is%3Aopen+label%3Abug\n" COLOR_RESET
        "\nPlease report this error to the developers by opening an issue on GitHub:\n" COLOR_BRIGHT_CYAN
        "\thttps://github.com/MasterLaplace/Flakkari/issues/"
        "new?assignees=&labels=bug&projects=&template=bug_report.yml&title=[Bug%20::%20"
#    ifdef __APPLE__
        "macOS]"
#    elif __linux__
        "Linux]"
#    elif __unix__
        "Unix]"
#    elif __posix__
        "Posix]"
#    endif
        COLOR_RESET "\nThank you for your help!");

#else

    std::string message = std::string("\nThis is a fatal error, the program will now exit.\n"
                                      "Please check the following issues:\n");
    setColor(COLOR_BRIGHT_CYAN);
    message += "\thttps://github.com/masterlaplace/Flakkari/issues?q=is%3Aissue+is%3Aopen+label%3Abug\n";
    setColor(COLOR_RESET);
    message += "\nPlease report this error to the developers by opening an issue on GitHub:\n";
    setColor(COLOR_BRIGHT_CYAN);
    message += "\thttps://github.com/MasterLaplace/Flakkari/issues/"
               "new?assignees=&labels=bug&projects=&template=bug_report.yml&title=[Bug :: ";
#    if defined(_WIN32) || defined(_WIN64)
    message += "Windows]";
#    endif
    setColor(COLOR_RESET);
    message += "\nThank you for your help!";
    return message;
#endif
}

void Logger::log(int level, std::string message, int line, std::string file) noexcept
{
    if (_mode == Logger::Mode::SILENT && level != LOG_FATAL)
        return;

    if (_mode == Logger::Mode::NORMAL && level == LOG_DEBUG)
        return;

#if !defined(_WIN32) || !defined(_WIN64)
    std::string color = COLOR_RESET;
    std::string levelStr = "INFO";

    switch (level)
    {
    case LOG_INFO:
        color = COLOR_CYAN;
        levelStr = "INFO";
        break;
    case LOG_LOG:
        color = COLOR_GREEN;
        levelStr = "LOG";
        break;
    case LOG_DEBUG:
        color = COLOR_MAGENTA;
        levelStr = "DEBUG";
        break;
    case LOG_WARNING:
        color = COLOR_YELLOW;
        levelStr = "WARNING";
        break;
    case LOG_ERROR:
        color = COLOR_ORANGE;
        levelStr = "ERROR";
        break;
    case LOG_FATAL:
        color = COLOR_BRIGHT_RED;
        levelStr = "FATAL";
        break;
    }

    std::cout << get_current_time();
    std::cout << color << " [" << levelStr << "] " << message << " (" << file << ":" << line << ")" << COLOR_RESET
              << std::endl;

#else

    std::string levelStr = "INFO";
    std::cout << get_current_time();

    switch (level)
    {
    case LOG_INFO:
        levelStr = "INFO";
        setColor(COLOR_CYAN);
        break;
    case LOG_LOG:
        levelStr = "LOG";
        setColor(COLOR_GREEN);
        break;
    case LOG_DEBUG:
        levelStr = "DEBUG";
        setColor(COLOR_MAGENTA);
        break;
    case LOG_WARNING:
        levelStr = "WARNING";
        setColor(COLOR_YELLOW);
        break;
    case LOG_ERROR:
        levelStr = "ERROR";
        setColor(COLOR_ORANGE);
        break;
    case LOG_FATAL:
        levelStr = "FATAL";
        setColor(COLOR_BRIGHT_RED);
        break;
    }

    std::cout << " [" << levelStr << "] " << message << " (" << file << ":" << line << ")" << std::endl;
    setColor(COLOR_RESET);
#endif

    if (level == LOG_FATAL)
        std::cout << fatal_error_message() << std::endl;
}

void Logger::log(int level, std::string message) noexcept
{
    if (_mode == Logger::Mode::SILENT && level != LOG_FATAL)
        return;

    if (_mode == Logger::Mode::NORMAL && level == LOG_DEBUG)
        return;

#if !defined(_WIN32) || !defined(_WIN64)
    std::string color = COLOR_RESET;
    std::string levelStr = "INFO";

    switch (level)
    {
    case LOG_INFO:
        color = COLOR_CYAN;
        levelStr = "INFO";
        break;
    case LOG_LOG:
        color = COLOR_GREEN;
        levelStr = "LOG";
        break;
    case LOG_DEBUG:
        color = COLOR_MAGENTA;
        levelStr = "DEBUG";
        break;
    case LOG_WARNING:
        color = COLOR_YELLOW;
        levelStr = "WARNING";
        break;
    case LOG_ERROR:
        color = COLOR_ORANGE;
        levelStr = "ERROR";
        break;
    case LOG_FATAL:
        color = COLOR_BRIGHT_RED;
        levelStr = "FATAL";
        break;
    }

    std::cout << get_current_time();
    std::cout << color << " [" << levelStr << "] " << message << COLOR_RESET << std::endl;

#else

    std::string levelStr = "INFO";
    std::cout << get_current_time();

    switch (level)
    {
    case LOG_INFO:
        levelStr = "INFO";
        setColor(COLOR_CYAN);
        break;
    case LOG_LOG:
        levelStr = "LOG";
        setColor(COLOR_GREEN);
        break;
    case LOG_DEBUG:
        levelStr = "DEBUG";
        setColor(COLOR_MAGENTA);
        break;
    case LOG_WARNING:
        levelStr = "WARNING";
        setColor(COLOR_YELLOW);
        break;
    case LOG_ERROR:
        levelStr = "ERROR";
        setColor(COLOR_ORANGE);
        break;
    case LOG_FATAL:
        levelStr = "FATAL";
        setColor(COLOR_BRIGHT_RED);
        break;
    }

    std::cout << " [" << levelStr << "] " << message << std::endl;
    setColor(COLOR_RESET);
#endif

    if (level == LOG_FATAL)
        std::cout << fatal_error_message() << std::endl;
}

} /* namespace Flakkari */
