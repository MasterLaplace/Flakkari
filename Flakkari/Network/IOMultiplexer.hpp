/**************************************************************************
 * Flakkari Library v0.2.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file IOMultiplexer.hpp
 * @brief This file contains the IOMultiplexer interface and the different
 *        IOMultiplexer implementations for different platforms.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.2.0
 * @date 2023-12-23
 **************************************************************************/

#ifndef IOMULTIPLEXER_HPP_
#define IOMULTIPLEXER_HPP_

// if im on windows, define all compatible IOMultiplexer
#ifdef _WIN32
    #define _PSELECT_
    #define _EPOLL_
    #define _KQUEUE_
    #define _IO_URING_
#else
    #define _PSELECT_
    #define _PPOLL_
    #define _EPOLL_
#endif

#include "Socket.hpp"
#include <vector>

namespace Flakkari::Network {

#if defined(_PSELECT_)
#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <winsock.h>

    #pragma comment(lib, "Ws2_32.lib")
#elif defined(__APPLE__)
    #include <sys/select.h>
    #include <sys/time.h>
    #include <sys/types.h>
#else
    #include <sys/select.h>
    #include <sys/time.h>
    #include <sys/types.h>
#endif

/**
 * @brief PSELECT is a class that represents a PSELECT
 *
 * @class PSELECT
 * @implements IOMultiplexer
 * @see IOMultiplexer
 *
 * @example "PSELECT example":
 * @code
 * auto socket = std::make_shared<Socket>(12345, Address::IpType::IPv4, Address::SocketType::UDP);
 * socket->bind();
 *
 * auto io = std::make_unique<PSELECT>();
 * io->addSocket(socket->getSocket());
 *
 * while (true) {
 *    int result = io->wait();
 *    if (result > 0) {
 *       for (auto &fd : *io) {
 *         if (io->isReady(fd)) {
 *          // do something
 *         }
 *       }
 *    }
 * }
 * @endcode
 */
class PSELECT {
    public:
        using FileDescriptor = int;

    public:
        PSELECT(int fileDescriptor);
        PSELECT();
        ~PSELECT() = default;

        /**
         * @brief Add a socket to the PSELECT list
         *
         * @param socket  The socket to add to the list
         */
        void addSocket(FileDescriptor socket);

        /**
         * @brief Remove a socket from the PSELECT list
         *
         * @param socket  The socket to remove from the list
         */
        void removeSocket(FileDescriptor socket);

        /**
         * @brief Wait for an event to happen on a socket or timeout
         *
         * @return int  The number of events that happened or -1 if an error occured or 0 if the timeout expired (EINTR)
         * @see pselect
         * @see errno
         */
        int wait();

        std::vector<FileDescriptor>::iterator begin() { return _sockets.begin(); }
        std::vector<FileDescriptor>::iterator end() { return _sockets.end(); }

        /**
         * @brief Check if a socket is ready to read from
         *
         * @param socket  The socket to check
         * @return true  If the socket is ready
         * @return false  If the socket is not ready
         */
        [[nodiscard]] bool isReady(FileDescriptor socket);

    protected:
    private:
        fd_set _fds;
        std::vector<FileDescriptor> _sockets;
        FileDescriptor _maxFd = 0;
        struct timespec _timeout = {0, 0};
};
#endif

#if defined(_PPOLL_)
#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <poll.h>
    #pragma comment(lib, "Ws2_32.lib")
#elif defined(__APPLE__)
    #include <sys/poll.h>
#else
    #include <sys/poll.h>
#endif

/**
 * @brief PPOLL is a class that represents a PPOLL
 *
 * @class PPOLL
 * @implements IOMultiplexer
 * @see IOMultiplexer
 *
 * @example "PPOLL example":
 * @code
 * auto socket = std::make_shared<Socket>(12345, Address::IpType::IPv4, Address::SocketType::UDP);
 * socket->bind();
 *
 * auto io = std::make_unique<PPOLL>();
 * io->addSocket(socket->getSocket());
 *
 * while (true) {
 *    int result = io->wait();
 *    if (result > 0) {
 *       for (auto &fd : *io) {
 *         if (io->isReady(fd)) {
 *          // do something
 *         }
 *       }
 *    }
 * }
 * @endcode
 */
class PPOLL {
    public:
        using FileDescriptor = int;
        using pollfd = struct pollfd;
        using event_t = short int;
        using revents_t = short int;
        using nfds_t = unsigned long int;

    public:
        PPOLL(int fileDescriptor, event_t events);
        PPOLL();
        ~PPOLL() = default;

        /**
         * @brief Add a socket to the PPOLL list
         *
         * @param socket  The socket to add to the list
         */
        void addSocket(FileDescriptor socket);

        /**
         * @brief Add a socket to the PPOLL list with specific events
         *
         * @param socket  The socket to add to the list
         * @param events  The events to listen to on the socket (POLLIN | POLLPRI)
         */
        void addSocket(FileDescriptor socket, event_t events);

        /**
         * @brief Remove a socket from the PPOLL list
         *
         * @param socket  The socket to remove from the list
         */
        void removeSocket(FileDescriptor socket);

        /**
         * @brief Wait for an event to happen on a socket or timeout
         *
         * @return int  The number of events that happened or -1 if an error occured or 0 if the timeout expired (EINTR)
         * @see ppoll
         * @see errno
         */
        int wait();

        pollfd &operator[](std::size_t index);
        pollfd &operator[](FileDescriptor socket);

        std::vector<pollfd>::iterator begin() { return _pollfds.begin(); }
        std::vector<pollfd>::iterator end() { return _pollfds.end(); }

        /**
         * @brief Check if a socket is ready to read from
         *
         * @param fd  The pollfd to check
         * @return true  If the socket is ready
         * @return false  If the socket is not ready
         */
        [[nodiscard]] bool isReady(pollfd fd);

        /**
         * @brief Check if a socket is ready to read from
         *
         * @param socket  The socket to check
         * @return true  If the socket is ready
         * @return false  If the socket is not ready
         */
        [[nodiscard]] bool isReady(FileDescriptor socket);

    protected:
    private:
        std::vector<pollfd> _pollfds;
        struct timespec _timeout = {0, 0};
};
#endif

} // namespace Flakkari::Network

#endif /* !IOMULTIPLEXER_HPP_ */
