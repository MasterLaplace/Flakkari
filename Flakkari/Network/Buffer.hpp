/**************************************************************************
 * Flakkari Library v0.10.0
 *
 * Flakkari Library is a C++ Library for Network.
 * @file Buffer.hpp
 * @brief Buffer is a class that represents a buffer of bytes.
 *        It is used to store data that will be sent or received.
 *
 * Flakkari Library is under MIT License.
 * https://opensource.org/licenses/MIT
 * © 2023 @MasterLaplace
 * @version 0.10.0
 * @date 2023-12-21
 **************************************************************************/

#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

using byte = std::uint8_t;

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
     * @brief Construct a new Buffer object
     *
     * @param data  Data of the buffer
     */
    Buffer(std::string data);

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
     * @param offset  Offset of the data to extract
     * @param length  Length of the data to extract
     * @return std::string  Data of the buffer
     */
    Buffer extractData(std::size_t offset, std::size_t length) const;

    /**
     * @brief Concatenate two buffers together
     *
     * @param otherBuffer Buffer to concatenate
     */
    void concat(const Buffer &otherBuffer);

    /**
     * @brief Calculate the checksum of the buffer
     *
     * @return uint16_t Checksum of the buffer
     *
     * @note The checksum is calculated by adding all the bytes of the buffer.
     * The result is then truncated to 16 bits.
     * @see https://en.wikipedia.org/wiki/Checksum
     */
    uint16_t calculateChecksum() const;

    /**
     * @brief Encode the buffer using XOR algorithm with a key
     *
     * @param key  Key to encode the buffer
     */
    void bitwiseXOR(const Buffer &otherBuffer);

    /**
     * @brief Encode the buffer using run-length encoding algorithm
     *
     * @note The buffer must be encoded using run-length decoding algorithm.
     * @see https://en.wikipedia.org/wiki/Run-length_encoding
     */
    void runLengthEncode();

    /**
     * @brief Decode the buffer using run-length decoding algorithm
     *
     */
    void runLengthDecode();

    /**
     * @brief Convert the buffer from binary to hexadecimal
     *
     */
    void convertToHex();

    /**
     * @brief Convert the buffer from hexadecimal to binary
     *
     */
    void convertFromHex();

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

    /**
     * @brief Concatenate two buffers together
     *
     * @param otherBuffer  Buffer to add
     * @return Buffer  Result of the addition
     */
    Buffer operator+(const Buffer &otherBuffer) const;

    /**
     * @brief Concatenate two buffers together
     *
     * @param otherBuffer  Buffer to add
     * @return Buffer  Result of the addition
     */
    Buffer operator+=(const Buffer &otherBuffer);

    /**
     * @brief Remove a buffer from another buffer
     *
     * @param otherBuffer  Buffer to remove
     * @return Buffer  Result of the subtraction
     */
    Buffer operator-(const Buffer &otherBuffer) const;

    /**
     * @brief Remove a buffer from another buffer
     *
     * @param otherBuffer  Buffer to remove
     * @return Buffer  Result of the subtraction
     */
    Buffer operator-=(const Buffer &otherBuffer);

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

/**
 * @brief Convert string to Buffer
 *
 * @param is  Input stream
 * @param addr  Buffer to convert
 * @return std::istream&  Input stream
 */
std::istream &operator>>(std::istream &is, Buffer &buffer);

} // namespace Flakkari::Network

#endif /* !BUFFER_HPP_ */
