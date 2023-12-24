/**************************************************************************
 * Flakkari Library v0.1.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Client.hpp
 * @brief This file contains the Client class. It is used to handle
 * the client's activity and disconnection timeout.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.1.0
 * @date 2023-12-24
 **************************************************************************/

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <chrono>

#include "Network/Address.hpp"

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
         */
        Client(std::shared_ptr<Network::Address> address);
        ~Client() = default;

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
         * @brief Get the client's address
         *
         * @return std::shared_ptr<Network::Address>  The client's address
         */
        [[nodiscard]] std::shared_ptr<Network::Address> getAddress() const { return _address; }

    protected:
    private:
        std::chrono::steady_clock::time_point _lastActivity;
        std::shared_ptr<Network::Address> _address;
};

} /* namespace Flakkari */

#endif /* !CLIENT_HPP_ */
