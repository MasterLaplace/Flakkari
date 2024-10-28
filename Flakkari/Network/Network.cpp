/*
** EPITECH PROJECT, 2023
** Title: Flakkari
** Author: MasterLaplace
** Created: 2024-10-27
** File description:
** Network
*/

#include "Network.hpp"

namespace Flakkari::Network {

void initNetwork()
{
#ifdef _WIN32
    WSADATA WSAData;

    if (::WSAStartup(MAKEWORD(2, 2), &WSAData) != NO_ERROR)
        FLAKKARI_LOG_FATAL("WSAStartup failed");
#endif
}

void cleanupNetwork()
{
#ifdef _WIN32
    ::WSACleanup();
#endif
}

} // namespace Flakkari::Network
