/**************************************************************************
 * Flakkari Library v0.10.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file IEntity.hpp
 * @brief IEntity class for ECS (Entity Component System) - Simple Entity wrapper.
 *
 * This class provides a simple entity wrapper that can be used with different
 * ECS backends. It stores an entity ID and provides common operations.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.10.0
 * @date 2025-12-19
 **************************************************************************/

#ifndef FLAKKARI_IENTITY_HPP_
#define FLAKKARI_IENTITY_HPP_

#include <cstddef>
#include <cstdint>
#include <memory>

namespace Flakkari::Engine::API {

class IRegistry;

/**
 * @class IEntity
 * @brief Simple entity class that wraps an entity ID.
 *
 * This class provides a simple entity wrapper that can be used with different
 * ECS backends. Unlike a pure interface, this is a concrete class that can be
 * instantiated and used directly.
 */
class IEntity {
public:
    friend class IRegistry;

    /**
     * @brief Construct an IEntity from an ID.
     * @param id The entity ID (default 0).
     */
    explicit IEntity(std::size_t id = 0) : _id(id) {}

    ~IEntity() = default;

    // Copy and move constructors/operators
    IEntity(const IEntity &) = default;
    IEntity(IEntity &&) = default;
    IEntity &operator=(const IEntity &) = default;
    IEntity &operator=(IEntity &&) = default;

    /**
     * @brief Get the unique identifier of the entity.
     * @return The entity's ID as a size_t.
     */
    std::size_t getId() const { return _id; }

    /**
     * @brief Check if two entities are equal.
     * @param other The other entity to compare with.
     * @return true if entities have the same ID.
     */
    bool operator==(const IEntity &other) const { return _id == other._id; }

    /**
     * @brief Check if two entities are different.
     * @param other The other entity to compare with.
     * @return true if entities have different IDs.
     */
    bool operator!=(const IEntity &other) const { return _id != other._id; }

    /**
     * @brief Less than comparison for ordering.
     * @param other The other entity to compare with.
     * @return true if this entity's ID is less than other's.
     */
    bool operator<(const IEntity &other) const { return _id < other._id; }

    /**
     * @brief Less than or equal comparison.
     * @param other The other entity to compare with.
     * @return true if this entity's ID is less than or equal to other's.
     */
    bool operator<=(const IEntity &other) const { return _id <= other._id; }

    /**
     * @brief Greater than comparison.
     * @param other The other entity to compare with.
     * @return true if this entity's ID is greater than other's.
     */
    bool operator>(const IEntity &other) const { return _id > other._id; }

    /**
     * @brief Greater than or equal comparison.
     * @param other The other entity to compare with.
     * @return true if this entity's ID is greater than or equal to other's.
     */
    bool operator>=(const IEntity &other) const { return _id >= other._id; }

    /**
     * @brief Implicit conversion to size_t for compatibility.
     * @return The entity's ID.
     */
    operator std::size_t() const { return _id; }

    /**
     * @brief Assignment from size_t.
     * @param id The new entity ID.
     * @return Reference to this entity.
     */
    IEntity &operator=(std::size_t id)
    {
        _id = id;
        return *this;
    }

    /**
     * @brief Pre-increment operator.
     * @return The incremented entity ID.
     */
    std::size_t operator++() { return ++_id; }

    /**
     * @brief Post-increment operator.
     * @return The entity ID before increment.
     */
    std::size_t operator++(int) { return _id++; }

private:
    std::size_t _id;
};

} // namespace Flakkari::Engine::API

#include <unordered_map>

namespace std {
template <> struct hash<Flakkari::Engine::API::IEntity> {
    size_t operator()(const Flakkari::Engine::API::IEntity &entity) const noexcept
    {
        return std::hash<std::size_t>()(entity.getId());
    }
};
} // namespace std

#endif /* !FLAKKARI_IENTITY_HPP_ */
