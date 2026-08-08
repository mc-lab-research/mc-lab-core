# ==============================================================================
# MC-LAB-CORE - Root Build Configuration
# ==============================================================================

cmake_minimum_required(VERSION 3.25...4.4)

# Platform presets select an explicit contract. Manual builds and downstream
# consumers remain unconstrained unless they opt into one deliberately.
set(
    MC_LAB_CORE_EXPECTED_TOOLCHAIN
    "AUTO"
    CACHE STRING
    "Toolchain and activated-development-environment contract"
)

set_property(
    CACHE MC_LAB_CORE_EXPECTED_TOOLCHAIN
    PROPERTY STRINGS
        AUTO
        WINDOWS_MSVC
        WINDOWS_VISUAL_STUDIO
        WINDOWS_CLANGCL
        WINDOWS_UCRT64_GCC
        WINDOWS_CLANG64
        LINUX_GCC
        LINUX_CLANG
        MACOS_APPLECLANG
)

set(
    MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE
    "AUTO"
    CACHE STRING
    "Native host architecture required by the selected platform preset"
)

set_property(
    CACHE MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE
    PROPERTY STRINGS
        AUTO
        x64
)

set(
    MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE
    "AUTO"
    CACHE STRING
    "Target architecture required by the selected platform preset"
)

set_property(
    CACHE MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE
    PROPERTY STRINGS
        AUTO
        x64
)

# Environment validation starts before project() so a missing vendor shell
# produces an actionable diagnostic before CMake attempts compiler discovery.
include(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/env_validation.cmake"
)

project(
    MC_LAB_CORE
    VERSION 0.1.0
    DESCRIPTION
        "Portable C++20 platform for executable semantics, exploration, and formal verification"
    LANGUAGES CXX
)

# Preflight validation proves that the requested vendor environment is active;
# this second phase proves that CMake actually selected the promised compiler,
# frontend, target system, architecture, generator, and distribution.
if(PROJECT_IS_TOP_LEVEL)
    mc_lab_core_validate_build_environment()
endif()

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# ==============================================================================
# Repository Build Safety
# ==============================================================================

# Repository-local build directories such as build/linux-clang-debug are
# supported. Configuring directly in the source root is not.
if(PROJECT_IS_TOP_LEVEL)
    file(
        REAL_PATH
        "${CMAKE_SOURCE_DIR}"
        mc_lab_core_source_directory
    )

    file(
        REAL_PATH
        "${CMAKE_BINARY_DIR}"
        mc_lab_core_binary_directory
    )

    if(mc_lab_core_source_directory STREQUAL mc_lab_core_binary_directory)
        message(
            FATAL_ERROR
            "In-source builds are not supported.\n"
            "\n"
            "Use one of the platform presets, for example:\n"
            "  cmake --preset windows-msvc-debug\n"
            "  cmake --preset linux-gcc-debug\n"
            "  cmake --preset macos-appleclang-debug"
        )
    endif()
endif()

# Provide a predictable fallback for manual single-configuration builds.
# Repository and CI workflows should prefer the explicit platform presets.
if(
    PROJECT_IS_TOP_LEVEL
    AND NOT CMAKE_CONFIGURATION_TYPES
    AND NOT CMAKE_BUILD_TYPE
)
    set(
        CMAKE_BUILD_TYPE
        "Debug"
        CACHE STRING
        "MC-LAB-CORE build configuration"
        FORCE
    )

    set_property(
        CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS
            Debug
            Release
            RelWithDebInfo
    )
endif()

if(PROJECT_IS_TOP_LEVEL)
    set(
        CMAKE_EXPORT_COMPILE_COMMANDS
        ON
        CACHE BOOL
        "Generate compile_commands.json when supported by the generator"
    )

    set_property(
        GLOBAL
        PROPERTY USE_FOLDERS ON
    )
endif()

# ==============================================================================
# Public Build Options
# ==============================================================================

option(
    MC_LAB_CORE_BUILD_CLI
    "Build the MC-LAB-CORE command-line product"
    ON
)

option(
    MC_LAB_CORE_COMPILER_WARNINGS_AS_ERRORS
    "Treat compiler warnings from first-party targets as errors"
    OFF
)

set(
    MC_LAB_CORE_SANITIZER
    "OFF"
    CACHE STRING
    "Runtime sanitizer applied to first-party targets"
)

set_property(
    CACHE MC_LAB_CORE_SANITIZER
    PROPERTY STRINGS
        OFF
        ADDRESS
)

option(
    MC_LAB_CORE_ENABLE_COVERAGE
    "Instrument first-party targets for code coverage"
    OFF
)

# ==============================================================================
# Project-Wide C++ Contract
# ==============================================================================

# This interface target contains portable language requirements only.
# Compiler warnings, sanitizers, and coverage are separate policy targets so
# each first-party target opts into them explicitly and third-party code does
# not inherit repository policies.
add_library(
    mc_lab_core_project_options
    INTERFACE
)

add_library(
    MC_LAB_CORE::ProjectOptions
    ALIAS
    mc_lab_core_project_options
)

target_compile_features(
    mc_lab_core_project_options
    INTERFACE
        cxx_std_20
)

set(CMAKE_CXX_EXTENSIONS OFF)

# ==============================================================================
# CMake Infrastructure
# ==============================================================================

list(
    PREPEND
    CMAKE_MODULE_PATH
    "${MC_LAB_CORE_SOURCE_DIR}/cmake/modules"
)

include(warnings)
include(sanitizers)
include(static_analysis)
include(formatting)
include(coverage)

# CTest owns BUILD_TESTING. A parent project remains responsible for deciding
# whether and how MC-LAB-CORE tests are integrated when used as a subproject.
if(PROJECT_IS_TOP_LEVEL)
    include(CTest)

    if(BUILD_TESTING)
        add_subdirectory(
            cmake/tests
        )
    endif()
endif()

# ==============================================================================
# Product Architecture
# ==============================================================================

add_subdirectory(
    src/semantic
)

add_subdirectory(
    src/algorithm
)

if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
    add_subdirectory(
        tests/semantic
    )

    add_subdirectory(
        tests/algorithm
    )
endif()

if(MC_LAB_CORE_BUILD_CLI)
    add_subdirectory(
        tools/cli
    )
endif()

# ==============================================================================
# Quality Workflow Finalization
# ==============================================================================

# Top-level formatting targets are registered during formatting.cmake import.
# Static analysis is finalized after every first-party target has registered.
mc_lab_core_finalize_static_analysis()

# Coverage targets exist only in instrumented coverage configurations and are
# finalized after every test executable and first-party binary is known.
if(MC_LAB_CORE_ENABLE_COVERAGE)
    mc_lab_core_finalize_coverage()
endif()

# ==============================================================================
# Configuration Summary
# ==============================================================================

if(PROJECT_IS_TOP_LEVEL)
    include(config_summary)
    mc_lab_core_print_configuration_summary()
endif()
