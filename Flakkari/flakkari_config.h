/**************************************************************************
 * Flakkari Library v0.8.0
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
 * @file flakkari_config.h
 * @brief Compile-Time Configuration Parameters for Flakkari.
 *
 * @author @MasterLaplace
 * @version 0.8.0
 * @date 2024-01-06
 **************************************************************************/

// clang-format off
#ifndef FLAKKARI_CONFIG_H_
    #define FLAKKARI_CONFIG_H_

#ifdef __cplusplus
    #include <utility>
    #include <type_traits>

    #include <cstddef>
    #include <cstdint>
#else
    #include <stddef.h>
    #include <stdint.h>
#endif


#ifndef LAPLACE_CONFIG_UTILS
    #define LAPLACE_CONFIG_UTILS

////////////////////////////////////////////////////////////
// Define shared portable macros for various compilers
////////////////////////////////////////////////////////////
#define LPL_NEED_COMMA struct _
#define LPL_ATTRIBUTE(key) __attribute__((key))
#define LPL_UNUSED_ATTRIBUTE LPL_ATTRIBUTE(unused)
#define LPL_UNUSED(x) (void)(x)

////////////////////////////////////////////////////////////
// Define portable NULL pointer using C++11 nullptr keyword
////////////////////////////////////////////////////////////
#if defined(__cplusplus) && __cplusplus >= 201103L
    #define lpl_nullptr nullptr
#elif !defined(NULL)
    #define lpl_nullptr ((void*)0)
#else
    #define lpl_nullptr NULL
#endif

////////////////////////////////////////////////////////////
// Define boolean type and values
////////////////////////////////////////////////////////////
#if !defined(__bool_true_false_are_defined) && !defined(__cplusplus)
    #define bool _Bool
    #define true 1
    #define false 0
    #define __bool_true_false_are_defined 1
#endif

#if defined __GNUC__ && defined __GNUC_MINOR__
# define __GNUC_PREREQ(maj, min) \
    ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#elif !defined(__GNUC_PREREQ)
# define __GNUC_PREREQ(maj, min) 0
#endif

////////////////////////////////////////////////////////////
// Define a portable way for packing structures
////////////////////////////////////////////////////////////
/** Usage:
 * @example
 * LPL_PACKED(struct MyStruct
 * {
 *     int a;
 *     char b;
 *     ...
 * });
\**********************************************************/
#if defined(_MSC_VER) || defined(_MSVC_LANG)
    #define LPL_PACKED( __Declaration__ ) __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
    #define LPL_PACKED_START __pragma(pack(push, 1))
    #define LPL_PACKED_END   __pragma(pack(pop))
#elif defined(__GNUC__) || defined(__GNUG__)
    #define LPL_PACKED( __Declaration__ ) __Declaration__ __attribute__((__packed__))
    #define LPL_PACKED_START _Pragma("pack(1)")
    #define LPL_PACKED_END   _Pragma("pack()")
#else
    #define LPL_PACKED( __Declaration__ ) __Declaration__
    #define LPL_PACKED_START
    #define LPL_PACKED_END
#endif

////////////////////////////////////////////////////////////
// Helper macro to convert a macro to a string
////////////////////////////////////////////////////////////
#define LPL_STRINGIFY(x) #x
#define LPL_TOSTRING(x) LPL_STRINGIFY(x)

#endif /* !LAPLACE_CONFIG_UTILS */


#ifndef FLAKKARI_DISTRIBUTION_H_
    #define FLAKKARI_DISTRIBUTION_H_

////////////////////////////////////////////////////////////
// Identify the Compiler
////////////////////////////////////////////////////////////
#if defined(_MSC_VER) || defined(_MSVC_LANG)
    #define FLAKKARI_COMPILER_MSVC
    #define FLAKKARI_COMPILER_STRING "MSVC"
#elif defined(__GNUC__) || defined(__GNUG__)
    #define FLAKKARI_COMPILER_GCC
    #define FLAKKARI_COMPILER_STRING "GCC"
#elif defined(__clang__) || defined(__llvm__)
    #define FLAKKARI_COMPILER_CLANG
    #define FLAKKARI_COMPILER_STRING "Clang"
#elif defined(__MINGW32__) || defined(__MINGW64__)
    #define FLAKKARI_COMPILER_MINGW
    #define FLAKKARI_COMPILER_STRING "MinGW"
#elif defined(__CYGWIN__)
    #define FLAKKARI_COMPILER_CYGWIN
    #define FLAKKARI_COMPILER_STRING "Cygwin"
#else
    #error [Config@Distribution]: This compiler is not supported by FLAKKARI library.
#endif


////////////////////////////////////////////////////////////
// Identify the Operating System
////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(__WIN32__) || defined(FLAKKARI_COMPILER_MINGW) || defined(FLAKKARI_COMPILER_CYGWIN)

    #define FLAKKARI_SYSTEM_WINDOWS
    #define FLAKKARI_SYSTEM_STRING "Windows"

// Android is based on the Linux FLAKKARI, so it has to appear before Linux
#elif defined(__ANDROID__)

    #define FLAKKARI_SYSTEM_ANDROID
    #define FLAKKARI_SYSTEM_STRING "Android"

#elif defined(linux) || defined(__linux)

    #define FLAKKARI_SYSTEM_LINUX
    #define FLAKKARI_SYSTEM_STRING "Linux"

#elif defined(__unix) || defined(__unix__)

    #define FLAKKARI_SYSTEM_UNIX
    #define FLAKKARI_SYSTEM_STRING "Unix"

#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)

    #define FLAKKARI_SYSTEM_MACOS
    #define FLAKKARI_SYSTEM_STRING "MacOS"

#elif defined(__FreeBSD__) || defined(__FreeBSD_FLAKKARI__)

    #define FLAKKARI_SYSTEM_FREEBSD
    #define FLAKKARI_SYSTEM_STRING "FreeBSD"

#elif defined(LAPLACE_KERNEL_PANIC)

    #define FLAKKARI_SYSTEM_KERNEL
    #define FLAKKARI_SYSTEM_STRING "Laplace Kernel"

#else
    #error [Config@Distribution]: This operating system is not supported by FLAKKARI library.
#endif

#ifdef __cplusplus
    #define FLAKKARI_EXTERN_C extern "C"

    #if __cplusplus >= 202203L
        #define FLAKKARI_CPP23(_) _
        #define FLAKKARI_CPP20(_) _
        #define FLAKKARI_CPP17(_) _
        #define FLAKKARI_CPP14(_) _
        #define FLAKKARI_CPP11(_) _
        #define FLAKKARI_CPP99(_) _
    #elif __cplusplus >= 202002L
        #define FLAKKARI_CPP23(_)
        #define FLAKKARI_CPP20(_) _
        #define FLAKKARI_CPP17(_) _
        #define FLAKKARI_CPP14(_) _
        #define FLAKKARI_CPP11(_) _
        #define FLAKKARI_CPP99(_) _
    #elif __cplusplus >= 201703L
        #define FLAKKARI_CPP23(_)
        #define FLAKKARI_CPP20(_)
        #define FLAKKARI_CPP17(_) _
        #define FLAKKARI_CPP14(_) _
        #define FLAKKARI_CPP11(_) _
        #define FLAKKARI_CPP99(_) _
    #elif __cplusplus >= 201402L
        #define FLAKKARI_CPP23(_)
        #define FLAKKARI_CPP20(_)
        #define FLAKKARI_CPP17(_)
        #define FLAKKARI_CPP14(_) _
        #define FLAKKARI_CPP11(_) _
        #define FLAKKARI_CPP99(_) _
    #elif __cplusplus >= 201103L
        #define FLAKKARI_CPP23(_)
        #define FLAKKARI_CPP20(_)
        #define FLAKKARI_CPP17(_)
        #define FLAKKARI_CPP14(_)
        #define FLAKKARI_CPP11(_) _
        #define FLAKKARI_CPP99(_) _
    #elif __cplusplus >= 199711L
        #define FLAKKARI_CPP23(_)
        #define FLAKKARI_CPP20(_)
        #define FLAKKARI_CPP17(_)
        #define FLAKKARI_CPP14(_)
        #define FLAKKARI_CPP11(_)
        #define FLAKKARI_CPP99(_) _
    #else
        #define FLAKKARI_CPP23(_)
        #define FLAKKARI_CPP20(_)
        #define FLAKKARI_CPP17(_)
        #define FLAKKARI_CPP14(_)
        #define FLAKKARI_CPP11(_)
        #define FLAKKARI_CPP99(_)
    #endif

    ////////////////////////////////////////////////////////////
    // Define a macro to handle cpp features compatibility
    ////////////////////////////////////////////////////////////
    /** Usage:
     * @example
     * void func() FLAKKARI_CPP14([[deprecated]]);
     *
     * @example
     * void func() FLAKKARI_CPP([[deprecated]], 14);
    \**********************************************************/
    #define FLAKKARI_CPP(_, version) FLAKKARI_CPP##version(_)

#else
    #define FLAKKARI_EXTERN_C extern

    #define FLAKKARI_CPP23(_)
    #define FLAKKARI_CPP20(_)
    #define FLAKKARI_CPP17(_)
    #define FLAKKARI_CPP14(_)
    #define FLAKKARI_CPP11(_)
    #define FLAKKARI_CPP99(_)
    #define FLAKKARI_CPP(_, version)
#endif

////////////////////////////////////////////////////////////
// Define helpers to create portable import / export macros for each module
////////////////////////////////////////////////////////////
#if defined(FLAKKARI_SYSTEM_WINDOWS)

    // Windows compilers need specific (and different) keywords for export and import
    #define FLAKKARI_API_EXPORT extern "C" __declspec(dllexport)
    #define FLAKKARI_API_IMPORT FLAKKARI_EXTERN_C __declspec(dllimport)

    // For Visual C++ compilers, we also need to turn off this annoying C4251 warning
    #ifdef _MSC_VER

        #pragma warning(disable : 4251)

    #endif

#else // Linux, FreeBSD, Mac OS X

    #if __GNUC__ >= 4

        // GCC 4 has special keywords for showing/hidding symbols,
        // the same keyword is used for both importing and exporting
        #define FLAKKARI_API_EXPORT extern "C" __attribute__ ((__visibility__ ("default")))
        #define FLAKKARI_API_IMPORT FLAKKARI_EXTERN_C __attribute__ ((__visibility__ ("default")))

    #else

        // GCC < 4 has no mechanism to explicitely hide symbols, everything's exported
        #define FLAKKARI_API_EXPORT extern "C"
        #define FLAKKARI_API_IMPORT FLAKKARI_EXTERN_C

    #endif

#endif


#ifdef FLAKKARI_SYSTEM_WINDOWS

    // Windows compilers use a different name for the main function
    #define FLAKKARI_GUI_MAIN(hInstance, hPrevInstance, lpCmdLine, nCmdShow) WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
    #define FLAKKARI_MAIN(ac, av, env) main(int ac, char *av[], char *env[])

#elif defined(FLAKKARI_SYSTEM_ANDROID)

    // Android doesn't need a main function
    #define FLAKKARI_GUI_MAIN(app) android_main(struct android_app* app)
    #define FLAKKARI_MAIN

#elif defined(FLAKKARI_SYSTEM_MACOS)

    // On MacOS X, we use a Unix main function
    #define FLAKKARI_MAIN(ac, av, env, apple) main(int ac, char *av[], char *env[], char *apple[])

#else

    // Other platforms should use the standard main function
    #define FLAKKARI_MAIN(ac, av, env) main(int ac, char *av[], char *env[])
#endif

////////////////////////////////////////////////////////////
// Define a portable debug macro
////////////////////////////////////////////////////////////
#if (defined(_DEBUG) || defined(DEBUG)) && !defined(NDEBUG)

    #define FLAKKARI_DEBUG
    #define FLAKKARI_DEBUG_STRING "Debug"

#else
    #define FLAKKARI_DEBUG_STRING "Release"
#endif

////////////////////////////////////////////////////////////
// Define a portable way to declare a function as deprecated
////////////////////////////////////////////////////////////
/** Usage:
 * @example "for functions"
 *   FLAKKARI_DEPRECATED void func();
 * @example "for structs"
 *   struct FLAKKARI_DEPRECATED MyStruct { ... };
 * @example "for enums"
 *   enum FLAKKARI_DEPRECATED MyEnum { ... };
 *   enum MyEnum {
 *        MyEnum1 = 0,
 *        MyEnum2 FLAKKARI_DEPRECATED,
 *        MyEnum3
 *   };
 * @example "for classes"
 *   class FLAKKARI_DEPRECATED MyClass { ... };
\**********************************************************/
#ifdef FLAKKARI_DISABLE_DEPRECATION

    #define FLAKKARI_DEPRECATED
    #define FLAKKARI_DEPRECATED_MSG(message)
    #define FLAKKARI_DEPRECATED_VMSG(version, message)

#elif defined(__cplusplus) && (__cplusplus >= 201402)

    #define FLAKKARI_DEPRECATED [[deprecated]]

    #if (__cplusplus >= 201402) && (__cplusplus < 201703)

        #define FLAKKARI_DEPRECATED_MSG(message) [[deprecated(message)]]
        #define FLAKKARI_DEPRECATED_VMSG(version, message) \
            [[deprecated("since " # version ". " message)]]

    #else
        #define FLAKKARI_DEPRECATED_MSG(message) [[deprecated]]
        #define FLAKKARI_DEPRECATED_VMSG(version, message) [[deprecated]]
    #endif

#elif defined(FLAKKARI_COMPILER_MSVC) && (_MSC_VER >= 1400)

    #define FLAKKARI_DEPRECATED __declspec(deprecated)

    #if (_MSC_VER >= 1900)

        #define FLAKKARI_DEPRECATED_MSG(message) __declspec(deprecated(message))
        #define FLAKKARI_DEPRECATED_VMSG(version, message) \
            __declspec(deprecated("since " # version ". " message))

    #else
        #define FLAKKARI_DEPRECATED_MSG(message) __declspec(deprecated)
        #define FLAKKARI_DEPRECATED_VMSG(version, message) __declspec(deprecated)
    #endif

#elif defined(FLAKKARI_COMPILER_CLANG) && defined(__has_feature)

    #define FLAKKARI_DEPRECATED __attribute__((deprecated))

    #if __has_feature(attribute_deprecated_with_message)

        #define FLAKKARI_DEPRECATED_MSG(message) __attribute__((deprecated(message)))
        #define FLAKKARI_DEPRECATED_VMSG(version, message) \
            __attribute__((deprecated("since " # version ". " message)))

    #else
        #define FLAKKARI_DEPRECATED_MSG(message) __attribute__((deprecated))
        #define FLAKKARI_DEPRECATED_VMSG(version, message) __attribute__((deprecated))
    #endif

#elif defined(FLAKKARI_COMPILER_GCC) && defined(__GNUC__) && __GNUC_PREREQ(4, 5)

    #define FLAKKARI_DEPRECATED __attribute__((deprecated))

    #if defined(FLAKKARI_COMPILER_GCC) && defined(__GNUC__) && __GNUC_PREREQ(4, 9)

        #define FLAKKARI_DEPRECATED_MSG(message) __attribute__((deprecated(message)))
        #define FLAKKARI_DEPRECATED_VMSG(version, message) \
            __attribute__((deprecated("since " # version ". " message)))

    #else
        #define FLAKKARI_DEPRECATED_MSG(message) __attribute__((deprecated))
        #define FLAKKARI_DEPRECATED_VMSG(version, message) __attribute__((deprecated))
    #endif

#else

    #pragma message("WARNING: FLAKKARI_DEPRECATED not supported on this compiler")
    #define FLAKKARI_DEPRECATED
    #define FLAKKARI_DEPRECATED_MSG(message)
    #define FLAKKARI_DEPRECATED_VMSG(version, message)
#endif

#endif /* !FLAKKARI_DISTRIBUTION_H_ */


#ifndef FLAKKARI_VERSION_H_
    #define FLAKKARI_VERSION_H_

////////////////////////////////////////////////////////////
// Define the FLAKKARI version
////////////////////////////////////////////////////////////
#ifdef FLAG_VERSION_MAJOR
    #define FLAKKARI_VERSION_MAJOR FLAG_VERSION_MAJOR
#else
    #define FLAKKARI_VERSION_MAJOR 0
#endif

#ifdef FLAG_VERSION_MINOR
    #define FLAKKARI_VERSION_MINOR FLAG_VERSION_MINOR
#else
    #define FLAKKARI_VERSION_MINOR 8
#endif

#ifdef FLAG_VERSION_PATCH
    #define FLAKKARI_VERSION_PATCH FLAG_VERSION_PATCH
#else
    #define FLAKKARI_VERSION_PATCH 0
#endif

#ifdef FLAG_VERSION_TWEAK
    #define FLAKKARI_VERSION_TWEAK FLAG_VERSION_TWEAK
#else
    #define FLAKKARI_VERSION_TWEAK 0
#endif

////////////////////////////////////////////////////////////
// Define the FLAKKARI version number
////////////////////////////////////////////////////////////
#define FLAKKARI_VERSION \
        (FLAKKARI_VERSION_MAJOR * 1000000 + \
        FLAKKARI_VERSION_MINOR * 10000 + \
        FLAKKARI_VERSION_PATCH * 100 + \
        FLAKKARI_VERSION_TWEAK)

#define FLAKKARI_PREREQ_VERSION(maj, min, pat) (FLAKKARI_VERSION >= (maj * 1000000 + min * 10000 + pat * 100))

////////////////////////////////////////////////////////////
// Define the FLAKKARI version concatenated
////////////////////////////////////////////////////////////
#define FLAKKARI_VERSION_CCT FLAKKARI_VERSION_MAJOR##_##FLAKKARI_VERSION_MINOR##_##FLAKKARI_VERSION_PATCH##_##FLAKKARI_VERSION_TWEAK


////////////////////////////////////////////////////////////
// Define the FLAKKARI version string
////////////////////////////////////////////////////////////
#define FLAKKARI_VERSION_STRING \
        LPL_TOSTRING(FLAKKARI_VERSION_MAJOR) "." \
        LPL_TOSTRING(FLAKKARI_VERSION_MINOR) "." \
        LPL_TOSTRING(FLAKKARI_VERSION_PATCH) "." \
        LPL_TOSTRING(FLAKKARI_VERSION_TWEAK)

#endif /* !FLAKKARI_VERSION_H_ */


////////////////////////////////////////////////////////////
// Compile-Time Configuration Parameters
////////////////////////////////////////////////////////////
#define FLAKKARI_CONFIG_STRING \
        "FLAKKARI_VERSION=" FLAKKARI_VERSION_STRING "\n" \
        "FLAKKARI_SYSTEM=" FLAKKARI_SYSTEM_STRING "\n" \
        "FLAKKARI_COMPILER=" FLAKKARI_COMPILER_STRING "\n" \
        "FLAKKARI_DEBUG=" FLAKKARI_DEBUG_STRING "\n"

#endif /* !FLAKKARI_CONFIG_H_ */
// clang-format on
