/**************************************************************************
 * Flakkari Library v0.8.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Network.hpp
 * @brief This file contains the Network functions. It is used to initialize
 *        and cleanup the network. It is only used by Windows.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.8.0
 * @date 2024-10-27
 **************************************************************************/

#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include "Socket.hpp"

namespace Flakkari::Network {

/**
 * @brief Initialize the network.
 * This function is only used by Windows.
 *
 */
void init();

/**
 * @brief Cleanup the network.
 * This function is only used by Windows.
 *
 */
void cleanup();

} // namespace Flakkari::Network

#endif /* !NETWORK_HPP_ */
