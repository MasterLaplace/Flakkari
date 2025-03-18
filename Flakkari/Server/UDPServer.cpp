/*
** EPITECH PROJECT, 2023
** Title: Flakkari
** Author: MasterLaplace
** Created: 2023-12-24
** File description:
** UDPServer
*/

#include "UDPServer.hpp"

using namespace Flakkari;

UDPServer::UDPServer(const std::string &gameDir, const std::string &ip, unsigned short port)
{
    Network::init();

    _socket = std::make_shared<Network::Socket>();
    _socket->create(ip, port, Network::Address::IpType::IPv4, Network::Address::SocketType::UDP);

    FLAKKARI_LOG_INFO(std::string(*_socket));
    _socket->setBlocking(false);
    _socket->bind();

    _io = std::make_unique<IO_SELECTED>(_socket->getSocket());
    _io->addSocket(STDIN_FILENO);

    ClientManager::CreateInstance(_socket);
    ResourceManager::CreateInstance();
    GameManager::CreateInstance(gameDir);
}

UDPServer::~UDPServer()
{
    ClientManager::DestroyInstance();
    ResourceManager::DestroyInstance();
    GameManager::DestroyInstance();
    Network::cleanup();
    FLAKKARI_LOG_INFO("UDPServer is now stopped");
}

bool UDPServer::handleTimeout(int event)
{
    if (event != 0)
        return false;
    FLAKKARI_LOG_DEBUG(LPL_TOSTRING(IO_SELECTED) " timed out");
    ClientManager::GetInstance().checkInactiveClients();
    ClientManager::UnlockInstance();
    return true;
}

bool UDPServer::handleInput(int fd)
{
    if (fd != STDIN_FILENO)
        return false;
    if (std::cin.eof())
        throw std::runtime_error("EOF on stdin");
    Internals::CommandManager::handleCommand();
    return true;
}

void UDPServer::handlePacket()
{
    auto packet = _socket->receiveFrom();
    if (!packet.has_value())
        return;
    auto resultAddClient = ClientManager::GetInstance().addClient(packet->first, packet->second);
    ClientManager::UnlockInstance();
    if (!resultAddClient.has_value())
        return;

    if (!resultAddClient->first.empty())
    {
        bool result = GameManager::GetInstance().addClientToGame(resultAddClient->first, resultAddClient->second);
        GameManager::UnlockInstance();
        if (!result)
        {
            ClientManager::GetInstance().removeClient(packet->first->toString().value_or(""));
            ClientManager::UnlockInstance();
        }
    }

    ClientManager::GetInstance().checkInactiveClients();
    ClientManager::UnlockInstance();
    if (resultAddClient->first.empty())
    {
        auto resultRecvClient = ClientManager::GetInstance().receivePacketFromClient(packet->first, packet->second);
        ClientManager::UnlockInstance();
        if (!resultRecvClient.has_value())
            return;
        GameManager::GetInstance().removeClientFromGame(resultRecvClient->first, resultRecvClient->second);
        GameManager::UnlockInstance();
    }
}

void UDPServer::run()
{
    INIT_LOOP;
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
        if (handleInput((int) fd))
            continue;
        handlePacket();
    }
    GOTO_LOOP;
}
