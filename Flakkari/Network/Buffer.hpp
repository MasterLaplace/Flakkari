/**************************************************************************
 * Flakkari Library v0.0.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Buffer.hpp
 * @brief Buffer is a class that represents a buffer of bytes.
 * It is used to store data that will be sent or received.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.0.0
 * @date 2023-12-21
 **************************************************************************/

#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <vector>
#include <string>
#include <iostream>

using byte = unsigned char;
using byte_t = byte;

namespace Flakkari::Network {

/**
 * @brief Buffer is a class that represents a buffer of bytes.
 *
 * It is used to store data that will be sent or received.
 * Here is an example of how to use Buffer:
 * @example "Flakkari/Network/Buffer.cpp"
 * @code
 * #include "Buffer.hpp"
 *
 * int main() {
 *     Flakkari::Network::Buffer buffer;
 *     std::string str = "Hello World!";
 *
 *     buffer = str;
 *     std::cout << buffer << std::endl;
 *
 *     return 0;
 * }
 * @endcode
 * @see Flakkari::Network::Socket
 */
class Buffer : public std::vector<byte> {
    public:
        using std::vector<byte>::operator=;
        using std::vector<byte>::operator[];
        using std::vector<byte>::at;
        using std::vector<byte>::front;
        using std::vector<byte>::back;
        using std::vector<byte>::begin;
        using std::vector<byte>::end;
        using std::vector<byte>::rbegin;
        using std::vector<byte>::rend;
        using std::vector<byte>::empty;
        using std::vector<byte>::size;
        using std::vector<byte>::max_size;
        using std::vector<byte>::reserve;
        using std::vector<byte>::capacity;
        using std::vector<byte>::shrink_to_fit;
        using std::vector<byte>::clear;
        using std::vector<byte>::insert;
        using std::vector<byte>::erase;
        using std::vector<byte>::push_back;
        using std::vector<byte>::pop_back;
        using std::vector<byte>::resize;
        using std::vector<byte>::swap;

    public:
        using std::vector<byte>::vector;

    public:
        /**
         * @brief Get the size of the buffer
         *
         * @return size_t  Size of the buffer
         */
        std::size_t getSize() const;

        /**
         * @brief Get the capacity of the buffer
         *
         * @return size_t  Capacity of the buffer
         */
        std::size_t getCapacity() const;

        /**
         * @brief Get the remaining space in the buffer
         *
         * @return size_t  Remaining space in the buffer
         */
        std::size_t getRemainingSpace() const;

        /**
         * @brief Get the data of the buffer
         *
         * @return byte*  Data of the buffer
         */
        byte *getData();

        /**
         * @brief Get the data of the buffer
         *
         * @return const byte*  Data of the buffer
         */
        const byte *getData() const;

        /**
         * @brief Get the data of the buffer
         *
         * @return byte*  Data of the buffer
         */
        operator byte *();

        /**
         * @brief Get the data of the buffer
         *
         * @return const byte*  Data of the buffer
         */
        operator const byte *() const;

        /**
         * @brief Get the data of the buffer
         *
         * @return byte*  Data of the buffer
         */
        operator std::string();

        /**
         * @brief Get the data of the buffer
         *
         * @return const byte*  Data of the buffer
         */
        operator const std::string() const;

    protected:
    private:
};

/**
 * @brief Convert Buffer to string
 *
 * @param os  Output stream
 * @param addr  Buffer to convert
 * @return std::ostream&  Output stream
 */
std::ostream &operator<<(std::ostream &os, const Buffer &buffer);

} // namespace Flakkari::Network

#endif /* !BUFFER_HPP_ */
