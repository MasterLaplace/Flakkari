/**************************************************************************
 * Flakkari Engine Library v0.7.1
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Factory.hpp
 * @brief Flakkari::Engine::ECS::Factory functions header. This class is
 *        used to create an entity from a template. It is used to create
 *        entities from the server or from the client.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.7.1
 * @date 2024-01-13
 **************************************************************************/

#ifndef FLAKKARI_ECS_FACTORY_HPP_
#define FLAKKARI_ECS_FACTORY_HPP_

#include <nlohmann/json.hpp>

#include "Registry.hpp"

#include "Components/Components2D.hpp"
#include "Components/Components3D.hpp"
#include "Components/ComponentsCommon.hpp"

namespace Flakkari::Engine::ECS::Factory {

using nl_template = nlohmann::json;
using nl_templates = nlohmann::json;

/**
 * @brief Create a Entity From Template object based on a template JSON
 *
 * @details This function will create an entity based on the template JSON
 *        (using the template name)
 *
 * @see ResourceManager::getTemplateById
 *
 * @param registry  The registry to add the entity to
 * @param templateJson  The template JSON
 * @return Entity  The created entity
 */
Entity createEntityFromTemplate(Registry &registry, const nl_template &templateJson);

/**
 * @brief Add an entity to the registry based on a template JSON
 *
 * @details This function will add all the components to the entity
 *         based on the template JSON (using the template name)
 *
 * @see ResourceManager::getTemplateById
 *
 * @param registry  The registry to add the entity to
 * @param entity  The entity to add the components to
 * @param templateJson  The template JSON
 */
void RegistryEntityByTemplate(Registry &registry, Entity entity, const nl_template &templateJson,
                              const nl_templates &templates = nl_templates());

} // namespace Flakkari::Engine::ECS::Factory

#endif /* !FLAKKARI_ECS_FACTORY_HPP_ */
