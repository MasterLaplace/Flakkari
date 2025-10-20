/**************************************************************************
 * Flakkari Engine Library v0.8.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file ComponentsCommon.hpp
 * @brief ComponentsCommon header. Contains all common components.
 *        (Parent, Tag, Id, Template, Child, Evolve, Health, Level,
 *         Script, Spawned, Weapon)
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.8.0
 * @date 2024-01-06
 **************************************************************************/

#ifndef FLAKKARI_COMPONENTSCOMMON_HPP_
#define FLAKKARI_COMPONENTSCOMMON_HPP_

#include "Common/Child.hpp"    // Child component (name)
#include "Common/Evolve.hpp"   // Evolve component (name)
#include "Common/Health.hpp"   // Health component (health)
#include "Common/Id.hpp"       // Id component (id)
#include "Common/Level.hpp"    // Level component (level)
#include "Common/Parent.hpp"   // Parent component (entity)
#include "Common/Spawned.hpp"  // Spawned component (spawned)
#include "Common/Tag.hpp"      // Tag component (tag)
#include "Common/Template.hpp" // Template component (name)
#include "Common/Timer.hpp"    // Timer component (time)
#include "Common/Weapon.hpp"   // Weapon component (name)

#include "Common/NetworkEvent.hpp" // NetworkEvent component (event)
#include "Common/NetworkIp.hpp"    // NetworkIp component (ip)

#endif /* !FLAKKARI_COMPONENTSCOMMON_HPP_ */
