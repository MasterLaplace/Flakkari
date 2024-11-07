/**************************************************************************
 * Flakkari Library v0.3.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file ClientManager.hpp
 * @brief This file contains the ClientManager class. It is used to handle
 * the clients connected to the server and the clients disconnection timeout.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.3.0
 * @date 2023-12-24
 **************************************************************************/

#ifndef CLIENTMANAGER_HPP_
#define CLIENTMANAGER_HPP_

#include "Client.hpp"
#include "Network/Network.hpp"
#include "Network/Serializer.hpp"

#define SINGLETON_IMPLEMENTATION
#include <Singleton.hpp>
#include <unordered_map>
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
        explicit ClientManager(std::shared_ptr<Network::Socket> socket) : _socket(socket) {}

    /**
     * @brief Destroy the ClientManager object
     *
     */
    ~ClientManager() = default;

        /**
         * @brief Add a client to the client manager or update the last activity of the client
         *
         * @param client  The client's address
         */
        void addClient(std::shared_ptr<Network::Address> client, Network::Buffer &buffer);

        /**
         * @brief Remove a client from the client manager
         *
         * @param client  The client's address
         */
        void removeClient(std::shared_ptr<Network::Address> client);

        /**
         * @brief Ban a client from the server
         *
         * @param client  The client's address
         */
        void banClient(std::shared_ptr<Network::Address> client);

        [[nodiscard]] bool isBanned(std::shared_ptr<Network::Address> client);

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
        void sendPacketToClient(std::shared_ptr<Network::Address> client, const Network::Buffer &packet);

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
        void sendPacketToAllClientsExcept(std::shared_ptr<Network::Address> client, const Network::Buffer &packet);

        /**
         * @brief Receive a packet from a client
         *
         * @param client  The client's address
         * @param packet  The packet received
         */
        void receivePacketFromClient(std::shared_ptr<Network::Address> client, const Network::Buffer &packet);

        /**
         * @brief Get the Client object
         *
         * @param client  The client's address
         * @return std::shared_ptr<Client>  The client object
         */
        std::shared_ptr<Client> getClient(std::shared_ptr<Network::Address> client);

        /**
         * @brief Get the Client object
         *
         * @param id  The client's id
         * @return std::shared_ptr<Client>  The client object
         */
        std::shared_ptr<Client> getClient(std::string id);

        /**
         * @brief Get the Address object
         *
         * @param id The client's id
         * @return std::shared_ptr<Network::Address>  The client's address
         */
        std::shared_ptr<Network::Address> getAddress(std::string id);

    /**
     * @brief Get the client object from the client manager
     *
     * @param id  The client's id
     * @return std::shared_ptr<Client>  The client object
     */
    std::shared_ptr<Client> operator[](std::string id);
};

} /* namespace Flakkari */

#endif /* !CLIENTMANAGER_HPP_ */
