#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace Optimizing::World {

// Radix sort variants
void radix_sort_u64_indices(std::vector<uint64_t> &keys, std::vector<int> &idx);
void radix_sort_u64_indices_b16(std::vector<uint64_t> &keys, std::vector<int> &idx);
void radix_sort_u64_indices_parallel(std::vector<uint64_t> &keys, std::vector<int> &idx, int numThreads);
// Radix variant that takes preallocated temporary buffers (no internal allocs)
void radix_sort_u64_indices_scratch(std::vector<uint64_t> &keys, std::vector<int> &indices,
                                    std::vector<uint64_t> &tmpKeys, std::vector<int> &tmpIndices);
// 16-bit pass variant (4 passes) using scratch buffers — fewer passes, good locality for large arrays
void radix_sort_u64_indices_b16_scratch(std::vector<uint64_t> &keys, std::vector<int> &indices,
                                        std::vector<uint64_t> &tmpKeys, std::vector<int> &tmpIndices);

} // namespace Optimizing::World
