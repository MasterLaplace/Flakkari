/**************************************************************************
 * Flakkari Library v0.8.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file PacketQueue.hpp
 * @brief This file contains the PacketQueue class. It is used to store
 *        packets in a queue. It is thread safe. It is used by the client
 *        and the server to store incoming packets.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.8.0
 * @date 2024-01-12
 **************************************************************************/

#ifndef PACKETQUEUE_HPP_
#define PACKETQUEUE_HPP_

#include <deque>
#include <mutex>

namespace Flakkari::Network {

/**
 * @brief Thread-safe packet queue class
 * @tparam T Type of packets to be stored in the queue
 *
 * @example "Flakkari/Network/PacketQueue.hpp"
 * @code
 * #include "PacketQueue.hpp"
 * PacketQueue<Protocol::Packet<Protocol::CommandId>> packetQueue;
 * packetQueue.push_back(packet);
 * auto packet = packetQueue.pop_front();
 * @endcode
 */
template <typename T> class PacketQueue {
public:
    PacketQueue() = default;
    PacketQueue(const PacketQueue<T> &) = delete;
    virtual ~PacketQueue() { clear(); }

public:
    const T &front()
    {
        std::scoped_lock lock(_mutex);
        return _queue.front();
    }

    const T &back()
    {
        std::scoped_lock lock(_mutex);
        return _queue.back();
    }

    void push_back(const T &value)
    {
        std::scoped_lock lock(_mutex);
        _queue.push_back(value);
    }

    void push_front(const T &value)
    {
        std::scoped_lock lock(_mutex);
        _queue.push_front(value);
    }

    T pop_front()
    {
        std::scoped_lock lock(_mutex);
        auto value = std::move(_queue.front());
        _queue.pop_front();
        return value;
    }

    T pop_back()
    {
        std::scoped_lock lock(_mutex);
        auto value = std::move(_queue.back());
        _queue.pop_back();
        return value;
    }

    bool empty()
    {
        std::scoped_lock lock(_mutex);
        return _queue.empty();
    }

    size_t size()
    {
        std::scoped_lock lock(_mutex);
        return _queue.size();
    }

    void clear()
    {
        std::scoped_lock lock(_mutex);
        _queue.clear();
    }

protected:
private:
    std::mutex _mutex;
    std::deque<T> _queue;
};

} // namespace Flakkari::Network

#endif /* !PACKETQUEUE_HPP_ */
