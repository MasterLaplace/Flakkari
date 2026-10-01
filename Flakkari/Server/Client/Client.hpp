/**************************************************************************
 * Flakkari Library v0.10.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Client.hpp
 * @brief This file contains the Client class. It is used to handle
 * the client's activity and disconnection timeout.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.10.0
 * @date 2023-12-24
 **************************************************************************/

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <chrono>
#include <mutex>

#include "API/IEntity.hpp"
#include "Game/GameManager.hpp"
#include "Network/PacketQueue.hpp"
#include "Network/Socket.hpp"
#include "Protocol/Packet.hpp"

namespace Flakkari {

/**
 * @brief Client class that handles the client's activity and disconnection timeout
 *
 * @details This class is used by the ClientManager to handle the client's activity
 * and disconnection timeout (5 seconds by default) and to check if the client is
 * still connected to the server or not (using the isConnected() method) and to
 * update the last activity of the client (using the keepAlive() method) when a
 * packet is received from the client or when a packet is sent to the client.
 *
 * @see ClientManager
 * @see Network::Address
 */
class Client {
public:
    /**
     * @brief Construct a new Client object
     *
     * @param address The client's address
     * @param name The Game's name
     * @param apiVersion The API version used by the client
     */
    Client(const std::shared_ptr<Network::Address> &address, const std::string &name, Protocol::ApiVersion apiVersion);
    ~Client();

    /**
     * @brief Check if the client is still connected to the server
     *
     * @param timeout  The timeout in seconds (default: 5)
     * @return true  If the client is still connected
     * @return false  If the client is not connected anymore
     */
    [[nodiscard]] bool isConnected(float timeout = 5);

    /**
     * @brief Update the last activity of the client
     *
     */
    void keepAlive();

    /**
     * @brief Add a packet to the client's packet history
     *
     * @param packet  The packet to add
     */
    void addPacketToHistory(const Network::Buffer &packet);

    /**
     * @brief Increment the warning count of the client
     *
     * @return true  If the client has been disconnected
     * @return false  If the client has not been disconnected
     */
    bool incrementWarningCount();

    /**
     * @brief Add a packet to the client's receive queue and set the api version
     * used by the client
     *
     * @param packet  The packet to add
     */
    void addPacketToReceiveQueue(const Protocol::Packet<Protocol::CommandId> &packet);

    /**
     * @brief Add a packet to the client's send queue and set the api version
     * used by the client
     *
     * @param packet  The packet to add
     */
    void addPacketToSendQueue(const Protocol::Packet<Protocol::CommandId> &packet);

    /**
     * @brief Get the client's address
     *
     * @return std::shared_ptr<Network::Address>  The client's address
     */
    [[nodiscard]] std::shared_ptr<Network::Address> getAddress() const { return _address; }

    /**
     * @brief Get the Entity object
     *
     * @return Entity  The entity of the client
     */
    [[nodiscard]] Engine::API::IEntity getEntity() const { return _entity; }
    void setEntity(Engine::API::IEntity entity) { _entity = entity; }

    [[nodiscard]] std::string getSceneName() const { return _sceneName; }
    void setSceneName(std::string sceneName) { _sceneName = sceneName; }

    [[nodiscard]] std::string getGameName() const { return _gameName; }
    void setGameName(std::string gameName) { _gameName = gameName; }

    [[nodiscard]] std::optional<std::string> getName() const { return _name; }
    void setName(std::string name) { _name = name; }

    [[nodiscard]] unsigned short getWarningCount() const { return _warningCount; }

    [[nodiscard]] unsigned short getMaxWarningCount() const { return _maxWarningCount; }

    [[nodiscard]] unsigned short getMaxPacketHistory() const { return _maxPacketHistory; }

    [[nodiscard]] Protocol::ApiVersion getApiVersion() const { return _apiVersion; }

    [[nodiscard]] Network::PacketQueue<Protocol::Packet<Protocol::CommandId>> &getReceiveQueue()
    {
        return _receiveQueue;
    }

    [[nodiscard]] Network::PacketQueue<Protocol::Packet<Protocol::CommandId>> &getSendQueue() { return _sendQueue; }

private:
    std::chrono::steady_clock::time_point _lastActivity;
    std::shared_ptr<Network::Address> _address;
    Engine::API::IEntity _entity;
    std::string _sceneName;
    std::string _gameName;
    bool _isConnected = true;
    std::string _name;
    Protocol::ApiVersion _apiVersion;
    unsigned short _warningCount = 0;
    unsigned short _maxWarningCount = 5;
    unsigned short _maxPacketHistory = 10;

    std::vector<Network::Buffer> _packetHistory;
    Network::PacketQueue<Protocol::Packet<Protocol::CommandId>> _sendQueue;
    Network::PacketQueue<Protocol::Packet<Protocol::CommandId>> _receiveQueue;
};

} /* namespace Flakkari */

#endif /* !CLIENT_HPP_ */
