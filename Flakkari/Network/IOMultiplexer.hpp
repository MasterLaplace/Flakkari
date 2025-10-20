/**************************************************************************
 * Flakkari Library v0.8.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file IOMultiplexer.hpp
 * @brief This file contains the IOMultiplexer interface and the different
 *        IOMultiplexer implementations for different platforms.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.8.0
 * @date 2023-12-23
 **************************************************************************/

#ifndef IOMULTIPLEXER_HPP_
#define IOMULTIPLEXER_HPP_

// if im on windows, define all compatible IOMultiplexer
#ifdef _WIN32
#    define _EPOLL_
#    define _KQUEUE_
#    define _IO_URING_
#    define _WSA_
#else
#    define _PSELECT_
#    define _PPOLL_
#    define _EPOLL_
#endif

#include "Socket.hpp"
#include <unordered_map>
#include <vector>

namespace Flakkari::Network {

#if defined(_PSELECT_)
#    include <sys/select.h>
#    include <sys/time.h>
#    include <sys/types.h>

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
    PSELECT(FileDescriptor fileDescriptorn, long int seconds = 1, long int microseconds = 0);
    PSELECT(long int seconds = 1, long int microseconds = 0);
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

    /**
     * @brief Check if the error is skipable
     *
     * @return true  If the error is skipable
     * @return false  If the error is not skipable
     */
    [[nodiscard]] bool skipableError();

protected:
private:
    fd_set _fds;
    std::vector<FileDescriptor> _sockets;
    FileDescriptor _maxFd = 0;
    struct timespec _timeout = {0, 0};
};
#endif

#if defined(_PPOLL_)
#    if defined(_WIN32)
#        include <poll.h>
#    elif defined(__APPLE__)
#        include <sys/poll.h>
#    else
#        include <sys/poll.h>
#    endif

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
    PPOLL(FileDescriptor fileDescriptor, event_t events, long int seconds = 1, long int microseconds = 0);
    PPOLL(long int seconds = 1, long int microseconds = 0);
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

    std::vector<pollfd>::const_iterator begin() const { return _pollfds.begin(); }
    std::vector<pollfd>::const_iterator end() const { return _pollfds.end(); }

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

    /**
     * @brief Check if the error is skipable
     *
     * @return true  If the error is skipable
     * @return false  If the error is not skipable
     */
    [[nodiscard]] bool skipableError();

protected:
private:
    std::vector<pollfd> _pollfds;
    struct timespec _timeout = {0, 0};
};
#endif

#if defined(_WSA_)

#    define MAX_POLLFD 1024

/**
 * @brief WSA is a class that represents a WSA
 *
 * @class WSA
 * @implements IOMultiplexer
 * @see IOMultiplexer
 *
 * @example "WSA example":
 * @code
 * auto socket = std::make_shared<Socket>(12345, Address::IpType::IPv4, Address::SocketType::UDP);
 * socket->bind();
 *
 * auto io = std::make_unique<WSA>();
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
class WSA {
public:
    using FileDescriptor = SOCKET;

public:
    /**
     * @brief Construct a new WSA object with a timeout of 1 second by default
     *
     * @param socket  The server socket
     * @param seconds  The seconds to wait for an event
     * @param microseconds  The microseconds to wait for an event
     */
    WSA(FileDescriptor socket, int seconds = 1, int microseconds = 0);
    ~WSA() = default;

    /**
     * @brief Add a socket to the WSA list
     *
     * @param socket  The socket to add to the list
     */
    void addSocket(FileDescriptor socket);

    /**
     * @brief Remove a socket from the WSA list
     *
     * @param socket  The socket to remove from the list
     */
    void removeSocket(FileDescriptor socket);

    /**
     * @brief Wait for an event to happen on a socket or timeout
     *
     * @return int  The number of events that happened or -1 if an error occured or 0 if the timeout expired (EINTR)
     * @see WSAPoll
     */
    int wait();

    std::vector<FileDescriptor>::const_iterator begin() const { return _sockets.cbegin(); }
    std::vector<FileDescriptor>::const_iterator end() const { return _sockets.cend(); }

    /**
     * @brief Check if a socket is ready to read from
     *
     * @param socket  The socket to check
     * @return true  If the socket is ready
     * @return false  If the socket is not ready
     */
    [[nodiscard]] bool isReady(FileDescriptor socket);

    /**
     * @brief Check if the error is skipable
     *
     * @return true  If the error is skipable
     * @return false  If the error is not skipable
     */
    [[nodiscard]] bool skipableError();

protected:
private:
    std::vector<FileDescriptor> _sockets;
    std::vector<WSAPOLLFD> _fdArray;
    std::vector<size_t> _freeSpace;
    INT _timeoutInMs;
};
#endif

#ifdef _PSELECT_
#    define IO_SELECTED Network::PSELECT
#elif defined(_WSA_)
#    define IO_SELECTED Network::WSA
#endif

} // namespace Flakkari::Network

#endif /* !IOMULTIPLEXER_HPP_ */
