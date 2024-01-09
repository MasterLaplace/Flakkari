/*
** EPITECH PROJECT, 2024
** Title: Flakkari
** Author: MasterLaplace
** Created: 2023-01-06
** File description:
** Child
*/

#ifndef CHILD_HPP_
#define CHILD_HPP_

#include <string>

namespace Flakkari::Engine::ECS::Components::Common {

/**
 * @brief Child component for ECS entities that have a child entity attached to them
 *
 * @details This component is used to create a child entity based on a prefab
 *          and attach it to the entity that has this component
 */
struct Child {
    std::string name;

    Child() : name("") {}
    Child(const std::string &name) : name(name) {}
    Child(const Child &other) : name(other.name) {}
};

} // namespace Flakkari::Engine::ECS::Components::Common

#endif /* !CHILD_HPP_ */
