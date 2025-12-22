/*
** EPITECH PROJECT, 2025
** Flakkari
** File description:
** ESRegistry - Wrapper for EngineSquared Core (Registry equivalent)
*/

#ifndef ES_REGISTRY_HPP_
#define ES_REGISTRY_HPP_

#include "API/IRegistry.hpp"
#include "Entity.hpp"

#include <unordered_set>

// Include EngineSquared headers
// These paths should match your EngineSquared installation
#include <core/Core.hpp>
#include <entity/Entity.hpp>

namespace Flakkari::Engine::API {

/**
 * @class ESRegistry
 * @brief Wrapper class that adapts EngineSquared's Core to the IRegistry interface.
 *
 * This class allows EngineSquared to be used through the abstract IRegistry interface,
 * enabling polymorphic usage of different ECS backends.
 *
 * EngineSquared uses `Engine::Core` as its main ECS registry, which internally uses
 * entt::registry. This wrapper provides the same interface as FlakkariRegistry.
 */
class ESRegistry : public IRegistry {
public:
    ESRegistry() = default;
    ~ESRegistry() override = default;

    // ==================== Entity Management ====================

    std::size_t spawn_entity() override
    {
        ::Engine::Entity entity = _core.CreateEntity();
        return static_cast<std::size_t>(static_cast<::Engine::Entity::entity_id_type>(entity));
    }

    std::size_t entity_from_index(std::size_t idx) override
    {
        // EngineSquared uses entity IDs directly
        return idx;
    }

    void kill_entity(std::size_t entityId) override
    {
        ::Engine::Entity entity(static_cast<::Engine::Entity::entity_id_type>(entityId));
        _core.KillEntity(entity);
    }

    // ==================== Component Management (Type-Erased) ====================

    void registerComponentType(std::type_index typeIndex) override
    {
        _registeredTypes.insert(typeIndex);
    }

    void addComponentAny(std::size_t entityId, std::type_index typeIndex, std::any component) override
    {
        // Store component in type-erased storage
        if (_componentStorage.find(typeIndex) == _componentStorage.end())
        {
            _componentStorage[typeIndex] = std::unordered_map<std::size_t, std::any>();
            _registeredTypes.insert(typeIndex);
        }

        auto &storage = std::any_cast<std::unordered_map<std::size_t, std::any> &>(_componentStorage[typeIndex]);
        storage[entityId] = std::move(component);
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
        _core.ClearEntities();
        _componentStorage.clear();
        _registeredTypes.clear();
        _systems.clear();
    }

    // ==================== Native EngineSquared Access ====================

    /**
     * @brief Get the underlying EngineSquared Core.
     * @return Reference to the EngineSquared Core.
     */
    ::Engine::Core &getNative() { return _core; }

    /**
     * @brief Get the underlying EngineSquared Core (const).
     * @return Const reference to the EngineSquared Core.
     */
    const ::Engine::Core &getNative() const { return _core; }

    /**
     * @brief Get the underlying entt registry from EngineSquared.
     * @return Reference to the entt::registry.
     */
    entt::registry &getEnttRegistry() { return _core.GetRegistry(); }

    /**
     * @brief Get the underlying entt registry from EngineSquared (const).
     * @return Const reference to the entt::registry.
     */
    const entt::registry &getEnttRegistry() const { return const_cast<::Engine::Core &>(_core).GetRegistry(); }

    // ==================== Native Component Operations ====================

    /**
     * @brief Add a component to an entity using native EngineSquared API.
     * @tparam TComponent The component type.
     * @param entityId The entity ID.
     * @param component The component to add.
     * @return Reference to the added component.
     */
    template <typename TComponent> decltype(auto) addNativeComponent(std::size_t entityId, TComponent &&component)
    {
        ::Engine::Entity entity(static_cast<::Engine::Entity::entity_id_type>(entityId));
        _registeredTypes.insert(std::type_index(typeid(TComponent)));
        return entity.AddComponent<std::decay_t<TComponent>>(_core, std::forward<TComponent>(component));
    }

    /**
     * @brief Add a component to an entity using native EngineSquared API (in-place construction).
     * @tparam TComponent The component type.
     * @tparam TArgs Constructor argument types.
     * @param entityId The entity ID.
     * @param args Constructor arguments.
     * @return Reference to the added component.
     */
    template <typename TComponent, typename... TArgs> decltype(auto) emplaceNativeComponent(std::size_t entityId, TArgs &&...args)
    {
        ::Engine::Entity entity(static_cast<::Engine::Entity::entity_id_type>(entityId));
        _registeredTypes.insert(std::type_index(typeid(TComponent)));
        return entity.AddComponent<TComponent>(_core, std::forward<TArgs>(args)...);
    }

    /**
     * @brief Remove a component from an entity using native EngineSquared API.
     * @tparam TComponent The component type.
     * @param entityId The entity ID.
     */
    template <typename TComponent> void removeNativeComponent(std::size_t entityId)
    {
        ::Engine::Entity entity(static_cast<::Engine::Entity::entity_id_type>(entityId));
        entity.RemoveComponent<TComponent>(_core);
    }

    /**
     * @brief Check if an entity has a component using native EngineSquared API.
     * @tparam TComponent The component type(s).
     * @param entityId The entity ID.
     * @return true if entity has all specified components.
     */
    template <typename... TComponent> [[nodiscard]] bool hasNativeComponent(std::size_t entityId) const
    {
        ::Engine::Entity entity(static_cast<::Engine::Entity::entity_id_type>(entityId));
        return entity.HasComponents<TComponent...>(const_cast<::Engine::Core &>(_core));
    }

    /**
     * @brief Get a component from an entity using native EngineSquared API.
     * @tparam TComponent The component type(s).
     * @param entityId The entity ID.
     * @return Reference to the component(s).
     */
    template <typename... TComponent> decltype(auto) getNativeComponent(std::size_t entityId)
    {
        ::Engine::Entity entity(static_cast<::Engine::Entity::entity_id_type>(entityId));
        return entity.GetComponents<TComponent...>(_core);
    }

    /**
     * @brief Register a system using native EngineSquared scheduler API.
     * @tparam TScheduler The scheduler type (defaults to Update).
     * @tparam Systems The system function types.
     * @param systems The system functions to register.
     */
    template <typename TScheduler = ::Engine::Scheduler::Update, typename... Systems>
    decltype(auto) registerNativeSystem(Systems... systems)
    {
        return _core.RegisterSystem<TScheduler>(systems...);
    }

    /**
     * @brief Run all native EngineSquared systems.
     */
    void runNativeSystems() { _core.RunSystems(); }

    /**
     * @brief Check if the EngineSquared core is running.
     * @return true if running.
     */
    bool isRunning() const { return const_cast<::Engine::Core &>(_core).IsRunning(); }

    /**
     * @brief Stop the EngineSquared core.
     */
    void stop() { _core.Stop(); }

    /**
     * @brief Run the EngineSquared core main loop.
     */
    void runCore() { _core.RunCore(); }

    /**
     * @brief Register a resource in EngineSquared.
     * @tparam TResource The resource type.
     * @param resource The resource to register.
     * @return Reference to the registered resource.
     */
    template <typename TResource> TResource &registerResource(TResource &&resource)
    {
        return _core.RegisterResource(std::forward<TResource>(resource));
    }

    /**
     * @brief Get a resource from EngineSquared.
     * @tparam TResource The resource type.
     * @return Reference to the resource.
     */
    template <typename TResource> TResource &getResource() { return _core.GetResource<TResource>(); }

    /**
     * @brief Get a resource from EngineSquared (const).
     * @tparam TResource The resource type.
     * @return Const reference to the resource.
     */
    template <typename TResource> const TResource &getResource() const { return _core.GetResource<TResource>(); }

private:
    ::Engine::Core _core;
    std::unordered_map<std::type_index, std::any> _componentStorage;
    std::unordered_set<std::type_index> _registeredTypes;
    std::vector<SystemFn> _systems;
};

} // namespace Flakkari::Engine::API

#endif /* !ES_REGISTRY_HPP_ */
