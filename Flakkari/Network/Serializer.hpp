/**************************************************************************
 * Flakkari Library v0.9.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Serializer.hpp
 * @brief Class used to serialize and deserialize objects. (POD types)
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.9.0
 * @date 2023-12-2
 **************************************************************************/

#ifndef SERIALIZER_HPP_
#define SERIALIZER_HPP_

#include "Buffer.hpp"

namespace Flakkari::Network {

/**
 * @brief Class used to serialize and deserialize objects.
 *
 * @details This class is used to serialize and deserialize objects.
 *          It is used to serialize and deserialize especially components
 *          in the Flakkari/Protocol/Component.hpp file.
 */
class Serializer {
public:
    /**
     * @brief Serialize an object into a buffer.
     *
     * @tparam T  The type of the object to serialize. Must be a POD type. (Plain Old Data)
     * @param obj  The object to serialize. Must be a POD type. (Plain Old Data)
     * @return Network::Buffer  The serialized object.
     */
    template <typename T> static Network::Buffer serialize(const T &obj)
    {
        Network::Buffer buffer(sizeof(obj));
        std::copy((byte *) &obj, (byte *) &obj + sizeof(obj), buffer.begin());
        return buffer;
    }

    /**
     * @brief Deserialize an object from a buffer.
     *
     * @tparam T  The type of the object to deserialize. Must be a POD type. (Plain Old Data)
     * @param buffer  The buffer to deserialize the object from.
     * @return T  The deserialized object. Must be a POD type. (Plain Old Data)
     */
    template <typename T> static T deserialize(const Network::Buffer &buffer)
    {
        T obj;
        std::copy(buffer.begin(), buffer.begin() + sizeof(T), (byte *) &obj);
        return obj;
    }
};

} // namespace Flakkari::Network

#endif /* !SERIALIZER_HPP_ */
