/**************************************************************************
 * Flakkari Library v0.10.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file ComponentRegistry.hpp
 * @brief Engine-agnostic component registry for Protocol V2.
 *        Maps component names to serializers/deserializers for any ECS engine.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.10.0
 * @date 2024-01-13
 **************************************************************************/

#ifndef COMPONENTREGISTRY_HPP_
#define COMPONENTREGISTRY_HPP_

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <any>
#include <optional>
#include <vector>

#include "Network/Buffer.hpp"

namespace Flakkari::Protocol {

/**
 * @brief Compile-time FNV-1a hash for component names.
 *
 * This allows using string names in switch statements and as enum-like values.
 */
constexpr uint32_t fnv1a_hash(std::string_view str) noexcept
{
    uint32_t hash = 2166136261u;
    for (char c : str)
    {
        hash ^= static_cast<uint32_t>(c);
        hash *= 16777619u;
    }
    return hash;
}

/**
 * @brief Runtime version of FNV-1a hash.
 */
inline uint32_t fnv1a_hash_runtime(const std::string& str) noexcept
{
    return fnv1a_hash(std::string_view(str));
}

/**
 * @brief Component identifier for Protocol V2.
 *
 * Uses a 32-bit hash of the component name for identification.
 * This allows any engine to define its own components while maintaining
 * protocol compatibility.
 */
using ComponentHash = uint32_t;

/**
 * @brief Well-known component hashes for cross-engine compatibility.
 *
 * These are the "standard" components that different engines should map
 * their equivalents to for interoperability.
 */
namespace StandardComponents {

// Transform components
constexpr ComponentHash TRANSFORM_2D = fnv1a_hash("Transform2D");
constexpr ComponentHash TRANSFORM_3D = fnv1a_hash("Transform3D");

// Movement components
constexpr ComponentHash MOVABLE_2D = fnv1a_hash("Movable2D");
constexpr ComponentHash MOVABLE_3D = fnv1a_hash("Movable3D");

// Control components
constexpr ComponentHash CONTROL_2D = fnv1a_hash("Control2D");
constexpr ComponentHash CONTROL_3D = fnv1a_hash("Control3D");

// Physics components
constexpr ComponentHash RIGIDBODY_2D = fnv1a_hash("RigidBody2D");
constexpr ComponentHash RIGIDBODY_3D = fnv1a_hash("RigidBody3D");
constexpr ComponentHash COLLIDER_2D = fnv1a_hash("Collider2D");
constexpr ComponentHash BOX_COLLIDER_3D = fnv1a_hash("BoxCollider3D");
constexpr ComponentHash SPHERE_COLLIDER_3D = fnv1a_hash("SphereCollider3D");

// Common components
constexpr ComponentHash CHILD = fnv1a_hash("Child");
constexpr ComponentHash PARENT = fnv1a_hash("Parent");
constexpr ComponentHash TAG = fnv1a_hash("Tag");
constexpr ComponentHash HEALTH = fnv1a_hash("Health");
constexpr ComponentHash ID = fnv1a_hash("Id");
constexpr ComponentHash LEVEL = fnv1a_hash("Level");
constexpr ComponentHash TIMER = fnv1a_hash("Timer");
constexpr ComponentHash WEAPON = fnv1a_hash("Weapon");
constexpr ComponentHash TEMPLATE = fnv1a_hash("Template");
constexpr ComponentHash EVOLVE = fnv1a_hash("Evolve");

} // namespace StandardComponents

/**
 * @brief Serialization data for a component.
 */
struct ComponentData {
    ComponentHash hash;
    std::vector<byte> data;
};

/**
 * @brief Interface for component serializers.
 *
 * Each engine implements serializers for its component types.
 */
class IComponentSerializer {
public:
    virtual ~IComponentSerializer() = default;

    /**
     * @brief Get the component hash this serializer handles.
     */
    virtual ComponentHash getHash() const = 0;

    /**
     * @brief Get the component name for debugging.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Serialize a component to binary data.
     * @param component The component as std::any.
     * @return Serialized binary data.
     */
    virtual std::vector<byte> serialize(const std::any& component) const = 0;

    /**
     * @brief Deserialize binary data to a component.
     * @param data The binary data.
     * @return The component as std::any.
     */
    virtual std::any deserialize(const std::vector<byte>& data) const = 0;
};

/**
 * @brief Template helper to create component serializers.
 *
 * @tparam T The component type.
 */
template<typename T>
class ComponentSerializer : public IComponentSerializer {
public:
    using SerializeFunc = std::function<std::vector<byte>(const T&)>;
    using DeserializeFunc = std::function<T(const std::vector<byte>&)>;

    ComponentSerializer(ComponentHash hash, std::string name,
                       SerializeFunc serializer, DeserializeFunc deserializer)
        : _hash(hash), _name(std::move(name)),
          _serializer(std::move(serializer)), _deserializer(std::move(deserializer))
    {}

    ComponentHash getHash() const override { return _hash; }
    std::string getName() const override { return _name; }

    std::vector<byte> serialize(const std::any& component) const override
    {
        return _serializer(std::any_cast<const T&>(component));
    }

    std::any deserialize(const std::vector<byte>& data) const override
    {
        return _deserializer(data);
    }

private:
    ComponentHash _hash;
    std::string _name;
    SerializeFunc _serializer;
    DeserializeFunc _deserializer;
};

/**
 * @brief Registry for component serializers.
 *
 * Each engine registers its component serializers here.
 * This allows the protocol to serialize/deserialize components
 * without knowing the specific engine types.
 */
class ComponentSerializerRegistry {
public:
    static ComponentSerializerRegistry& getInstance()
    {
        static ComponentSerializerRegistry instance;
        return instance;
    }

    /**
     * @brief Register a component serializer.
     */
    void registerSerializer(std::unique_ptr<IComponentSerializer> serializer)
    {
        _serializers[serializer->getHash()] = std::move(serializer);
    }

    /**
     * @brief Register a component serializer using template helper.
     */
    template<typename T>
    void registerSerializer(ComponentHash hash, const std::string& name,
                           typename ComponentSerializer<T>::SerializeFunc serializer,
                           typename ComponentSerializer<T>::DeserializeFunc deserializer)
    {
        auto s = std::make_unique<ComponentSerializer<T>>(hash, name, serializer, deserializer);
        _serializers[hash] = std::move(s);
    }

    /**
     * @brief Get a serializer for a component hash.
     */
    IComponentSerializer* getSerializer(ComponentHash hash) const
    {
        auto it = _serializers.find(hash);
        return it != _serializers.end() ? it->second.get() : nullptr;
    }

    /**
     * @brief Check if a serializer exists for a component hash.
     */
    bool hasSerializer(ComponentHash hash) const
    {
        return _serializers.find(hash) != _serializers.end();
    }

    /**
     * @brief Get all registered component hashes.
     */
    std::vector<ComponentHash> getRegisteredHashes() const
    {
        std::vector<ComponentHash> hashes;
        hashes.reserve(_serializers.size());
        for (const auto& [hash, _] : _serializers)
            hashes.push_back(hash);
        return hashes;
    }

private:
    ComponentSerializerRegistry() = default;
    std::unordered_map<ComponentHash, std::unique_ptr<IComponentSerializer>> _serializers;
};

} // namespace Flakkari::Protocol

#endif /* !COMPONENTREGISTRY_HPP_ */
