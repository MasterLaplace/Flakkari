/*
** EPITECH PROJECT, 2023
** Title: Flakkari
** Author: MasterLaplace
** Created: 2023-12-24
** File description:
** ClientManager
*/

#include "ClientManager.hpp"
#include "Game/GameManager.hpp"

#include <future>

namespace Flakkari {

std::optional<std::pair<std::string, std::shared_ptr<Client>>>
ClientManager::addClient(const std::shared_ptr<Network::Address> &client, Network::Buffer &buffer)
{
    if (this->isBanned(client))
    {
        FLAKKARI_LOG_LOG("Client " + client->toString().value_or("Unknown") + " tried to connect but is banned");
        return std::nullopt;
    }

    auto clientString = client->toString().value_or("");

    if (_clients.contains(clientString))
    {
        _clients[clientString]->keepAlive();
        return std::make_pair("", nullptr);
    }

    Protocol::Packet<Protocol::CommandId> packet;
    if (!packet.deserialize(buffer))
    {
        FLAKKARI_LOG_WARNING("Client " + client->toString().value_or("Unknown") + " sent an invalid packet");
        _bannedClients.push_back(client->getIp().value());
        return std::nullopt;
    }

    if (packet.header._commandId != Protocol::CommandId::REQ_CONNECT)
    {
        FLAKKARI_LOG_WARNING("Client " + client->toString().value_or("Unknown") + " sent an invalid packet");
        _bannedClients.push_back(client->getIp().value());
        return std::nullopt;
    }

    std::string gameName = packet.extractString();
    auto apiVersion = packet.header._apiVersion;
    _clients[clientString] = std::make_shared<Client>(client, gameName, apiVersion);

    return std::make_pair(gameName, _clients[clientString]);
}

void ClientManager::removeClient(const std::string &clientName)
{
    if (_clients.find(clientName) == _clients.end())
        return;

    _clients.erase(clientName);
}

bool ClientManager::isBanned(const std::shared_ptr<Network::Address> &client)
{
    return std::find(_bannedClients.begin(), _bannedClients.end(), client->getIp().value_or("")) !=
           _bannedClients.end();
}

void ClientManager::checkInactiveClients()
{
    for (auto it = _clients.begin(); it != _clients.end();)
    {
        if (!it->second->isConnected())
        {
            FLAKKARI_LOG_LOG("Client " + it->first + " disconnected");
            it = _clients.erase(it);
            continue;
        }

        ++it;
    }
}

void ClientManager::sendPacketToClient(const std::shared_ptr<Network::Address> &client, const Network::Buffer &packet)
{
    _socket->sendTo(client, packet);
}

void ClientManager::sendPacketToAllClients(const Network::Buffer &packet)
{
    for (auto &tmp_client : _clients)
    {
        if (tmp_client.second->isConnected())
            _socket->sendTo(tmp_client.second->getAddress(), packet);
    }
}

void ClientManager::sendPacketToAllClientsExcept(const std::shared_ptr<Network::Address> &client,
                                                 const Network::Buffer &packet)
{
    auto clientKey = client->toString().value_or("");

    for (auto &tmp_client : _clients)
    {
        auto tmp_clientKey = tmp_client.second->getName().value_or("");

        if (tmp_client.second->isConnected() && tmp_clientKey != clientKey)
            _socket->sendTo(tmp_client.second->getAddress(), packet);
    }
}

std::optional<std::pair<std::string, std::shared_ptr<Client>>>
ClientManager::receivePacketFromClient(const std::shared_ptr<Network::Address> &client, const Network::Buffer &buffer)
{
    auto clientName = client->toString().value_or("");
    auto ip = client->getIp().value_or("");

    if (std::find(_bannedClients.begin(), _bannedClients.end(), ip) != _bannedClients.end())
    {
        FLAKKARI_LOG_LOG("Client " + clientName + " tried to connect but is banned");
        return std::nullopt;
    }

    if (!_clients.contains(clientName))
        return std::nullopt;
    auto &tmp_client = _clients[clientName];

    Protocol::Packet<Protocol::CommandId> packet;
    if (packet.deserialize(buffer))
    {
        FLAKKARI_LOG_LOG("Client " + clientName + " sent a valid packet: " + packet.to_string());

        if (packet.header._commandId == Protocol::CommandId::REQ_DISCONNECT)
        {
            FLAKKARI_LOG_LOG("Client " + clientName + " disconnected");
            return std::make_pair(tmp_client->getGameName(), tmp_client);
        }

        tmp_client->addPacketToReceiveQueue(packet);
        return std::nullopt;
    }

    FLAKKARI_LOG_WARNING("Client " + clientName + " sent an invalid packet");

    if (!tmp_client->incrementWarningCount())
        return std::nullopt;

    FLAKKARI_LOG_LOG("Client " + clientName + " has been banned");

    _bannedClients.push_back(ip);
    FLAKKARI_LOG_LOG("Client " + clientName + " banned");
    return std::make_pair(tmp_client->getGameName(), tmp_client);
}

std::shared_ptr<Client> ClientManager::getClient(const std::shared_ptr<Network::Address> &client)
{
    return _clients[client->toString().value_or("")];
}

std::shared_ptr<Client> ClientManager::getClient(const std::string &id) { return _clients[id]; }

std::shared_ptr<Network::Address> ClientManager::getAddress(const std::string &id)
{
    return _clients[id]->getAddress();
}

std::shared_ptr<Client> ClientManager::operator[](const std::string &id) { return _clients[id]; }

} /* namespace Flakkari */
