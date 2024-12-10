/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2024-01-12
** File description:
** ResourceManager
*/

#include "ResourceManager.hpp"

namespace Flakkari {

void ResourceManager::addScene(const std::shared_ptr<nlohmann::json> &config, const std::string &scene)
{
    for (auto &_scene : (*config)["scenes"].items())
    {
        for (auto sceneInfo : _scene.value().items())
        {
            if (sceneInfo.key() != scene)
                continue;

            for (auto &template_ : sceneInfo.value()["templates"].items())
            {
                for (auto &templateInfo : template_.value().items())
                    _templates[(*config)["title"]][sceneInfo.key()][templateInfo.key()] = templateInfo.value();
            }
        }
    }
}

void ResourceManager::deleteScene(const std::string &game, const std::string &scene) { _templates[game].erase(scene); }

std::optional<ResourceManager::nl_template>
ResourceManager::getTemplateById(const std::string &game, const std::string &scene, const std::string &templateId)
{
    return _templates[game][scene][templateId];
}

void ResourceManager::deleteGame(const std::string &game)
{
    _templates.erase(game);
}

} // namespace Flakkari
