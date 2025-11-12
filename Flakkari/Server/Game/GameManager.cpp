/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2024-01-07
** File description:
** GameManager
*/

#include "GameManager.hpp"
#include "../Client/Client.hpp"

#include <future>

namespace Flakkari {

#define STR_ADDRESS std::string(*client->getAddress())

GameManager::GameManager(const std::string &gameDir) : _game_dir(gameDir)
{
    if (_game_dir.empty())
        throw std::runtime_error("No game directory set: please download games in the Games folder");

    for (const auto &entry : std::filesystem::directory_iterator(_game_dir))
    {
        std::string gameName = entry.path().string();
        gameName = gameName.substr(std::max(gameName.find_last_of("/") + 1, gameName.find_last_of("\\") + 1));

        if (gameName == ".gitkeep")
            continue;

        if (_gamesStore.find(gameName) != _gamesStore.end())
        {
            FLAKKARI_LOG_ERROR("game already loaded");
            continue;
        }

        std::ifstream configFile(_game_dir + "/" + gameName + "/config.cfg");
        nlohmann::json config;

        if (!configFile.is_open())
        {
            FLAKKARI_LOG_ERROR("could not open config file for \"" + gameName + "\" game");
            continue;
        }
        configFile >> config;

        if (config["scenes"].empty())
        {
            FLAKKARI_LOG_ERROR("no scenes found");
            continue;
        }

        _gamesStore[gameName] = std::make_shared<nlohmann::json>(config);
        FLAKKARI_LOG_INFO("\"" + gameName + "\" game loaded");
    }
}

int GameManager::addGame(const std::string &gameName)
{
    if (_gamesStore.find(gameName) != _gamesStore.end())
        return FLAKKARI_LOG_ERROR("game already loaded"), 1;
    std::ifstream configFile(_game_dir + "/" + gameName + "/config.cfg");
    nlohmann::json config;

    if (!configFile.is_open())
        return FLAKKARI_LOG_ERROR("could not open config file"), 3;
    configFile >> config;

    if (config["scenes"].empty())
        return FLAKKARI_LOG_ERROR("no scenes found"), 3;

    _gamesStore[gameName] = std::make_shared<nlohmann::json>(config);
    FLAKKARI_LOG_INFO("\"" + gameName + "\" game loaded");
    return 0;
}

int GameManager::updateGame(const std::string &gameName)
{
    if (_gamesStore.find(gameName) == _gamesStore.end())
        return FLAKKARI_LOG_ERROR("game not found"), 1;
    std::ifstream configFile(_game_dir + "/" + gameName + "/config.cfg");
    nlohmann::json config;

    if (!configFile.is_open())
        return FLAKKARI_LOG_ERROR("could not open config file"), 3;
    configFile >> config;

    _gamesStore[gameName] = std::make_shared<nlohmann::json>(config);
    FLAKKARI_LOG_INFO("\"" + gameName + "\" game updated");
    return 0;
}

int GameManager::removeGame(const std::string &gameName)
{
    if (_gamesStore.find(gameName) == _gamesStore.end())
        return FLAKKARI_LOG_ERROR("game not found"), 1;

    _gamesRemoveRequest[gameName] = true;
    FLAKKARI_LOG_INFO("\"" + gameName + "\" game removed");
    return 0;
}

void GameManager::listGames()
{
    std::string gamesList = "Games list:\n";

    for (const auto &game : _gamesStore)
        gamesList += " - " + game.first + "\n";
    FLAKKARI_LOG_INFO(gamesList);
}

bool GameManager::addClientToGame(const std::string &gameName, std::shared_ptr<Client> client)
{
    if (_gamesStore.find(gameName) == _gamesStore.end())
        return FLAKKARI_LOG_ERROR("game not found"), false;

    auto &gameStore = _gamesStore[gameName];
    auto &gameInstance = _gamesInstances[gameName];

    if (gameStore->at("online").get<bool>() == false)
        return FLAKKARI_LOG_ERROR("game \"" + gameName + "\" is'nt an online game"), false;

    auto minPlayers = gameStore->at("minPlayers").get<size_t>();
    auto maxPlayers = gameStore->at("maxPlayers").get<size_t>();
    auto maxInstances = gameStore->at("maxInstances").get<size_t>();
    auto lobby = gameStore->at("lobby").get<std::string>();

    FLAKKARI_LOG_INFO("<Game>: " + gameName + ", minPlayers: " + std::to_string(minPlayers) +
                      ",  maxPlayers: " + std::to_string(maxPlayers) +
                      ", maxInstances: " + std::to_string(maxInstances) + ", lobby: " + lobby);

    if (gameInstance.empty() || gameInstance.back()->getPlayers().size() >= maxPlayers)
    {
        if (gameInstance.size() >= maxInstances)
        {
            FLAKKARI_LOG_ERROR("game \"" + gameName + "\"is full");
            _waitingClients[gameName].push(client);
            return true;
        }
        gameInstance.push_back(std::make_shared<Game>(gameName, gameStore));
        FLAKKARI_LOG_INFO("game \"" + gameName + "\" created");
    }

    if (gameInstance.back()->addPlayer(client))
    {
        if (lobby == "Matchmaking" && gameInstance.back()->getPlayers().size() >= minPlayers &&
            !gameInstance.back()->isRunning())
            gameInstance.back()->start();
        if (lobby == "OpenWorld" && !gameInstance.back()->isRunning())
            gameInstance.back()->start();
        return true;
    }
    FLAKKARI_LOG_ERROR("could not add client \"" + STR_ADDRESS + "\" to game \"" + gameName + "\"");
    return false;
}

bool GameManager::removeClientFromGame(const std::string &gameName, const std::shared_ptr<Client> &client)
{
    if (_gamesStore.find(gameName) == _gamesStore.end())
        return FLAKKARI_LOG_ERROR("game not found"), true;

    auto &waitingQueue = _waitingClients[gameName];

    auto minPlayers = _gamesStore[gameName]->at("minPlayers").get<size_t>();

    for (auto &instance : _gamesInstances[gameName])
    {
        if (!instance->removePlayer(client))
            continue;

        if (instance->getPlayers().empty())
        {
            _gamesInstances[gameName].pop_back();
            FLAKKARI_LOG_INFO("game \"" + gameName + "\" removed");
            return true;
        }
        else if (instance->getPlayers().size() > minPlayers)
        {
            FLAKKARI_LOG_INFO("game \"" + gameName + "\" is not full anymore");

            if (waitingQueue.empty())
            {
                if (_gamesRemoveRequest[gameName])
                {
                    _gamesStore[gameName].reset();
                    _gamesStore.erase(gameName);
                    _gamesRemoveRequest.erase(gameName);
                    FLAKKARI_LOG_INFO("game \"" + gameName + "\" removed from store following request");
                    return false;
                }
                return true;
            }
            auto waitingClient = waitingQueue.front();
            if (instance->addPlayer(waitingClient))
            {
                waitingQueue.pop();
                return true;
            }
            FLAKKARI_LOG_WARNING("could not add client \"" + STR_ADDRESS + "\" to game \"" + gameName + "\"");
            if (waitingClient->isConnected())
                waitingQueue.pop();
        }
        return true;
    }
    FLAKKARI_LOG_ERROR("could not remove client \"" + STR_ADDRESS + "\" from game \"" + gameName + "\"");
    return true;
}

bool GameManager::isClientWaiting(const std::string &gameName)
{
    if (_gamesStore.find(gameName) == _gamesStore.end())
        return FLAKKARI_LOG_ERROR("game not found"), true;

    return _waitingClients[gameName].empty();
}

int GameManager::getIndexInWaitingQueue(const std::string &gameName, const std::shared_ptr<Client> &client)
{
    if (_gamesStore.find(gameName) == _gamesStore.end())
        return FLAKKARI_LOG_ERROR("game not found"), -1;

    auto tmpQueue = _waitingClients[gameName];
    for (int i = 0; !tmpQueue.empty(); i++)
    {
        if (tmpQueue.front() == client)
            return FLAKKARI_LOG_INFO("client \"" + STR_ADDRESS + "\" found in waiting queue at " + std::to_string(i)),
                   i;
        tmpQueue.pop();
    }
    return FLAKKARI_LOG_ERROR("client \"" + STR_ADDRESS + "\" not found in waiting queue"), -1;
}

} /* namespace Flakkari */
