/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2024-01-14
** File description:
** Weapon
*/

#ifndef WEAPON_HPP_
#define WEAPON_HPP_

#include <cstring>
#include <string>

#include "flakkari_config.h"

namespace Flakkari::Engine::ECS::Components::Common {
LPL_PACKED_START

/**
 * @brief Weapon is a structure that defines the characteristics of a weapon.
 *
 * @details
 * The Weapon structure is used to define the characteristics of a weapon.
 * It is used by the WeaponComponent to handle the firing and reloading logic.
 *
 * @param
 * minDamage: The amount of damage dealt by the weapon.
 * maxDamage: The maximum amount of damage dealt by the weapon.
 * chargeTime: The time it takes to shoot a charged bullet.
 * chargeMaxTime: The maximum time it takes to charge a bullet at max power.
 * isCharging: A flag that indicates if the weapon is charging.
 * fireRate: The rate of fire, shots per second.
 * timeSinceLastShot: Time elapsed since the last shot was fired.
 */
struct Weapon {
    std::size_t minDamage;
    std::size_t maxDamage;
    float chargeTime = 0;
    float chargeMaxTime;
    bool isCharging = false;
    float timeSinceLastShot = 0;
    float fireRate;
    std::size_t level;

    Weapon() = default;
    Weapon(const Weapon &other) = default;
    Weapon(std::size_t min_dmg, std::size_t max_dmg, float chargeMax, float rate, std::size_t lvl)
        : minDamage(min_dmg), maxDamage(max_dmg), chargeMaxTime(chargeMax), fireRate(rate), level(lvl){};

    Weapon &operator=(const Weapon &other)
    {
        if (this != &other)
        {
            minDamage = other.minDamage;
            maxDamage = other.maxDamage;
            chargeTime = other.chargeTime;
            chargeMaxTime = other.chargeMaxTime;
            isCharging = other.isCharging;
            fireRate = other.fireRate;
            level = other.level;
        }

        return *this;
    }

    std::size_t size() const { return sizeof(*this); };
};

LPL_PACKED_END
} // namespace Flakkari::Engine::ECS::Components::Common

#endif /* !WEAPON_HPP_ */
