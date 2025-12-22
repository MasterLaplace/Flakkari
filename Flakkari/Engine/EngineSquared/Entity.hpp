/*
** EPITECH PROJECT, 2025
** Flakkari
** File description:
** ESEntity - Wrapper for EngineSquared Entity
*/

#ifndef ES_ENTITY_HPP_
#define ES_ENTITY_HPP_

#include "API/IEntity.hpp"

// Forward declare EngineSquared types to avoid include dependency
// The actual implementation will include the real headers
namespace Engine {
class Entity;
class Core;
} // namespace Engine

namespace Flakkari::Engine::API {

/**
 * @class ESEntity
 * @brief Wrapper class that adapts EngineSquared's Entity to the IEntity interface.
 *
 * This class allows EngineSquared to be used through the abstract IEntity interface,
 * enabling polymorphic usage of different ECS backends.
 */
class ESEntity : public IEntity {
public:
    using entity_id_type = uint32_t;
    static constexpr entity_id_type null_entity = static_cast<entity_id_type>(-1);

    /**
     * @brief Construct an ESEntity from an entity ID.
     * @param id The entity ID.
     */
    explicit ESEntity(entity_id_type id = null_entity) : _entityId(id) {}

    /**
     * @brief Construct an ESEntity from a size_t ID.
     * @param id The entity ID as size_t.
     */
    explicit ESEntity(std::size_t id) : _entityId(static_cast<entity_id_type>(id)) {}

    ~ESEntity() override = default;

    /**
     * @brief Get the entity's unique identifier.
     * @return The entity ID as size_t.
     */
    std::size_t getId() const override { return static_cast<std::size_t>(_entityId); }

    /**
     * @brief Clone this entity.
     * @return A new ESEntity with the same ID.
     */
    std::unique_ptr<IEntity> clone() const override { return std::make_unique<ESEntity>(_entityId); }

    /**
     * @brief Get the raw entity ID.
     * @return The entity ID.
     */
    entity_id_type getRawId() const { return _entityId; }

    /**
     * @brief Check if this entity is valid (not null).
     * @return true if entity is valid.
     */
    bool isValid() const { return _entityId != null_entity; }

    /**
     * @brief Set the entity ID from an EngineSquared entity.
     * @param id The entity ID.
     */
    void setFromId(entity_id_type id) { _entityId = id; }

private:
    entity_id_type _entityId;
};

} // namespace Flakkari::Engine::API

#endif /* !ES_ENTITY_HPP_ */
