cmake_minimum_required(VERSION 3.25)

# ==============================================================================
# Coverage Data Reset
# ==============================================================================

# The coverage workflow builds first, resets second, and executes tests third.
# Resetting after the instrumented build preserves compile-time .gcno notes
# while removing only runtime data and derived reports.

foreach(
    mc_lab_core_required_variable
    IN ITEMS
        MC_LAB_CORE_COVERAGE_BACKEND
        MC_LAB_CORE_BINARY_DIRECTORY
        MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY
)
    if(
        NOT DEFINED "${mc_lab_core_required_variable}"
        OR "${${mc_lab_core_required_variable}}" STREQUAL ""
    )
        message(
            FATAL_ERROR
            "reset_coverage.cmake requires "
            "${mc_lab_core_required_variable}."
        )
    endif()
endforeach()

if(NOT MC_LAB_CORE_COVERAGE_BACKEND MATCHES "^(LLVM|GCOV)$")
    message(
        FATAL_ERROR
        "Unsupported coverage backend "
        "'${MC_LAB_CORE_COVERAGE_BACKEND}'."
    )
endif()

cmake_path(
    NORMAL_PATH
    MC_LAB_CORE_BINARY_DIRECTORY
    OUTPUT_VARIABLE mc_lab_core_binary_directory
)

cmake_path(
    NORMAL_PATH
    MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY
    OUTPUT_VARIABLE mc_lab_core_coverage_report_directory
)

if(NOT IS_DIRECTORY "${mc_lab_core_binary_directory}")
    message(
        FATAL_ERROR
        "Coverage build directory not found: "
        "${mc_lab_core_binary_directory}"
    )
endif()

file(
    RELATIVE_PATH
    mc_lab_core_coverage_report_relative
    "${mc_lab_core_binary_directory}"
    "${mc_lab_core_coverage_report_directory}"
)

if(
    mc_lab_core_coverage_report_relative STREQUAL "."
    OR mc_lab_core_coverage_report_relative STREQUAL ".."
    OR mc_lab_core_coverage_report_relative MATCHES "^\\.\\.[/\\\\]"
)
    message(
        FATAL_ERROR
        "Refusing to reset coverage outside a dedicated build subdirectory: "
        "${mc_lab_core_coverage_report_directory}"
    )
endif()

if(MC_LAB_CORE_COVERAGE_BACKEND STREQUAL "GCOV")
    file(
        GLOB_RECURSE
        mc_lab_core_gcov_runtime_files
        LIST_DIRECTORIES FALSE
        "${mc_lab_core_binary_directory}/*.gcda"
    )

    if(mc_lab_core_gcov_runtime_files)
        file(
            REMOVE
            ${mc_lab_core_gcov_runtime_files}
        )
    endif()
endif()

# Object/source manifests are configure-time inputs and must survive reset.
file(
    REMOVE_RECURSE
    "${mc_lab_core_coverage_report_directory}/raw"
    "${mc_lab_core_coverage_report_directory}/html"
)

file(
    REMOVE
    "${mc_lab_core_coverage_report_directory}/coverage.info"
    "${mc_lab_core_coverage_report_directory}/coverage-unfiltered.info"
    "${mc_lab_core_coverage_report_directory}/coverage.profdata"
    "${mc_lab_core_coverage_report_directory}/coverage-summary.txt"
    "${mc_lab_core_coverage_report_directory}/profraw-files.txt"
)

file(
    MAKE_DIRECTORY
    "${mc_lab_core_coverage_report_directory}/raw"
)

message(
    STATUS
    "Coverage runtime data and derived reports were reset."
)
