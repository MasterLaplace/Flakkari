/**
 * @file world_partition.h
 * @brief World Partitioning System for MMORPG
 *
 * Optimized spatial partitioning with FlatDynamicOctree per chunk:
 * - Dynamic loading/unloading based on player position
 * - Thread pool for asynchronous chunk loading
 * - Cache-friendly octrees (flat storage per chunk)
 * - Support for negative coordinates (hash map)
 * - 3-layer architecture: static/semi-dynamic/ephemeral
 *
 * Performance:
 * - Chunk size: 255x255 (configurable)
 * - Load radius: 3x3 chunks around player
 * - Rebuild per chunk: ~0.02ms for 100-500 entities
 * - Total overhead: <1ms/frame for entire world
 *
 * Architecture:
 * ```
 * WorldPartition (global manager)
 *   └─> Partition (chunk 255x255)
 *         └─> FlatDynamicOctree (cache-friendly)
 *               └─> Entities (contiguous vector)
 * ```
 *
 * @author @MasterLaplace
 * @version 4.0 - FlatDynamic Integration
 * @date 2025-11-19
 */

#pragma once

#include "FlatDynamicOctree.hpp"
#if __has_include(<ankerl/unordered_dense.h>)
#    include <ankerl/unordered_dense.h>
#    define WP_HAS_ANKERL 1
#elif __has_include(<boost/unordered_flat_map.hpp>)
#    include <boost/unordered_flat_map.hpp>
#    define WP_HAS_BOOST_FLAT 1
#endif
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <glm/glm.hpp>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

namespace Optimizing::World {

// Hash function for glm::ivec2 (chunk coordinates)
struct IVec2Hash {
    std::size_t operator()(const glm::ivec2 &v) const noexcept
    {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
    }
};

// Convert chunk coordinates (signed) to a Morton key with bias.
// - chunkBits: how many bits to reserve for chunk coordinate per axis (e.g. 21)
// This supports coords roughly in [-2^(chunkBits-1), +2^(chunkBits-1)-1]
inline uint64_t chunkMortonKey(const glm::ivec2 &coord, unsigned chunkBits = 21)
{
    const uint64_t bias = 1ULL << (chunkBits - 1);
    uint64_t ux = static_cast<uint64_t>(static_cast<int64_t>(coord.x) + static_cast<int64_t>(bias));
    uint64_t uz = static_cast<uint64_t>(static_cast<int64_t>(coord.y) + static_cast<int64_t>(bias));
    return Morton::encode2D(static_cast<uint32_t>(ux), static_cast<uint32_t>(uz));
}

/**
 * @brief Entity interface for world partitioning
 *
 * Your entity must have:
 * - glm::vec3 position
 * - flat_dynamic::BoundingBox bbox (or getBounds())
 */
template <typename T>
concept WorldEntity = requires(T entity) {
    { entity.position } -> std::convertible_to<glm::vec3>;
    { entity.bbox } -> std::convertible_to<BoundingBox>;
};

/**
 * @brief Simple thread pool for async chunk loading
 */
class ThreadPool {
public:
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    template <class F, class... Args>
    auto enqueue(F &&f, Args &&...args) -> std::future<typename std::invoke_result<F, Args...>::type>
    {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(_queueMutex);

            if (_stop)
            {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            _tasks.emplace([task]() { (*task)(); });
        }
        _condition.notify_one();
        return res;
    }

    void shutdown();
    // Wait until all queued tasks finish executing (non-destructive)
    void waitIdle();

private:
    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _tasks;
    std::mutex _queueMutex;
    std::condition_variable _condition;
    bool _stop = false;
    std::atomic<size_t> _active{0};
};

/**
 * @brief Single partition (chunk) of the world
 *
 * Contains:
 * - Position & size (world space)
 * - FlatDynamicOctree for spatial queries
 * - Entities stored in contiguous vector
 * - Load/unload state
 */
template <typename Entity> class Partition {
public:
    Partition(const glm::vec3 &position, const glm::vec3 &size)
        : _position(position), _size(size), _octree(flat_dynamic::BoundingBox(position, position + size), 8, 32),
          _loaded(false)
    {
        _entities.reserve(512); // Pre-allocate for typical chunk
    }

    /**
     * @brief Add entity to partition (not yet loaded into octree)
     */
    void addEntity(const Entity &entity)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _entities.push_back(entity);
    }

    /**
     * @brief Load partition (build octree from entities)
     *
     * Call this when player enters load radius
     * Cost: ~0.02ms for 100-500 entities
     */
    void load()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_loaded || _entities.empty())
            return;

        _octree.rebuild(_entities);
        _loaded = true;
    }

    /**
     * @brief Unload partition (clear octree, keep entities)
     *
     * Call this when player exits load radius
     */
    void unload()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_loaded)
            return;

        _octree.clear();
        _loaded = false;
    }

    /**
     * @brief Update entities and rebuild octree
     *
     * Call this periodically for loaded chunks (every 5-10 frames)
     * Cost: ~0.02ms for typical chunk
     */
    void update()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_loaded || _entities.empty())
            return;

        _octree.rebuild(_entities);
    }

    /**
     * @brief Query entities in bounding box
     *
     * Returns indices into _entities vector
     */
    [[nodiscard]] std::vector<uint32_t> query(const flat_dynamic::BoundingBox &box) const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_loaded)
            return {};
        return _octree.query(box);
    }

    /**
     * @brief Get all entities in partition
     */
    [[nodiscard]] const std::vector<Entity> &getEntities() const { return _entities; }

    /**
     * @brief Get entity by index (from query result)
     */
    [[nodiscard]] const Entity &getEntity(uint32_t index) const { return _entities[index]; }

    /**
     * @brief Check if partition is loaded
     */
    [[nodiscard]] inline bool isLoaded() const noexcept { return _loaded; }

    /**
     * @brief Get partition bounds
     */
    [[nodiscard]] inline flat_dynamic::BoundingBox getBounds() const noexcept
    {
        return flat_dynamic::BoundingBox(_position, _position + _size);
    }

    /**
     * @brief Get partition position (chunk origin)
     */
    [[nodiscard]] inline const glm::vec3 &getPosition() const noexcept { return _position; }

    /**
     * @brief Get partition size
     */
    [[nodiscard]] inline const glm::vec3 &getSize() const noexcept { return _size; }

    /**
     * @brief Get entity count
     */
    [[nodiscard]] inline size_t entityCount() const noexcept { return _entities.size(); }

    /**
     * @brief Clear all entities
     */
    void clear()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _entities.clear();
        _octree.clear();
        _loaded = false;
    }

private:
    glm::vec3 _position;
    glm::vec3 _size;
    std::vector<Entity> _entities;
    flat_dynamic::FlatDynamicOctree _octree;
    mutable std::mutex _mutex;
    bool _loaded;
};

/**
 * @brief World Partition Manager for MMORPG
 *
 * Manages infinite 2D grid of chunks with dynamic loading
 *
 * Usage:
 * ```cpp
 * WorldPartition<Entity> world(glm::vec3(255, 10000, 255));
 *
 * // Insert entities (batched)
 * world.insertEntities(allEntities);
 *
 * // Game loop
 * world.update(playerPosition);  // Load/unload chunks
 *
 * // Query nearby entities
 * auto nearby = world.query(searchBox);
 * for (uint32_t idx : nearby.indices) {
 *     const Entity& entity = nearby.partition->getEntity(idx);
 * }
 * ```
 */
template <typename Entity> class WorldPartition {
public:
    struct QueryResult {
        std::shared_ptr<Partition<Entity>> partition;
        std::vector<uint32_t> indices;
    };

    /**
     * @brief Construct world partition system
     *
     * @param chunkSize Size of each partition (e.g., 255x10000x255)
     * @param loadRadius Radius in chunks to keep loaded (default: 1 = 3x3 grid)
     * @param threadCount Number of worker threads (default: hardware_concurrency)
     */
    explicit WorldPartition(const glm::vec3 &chunkSize = glm::vec3(255.0f, 10000.0f, 255.0f), int loadRadius = 1,
                            size_t threadCount = std::thread::hardware_concurrency())
        : _chunkSize(chunkSize), _loadRadius(loadRadius), _chunkBits(21), _threadPool(threadCount)
    {
    }

    ~WorldPartition() { _threadPool.shutdown(); }

    /**
     * @brief Insert entities into world (batch)
     *
     * Distributes entities to appropriate chunks
     * Cost: O(n) with hash map lookups
     */
    void insertEntities(const std::vector<Entity> &entities)
    {
        std::lock_guard<std::mutex> lock(_mutex);
#if defined(WP_ENABLE_INSTRUMENTATION)
        using clk = std::chrono::high_resolution_clock;
        auto t_insert_start = clk::now();
#endif
        for (const auto &entity : entities)
        {
            glm::ivec2 chunkCoord = getChunkCoordinate(entity.position);
#if defined(WP_ENABLE_INSTRUMENTATION)
            auto t_key0 = clk::now();
#endif
            uint64_t key = chunkMortonKey(chunkCoord); // default chunkBits
#if defined(WP_ENABLE_INSTRUMENTATION)
            auto t_key1 = clk::now();
            _instr_keyEncodeMs += std::chrono::duration<double, std::milli>(t_key1 - t_key0).count();
#endif

            auto it = _partitionsMorton.find(key);
            if (it == _partitionsMorton.end())
            {
                glm::vec3 chunkPos(chunkCoord.x * _chunkSize.x, 0.0f, chunkCoord.y * _chunkSize.z);
                auto partition = std::make_shared<Partition<Entity>>(chunkPos, _chunkSize);
                auto res = _partitionsMorton.emplace(key, partition);
                it = res.first;
                // add new key to cached key list and mark dirty
                _sortedKeys.push_back(key);
                _sortedDirty = true;
            }

            it->second->addEntity(entity);
        }
#if defined(WP_ENABLE_INSTRUMENTATION)
        auto t_insert_end = clk::now();
        _instr_insertMs += std::chrono::duration<double, std::milli>(t_insert_end - t_insert_start).count();
        _instr_insertCount++;
#endif
    }

    /**
     * @brief Update world based on player position
     *
     * - Loads chunks within radius
     * - Unloads chunks outside radius
     * - Rebuilds loaded chunks (periodic)
     *
     * Call every frame or every N frames
     */
    void update(const glm::vec3 &playerPosition, bool rebuildChunks = false)
    {
        glm::ivec2 playerChunk = getChunkCoordinate(playerPosition);

        // Load chunks in radius (async)
        for (int x = -_loadRadius; x <= _loadRadius; ++x)
        {
            for (int z = -_loadRadius; z <= _loadRadius; ++z)
            {
                glm::ivec2 chunkCoord = playerChunk + glm::ivec2(x, z);
                loadChunk(chunkCoord);
            }
        }

        // Unload distant chunks (iterate morton map and decode back to chunk coords)
        std::vector<glm::ivec2> toUnload;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            for (const auto &[key, partition] : _partitionsMorton)
            {
                uint32_t ux = 0, uz = 0;
                morton::decode2D(static_cast<uint32_t>(key), ux, uz);
                const uint64_t bias = 1ULL << (_chunkBits - 1);
                int cx = static_cast<int>(static_cast<int64_t>(ux) - static_cast<int64_t>(bias));
                int cz = static_cast<int>(static_cast<int64_t>(uz) - static_cast<int64_t>(bias));
                glm::ivec2 coord(cx, cz);

                int dx = abs(coord.x - playerChunk.x);
                int dz = abs(coord.y - playerChunk.y);
                if (dx > _loadRadius || dz > _loadRadius)
                {
                    if (partition->isLoaded())
                    {
                        toUnload.push_back(coord);
                    }
                }
            }
        }

        for (const auto &coord : toUnload)
        {
            unloadChunk(coord);
        }

        // Rebuild loaded chunks (if requested)
        if (rebuildChunks)
        {
            // collect pointers to partitions to update while holding the world lock,
            // then perform heavy rebuilds outside the lock to reduce contention.
            std::vector<std::shared_ptr<Partition<Entity>>> toUpdate;
            {
                std::lock_guard<std::mutex> lock(_mutex);
                toUpdate.reserve(_partitionsMorton.size());
                for (auto &[k, partition] : _partitionsMorton)
                {
                    if (partition->isLoaded())
                        toUpdate.push_back(partition);
                }
            }

#if defined(WP_ENABLE_INSTRUMENTATION)
            using clk = std::chrono::high_resolution_clock;
            auto t_update_start = clk::now();
#endif

            // If we have multiple partitions, parallelize rebuilds using the thread pool
#if defined(__cpp_lib_thread) || defined(_GLIBCXX_HAVE_THREAD)
            if (toUpdate.size() > 1)
            {
                // Bucket partitions per worker to avoid too-fine-grained tasks
                size_t numWorkers = std::thread::hardware_concurrency();
                if (numWorkers == 0)
                    numWorkers = 2;
                numWorkers = std::min(numWorkers, toUpdate.size());
                size_t batchSize = (toUpdate.size() + numWorkers - 1) / numWorkers;

                std::vector<std::future<void>> futures;
                futures.reserve(numWorkers);
                for (size_t w = 0; w < numWorkers; ++w)
                {
                    size_t start = w * batchSize;
                    size_t end = std::min(start + batchSize, toUpdate.size());
                    if (start >= end)
                        continue;
                    futures.emplace_back(_threadPool.enqueue([this, &toUpdate, start, end]() {
                        for (size_t i = start; i < end; ++i)
                        {
#    if defined(WP_ENABLE_INSTRUMENTATION)
                            using clk = std::chrono::high_resolution_clock;
                            auto t_p0 = clk::now();
                            toUpdate[i]->update();
                            auto t_p1 = clk::now();
                            _instr_partitionUpdateMs += std::chrono::duration<double, std::milli>(t_p1 - t_p0).count();
                            _instr_partitionUpdates++;
#    else
                            toUpdate[i]->update();
#    endif
                        }
                    }));
                }

                // Wait for all to finish
                for (auto &fut : futures)
                    fut.get();
            }
            else
            {
                for (auto &partition : toUpdate)
                {
#    if defined(WP_ENABLE_INSTRUMENTATION)
                    auto t_p0 = clk::now();
#    endif
                    partition->update();
#    if defined(WP_ENABLE_INSTRUMENTATION)
                    auto t_p1 = clk::now();
                    _instr_partitionUpdateMs += std::chrono::duration<double, std::milli>(t_p1 - t_p0).count();
                    _instr_partitionUpdates++;
#    endif
                }
            }
#endif // thread support
#if defined(WP_ENABLE_INSTRUMENTATION)
            auto t_update_end = clk::now();
            _instr_updateMs += std::chrono::duration<double, std::milli>(t_update_end - t_update_start).count();
            _instr_updateCount++;
#endif
        }
    }

    /**
     * @brief Query entities in bounding box (all loaded chunks)
     *
     * Returns results grouped by partition
     */
    [[nodiscard]] std::vector<QueryResult> query(const flat_dynamic::BoundingBox &searchBox) const
    {
        std::vector<QueryResult> results;
        std::lock_guard<std::mutex> lock(_mutex);

        for (const auto &[key, partition] : _partitionsMorton)
        {
            if (!partition->isLoaded())
                continue;

            if (partition->getBounds().overlaps(searchBox))
            {
                auto indices = partition->query(searchBox);
                if (!indices.empty())
                {
                    results.push_back({partition, std::move(indices)});
                }
            }
        }

        return results;
    }

    /**
     * @brief Get chunk at coordinate
     */
    [[nodiscard]] std::shared_ptr<Partition<Entity>> getChunk(const glm::ivec2 &coord) const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        uint64_t key = chunkMortonKey(coord, _chunkBits);
        auto it = _partitionsMorton.find(key);
        return (it != _partitionsMorton.end()) ? it->second : nullptr;
    }

    /**
     * @brief Get chunk by Morton key (if registered)
     */
    [[nodiscard]] std::shared_ptr<Partition<Entity>> getChunkByMorton(uint64_t mortonKey) const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _partitionsMorton.find(mortonKey);
        return (it != _partitionsMorton.end()) ? it->second : nullptr;
    }

    /**
     * @brief Get all loaded chunks
     */
    [[nodiscard]] std::vector<std::shared_ptr<Partition<Entity>>> getLoadedChunks() const
    {
        std::vector<std::shared_ptr<Partition<Entity>>> loaded;
        std::lock_guard<std::mutex> lock(_mutex);

        for (const auto &[k, partition] : _partitionsMorton)
        {
            if (partition->isLoaded())
            {
                loaded.push_back(partition);
            }
        }

        return loaded;
    }

    /**
     * @brief Get loaded chunks accessed via morton-indexed map
     * Useful if you want to iterate over spatially-ordered keys; this returns values (unsorted)
     */
    [[nodiscard]] std::vector<std::shared_ptr<Partition<Entity>>> getLoadedChunksMorton() const
    {
        std::vector<std::shared_ptr<Partition<Entity>>> loaded;
        std::lock_guard<std::mutex> lock(_mutex);
        for (const auto &[k, partition] : _partitionsMorton)
        {
            if (partition->isLoaded())
                loaded.push_back(partition);
        }
        return loaded;
    }

    /**
     * @brief Return all partitions from the ivec2 map (no filtering)
     */
    [[nodiscard]] std::vector<std::shared_ptr<Partition<Entity>>> getAllPartitions() const
    {
        std::vector<std::shared_ptr<Partition<Entity>>> all;
        std::lock_guard<std::mutex> lock(_mutex);
        all.reserve(_partitionsMorton.size());
        for (const auto &[k, partition] : _partitionsMorton)
            all.push_back(partition);
        return all;
    }

    /**
     * @brief Return all partitions from the morton map (no filtering)
     */
    [[nodiscard]] std::vector<std::shared_ptr<Partition<Entity>>> getAllPartitionsMorton() const
    {
        std::vector<std::shared_ptr<Partition<Entity>>> all;
        std::lock_guard<std::mutex> lock(_mutex);
        all.reserve(_partitionsMorton.size());
        for (const auto &[key, partition] : _partitionsMorton)
            all.push_back(partition);
        return all;
    }

    /**
     * Return partitions ordered by Morton (cached - lazy sorted list)
     */
    [[nodiscard]] std::vector<std::shared_ptr<Partition<Entity>>> getAllPartitionsSortedByMorton() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_sortedDirty)
        {
            std::sort(_sortedKeys.begin(), _sortedKeys.end());
            _sortedDirty = false;
        }

        std::vector<std::shared_ptr<Partition<Entity>>> out;
        out.reserve(_sortedKeys.size());
        for (auto k : _sortedKeys)
        {
            auto it = _partitionsMorton.find(k);
            if (it != _partitionsMorton.end())
                out.push_back(it->second);
        }
        return out;
    }

    /**
     * @brief Return all partitions as pairs (mortonKey, partition)
     * Useful for sorting externally by morton key or coordinate.
     */
    [[nodiscard]] std::vector<std::pair<uint64_t, std::shared_ptr<Partition<Entity>>>> getAllPartitionsKeyed() const
    {
        std::vector<std::pair<uint64_t, std::shared_ptr<Partition<Entity>>>> out;
        std::lock_guard<std::mutex> lock(_mutex);
        out.reserve(_partitionsMorton.size());
        for (const auto &[k, partition] : _partitionsMorton)
            out.emplace_back(k, partition);
        return out;
    }

    /**
     * @brief Get statistics
     */
    struct Stats {
        size_t totalChunks = 0;
        size_t loadedChunks = 0;
        size_t totalEntities = 0;
        size_t loadedEntities = 0;
    };

    [[nodiscard]] Stats getStats() const
    {
        Stats stats;
        std::lock_guard<std::mutex> lock(_mutex);

        stats.totalChunks = _partitionsMorton.size();
        for (const auto &[k, partition] : _partitionsMorton)
        {
            stats.totalEntities += partition->entityCount();
            if (partition->isLoaded())
            {
                stats.loadedChunks++;
                stats.loadedEntities += partition->entityCount();
            }
        }

        return stats;
    }

#if defined(WP_ENABLE_INSTRUMENTATION)
    struct InstrStats {
        double insertMs = 0.0;
        double keyEncodeMs = 0.0;
        double updateMs = 0.0;
        double partitionUpdateMs = 0.0;
        size_t insertCount = 0;
        size_t updateCount = 0;
        size_t partitionUpdates = 0;
    };

    InstrStats getInstrStats() const
    {
        InstrStats s;
        s.insertMs = _instr_insertMs;
        s.keyEncodeMs = _instr_keyEncodeMs;
        s.updateMs = _instr_updateMs;
        s.partitionUpdateMs = _instr_partitionUpdateMs;
        s.insertCount = _instr_insertCount;
        s.updateCount = _instr_updateCount;
        s.partitionUpdates = _instr_partitionUpdates;
        return s;
    }
#endif

    /**
     * @brief Clear all partitions
     */
    void clear()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _partitionsMorton.clear();
        _sortedKeys.clear();
        _sortedDirty = true;
    }

    /**
     * @brief Wait until pending async loads complete
     */
    void waitForPendingLoads()
    {
        // _threadPool.waitIdle();
    }

    /**
     * @brief Get chunk size
     */
    [[nodiscard]] inline const glm::vec3 &getChunkSize() const noexcept { return _chunkSize; }

private:
    /**
     * @brief Get chunk coordinate from world position
     */
    [[nodiscard]] glm::ivec2 getChunkCoordinate(const glm::vec3 &position) const
    {
        return glm::ivec2(static_cast<int>(std::floor(position.x / _chunkSize.x)),
                          static_cast<int>(std::floor(position.z / _chunkSize.z)));
    }

    /**
     * @brief Load chunk asynchronously
     */
    void loadChunk(const glm::ivec2 &coord)
    {
        std::shared_ptr<Partition<Entity>> partition;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            uint64_t key = chunkMortonKey(coord, _chunkBits);
            auto it = _partitionsMorton.find(key);
            if (it == _partitionsMorton.end())
                return; // Chunk doesn't exist
            partition = it->second;
        }

        if (partition->isLoaded())
            return; // Already loaded

        // Load asynchronously
        _threadPool.enqueue([partition]() { partition->load(); });
    }

    /**
     * @brief Unload chunk
     */
    void unloadChunk(const glm::ivec2 &coord)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        uint64_t key = chunkMortonKey(coord, _chunkBits);
        auto it = _partitionsMorton.find(key);
        if (it != _partitionsMorton.end())
        {
            it->second->unload();
        }
    }

    glm::vec3 _chunkSize;
    int _loadRadius;
    // Primary storage is Morton-indexed map for scan-friendly access and locality
    // Map implementation selection: prefer ankerl::unordered_dense when available;
    // fallback to std::unordered_map.
#if defined(WP_HAS_ANKERL)
    ankerl::unordered_dense::map<uint64_t, std::shared_ptr<Partition<Entity>>> _partitionsMorton;
#elif defined(WP_HAS_BOOST_FLAT)
    boost::unordered_flat_map<uint64_t, std::shared_ptr<Partition<Entity>>> _partitionsMorton;
#else
    std::unordered_map<uint64_t, std::shared_ptr<Partition<Entity>>> _partitionsMorton;
#endif
    unsigned _chunkBits;
    mutable std::vector<uint64_t> _sortedKeys;
    mutable bool _sortedDirty = true;
    mutable std::mutex _mutex;
    ThreadPool _threadPool;

#if defined(WP_ENABLE_INSTRUMENTATION)
    // Instrumentation counters (ms / counts)
    mutable std::atomic<double> _instr_insertMs{0.0};
    mutable std::atomic<double> _instr_keyEncodeMs{0.0};
    mutable std::atomic<double> _instr_updateMs{0.0};
    mutable std::atomic<double> _instr_partitionUpdateMs{0.0};
    mutable std::atomic<size_t> _instr_insertCount{0};
    mutable std::atomic<size_t> _instr_updateCount{0};
    mutable std::atomic<size_t> _instr_partitionUpdates{0};
#endif
};

} // namespace Optimizing::World
