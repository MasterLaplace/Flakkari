#include "RadixSort.hpp"
#include <algorithm>
#include <array>
#include <thread>

namespace Optimizing::World {

using namespace std;

void radix_sort_u64_indices(vector<uint64_t> &keys, vector<int> &indices)
{
    size_t n = keys.size();
    if (n == 0)
        return;

    vector<uint64_t> tmpKeys(n);
    vector<int> tmpIndices(n);

    for (int pass = 0; pass < 8; ++pass)
    {
        int shift = pass * 8;
        array<size_t, 256> count = {};

        for (size_t i = 0; i < n; ++i)
        {
            uint8_t byte = (keys[i] >> shift) & 0xFF;
            count[byte]++;
        }

        size_t sum = 0;
        for (int i = 0; i < 256; ++i)
        {
            size_t c = count[i];
            count[i] = sum;
            sum += c;
        }

        for (size_t i = 0; i < n; ++i)
        {
            uint8_t byte = (keys[i] >> shift) & 0xFF;
            size_t pos = count[byte]++;
            tmpKeys[pos] = keys[i];
            tmpIndices[pos] = indices[i];
        }

        swap(keys, tmpKeys);
        swap(indices, tmpIndices);
    }
}

void radix_sort_u64_indices_scratch(vector<uint64_t> &keys, vector<int> &indices, vector<uint64_t> &tmpKeys,
                                    vector<int> &tmpIndices)
{
    size_t n = keys.size();
    if (n == 0)
        return;

    // Ensure scratch buffers are sized
    if (tmpKeys.size() < n)
        tmpKeys.resize(n);
    if (tmpIndices.size() < n)
        tmpIndices.resize(n);

    for (int pass = 0; pass < 8; ++pass)
    {
        int shift = pass * 8;
        array<size_t, 256> count = {};

        for (size_t i = 0; i < n; ++i)
        {
            uint8_t byte = (keys[i] >> shift) & 0xFF;
            count[byte]++;
        }

        size_t sum = 0;
        for (int i = 0; i < 256; ++i)
        {
            size_t c = count[i];
            count[i] = sum;
            sum += c;
        }

        for (size_t i = 0; i < n; ++i)
        {
            uint8_t byte = (keys[i] >> shift) & 0xFF;
            size_t pos = count[byte]++;
            tmpKeys[pos] = keys[i];
            tmpIndices[pos] = indices[i];
        }

        // swap contents
        std::swap(keys, tmpKeys);
        std::swap(indices, tmpIndices);
    }
}

// 16-bit radix (4 passes) with provided scratch buffers — good tradeoff for large arrays
void radix_sort_u64_indices_b16_scratch(vector<uint64_t> &keys, vector<int> &indices, vector<uint64_t> &tmpKeys,
                                        vector<int> &tmpIndices)
{
    size_t n = keys.size();
    if (n == 0)
        return;

    if (tmpKeys.size() < n)
        tmpKeys.resize(n);
    if (tmpIndices.size() < n)
        tmpIndices.resize(n);

    // We'll use 4 passes: shift = 0,16,32,48
    for (int pass = 0; pass < 4; ++pass)
    {
        int shift = pass * 16;

        // count 65536 buckets — make the count vector thread_local so concurrent calls are safe
        static thread_local std::vector<size_t> count;
        if (count.size() < 65536)
            count.resize(65536);
        std::fill(count.begin(), count.end(), 0);

        for (size_t i = 0; i < n; ++i)
        {
            uint16_t word = static_cast<uint16_t>((keys[i] >> shift) & 0xFFFF);
            ++count[word];
        }

        size_t sum = 0;
        for (size_t i = 0; i < 65536; ++i)
        {
            size_t c = count[i];
            count[i] = sum;
            sum += c;
        }

        for (size_t i = 0; i < n; ++i)
        {
            uint16_t word = static_cast<uint16_t>((keys[i] >> shift) & 0xFFFF);
            size_t pos = count[word]++;
            tmpKeys[pos] = keys[i];
            tmpIndices[pos] = indices[i];
        }

        std::swap(keys, tmpKeys);
        std::swap(indices, tmpIndices);
    }
}

void radix_sort_u64_indices_b16(vector<uint64_t> &keys, vector<int> &indices)
{
    size_t n = keys.size();
    if (n == 0)
        return;

    vector<uint64_t> tmpKeys(n);
    vector<int> tmpIndices(n);

    for (int pass = 0; pass < 4; ++pass)
    {
        int shift = pass * 16;
        vector<size_t> count(65536, 0);

        for (size_t i = 0; i < n; ++i)
        {
            uint16_t word = (keys[i] >> shift) & 0xFFFF;
            count[word]++;
        }

        size_t sum = 0;
        for (size_t i = 0; i < 65536; ++i)
        {
            size_t c = count[i];
            count[i] = sum;
            sum += c;
        }

        for (size_t i = 0; i < n; ++i)
        {
            uint16_t word = (keys[i] >> shift) & 0xFFFF;
            size_t pos = count[word]++;
            tmpKeys[pos] = keys[i];
            tmpIndices[pos] = indices[i];
        }

        swap(keys, tmpKeys);
        swap(indices, tmpIndices);
    }
}

void radix_sort_u64_indices_parallel(vector<uint64_t> &keys, vector<int> &indices, int numThreads)
{
    size_t n = keys.size();
    if (n == 0)
        return;

    vector<uint64_t> tmpKeys(n);
    vector<int> tmpIndices(n);

    constexpr size_t B = 256;

    for (int pass = 0; pass < 8; ++pass)
    {
        int shift = pass * 8;

        vector<size_t> localCounts(numThreads * B);
        fill(localCounts.begin(), localCounts.end(), 0);

        auto countFunc = [&](int tid) {
            size_t start = (n * tid) / numThreads;
            size_t end = (n * (tid + 1)) / numThreads;
            size_t offset = tid * B;
            for (size_t i = start; i < end; ++i)
            {
                uint8_t byte = (keys[i] >> shift) & 0xFF;
                localCounts[offset + byte]++;
            }
        };

        vector<thread> threads;
        for (int t = 0; t < numThreads; ++t)
        {
            threads.emplace_back(countFunc, t);
        }
        for (auto &th : threads)
            th.join();
        threads.clear();

        array<size_t, B> globalCount = {};
        for (int t = 0; t < numThreads; ++t)
        {
            size_t offset = t * B;
            for (size_t b = 0; b < B; ++b)
            {
                globalCount[b] += localCounts[offset + b];
            }
        }

        array<size_t, B> prefix = {};
        size_t sum = 0;
        for (size_t b = 0; b < B; ++b)
        {
            prefix[b] = sum;
            sum += globalCount[b];
        }

        for (int t = 0; t < numThreads; ++t)
        {
            size_t offset = t * B;
            for (size_t b = 0; b < B; ++b)
            {
                size_t old = localCounts[offset + b];
                localCounts[offset + b] = prefix[b];
                prefix[b] += old;
            }
        }

        auto scatterFunc = [&](int tid) {
            size_t start = (n * tid) / numThreads;
            size_t end = (n * (tid + 1)) / numThreads;
            size_t offset = tid * B;
            for (size_t i = start; i < end; ++i)
            {
                uint8_t byte = (keys[i] >> shift) & 0xFF;
                size_t pos = localCounts[offset + byte]++;
                tmpKeys[pos] = keys[i];
                tmpIndices[pos] = indices[i];
            }
        };

        for (int t = 0; t < numThreads; ++t)
        {
            threads.emplace_back(scatterFunc, t);
        }
        for (auto &th : threads)
            th.join();

        swap(keys, tmpKeys);
        swap(indices, tmpIndices);
    }
}

} // namespace Optimizing::World
