/**************************************************************************
 * Flakkari Library v0.10.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file PacketFactoryV2.hpp
 * @brief Engine-agnostic packet factory for Protocol V2.
 *        Works with any ECS engine through the IRegistry interface.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.10.0
 * @date 2024-01-13
 **************************************************************************/

#ifndef PACKETFACTORYV2_HPP_
#define PACKETFACTORYV2_HPP_

#include "ComponentRegistry.hpp"
#include "Engine/API/IRegistry.hpp"
#include "Packet.hpp"

#include <typeindex>

namespace Flakkari::Protocol {

/**
 * @brief Engine-agnostic packet factory for Protocol V2.
 *
 * Unlike the legacy PacketFactory which is tightly coupled to Flakkari ECS,
 * PacketFactoryV2 works with any ECS engine through the IRegistry interface
 * and uses ComponentHash for component identification.
 */
class PacketFactoryV2 {
public:
    /**
     * @brief Add a single component to a packet using V2 protocol.
     *
     * The packet format is:
     * - ComponentHash (4 bytes): The hash identifying the component type
     * - DataSize (2 bytes): Size of the serialized component data
     * - Data (variable): The serialized component bytes
     *
     * @tparam Id Packet ID type
     * @tparam T Component type
     * @param packet The packet to add to
     * @param hash The component hash
     * @param component The component value
     */
    template<typename Id, typename T>
    static void addComponentToPacket(Packet<Id>& packet, ComponentHash hash, const T& component)
    {
        auto& registry = ComponentSerializerRegistry::getInstance();
        auto* serializer = registry.getSerializer(hash);

        if (!serializer)
        {
            // Component not registered, skip
            return;
        }

        std::vector<byte> data = serializer->serialize(component);

        packet << hash;
        packet << static_cast<uint16_t>(data.size());
        for (const auto& b : data)
            packet << b;
    }

    /**
     * @brief Add a component from a registry to a packet using V2 protocol.
     *
     * @tparam Id Packet ID type
     * @tparam T Component type
     * @param packet The packet to add to
     * @param registry The ECS registry
     * @param entity The entity to get the component from
     * @param hash The component hash
     */
    template<typename Id, typename T>
    static void addComponentFromRegistry(Packet<Id>& packet, Engine::API::IRegistry& registry,
                                         Engine::API::IEntity entity, ComponentHash hash)
    {
        auto componentOpt = registry.getComponent<T>(entity);

        if (componentOpt.has_value())
        {
            addComponentToPacket<Id, T>(packet, hash, componentOpt.value());
        }
    }

    /**
     * @brief Read a component from a packet using V2 protocol.
     *
     * @tparam Id Packet ID type
     * @param packet The packet to read from
     * @param[out] hash The component hash that was read
     * @return The deserialized component as std::any, or empty if unknown
     */
    template<typename Id>
    static std::any readComponentFromPacket(Packet<Id>& packet, ComponentHash& hash)
    {
        packet >> hash;

        uint16_t dataSize;
        packet >> dataSize;

        std::vector<byte> data(dataSize);
        for (uint16_t i = 0; i < dataSize; ++i)
            packet >> data[i];

        auto& registry = ComponentSerializerRegistry::getInstance();
        auto* serializer = registry.getSerializer(hash);

        if (!serializer)
        {
            return std::any{}; // Unknown component type
        }

        return serializer->deserialize(data);
    }

    /**
     * @brief Add entity header to packet.
     *
     * @tparam Id Packet ID type
     * @param packet The packet
     * @param entity The entity
     * @param componentCount Number of components that follow
     */
    template<typename Id>
    static void addEntityHeader(Packet<Id>& packet, Engine::API::IEntity entity, uint8_t componentCount)
    {
        packet << static_cast<std::size_t>(entity);
        packet << componentCount;
    }

    /**
     * @brief Helper to create a packet with entity and multiple components.
     *
     * @tparam Id Packet ID type
     * @param commandId The command ID for this packet
     * @param entity The entity
     * @param components Vector of (hash, serialized data) pairs
     * @return The constructed packet
     */
    template<typename Id>
    static Packet<Id> createEntityPacket(Id commandId, Engine::API::IEntity entity,
                                         const std::vector<ComponentData>& components)
    {
        Packet<Id> packet;
        packet << commandId;
        addEntityHeader(packet, entity, static_cast<uint8_t>(components.size()));

        for (const auto& comp : components)
        {
            packet << comp.hash;
            packet << static_cast<uint16_t>(comp.data.size());
            for (const auto& b : comp.data)
                packet << b;
        }

        return packet;
    }
};

} // namespace Flakkari::Protocol

#endif /* !PACKETFACTORYV2_HPP_ */
