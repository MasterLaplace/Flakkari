/**************************************************************************
 * Flakkari Library v0.1.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Entity.hpp
 * @brief Entity class for ECS (Entity Component System).
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.1.0
 * @date 2023-01-05
 **************************************************************************/

#ifndef ENTITY_HPP_
    #define ENTITY_HPP_

#include <cstddef>

namespace Flakkari::Engine::ECS {

class Registry;

class Entity {
public:
    friend class Registry;

    explicit Entity(std::size_t id) : _id(id) {}
    Entity() : _id(0) {}

    operator std::size_t() const { return _id;}

    std::size_t operator++() { return ++_id; }
    Entity &operator=(std::size_t id) { _id = id; return *this; }
    Entity &operator=(int id) { _id = (id < 0) ? 0 : id; return *this; }

private:
    std::size_t _id;
};

} // namespace Flakkari::Engine::ECS

#endif /* !ENTITY_HPP_ */
