/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2024-01-06
** File description:
** Parent
*/

#ifndef FLAKKARI_PARENT_HPP_
#define FLAKKARI_PARENT_HPP_

#include <cstddef>

#include "flakkari_config.h"

namespace Flakkari::Engine::ECS::Components::Common {
LPL_PACKED_START

/**
 * @brief Parent component for ECS entities that have a parent entity attached to them
 *
 * @details This component is used to store the parent entity of an entity in the ECS
 */
struct Parent {
    std::size_t entity;

    Parent() : entity(0) {}
    Parent(const std::size_t &entity) : entity(entity) {}
    Parent(const Parent &other) : entity(other.entity) {}

    Parent &operator=(const Parent &other)
    {
        if (this != &other)
            entity = other.entity;

        return *this;
    }

    std::size_t size() const { return sizeof(*this); }
};

LPL_PACKED_END
} // namespace Flakkari::Engine::ECS::Components::Common

#endif /* !FLAKKARI_PARENT_HPP_ */
