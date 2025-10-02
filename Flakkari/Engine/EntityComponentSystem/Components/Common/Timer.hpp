/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2024-11-17
** File description:
** Timer
*/

#ifndef Timer_HPP_
#define Timer_HPP_

#include <chrono>
#include <cstring>
#include <string>

#include "flakkari_config.h"

namespace Flakkari::Engine::ECS::Components::Common {
LPL_PACKED_START

/**
 * @brief  Timer component for ECS entities that have a timer attached to them
 *
 * @details This component is used to store the current time and the maximum time
 */
struct Timer {
    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    float maxTime;

    Timer() : maxTime(0) {}
    Timer(bool spawed) : maxTime(spawed) {}
    Timer(const Timer &other) : maxTime(other.maxTime) {}

    Timer &operator=(const Timer &other)
    {
        if (this != &other)
        {
            lastTime = other.lastTime;
            maxTime = other.maxTime;
        }

        return *this;
    }

    std::size_t size() const { return sizeof(*this); }
};

LPL_PACKED_END
} // namespace Flakkari::Engine::ECS::Components::Common

#endif /* !Timer_HPP_ */
