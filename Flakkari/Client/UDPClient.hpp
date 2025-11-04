/**************************************************************************
 * Flakkari Library v0.8.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file UDPClient.hpp
 * @brief This file contains the UDPClient class. It is used to handle
 * incoming packets and clients. It is the main class of the server.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2025 @MasterLaplace
 * @version 0.8.0
 * @date 2025-11-04
 **************************************************************************/

#ifndef UDPCLIENT_HPP_
#define UDPCLIENT_HPP_

#include "Network/IOMultiplexer.hpp"
#include "Network/Network.hpp"
#include "Network/PacketQueue.hpp"
#include "Network/Serializer.hpp"
#include "Protocol/Packet.hpp"
#include <atomic>
#include <mutex>
#include <optional>
#include <thread>

namespace Flakkari {

#define INIT_LOOP loop:
#define GOTO_LOOP goto loop;

#ifndef STDIN_FILENO
#    define STDIN_FILENO _fileno(stdin)
#endif

/**
 * @brief UDP Client class that handles incoming packets and clients
 *
 * @details This class is the main class of the server, it handles incoming
 * packets and clients, it also handles the client's timeout and disconnection
 *
 * @example "Flakkari/Client/UDPClient.cpp"
 * @code
 * #include "UDPClient.hpp"
 *
 * Flakkari::UDPClient server("Games", "localhost", 8081);
 * server.connectToServer();
 * auto packet = Protocol::Packet<Protocol::CommandId>();
 * server.sendPacket(packet);
 * packet = server.getNextPacket();
 * server.disconnectFromServer();
 * @endcode
 */
class UDPClient {
public:
    /**
     * @brief Construct a new UDPClient object
     *
     * @param gameDir The directory of the games folder
     * @param ip The ip to bind the server to (default: localhost)
     * @param port The port to bind the server to (default: 8081)
     */
    UDPClient(const std::string &gameDir, const std::string &ip = "localhost", unsigned short port = 8081);
    ~UDPClient();

    /**
     * @brief Connect to the game server
     *
     * @details This function will connect the client to the game server
     */
    void connectToServer();

    /**
     * @brief Disconnect from the game server
     *
     * @details This function will disconnect the client from the game server
     */
    void disconnectFromServer();

    void sendPacket(const Protocol::Packet<Protocol::CommandId> &packet);

    /**
     * @brief Get the next packet from the packet queue
     *
     * @return Protocol::Packet<Protocol::CommandId>  The next packet
     */
    std::optional<Protocol::Packet<Protocol::CommandId>> getNextPacket();

private:
    /**
     * @brief Add a packet to the packet queue
     *
     * @param packet  The packet to add
     */
    void addPacket(const Protocol::Packet<Protocol::CommandId> &packet);

    /**
     * @brief Handle the timeout of the server (check for inactive clients)
     *
     * @param event  The event that triggered the timeout (0 if timeout)
     * @return true  If the timeout was handled
     * @return false  If the timeout was not handled
     */
    [[nodiscard]] bool handleTimeout(int event);

    /**
     * @brief Handle the incoming packets from the clients (UDP)
     *
     */
    void handlePacket();

    /**
     * @brief Run the server and wait for incoming packets and clients
     *
     * @details This function is blocking, it will wait for incoming packets
     */
    void run();

private:
    std::shared_ptr<Network::Socket> _socket;
    std::unique_ptr<IO_SELECTED> _io;
    std::atomic<bool> _running{false};
    std::thread _thread;
    std::mutex _mutex;
    Network::PacketQueue<Protocol::Packet<Protocol::CommandId>> _packetQueue;
};

} /* namespace Flakkari */

#endif /* !UDPCLIENT_HPP_ */
