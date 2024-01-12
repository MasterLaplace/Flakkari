/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2023-01-12
** File description:
** ResourceManager
*/

#include "ResourceManager.hpp"

namespace Flakkari {

std::shared_ptr<ResourceManager> ResourceManager::_instance = nullptr;
std::mutex ResourceManager::_mutex;

std::shared_ptr<ResourceManager> ResourceManager::getInstance()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_instance == nullptr)
        _instance = std::make_shared<ResourceManager>();
    return _instance;
}

void ResourceManager::addScene(const std::string &configPath, const std::string &scene)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_instance == nullptr)
        _instance = std::make_shared<ResourceManager>();
    _instance->loadConfig(configPath, scene);
}

void ResourceManager::deleteScene(const std::string &game, const std::string &scene)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_instance == nullptr)
        return;
    _instance->_templates[game].erase(scene);
}

std::optional<ResourceManager::nl_template> ResourceManager::getTemplateById (
    const std::string &game, const std::string &scene, const std::string &templateId
) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_instance == nullptr)
        return std::nullopt;
    return _instance->_templates[game][scene][templateId];
}

void ResourceManager::loadConfig(const std::string &configPath, const std::string &scene)
{
    std::ifstream configFile(configPath);
    nlohmann::json config;

    if (!configFile.is_open())
        return FLAKKARI_LOG_ERROR("could not open config file \"" + configPath + "\""), void();
    configFile >> config;

    for (auto &_scene : config["scenes"].items()) {
        for (auto sceneInfo : _scene.value().items()) {
            if (sceneInfo.key() != scene)
                continue;

            for (auto &template_ : sceneInfo.value()["templates"].items()) {
                for (auto &templateInfo : template_.value().items()) {
                    _templates[config["title"]][sceneInfo.key()][templateInfo.key()] = templateInfo.value();
                }
            }
        }
    }
}

} // namespace Engine::Resource
