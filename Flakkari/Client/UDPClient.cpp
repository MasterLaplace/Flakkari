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

UDPClient::UDPClient(const std::string &game, const std::string &ip, unsigned short port, long int keepAliveInterval)
    : _KEEP_ALIVE_INTERVAL(keepAliveInterval), _GAME_NAME(game)
{
    Network::init();

    _socket = std::make_shared<Network::Socket>();
    _socket->create(ip, port, Network::Address::IpType::IPv4, Network::Address::SocketType::UDP);

    FLAKKARI_LOG_INFO(std::string(*_socket));
    _socket->setBlocking(false);

    _io = std::make_unique<IO_SELECTED>(_socket->getSocket(), _KEEP_ALIVE_INTERVAL);
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

    Protocol::Packet<Protocol::CommandId> packet;
    packet.header._apiVersion = packet.header._apiVersion;
    packet.header._commandId = Protocol::CommandId::REQ_CONNECT;
    packet.injectString(_GAME_NAME);
    sendPacket(packet.serialize());
}

void UDPClient::disconnectFromServer() { _running = false; }

void UDPClient::sendPacket(const Network::Buffer &serializedPacket)
{
    if (!_socket)
        return;
    _socket->sendTo(_socket->getAddress(), serializedPacket);
}

void UDPClient::reqUserUpdates(std::vector<Protocol::Event> events,
                               std::unordered_map<Protocol::EventId, float> axisEvents)
{
    if (events.empty() && axisEvents.empty())
        return;

    Protocol::Packet<Protocol::CommandId> packet;
    packet.header._priority = Protocol::Priority::HIGH;
    packet.header._commandId = Protocol::CommandId::REQ_USER_UPDATES;

    uint16_t eventCount = static_cast<uint16_t>(events.size());
    packet << eventCount;
    for (const auto &event : events)
        packet << event;

    uint16_t axisEventCount = static_cast<uint16_t>(axisEvents.size());
    packet << axisEventCount;
    for (const auto &[eventId, value] : axisEvents)
    {
        packet << eventId;
        packet << value;
    }

    sendPacket(packet.serialize());
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

    Protocol::Packet<Protocol::CommandId> packet;
    packet.header._apiVersion = packet.header._apiVersion;
    packet.header._commandId = Protocol::CommandId::REQ_HEARTBEAT;
    sendPacket(packet.serialize());
    return true;
}

void UDPClient::handlePacket()
{
    if (!_socket)
        return;

    auto responses = _socket->receiveBatch(32);
    if (responses.empty())
        return;

    for (auto &resp : responses)
    {
        Protocol::Packet<Protocol::CommandId> packet;
        if (!packet.deserialize(resp.second))
        {
            FLAKKARI_LOG_WARNING("Received an invalid packet");
            continue;
        }
        addPacket(packet);
    }
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
