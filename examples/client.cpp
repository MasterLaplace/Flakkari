#include "Flakkari/Client/UDPClient.hpp"
#include <iostream>

int main()
{
    std::cout << "[CLIENT] Starting Flakkari example client..." << std::endl;

    Flakkari::UDPClient client("MyGame", "127.0.0.1", 12345);

    std::cout << "[CLIENT] Connecting to server at 127.0.0.1:12345" << std::endl;
    client.connectToServer();

    std::cout << "[CLIENT] Connected! Waiting for packets..." << std::endl;
    std::cout << "[CLIENT] Press Ctrl+C to stop" << std::endl;

    int packetCount = 0;
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto packet = client.getNextPacket();

        if (packet && packet->header._commandId == Flakkari::Protocol::CommandId::REP_DISCONNECT)
        {
            std::cout << "[CLIENT] Received disconnect command from server" << std::endl;
            client.disconnectFromServer();
            break;
        }
        else if (!packet)
            continue;

        packetCount++;
        std::cout << "[CLIENT] Received packet #" << packetCount
                  << " (CommandId: " << static_cast<int>(packet->header._commandId) << ")" << std::endl;

        client.sendPacket(packet->serialize());
    }

    std::cout << "[CLIENT] Disconnected. Total packets received: " << packetCount << std::endl;
    return 0;
}
