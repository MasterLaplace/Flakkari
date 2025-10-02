/**************************************************************************
 * Flakkari Library v0.6.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Address.hpp
 * @brief Address header file.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.6.0
 * @date 2023-12-20
 **************************************************************************/

#ifndef ADDRESS_HPP_
#define ADDRESS_HPP_

#if !defined(_WIN32) && !defined(_WIN64)
#    include <arpa/inet.h>
#    include <netdb.h>
#else
#    define WIN32_LEAN_AND_MEAN
#    define _WINSOCK_DEPRECATED_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#    include <ws2tcpip.h>

#    pragma comment(lib, "Ws2_32.lib")
#endif

#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "../Logger/Logger.hpp"

namespace Flakkari::Network {

class Address {
public:
    enum class IpType {
        None,
        IPv4, // Internet Protocol version 4
        IPv6, // Internet Protocol version 6
    };

    enum class SocketType {
        None,
        TCP, // Transmission Control Protocol
        UDP, // User Datagram Protocol
    };

public:
    /**
     * @brief Convert IpType to string
     *
     * @param ip_type  IpType to convert
     * @return constexpr const char*  String representation of IpType
     */
    static constexpr const char *ipTypeToString(IpType ip_type);

    /**
     * @brief Convert SocketType to string
     *
     * @param socket_type  SocketType to convert
     * @return constexpr const char*  String representation of SocketType
     */
    static constexpr const char *socketTypeToString(SocketType socket_type);

public:
    using address_t = const std::string;
    using port_t = unsigned short;
    using id_t = short;

public:
    Address(address_t &address, port_t port, SocketType socket_type, IpType ip_type);
    Address(port_t port, SocketType socket_type, IpType ip_type);
    Address(const sockaddr_in &clientAddr, SocketType socket_type, IpType ip_type);
    Address(const sockaddr_storage &clientAddr, SocketType socket_type, IpType ip_type);
    Address(const Address &) = default;
    Address(Address &&) = default;
    Address() = default;
    ~Address() = default;

    /**
     * @brief Convert Address to string
     *
     * @return std::optional<std::string>  String representation of Address
     */
    [[nodiscard]] std::optional<std::string> toString() const;

    /**
     * @brief Get the Address Info object
     *
     * @return const std::shared_ptr<addrinfo>&  Address Info
     */
    [[nodiscard]] const std::shared_ptr<addrinfo> &getAddrInfo() const { return _addrInfo; }

    /**
     * @brief Get the Sock Addr object
     *
     * @return sockaddr*  Sock Addr
     */
    [[nodiscard]] sockaddr *getSockAddr() const { return _addrInfo->ai_addr; }

    /**
     * @brief Get the Sock Addr In object
     *
     * @return sockaddr_in*  Sock Addr In
     */
    [[nodiscard]] sockaddr_in *getSockAddrIn() const { return (sockaddr_in *) _addrInfo->ai_addr; }

    /**
     * @brief Get the Ip object (std::string)
     *
     * @return std::optional<std::string>  Ip
     * @example "Flakkari/Network/Address.cpp"
     * @code
     * #include "Address.hpp"
     *
     * Flakkari::Network::Address address("localhost", 8080, Flakkari::Network::Address::SocketType::TCP,
     * Flakkari::Network::Address::IpType::IPv4); std::cout << "Ip: " << address.getIp().value_or("Unknown") <<
     * std::endl;
     * @endcode
     * output:
     * @code
     * Ip: localhost
     */
    [[nodiscard]] std::optional<std::string> getIp() const;

    /**
     * @brief Get the Port object
     *
     * @return port_t  Port
     */
    [[nodiscard]] port_t getPort() const { return ntohs(getSockAddrIn()->sin_port); }

    /**
     * @brief Get the Socket Type object
     *
     * @return SocketType  Socket Type
     */
    [[nodiscard]] SocketType getSocketType() const { return _socket_type; }

    /**
     * @brief Get the Ip Type object
     *
     * @return IpType  Ip Type
     */
    [[nodiscard]] IpType getIpType() const { return _ip_type; }

    /**
     * @brief Convert Address to string (std::string)
     *
     * @return std::string  String representation of Address
     */
    operator std::string() const;

protected:
private:
    std::shared_ptr<addrinfo> _addrInfo = nullptr;
    SocketType _socket_type = SocketType::None;
    IpType _ip_type = IpType::None;
};

/**
 * @brief Overload of operator<< for Address
 *
 * @param os  Output stream
 * @param addr  Address to output
 * @return std::ostream&  Output stream
 */
std::ostream &operator<<(std::ostream &os, const Address &addr);

} // namespace Flakkari::Network

#endif /* !ADDRESS_HPP_ */
