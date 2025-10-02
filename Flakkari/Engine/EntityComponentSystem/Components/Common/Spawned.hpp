/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2024-01-14
** File description:
** Spawned
*/

#ifndef SPAWNED_HPP_
#define SPAWNED_HPP_

#include <cstring>
#include <string>

#include "flakkari_config.h"

namespace Flakkari::Engine::ECS::Components::Common {
LPL_PACKED_START

/**
 * @brief  Spawned component for ECS entities that have a script attached to them
 *
 * @details This component is used to store the path to the script that will be executed
 */
struct Spawned {
    bool has_spawned;

    Spawned() : has_spawned(false) {}
    Spawned(bool spawed) : has_spawned(spawed) {}
    Spawned(const Spawned &other) : has_spawned(other.has_spawned) {}

    Spawned &operator=(const Spawned &other)
    {
        if (this != &other)
            has_spawned = other.has_spawned;

        return *this;
    }

    std::size_t size() const { return sizeof(has_spawned); }
};

LPL_PACKED_END
} // namespace Flakkari::Engine::ECS::Components::Common

#endif /* !SPAWNED_HPP_ */
