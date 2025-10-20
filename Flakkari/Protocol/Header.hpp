/**************************************************************************
 * Flakkari Library v0.8.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Header.hpp
 * @brief This file contains the Header struct. It is used to create a header
 *        for the Flakkari Protocol.
 *
 * @see inspired by the https://en.wikipedia.org/wiki/IPv4 header
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.8.0
 * @date 2023-12-24
 **************************************************************************/

#ifndef HEADER_HPP_
#define HEADER_HPP_

#define PROTOCOL_VERSION 1

#include "flakkari_config.h"

#include "../Network/Buffer.hpp"
#include "Commands.hpp"
#include <chrono>

namespace Flakkari::Protocol {

using ushort = uint16_t; // 16 bits (max: 65535) (2 bytes)
using uint = uint32_t;   // 32 bits (max: 4294967295) (4 bytes)
using ulong = uint64_t;  // 64 bits (max: 18446744073709551615) (8 bytes)

/**
 * @brief The version of the protocol used
 *
 */
enum class ApiVersion : byte {
    V_0 = 0,
    V_1 = 1,
    MAX_VERSION
};

/**
 * @brief The priority of the message in the queue
 *
 * @note The priority is used to determine the order of the messages in the queue.
 *      The higher the priority, the faster the message will be processed.
 */
enum class Priority : byte {
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2,
    CRITICAL = 3,
    MAX_PRIORITY = 4
};

namespace V_0 {

LPL_PACKED_START

template <typename Id> struct Header {
    Priority _priority     : 4 = Priority::LOW;
    ApiVersion _apiVersion : 4 = ApiVersion::V_0;
    Id _commandId;
    ushort _contentLength = 0;
    uint _sequenceNumber = static_cast<uint>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count());
};

LPL_PACKED_END

} // namespace V_0

inline namespace V_1 {

LPL_PACKED_START

/**
 * @brief Flakkari Header v1 (new header)
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Priority| Api V.|   CommandId   |       ContentLength          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                        SequenceNumber                         |
 * |                                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
template <typename Id> struct Header {
    Priority _priority     : 4 = Priority::LOW;
    ApiVersion _apiVersion : 4 = ApiVersion::V_1;
    Id _commandId;
    uint16_t _contentLength = 0;
    uint64_t _sequenceNumber = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count());

    std::size_t size() const { return sizeof(*this); }
};

LPL_PACKED_END

} // namespace V_1

} // namespace Flakkari::Protocol

#endif /* !HEADER_HPP_ */
