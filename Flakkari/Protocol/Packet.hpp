/**************************************************************************
 * Flakkari Library v0.5.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Packet.hpp
 * @brief Flakkari::Protocol::Packet class header. This class is used to
 * represent a packet. A packet is a header and a payload.
 *  - The header is a Flakkari::Protocol::Header object.
 *  - The payload is a Flakkari::Network::Buffer object.
 *
 * @see Flakkari::Protocol::Header
 * @see Flakkari::Network::Buffer
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.5.0
 * @date 2023-12-24
 **************************************************************************/

#ifndef PACKET_HPP_
#define PACKET_HPP_

#include "Components.hpp"
#include "Events.hpp"
#include "Header.hpp"
#include "Logger/Logger.hpp"

#include <cstring>

namespace Flakkari::Protocol {

/**
 * @brief Flakkari Packet v1 (new packet)
 *
 * @tparam Id: The type of the command id.
 * @param header: The header of the packet.
 * @param payload: The payload of the packet.
 */
template <typename Id> struct Packet {
    Header<Id> header;
    Network::Buffer payload;

    /**
     * @brief Get the size of the packet.
     *
     */
    std::size_t size() const { return sizeof(header) + payload.size(); }

    /**
     * @brief Convert the packet to a string.
     *
     * @return std::string  The packet as a string.
     */
    std::string to_string()
    {
        std::string str = "Packet<Id: " + Commands::command_to_string(header._commandId) +
                          ", ContentLength: " + std::to_string(int(header._contentLength)) +
                          ", SequenceNumber: " + std::to_string(long(header._sequenceNumber)) +
                          ", Payload: " + std::string((const char *) payload.data(), payload.size()) + ">";
        return str;
    }

    /**
     * @brief Overload the << operator to print the packet.
     *
     * @param os  The output stream.
     * @param packet  The packet to print.
     * @return std::ostream&  The output stream.
     */
    friend std::ostream &operator<<(std::ostream &os, const Packet &packet)
    {
        os << "Packet<Id: " << htons(packet.header._commandId)
           << ", ContentLength: " << htons(packet.header._contentLength)
           << ", SequenceNumber: " << htonl(packet.header._sequenceNumber) << ", Payload: " << packet.payload << ">";
        return os;
    }

    /**
     * @brief Add data to the packet.
     *
     * @tparam DataType  Type of the data to add.
     * @param packet  The packet to add the data to.
     * @param data  The data to add.
     * @return Packet<Id> & The packet with the data added.
     */
    template <typename DataType> friend Packet<Id> &operator<<(Packet<Id> &packet, const DataType &data)
    {
        static_assert(std::is_trivially_copyable<DataType>::value,
                      "DataType must be trivially copyable to be used in a packet.");
        static_assert(std::is_standard_layout<DataType>::value,
                      "DataType must be standard layout to be used in a packet.");

        const byte *dataBytes = reinterpret_cast<const byte *>(&data);
        packet.payload.insert(packet.payload.end(), dataBytes, dataBytes + sizeof(data));
        packet.header._contentLength += (ushort) sizeof(data);
        return packet;
    }

    friend Packet<Id> &operator<<(Packet<Id> &packet, const std::string &data) = delete;

    /**
     * @brief Extract data from the packet.
     *
     * @tparam DataType  Type of the data to extract.
     * @param packet  The packet to extract the data from.
     * @param data  The data to extract.
     * @return Packet<Id> & The packet with the data extracted.
     */
    template <typename DataType> friend Packet<Id> &operator>>(Packet<Id> &packet, DataType &data)
    {
        static_assert(std::is_trivially_copyable<DataType>::value,
                      "DataType must be trivially copyable to be used in a packet.");
        static_assert(std::is_standard_layout<DataType>::value,
                      "DataType must be standard layout to be used in a packet.");

        if (packet.payload.size() < sizeof(data))
            throw std::runtime_error("Packet payload is too small to extract data.");

        std::memcpy(&data, packet.payload.data(), sizeof(data));
        packet.payload.erase(packet.payload.begin(), packet.payload.begin() + sizeof(data));
        packet.header._contentLength -= sizeof(data);
        return packet;
    }

    friend Packet<Id> &operator>>(Packet<Id> &packet, std::string &data) = delete;

    /**
     * @brief Inject a string into the packet.
     *
     * @param str  The string to inject.
     */
    void injectString(const std::string &str)
    {
        int intValue = (int) str.size();
        const byte *dataBytes = reinterpret_cast<const byte *>(&intValue);
        payload.insert(payload.end(), dataBytes, dataBytes + sizeof(intValue));
        payload += str;
        header._contentLength += (ushort) str.size() + (ushort) sizeof(intValue);
    }

    /**
     * @brief Extract a string from the packet.
     *
     * @param packet  The packet to extract the string from.
     * @return std::string  The extracted string.
     */
    std::string extractString()
    {
        if (payload.size() < sizeof(int))
            throw std::runtime_error("Packet payload is too small to extract string.");

        int intValue;
        std::memcpy(&intValue, payload.data(), sizeof(intValue));
        payload.erase(payload.begin(), payload.begin() + sizeof(intValue));
        std::string str(payload.begin(), payload.begin() + intValue);
        payload.erase(payload.begin(), payload.begin() + intValue);
        header._contentLength -= (ushort) intValue + (ushort) sizeof(intValue);
        return str;
    }

    /**
     * @brief Serialize the packet into a buffer to be sent over the network.
     *
     * @return Network::Buffer  The buffer containing the serialized packet.
     */
    Network::Buffer serialize() const
    {
        Network::Buffer buffer(size());
        std::memcpy(buffer.data(), &header, sizeof(header));
        std::memcpy(buffer.data() + sizeof(header), payload.data(), payload.size());
        return buffer;
    }

    /**
     * @brief Deserialize the buffer into a packet.
     *
     * @param buffer  The buffer containing the serialized packet.
     * @return true  The packet has been deserialized successfully.
     * @return false  The packet has not been deserialized successfully.
     */
    [[nodiscard]] bool deserialize(const Network::Buffer &buffer)
    {
        if (buffer.size() < header.size())
            return FLAKKARI_LOG_WARNING("Buffer is too small to deserialize a packet."), false;
        std::memcpy(&header, buffer.data(), header.size());
        if (header._priority >= Priority::MAX_PRIORITY)
            return FLAKKARI_LOG_WARNING("Priority is too big (" + std::to_string((int) header._priority) + ")"), false;
        if (header._apiVersion >= ApiVersion::MAX_VERSION)
            return FLAKKARI_LOG_WARNING("ApiVersion is too big (" + std::to_string((int) header._apiVersion) + ")"),
                   false;
        if (header._commandId >= CommandId::MAX_COMMAND_ID)
            return FLAKKARI_LOG_WARNING("CommandId is too big (" + std::to_string((int) header._commandId) + ")"),
                   false;
        if (header._contentLength > buffer.size() - header.size())
            return false;
        payload = buffer.extractData(header.size(), header._contentLength);
        return true;
    }
};

} // namespace Flakkari::Protocol

#endif /* !PACKET_HPP_ */
