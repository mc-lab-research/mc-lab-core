cmake_minimum_required(VERSION 3.25)

# ==============================================================================
# Coverage Policy Gate
# ==============================================================================

# The canonical LCOV artifact normalizes LLVM and GCC coverage. This script
# validates its first-party scope and enforces repository-wide line, branch,
# and function thresholds without depending on backend-specific output text.

foreach(
    mc_lab_core_required_variable
    IN ITEMS
        MC_LAB_CORE_COVERAGE_INFO
        MC_LAB_CORE_COVERAGE_SOURCE_MANIFEST
        MC_LAB_CORE_COVERAGE_LINE_THRESHOLD
        MC_LAB_CORE_COVERAGE_BRANCH_THRESHOLD
        MC_LAB_CORE_COVERAGE_FUNCTION_THRESHOLD
)
    if(
        NOT DEFINED "${mc_lab_core_required_variable}"
        OR "${${mc_lab_core_required_variable}}" STREQUAL ""
    )
        message(
            FATAL_ERROR
            "check_coverage.cmake requires "
            "${mc_lab_core_required_variable}."
        )
    endif()
endforeach()

foreach(
    mc_lab_core_threshold_variable
    IN ITEMS
        MC_LAB_CORE_COVERAGE_LINE_THRESHOLD
        MC_LAB_CORE_COVERAGE_BRANCH_THRESHOLD
        MC_LAB_CORE_COVERAGE_FUNCTION_THRESHOLD
)
    if(
        NOT "${${mc_lab_core_threshold_variable}}" MATCHES "^[0-9]+$"
        OR "${${mc_lab_core_threshold_variable}}" GREATER 100
    )
        message(
            FATAL_ERROR
            "${mc_lab_core_threshold_variable} must be an integer from 0 "
            "through 100."
        )
    endif()
endforeach()

if(NOT EXISTS "${MC_LAB_CORE_COVERAGE_INFO}")
    message(
        FATAL_ERROR
        "Coverage report not found: ${MC_LAB_CORE_COVERAGE_INFO}"
    )
endif()

if(NOT EXISTS "${MC_LAB_CORE_COVERAGE_SOURCE_MANIFEST}")
    message(
        FATAL_ERROR
        "Coverage source-path manifest not found: "
        "${MC_LAB_CORE_COVERAGE_SOURCE_MANIFEST}"
    )
endif()

include("${MC_LAB_CORE_COVERAGE_SOURCE_MANIFEST}")

if(NOT MC_LAB_CORE_COVERAGE_SOURCE_PATHS)
    message(
        FATAL_ERROR
        "The coverage source-path manifest is empty."
    )
endif()

file(
    STRINGS
    "${MC_LAB_CORE_COVERAGE_INFO}"
    mc_lab_core_coverage_metric_lines
    REGEX "^(SF:|LF:|LH:|BRF:|BRH:|FNF:|FNH:)"
    ENCODING UTF-8
)

set(mc_lab_core_source_file_count 0)
set(mc_lab_core_lines_found 0)
set(mc_lab_core_lines_hit 0)
set(mc_lab_core_branches_found 0)
set(mc_lab_core_branches_hit 0)
set(mc_lab_core_functions_found 0)
set(mc_lab_core_functions_hit 0)
set(mc_lab_core_out_of_scope_sources)

foreach(
    mc_lab_core_coverage_metric_line
    IN LISTS mc_lab_core_coverage_metric_lines
)
    if(mc_lab_core_coverage_metric_line MATCHES "^SF:(.*)$")
        set(
            mc_lab_core_coverage_source_file
            "${CMAKE_MATCH_1}"
        )

        math(
            EXPR
            mc_lab_core_source_file_count
            "${mc_lab_core_source_file_count} + 1"
        )

        file(
            TO_CMAKE_PATH
            "${mc_lab_core_coverage_source_file}"
            mc_lab_core_coverage_source_file
        )

        cmake_path(
            NORMAL_PATH
            mc_lab_core_coverage_source_file
            OUTPUT_VARIABLE mc_lab_core_coverage_source_file
        )

        set(mc_lab_core_source_is_first_party FALSE)

        foreach(
            mc_lab_core_coverage_source_path
            IN LISTS MC_LAB_CORE_COVERAGE_SOURCE_PATHS
        )
            file(
                RELATIVE_PATH
                mc_lab_core_source_relative
                "${mc_lab_core_coverage_source_path}"
                "${mc_lab_core_coverage_source_file}"
            )

            if(
                NOT mc_lab_core_source_relative STREQUAL ".."
                AND NOT mc_lab_core_source_relative MATCHES "^\\.\\.[/\\\\]"
            )
                set(mc_lab_core_source_is_first_party TRUE)
                break()
            endif()
        endforeach()

        if(NOT mc_lab_core_source_is_first_party)
            list(
                APPEND
                mc_lab_core_out_of_scope_sources
                "${mc_lab_core_coverage_source_file}"
            )
        endif()
    elseif(mc_lab_core_coverage_metric_line MATCHES "^LF:([0-9]+)$")
        math(
            EXPR
            mc_lab_core_lines_found
            "${mc_lab_core_lines_found} + ${CMAKE_MATCH_1}"
        )
    elseif(mc_lab_core_coverage_metric_line MATCHES "^LH:([0-9]+)$")
        math(
            EXPR
            mc_lab_core_lines_hit
            "${mc_lab_core_lines_hit} + ${CMAKE_MATCH_1}"
        )
    elseif(mc_lab_core_coverage_metric_line MATCHES "^BRF:([0-9]+)$")
        math(
            EXPR
            mc_lab_core_branches_found
            "${mc_lab_core_branches_found} + ${CMAKE_MATCH_1}"
        )
    elseif(mc_lab_core_coverage_metric_line MATCHES "^BRH:([0-9]+)$")
        math(
            EXPR
            mc_lab_core_branches_hit
            "${mc_lab_core_branches_hit} + ${CMAKE_MATCH_1}"
        )
    elseif(mc_lab_core_coverage_metric_line MATCHES "^FNF:([0-9]+)$")
        math(
            EXPR
            mc_lab_core_functions_found
            "${mc_lab_core_functions_found} + ${CMAKE_MATCH_1}"
        )
    elseif(mc_lab_core_coverage_metric_line MATCHES "^FNH:([0-9]+)$")
        math(
            EXPR
            mc_lab_core_functions_hit
            "${mc_lab_core_functions_hit} + ${CMAKE_MATCH_1}"
        )
    endif()
endforeach()

if(mc_lab_core_out_of_scope_sources)
    list(
        REMOVE_DUPLICATES
        mc_lab_core_out_of_scope_sources
    )

    string(
        JOIN
        "\n  "
        mc_lab_core_out_of_scope_source_list
        ${mc_lab_core_out_of_scope_sources}
    )

    message(
        FATAL_ERROR
        "The normalized coverage report contains files outside the configured "
        "first-party production paths:\n"
        "  ${mc_lab_core_out_of_scope_source_list}\n"
        "Update instrumentation ownership or the explicit coverage exclusion "
        "policy."
    )
endif()

if(mc_lab_core_source_file_count EQUAL 0 OR mc_lab_core_lines_found EQUAL 0)
    message(
        FATAL_ERROR
        "The coverage report contains no first-party executable lines."
    )
endif()

function(
    mc_lab_core_coverage_percentage
    hit_count
    found_count
    output_percentage
    output_scaled
)
    if(found_count EQUAL 0)
        set(mc_lab_core_percentage_scaled 10000)
    else()
        math(
            EXPR
            mc_lab_core_percentage_scaled
            "(${hit_count} * 10000) / ${found_count}"
        )
    endif()

    math(
        EXPR
        mc_lab_core_percentage_whole
        "${mc_lab_core_percentage_scaled} / 100"
    )

    math(
        EXPR
        mc_lab_core_percentage_fraction
        "${mc_lab_core_percentage_scaled} % 100"
    )

    if(mc_lab_core_percentage_fraction LESS 10)
        set(
            mc_lab_core_percentage_fraction
            "0${mc_lab_core_percentage_fraction}"
        )
    endif()

    set(
        "${output_percentage}"
        "${mc_lab_core_percentage_whole}.${mc_lab_core_percentage_fraction}%"
        PARENT_SCOPE
    )

    set(
        "${output_scaled}"
        "${mc_lab_core_percentage_scaled}"
        PARENT_SCOPE
    )
endfunction()

mc_lab_core_coverage_percentage(
    "${mc_lab_core_lines_hit}"
    "${mc_lab_core_lines_found}"
    mc_lab_core_line_percentage
    mc_lab_core_line_percentage_scaled
)

mc_lab_core_coverage_percentage(
    "${mc_lab_core_branches_hit}"
    "${mc_lab_core_branches_found}"
    mc_lab_core_branch_percentage
    mc_lab_core_branch_percentage_scaled
)

mc_lab_core_coverage_percentage(
    "${mc_lab_core_functions_hit}"
    "${mc_lab_core_functions_found}"
    mc_lab_core_function_percentage
    mc_lab_core_function_percentage_scaled
)

message(STATUS "MC-LAB-CORE coverage policy")
message(
    STATUS
    "  Lines:     ${mc_lab_core_line_percentage} "
    "(${mc_lab_core_lines_hit}/${mc_lab_core_lines_found}, required "
    "${MC_LAB_CORE_COVERAGE_LINE_THRESHOLD}%)"
)
message(
    STATUS
    "  Branches:  ${mc_lab_core_branch_percentage} "
    "(${mc_lab_core_branches_hit}/${mc_lab_core_branches_found}, required "
    "${MC_LAB_CORE_COVERAGE_BRANCH_THRESHOLD}%)"
)
message(
    STATUS
    "  Functions: ${mc_lab_core_function_percentage} "
    "(${mc_lab_core_functions_hit}/${mc_lab_core_functions_found}, required "
    "${MC_LAB_CORE_COVERAGE_FUNCTION_THRESHOLD}%)"
)

set(mc_lab_core_coverage_failures)

math(
    EXPR
    mc_lab_core_line_threshold_scaled
    "${MC_LAB_CORE_COVERAGE_LINE_THRESHOLD} * 100"
)

math(
    EXPR
    mc_lab_core_branch_threshold_scaled
    "${MC_LAB_CORE_COVERAGE_BRANCH_THRESHOLD} * 100"
)

math(
    EXPR
    mc_lab_core_function_threshold_scaled
    "${MC_LAB_CORE_COVERAGE_FUNCTION_THRESHOLD} * 100"
)

if(mc_lab_core_line_percentage_scaled LESS mc_lab_core_line_threshold_scaled)
    list(
        APPEND
        mc_lab_core_coverage_failures
        "line coverage ${mc_lab_core_line_percentage} < ${MC_LAB_CORE_COVERAGE_LINE_THRESHOLD}%"
    )
endif()

if(
    mc_lab_core_branch_percentage_scaled
    LESS mc_lab_core_branch_threshold_scaled
)
    list(
        APPEND
        mc_lab_core_coverage_failures
        "branch coverage ${mc_lab_core_branch_percentage} < ${MC_LAB_CORE_COVERAGE_BRANCH_THRESHOLD}%"
    )
endif()

if(
    mc_lab_core_function_percentage_scaled
    LESS mc_lab_core_function_threshold_scaled
)
    list(
        APPEND
        mc_lab_core_coverage_failures
        "function coverage ${mc_lab_core_function_percentage} < ${MC_LAB_CORE_COVERAGE_FUNCTION_THRESHOLD}%"
    )
endif()

if(mc_lab_core_coverage_failures)
    string(
        JOIN
        "\n  "
        mc_lab_core_coverage_failure_list
        ${mc_lab_core_coverage_failures}
    )

    message(
        FATAL_ERROR
        "Coverage policy failed:\n"
        "  ${mc_lab_core_coverage_failure_list}"
    )
endif()

message(
    STATUS
    "Coverage policy passed for "
    "${mc_lab_core_source_file_count} source file(s)."
)
