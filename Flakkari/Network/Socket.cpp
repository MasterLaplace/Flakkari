/*
** EPITECH PROJECT, 2023
** Title: Flakkari
** Author: MasterLaplace
** Created: 2023-12-20
** File description:
** Socket
*/

#include "Socket.hpp"

namespace Flakkari::Network {

#if defined(__linux__)
#    include <cstring>
#    include <sys/socket.h>
#    include <sys/uio.h>
#endif

void Socket::create(const std::shared_ptr<Address> &address)
{
    _address = address;
    _socket = INVALID_SOCKET;

    const auto &addr = _address->getAddrInfo();

    if (addr == nullptr)
    {
        FLAKKARI_LOG_ERROR("Address is nullptr");
        return;
    }

    _socket = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (_socket == INVALID_SOCKET)
        throw std::runtime_error("Failed to create socket, error: " + STD_ERROR);

#if __APPLE__
    int optval = 1;
    ::setsockopt(_socket, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval));
#elif __linux__
    if (::setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, "\001", 4))
        throw std::runtime_error("Failed to set socket to reuse address and port, error: " + STD_ERROR);
#endif
}

void Socket::create(socket_t socket, const std::shared_ptr<Address> &address)
{
    _socket = socket;
    _address = address;

#if _WIN32
    u_long mode = 1;
    if (::ioctlsocket(_socket, FIONBIO, &mode) != NO_ERROR)
        throw std::runtime_error("Failed to set socket to non-blocking, error: " + STD_ERROR);
#elif __APPLE__
    int optval = 1;
    ::setsockopt(_socket, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval));
#elif __linux__
    if (::setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, "\001", 4))
        throw std::runtime_error("Failed to set socket to reuse address and port, error: " + STD_ERROR);
#endif
}

void Socket::create(const Address &address)
{
    _address = std::make_shared<Address>(address);
    _socket = INVALID_SOCKET;

    const auto &addr = _address->getAddrInfo();

    if (addr == nullptr)
    {
        FLAKKARI_LOG_ERROR("Address is nullptr");
        return;
    }

    _socket = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (_socket == INVALID_SOCKET)
        throw std::runtime_error("Failed to create socket, error: " + STD_ERROR);

#if __APPLE__
    int optval = 1;
    ::setsockopt(_socket, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval));
#elif __linux__
    if (::setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, "\001", 4))
        throw std::runtime_error("Failed to set socket to reuse address and port, error: " + STD_ERROR);
#endif
}

void Socket::create(ip_t address, port_t port, Address::IpType ip_type, Address::SocketType socket_type)
{
    _address = std::make_shared<Address>(address, port, socket_type, ip_type);
    _socket = INVALID_SOCKET;

    const auto &addr = _address->getAddrInfo();

    if (addr == nullptr)
    {
        FLAKKARI_LOG_ERROR("Address is nullptr");
        return;
    }

    _socket = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (_socket == INVALID_SOCKET)
        throw std::runtime_error("Failed to create socket, error: " + STD_ERROR);

#if __APPLE__
    int optval = 1;
    ::setsockopt(_socket, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval));
#elif __linux__
    if (::setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, "\001", 4))
        throw std::runtime_error("Failed to set socket to reuse address and port, error: " + STD_ERROR);
#endif
}

Socket::~Socket() { this->close(); }

void Socket::bind() const
{
    const auto &addr = _address->getAddrInfo();

    if (addr == nullptr)
        return FLAKKARI_LOG_ERROR("Address is nullptr"), void();

    if (::bind(_socket, addr->ai_addr, (int) addr->ai_addrlen) == SOCKET_ERROR)
        throw std::runtime_error("Failed to bind socket, error: " + STD_ERROR);
}

void Socket::listen(int backlog) const
{
    if (_address && _address->getSocketType() != Address::SocketType::TCP)
        return FLAKKARI_LOG_ERROR("Socket is not TCP"), void();

    if (::listen(_socket, backlog) == SOCKET_ERROR)
        throw std::runtime_error("Failed to listen on socket, error: " + STD_ERROR);
}

void Socket::connect() const
{
    const auto &addr = _address->getAddrInfo();

    if (addr == nullptr)
        return FLAKKARI_LOG_ERROR("Address is nullptr"), void();

    if (::connect(_socket, addr->ai_addr, (int) addr->ai_addrlen) == SOCKET_ERROR)
        throw std::runtime_error("Failed to connect socket, error: " + STD_ERROR);
}

void Socket::disconnect()
{
#if _WIN32
    if (::shutdown(_socket, SD_BOTH) == SOCKET_ERROR)
        throw std::runtime_error("Failed to disconnect socket, error: " + STD_ERROR);
#else
    if (::shutdown(_socket, SHUT_RDWR) == SOCKET_ERROR)
        throw std::runtime_error("Failed to disconnect socket, error: " + STD_ERROR);
#endif

#ifdef _WIN32
    if (::closesocket(_socket) == SOCKET_ERROR)
        throw std::runtime_error("Failed to close socket, error: " + STD_ERROR);
#else
    if (::close(_socket) == SOCKET_ERROR)
        throw std::runtime_error("Failed to close socket, error: " + STD_ERROR);
#endif

#ifdef _WIN32
    WSACleanup();
#endif

    _socket = INVALID_SOCKET;
}

std::shared_ptr<Socket> Socket::accept() const
{
    sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    SOCKET clientSocket = ::accept(_socket, (struct sockaddr *) &clientAddr, &clientAddrLen);

    if (clientSocket == INVALID_SOCKET)
        throw std::runtime_error("Failed to accept socket, error: " + STD_ERROR);

    Address::IpType ip_type = getIpTypeFromFamily(clientAddr);

    auto clientAddress = std::make_shared<Address>(clientAddr, _address->getSocketType(), ip_type);
    auto client = std::make_shared<Socket>();
    client->create(clientSocket, clientAddress);
    return client;
}

void Socket::send(const Buffer &data, int flags) const
{
#ifdef _WIN32
    if (::send(_socket, (const char *) data.getData(), (int) data.getSize(), flags) == SOCKET_ERROR)
    {
        FLAKKARI_LOG_ERROR("Failed to send \"" + std::string(data.begin(), data.end()) + "\" to socket(" +
                           std::to_string(_socket) + "), error: " + STD_ERROR);
        return;
    }
#else
    if (::send(_socket, data.getData(), data.getSize(), flags) == SOCKET_ERROR)
    {
        FLAKKARI_LOG_ERROR("Failed to send \"" + std::string(data.begin(), data.end()) + "\" to socket(" +
                           std::to_string(_socket) + "), error: " + STD_ERROR);
        return;
    }
#endif
}

void Socket::send(const Buffer &data, size_t size, int flags) const
{
#ifdef _WIN32
    if (::send(_socket, (const char *) data.getData(), (int) size, flags) == SOCKET_ERROR)
    {
        FLAKKARI_LOG_ERROR("Failed to send \"" + std::string(data.begin(), data.end()) + "\" to socket(" +
                           std::to_string(_socket) + "), error: " + STD_ERROR);
        return;
    }
#else
    if (::send(_socket, data.getData(), size, flags) == SOCKET_ERROR)
    {
        FLAKKARI_LOG_ERROR("Failed to send \"" + std::string(data.begin(), data.end()) + "\" to socket(" +
                           std::to_string(_socket) + "), error: " + STD_ERROR);
        return;
    }
#endif
}

void Socket::sendTo(const std::shared_ptr<Address> &address, const Buffer &data, int flags) const
{
    const auto &addr = address->getAddrInfo();

    if (addr == nullptr)
        return FLAKKARI_LOG_ERROR("Address is nullptr"), void();

#ifdef _WIN32
    if (::sendto(_socket, (const char *) data.getData(), (int) data.getSize(), flags, addr->ai_addr,
                 (int) addr->ai_addrlen) == SOCKET_ERROR)
    {
        FLAKKARI_LOG_ERROR("Failed to send \"" + std::string(data.begin(), data.end()) + "\" to \"" +
                           address->toString().value_or("No address") + "\", error: " + STD_ERROR);
        return;
    }
#else
    if (::sendto(_socket, data.getData(), data.getSize(), flags, addr->ai_addr, addr->ai_addrlen) == SOCKET_ERROR)
    {
        FLAKKARI_LOG_ERROR("Failed to send \"" + std::string(data) + "\" to \"" +
                           address->toString().value_or("No address") + "\", error: " + STD_ERROR);
        return;
    }
#endif
}

void Socket::sendTo(const std::shared_ptr<Address> &address, const byte *data, const size_t &size, int flags) const
{
    const auto &addr = address->getAddrInfo();

    if (addr == nullptr)
        return FLAKKARI_LOG_ERROR("Address is nullptr"), void();

#if _WIN32
    if (::sendto(_socket, (const char *) data, (int) size, flags, addr->ai_addr, (int) addr->ai_addrlen) ==
        SOCKET_ERROR)
    {
        FLAKKARI_LOG_ERROR("Failed to send \"" + std::string(data, data + size) + "\" to \"" +
                           address->toString().value_or("No address") + "\", error: " + STD_ERROR);
        return;
    }
#else
    if (::sendto(_socket, data, size, flags, addr->ai_addr, addr->ai_addrlen) == SOCKET_ERROR)
    {
        FLAKKARI_LOG_ERROR("Failed to send \"" + std::string(data, data + size) + "\" to \"" +
                           address->toString().value_or("No address") + "\", error: " + STD_ERROR);
        return;
    }
#endif
}

std::optional<Buffer> Socket::receive(size_t size, int flags) const
{
    Buffer data(size, 0);

    if (::recv(_socket, (char *) &data[0], (int) size, flags) == SOCKET_ERROR)
    {
        FLAKKARI_LOG_ERROR("Failed to receive data from socket(" + std::to_string(_socket) + "), error: " + STD_ERROR);
        return {};
    }
    return data;
}

std::optional<Buffer> Socket::receive(int flags) const
{
    Buffer data(4096, 0);

    if (::recv(_socket, (char *) &data[0], 4096, flags) == SOCKET_ERROR)
    {
        FLAKKARI_LOG_ERROR("Failed to receive data from socket(" + std::to_string(_socket) + "), error: " + STD_ERROR);
        return {};
    }
    return data;
}

std::optional<std::pair<std::shared_ptr<Address>, Buffer>> Socket::receiveFrom(size_t size, int flags) const
{
    Buffer data(size, 0);
    sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);

    if (::recvfrom(_socket, (char *) data.data(), (int) size, flags, (sockaddr *) &addr, &addrlen) == SOCKET_ERROR)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return {};
        FLAKKARI_LOG_ERROR("Failed to receive data from \"" + _address->toString().value_or("No address") +
                           "\", error: " + STD_ERROR);
        return {};
    }

    Address::IpType ip_type = getIpTypeFromFamily(addr);

    return std::make_pair(std::make_shared<Address>(addr, _address->getSocketType(), ip_type),
                          Buffer(data.begin(), data.end()));
}

std::optional<std::pair<std::shared_ptr<Address>, Buffer>> Socket::receiveFrom(int flags) const
{
    Buffer data(4096, 0);
    sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);

    if (::recvfrom(_socket, (char *) &data[0], 4096, flags, (sockaddr *) &addr, &addrlen) == SOCKET_ERROR)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return {};
        FLAKKARI_LOG_ERROR("Failed to receive data from \"" + _address->toString().value_or("No address") +
                           "\", error: " + STD_ERROR);
        return {};
    }

    Address::IpType ip_type = getIpTypeFromFamily(addr);

    return std::make_pair(std::make_shared<Address>(addr, _address->getSocketType(), ip_type), data);
}

std::vector<std::pair<std::shared_ptr<Address>, Buffer>> Socket::receiveBatch(uint32_t maxMessages, int flags) const
{
    std::vector<std::pair<std::shared_ptr<Address>, Buffer>> results;
    if (maxMessages == 0u)
        return results;

#if defined(__linux__)
    std::vector<Buffer> buffers(maxMessages, Buffer(4096, 0));
    std::vector<mmsghdr> msgs(maxMessages);
    std::vector<iovec> iov(maxMessages);
    std::vector<sockaddr_storage> addrs(maxMessages);

    std::memset(msgs.data(), 0u, sizeof(mmsghdr) * msgs.size());

    for (uint32_t i = 0u; i < maxMessages; ++i)
    {
        iov[i].iov_base = buffers[i].data();
        iov[i].iov_len = buffers[i].size();

        msgs[i].msg_hdr.msg_iov = &iov[i];
        msgs[i].msg_hdr.msg_iovlen = 1u;
        msgs[i].msg_hdr.msg_name = &addrs[i];
        msgs[i].msg_hdr.msg_namelen = sizeof(sockaddr_storage);
    }

    int received = ::recvmmsg(_socket, msgs.data(), (uint32_t) msgs.size(), flags, nullptr);
    if (received == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return results;
        FLAKKARI_LOG_ERROR("recvmmsg failed, error: " + STD_ERROR);
        return results;
    }

    Address::SocketType socket_type = _address ? _address->getSocketType() : Address::SocketType::UDP;

    for (int i = 0; i < received; ++i)
    {
        auto &m = msgs[i];
        ssize_t len = m.msg_len;
        sockaddr_storage &addr = addrs[i];
        Address::IpType ip_type = getIpTypeFromFamily(addr);

        results.emplace_back(
            std::make_shared<Address>(addr, socket_type, ip_type),
            Buffer(buffers[i].begin(), buffers[i].begin() + (size_t) len));
    }

    return results;
#else
    for (uint32_t i = 0u; i < maxMessages; ++i)
    {
        auto pkt = receiveFrom(flags);
        if (!pkt.has_value())
            break;
        results.emplace_back(std::move(*pkt));
    }
    return results;
#endif
}

void Socket::close() const
{
#ifdef _WIN32
    ::closesocket(_socket);
#else
    ::close(_socket);
#endif
}

void Socket::setBlocking(bool blocking) const
{
#ifdef _WIN32
    u_long mode = blocking ? 0 : 1;
    int result = ::ioctlsocket(_socket, FIONBIO, &mode);
#else
    int flags = ::fcntl(_socket, F_GETFL, 0);

    if (flags == -1)
    {
        FLAKKARI_LOG_ERROR("Failed to get socket flags, error: " + STD_ERROR);
        return;
    }

    int result = ::fcntl(_socket, F_SETFL, flags | (blocking ? 0 : O_NONBLOCK));
#endif

    if (result == SOCKET_ERROR)
        FLAKKARI_LOG_ERROR("Failed to set socket to " + std::string(blocking ? "" : "non") +
                           "blocking, error: " + STD_ERROR);
}

Socket::operator std::string() const
{
    return std::string(std::string(*(this->getAddress())) + " (" + std::to_string(this->getSocket()) + ")");
}

inline Address::IpType Socket::getIpTypeFromFamily(const sockaddr_storage &addrStorage) const
{
    switch (addrStorage.ss_family)
    {
    case AF_INET:
        return Address::IpType::IPv4;
    case AF_INET6:
        return Address::IpType::IPv6;
    default:
        return Address::IpType::None;
    }
}

std::ostream &operator<<(std::ostream &os, const Socket &socket)
{
    os << *socket.getAddress();
    os << " (";
    os << socket.getSocket();
    os << ")";
    return os;
}

} // namespace Flakkari::Network
