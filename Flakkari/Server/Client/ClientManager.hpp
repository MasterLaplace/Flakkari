/**************************************************************************
 * Flakkari Library v0.7.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file ClientManager.hpp
 * @brief This file contains the ClientManager class. It is used to handle
 * the clients connected to the server and the clients disconnection timeout.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.7.0
 * @date 2023-12-24
 **************************************************************************/

#ifndef CLIENTMANAGER_HPP_
#define CLIENTMANAGER_HPP_

#include "Client.hpp"
#include "Network/Network.hpp"
#include "Network/Serializer.hpp"

#define SINGLETON_IMPLEMENTATION
#include <Singleton.hpp>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace Flakkari {

/**
 * @brief Client manager class that handles the clients connected to the server
 * and the clients disconnection timeout.
 *
 * @details This class is used by the UDPServer to handle the clients connected
 * to the server and the clients disconnection timeout (5 seconds by default).
 * It is also used to check if the clients are still connected to the server
 * (using the checkInactiveClients() method) and to add a client to the client
 *
 * @see UDPServer
 * @see Client
 *
 * @example "Flakkari/Server/Client/ClientManager.cpp"
 * @code
 * #include "ClientManager.hpp"
 *
 * Flakkari::ClientManager::addClient(std::make_shared<Flakkari::Network::Address>("...")>);
 * Flakkari::ClientManager::checkInactiveClients();
 * @endcode
 */
class ClientManager : public Singleton<ClientManager> {
private:
    std::unordered_map<std::string /*ip*/, std::shared_ptr<Client>> _clients;
    std::vector<std::string /*ip*/> _bannedClients;
    std::shared_ptr<Network::Socket> _socket;

    using id_t = short;

public:
    /**
     * @brief Construct a new ClientManager object
     *
     * @param socket  The server's socket
     */
    explicit ClientManager(const std::shared_ptr<Network::Socket> &socket) : _socket(socket) {}

    /**
     * @brief Destroy the ClientManager object
     *
     */
    ~ClientManager() = default;

    /**
     * @brief Add a client to the client manager or update the last activity of the client
     *
     * @param client  The client's address
     * @param buffer  The packet received from the client
     * @return std::optional<std::pair<std::string, std::shared_ptr<Client>>
     *         The client's name and the client object
     */
    std::optional<std::pair<std::string, std::shared_ptr<Client>>>
    addClient(const std::shared_ptr<Network::Address> &client, Network::Buffer &buffer);

    /**
     * @brief Remove a client from the client manager
     *
     * @param client  The client's name
     */
    void removeClient(const std::string &client);

    /**
     * @brief Check if a client is banned
     *
     * @param client  The client's address
     * @return true  If the client is banned
     * @return false  If the client is not banned
     */
    [[nodiscard]] bool isBanned(const std::shared_ptr<Network::Address> &client);

    /**
     * @brief Check if the clients are still connected to the server
     * and remove the inactive clients from the client manager
     * (inactive clients are clients that didn't send any packet to the server
     * for more than 5 seconds)
     *
     * @see Client::isConnected()
     * @see Client::keepAlive()
     */
    void checkInactiveClients();

    /**
     * @brief Send a packet to a client
     *
     * @param client  The client's address
     * @param packet  The packet to send
     */
    void sendPacketToClient(const std::shared_ptr<Network::Address> &client, const Network::Buffer &packet);

    /**
     * @brief Send a packet to all clients
     *
     * @param packet  The packet to send
     */
    void sendPacketToAllClients(const Network::Buffer &packet);

    /**
     * @brief Send a packet to all clients except one
     *
     * @param client  The client's address
     * @param packet  The packet to send
     */
    void sendPacketToAllClientsExcept(const std::shared_ptr<Network::Address> &client, const Network::Buffer &packet);

    /**
     * @brief Receive a packet from a client
     *
     * @param client  The client's address
     * @param packet  The packet received
     * @return std::optional<std::pair<const std::string &, std::shared_ptr<Client>>
     *         A pair of the client's game name and the client object
     */
    std::optional<std::pair<std::string, std::shared_ptr<Client>>>
    receivePacketFromClient(const std::shared_ptr<Network::Address> &client, const Network::Buffer &packet);

    /**
     * @brief Get the Client object
     *
     * @param client  The client's address
     * @return std::shared_ptr<Client>  The client object
     */
    std::shared_ptr<Client> getClient(const std::shared_ptr<Network::Address> &client);

    /**
     * @brief Get the Client object
     *
     * @param id  The client's id
     * @return std::shared_ptr<Client>  The client object
     */
    std::shared_ptr<Client> getClient(const std::string &id);

    /**
     * @brief Get the Address object
     *
     * @param id The client's id
     * @return std::shared_ptr<Network::Address>  The client's address
     */
    std::shared_ptr<Network::Address> getAddress(const std::string &id);

    /**
     * @brief Get the client object from the client manager
     *
     * @param id  The client's id
     * @return std::shared_ptr<Client>  The client object
     */
    std::shared_ptr<Client> operator[](const std::string &id);
};

} /* namespace Flakkari */

#endif /* !CLIENTMANAGER_HPP_ */
