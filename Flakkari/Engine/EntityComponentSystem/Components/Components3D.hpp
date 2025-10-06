/**************************************************************************
 * Flakkari Engine Library v0.7.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Components3D.hpp
 * @brief Components3D header. Contains all 3D components.
 *        (BoxCollider, Control, Movable, RigidBody, SphereCollider, Transform)
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.7.0
 * @date 2024-11-11
 **************************************************************************/

#ifndef FLAKKARI_COMPONENTS3D_HPP_
#define FLAKKARI_COMPONENTS3D_HPP_

#include "3D/BoxCollider.hpp"    // Collider component (center, size)
#include "3D/Control.hpp"        // Control component (move_[up, down, left, right, ...], look_[*] shoot)
#include "3D/Movable.hpp"        // Movable component (velocity, acceleration, minSpeed, maxSpeed)
#include "3D/RigidBody.hpp"      // RigidBody component (mass, drag, angularDrag, useGravity, isKinematic)
#include "3D/SphereCollider.hpp" // Collider component (center, radius)
#include "3D/Transform.hpp"      // Transform component (position, rotation, scale)

#endif /* !FLAKKARI_COMPONENTS3D_HPP_ */
