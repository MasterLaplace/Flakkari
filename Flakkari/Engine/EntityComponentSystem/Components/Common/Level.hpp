/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2024-01-14
** File description:
** Level
*/

#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <cstring>
#include <string>

#include "flakkari_config.h"

namespace Flakkari::Engine::ECS::Components::Common {

/**
 * @brief FontInfo is a structure that holds information about a Font
 *
 */
struct Level {
    std::size_t level;
    std::string currentWeapon;
    std::size_t currentExp;
    std::size_t requiredExp;

    Level() : level(1), currentWeapon(""), currentExp(0), requiredExp(100) {}
    Level(std::size_t level, std::string currentWeapon, std::size_t currentExp, std::size_t requiredExp)
        : level(level), currentWeapon(currentWeapon.c_str()), currentExp(currentExp), requiredExp(requiredExp)
    {
    }
    Level(const Level &other)
        : level(other.level), currentWeapon(other.currentWeapon), currentExp(other.currentExp),
          requiredExp(other.requiredExp)
    {
    }

    Level &operator=(const Level &other)
    {
        if (this != &other)
        {
            level = other.level;
            currentWeapon = other.currentWeapon;
            currentExp = other.currentExp;
            requiredExp = other.requiredExp;
        }

        return *this;
    }

    std::size_t size() const { return sizeof(level) + currentWeapon.size() + sizeof(currentExp) + sizeof(requiredExp); }
};

} // namespace Flakkari::Engine::ECS::Components::Common

#endif // LEVEL_HPP
