/**************************************************************************
 * Flakkari Engine Library v0.10.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Systems.hpp
 * @brief Systems header. Contains all systems.
 *        (position, rigid_body)
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.10.0
 * @date 2024-01-06
 **************************************************************************/

#ifndef FLAKKARI_SYSTEMS_HPP_
#define FLAKKARI_SYSTEMS_HPP_

#include "EntityComponentSystem/Factory.hpp"

#include <chrono>
#include <cmath>
#include <cstdlib>

namespace Flakkari::Engine::ECS::Systems::_2D {

/**
 * @brief Updates the position of all entities with a Position and a Movable component based on their velocity.
 *
 * @note The velocity vector is normalized following this advice: https://youtube.com/shorts/0cYjreg7dpg
 *
 * @param r  The registry containing the entities to update.
 * @param deltaTime  The time elapsed since the last update.
 */
void position(Registry &r, float deltaTime);

} // namespace Flakkari::Engine::ECS::Systems::_2D

namespace Flakkari::Engine::ECS::Systems::_3D {

/**
 * @brief Updates the position of all entities with a Transform and a Movable component based on their velocity.
 *
 * @param r  The registry containing the entities to update.
 * @param deltaTime  The time elapsed since the last update.
 */
void apply_movable(Registry &r, float deltaTime);

/**
 * @brief Spawns a random entity within a skybox.
 *
 * @param r  The registry containing the entities to update.
 * @param entities  The updated entities.
 */
void spawn_random_within_skybox(Registry &r, std::vector<Entity> &entities);

/**
 * @brief Spawns an enemy entity.
 *
 * @param r  The registry containing the entities to update.
 * @param templateName  The name of the template to use for the enemy entity.
 * @param entity  The entity to spawn.
 * @return true if the enemy was spawned, false otherwise.
 */
bool spawn_enemy(Registry &r, std::string &templateName, Entity &entity);

/**
 * @brief Handles collisions between entities.
 *
 * @param r  The registry containing the entities to update.
 * @param entities  The updated entities map.
 *
 * @details This function will handle collisions between entities.
 *          It will check if the entities are out of the skybox and resolve the collision.
 *          It will also check for collisions between entities with a BoxCollider component.
 *          If a collision is detected, it will resolve the collision.
 *
 * @details This function will also handle the death of entities.
 *          If an entity's health reaches 0, it will be killed.
 *
 * @details The entities map will store the updated or killed entity
 *          - false means the entity is killed
 *          - true means the entity was updated
 */
void handle_collisions(Registry &r, std::unordered_map<Entity, bool> &entities);

} // namespace Flakkari::Engine::ECS::Systems::_3D

#endif /* !FLAKKARI_SYSTEMS_HPP_ */
