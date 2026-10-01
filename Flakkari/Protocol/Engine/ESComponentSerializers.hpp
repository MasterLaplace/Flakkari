/**************************************************************************
 * Flakkari Library v0.10.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file ESComponentSerializers.hpp
 * @brief Serializers for EngineSquared components.
 *        Registers EngineSquared components with the ComponentSerializerRegistry.
 *
 * This file is only compiled when FLAKKARI_HAS_ENGINESQUARED is defined.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.10.0
 * @date 2024-01-13
 **************************************************************************/

#ifndef ESCOMPONENTSERIALIZERS_HPP_
#define ESCOMPONENTSERIALIZERS_HPP_

#include "flakkari_config.h"

#ifdef FLAKKARI_HAS_ENGINESQUARED

#    include "ComponentRegistry.hpp"
#    include "FlakkariComponentSerializers.hpp" // For pushBytes/popBytes helpers

// EngineSquared includes
#    include <JoltPhysics.hpp> // ES::Plugin::Physics::Component::RigidBody3D
#    include <Object.hpp>      // ES::Plugin::Object::Component::Transform

#    include <cstring>

namespace Flakkari::Protocol {

/**
 * @brief Registers all EngineSquared component serializers.
 *
 * Call this at startup to enable EngineSquared component serialization.
 * This maps ES components to the same StandardComponents hashes as Flakkari,
 * enabling cross-engine compatibility.
 */
inline void registerEngineSquaredComponentSerializers()
{
    auto &registry = ComponentSerializerRegistry::getInstance();

    // ============ Transform (3D - EngineSquared only has 3D) ============
    // Note: ES::Plugin::Object::Component::Transform uses glm types
    registry.registerSerializer<ES::Plugin::Object::Component::Transform>(
        StandardComponents::TRANSFORM_3D, "EngineSquared::Transform",
        [](const ES::Plugin::Object::Component::Transform &t) -> std::vector<byte> {
            std::vector<byte> data;
            // Position (3 floats)
            pushBytes(data, t.position.x);
            pushBytes(data, t.position.y);
            pushBytes(data, t.position.z);
            // Rotation (quaternion as 4 floats: x, y, z, w)
            pushBytes(data, t.rotation.x);
            pushBytes(data, t.rotation.y);
            pushBytes(data, t.rotation.z);
            pushBytes(data, t.rotation.w);
            // Scale (3 floats)
            pushBytes(data, t.scale.x);
            pushBytes(data, t.scale.y);
            pushBytes(data, t.scale.z);
            return data;
        },
        [](const std::vector<byte> &data) -> ES::Plugin::Object::Component::Transform {
            size_t offset = 0;
            glm::vec3 pos, scale;
            glm::quat rot;

            pos.x = popBytes<float>(data, offset);
            pos.y = popBytes<float>(data, offset);
            pos.z = popBytes<float>(data, offset);

            rot.x = popBytes<float>(data, offset);
            rot.y = popBytes<float>(data, offset);
            rot.z = popBytes<float>(data, offset);
            rot.w = popBytes<float>(data, offset);

            scale.x = popBytes<float>(data, offset);
            scale.y = popBytes<float>(data, offset);
            scale.z = popBytes<float>(data, offset);

            return ES::Plugin::Object::Component::Transform(pos, scale, rot);
        });

    // Note: EngineSquared uses Jolt Physics which has very different component
    // structures than Flakkari's simple RigidBody. For network synchronization,
    // we only serialize the essential transform/velocity data, not the full
    // physics state (which is managed by Jolt on each instance).
    //
    // Additional ES-specific components can be registered here as needed.
    // The key insight is that network protocol doesn't need to sync full
    // physics engine state - just the resulting transforms and velocities.
}

} // namespace Flakkari::Protocol

#endif // FLAKKARI_HAS_ENGINESQUARED

#endif /* !ESCOMPONENTSERIALIZERS_HPP_ */
