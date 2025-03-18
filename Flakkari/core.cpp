/*
** EPITECH PROJECT, 2023
** Flakkari
** File description:
** core
*/

#include "ParseArgument.hpp"

int main(int ac, const char *av[])
{
    try
    {
        Flakkari::ParseArgument parseArg(ac, av);

        Flakkari::UDPServer server(parseArg.getGameDir(), parseArg.getIp(), parseArg.getPort());

        if (parseArg.isDryRun())
            return 0;

        server.run();
    }
    catch (const std::exception &e)
    {
        if (std::string(e.what()) != "exit")
            return FLAKKARI_LOG_FATAL(e.what()), 84;
    }
    catch (...)
    {
        return FLAKKARI_LOG_FATAL("Unknown error"), 84;
    }
    return 0;
}
