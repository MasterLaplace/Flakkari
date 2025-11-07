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
#include <optional>
#include <thread>

namespace Flakkari {

#define INIT_LOOP loop:
#define GOTO_LOOP goto loop;

#ifndef STDIN_FILENO
#    define STDIN_FILENO _fileno(stdin)
#endif

/**
 * @brief UDP Client class that handles incoming and outgoing packets
 *
 * @details This class is the main class of the client, it handles incoming
 * packets from the server and outgoing packets to the server.
 *
 * @example "Flakkari/Client/UDPClient.cpp"
 * @code
 * #include "UDPClient.hpp"
 *
 * Flakkari::UDPClient client("localhost", 8081);
 * client.connectToServer();
 * auto packet = Protocol::Packet<Protocol::CommandId>();
 * client.sendPacket(packet.serialize());
 * packet = client.getNextPacket();
 * client.disconnectFromServer();
 * @endcode
 */
class UDPClient {
public:
    /**
     * @brief Construct a new UDPClient object
     *
     * @param game The name of the game
     * @param ip The ip to bind the server to (default: localhost)
     * @param port The port to bind the server to (default: 8081)
     * @param keepAliveInterval The interval to send keep alive packets (default: 3000 ms)
     */
    UDPClient(const std::string &game, const std::string &ip = "localhost", unsigned short port = 8081, long int keepAliveInterval = 3000);
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

    /**
     * @brief Send a serialized packet to the server
     *
     * @param serializedPacket The serialized packet to send
     */
    void sendPacket(const Flakkari::Network::Buffer &serializedPacket);

    /**
     * @brief Send a REQ_USER_UPDATES packet to the server
     *
     * @param events The list of events to send
     * @param axisEvents The dictionary of axis events to send
     */
    void reqUserUpdates(std::vector<Protocol::Event> events, std::unordered_map<Protocol::EventId, float> axisEvents);

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
     * @brief Run the client and wait for incoming packets
     *
     * @details This function is blocking, it will wait for incoming packets
     */
    void run();

private:
    std::shared_ptr<Network::Socket> _socket;
    std::unique_ptr<IO_SELECTED> _io;
    std::atomic<bool> _running{false};
    std::thread _thread;
    Network::PacketQueue<Protocol::Packet<Protocol::CommandId>> _packetQueue;
    const long int _KEEP_ALIVE_INTERVAL = 3000; // 3 seconds
    const std::string _GAME_NAME;
};

} /* namespace Flakkari */

#endif /* !UDPCLIENT_HPP_ */
