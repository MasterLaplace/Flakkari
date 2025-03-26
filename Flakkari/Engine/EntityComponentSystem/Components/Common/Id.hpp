/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2024-01-06
** File description:
** Id
*/

#ifndef FLAKKARI_ID_HPP_
#define FLAKKARI_ID_HPP_

#include <cstdint>

#include "flakkari_config.h"

namespace Flakkari::Engine::ECS::Components::Common {
LPL_PACKED_START

struct Id {
    std::size_t id;

    Id() : id(0) {}
    Id(std::size_t id) : id(id) {}
    Id(const Id &other) : id(other.id) {}

    Id &operator=(const Id &other)
    {
        if (this != &other)
            id = other.id;

        return *this;
    }

    std::size_t size() const { return sizeof(id); }
};

LPL_PACKED_END
} // namespace Flakkari::Engine::ECS::Components::Common

#endif /* !FLAKKARI_ID_HPP_ */
