/**************************************************************************
 * Flakkari Library v0.5.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Events.hpp
 * @brief This file contains the EventId enum class. It is used to identify
 *        the events in the Flakkari Protocol. It is used to send an event
 *        from the client to the server.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.5.0
 * @date 2024-01-14
 **************************************************************************/

#ifndef EVENTS_HPP_
#define EVENTS_HPP_

#include "flakkari_config.h"

namespace Flakkari::Protocol {

namespace V_0 {

enum class EventId : uint8_t {
    MOVE_LEFT = 0,
    MOVE_RIGHT = 1,
    MOVE_UP = 2,
    MOVE_DOWN = 3,
    SHOOT = 4,
    MAX_EVENT
};

enum class EventAxisId : uint8_t {
    MAX_EVENT
};

enum class EventState : uint8_t {
    NONE = 0,
    PRESSED = 1,
    RELEASED = 2,
    MAX_STATE
};

LPL_PACKED_START

struct Event {
    V_0::EventId id;
    V_0::EventState state;
};

struct EventAxis {
    V_0::EventAxisId id;
};

LPL_PACKED_END

} /* namespace V_0 */

inline namespace V_1 {

enum class EventId : uint8_t {
    MOVE_LEFT = 0,
    MOVE_RIGHT = 1,
    MOVE_UP = 2,
    MOVE_DOWN = 3,
    MOVE_FRONT = 4,
    MOVE_BACK = 5,
    LOOK_LEFT = 6,
    LOOK_RIGHT = 7,
    LOOK_UP = 8,
    LOOK_DOWN = 9,
    SHOOT = 10,
    MAX_EVENT
};

enum class EventState : uint8_t {
    PRESSED = 0,
    RELEASED = 1,
    MAX_STATE
};

LPL_PACKED_START

struct Event {
    V_1::EventId id;
    V_1::EventState state;

    void print() const { std::cout << "Event: " << int(id) << " " << int(state) << std::endl; }
};

struct EventAxis {
    V_1::EventId id;
    float value;

    void print() const { std::cout << "EventAxis: " << int(id) << " " << value << std::endl; }
};

LPL_PACKED_END

} /* namespace V_1 */

} // namespace Flakkari::Protocol

#endif /* !EVENTS_HPP_ */
