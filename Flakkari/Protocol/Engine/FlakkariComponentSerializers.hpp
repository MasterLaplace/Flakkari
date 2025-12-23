/**************************************************************************
 * Flakkari Library v0.10.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file FlakkariComponentSerializers.hpp
 * @brief Serializers for Flakkari ECS components.
 *        Registers Flakkari components with the ComponentSerializerRegistry.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.10.0
 * @date 2024-01-13
 **************************************************************************/

#ifndef FLAKKARICOMPONENTSERIALIZERS_HPP_
#define FLAKKARICOMPONENTSERIALIZERS_HPP_

#include "ComponentRegistry.hpp"
#include "EntityComponentSystem/Components/Components.hpp"

#include <cstring>

namespace Flakkari::Protocol {

/**
 * @brief Helper to serialize POD types to bytes.
 */
template<typename T>
inline void pushBytes(std::vector<byte>& buffer, const T& value)
{
    const byte* ptr = reinterpret_cast<const byte*>(&value);
    buffer.insert(buffer.end(), ptr, ptr + sizeof(T));
}

/**
 * @brief Helper to deserialize POD types from bytes.
 */
template<typename T>
inline T popBytes(const std::vector<byte>& buffer, size_t& offset)
{
    T value;
    std::memcpy(&value, buffer.data() + offset, sizeof(T));
    offset += sizeof(T);
    return value;
}

/**
 * @brief Helper to serialize strings.
 */
inline void pushString(std::vector<byte>& buffer, const std::string& str)
{
    uint16_t len = static_cast<uint16_t>(str.size());
    pushBytes(buffer, len);
    buffer.insert(buffer.end(), str.begin(), str.end());
}

/**
 * @brief Helper to deserialize strings.
 */
inline std::string popString(const std::vector<byte>& buffer, size_t& offset)
{
    uint16_t len = popBytes<uint16_t>(buffer, offset);
    std::string str(reinterpret_cast<const char*>(buffer.data() + offset), len);
    offset += len;
    return str;
}

/**
 * @brief Registers all Flakkari ECS component serializers.
 *
 * Call this at startup to enable Flakkari component serialization.
 */
inline void registerFlakkariComponentSerializers()
{
    auto& registry = ComponentSerializerRegistry::getInstance();

    // ============ 2D Transform ============
    registry.registerSerializer<Engine::ECS::Components::_2D::Transform>(
        StandardComponents::TRANSFORM_2D,
        "Flakkari::Transform2D",
        [](const Engine::ECS::Components::_2D::Transform& t) -> std::vector<byte> {
            std::vector<byte> data;
            data.reserve(sizeof(float) * 5);
            pushBytes(data, t._position.vec.x);
            pushBytes(data, t._position.vec.y);
            pushBytes(data, t._scale.vec.x);
            pushBytes(data, t._scale.vec.y);
            pushBytes(data, t._rotation);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::_2D::Transform {
            size_t offset = 0;
            Engine::ECS::Components::_2D::Transform t;
            t._position.vec.x = popBytes<float>(data, offset);
            t._position.vec.y = popBytes<float>(data, offset);
            t._scale.vec.x = popBytes<float>(data, offset);
            t._scale.vec.y = popBytes<float>(data, offset);
            t._rotation = popBytes<float>(data, offset);
            return t;
        }
    );

    // ============ 2D Movable ============
    registry.registerSerializer<Engine::ECS::Components::_2D::Movable>(
        StandardComponents::MOVABLE_2D,
        "Flakkari::Movable2D",
        [](const Engine::ECS::Components::_2D::Movable& m) -> std::vector<byte> {
            std::vector<byte> data;
            data.reserve(sizeof(float) * 4);
            pushBytes(data, m._velocity.vec.x);
            pushBytes(data, m._velocity.vec.y);
            pushBytes(data, m._acceleration.vec.x);
            pushBytes(data, m._acceleration.vec.y);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::_2D::Movable {
            size_t offset = 0;
            Engine::ECS::Components::_2D::Movable m;
            m._velocity.vec.x = popBytes<float>(data, offset);
            m._velocity.vec.y = popBytes<float>(data, offset);
            m._acceleration.vec.x = popBytes<float>(data, offset);
            m._acceleration.vec.y = popBytes<float>(data, offset);
            return m;
        }
    );

    // ============ 2D Control ============
    registry.registerSerializer<Engine::ECS::Components::_2D::Control>(
        StandardComponents::CONTROL_2D,
        "Flakkari::Control2D",
        [](const Engine::ECS::Components::_2D::Control& c) -> std::vector<byte> {
            std::vector<byte> data;
            data.reserve(5);
            pushBytes(data, c._up);
            pushBytes(data, c._down);
            pushBytes(data, c._left);
            pushBytes(data, c._right);
            pushBytes(data, c._shoot);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::_2D::Control {
            size_t offset = 0;
            Engine::ECS::Components::_2D::Control c;
            c._up = popBytes<bool>(data, offset);
            c._down = popBytes<bool>(data, offset);
            c._left = popBytes<bool>(data, offset);
            c._right = popBytes<bool>(data, offset);
            c._shoot = popBytes<bool>(data, offset);
            return c;
        }
    );

    // ============ 2D Collider ============
    registry.registerSerializer<Engine::ECS::Components::_2D::Collider>(
        StandardComponents::COLLIDER_2D,
        "Flakkari::Collider2D",
        [](const Engine::ECS::Components::_2D::Collider& c) -> std::vector<byte> {
            std::vector<byte> data;
            pushBytes(data, c._size.vec.x);
            pushBytes(data, c._size.vec.y);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::_2D::Collider {
            size_t offset = 0;
            Engine::ECS::Components::_2D::Collider c;
            c._size.vec.x = popBytes<float>(data, offset);
            c._size.vec.y = popBytes<float>(data, offset);
            return c;
        }
    );

    // ============ 2D RigidBody ============
    registry.registerSerializer<Engine::ECS::Components::_2D::RigidBody>(
        StandardComponents::RIGIDBODY_2D,
        "Flakkari::RigidBody2D",
        [](const Engine::ECS::Components::_2D::RigidBody& rb) -> std::vector<byte> {
            std::vector<byte> data;
            pushBytes(data, rb._mass);
            pushBytes(data, rb._restitution);
            pushBytes(data, rb._friction);
            pushBytes(data, rb._gravityScale);
            pushBytes(data, rb._isKinematic);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::_2D::RigidBody {
            size_t offset = 0;
            Engine::ECS::Components::_2D::RigidBody rb;
            rb._mass = popBytes<float>(data, offset);
            rb._restitution = popBytes<float>(data, offset);
            rb._friction = popBytes<float>(data, offset);
            rb._gravityScale = popBytes<float>(data, offset);
            rb._isKinematic = popBytes<bool>(data, offset);
            return rb;
        }
    );

    // ============ 3D Transform ============
    registry.registerSerializer<Engine::ECS::Components::_3D::Transform>(
        StandardComponents::TRANSFORM_3D,
        "Flakkari::Transform3D",
        [](const Engine::ECS::Components::_3D::Transform& t) -> std::vector<byte> {
            std::vector<byte> data;
            // Position (3 floats)
            pushBytes(data, t._position.vec.x);
            pushBytes(data, t._position.vec.y);
            pushBytes(data, t._position.vec.z);
            // Rotation (quaternion as 4 floats)
            pushBytes(data, static_cast<float>(t._rotation.vec.x));
            pushBytes(data, static_cast<float>(t._rotation.vec.y));
            pushBytes(data, static_cast<float>(t._rotation.vec.z));
            pushBytes(data, static_cast<float>(t._rotation.vec.w));
            // Scale (3 floats)
            pushBytes(data, t._scale.vec.x);
            pushBytes(data, t._scale.vec.y);
            pushBytes(data, t._scale.vec.z);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::_3D::Transform {
            size_t offset = 0;
            Engine::ECS::Components::_3D::Transform t;
            t._position.vec.x = popBytes<float>(data, offset);
            t._position.vec.y = popBytes<float>(data, offset);
            t._position.vec.z = popBytes<float>(data, offset);
            t._rotation.vec.x = popBytes<float>(data, offset);
            t._rotation.vec.y = popBytes<float>(data, offset);
            t._rotation.vec.z = popBytes<float>(data, offset);
            t._rotation.vec.w = popBytes<float>(data, offset);
            t._scale.vec.x = popBytes<float>(data, offset);
            t._scale.vec.y = popBytes<float>(data, offset);
            t._scale.vec.z = popBytes<float>(data, offset);
            return t;
        }
    );

    // ============ 3D Movable ============
    registry.registerSerializer<Engine::ECS::Components::_3D::Movable>(
        StandardComponents::MOVABLE_3D,
        "Flakkari::Movable3D",
        [](const Engine::ECS::Components::_3D::Movable& m) -> std::vector<byte> {
            std::vector<byte> data;
            pushBytes(data, m._velocity.vec.x);
            pushBytes(data, m._velocity.vec.y);
            pushBytes(data, m._velocity.vec.z);
            pushBytes(data, m._acceleration.vec.x);
            pushBytes(data, m._acceleration.vec.y);
            pushBytes(data, m._acceleration.vec.z);
            pushBytes(data, m._minSpeed);
            pushBytes(data, m._maxSpeed);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::_3D::Movable {
            size_t offset = 0;
            Engine::ECS::Components::_3D::Movable m;
            m._velocity.vec.x = popBytes<float>(data, offset);
            m._velocity.vec.y = popBytes<float>(data, offset);
            m._velocity.vec.z = popBytes<float>(data, offset);
            m._acceleration.vec.x = popBytes<float>(data, offset);
            m._acceleration.vec.y = popBytes<float>(data, offset);
            m._acceleration.vec.z = popBytes<float>(data, offset);
            m._minSpeed = popBytes<float>(data, offset);
            m._maxSpeed = popBytes<float>(data, offset);
            return m;
        }
    );

    // ============ Health ============
    registry.registerSerializer<Engine::ECS::Components::Common::Health>(
        StandardComponents::HEALTH,
        "Flakkari::Health",
        [](const Engine::ECS::Components::Common::Health& h) -> std::vector<byte> {
            std::vector<byte> data;
            pushBytes(data, h.currentHealth);
            pushBytes(data, h.maxHealth);
            pushBytes(data, h.shield);
            pushBytes(data, h.maxShield);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::Common::Health {
            size_t offset = 0;
            Engine::ECS::Components::Common::Health h;
            h.currentHealth = popBytes<std::size_t>(data, offset);
            h.maxHealth = popBytes<std::size_t>(data, offset);
            h.shield = popBytes<std::size_t>(data, offset);
            h.maxShield = popBytes<std::size_t>(data, offset);
            return h;
        }
    );

    // ============ Tag ============
    registry.registerSerializer<Engine::ECS::Components::Common::Tag>(
        StandardComponents::TAG,
        "Flakkari::Tag",
        [](const Engine::ECS::Components::Common::Tag& t) -> std::vector<byte> {
            std::vector<byte> data;
            pushString(data, t.tag);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::Common::Tag {
            size_t offset = 0;
            Engine::ECS::Components::Common::Tag t;
            t.tag = popString(data, offset);
            return t;
        }
    );

    // ============ Child ============
    registry.registerSerializer<Engine::ECS::Components::Common::Child>(
        StandardComponents::CHILD,
        "Flakkari::Child",
        [](const Engine::ECS::Components::Common::Child& c) -> std::vector<byte> {
            std::vector<byte> data;
            pushString(data, c.name);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::Common::Child {
            size_t offset = 0;
            Engine::ECS::Components::Common::Child c;
            c.name = popString(data, offset);
            return c;
        }
    );

    // ============ Parent ============
    registry.registerSerializer<Engine::ECS::Components::Common::Parent>(
        StandardComponents::PARENT,
        "Flakkari::Parent",
        [](const Engine::ECS::Components::Common::Parent& p) -> std::vector<byte> {
            std::vector<byte> data;
            pushBytes(data, p.entity);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::Common::Parent {
            size_t offset = 0;
            Engine::ECS::Components::Common::Parent p;
            p.entity = popBytes<std::size_t>(data, offset);
            return p;
        }
    );

    // ============ Id ============
    registry.registerSerializer<Engine::ECS::Components::Common::Id>(
        StandardComponents::ID,
        "Flakkari::Id",
        [](const Engine::ECS::Components::Common::Id& id) -> std::vector<byte> {
            std::vector<byte> data;
            pushBytes(data, id.id);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::Common::Id {
            size_t offset = 0;
            Engine::ECS::Components::Common::Id id;
            id.id = popBytes<std::size_t>(data, offset);
            return id;
        }
    );

    // ============ Level ============
    registry.registerSerializer<Engine::ECS::Components::Common::Level>(
        StandardComponents::LEVEL,
        "Flakkari::Level",
        [](const Engine::ECS::Components::Common::Level& l) -> std::vector<byte> {
            std::vector<byte> data;
            pushBytes(data, l.level);
            pushString(data, l.currentWeapon);
            pushBytes(data, l.currentExp);
            pushBytes(data, l.requiredExp);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::Common::Level {
            size_t offset = 0;
            Engine::ECS::Components::Common::Level l;
            l.level = popBytes<std::size_t>(data, offset);
            l.currentWeapon = popString(data, offset);
            l.currentExp = popBytes<std::size_t>(data, offset);
            l.requiredExp = popBytes<std::size_t>(data, offset);
            return l;
        }
    );

    // ============ Weapon ============
    registry.registerSerializer<Engine::ECS::Components::Common::Weapon>(
        StandardComponents::WEAPON,
        "Flakkari::Weapon",
        [](const Engine::ECS::Components::Common::Weapon& w) -> std::vector<byte> {
            std::vector<byte> data;
            pushBytes(data, w.minDamage);
            pushBytes(data, w.maxDamage);
            pushBytes(data, w.chargeMaxTime);
            pushBytes(data, w.fireRate);
            pushBytes(data, w.level);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::Common::Weapon {
            size_t offset = 0;
            Engine::ECS::Components::Common::Weapon w;
            w.minDamage = popBytes<std::size_t>(data, offset);
            w.maxDamage = popBytes<std::size_t>(data, offset);
            w.chargeMaxTime = popBytes<float>(data, offset);
            w.fireRate = popBytes<float>(data, offset);
            w.level = popBytes<std::size_t>(data, offset);
            return w;
        }
    );

    // ============ Evolve ============
    registry.registerSerializer<Engine::ECS::Components::Common::Evolve>(
        StandardComponents::EVOLVE,
        "Flakkari::Evolve",
        [](const Engine::ECS::Components::Common::Evolve& e) -> std::vector<byte> {
            std::vector<byte> data;
            pushString(data, e.name);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::Common::Evolve {
            size_t offset = 0;
            Engine::ECS::Components::Common::Evolve e;
            e.name = popString(data, offset);
            return e;
        }
    );

    // ============ Template ============
    registry.registerSerializer<Engine::ECS::Components::Common::Template>(
        StandardComponents::TEMPLATE,
        "Flakkari::Template",
        [](const Engine::ECS::Components::Common::Template& t) -> std::vector<byte> {
            std::vector<byte> data;
            pushString(data, t.name);
            return data;
        },
        [](const std::vector<byte>& data) -> Engine::ECS::Components::Common::Template {
            size_t offset = 0;
            Engine::ECS::Components::Common::Template t;
            t.name = popString(data, offset);
            return t;
        }
    );
}

} // namespace Flakkari::Protocol

#endif /* !FLAKKARICOMPONENTSERIALIZERS_HPP_ */
