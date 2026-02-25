/**************************************************************************
 * Flakkari Library v0.10.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file IRegistry.hpp
 * @brief IRegistry interface for ECS (Entity Component System) - Abstract Interface.
 *
 * This interface allows Flakkari to support multiple ECS engines transparently.
 * Since template methods cannot be virtual, this interface uses type-erased
 * component operations via std::any and std::type_index.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.10.0
 * @date 2024-01-05
 **************************************************************************/

#ifndef FLAKKARI_IREGISTRY_HPP_
#define FLAKKARI_IREGISTRY_HPP_

#include "IEntity.hpp"

#include <any>
#include <climits>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Flakkari::Engine::API {

/**
 * @class IRegistry
 * @brief Abstract interface for managing entities, components, and systems in an ECS architecture.
 *
 * This interface provides methods to create, manage, and destroy entities and components,
 * as well as to add and run systems. It serves as an abstraction layer allowing Flakkari
 * to work with different ECS backends (native ECS, EngineSquared, etc.).
 *
 * @note Since C++ doesn't allow virtual template methods, component operations use
 *       type-erased interfaces via std::any and std::type_index.
 */
class IRegistry {
public:
    using SystemFn = std::function<void(IRegistry &)>;

    virtual ~IRegistry() = default;

    // ==================== Entity Management ====================

    /**
     * @brief Spawn a new entity in the registry.
     * @return The ID of the created entity.
     */
    virtual std::size_t spawn_entity() = 0;

    /**
     * @brief Get an entity from its index.
     * @param idx The index of the entity.
     * @return The entity ID.
     */
    virtual std::size_t entity_from_index(std::size_t idx) = 0;

    /**
     * @brief Kill an entity and remove all its components.
     * @param entityId The ID of the entity to kill.
     */
    virtual void kill_entity(std::size_t entityId) = 0;

    // ==================== Component Management (Type-Erased) ====================

    /**
     * @brief Register a component type in the registry.
     * @param typeIndex The type index of the component.
     */
    virtual void registerComponentType(std::type_index typeIndex) = 0;

    /**
     * @brief Add a component to an entity (type-erased version).
     * @param entityId The entity ID.
     * @param typeIndex The type index of the component.
     * @param component The component data wrapped in std::any.
     */
    virtual void addComponentAny(std::size_t entityId, std::type_index typeIndex, std::any component) = 0;

    /**
     * @brief Remove a component from an entity.
     * @param entityId The entity ID.
     * @param typeIndex The type index of the component to remove.
     */
    virtual void removeComponentAny(std::size_t entityId, std::type_index typeIndex) = 0;

    /**
     * @brief Check if an entity has a specific component.
     * @param entityId The entity ID.
     * @param typeIndex The type index of the component.
     * @return true if the entity has the component.
     */
    virtual bool hasComponent(std::size_t entityId, std::type_index typeIndex) const = 0;

    /**
     * @brief Check if a component type is registered.
     * @param typeIndex The type index of the component.
     * @return true if the component type is registered.
     */
    virtual bool isComponentTypeRegistered(std::type_index typeIndex) const = 0;

    /**
     * @brief Get a component from an entity (type-erased version).
     * @param entityId The entity ID.
     * @param typeIndex The type index of the component.
     * @return The component data wrapped in std::any, or empty if not found.
     */
    virtual std::any getComponentAny(std::size_t entityId, std::type_index typeIndex) = 0;

    /**
     * @brief Get all components of a specific type (type-erased version).
     * @param typeIndex The type index of the component.
     * @return Reference to the internal component storage as std::any.
     */
    virtual std::any &getComponentsAny(std::type_index typeIndex) = 0;

    /**
     * @brief Get the number of entities with a specific component type.
     * @param typeIndex The type index of the component.
     * @return The count of components.
     */
    virtual std::size_t getComponentCount(std::type_index typeIndex) const = 0;

    // ==================== System Management ====================

    /**
     * @brief Add a system to the registry.
     * @param system The system function to add.
     */
    virtual void add_system(SystemFn system) = 0;

    /**
     * @brief Run all registered systems.
     */
    virtual void run_systems() = 0;

    // ==================== Utility ====================

    /**
     * @brief Clear all entities, components, and systems from the registry.
     */
    virtual void clear() = 0;

    // ==================== Typed Helper Methods (Non-Virtual Templates) ====================

    /**
     * @brief Register a component type (typed version).
     * @tparam Component The component type to register.
     */
    template <typename Component> void registerComponent()
    {
        registerComponentType(std::type_index(typeid(Component)));
    }

    /**
     * @brief Add a component to an entity (typed version).
     * @tparam Component The component type.
     * @param entityId The entity ID.
     * @param component The component to add.
     */
    template <typename Component> void add_component(std::size_t entityId, Component &&component)
    {
        addComponentAny(entityId, std::type_index(typeid(Component)),
                        std::make_any<std::decay_t<Component>>(std::forward<Component>(component)));
    }

    /**
     * @brief Emplace a component to an entity (typed version).
     * @tparam Component The component type.
     * @tparam Args Constructor argument types.
     * @param entityId The entity ID.
     * @param args Constructor arguments.
     */
    template <typename Component, typename... Args> void emplace_component(std::size_t entityId, Args &&...args)
    {
        addComponentAny(entityId, std::type_index(typeid(Component)),
                        std::make_any<Component>(std::forward<Args>(args)...));
    }

    /**
     * @brief Remove a component from an entity (typed version).
     * @tparam Component The component type.
     * @param entityId The entity ID.
     */
    template <typename Component> void remove_component(std::size_t entityId)
    {
        removeComponentAny(entityId, std::type_index(typeid(Component)));
    }

    /**
     * @brief Check if a component type is registered (typed version).
     * @tparam Component The component type.
     * @return true if registered.
     */
    template <typename Component> [[nodiscard]] bool isRegistered() const
    {
        return isComponentTypeRegistered(std::type_index(typeid(Component)));
    }

    /**
     * @brief Check if an entity has a specific component (typed version).
     * @tparam Component The component type.
     * @param entityId The entity ID.
     * @return true if the entity has the component.
     */
    template <typename Component> [[nodiscard]] bool isRegistered(std::size_t entityId) const
    {
        return hasComponent(entityId, std::type_index(typeid(Component)));
    }

    /**
     * @brief Get a component from an entity (typed version).
     * @tparam Component The component type.
     * @param entityId The entity ID.
     * @return Optional containing the component if found.
     */
    template <typename Component> [[nodiscard]] std::optional<Component> getComponent(std::size_t entityId)
    {
        std::any comp = getComponentAny(entityId, std::type_index(typeid(Component)));
        if (comp.has_value())
        {
            try
            {
                return std::any_cast<Component>(comp);
            }
            catch (const std::bad_any_cast &)
            {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }

    /**
     * @brief Get a component reference from an entity (typed version).
     * @tparam Component The component type.
     * @param entityId The entity ID.
     * @return Pointer to the component if found, nullptr otherwise.
     */
    template <typename Component> [[nodiscard]] Component *getComponentPtr(std::size_t entityId)
    {
        std::any comp = getComponentAny(entityId, std::type_index(typeid(Component)));
        if (comp.has_value())
        {
            try
            {
                return std::any_cast<Component>(&comp);
            }
            catch (const std::bad_any_cast &)
            {
                return nullptr;
            }
        }
        return nullptr;
    }

    /**
     * @brief Add a typed system to the registry.
     * @tparam Components The component types used by the system.
     * @tparam Function The system function type.
     * @param f The system function.
     */
    template <typename... Components, typename Function> void add_system(Function &&f)
    {
        add_system(SystemFn([sys = std::forward<Function>(f)](IRegistry &r) mutable { sys(r); }));
    }
};

} // namespace Flakkari::Engine::API

#endif /* !FLAKKARI_IREGISTRY_HPP_ */
