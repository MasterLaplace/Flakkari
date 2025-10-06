/**************************************************************************
 * Flakkari Library v0.7.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file PacketFactory.hpp
 * @brief Flakkari::Protocol::PacketFactory class header. This class is used to
 *        create a packet from an entity. It is used to send an entity to the
 *        server or to the client.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.7.0
 * @date 2024-01-13
 **************************************************************************/

#ifndef PACKETFACTORY_HPP_
#define PACKETFACTORY_HPP_

#include "flakkari_config.h"

#include "Engine/EntityComponentSystem/Systems/Systems.hpp"
#include "Packet.hpp"

namespace Flakkari::Protocol {

class PacketFactory {
public:
    /**
     * @brief Add all the commons components of an entity to a packet.
     *
     * @tparam Id  Type of the entity id.
     * @param packet  Packet to add the components to.
     * @param registry  Registry to get the components from.
     * @param entity  Entity to get the components from.
     */
    template <typename Id>
    static void addCommonsToPacketByEntity(Protocol::Packet<Id> &packet, Engine::ECS::Registry &registry,
                                           Engine::ECS::Entity entity)
    {
        auto child = registry.getComponents<Engine::ECS::Components::Common::Child>()[entity];

        if (child.has_value())
        {
            packet << Protocol::ComponentId::CHILD;
            packet.injectString(child->name);
        }

        auto evolve = registry.getComponents<Engine::ECS::Components::Common::Evolve>()[entity];

        if (evolve.has_value())
        {
            packet << Protocol::ComponentId::EVOLVE;
            packet.injectString(evolve->name);
        }

        auto health = registry.getComponents<Engine::ECS::Components::Common::Health>()[entity];

        if (health.has_value())
        {
            packet << Protocol::ComponentId::HEALTH;
            packet << health->currentHealth;
            packet << health->maxHealth;
            packet << health->shield;
            packet << health->maxShield;
        }

        auto id = registry.getComponents<Engine::ECS::Components::Common::Id>()[entity];

        if (id.has_value())
        {
            packet << Protocol::ComponentId::ID;
            packet << id->id;
        }

        auto level = registry.getComponents<Engine::ECS::Components::Common::Level>()[entity];

        if (level.has_value())
        {
            packet << Protocol::ComponentId::LEVEL;
            packet << level->level;
            packet.injectString(level->currentWeapon);
            packet << level->currentExp;
            packet << level->requiredExp;
        }

        auto parent = registry.getComponents<Engine::ECS::Components::Common::Parent>()[entity];

        if (parent.has_value())
        {
            packet << Protocol::ComponentId::PARENT;
            packet << parent->entity;
        }

        auto tag = registry.getComponents<Engine::ECS::Components::Common::Tag>()[entity];

        if (tag.has_value())
        {
            packet << Protocol::ComponentId::TAG;
            packet.injectString(tag->tag);
        }

        auto template_ = registry.getComponents<Engine::ECS::Components::Common::Template>()[entity];

        if (template_.has_value())
        {
            packet << Protocol::ComponentId::TEMPLATE;
            packet.injectString(template_->name);
        }

        auto timer = registry.getComponents<Engine::ECS::Components::Common::Timer>()[entity];

        if (timer.has_value())
        {
            packet << Protocol::ComponentId::TIMER;
            packet << timer->lastTime.time_since_epoch().count();
            packet << timer->maxTime;
        }

        auto weapon = registry.getComponents<Engine::ECS::Components::Common::Weapon>()[entity];

        if (weapon.has_value())
        {
            packet << Protocol::ComponentId::WEAPON;
            packet << weapon->minDamage;
            packet << weapon->maxDamage;
            packet << weapon->chargeMaxTime;
            packet << weapon->fireRate;
            packet << weapon->level;
        }
    }

    /**
     * @brief Add all the 2D components of an entity to a packet.
     *
     * @tparam Id  Type of the entity id.
     * @param packet  Packet to add the components to.
     * @param registry  Registry to get the components from.
     * @param entity  Entity to get the components from.
     */
    template <typename Id>
    static void add2dToPacketByEntity(Packet<Id> &packet, Engine::ECS::Registry &registry, Engine::ECS::Entity entity)
    {
        auto transform = registry.getComponents<Engine::ECS::Components::_2D::Transform>()[entity];

        if (transform.has_value())
        {
            packet << ComponentId::TRANSFORM;
            packet << transform->_position.vec.x;
            packet << transform->_position.vec.y;
            packet << transform->_rotation;
            packet << transform->_scale.vec.x;
            packet << transform->_scale.vec.y;
        }

        auto movable = registry.getComponents<Engine::ECS::Components::_2D::Movable>()[entity];

        if (movable.has_value())
        {
            packet << ComponentId::MOVABLE;
            packet << movable->_velocity.vec.x;
            packet << movable->_velocity.vec.y;
            packet << movable->_acceleration.vec.x;
            packet << movable->_acceleration.vec.y;
        }

        auto control = registry.getComponents<Engine::ECS::Components::_2D::Control>()[entity];

        if (control.has_value())
        {
            packet << ComponentId::CONTROL;
            packet << control->_up;
            packet << control->_down;
            packet << control->_left;
            packet << control->_right;
            packet << control->_shoot;
        }

        auto collider = registry.getComponents<Engine::ECS::Components::_2D::Collider>()[entity];

        if (collider.has_value())
        {
            packet << ComponentId::COLLIDER;
            packet << collider->_size.vec.x;
            packet << collider->_size.vec.y;
        }

        auto rigidbody = registry.getComponents<Engine::ECS::Components::_2D::RigidBody>()[entity];

        if (rigidbody.has_value())
        {
            packet << ComponentId::RIGIDBODY;
            packet << rigidbody->_mass;
            packet << rigidbody->_restitution;
            packet << rigidbody->_friction;
            packet << rigidbody->_gravityScale;
            packet << rigidbody->_isKinematic;
        }
    }

    /**
     * @brief Add all the 3D components of an entity to a packet.
     *
     * @tparam Id  Type of the entity id.
     * @param packet  Packet to add the components to.
     * @param registry  Registry to get the components from.
     * @param entity  Entity to get the components from.
     */
    template <typename Id>
    static void add3dToPacketByEntity(Packet<Id> &packet, Engine::ECS::Registry &registry, Engine::ECS::Entity entity)
    {
        auto transform = registry.getComponents<Engine::ECS::Components::_3D::Transform>()[entity];

        if (transform.has_value())
        {
            packet << ComponentId::TRANSFORM_3D;
            packet << transform->_position.vec.x;
            packet << transform->_position.vec.y;
            packet << transform->_position.vec.z;
            packet << (float) transform->_rotation.vec.x;
            packet << (float) transform->_rotation.vec.y;
            packet << (float) transform->_rotation.vec.z;
            packet << (float) transform->_rotation.vec.w;
            packet << transform->_scale.vec.x;
            packet << transform->_scale.vec.y;
            packet << transform->_scale.vec.z;
        }

        auto movable = registry.getComponents<Engine::ECS::Components::_3D::Movable>()[entity];

        if (movable.has_value())
        {
            packet << ComponentId::MOVABLE_3D;
            packet << movable->_velocity.vec.x;
            packet << movable->_velocity.vec.y;
            packet << movable->_velocity.vec.z;
            packet << movable->_acceleration.vec.x;
            packet << movable->_acceleration.vec.y;
            packet << movable->_acceleration.vec.z;
            packet << movable->_minSpeed;
            packet << movable->_maxSpeed;
        }

        auto control = registry.getComponents<Engine::ECS::Components::_3D::Control>()[entity];

        if (control.has_value())
        {
            packet << ComponentId::CONTROL_3D;
            packet << control.value().toSerialized();
        }

        auto boxCollider = registry.getComponents<Engine::ECS::Components::_3D::BoxCollider>()[entity];

        if (boxCollider.has_value())
        {
            packet << ComponentId::BOXCOLLIDER;
            packet << boxCollider->_size.vec.x;
            packet << boxCollider->_size.vec.y;
            packet << boxCollider->_size.vec.z;
            packet << boxCollider->_center.vec.x;
            packet << boxCollider->_center.vec.y;
            packet << boxCollider->_center.vec.z;
        }

        auto sphereCollider = registry.getComponents<Engine::ECS::Components::_3D::SphereCollider>()[entity];

        if (sphereCollider.has_value())
        {
            packet << ComponentId::SPHERECOLLIDER;
            packet << sphereCollider->_center.vec.x;
            packet << sphereCollider->_center.vec.y;
            packet << sphereCollider->_center.vec.z;
            packet << sphereCollider->_radius;
        }

        auto rigidbody = registry.getComponents<Engine::ECS::Components::_3D::RigidBody>()[entity];

        if (rigidbody.has_value())
        {
            packet << ComponentId::RIGIDBODY_3D;
            packet << rigidbody->_mass;
            packet << rigidbody->_drag;
            packet << rigidbody->_angularDrag;
            packet << (byte) rigidbody->_useGravity;
        }
    }

    /**
     * @brief Add all the components of an entity to a packet.
     *
     * @tparam Id  Type of the entity id.
     * @param packet  Packet to add the components to.
     * @param registry  Registry to get the components from.
     * @param entity  Entity to get the components from.
     */
    template <typename Id>
    static void addComponentsToPacketByEntity(Packet<Id> &packet, Engine::ECS::Registry &registry,
                                              Engine::ECS::Entity entity)
    {
        /*_ Common Components _*/

        addCommonsToPacketByEntity<Id>(packet, registry, entity);

        /*_ 2D Components _*/

        add2dToPacketByEntity<Id>(packet, registry, entity);

        /*_ 3D Components _*/

        add3dToPacketByEntity<Id>(packet, registry, entity);
    }

    struct UpdateMovement {
        Engine::ECS::Entity entity;
        Engine::ECS::Components::_2D::Transform pos;
        Engine::ECS::Components::_2D::Movable vel;
    };

    template <typename Id>
    static void addUpdateMovementToPacket(Packet<Id> &packet, Engine::ECS::Entity entity,
                                          Engine::ECS::Components::_2D::Transform pos,
                                          Engine::ECS::Components::_2D::Movable vel)
    {
        packet << entity;
        packet << pos._position.vec.x;
        packet << pos._position.vec.y;
        packet << pos._rotation;
        packet << pos._scale.vec.x;
        packet << pos._scale.vec.y;
        packet << vel._velocity.vec.x;
        packet << vel._velocity.vec.y;
        packet << vel._acceleration.vec.x;
        packet << vel._acceleration.vec.y;
    }
};

} // namespace Flakkari::Protocol

#endif /* !PACKETFACTORY_HPP_ */
