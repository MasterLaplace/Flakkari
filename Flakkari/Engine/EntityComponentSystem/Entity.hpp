/**************************************************************************
 * Flakkari Library v0.9.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Entity.hpp
 * @brief Entity class for ECS (Entity Component System).
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.9.0
 * @date 2024-01-05
 **************************************************************************/

#ifndef FLAKKARI_ENTITY_HPP_
#define FLAKKARI_ENTITY_HPP_

#include <cstddef>
#include <cstdint>

namespace Flakkari::Engine::ECS {

class Registry;

class Entity {
public:
    friend class Registry;

    explicit Entity(std::size_t id) : _id(id) {}
    Entity() : _id(0) {}

    operator std::size_t() const { return _id; }

    std::size_t operator++() { return ++_id; }
    Entity &operator=(std::size_t id)
    {
        _id = id;
        return *this;
    }
    Entity &operator=(int id)
    {
        _id = (id < 0) ? 0 : id;
        return *this;
    }

    bool operator==(const Entity &other) const { return _id == other._id; }

    std::size_t getId() const { return _id; }

private:
    std::size_t _id;
};

} // namespace Flakkari::Engine::ECS

#include <unordered_map>

namespace std {
template <> struct hash<Flakkari::Engine::ECS::Entity> {
    size_t operator()(const Flakkari::Engine::ECS::Entity &entity) const noexcept
    {
        return std::hash<std::size_t>()(entity.getId());
    }
};
} // namespace std

#endif /* !FLAKKARI_ENTITY_HPP_ */
