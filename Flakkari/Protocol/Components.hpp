/**************************************************************************
 * Flakkari Library v0.2.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Components.hpp
 * @brief This file contains the Components enum class.
 *        It is used to identify the components in the Flakkari Protocol.
 *
 * @see inspired by the https://en.wikipedia.org/wiki/IPv4 header
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.2.0
 * @date 2024-01-12
 **************************************************************************/


#ifndef COMPONENTS_HPP_
    #define COMPONENTS_HPP_

#include <cstdint>

namespace Flakkari::Protocol::API {

inline namespace V_0 {

    enum class ComponentId : uint8_t {
        // 0 - 9: 2D components
        CONTROL = 0,
        MOVABLE = 1,
        TRANSFORM = 2,
        // 10 - 19: Common components
        CHILD = 10,
        PARENT = 11,
        TAG = 12,
        ID = 13,
        TEMPLATE = 14
    };

} /* namespace V_0 */

} // namespace Flakkari::Protocol::API

#endif /* !COMPONENTS_HPP_ */
