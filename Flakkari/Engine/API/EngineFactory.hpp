/*
** EPITECH PROJECT, 2025
** Flakkari
** File description:
** EngineFactory - Factory for creating ECS engine instances
*/

#ifndef ENGINEFACTORY_HPP_
#define ENGINEFACTORY_HPP_

#include "IRegistry.hpp"
#include <memory>
#include <string>
#include <stdexcept>

// Forward declarations - actual includes happen in the .cpp or when using specific engine
namespace Flakkari::Engine::API {
class FlakkariRegistry;
class ESRegistry;
} // namespace Flakkari::Engine::API

namespace Flakkari::Engine::API {

/**
 * @enum EngineType
 * @brief Enumeration of supported ECS engine backends.
 */
enum class EngineType : uint8_t {
    Flakkari,      ///< Native Flakkari ECS (default)
    EngineSquared, ///< EngineSquared ECS backend
    Auto           ///< Automatically select based on availability
};

/**
 * @class EngineFactory
 * @brief Factory class for creating ECS engine instances.
 *
 * This factory allows Flakkari to instantiate different ECS backends
 * based on configuration or runtime selection. It provides a unified
 * way to create Registry instances without coupling the server code
 * to a specific ECS implementation.
 *
 * @example
 * ```cpp
 * // Create a Flakkari ECS registry
 * auto registry = EngineFactory::createRegistry(EngineType::Flakkari);
 *
 * // Create an EngineSquared registry
 * auto esRegistry = EngineFactory::createRegistry(EngineType::EngineSquared);
 *
 * // Create from string configuration
 * auto configRegistry = EngineFactory::createRegistry("EngineSquared");
 * ```
 */
class EngineFactory {
public:
    EngineFactory() = delete;
    ~EngineFactory() = delete;

    /**
     * @brief Create a registry instance for the specified engine type.
     * @param type The type of ECS engine to use.
     * @return A unique pointer to the created IRegistry.
     * @throws std::runtime_error if the engine type is not supported.
     */
    static std::unique_ptr<IRegistry> createRegistry(EngineType type = EngineType::Flakkari);

    /**
     * @brief Create a registry instance from a string identifier.
     * @param engineName The name of the engine ("Flakkari", "EngineSquared", "ES", "auto").
     * @return A unique pointer to the created IRegistry.
     * @throws std::runtime_error if the engine name is not recognized.
     */
    static std::unique_ptr<IRegistry> createRegistry(const std::string &engineName);

    /**
     * @brief Get the string name for an engine type.
     * @param type The engine type.
     * @return The string representation of the engine type.
     */
    static std::string getEngineName(EngineType type);

    /**
     * @brief Parse an engine type from a string.
     * @param name The engine name string.
     * @return The corresponding EngineType.
     * @throws std::runtime_error if the name is not recognized.
     */
    static EngineType parseEngineType(const std::string &name);

    /**
     * @brief Check if an engine type is available/compiled.
     * @param type The engine type to check.
     * @return true if the engine is available.
     */
    static bool isEngineAvailable(EngineType type);

    /**
     * @brief Get the default engine type.
     * @return The default EngineType (Flakkari).
     */
    static constexpr EngineType getDefaultEngine() { return EngineType::Flakkari; }
};

} // namespace Flakkari::Engine::API

#endif /* !ENGINEFACTORY_HPP_ */
