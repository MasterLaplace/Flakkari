/**************************************************************************
 * Flakkari Library v0.2.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file ResourceManager.hpp
 * @brief This file contains the ResourceManager class. It is used to load
 *        and store resources (templates).
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.2.0
 * @date 2024-01-12
 **************************************************************************/

#ifndef RESOURCEMANAGER_HPP_
#define RESOURCEMANAGER_HPP_

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <unordered_map>

#include "Logger/Logger.hpp"

namespace Flakkari {

/**
 * @brief ResourceManager class
 *
 * This class is used to load and store resources (teemplates)
 * It is a singleton, so it can be accessed from anywhere in the code
 * It is initialized with a config file path, which contains the paths to the resources
 * The config file must be a JSON file, with the following structure:
 *
 * @example "Games/<game>/config.json"
 * @code
 * ResourceManager::adddScene("Games/<game>/config.json", "<scene>");
 * auto template = ResourceManager::getTemplateById("<game>", "<scene>", "<templateId>");
 * @endcode
 *
 * TODO: add a way to unload resources
 * TODO: add a way to load multiple config files cause multiple scenes could be used for multiple windows
 */
class ResourceManager {
    private:
    static std::shared_ptr<ResourceManager> _instance;
    static std::mutex _mutex;

    using nl_template = nlohmann::json;

    public:
    std::map<std::string /*game*/, std::map<std::string /*scene*/, std::map<std::string /*template*/, nl_template>>>
        _templates;

    ResourceManager() = default;
    ~ResourceManager() = default;

    public:
    ResourceManager(const ResourceManager &) = delete;
    ResourceManager(const std::shared_ptr<ResourceManager> &) = delete;
    void operator=(const ResourceManager &) = delete;
    void operator=(const std::shared_ptr<ResourceManager> &) = delete;

    /**
     * @brief Get the ResourceManager instance
     *
     * @param configPath The path to the config file
     * @return ResourceManager & The ResourceManager instance
     */
    static std::shared_ptr<ResourceManager> getInstance();

    /**
     * @brief Add a scene to the ResourceManager instance
     *
     * @param configPath  The path to the config file of the game to add
     * @param scene  The scene to add to the ResourceManager instance
     */
    static void addScene(std::shared_ptr<nlohmann::json> config, const std::string &scene);

    /**
     * @brief Delete a scene from the ResourceManager instance
     *
     * @param configPath  The path to the config file of the game to delete
     * @param scene  The scene to delete from the ResourceManager instance
     */
    static void deleteScene(const std::string &game, const std::string &scene);

    /**
     * @brief Get the Template By Id object from the config file of the game
     *
     * @param game  The game to get the template from (name of the file in Games/ folder)
     * @param scene  The scene to get the template from (name of the file in Games/<game>/Scenes/ folder)
     * @param templateId  The id of the template to get (name of the template in the config file)
     * @return std::optional<nl_template>  The template if found, std::nullopt otherwise
     */
    [[nodiscard]] static std::optional<nl_template> getTemplateById(const std::string &game, const std::string &scene,
                                                                    const std::string &templateId);

    private:
    void loadConfig(std::shared_ptr<nlohmann::json> config, const std::string &scene);
};

} /* namespace Flakkari */

#endif /* !RESOURCEMANAGER_HPP_ */
