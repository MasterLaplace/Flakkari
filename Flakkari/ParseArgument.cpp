/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2024-12-07
** File description:
** ParseArgument
*/

#include "ParseArgument.hpp"

namespace Flakkari {

ParseArgument::ParseArgument(int ac, const char *av[])
{
    for (uint_fast8_t i = 0; i < ac; ++i)
    {
        if (std::string(av[i]) == "-d" || std::string(av[i]) == "--default")
        {
            _gameDir = "Games";
            _ip = "localhost";
            _port = 8081;
            return;
        }
        else if (std::string(av[i]) == "-gameDir")
        {
            _gameDir = av[i + 1];
            ++i;
        }
        else if (std::string(av[i]) == "-ip")
        {
            _ip = av[i + 1];
            ++i;
        }
        else if (std::string(av[i]) == "-port")
        {
            _port = static_cast<unsigned short>(std::stoi(av[i + 1]));
            if (_port < 1024 || _port > 65535)
                throw std::runtime_error("Invalid port number, must be between 1024 and 65535");
            ++i;
        }
        else if (std::string(av[i]) == "-h" || std::string(av[i]) == "--help")
        {
            std::cout << HELP_MESSAGE << std::endl;
            exit(0);
        }
        else if (std::string(av[i]) == "-v" || std::string(av[i]) == "--version")
        {
            std::cout << "Flakkari Library v" FLAKKARI_VERSION_STRING " - © 2024 MasterLaplace" << std::endl;
            exit(0);
        }
    }

    if (_gameDir.empty())
        GetGameDirEnv();

    if (_ip.empty() && (!GetIPv4Addresses() && !GetIPv6Addresses()))
        _ip = "localhost";

    if (_port == 0)
        _port = 8081;
}

const std::string &ParseArgument::getGameDir() const { return _gameDir; }

const std::string &ParseArgument::getIp() const { return _ip; }

unsigned short ParseArgument::getPort() const { return _port; }

bool ParseArgument::GetIPv4Addresses()
{
#if defined(FLAKKARI_SYSTEM_WINDOWS)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw std::runtime_error("WSAStartup failed!");

    IP_ADAPTER_ADDRESSES *pAdapterAddresses = nullptr;
    ULONG bufferSize = 0;
    DWORD dwRetVal =
        GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_ALL_INTERFACES, nullptr, pAdapterAddresses, &bufferSize);

    if (dwRetVal == ERROR_BUFFER_OVERFLOW)
    {
        pAdapterAddresses = (IP_ADAPTER_ADDRESSES *) malloc(bufferSize);
        dwRetVal =
            GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_ALL_INTERFACES, nullptr, pAdapterAddresses, &bufferSize);
    }

    if (dwRetVal != NO_ERROR)
    {
        free(pAdapterAddresses);
        WSACleanup();
        throw std::runtime_error("GetAdaptersAddresses failed with error: " + std::to_string(dwRetVal));
    }

    IP_ADAPTER_ADDRESSES *pAdapter = pAdapterAddresses;
    while (pAdapter)
    {
        IP_ADAPTER_UNICAST_ADDRESS *pUnicast = pAdapter->FirstUnicastAddress;
        while (pUnicast)
        {
            sockaddr_in *sa_in = (sockaddr_in *) pUnicast->Address.lpSockaddr;
            pUnicast = pUnicast->Next;

            if (sa_in->sin_family != AF_INET)
                continue;

            char ipStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(sa_in->sin_addr), ipStr, INET_ADDRSTRLEN);
            FLAKKARI_LOG_INFO("Adresse IPv4 de l'interface " + std::string(pAdapter->AdapterName) + ": " + ipStr);
            _ip = ipStr;
        }
        pAdapter = pAdapter->Next;
    }

    free(pAdapterAddresses);
    WSACleanup();
#else
    struct ifaddrs *ifAddrStruct = nullptr;
    struct ifaddrs *ifa = nullptr;

    if (getifaddrs(&ifAddrStruct) == -1)
        throw std::runtime_error("getifaddrs failed!");

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr->sa_family != AF_INET)
            continue;

        void *tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
        char addressBuffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

        if (strcmp(addressBuffer, "127.0.0.1") != 0)
            FLAKKARI_LOG_INFO("Interface " + ifa->ifa_name + " a l'adresse IPv4 : " + addressBuffer);
    }

    freeifaddrs(ifAddrStruct);
#endif
}

bool ParseArgument::GetIPv6Addresses()
{
#if defined(FLAKKARI_SYSTEM_WINDOWS)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw std::runtime_error("WSAStartup failed!");

    IP_ADAPTER_ADDRESSES *pAdapterAddresses = nullptr;
    ULONG bufferSize = 0;
    DWORD dwRetVal =
        GetAdaptersAddresses(AF_INET6, GAA_FLAG_INCLUDE_ALL_INTERFACES, nullptr, pAdapterAddresses, &bufferSize);

    if (dwRetVal == ERROR_BUFFER_OVERFLOW)
    {
        pAdapterAddresses = (IP_ADAPTER_ADDRESSES *) malloc(bufferSize);
        dwRetVal =
            GetAdaptersAddresses(AF_INET6, GAA_FLAG_INCLUDE_ALL_INTERFACES, nullptr, pAdapterAddresses, &bufferSize);
    }

    if (dwRetVal != NO_ERROR)
    {
        free(pAdapterAddresses);
        WSACleanup();
        throw std::runtime_error("GetAdaptersAddresses failed with error: " + std::to_string(dwRetVal));
    }

    IP_ADAPTER_ADDRESSES *pAdapter = pAdapterAddresses;
    while (pAdapter)
    {
        IP_ADAPTER_UNICAST_ADDRESS *pUnicast = pAdapter->FirstUnicastAddress;
        while (pUnicast)
        {
            sockaddr_in6 *sa_in6 = (sockaddr_in6 *) pUnicast->Address.lpSockaddr;
            pUnicast = pUnicast->Next;

            if (sa_in6->sin6_family != AF_INET6)
                continue;

            char ipStr[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &(sa_in6->sin6_addr), ipStr, INET6_ADDRSTRLEN);
            if (strcmp(ipStr, "::1") != 0)
            {
                FLAKKARI_LOG_INFO("IPv6 address of interface " + std::string(pAdapter->AdapterName) + ": " + ipStr);
                _ip = ipStr;
            }
        }
        pAdapter = pAdapter->Next;
    }

    free(pAdapterAddresses);
    WSACleanup();
#else
    struct ifaddrs *ifAddrStruct = nullptr;
    struct ifaddrs *ifa = nullptr;

    if (getifaddrs(&ifAddrStruct) == -1)
        throw std::runtime_error("getifaddrs failed!");

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr->sa_family != AF_INET6)
            continue;

        void *tmpAddrPtr = &((struct sockaddr_in6 *) ifa->ifa_addr)->sin6_addr;
        char addressBuffer[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);

        if (strcmp(addressBuffer, "::1") != 0)
            FLAKKARI_LOG_INFO("Interface " + ifa->ifa_name + " a l'adresse IPv6 : " + addressBuffer);
    }

    freeifaddrs(ifAddrStruct);
#endif
}

void ParseArgument::GetGameDirEnv()
{
    const char *dir = std::getenv("FLAKKARI_GAME_DIR");

    if (dir)
        _gameDir = dir;
    else
        throw std::runtime_error("FLAKKARI_GAME_DIR not set");
}

} /* namespace Flakkari */
