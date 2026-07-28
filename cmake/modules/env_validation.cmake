include_guard(GLOBAL)

# ==============================================================================
# Build Environment Contract
# ==============================================================================

# Compiler names such as "gcc", "clang++", and "cl" are intentionally resolved
# through the active development environment. This module ensures that the
# environment was activated by the expected vendor and that CMake selected the
# corresponding compiler family.
#
# Validation has two phases:
#
#   1. mc_lab_core_validate_build_environment_preflight()
#      Runs through CMAKE_PROJECT_TOP_LEVEL_INCLUDES before languages are
#      enabled. It validates the shell and produces an actionable error before
#      CMake emits a generic "compiler not found" diagnostic.
#
#   2. mc_lab_core_validate_build_environment()
#      Runs immediately after project(). It validates the resolved compiler,
#      frontend, generator, target platform, and MSYS2 distribution path.
#
# The selected contracts are carried by MC_LAB_CORE_EXPECTED_TOOLCHAIN,
# MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE, and
# MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE. AUTO keeps manual and downstream
# builds portable and disables repository-specific environment assumptions.

set(
    mc_lab_core_known_toolchain_contracts
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
    mc_lab_core_known_architecture_contracts
    AUTO
    x64
)

set(
    mc_lab_core_known_host_architecture_contracts
    AUTO
    x64
)

# Presets provide this optional context so diagnostics can reproduce the exact
# configure command. Normalize it eagerly: successful configurations must not
# report the cache entry as an unused command-line variable.
set(mc_lab_core_configure_preset "")

if(
    DEFINED MC_LAB_CORE_CONFIGURE_PRESET
    AND NOT MC_LAB_CORE_CONFIGURE_PRESET STREQUAL ""
)
    set(
        mc_lab_core_configure_preset
        "${MC_LAB_CORE_CONFIGURE_PRESET}"
    )
endif()

# Normalize the architecture labels emitted by Windows, CMake, MSVC, LLVM, and
# GNU-family toolchains. The contract concerns the x86-64 instruction set, not
# one vendor-specific spelling.
function(
    _mc_lab_core_is_x86_64
    architecture
    output_variable
)
    string(
        TOUPPER
        "${architecture}"
        mc_lab_core_normalized_architecture
    )

    if(
        mc_lab_core_normalized_architecture
            MATCHES "^(AMD64|X64|X86_64)$"
    )
        set(mc_lab_core_architecture_is_x86_64 TRUE)
    else()
        set(mc_lab_core_architecture_is_x86_64 FALSE)
    endif()

    set(
        "${output_variable}"
        "${mc_lab_core_architecture_is_x86_64}"
        PARENT_SCOPE
    )
endfunction()

# Resolve the native Windows architecture rather than merely the architecture
# of a compatibility-layer process. PROCESSOR_ARCHITEW6432, when present,
# identifies the native host behind an emulated process.
function(_mc_lab_core_detect_windows_host_architecture output_variable)
    set(mc_lab_core_detected_host_architecture "")

    if(DEFINED ENV{PROCESSOR_ARCHITEW6432})
        set(
            mc_lab_core_detected_host_architecture
            "$ENV{PROCESSOR_ARCHITEW6432}"
        )
    endif()

    if(
        mc_lab_core_detected_host_architecture STREQUAL ""
        AND DEFINED ENV{PROCESSOR_ARCHITECTURE}
    )
        set(
            mc_lab_core_detected_host_architecture
            "$ENV{PROCESSOR_ARCHITECTURE}"
        )
    endif()

    if(
        mc_lab_core_detected_host_architecture STREQUAL ""
        AND DEFINED CMAKE_HOST_SYSTEM_PROCESSOR
    )
        set(
            mc_lab_core_detected_host_architecture
            "${CMAKE_HOST_SYSTEM_PROCESSOR}"
        )
    endif()

    if(mc_lab_core_detected_host_architecture STREQUAL "")
        set(
            mc_lab_core_detected_host_architecture
            "not found in this shell"
        )
    endif()

    set(
        "${output_variable}"
        "${mc_lab_core_detected_host_architecture}"
        PARENT_SCOPE
    )
endfunction()

function(
    _mc_lab_core_environment_failure
    summary
    remediation
)
    set(mc_lab_core_host_only_failure FALSE)

    if(ARGC GREATER 2 AND ARGV2 STREQUAL "HOST_ONLY")
        set(mc_lab_core_host_only_failure TRUE)
    endif()

    if(mc_lab_core_host_only_failure)
        _mc_lab_core_detect_windows_host_architecture(
            mc_lab_core_detected_host_architecture
        )

        string(
            CONCAT
            mc_lab_core_contract_context
            "Architecture contract:\n"
            "  Expected: ${MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE} "
            "(native x86-64)\n"
            "  Detected: "
            "${mc_lab_core_detected_host_architecture}\n"
        )
    else()
        string(
            CONCAT
            mc_lab_core_contract_context
            "Expected toolchain:\n"
            "  ${MC_LAB_CORE_EXPECTED_TOOLCHAIN}\n"
        )
    endif()

    if(NOT mc_lab_core_configure_preset STREQUAL "")
        string(
            PREPEND
            mc_lab_core_contract_context
            "Configure preset:\n"
            "  ${mc_lab_core_configure_preset}\n"
            "\n"
        )
    endif()

    message(
        FATAL_ERROR
        "MC-LAB-CORE cannot configure the selected build environment.\n"
        "\n"
        "Reason:\n"
        "  ${summary}\n"
        "\n"
        "${mc_lab_core_contract_context}"
        "\n"
        "${remediation}"
    )
endfunction()

function(
    _mc_lab_core_require_program
    program_name
    remediation
)
    find_program(
        mc_lab_core_required_program
        NAMES "${program_name}"
        NO_CACHE
    )

    if(NOT mc_lab_core_required_program)
        _mc_lab_core_environment_failure(
            "Required program '${program_name}' was not found in this shell PATH."
            "${remediation}"
        )
    endif()
endfunction()

# Route Windows environment recovery through the repository launcher. The
# launcher owns installation discovery, package-level diagnostics, and shell
# activation; CMake owns contract validation and the exact retry command.
function(
    _mc_lab_core_get_msys2_recovery
    expected_environment
    output_variable
)
    if(NOT mc_lab_core_configure_preset STREQUAL "")
        set(
            mc_lab_core_retry_command
            "cmake --fresh --preset ${mc_lab_core_configure_preset}"
        )
    else()
        set(
            mc_lab_core_retry_command
            "cmake --fresh --preset <preset-name>"
        )
    endif()

    string(
        TOLOWER
        "${expected_environment}"
        mc_lab_core_environment_argument
    )

    if(
        NOT expected_environment STREQUAL "UCRT64"
        AND NOT expected_environment STREQUAL "CLANG64"
    )
        message(
            FATAL_ERROR
            "Unsupported MSYS2 recovery environment '${expected_environment}'."
        )
    endif()

    string(
        CONCAT
        mc_lab_core_recovery
        "From PowerShell at the repository root, run:\n"
        "  .\\launch-dev-shell.ps1 "
        "${mc_lab_core_environment_argument}\n"
        "\n"
        "The launcher diagnoses the ${expected_environment} installation, "
        "reports missing packages, and opens the correct shell.\n"
        "\n"
        "Then retry:\n"
        "  ${mc_lab_core_retry_command}\n"
        "\n"
        "Detailed guide:\n"
        "  docs/development/windows-build-environments.md"
    )

    set(
        "${output_variable}"
        "${mc_lab_core_recovery}"
        PARENT_SCOPE
    )
endfunction()

# Route Microsoft-ABI recovery through the same repository launcher.
function(
    _mc_lab_core_get_visual_studio_recovery
    output_variable
)
    if(NOT mc_lab_core_configure_preset STREQUAL "")
        set(
            mc_lab_core_retry_command
            "cmake --fresh --preset ${mc_lab_core_configure_preset}"
        )
    else()
        set(
            mc_lab_core_retry_command
            "cmake --fresh --preset <preset-name>"
        )
    endif()

    string(
        CONCAT
        mc_lab_core_recovery
        "From PowerShell at the repository root, run:\n"
        "  .\\launch-dev-shell.ps1 msvc\n"
        "\n"
        "The launcher diagnoses the Visual Studio C++ installation and opens "
        "the correct developer shell.\n"
        "\n"
        "Then retry:\n"
        "  ${mc_lab_core_retry_command}\n"
        "\n"
        "Detailed guide:\n"
        "  docs/development/windows-build-environments.md"
    )

    set(
        "${output_variable}"
        "${mc_lab_core_recovery}"
        PARENT_SCOPE
    )
endfunction()

# Build native Linux and macOS recovery procedures with commands matching the
# supported host matrix. These paths use the current terminal rather than a
# vendor shell launcher.
function(
    _mc_lab_core_get_native_recovery
    output_variable
)
    if(NOT mc_lab_core_configure_preset STREQUAL "")
        set(
            mc_lab_core_retry_command
            "cmake --fresh --preset ${mc_lab_core_configure_preset}"
        )
    else()
        set(
            mc_lab_core_retry_command
            "cmake --fresh --preset <preset-name>"
        )
    endif()

    if(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "LINUX_GCC")
        string(
            CONCAT
            mc_lab_core_installation
            "   Debian or Ubuntu:\n"
            "     sudo apt-get update\n"
            "     sudo apt-get install build-essential cmake ninja-build\n"
            "\n"
            "   Fedora:\n"
            "     sudo dnf install gcc-c++ cmake ninja-build"
        )
        set(mc_lab_core_toolchain_label "GCC")
    elseif(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "LINUX_CLANG")
        string(
            CONCAT
            mc_lab_core_installation
            "   Debian or Ubuntu:\n"
            "     sudo apt-get update\n"
            "     sudo apt-get install clang cmake ninja-build\n"
            "\n"
            "   Fedora:\n"
            "     sudo dnf install clang cmake ninja-build"
        )
        set(mc_lab_core_toolchain_label "Clang")
    elseif(
        MC_LAB_CORE_EXPECTED_TOOLCHAIN
        STREQUAL "MACOS_APPLECLANG"
    )
        string(
            CONCAT
            mc_lab_core_installation
            "   Install the Apple command-line developer tools:\n"
            "     xcode-select --install\n"
            "\n"
            "   Install CMake and Ninja with your package manager, for "
            "example Homebrew:\n"
            "     brew install cmake ninja"
        )
        set(mc_lab_core_toolchain_label "AppleClang")
    else()
        message(
            FATAL_ERROR
            "Unsupported native recovery contract "
            "'${MC_LAB_CORE_EXPECTED_TOOLCHAIN}'."
        )
    endif()

    string(
        CONCAT
        mc_lab_core_recovery
        "Recovery options:\n"
        "\n"
        "1. Install the ${mc_lab_core_toolchain_label} toolchain and build "
        "tools.\n"
        "\n"
        "${mc_lab_core_installation}\n"
        "\n"
        "2. If the tools are already installed, open a terminal where they "
        "are available on PATH and retry:\n"
        "\n"
        "     ${mc_lab_core_retry_command}\n"
        "\n"
        "Detailed guide:\n"
        "  docs/cmake/cmake-presets.md"
    )

    set(
        "${output_variable}"
        "${mc_lab_core_recovery}"
        PARENT_SCOPE
    )
endfunction()

function(
    _mc_lab_core_get_toolchain_recovery
    output_variable
)
    if(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_UCRT64_GCC")
        _mc_lab_core_get_msys2_recovery(
            UCRT64
            mc_lab_core_recovery
        )
    elseif(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_CLANG64")
        _mc_lab_core_get_msys2_recovery(
            CLANG64
            mc_lab_core_recovery
        )
    elseif(
        MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_MSVC"
        OR MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_CLANGCL"
        OR MC_LAB_CORE_EXPECTED_TOOLCHAIN
            STREQUAL "WINDOWS_VISUAL_STUDIO"
    )
        _mc_lab_core_get_visual_studio_recovery(
            mc_lab_core_recovery
        )
    elseif(
        MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "LINUX_GCC"
        OR MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "LINUX_CLANG"
        OR MC_LAB_CORE_EXPECTED_TOOLCHAIN
            STREQUAL "MACOS_APPLECLANG"
    )
        _mc_lab_core_get_native_recovery(
            mc_lab_core_recovery
        )
    else()
        set(
            mc_lab_core_recovery
            "Install the requested compiler, CMake, and Ninja, then retry the selected configure preset with --fresh."
        )
    endif()

    set(
        "${output_variable}"
        "${mc_lab_core_recovery}"
        PARENT_SCOPE
    )
endfunction()

function(
    _mc_lab_core_require_msys2_environment
    expected_environment
    compiler_name
)
    _mc_lab_core_get_msys2_recovery(
        "${expected_environment}"
        mc_lab_core_recovery
    )

    if(
        NOT DEFINED ENV{MSYSTEM}
        OR NOT "$ENV{MSYSTEM}" STREQUAL "${expected_environment}"
    )
        if(NOT DEFINED ENV{MSYSTEM} OR "$ENV{MSYSTEM}" STREQUAL "")
            set(
                mc_lab_core_msys2_failure_summary
                "The ${expected_environment} MSYS2 environment was not found in this shell."
            )
        else()
            set(
                mc_lab_core_msys2_failure_summary
                "This shell is using the '$ENV{MSYSTEM}' MSYS2 environment, but '${expected_environment}' is required."
            )
        endif()

        _mc_lab_core_environment_failure(
            "${mc_lab_core_msys2_failure_summary}"
            "${mc_lab_core_recovery}"
        )
    endif()

    if(
        MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE STREQUAL "x64"
        AND (
            NOT DEFINED ENV{MSYSTEM_CARCH}
            OR NOT "$ENV{MSYSTEM_CARCH}" STREQUAL "x86_64"
        )
    )
        _mc_lab_core_environment_failure(
            "The selected preset requires an x64 MSYS2 toolchain, but this shell does not provide MSYSTEM_CARCH=x86_64."
            "${mc_lab_core_recovery}"
        )
    endif()

    _mc_lab_core_require_program(
        "${compiler_name}"
        "${mc_lab_core_recovery}"
    )

    _mc_lab_core_require_program(
        "ninja"
        "${mc_lab_core_recovery}"
    )
endfunction()

# Enforce native x86-64 Windows hardware independently from the target
# architecture. This deliberately rejects x64 emulation or cross-compilation
# from ARM/ARM64 hosts even when the selected compiler could produce x64 code.
function(_mc_lab_core_require_host_architecture)
    if(
        NOT DEFINED MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE
        OR MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE STREQUAL ""
        OR MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE STREQUAL "AUTO"
    )
        return()
    endif()

    if(MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE STREQUAL "x64")
        _mc_lab_core_detect_windows_host_architecture(
            mc_lab_core_detected_host_architecture
        )

        _mc_lab_core_is_x86_64(
            "${mc_lab_core_detected_host_architecture}"
            mc_lab_core_host_is_x86_64
        )

        if(NOT mc_lab_core_host_is_x86_64)
            _mc_lab_core_environment_failure(
                "The selected Windows preset requires a native x86-64 host, but '${mc_lab_core_detected_host_architecture}' was detected. x86 32-bit and ARM/ARM64 hosts are not supported, including x64 emulation or cross-compilation from ARM hardware."
                "Use a native x86-64 Windows machine or an x64 CI runner."
                HOST_ONLY
            )
        endif()

        if(
            DEFINED CMAKE_HOST_SYSTEM_PROCESSOR
            AND NOT CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL ""
        )
            _mc_lab_core_is_x86_64(
                "${CMAKE_HOST_SYSTEM_PROCESSOR}"
                mc_lab_core_cmake_host_is_x86_64
            )

            if(NOT mc_lab_core_cmake_host_is_x86_64)
                _mc_lab_core_environment_failure(
                    "CMake identified the host processor as '${CMAKE_HOST_SYSTEM_PROCESSOR}', but the selected Windows preset requires native x86-64 hardware. x86 32-bit and ARM/ARM64 hosts are not supported."
                    "Use a native x86-64 Windows machine or an x64 CI runner."
                    HOST_ONLY
                )
            endif()
        endif()
    endif()
endfunction()

# Validate environment activation before CMake enables C++.
function(mc_lab_core_validate_build_environment_preflight)
    if(
        NOT DEFINED MC_LAB_CORE_EXPECTED_TOOLCHAIN
        OR MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL ""
        OR MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "AUTO"
    )
        return()
    endif()

    if(
        NOT MC_LAB_CORE_EXPECTED_TOOLCHAIN
            IN_LIST mc_lab_core_known_toolchain_contracts
    )
        list(
            JOIN
            mc_lab_core_known_toolchain_contracts
            ", "
            mc_lab_core_known_toolchain_contracts_text
        )

        _mc_lab_core_environment_failure(
            "Unknown toolchain contract '${MC_LAB_CORE_EXPECTED_TOOLCHAIN}'."
            "Supported contracts: ${mc_lab_core_known_toolchain_contracts_text}."
        )
    endif()

    if(
        DEFINED MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE
        AND NOT MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE
            IN_LIST mc_lab_core_known_host_architecture_contracts
    )
        list(
            JOIN
            mc_lab_core_known_host_architecture_contracts
            ", "
            mc_lab_core_known_host_architecture_contracts_text
        )

        _mc_lab_core_get_toolchain_recovery(
            mc_lab_core_recovery
        )

        _mc_lab_core_environment_failure(
            "Unknown host architecture contract '${MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE}'."
            "Supported host architecture contracts: ${mc_lab_core_known_host_architecture_contracts_text}.\n\n${mc_lab_core_recovery}"
        )
    endif()

    if(
        DEFINED MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE
        AND NOT MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE
            IN_LIST mc_lab_core_known_architecture_contracts
    )
        list(
            JOIN
            mc_lab_core_known_architecture_contracts
            ", "
            mc_lab_core_known_architecture_contracts_text
        )

        _mc_lab_core_get_toolchain_recovery(
            mc_lab_core_recovery
        )

        _mc_lab_core_environment_failure(
            "Unknown target architecture contract '${MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE}'."
            "Supported target architecture contracts: ${mc_lab_core_known_architecture_contracts_text}.\n\n${mc_lab_core_recovery}"
        )
    endif()

    _mc_lab_core_require_host_architecture()

    if(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_UCRT64_GCC")
        _mc_lab_core_require_msys2_environment(
            UCRT64
            g++
        )
    elseif(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_CLANG64")
        _mc_lab_core_require_msys2_environment(
            CLANG64
            clang++
        )
    elseif(
        MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_MSVC"
        OR MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_CLANGCL"
    )
        _mc_lab_core_get_visual_studio_recovery(
            mc_lab_core_recovery
        )

        if(
            NOT DEFINED ENV{VSCMD_VER}
            OR "$ENV{VSCMD_VER}" STREQUAL ""
            OR NOT DEFINED ENV{VCINSTALLDIR}
            OR "$ENV{VCINSTALLDIR}" STREQUAL ""
        )
            _mc_lab_core_environment_failure(
                "A complete Visual Studio developer environment was not found in this shell."
                "${mc_lab_core_recovery}"
            )
        endif()

        if(MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE STREQUAL "x64")
            if(
                NOT DEFINED ENV{VSCMD_ARG_TGT_ARCH}
                OR "$ENV{VSCMD_ARG_TGT_ARCH}" STREQUAL ""
            )
                _mc_lab_core_environment_failure(
                    "The selected preset requires the Visual Studio x64 target architecture, but VSCMD_ARG_TGT_ARCH was not found in this shell."
                    "${mc_lab_core_recovery}"
                )
            else()
                _mc_lab_core_is_x86_64(
                    "$ENV{VSCMD_ARG_TGT_ARCH}"
                    mc_lab_core_visual_studio_target_is_x86_64
                )

                if(NOT mc_lab_core_visual_studio_target_is_x86_64)
                    _mc_lab_core_environment_failure(
                        "The selected preset requires the Visual Studio x64 target architecture, but this shell targets '$ENV{VSCMD_ARG_TGT_ARCH}'."
                        "${mc_lab_core_recovery}"
                    )
                endif()
            endif()
        endif()

        if(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_MSVC")
            set(mc_lab_core_frontend_program "cl")
        else()
            set(mc_lab_core_frontend_program "clang-cl")
        endif()

        _mc_lab_core_require_program(
            "${mc_lab_core_frontend_program}"
            "${mc_lab_core_recovery}"
        )

        _mc_lab_core_require_program(
            "ninja"
            "${mc_lab_core_recovery}"
        )
    elseif(
        MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "LINUX_GCC"
        OR MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "LINUX_CLANG"
        OR MC_LAB_CORE_EXPECTED_TOOLCHAIN
            STREQUAL "MACOS_APPLECLANG"
    )
        _mc_lab_core_get_native_recovery(
            mc_lab_core_recovery
        )

        if(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "LINUX_GCC")
            set(mc_lab_core_frontend_program "g++")
        else()
            set(mc_lab_core_frontend_program "clang++")
        endif()

        _mc_lab_core_require_program(
            "${mc_lab_core_frontend_program}"
            "${mc_lab_core_recovery}"
        )

        _mc_lab_core_require_program(
            "ninja"
            "${mc_lab_core_recovery}"
        )
    endif()
endfunction()

function(
    _mc_lab_core_require_compiler
    compiler_id
    frontend_variant
)
    _mc_lab_core_get_toolchain_recovery(
        mc_lab_core_recovery
    )

    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "${compiler_id}")
        _mc_lab_core_environment_failure(
            "CMake selected '${CMAKE_CXX_COMPILER_ID}' at '${CMAKE_CXX_COMPILER}', but '${compiler_id}' is required."
            "${mc_lab_core_recovery}"
        )
    endif()

    if(
        NOT "${frontend_variant}" STREQUAL ""
        AND NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT
            STREQUAL "${frontend_variant}"
    )
        _mc_lab_core_environment_failure(
            "CMake selected the '${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}' command-line frontend, but '${frontend_variant}' is required."
            "${mc_lab_core_recovery}"
        )
    endif()
endfunction()

function(
    _mc_lab_core_require_target_system
    expected_system
)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "${expected_system}")
        _mc_lab_core_environment_failure(
            "CMake selected target system '${CMAKE_SYSTEM_NAME}', but '${expected_system}' is required."
            "Use the platform preset matching the active host environment."
        )
    endif()
endfunction()

function(
    _mc_lab_core_require_compiler_path_segment
    expected_segment
)
    _mc_lab_core_get_toolchain_recovery(
        mc_lab_core_recovery
    )

    file(
        TO_CMAKE_PATH
        "${CMAKE_CXX_COMPILER}"
        mc_lab_core_normalized_compiler
    )

    string(
        TOLOWER
        "${mc_lab_core_normalized_compiler}"
        mc_lab_core_normalized_compiler
    )

    if(NOT mc_lab_core_normalized_compiler MATCHES "${expected_segment}")
        _mc_lab_core_environment_failure(
            "The selected compiler does not belong to the expected MSYS2 distribution:\n  ${CMAKE_CXX_COMPILER}"
            "${mc_lab_core_recovery}"
        )
    endif()
endfunction()

# Validate the architecture produced by the resolved compiler and generator.
# Preflight checks protect vendor-shell selection; these post-project checks
# protect against stale caches and manually altered compiler settings.
function(_mc_lab_core_require_target_architecture)
    if(
        NOT DEFINED MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE
        OR MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE STREQUAL ""
        OR MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE STREQUAL "AUTO"
    )
        return()
    endif()

    _mc_lab_core_get_toolchain_recovery(
        mc_lab_core_recovery
    )

    if(MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE STREQUAL "x64")
        if(
            DEFINED CMAKE_SIZEOF_VOID_P
            AND NOT CMAKE_SIZEOF_VOID_P EQUAL 8
        )
            _mc_lab_core_environment_failure(
                "The configured compiler produces ${CMAKE_SIZEOF_VOID_P}-byte pointers, but the x64 contract requires 8-byte pointers."
                "${mc_lab_core_recovery}"
            )
        endif()

        if(
            CMAKE_GENERATOR MATCHES "^Visual Studio "
            AND NOT CMAKE_GENERATOR_PLATFORM STREQUAL "x64"
        )
            _mc_lab_core_environment_failure(
                "Visual Studio generator platform '${CMAKE_GENERATOR_PLATFORM}' does not satisfy the required x64 contract."
                "${mc_lab_core_recovery}"
            )
        endif()

        if(
            DEFINED CMAKE_CXX_COMPILER_ARCHITECTURE_ID
            AND NOT CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL ""
        )
            _mc_lab_core_is_x86_64(
                "${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}"
                mc_lab_core_compiler_target_is_x86_64
            )

            if(NOT mc_lab_core_compiler_target_is_x86_64)
                _mc_lab_core_environment_failure(
                    "Compiler architecture '${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}' does not satisfy the required x64 contract."
                    "${mc_lab_core_recovery}"
                )
            endif()
        endif()
    endif()
endfunction()

# Validate the toolchain CMake actually selected.
function(mc_lab_core_validate_build_environment)
    if(
        NOT DEFINED MC_LAB_CORE_EXPECTED_TOOLCHAIN
        OR MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL ""
        OR MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "AUTO"
    )
        return()
    endif()

    _mc_lab_core_require_host_architecture()

    if(MC_LAB_CORE_EXPECTED_TOOLCHAIN MATCHES "^WINDOWS_")
        _mc_lab_core_require_target_system("Windows")
    elseif(MC_LAB_CORE_EXPECTED_TOOLCHAIN MATCHES "^LINUX_")
        _mc_lab_core_require_target_system("Linux")
    elseif(MC_LAB_CORE_EXPECTED_TOOLCHAIN MATCHES "^MACOS_")
        _mc_lab_core_require_target_system("Darwin")
    endif()

    _mc_lab_core_require_target_architecture()

    if(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_MSVC")
        _mc_lab_core_require_compiler("MSVC" "")
    elseif(
        MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_VISUAL_STUDIO"
    )
        if(NOT CMAKE_GENERATOR MATCHES "^Visual Studio ")
            _mc_lab_core_get_visual_studio_recovery(
                mc_lab_core_recovery
            )

            _mc_lab_core_environment_failure(
                "Generator '${CMAKE_GENERATOR}' is not a Visual Studio generator."
                "${mc_lab_core_recovery}"
            )
        endif()

        _mc_lab_core_require_compiler("MSVC" "")
    elseif(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_CLANGCL")
        _mc_lab_core_require_compiler("Clang" "MSVC")
    elseif(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_UCRT64_GCC")
        _mc_lab_core_require_compiler("GNU" "")
        _mc_lab_core_require_compiler_path_segment("/ucrt64/bin/")
    elseif(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "WINDOWS_CLANG64")
        _mc_lab_core_require_compiler("Clang" "GNU")
        _mc_lab_core_require_compiler_path_segment("/clang64/bin/")
    elseif(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "LINUX_GCC")
        _mc_lab_core_require_compiler("GNU" "")
    elseif(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "LINUX_CLANG")
        _mc_lab_core_require_compiler("Clang" "GNU")
    elseif(MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL "MACOS_APPLECLANG")
        _mc_lab_core_require_compiler("AppleClang" "")
    endif()
endfunction()

# The root CMakeLists includes this module before project(). Avoid imposing a
# repository-specific shell contract when MC-LAB-CORE is consumed by a parent.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    mc_lab_core_validate_build_environment_preflight()
endif()
