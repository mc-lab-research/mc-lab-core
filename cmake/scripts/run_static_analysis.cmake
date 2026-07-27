cmake_minimum_required(VERSION 3.25)

# ==============================================================================
# clang-tidy Target Runner
# ==============================================================================

# This script is executed by the `static-analysis` build target. Keeping process
# orchestration in script mode avoids shell-specific quoting and gives the same
# failure behavior on Windows, Linux, and macOS.

foreach(
    mc_lab_core_required_variable
    IN ITEMS
        MC_LAB_CORE_CLANG_TIDY
        MC_LAB_CORE_CLANG_TIDY_CONFIG
        MC_LAB_CORE_COMPILE_DATABASE
        MC_LAB_CORE_SOURCE_MANIFEST
)
    if(
        NOT DEFINED "${mc_lab_core_required_variable}"
        OR "${${mc_lab_core_required_variable}}" STREQUAL ""
    )
        message(
            FATAL_ERROR
            "run_static_analysis.cmake requires "
            "${mc_lab_core_required_variable}."
        )
    endif()
endforeach()

if(
    MC_LAB_CORE_CLANG_TIDY MATCHES "-NOTFOUND$"
    OR NOT EXISTS "${MC_LAB_CORE_CLANG_TIDY}"
)
    message(
        FATAL_ERROR
        "clang-tidy was not found. Install it beside the selected LLVM "
        "toolchain or configure MC_LAB_CORE_CLANG_TIDY with its full path."
    )
endif()

if(NOT EXISTS "${MC_LAB_CORE_CLANG_TIDY_CONFIG}")
    message(
        FATAL_ERROR
        "clang-tidy configuration not found: "
        "${MC_LAB_CORE_CLANG_TIDY_CONFIG}"
    )
endif()

if(NOT EXISTS "${MC_LAB_CORE_COMPILE_DATABASE}/compile_commands.json")
    message(
        FATAL_ERROR
        "compile_commands.json was not generated in "
        "${MC_LAB_CORE_COMPILE_DATABASE}. Use a Ninja or Makefiles platform "
        "preset with CMAKE_EXPORT_COMPILE_COMMANDS enabled."
    )
endif()

if(NOT EXISTS "${MC_LAB_CORE_SOURCE_MANIFEST}")
    message(
        FATAL_ERROR
        "Static-analysis source manifest not found: "
        "${MC_LAB_CORE_SOURCE_MANIFEST}"
    )
endif()

include("${MC_LAB_CORE_SOURCE_MANIFEST}")

if(NOT MC_LAB_CORE_STATIC_ANALYSIS_SOURCES)
    message(
        FATAL_ERROR
        "No first-party C++ translation units were registered for static "
        "analysis. Call mc_lab_core_register_static_analysis_target() from "
        "each owning component."
    )
endif()

list(
    LENGTH
    MC_LAB_CORE_STATIC_ANALYSIS_SOURCES
    mc_lab_core_static_analysis_source_count
)

set(mc_lab_core_static_analysis_failures)
set(mc_lab_core_static_analysis_index 0)

foreach(
    mc_lab_core_static_analysis_source
    IN LISTS MC_LAB_CORE_STATIC_ANALYSIS_SOURCES
)
    math(
        EXPR
        mc_lab_core_static_analysis_index
        "${mc_lab_core_static_analysis_index} + 1"
    )

    message(
        STATUS
        "[${mc_lab_core_static_analysis_index}/"
        "${mc_lab_core_static_analysis_source_count}] "
        "clang-tidy: ${mc_lab_core_static_analysis_source}"
    )

    execute_process(
        COMMAND
            "${MC_LAB_CORE_CLANG_TIDY}"
            "--config-file=${MC_LAB_CORE_CLANG_TIDY_CONFIG}"
            "--quiet"
            "-p=${MC_LAB_CORE_COMPILE_DATABASE}"
            "${mc_lab_core_static_analysis_source}"
        RESULT_VARIABLE
            mc_lab_core_clang_tidy_result
        COMMAND_ECHO
            STDOUT
        ENCODING
            UTF-8
    )

    if(NOT mc_lab_core_clang_tidy_result EQUAL 0)
        list(
            APPEND
            mc_lab_core_static_analysis_failures
            "${mc_lab_core_static_analysis_source}"
        )
    endif()
endforeach()

if(mc_lab_core_static_analysis_failures)
    list(
        LENGTH
        mc_lab_core_static_analysis_failures
        mc_lab_core_static_analysis_failure_count
    )

    string(
        JOIN
        "\n  "
        mc_lab_core_static_analysis_failure_list
        ${mc_lab_core_static_analysis_failures}
    )

    message(
        FATAL_ERROR
        "clang-tidy failed for "
        "${mc_lab_core_static_analysis_failure_count} translation unit(s):\n"
        "  ${mc_lab_core_static_analysis_failure_list}"
    )
endif()

message(
    STATUS
    "clang-tidy passed for "
    "${mc_lab_core_static_analysis_source_count} translation unit(s)."
)
