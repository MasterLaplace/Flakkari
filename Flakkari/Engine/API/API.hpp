/*
** EPITECH PROJECT, 2025
** Flakkari
** File description:
** API.hpp - Main header for the Engine API abstraction layer
**
** This API provides a unified interface for different ECS backends,
** allowing Flakkari to work with multiple engine implementations.
*/

#ifndef FLAKKARI_ENGINE_API_HPP_
#define FLAKKARI_ENGINE_API_HPP_

// Core API interfaces
#include "IEntity.hpp"
#include "IRegistry.hpp"
#include "EngineFactory.hpp"

// Flakkari native ECS wrapper
#include "EntityComponentSystem/FlakkariEntity.hpp"
#include "EntityComponentSystem/FlakkariRegistry.hpp"

// EngineSquared wrapper (conditionally included)
#ifdef FLAKKARI_HAS_ENGINESQUARED
#include "EngineSquared/Entity.hpp"
#include "EngineSquared/Registry.hpp"
#endif

#endif /* !FLAKKARI_ENGINE_API_HPP_ */
