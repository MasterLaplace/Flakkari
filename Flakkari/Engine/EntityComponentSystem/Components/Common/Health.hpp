/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2023-01-14
** File description:
** Health
*/

#ifndef Health_HPP_
#define Health_HPP_

#include <cstddef>

#include "Network/Packed.hpp"

namespace Flakkari::Engine::ECS::Components::Common {
PACKED_START

/**
 * @brief Health is a structure that represents the life of an "living object"
 *
 */
struct Health {
    std::size_t currentHealth;
    std::size_t maxHealth = 100;
    std::size_t shield = 0;
    std::size_t maxShield = 100;

    Health() : currentHealth(100), maxHealth(100), shield(0), maxShield(100) {};
    Health(std::size_t currentHealth, std::size_t maxHealth, std::size_t shield, std::size_t maxShield) :
        currentHealth(currentHealth), maxHealth(maxHealth), shield(shield), maxShield(maxShield) {};
    Health(const Health &other) : currentHealth(other.currentHealth), maxHealth(other.maxHealth), shield(other.shield), maxShield(other.maxShield) {};

    std::size_t size() const {
        return sizeof(*this);
    }
};

PACKED_END
} // namespace Flakkari::Engine::ECS::Components::Common

#endif /* !Health_HPP_ */
