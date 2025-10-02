/**************************************************************************
 * Flakkari Library v0.6.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Commands.hpp
 * @brief This file contains the Commands enum class. It is used to identify
 *        the command in the Flakkari Protocol.
 *
 * @see inspired by the https://en.wikipedia.org/wiki/IPv4 header
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.6.0
 * @date 2024-01-12
 **************************************************************************/

#ifndef COMMANDS_HPP_
#define COMMANDS_HPP_

#include <cstdint>

namespace Flakkari::Protocol {

namespace V_0 {

enum class CommandId : uint8_t {
    // 0 - 9: System
    REQ_CONNECT = 0,    // Client -> Server [Connect to server]: (username, password game)
    REP_CONNECT = 1,    // Server -> Client [Connection accepted]: (user_id)
    REQ_DISCONNECT = 2, // Client -> Server [Disconnect from server]: (user_id)
    REP_DISCONNECT = 3, // Server -> Client [Disconnect accepted]: ()
    REQ_PING = 4,       // Client -> Server [Ping server]: (user_id)
    REP_PING = 5,       // Server -> Client [Ping accepted]: ()
    REQ_PONG = 6,       // Client -> Server [Pong server]: (user_id)
    REP_PONG = 7,       // Server -> Client [Pong accepted]: ()
    REQ_HEARTBEAT = 8,  // Client -> Server [Heartbeat server) (Keep alive)]: (user_id)
    REP_HEARTBEAT = 9,  // Server -> Client [Heartbeat accepted) (Keep alive)]: ()
    // 10 - 19: Network
    REQ_LOGIN = 10,    // Client -> Server [Login]: (username, password game)
    REP_LOGIN = 11,    // Server -> Client [Login accepted]: ()
    REQ_LOGOUT = 12,   // Client -> Server [Logout]: (user_id)
    REP_LOGOUT = 13,   // Server -> Client [Logout accepted]: ()
    REQ_REGISTER = 14, // Client -> Server [Register]: (user_id, username, password)
    REP_REGISTER = 15, // Server -> Client [Register accepted]: ()
    // 20 - 29: Game
    REQ_ENTITY_SPAWN = 20,   // Server -> Client [Spawn entity]: (id)(component (position, rotation, velocity, etc))
    REP_ENTITY_SPAWN = 21,   // Client -> Server [Entity spawned]: ()
    REQ_ENTITY_UPDATE = 22,  // Server -> Client [Update entity]: (id)(component (position, rotation, velocity, etc))
    REP_ENTITY_UPDATE = 23,  // Client -> Server [Entity updated]: ()
    REQ_ENTITY_DESTROY = 24, // Server -> Client [Destroy entity]: (id)
    REP_ENTITY_DESTROY = 25, // Client -> Server [Entity destroyed]: ()
    REQ_ENTITY_MOVED = 26,   // Server -> Client [Move entity]: (id)(component (position, rotation, velocity, etc))
    REP_ENTITY_MOVED = 27,   // Client -> Server [Entity moved]: ()
    REQ_ENTITY_SHOOT = 28,   // Server -> Client [Shoot entity]: (id)(component (position, rotation, velocity, etc))
    REP_ENTITY_SHOOT = 29,   // Client -> Server [Entity shot]: ()
    // 30 - 39: User
    REQ_USER_UPDATE = 30, // Client -> Server [Update user]: (event_id, state)
    REP_USER_UPDATE = 31, // Server -> Client [User updated]: ()
    // 40 - 49: Chat
    // 50 - 59: Matchmaking
    REQ_CREATE_ROOM = 50, // Client -> Server [Create room]: (user_id)
    REP_CREATE_ROOM = 51, // Server -> Client [Room created]: (room_id)
    REQ_JOIN_ROOM = 52,   // Client -> Server [Join room]: (user_id, room_id)
    REP_JOIN_ROOM = 53,   // Server -> Client [Room joined]: ()
    REQ_LEAVE_ROOM = 54,  // Client -> Server [Leave room]: (user_id)
    REP_LEAVE_ROOM = 55,  // Server -> Client [Room left]: ()
    REQ_START_GAME = 56,  // Client -> Server [Start game]: (user_id)
    REP_START_GAME = 57,  // Server -> Client [Game started]: ()
    REQ_END_GAME = 58,    // Client -> Server [End game]: (user_id)
    REP_END_GAME = 59,    // Server -> Client [Game ended]: ()
    // 60 - 69: Leaderboard
    // 70 - 79: Achievement
    // 80 - 89: Inventory
    // 90 - 99: Store
    // 100 - 109: Party
    // 110 - 119: Tournament
    // 120 - 129: Telemetry
    // 130 - 139: Commerce
    // 140 - 149: Season
    // 150 - 159: PlayerData
    // 160 - 169: TitleData
    // 170 - 179: Snapshots
    // 180 - 189: CloudScript
    // 190 - 199: Server-Side CloudScript
    // 200 - 209: Shared Group Data
    // 210 - 219: Files
    // 220 - 229: Party
    // 230 - 239: Insights
    // 240 - 249: Messaging
    // 250 - 259: Multiplayer
    // 260 - 269: Server-Side CloudScript
    // 270 - 279: PlayStream
    // 280 - 289: Advertising
    // 290 - 299: Analytics
    // 300 - 309: Authentication
    // 310 - 319: Profiles
    // 320 - 329: Push Notifications
    // 330 - 339: Shared Group Data
    // 340 - 349: Title-Wide Data Management
    // 350 - 359: Characters
    // 360 - 369: Friend List
    // 370 - 379: Matchmaking
    // 380 - 389: Platform Specific
    // 390 - 399: Player Data Management
    // 400 - 409: Player Item Management
    // 410 - 419: Player Profile
    // 420 - 429: Title Data Management
    // 430 - 439: Title Internal
    // 440 - 449: Virtual Currency
    // 450 - 459: Player Data Management
    // 460 - 469: Player Item Management
    MAX_COMMAND_ID
};

static_assert(static_cast<uint8_t>(CommandId::MAX_COMMAND_ID) <= 0xFF, "CommandId is too big");

class Commands final {
public:
    static std::string command_to_string(CommandId id)
    {
        switch (id)
        {
        case CommandId::REQ_CONNECT: return "REQ_CONNECT";
        case CommandId::REP_CONNECT: return "REP_CONNECT";
        case CommandId::REQ_DISCONNECT: return "REQ_DISCONNECT";
        case CommandId::REP_DISCONNECT: return "REP_DISCONNECT";
        case CommandId::REQ_PING: return "REQ_PING";
        case CommandId::REP_PING: return "REP_PING";
        case CommandId::REQ_PONG: return "REQ_PONG";
        case CommandId::REP_PONG: return "REP_PONG";
        case CommandId::REQ_HEARTBEAT: return "REQ_HEARTBEAT";
        case CommandId::REP_HEARTBEAT: return "REP_HEARTBEAT";
        case CommandId::REQ_LOGIN: return "REQ_LOGIN";
        case CommandId::REP_LOGIN: return "REP_LOGIN";
        case CommandId::REQ_LOGOUT: return "REQ_LOGOUT";
        case CommandId::REP_LOGOUT: return "REP_LOGOUT";
        case CommandId::REQ_REGISTER: return "REQ_REGISTER";
        case CommandId::REP_REGISTER: return "REP_REGISTER";
        case CommandId::REQ_ENTITY_SPAWN: return "REQ_ENTITY_SPAWN";
        case CommandId::REP_ENTITY_SPAWN: return "REP_ENTITY_SPAWN";
        case CommandId::REQ_ENTITY_UPDATE: return "REQ_ENTITY_UPDATE";
        case CommandId::REP_ENTITY_UPDATE: return "REP_ENTITY_UPDATE";
        case CommandId::REQ_ENTITY_DESTROY: return "REQ_ENTITY_DESTROY";
        case CommandId::REP_ENTITY_DESTROY: return "REP_ENTITY_DESTROY";
        case CommandId::REQ_ENTITY_MOVED: return "REQ_ENTITY_MOVED";
        case CommandId::REP_ENTITY_MOVED: return "REP_ENTITY_MOVED";
        case CommandId::REQ_ENTITY_SHOOT: return "REQ_ENTITY_SHOOT";
        case CommandId::REP_ENTITY_SHOOT: return "REP_ENTITY_SHOOT";
        case CommandId::REQ_USER_UPDATE: return "REQ_USER_UPDATE";
        case CommandId::REP_USER_UPDATE: return "REP_USER_UPDATE";
        case CommandId::REQ_CREATE_ROOM: return "REQ_CREATE_ROOM";
        case CommandId::REP_CREATE_ROOM: return "REP_CREATE_ROOM";
        case CommandId::REQ_JOIN_ROOM: return "REQ_JOIN_ROOM";
        case CommandId::REP_JOIN_ROOM: return "REP_JOIN_ROOM";
        case CommandId::REQ_LEAVE_ROOM: return "REQ_LEAVE_ROOM";
        case CommandId::REP_LEAVE_ROOM: return "REP_LEAVE_ROOM";
        case CommandId::REQ_START_GAME: return "REQ_START_GAME";
        case CommandId::REP_START_GAME: return "REP_START_GAME";
        case CommandId::REQ_END_GAME: return "REQ_END_GAME";
        case CommandId::REP_END_GAME: return "REP_END_GAME";
        default: return "Unknown";
        }
    }
};

} /* namespace V_0 */

inline namespace V_1 {

enum class CommandId : uint8_t {
    // 0 - 9: System
    REQ_CONNECT = 0,    // Client -> Server [Connect to server]: (username, password game)
    REP_CONNECT = 1,    // Server -> Client [Connection accepted]: (user_id)
    REQ_DISCONNECT = 2, // Client -> Server [Disconnect from server]: (user_id)
    REP_DISCONNECT = 3, // Server -> Client [Disconnect accepted]: ()
    REQ_HEARTBEAT = 4,  // Client -> Server [Heartbeat server) (Keep alive)]: (user_id)
    REP_HEARTBEAT = 5,  // Server -> Client [Heartbeat accepted) (Keep alive)]: ()
    // 10 - 19: Network
    REQ_LOGIN = 10,    // Client -> Server [Login]: (username, password game)
    REP_LOGIN = 11,    // Server -> Client [Login accepted]: ()
    REQ_LOGOUT = 12,   // Client -> Server [Logout]: (user_id)
    REP_LOGOUT = 13,   // Server -> Client [Logout accepted]: ()
    REQ_REGISTER = 14, // Client -> Server [Register]: (user_id, username, password)
    REP_REGISTER = 15, // Server -> Client [Register accepted]: ()
    // 20 - 29: Game
    REQ_ENTITY_SPAWN = 20,   // Server -> Client [Spawn entity]: (id)(component (position, rotation, velocity, etc))
    REP_ENTITY_SPAWN = 21,   // Client -> Server [Entity spawned]: ()
    REQ_ENTITY_UPDATE = 22,  // Server -> Client [Update entity]: (id)(component (position, rotation, velocity, etc))
    REP_ENTITY_UPDATE = 23,  // Client -> Server [Entity updated]: ()
    REQ_ENTITY_DESTROY = 24, // Server -> Client [Destroy entity]: (id)
    REP_ENTITY_DESTROY = 25, // Client -> Server [Entity destroyed]: ()
    REQ_ENTITY_MOVED = 26,   // Server -> Client [Move entity]: (id)(component (position, rotation, velocity, etc))
    REP_ENTITY_MOVED = 27,   // Client -> Server [Entity moved]: ()
    REQ_ENTITY_SHOOT = 28,   // Server -> Client [Shoot entity]: (id)(component (position, rotation, velocity, etc))
    REP_ENTITY_SHOOT = 29,   // Client -> Server [Entity shot]: ()
    // 30 - 39: User
    REQ_USER_UPDATES = 30, // Client -> Server [Update user multi-events]: (event_id, state|event_id, float)
    REP_USER_UPDATES = 31, // Server -> Client [User updated]: ()
    // 40 - 49: Chat
    // 50 - 59: Matchmaking
    REQ_CREATE_ROOM = 50, // Client -> Server [Create room]: (user_id)
    REP_CREATE_ROOM = 51, // Server -> Client [Room created]: (room_id)
    REQ_JOIN_ROOM = 52,   // Client -> Server [Join room]: (user_id, room_id)
    REP_JOIN_ROOM = 53,   // Server -> Client [Room joined]: ()
    REQ_LEAVE_ROOM = 54,  // Client -> Server [Leave room]: (user_id)
    REP_LEAVE_ROOM = 55,  // Server -> Client [Room left]: ()
    REQ_START_GAME = 56,  // Client -> Server [Start game]: (user_id)
    REP_START_GAME = 57,  // Server -> Client [Game started]: ()
    REQ_END_GAME = 58,    // Client -> Server [End game]: (user_id)
    REP_END_GAME = 59,    // Server -> Client [Game ended]: ()
    MAX_COMMAND_ID
};

static_assert(static_cast<uint8_t>(CommandId::MAX_COMMAND_ID) <= 0xFF, "CommandId is too big");

class Commands final {
public:
    static std::string command_to_string(CommandId id)
    {
        switch (id)
        {
        case CommandId::REQ_CONNECT: return "REQ_CONNECT";
        case CommandId::REP_CONNECT: return "REP_CONNECT";
        case CommandId::REQ_DISCONNECT: return "REQ_DISCONNECT";
        case CommandId::REP_DISCONNECT: return "REP_DISCONNECT";
        case CommandId::REQ_HEARTBEAT: return "REQ_HEARTBEAT";
        case CommandId::REP_HEARTBEAT: return "REP_HEARTBEAT";
        case CommandId::REQ_LOGIN: return "REQ_LOGIN";
        case CommandId::REP_LOGIN: return "REP_LOGIN";
        case CommandId::REQ_LOGOUT: return "REQ_LOGOUT";
        case CommandId::REP_LOGOUT: return "REP_LOGOUT";
        case CommandId::REQ_REGISTER: return "REQ_REGISTER";
        case CommandId::REP_REGISTER: return "REP_REGISTER";
        case CommandId::REQ_ENTITY_SPAWN: return "REQ_ENTITY_SPAWN";
        case CommandId::REP_ENTITY_SPAWN: return "REP_ENTITY_SPAWN";
        case CommandId::REQ_ENTITY_UPDATE: return "REQ_ENTITY_UPDATE";
        case CommandId::REP_ENTITY_UPDATE: return "REP_ENTITY_UPDATE";
        case CommandId::REQ_ENTITY_DESTROY: return "REQ_ENTITY_DESTROY";
        case CommandId::REP_ENTITY_DESTROY: return "REP_ENTITY_DESTROY";
        case CommandId::REQ_ENTITY_MOVED: return "REQ_ENTITY_MOVED";
        case CommandId::REP_ENTITY_MOVED: return "REP_ENTITY_MOVED";
        case CommandId::REQ_ENTITY_SHOOT: return "REQ_ENTITY_SHOOT";
        case CommandId::REP_ENTITY_SHOOT: return "REP_ENTITY_SHOOT";
        case CommandId::REQ_USER_UPDATES: return "REQ_USER_UPDATES";
        case CommandId::REP_USER_UPDATES: return "REP_USER_UPDATES";
        case CommandId::REQ_CREATE_ROOM: return "REQ_CREATE_ROOM";
        case CommandId::REP_CREATE_ROOM: return "REP_CREATE_ROOM";
        case CommandId::REQ_JOIN_ROOM: return "REQ_JOIN_ROOM";
        case CommandId::REP_JOIN_ROOM: return "REP_JOIN_ROOM";
        case CommandId::REQ_LEAVE_ROOM: return "REQ_LEAVE_ROOM";
        case CommandId::REP_LEAVE_ROOM: return "REP_LEAVE_ROOM";
        case CommandId::REQ_START_GAME: return "REQ_START_GAME";
        case CommandId::REP_START_GAME: return "REP_START_GAME";
        case CommandId::REQ_END_GAME: return "REQ_END_GAME";
        case CommandId::REP_END_GAME: return "REP_END_GAME";
        default: return "Unknown";
        }
    }
};

} /* namespace V_1 */

} // namespace Flakkari::Protocol

#endif /* !COMMANDS_HPP_ */
