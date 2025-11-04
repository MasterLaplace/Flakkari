/*
** EPITECH PROJECT, 2023
** Title: Flakkari
** Author: MasterLaplace
** Created: 2023-12-24
** File description:
** UDPClient
*/

#include "UDPClient.hpp"

namespace Flakkari {

UDPClient::UDPClient(const std::string &gameDir, const std::string &ip, unsigned short port)
{
    Network::init();

    _socket = std::make_shared<Network::Socket>();
    _socket->create(ip, port, Network::Address::IpType::IPv4, Network::Address::SocketType::UDP);

    FLAKKARI_LOG_INFO(std::string(*_socket));
    _socket->setBlocking(false);

    _io = std::make_unique<IO_SELECTED>(_socket->getSocket());
}

UDPClient::~UDPClient()
{
    _running = false;
    if (_thread.joinable())
        _thread.join();

    Network::cleanup();
    FLAKKARI_LOG_INFO("UDPClient is now stopped");
}

void UDPClient::connectToServer()
{
    _running = true;

    _thread = std::thread(&UDPClient::run, this);
}

void UDPClient::disconnectFromServer() { _running = false; }

void UDPClient::sendPacket(const Protocol::Packet<Protocol::CommandId> &packet)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto serializedPacket = packet.serialize();
    _socket->sendTo(_socket->getAddress(), serializedPacket);
}

void UDPClient::addPacket(const Protocol::Packet<Protocol::CommandId> &packet) { _packetQueue.push_back(packet); }

std::optional<Protocol::Packet<Protocol::CommandId>> UDPClient::getNextPacket()
{
    if (_packetQueue.empty())
        return std::nullopt;
    return _packetQueue.pop_front();
}

bool UDPClient::handleTimeout(int event)
{
    if (event != 0)
        return false;
    FLAKKARI_LOG_DEBUG(LPL_TOSTRING(IO_SELECTED) " timed out");
    return true;
}

void UDPClient::handlePacket()
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto response = _socket->receiveFrom();
    if (!response.has_value())
        return;

    Protocol::Packet<Protocol::CommandId> packet;
    if (!packet.deserialize(response->second))
    {
        FLAKKARI_LOG_WARNING("Received an invalid packet");
        return;
    }

    addPacket(packet);
}

void UDPClient::run()
{
    INIT_LOOP;
    if (!_running)
        return;

    int result = _io->wait();

    if (result == -1)
    {
        if (_io->skipableError())
            GOTO_LOOP;
        throw std::runtime_error("Failed to poll sockets, error: " + SPECIAL_ERROR);
    }

    if (handleTimeout(result))
        GOTO_LOOP;

    for (auto &fd : *_io)
    {
        if (!_io->isReady(fd))
            continue;
        handlePacket();
    }
    GOTO_LOOP;
}

} /* namespace Flakkari */
