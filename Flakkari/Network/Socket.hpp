/**************************************************************************
 * Flakkari Library v0.2.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Socket.hpp
 * @brief This file contains the Socket class. It is used to create a socket
 *        and send/receive data from it.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.2.0
 * @date 2023-12-20
 **************************************************************************/

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include "Address.hpp"
#include "Buffer.hpp"

#include <iostream>
#include <memory>
#include <optional>
#include <utility>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>

    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "Mswsock.lib")

    typedef SOCKET SOCKET;
    #define INVALID_SOCKET (SOCKET)(~0)
    #define SOCKET_ERROR (-1)
#elif __linux__
    #include <cstring>
    #include <arpa/inet.h>

    #ifdef _POSIX_C_SOURCE
    #include <fcntl.h>
    #include <unistd.h>
    #else
    #error "POSIX not defined"
    #endif

    typedef int SOCKET;
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
#elif __APPLE__
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>

    typedef int SOCKET;
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
#endif

namespace Flakkari::Network {

/**
 * @brief This class is used to create a socket and send/receive data from it.
 * It is used by the Server and Client classes.
 *
 * @see Server
 * @see Client
 * @see Address
 *
 * @example "Socket class for a UDP server":
 * @code
 * // Create a socket
 * auto socket = std::make_shared<Socket>(12345, Address::IpType::IPv4, Address::SocketType::UDP);
 * // Bind the socket
 * socket->bind();
 * // Send data to a client
 * socket->sendTo("Hello World!", socket->getAddress());
 * // Receive data from a client
 * auto data = socket->receiveFrom();
 * std::cout << data->second << std::endl;
 * @endcode
 *
 * @example "Socket class for a TCP server":
 * @code
 * // Create a socket
 * auto socket = std::make_shared<Socket>(12345, Address::IpType::IPv4, Address::SocketType::TCP);
 * // Bind the socket
 * socket->bind();
 * // Listen for incoming connections
 * socket->listen();
 * // Accept a client
 * auto clientSocket = socket->accept();
 * // Send data to a client
 * clientSocket->send("Hello World!");
 * // Receive data from a client
 * auto data = clientSocket->receive();
 * @endcode
 *
 * @example "Socket class for a UDP client":
 * @code
 * // Create a socket
 * auto socket = std::make_shared<Socket>(12345, Address::IpType::IPv4, Address::SocketType::UDP);
 * // Send data to the server
 * socket->sendTo("Hello World!", socket->getAddress());
 * // Receive data from the server
 * auto data = socket->receiveFrom();
 * std::cout << data->second << std::endl;
 * @endcode
 *
 * @example "Socket class for a TCP client":
 * @code
 * // Create a socket
 * auto socket = std::make_shared<Socket>(12345, Address::IpType::IPv4, Address::SocketType::TCP);
 * // Connect to a server
 * socket->connect();
 * // Send data to the server
 * socket->send("Hello World!");
 * // Receive data from the server
 * auto data = socket->receive();
 * @endcode
 */
class Socket {
    public:
        using port_t = Address::port_t;
        using ip_t = const std::string &;
        using socket_t = SOCKET;

    public:
        Socket(std::shared_ptr<Address> address);
        Socket(socket_t socket, std::shared_ptr<Address> address);
        Socket(Address address);
        Socket(ip_t address, port_t port, Address::IpType ip_type, Address::SocketType socket_type);
        Socket(const Socket &) = default;
        Socket(Socket &&) = default;
        ~Socket();

        bool operator==(const Socket &other) const { return _socket == other._socket; }
        bool operator==(const int &socket) const { return _socket == socket; }

        /**
         * @brief Get the Socket object (SOCKET).
         *
         * @return socket_t  The socket.
         */
        socket_t getSocket() const { return _socket; }

        /**
         * @brief Get the Address object (std::shared_ptr<Address>).
         *
         * @return std::shared_ptr<Address>  The address.
         * @see Address
         */
        std::shared_ptr<Address> getAddress() const { return _address; }

        /**
         * @brief Bind the socket to the address and port specified in the constructor.
         *
         */
        void bind();

        /**
         * @brief Start listening for incoming connections.
         * This function is only used by TCP sockets.
         *
         * @param backlog  Maximum number of pending connections. Default is SOMAXCONN.
         */
        void listen(int backlog = SOMAXCONN);

        /**
         * @brief Accept an incoming connection.
         * This function is only used by TCP sockets.
         *
         * @return std::shared_ptr<Socket>  The socket of the client.
         */
        void connect();

        /**
         * @brief Accept an incoming connection.
         * This function is only used by TCP sockets.
         *
         * @return std::shared_ptr<Socket>  The socket of the client.
         */
        std::shared_ptr<Socket> accept();

        /**
         * @brief Set the socket to blocking or non-blocking.
         *
         * @param blocking  True to set the socket to blocking, false to set it to non-blocking.
         */
        void setBlocking(bool blocking = true);

        /**
         * @brief Send data to the socket.
         * This function is only used by TCP sockets.
         *
         * @param data  Data to send.
         * @param flags  Flags to pass to the send function.
         */
        void send(const Buffer &data, int flags = 0);

        /**
         * @brief Send data to the socket.
         * This function is only used by TCP sockets.
         *
         * @param data  Data to send.
         * @param size  Size of the data to send.
         * @param flags  Flags to pass to the send function.
         */
        void send(const Buffer &data, size_t size, int flags = 0);

        /**
         * @brief Send data to the socket.
         * This function is only used by UDP sockets.
         *
         * @param address  Address to send the data to.
         * @param data  Data to send.
         * @param flags  Flags to pass to the send function.
         */
        void sendTo(const std::shared_ptr<Address> &address, const Buffer &data, int flags = 0);

        /**
         * @brief Send data to the socket.
         * This function is only used by UDP sockets.
         *
         * @param address  Address to send the data to.
         * @param data  Data to send.
         * @param size  Size of the data to send.
         * @param flags  Flags to pass to the send function.
         */
        void sendTo(const std::shared_ptr<Address> &address, const byte *data, const size_t &size, int flags = 0);

        /**
         * @brief Receive data from the socket.
         * This function is only used by TCP sockets.
         *
         * @param size  Size of the data to receive.
         * @param flags  Flags to pass to the recv function.
         * @return std::optional<Buffer>  The data received.
         */
        std::optional<Buffer> receive(size_t size, int flags = 0);

        /**
         * @brief Receive data from the socket. The size of the data is determined by the size of the buffer.
         * This function is only used by TCP sockets.
         *
         * @param flags  Flags to pass to the recv function.
         * @return std::optional<Buffer>  The data received.
         */
        std::optional<Buffer> receive(int flags = 0);

        /**
         * @brief Receive data from the socket.
         * This function is only used by UDP sockets.
         *
         * @param size  Size of the data to receive.
         * @param flags  Flags to pass to the recvfrom function.
         * @return std::optional<std::pair<std::shared_ptr<Address>, Buffer>>  The data received and the address of the sender.
         */
        std::optional<std::pair<std::shared_ptr<Address>, Buffer>> receiveFrom(size_t size, int flags = 0);

        /**
         * @brief Receive data from the socket. The size of the data is determined by the size of the buffer.
         * This function is only used by UDP sockets.
         *
         * @param flags  Flags to pass to the recvfrom function.
         * @return std::optional<std::pair<std::shared_ptr<Address>, Buffer>>  The data received and the address of the sender.
         */
        std::optional<std::pair<std::shared_ptr<Address>, Buffer>> receiveFrom(int flags = 0);

        /**
         * @brief Close the socket.
         *
         */
        void close();

        /**
         * @brief Convert Socket to string (ip:port).
         *
         * @return std::string  The string representation of the socket.
         */
        operator std::string() const;

    protected:
    private:
        socket_t _socket;
        std::shared_ptr<Address> _address = nullptr;
};

/**
 * @brief Convert Socket to string
 *
 * @param os  Output stream
 * @param addr  Socket to convert
 * @return std::ostream&  Output stream
 */
std::ostream &operator<<(std::ostream &os, const Socket &socket);

} // namespace Flakkari::Network

#endif /* !SOCKET_HPP_ */
