#pragma once
#include <cstdint>
#include <utility>

// Morton encoding/decoding utilities
namespace Optimizing::World::Morton {

// 2D Morton code functions
inline uint32_t part1by1(uint32_t x)
{
    x &= 0x0000ffff;
    x = (x | (x << 8)) & 0x00FF00FF;
    x = (x | (x << 4)) & 0x0F0F0F0F;
    x = (x | (x << 2)) & 0x33333333;
    x = (x | (x << 1)) & 0x55555555;
    return x;
}

inline uint32_t compact1by1(uint32_t x)
{
    x &= 0x55555555;
    x = (x | (x >> 1)) & 0x33333333;
    x = (x | (x >> 4)) & 0x00FF00FF;
    x = (x | (x >> 8)) & 0x0000FFFF;
    return x;
}

inline uint32_t encode2D(uint32_t x, uint32_t y) { return (part1by1(y) << 1) | part1by1(x); }

inline void decode2D(uint32_t morton, uint32_t &x, uint32_t &y)
{
    x = compact1by1(morton);
    y = compact1by1(morton >> 1);
}

// 3D Morton code functions (64-bit)
inline uint64_t part1by2(uint64_t x)
{
    x &= 0x1fffff;
    x = (x | (x << 32)) & 0x1f00000000ffffULL;
    x = (x | (x << 16)) & 0x1f0000ff0000ffULL;
    x = (x | (x << 8)) & 0x100f00f00f00f00fULL;
    x = (x | (x << 4)) & 0x10c30c30c30c30c3ULL;
    x = (x | (x << 2)) & 0x1249249249249249ULL;
    return x;
}

inline uint64_t compact1by2(uint64_t x)
{
    x &= 0x1249249249249249ULL;
    x = (x ^ (x >> 2)) & 0x10c30c30c30c30c3ULL;
    x = (x ^ (x >> 4)) & 0x100f00f00f00f00fULL;
    x = (x ^ (x >> 8)) & 0x1f0000ff0000ffULL;
    x = (x ^ (x >> 16)) & 0x1f00000000ffffULL;
    x = (x ^ (x >> 32)) & 0x1fffffULL;
    return x;
}

inline uint64_t encode3D(uint64_t x, uint64_t y, uint64_t z)
{
    return (part1by2(z) << 2) | (part1by2(y) << 1) | part1by2(x);
}

inline void decode3D(uint64_t code, uint64_t &x, uint64_t &y, uint64_t &z)
{
    x = compact1by2(code);
    y = compact1by2(code >> 1);
    z = compact1by2(code >> 2);
}

inline std::pair<uint64_t, uint64_t> keyRangeFromPrefix(uint64_t prefix, unsigned prefixLenBits, unsigned totalBits)
{
    unsigned shift = totalBits - prefixLenBits;
    uint64_t minKey = prefix << shift;
    uint64_t maxKey = minKey | ((shift == 64) ? ~0ULL : ((1ULL << shift) - 1ULL));
    return {minKey, maxKey};
}

} // namespace Optimizing::World::Morton
