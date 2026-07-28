include_guard(GLOBAL)

# ==============================================================================
# Top-Level Configuration Summary
# ==============================================================================

# This module presents the effective MC-LAB-CORE configuration after the entire
# first-party target graph and all quality targets have been created.
#
# It is intentionally observational:
#
#   * it does not create or modify cache entries;
#   * it does not execute optional external tools;
#   * it does not turn a missing quality tool into a configure-time failure;
#   * it does not print when MC-LAB-CORE is consumed as a subproject.
#
# Public API:
#
#   mc_lab_core_print_configuration_summary()
#       Prints one deterministic, human-readable summary. Repeated calls are
#       idempotent.

# Convert a CMake boolean value into an unambiguous ON/OFF label.
function(
    mc_lab_core_summary_boolean
    value
    output_variable
)
    if("${value}")
        set(mc_lab_core_boolean_label "ON")
    else()
        set(mc_lab_core_boolean_label "OFF")
    endif()

    set(
        "${output_variable}"
        "${mc_lab_core_boolean_label}"
        PARENT_SCOPE
    )
endfunction()

# Render one consistently aligned summary row.
function(
    mc_lab_core_summary_row
    label
    value
)
    set(mc_lab_core_summary_label_width 25)

    string(
        LENGTH
        "${label}"
        mc_lab_core_summary_label_length
    )

    math(
        EXPR
        mc_lab_core_summary_padding_length
        "${mc_lab_core_summary_label_width} - ${mc_lab_core_summary_label_length}"
    )

    if(mc_lab_core_summary_padding_length LESS 1)
        set(mc_lab_core_summary_padding_length 1)
    endif()

    string(
        REPEAT
        " "
        "${mc_lab_core_summary_padding_length}"
        mc_lab_core_summary_padding
    )

    message(
        STATUS
        "  ${label}:${mc_lab_core_summary_padding}${value}"
    )
endfunction()

# Render a path-backed tool as either its resolved executable or a concise
# non-blocking status. The quality target itself owns strict validation.
function(
    mc_lab_core_summary_tool
    variable_name
    output_variable
)
    if(
        DEFINED "${variable_name}"
        AND NOT "${${variable_name}}" STREQUAL ""
        AND NOT "${${variable_name}}" MATCHES "-NOTFOUND$"
        AND EXISTS "${${variable_name}}"
    )
        set(
            mc_lab_core_tool_status
            "${${variable_name}}"
        )
    else()
        set(
            mc_lab_core_tool_status
            "not found (quality target only)"
        )
    endif()

    set(
        "${output_variable}"
        "${mc_lab_core_tool_status}"
        PARENT_SCOPE
    )
endfunction()

# Render a policy file without making its absence a configure-time error.
function(
    mc_lab_core_summary_policy
    policy_path
    output_variable
)
    if(
        NOT "${policy_path}" STREQUAL ""
        AND EXISTS "${policy_path}"
    )
        set(
            mc_lab_core_policy_status
            "${policy_path}"
        )
    else()
        set(
            mc_lab_core_policy_status
            "missing (quality target will explain)"
        )
    endif()

    set(
        "${output_variable}"
        "${mc_lab_core_policy_status}"
        PARENT_SCOPE
    )
endfunction()

# Print the final top-level configuration.
function(mc_lab_core_print_configuration_summary)
    if(NOT PROJECT_IS_TOP_LEVEL)
        return()
    endif()

    get_property(
        mc_lab_core_summary_already_printed
        GLOBAL
        PROPERTY MC_LAB_CORE_CONFIGURATION_SUMMARY_PRINTED
    )

    if(mc_lab_core_summary_already_printed)
        return()
    endif()

    set_property(
        GLOBAL
        PROPERTY MC_LAB_CORE_CONFIGURATION_SUMMARY_PRINTED TRUE
    )

    # Presets provide their configure-preset name as cache context. Manual and
    # downstream configurations deliberately remain independent from the
    # repository's preset graph.
    if(
        DEFINED MC_LAB_CORE_CONFIGURE_PRESET
        AND NOT MC_LAB_CORE_CONFIGURE_PRESET STREQUAL ""
    )
        set(
            mc_lab_core_active_preset
            "${MC_LAB_CORE_CONFIGURE_PRESET}"
        )
    else()
        set(
            mc_lab_core_active_preset
            "manual configuration"
        )
    endif()

    if(
        DEFINED MC_LAB_CORE_EXPECTED_TOOLCHAIN
        AND NOT MC_LAB_CORE_EXPECTED_TOOLCHAIN STREQUAL ""
    )
        set(
            mc_lab_core_toolchain_contract
            "${MC_LAB_CORE_EXPECTED_TOOLCHAIN}"
        )
    else()
        set(
            mc_lab_core_toolchain_contract
            "AUTO"
        )
    endif()

    if(
        DEFINED MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE
        AND NOT MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE STREQUAL ""
    )
        set(
            mc_lab_core_host_architecture_contract
            "${MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE}"
        )
    else()
        set(
            mc_lab_core_host_architecture_contract
            "AUTO"
        )
    endif()

    if(
        DEFINED MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE
        AND NOT MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE STREQUAL ""
    )
        set(
            mc_lab_core_target_architecture_contract
            "${MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE}"
        )
    else()
        set(
            mc_lab_core_target_architecture_contract
            "AUTO"
        )
    endif()

    # --------------------------------------------------------------------------
    # Platform and toolchain
    # --------------------------------------------------------------------------

    set(
        mc_lab_core_host
        "${CMAKE_HOST_SYSTEM_NAME}"
    )

    if(CMAKE_HOST_SYSTEM_PROCESSOR)
        string(
            APPEND
            mc_lab_core_host
            " / ${CMAKE_HOST_SYSTEM_PROCESSOR}"
        )
    endif()

    set(
        mc_lab_core_target_platform
        "${CMAKE_SYSTEM_NAME}"
    )

    if(CMAKE_SYSTEM_PROCESSOR)
        string(
            APPEND
            mc_lab_core_target_platform
            " / ${CMAKE_SYSTEM_PROCESSOR}"
        )
    endif()

    if(CMAKE_CROSSCOMPILING)
        string(
            APPEND
            mc_lab_core_target_platform
            " (cross-compiling)"
        )
    endif()

    set(
        mc_lab_core_generator
        "${CMAKE_GENERATOR}"
    )

    if(CMAKE_GENERATOR_PLATFORM)
        string(
            APPEND
            mc_lab_core_generator
            " / ${CMAKE_GENERATOR_PLATFORM}"
        )
    endif()

    if(CMAKE_GENERATOR_TOOLSET)
        string(
            APPEND
            mc_lab_core_generator
            " / ${CMAKE_GENERATOR_TOOLSET}"
        )
    endif()

    if(CMAKE_CONFIGURATION_TYPES)
        string(
            JOIN
            ", "
            mc_lab_core_configuration_list
            ${CMAKE_CONFIGURATION_TYPES}
        )

        set(
            mc_lab_core_configuration
            "multi-config [${mc_lab_core_configuration_list}]"
        )
    elseif(CMAKE_BUILD_TYPE)
        set(
            mc_lab_core_configuration
            "${CMAKE_BUILD_TYPE}"
        )
    else()
        set(
            mc_lab_core_configuration
            "not selected"
        )
    endif()

    set(
        mc_lab_core_compiler
        "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}"
    )

    if(
        CMAKE_CXX_COMPILER_FRONTEND_VARIANT
        AND NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL
            CMAKE_CXX_COMPILER_ID
    )
        string(
            APPEND
            mc_lab_core_compiler
            " (${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}-style command line)"
        )
    endif()

    # --------------------------------------------------------------------------
    # Effective build policy
    # --------------------------------------------------------------------------

    if(DEFINED BUILD_TESTING)
        mc_lab_core_summary_boolean(
            "${BUILD_TESTING}"
            mc_lab_core_testing
        )
    else()
        set(
            mc_lab_core_testing
            "not owned by this project"
        )
    endif()

    mc_lab_core_summary_boolean(
        "${CMAKE_EXPORT_COMPILE_COMMANDS}"
        mc_lab_core_compile_commands
    )

    if(
        mc_lab_core_compile_commands STREQUAL "ON"
        AND NOT CMAKE_GENERATOR MATCHES "Ninja|Makefiles"
    )
        string(
            APPEND
            mc_lab_core_compile_commands
            " (generator support required)"
        )
    endif()

    if(DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
        mc_lab_core_summary_boolean(
            "${CMAKE_POSITION_INDEPENDENT_CODE}"
            mc_lab_core_position_independent_code
        )
    else()
        set(
            mc_lab_core_position_independent_code
            "not globally set"
        )
    endif()

    mc_lab_core_summary_boolean(
        "${MC_LAB_CORE_COMPILER_WARNINGS_AS_ERRORS}"
        mc_lab_core_warnings_as_errors
    )

    if(
        mc_lab_core_warnings_as_errors STREQUAL "ON"
        AND MC_LAB_CORE_WARNINGS_AS_ERRORS_CONFIGURATIONS
    )
        string(
            JOIN
            ", "
            mc_lab_core_warnings_as_errors_configurations
            ${MC_LAB_CORE_WARNINGS_AS_ERRORS_CONFIGURATIONS}
        )

        string(
            APPEND
            mc_lab_core_warnings_as_errors
            " [${mc_lab_core_warnings_as_errors_configurations}]"
        )
    endif()

    if(DEFINED MC_LAB_CORE_SANITIZER)
        set(
            mc_lab_core_sanitizer
            "${MC_LAB_CORE_SANITIZER}"
        )
    else()
        set(
            mc_lab_core_sanitizer
            "OFF"
        )
    endif()

    mc_lab_core_summary_boolean(
        "${MC_LAB_CORE_ENABLE_COVERAGE}"
        mc_lab_core_coverage
    )

    # --------------------------------------------------------------------------
    # Optional quality tooling and repository targets
    # --------------------------------------------------------------------------

    mc_lab_core_summary_tool(
        MC_LAB_CORE_CLANG_FORMAT
        mc_lab_core_clang_format
    )

    mc_lab_core_summary_tool(
        MC_LAB_CORE_CLANG_TIDY
        mc_lab_core_clang_tidy
    )

    mc_lab_core_summary_tool(
        MC_LAB_CORE_GIT
        mc_lab_core_git
    )

    mc_lab_core_summary_policy(
        "${MC_LAB_CORE_CLANG_FORMAT_CONFIG}"
        mc_lab_core_clang_format_policy
    )

    mc_lab_core_summary_policy(
        "${MC_LAB_CORE_CLANG_TIDY_CONFIG}"
        mc_lab_core_clang_tidy_policy
    )

    if(DEFINED MC_LAB_CORE_FORMAT_PATHS)
        string(
            JOIN
            ", "
            mc_lab_core_format_paths
            ${MC_LAB_CORE_FORMAT_PATHS}
        )
    else()
        set(
            mc_lab_core_format_paths
            "not configured"
        )
    endif()

    set(mc_lab_core_quality_targets)

    foreach(
        mc_lab_core_quality_target
        IN ITEMS
            format-check
            format-staged
            static-analysis
            coverage-reset
            coverage-report
            coverage-check
    )
        if(TARGET "${mc_lab_core_quality_target}")
            list(
                APPEND
                mc_lab_core_quality_targets
                "${mc_lab_core_quality_target}"
            )
        endif()
    endforeach()

    if(mc_lab_core_quality_targets)
        string(
            JOIN
            ", "
            mc_lab_core_quality_target_list
            ${mc_lab_core_quality_targets}
        )
    else()
        set(
            mc_lab_core_quality_target_list
            "none"
        )
    endif()

    get_property(
        mc_lab_core_static_analysis_targets
        GLOBAL
        PROPERTY MC_LAB_CORE_STATIC_ANALYSIS_TARGETS
    )

    if(mc_lab_core_static_analysis_targets)
        list(
            REMOVE_DUPLICATES
            mc_lab_core_static_analysis_targets
        )

        list(
            LENGTH
            mc_lab_core_static_analysis_targets
            mc_lab_core_static_analysis_target_count
        )
    else()
        set(
            mc_lab_core_static_analysis_target_count
            0
        )
    endif()

    # --------------------------------------------------------------------------
    # Actionable developer commands
    # --------------------------------------------------------------------------

    set(mc_lab_core_developer_commands_available FALSE)

    if(NOT mc_lab_core_active_preset STREQUAL "manual configuration")
        set(mc_lab_core_developer_commands_available TRUE)

        if(mc_lab_core_active_preset MATCHES "^windows-visualstudio")
            # One Visual Studio configure tree intentionally serves both
            # ordinary configurations. Its workflows select the build and test
            # configuration through their corresponding build/test presets.
            # The configure preset is e.g. windows-visualstudio-2022; the
            # derived workflow, build, and test presets append -debug/-release.
            set(
                mc_lab_core_full_workflow_label
                "Clean Debug workflow"
            )
            set(
                mc_lab_core_full_workflow
                "cmake --workflow --preset ${mc_lab_core_active_preset}-debug --fresh"
            )
            set(
                mc_lab_core_alternate_workflow_label
                "Clean Release workflow"
            )
            set(
                mc_lab_core_alternate_workflow
                "cmake --workflow --preset ${mc_lab_core_active_preset}-release --fresh"
            )
            set(
                mc_lab_core_incremental_build_label
                "Debug build"
            )
            set(
                mc_lab_core_incremental_build
                "cmake --build --preset ${mc_lab_core_active_preset}-debug"
            )
            set(
                mc_lab_core_alternate_build
                "cmake --build --preset ${mc_lab_core_active_preset}-release"
            )
            set(
                mc_lab_core_incremental_test_label
                "Debug tests"
            )
            set(
                mc_lab_core_incremental_test
                "ctest --preset ${mc_lab_core_active_preset}-debug"
            )
            set(
                mc_lab_core_alternate_test
                "ctest --preset ${mc_lab_core_active_preset}-release"
            )
        else()
            set(
                mc_lab_core_full_workflow_label
                "Clean workflow"
            )
            set(
                mc_lab_core_full_workflow
                "cmake --workflow --preset ${mc_lab_core_active_preset} --fresh"
            )
            set(
                mc_lab_core_alternate_workflow_label
                ""
            )
            set(
                mc_lab_core_alternate_workflow
                ""
            )
            set(
                mc_lab_core_incremental_build_label
                "Incremental build"
            )
            set(
                mc_lab_core_incremental_build
                "cmake --build --preset ${mc_lab_core_active_preset}"
            )
            set(
                mc_lab_core_alternate_build
                ""
            )
            set(
                mc_lab_core_incremental_test_label
                "Incremental tests"
            )
            set(
                mc_lab_core_incremental_test
                "ctest --preset ${mc_lab_core_active_preset}"
            )
            set(
                mc_lab_core_alternate_test
                ""
            )
        endif()
    endif()

    # --------------------------------------------------------------------------
    # Human-readable output
    # --------------------------------------------------------------------------

    message(STATUS "")
    message(
        STATUS
        "MC-LAB-CORE ${PROJECT_VERSION} configuration"
    )
    message(
        STATUS
        "============================================================"
    )

    message(STATUS "Platform and toolchain")
    mc_lab_core_summary_row(
        "Configure preset"
        "${mc_lab_core_active_preset}"
    )
    mc_lab_core_summary_row(
        "Host"
        "${mc_lab_core_host}"
    )
    mc_lab_core_summary_row(
        "Target"
        "${mc_lab_core_target_platform}"
    )
    mc_lab_core_summary_row(
        "Generator"
        "${mc_lab_core_generator}"
    )
    mc_lab_core_summary_row(
        "Configuration"
        "${mc_lab_core_configuration}"
    )
    mc_lab_core_summary_row(
        "C++ compiler"
        "${mc_lab_core_compiler}"
    )
    mc_lab_core_summary_row(
        "Compiler executable"
        "${CMAKE_CXX_COMPILER}"
    )
    mc_lab_core_summary_row(
        "Toolchain contract"
        "${mc_lab_core_toolchain_contract}"
    )
    mc_lab_core_summary_row(
        "Host architecture"
        "${mc_lab_core_host_architecture_contract}"
    )
    mc_lab_core_summary_row(
        "Target architecture"
        "${mc_lab_core_target_architecture_contract}"
    )

    message(STATUS "")
    message(STATUS "Build contract")
    mc_lab_core_summary_row(
        "C++ language"
        "C++20 / extensions OFF"
    )
    mc_lab_core_summary_row(
        "Tests"
        "${mc_lab_core_testing}"
    )
    mc_lab_core_summary_row(
        "Compile database"
        "${mc_lab_core_compile_commands}"
    )
    mc_lab_core_summary_row(
        "Position-independent code"
        "${mc_lab_core_position_independent_code}"
    )
    mc_lab_core_summary_row(
        "Warnings as errors"
        "${mc_lab_core_warnings_as_errors}"
    )
    mc_lab_core_summary_row(
        "Sanitizer"
        "${mc_lab_core_sanitizer}"
    )
    mc_lab_core_summary_row(
        "Coverage"
        "${mc_lab_core_coverage}"
    )

    message(STATUS "")
    message(STATUS "Quality infrastructure (optional for normal builds)")
    mc_lab_core_summary_row(
        "clang-format"
        "${mc_lab_core_clang_format}"
    )
    mc_lab_core_summary_row(
        "Formatting policy"
        "${mc_lab_core_clang_format_policy}"
    )
    mc_lab_core_summary_row(
        "Formatting roots"
        "${mc_lab_core_format_paths}"
    )
    mc_lab_core_summary_row(
        "clang-tidy"
        "${mc_lab_core_clang_tidy}"
    )
    mc_lab_core_summary_row(
        "Static-analysis policy"
        "${mc_lab_core_clang_tidy_policy}"
    )
    mc_lab_core_summary_row(
        "Registered analysis targets"
        "${mc_lab_core_static_analysis_target_count}"
    )
    mc_lab_core_summary_row(
        "Git"
        "${mc_lab_core_git}"
    )
    mc_lab_core_summary_row(
        "Quality targets"
        "${mc_lab_core_quality_target_list}"
    )

    message(STATUS "")
    message(STATUS "Directories")
    mc_lab_core_summary_row(
        "Source"
        "${PROJECT_SOURCE_DIR}"
    )
    mc_lab_core_summary_row(
        "Build"
        "${CMAKE_BINARY_DIR}"
    )
    mc_lab_core_summary_row(
        "Install"
        "${CMAKE_INSTALL_PREFIX}"
    )

    if(mc_lab_core_developer_commands_available)
        message(STATUS "")
        message(STATUS "Next commands")
        mc_lab_core_summary_row(
            "${mc_lab_core_full_workflow_label}"
            "${mc_lab_core_full_workflow}"
        )

        if(NOT mc_lab_core_alternate_workflow STREQUAL "")
            mc_lab_core_summary_row(
                "${mc_lab_core_alternate_workflow_label}"
                "${mc_lab_core_alternate_workflow}"
            )
        endif()

        mc_lab_core_summary_row(
            "${mc_lab_core_incremental_build_label}"
            "${mc_lab_core_incremental_build}"
        )

        if(NOT mc_lab_core_alternate_build STREQUAL "")
            mc_lab_core_summary_row(
                "Release build"
                "${mc_lab_core_alternate_build}"
            )
        endif()

        mc_lab_core_summary_row(
            "${mc_lab_core_incremental_test_label}"
            "${mc_lab_core_incremental_test}"
        )

        if(NOT mc_lab_core_alternate_test STREQUAL "")
            mc_lab_core_summary_row(
                "Release tests"
                "${mc_lab_core_alternate_test}"
            )
        endif()
    endif()

    message(STATUS "")
endfunction()
