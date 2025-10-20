/**************************************************************************
 * Flakkari Library v0.7.1
 *
 * Flakkari is a server/client game library that is designed to be fast and easy
 * to use.
 *
 * This file is part of the Flakkari project that is under Anti-NN License.
 * https://github.com/MasterLaplace/Anti-NN_LICENSE
 * Copyright © 2024 by @MasterLaplace, All rights reserved.
 *
 * Flakkari is a free software: you can redistribute it and/or modify
 * it under the terms of the Anti-NN License as published by MasterLaplace.
 * See the Anti-NN License for more details.
 *
 * @file flakkari_export.h
 * @brief Compile-Time exportation of the project path.
 *
 * @author @MasterLaplace
 * @version 0.7.1
 * @date 2025-03-26
 **************************************************************************/

// clang-format off
#ifndef LAPLACE_EXPORT_H_
    #define LAPLACE_EXPORT_H_

#ifdef __cplusplus
#include <filesystem>

#define LPL_PROJECT_SOURCE_DIR std::filesystem::current_path().string() + "/"
#else
#include <stdlib.h>
#include <string.h>

#define LPL_PROJECT_SOURCE_DIR getenv("PWD")
#endif

#endif /* !LAPLACE_EXPORT_H_ */
// clang-format on
