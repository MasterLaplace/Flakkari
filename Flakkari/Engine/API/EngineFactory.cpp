/*
** EPITECH PROJECT, 2025
** Flakkari
** File description:
** EngineFactory - Implementation
*/

#include "EngineFactory.hpp"
#include "EntityComponentSystem/FlakkariRegistry.hpp"

// Conditionally include EngineSquared if available
#ifdef FLAKKARI_HAS_ENGINESQUARED
#    include "EngineSquared/Registry.hpp"
#endif

#include <algorithm>
#include <cctype>

namespace Flakkari::Engine::API {

std::unique_ptr<IRegistry> EngineFactory::createRegistry(EngineType type)
{
    switch (type)
    {
    case EngineType::Flakkari: return std::make_unique<FlakkariRegistry>();

    case EngineType::EngineSquared:
#ifdef FLAKKARI_HAS_ENGINESQUARED
        return std::make_unique<ESRegistry>();
#else
        throw std::runtime_error("EngineSquared is not available. Compile with FLAKKARI_HAS_ENGINESQUARED defined.");
#endif

    case EngineType::Auto:
        // Try EngineSquared first if available, otherwise fall back to Flakkari
#ifdef FLAKKARI_HAS_ENGINESQUARED
        return std::make_unique<ESRegistry>();
#else
        return std::make_unique<FlakkariRegistry>();
#endif

    default: throw std::runtime_error("Unknown engine type");
    }
}

std::unique_ptr<IRegistry> EngineFactory::createRegistry(const std::string &engineName)
{
    return createRegistry(parseEngineType(engineName));
}

std::string EngineFactory::getEngineName(EngineType type)
{
    switch (type)
    {
    case EngineType::Flakkari: return "Flakkari";
    case EngineType::EngineSquared: return "EngineSquared";
    case EngineType::Auto: return "Auto";
    default: return "Unknown";
    }
}

EngineType EngineFactory::parseEngineType(const std::string &name)
{
    // Convert to lowercase for case-insensitive comparison
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (lowerName == "flakkari" || lowerName == "native" || lowerName == "default")
    {
        return EngineType::Flakkari;
    }
    else if (lowerName == "enginesquared" || lowerName == "engine_squared" || lowerName == "engine-squared" ||
             lowerName == "es")
    {
        return EngineType::EngineSquared;
    }
    else if (lowerName == "auto" || lowerName == "automatic")
    {
        return EngineType::Auto;
    }

    throw std::runtime_error("Unknown engine type: " + name + ". Valid options are: Flakkari, EngineSquared, ES, Auto");
}

bool EngineFactory::isEngineAvailable(EngineType type)
{
    switch (type)
    {
    case EngineType::Flakkari: return true; // Always available

    case EngineType::EngineSquared:
#ifdef FLAKKARI_HAS_ENGINESQUARED
        return true;
#else
        return false;
#endif

    case EngineType::Auto: return true; // Auto always has a fallback

    default: return false;
    }
}

} // namespace Flakkari::Engine::API
