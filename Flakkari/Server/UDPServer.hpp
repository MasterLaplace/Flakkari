/**************************************************************************
 * Flakkari Library v0.2.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file UDPServer.hpp
 * @brief This file contains the UDPServer class. It is used to handle
 * incoming packets and clients. It is the main class of the server.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.2.0
 * @date 2023-12-24
 **************************************************************************/

#ifndef UDPSERVER_HPP_
#define UDPSERVER_HPP_

#include "Network/IOMultiplexer.hpp"
#include "Client/ClientManager.hpp"
#include "Protocol/Packet.hpp"
#include "Game/GameManager.hpp"
#include "Internals/CommandManager.hpp"

namespace Flakkari {

#define INIT_LOOP loop:
#define GOTO_LOOP goto loop;


/**
 * @brief UDP Server class that handles incoming packets and clients
 *
 * @details This class is the main class of the server, it handles incoming
 * packets and clients, it also handles the client's timeout and disconnection
 * @see ClientManager
 * @see Network::Socket
 * @see Network::PPOLL
 *
 * @example "Flakkari/Server/UDPServer.cpp"
 * @code
 * #include "UDPServer.hpp"
 *
 * Flakkari::UDPServer server("localhost", 8080);
 * return server.run();
 * @endcode
 */
class UDPServer {
    public:
        /**
         * @brief Construct a new UDPServer object
         *
         * @param ip The ip to bind the server to (default: localhost)
         * @param port The port to bind the server to (default: 8080)
         */
        UDPServer(std::string ip = "localhost", std::size_t port = 8080);
        ~UDPServer();

        /**
         * @brief Run the server and wait for incoming packets and clients
         *
         * @details This function is blocking, it will wait for incoming packets
         *
         */
        void run();

    private:
        /**
         * @brief Handle the timeout of the server (check for inactive clients)
         *
         * @param event  The event that triggered the timeout (0 if timeout)
         * @return true  If the timeout was handled
         * @return false  If the timeout was not handled
         */
        [[nodiscard]] bool handleTimeout(int event);

        /**
         * @brief Handle the input from the user (stdin)
         *
         * @param fd  The file descriptor to read from (stdin)
         * @return true  If the input was handled
         * @return false  If the input was not handled
         */
        [[nodiscard]] bool handleInput(int fd);

        /**
         * @brief Handle the incoming packets from the clients (UDP)
         *
         */
        void handlePacket();

    private:
        std::shared_ptr<Network::Socket> _socket;
        std::unique_ptr<Network::PSELECT> _io;
};

} /* namespace Flakkari */

#endif /* !UDPSERVER_HPP_ */
