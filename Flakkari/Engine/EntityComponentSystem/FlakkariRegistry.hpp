/*
** EPITECH PROJECT, 2025
** Flakkari
** File description:
** FlakkariRegistry - Wrapper for native Flakkari ECS Registry
*/

#ifndef FLAKKARI_FLAKKARI_REGISTRY_HPP_
#define FLAKKARI_FLAKKARI_REGISTRY_HPP_

#include "API/IRegistry.hpp"
#include "Registry.hpp"
#include "SparseArrays.hpp"
#include "FlakkariEntity.hpp"

#include <unordered_set>

namespace Flakkari::Engine::API {

/**
 * @class FlakkariRegistry
 * @brief Wrapper class that adapts the native Flakkari ECS Registry to the IRegistry interface.
 *
 * This class allows the native Flakkari ECS to be used through the abstract IRegistry interface,
 * enabling polymorphic usage of different ECS backends.
 */
class FlakkariRegistry : public IRegistry {
public:
    FlakkariRegistry() = default;
    ~FlakkariRegistry() override = default;

    // ==================== Entity Management ====================

    std::size_t spawn_entity() override { return static_cast<std::size_t>(_registry.spawn_entity()); }

    std::size_t entity_from_index(std::size_t idx) override
    {
        return static_cast<std::size_t>(_registry.entity_from_index(idx));
    }

    void kill_entity(std::size_t entityId) override { _registry.kill_entity(ECS::Entity(entityId)); }

    // ==================== Component Management (Type-Erased) ====================

    void registerComponentType(std::type_index typeIndex) override
    {
        // Store the type index for later use
        // The actual registration happens when adding components
        _registeredTypes.insert(typeIndex);
    }

    void addComponentAny(std::size_t entityId, std::type_index typeIndex, std::any component) override
    {
        // Store component in our type-erased storage
        if (_componentStorage.find(typeIndex) == _componentStorage.end())
        {
            _componentStorage[typeIndex] = std::unordered_map<std::size_t, std::any>();
            _registeredTypes.insert(typeIndex);
        }

        auto &storage = std::any_cast<std::unordered_map<std::size_t, std::any> &>(_componentStorage[typeIndex]);
        storage[entityId] = std::move(component);

        // Also register erase function for cleanup
        if (_eraseFunctions.find(typeIndex) == _eraseFunctions.end())
        {
            _eraseFunctions[typeIndex] = [](FlakkariRegistry &r, std::size_t e, std::type_index ti) {
                r.removeComponentAny(e, ti);
            };
        }
    }

    void removeComponentAny(std::size_t entityId, std::type_index typeIndex) override
    {
        auto it = _componentStorage.find(typeIndex);
        if (it != _componentStorage.end())
        {
            auto &storage = std::any_cast<std::unordered_map<std::size_t, std::any> &>(it->second);
            storage.erase(entityId);
        }
    }

    bool hasComponent(std::size_t entityId, std::type_index typeIndex) const override
    {
        auto it = _componentStorage.find(typeIndex);
        if (it != _componentStorage.end())
        {
            const auto &storage =
                std::any_cast<const std::unordered_map<std::size_t, std::any> &>(it->second);
            return storage.find(entityId) != storage.end();
        }
        return false;
    }

    bool isComponentTypeRegistered(std::type_index typeIndex) const override
    {
        return _registeredTypes.find(typeIndex) != _registeredTypes.end();
    }

    std::any getComponentAny(std::size_t entityId, std::type_index typeIndex) override
    {
        auto it = _componentStorage.find(typeIndex);
        if (it != _componentStorage.end())
        {
            auto &storage = std::any_cast<std::unordered_map<std::size_t, std::any> &>(it->second);
            auto compIt = storage.find(entityId);
            if (compIt != storage.end())
            {
                return compIt->second;
            }
        }
        return std::any{};
    }

    std::any &getComponentsAny(std::type_index typeIndex) override
    {
        if (_componentStorage.find(typeIndex) == _componentStorage.end())
        {
            _componentStorage[typeIndex] = std::unordered_map<std::size_t, std::any>();
            _registeredTypes.insert(typeIndex);
        }
        return _componentStorage[typeIndex];
    }

    std::size_t getComponentCount(std::type_index typeIndex) const override
    {
        auto it = _componentStorage.find(typeIndex);
        if (it != _componentStorage.end())
        {
            const auto &storage =
                std::any_cast<const std::unordered_map<std::size_t, std::any> &>(it->second);
            return storage.size();
        }
        return 0;
    }

    // ==================== System Management ====================

    void add_system(SystemFn system) override { _systems.push_back(std::move(system)); }

    void run_systems() override
    {
        for (auto &system : _systems)
        {
            system(*this);
        }
    }

    // ==================== Utility ====================

    void clear() override
    {
        _registry.clear();
        _componentStorage.clear();
        _registeredTypes.clear();
        _eraseFunctions.clear();
        _systems.clear();
    }

    // ==================== Native Access ====================

    /**
     * @brief Get the underlying native ECS Registry.
     * @return Reference to the native ECS::Registry.
     */
    ECS::Registry &getNative() { return _registry; }

    /**
     * @brief Get the underlying native ECS Registry (const).
     * @return Const reference to the native ECS::Registry.
     */
    const ECS::Registry &getNative() const { return _registry; }

    // ==================== Typed Component Access (For Native Usage) ====================

    /**
     * @brief Get components using native SparseArrays (typed version for internal use).
     * @tparam Component The component type.
     * @return Reference to the SparseArrays containing the components.
     */
    template <typename Component> ECS::SparseArrays<Component> &getComponents()
    {
        return _registry.getComponents<Component>();
    }

    /**
     * @brief Get components using native SparseArrays (typed const version).
     * @tparam Component The component type.
     * @return Const reference to the SparseArrays containing the components.
     */
    template <typename Component> const ECS::SparseArrays<Component> &getComponents() const
    {
        return _registry.getComponents<Component>();
    }

    /**
     * @brief Add a component using the native registry.
     * @tparam Component The component type.
     * @param entityId The entity ID.
     * @param component The component to add.
     * @return Reference to the added component.
     */
    template <typename Component>
    typename ECS::SparseArrays<Component>::reference_type addNativeComponent(std::size_t entityId,
                                                                              Component &&component)
    {
        return _registry.add_component<Component>(ECS::Entity(entityId), std::forward<Component>(component));
    }

    /**
     * @brief Register a component type in the native registry.
     * @tparam Component The component type.
     * @return Reference to the SparseArrays for this component type.
     */
    template <typename Component> ECS::SparseArrays<Component> &registerNativeComponent()
    {
        _registeredTypes.insert(std::type_index(typeid(Component)));
        return _registry.registerComponent<Component>();
    }

    /**
     * @brief Check if an entity has a component in the native registry.
     * @tparam Component The component type.
     * @param entityId The entity ID.
     * @return true if the entity has the component.
     */
    template <typename Component> [[nodiscard]] bool isNativeRegistered(std::size_t entityId) const
    {
        return _registry.isRegistered<Component>(ECS::Entity(entityId));
    }

    /**
     * @brief Check if a component type is registered in the native registry.
     * @tparam Component The component type.
     * @return true if registered.
     */
    template <typename Component> [[nodiscard]] bool isNativeRegistered() const
    {
        return _registry.isRegistered<Component>();
    }

private:
    ECS::Registry _registry;
    std::unordered_map<std::type_index, std::any> _componentStorage;
    std::unordered_set<std::type_index> _registeredTypes;
    std::unordered_map<std::type_index, std::function<void(FlakkariRegistry &, std::size_t, std::type_index)>>
        _eraseFunctions;
    std::vector<SystemFn> _systems;
};

} // namespace Flakkari::Engine::API

#endif /* !FLAKKARI_FLAKKARI_REGISTRY_HPP_ */
