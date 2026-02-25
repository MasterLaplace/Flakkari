/*
** EPITECH PROJECT, 2025
** Flakkari
** File description:
** FlakkariEntity - Wrapper for native Flakkari ECS Entity
*/

#ifndef FLAKKARI_FLAKKARI_ENTITY_HPP_
#define FLAKKARI_FLAKKARI_ENTITY_HPP_

#include "API/IEntity.hpp"
#include "Entity.hpp"

namespace Flakkari::Engine::API {

/**
 * @class FlakkariEntity
 * @brief Wrapper class that provides conversions between IEntity and native ECS::Entity.
 *
 * This class extends IEntity to add convenient conversions to the native Flakkari
 * ECS Entity type. It can be used when direct access to the native entity is needed.
 */
class FlakkariEntity : public IEntity {
public:
    /**
     * @brief Construct a FlakkariEntity from an ECS::Entity.
     * @param entity The native ECS entity.
     */
    explicit FlakkariEntity(const ECS::Entity &entity) : IEntity(entity.getId()), _entity(entity) {}

    /**
     * @brief Construct a FlakkariEntity from an entity ID.
     * @param id The entity ID.
     */
    explicit FlakkariEntity(std::size_t id = 0) : IEntity(id), _entity(id) {}

    ~FlakkariEntity() = default;

    /**
     * @brief Get the underlying native ECS entity.
     * @return Reference to the native ECS::Entity.
     */
    ECS::Entity &getNative() { return _entity; }

    /**
     * @brief Get the underlying native ECS entity (const).
     * @return Const reference to the native ECS::Entity.
     */
    const ECS::Entity &getNative() const { return _entity; }

    /**
     * @brief Implicit conversion to the native ECS::Entity.
     */
    operator ECS::Entity &() { return _entity; }

    /**
     * @brief Implicit conversion to the native ECS::Entity (const).
     */
    operator const ECS::Entity &() const { return _entity; }

private:
    ECS::Entity _entity;
};

} // namespace Flakkari::Engine::API

#endif /* !FLAKKARI_FLAKKARI_ENTITY_HPP_ */
