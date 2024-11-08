/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2024-01-06
** File description:
** Transform
*/

#ifndef FLAKKARI_TRANSFORM_HPP_
#define FLAKKARI_TRANSFORM_HPP_

#include "../../../Math/Vector.hpp"

namespace Flakkari::Engine::ECS::Components::_2D {
LPL_PACKED_START

struct Transform {
    Math::Vector2f position;
    Math::Vector2f scale;
    float rotation;

    Transform() : position(0, 0), scale(1, 1), rotation(0){};
    Transform(const Math::Vector2f &position, const Math::Vector2f &scale, float rotation)
        : position(position), scale(scale), rotation(rotation){};
    Transform(const Transform &other) : position(other.position), scale(other.scale), rotation(other.rotation){};

    Transform &operator=(const Transform &other)
    {
        if (this != &other)
        {
            position = other.position;
            scale = other.scale;
            rotation = other.rotation;
        }

        return *this;
    }

    std::size_t size() const { return sizeof(*this); }
};

LPL_PACKED_END
} // namespace Flakkari::Engine::ECS::Components::_2D

#endif /* !FLAKKARI_TRANSFORM_HPP_ */
