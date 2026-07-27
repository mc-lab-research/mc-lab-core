cmake_minimum_required(VERSION 3.25)

# ==============================================================================
# Normalized Coverage Report Generation
# ==============================================================================

# LLVM and GCC use different runtime formats, but both backends produce:
#
#   coverage.info          Canonical LCOV artifact for policy and Codecov.
#   coverage-summary.txt   Human-readable backend summary.
#   html/index.html        Local browsable report.
#
# Strict tool validation happens here rather than during ordinary platform
# configuration.

foreach(
    mc_lab_core_required_variable
    IN ITEMS
        MC_LAB_CORE_COVERAGE_BACKEND
        MC_LAB_CORE_BINARY_DIRECTORY
        MC_LAB_CORE_SOURCE_DIRECTORY
        MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY
        MC_LAB_CORE_COVERAGE_OBJECT_MANIFEST
        MC_LAB_CORE_COVERAGE_SOURCE_MANIFEST
)
    if(
        NOT DEFINED "${mc_lab_core_required_variable}"
        OR "${${mc_lab_core_required_variable}}" STREQUAL ""
    )
        message(
            FATAL_ERROR
            "generate_coverage_report.cmake requires "
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

if(NOT EXISTS "${MC_LAB_CORE_COVERAGE_OBJECT_MANIFEST}")
    message(
        FATAL_ERROR
        "Coverage object manifest not found: "
        "${MC_LAB_CORE_COVERAGE_OBJECT_MANIFEST}"
    )
endif()

if(NOT EXISTS "${MC_LAB_CORE_COVERAGE_SOURCE_MANIFEST}")
    message(
        FATAL_ERROR
        "Coverage source-path manifest not found: "
        "${MC_LAB_CORE_COVERAGE_SOURCE_MANIFEST}"
    )
endif()

include("${MC_LAB_CORE_COVERAGE_OBJECT_MANIFEST}")
include("${MC_LAB_CORE_COVERAGE_SOURCE_MANIFEST}")

if(NOT MC_LAB_CORE_COVERAGE_OBJECTS)
    message(
        FATAL_ERROR
        "The coverage object manifest is empty."
    )
endif()

if(NOT MC_LAB_CORE_COVERAGE_SOURCE_PATHS)
    message(
        FATAL_ERROR
        "The coverage source-path manifest is empty."
    )
endif()

foreach(
    mc_lab_core_coverage_object
    IN LISTS MC_LAB_CORE_COVERAGE_OBJECTS
)
    if(NOT EXISTS "${mc_lab_core_coverage_object}")
        message(
            FATAL_ERROR
            "Instrumented coverage object not found: "
            "${mc_lab_core_coverage_object}"
        )
    endif()
endforeach()

file(
    MAKE_DIRECTORY
    "${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}"
    "${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}/html"
)

set(
    mc_lab_core_coverage_info
    "${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}/coverage.info"
)

set(
    mc_lab_core_coverage_summary
    "${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}/coverage-summary.txt"
)

if(MC_LAB_CORE_COVERAGE_BACKEND STREQUAL "LLVM")
    foreach(
        mc_lab_core_llvm_tool
        IN ITEMS
            MC_LAB_CORE_LLVM_PROFDATA
            MC_LAB_CORE_LLVM_COV
    )
        if(
            NOT DEFINED "${mc_lab_core_llvm_tool}"
            OR "${${mc_lab_core_llvm_tool}}" STREQUAL ""
            OR "${${mc_lab_core_llvm_tool}}" MATCHES "-NOTFOUND$"
            OR NOT EXISTS "${${mc_lab_core_llvm_tool}}"
        )
            message(
                FATAL_ERROR
                "${mc_lab_core_llvm_tool} was not found. Install matching "
                "LLVM coverage tools or configure its full path."
            )
        endif()
    endforeach()

    file(
        GLOB_RECURSE
        mc_lab_core_raw_profiles
        LIST_DIRECTORIES FALSE
        "${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}/raw/*.profraw"
    )

    if(NOT mc_lab_core_raw_profiles)
        message(
            FATAL_ERROR
            "No LLVM .profraw files were found. Run coverage-reset, execute "
            "the instrumented test suite, then generate the report."
        )
    endif()

    list(
        SORT
        mc_lab_core_raw_profiles
    )

    set(
        mc_lab_core_profile_input_list
        "${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}/profraw-files.txt"
    )

    file(
        WRITE
        "${mc_lab_core_profile_input_list}"
        ""
    )

    foreach(mc_lab_core_raw_profile IN LISTS mc_lab_core_raw_profiles)
        file(
            APPEND
            "${mc_lab_core_profile_input_list}"
            "${mc_lab_core_raw_profile}\n"
        )
    endforeach()

    set(
        mc_lab_core_merged_profile
        "${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}/coverage.profdata"
    )

    execute_process(
        COMMAND
            "${MC_LAB_CORE_LLVM_PROFDATA}"
            merge
            --sparse
            "--input-files=${mc_lab_core_profile_input_list}"
            "--output=${mc_lab_core_merged_profile}"
        RESULT_VARIABLE
            mc_lab_core_profdata_result
        ERROR_VARIABLE
            mc_lab_core_profdata_error
        ENCODING
            UTF-8
    )

    if(NOT mc_lab_core_profdata_result EQUAL 0)
        message(
            FATAL_ERROR
            "llvm-profdata could not merge runtime profiles.\n"
            "${mc_lab_core_profdata_error}"
        )
    endif()

    list(
        GET
        MC_LAB_CORE_COVERAGE_OBJECTS
        0
        mc_lab_core_primary_coverage_object
    )

    set(mc_lab_core_additional_object_arguments)

    list(
        LENGTH
        MC_LAB_CORE_COVERAGE_OBJECTS
        mc_lab_core_coverage_object_count
    )

    if(mc_lab_core_coverage_object_count GREATER 1)
        math(
            EXPR
            mc_lab_core_last_coverage_object_index
            "${mc_lab_core_coverage_object_count} - 1"
        )

        foreach(
            mc_lab_core_coverage_object_index
            RANGE 1 "${mc_lab_core_last_coverage_object_index}"
        )
            list(
                GET
                MC_LAB_CORE_COVERAGE_OBJECTS
                "${mc_lab_core_coverage_object_index}"
                mc_lab_core_additional_coverage_object
            )

            list(
                APPEND
                mc_lab_core_additional_object_arguments
                "--object=${mc_lab_core_additional_coverage_object}"
            )
        endforeach()
    endif()

    set(mc_lab_core_llvm_filter_arguments)

    if(
        DEFINED MC_LAB_CORE_COVERAGE_EXCLUDE_REGEX
        AND NOT MC_LAB_CORE_COVERAGE_EXCLUDE_REGEX STREQUAL ""
    )
        list(
            APPEND
            mc_lab_core_llvm_filter_arguments
            "--ignore-filename-regex=${MC_LAB_CORE_COVERAGE_EXCLUDE_REGEX}"
        )
    endif()

    execute_process(
        COMMAND
            "${MC_LAB_CORE_LLVM_COV}"
            export
            --format=lcov
            "--instr-profile=${mc_lab_core_merged_profile}"
            ${mc_lab_core_llvm_filter_arguments}
            "${mc_lab_core_primary_coverage_object}"
            ${mc_lab_core_additional_object_arguments}
        RESULT_VARIABLE
            mc_lab_core_llvm_export_result
        OUTPUT_FILE
            "${mc_lab_core_coverage_info}"
        ERROR_VARIABLE
            mc_lab_core_llvm_export_error
        ENCODING
            UTF-8
    )

    if(NOT mc_lab_core_llvm_export_result EQUAL 0)
        message(
            FATAL_ERROR
            "llvm-cov could not export the LCOV report.\n"
            "${mc_lab_core_llvm_export_error}"
        )
    endif()

    execute_process(
        COMMAND
            "${MC_LAB_CORE_LLVM_COV}"
            report
            "--instr-profile=${mc_lab_core_merged_profile}"
            ${mc_lab_core_llvm_filter_arguments}
            "${mc_lab_core_primary_coverage_object}"
            ${mc_lab_core_additional_object_arguments}
        RESULT_VARIABLE
            mc_lab_core_llvm_summary_result
        OUTPUT_FILE
            "${mc_lab_core_coverage_summary}"
        ERROR_VARIABLE
            mc_lab_core_llvm_summary_error
        ENCODING
            UTF-8
    )

    if(NOT mc_lab_core_llvm_summary_result EQUAL 0)
        message(
            FATAL_ERROR
            "llvm-cov could not generate the text summary.\n"
            "${mc_lab_core_llvm_summary_error}"
        )
    endif()

    execute_process(
        COMMAND
            "${MC_LAB_CORE_LLVM_COV}"
            show
            --format=html
            "--output-dir=${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}/html"
            "--instr-profile=${mc_lab_core_merged_profile}"
            ${mc_lab_core_llvm_filter_arguments}
            "${mc_lab_core_primary_coverage_object}"
            ${mc_lab_core_additional_object_arguments}
        RESULT_VARIABLE
            mc_lab_core_llvm_html_result
        ERROR_VARIABLE
            mc_lab_core_llvm_html_error
        ENCODING
            UTF-8
    )

    if(NOT mc_lab_core_llvm_html_result EQUAL 0)
        message(
            FATAL_ERROR
            "llvm-cov could not generate the HTML report.\n"
            "${mc_lab_core_llvm_html_error}"
        )
    endif()
else()
    foreach(
        mc_lab_core_gcov_tool
        IN ITEMS
            MC_LAB_CORE_GCOV
            MC_LAB_CORE_LCOV
            MC_LAB_CORE_GENHTML
    )
        if(
            NOT DEFINED "${mc_lab_core_gcov_tool}"
            OR "${${mc_lab_core_gcov_tool}}" STREQUAL ""
            OR "${${mc_lab_core_gcov_tool}}" MATCHES "-NOTFOUND$"
            OR NOT EXISTS "${${mc_lab_core_gcov_tool}}"
        )
            message(
                FATAL_ERROR
                "${mc_lab_core_gcov_tool} was not found. Install gcov and "
                "LCOV tooling or configure its full path."
            )
        endif()
    endforeach()

    file(
        GLOB_RECURSE
        mc_lab_core_gcov_runtime_files
        LIST_DIRECTORIES FALSE
        "${MC_LAB_CORE_BINARY_DIRECTORY}/*.gcda"
    )

    if(NOT mc_lab_core_gcov_runtime_files)
        message(
            FATAL_ERROR
            "No GCC .gcda files were found. Run coverage-reset, execute the "
            "instrumented test suite, then generate the report."
        )
    endif()

    set(
        mc_lab_core_unfiltered_coverage_info
        "${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}/coverage-unfiltered.info"
    )

    execute_process(
        COMMAND
            "${MC_LAB_CORE_LCOV}"
            --capture
            --directory
            "${MC_LAB_CORE_BINARY_DIRECTORY}"
            --output-file
            "${mc_lab_core_unfiltered_coverage_info}"
            --gcov-tool
            "${MC_LAB_CORE_GCOV}"
            --branch-coverage
            --no-external
        RESULT_VARIABLE
            mc_lab_core_lcov_capture_result
        ERROR_VARIABLE
            mc_lab_core_lcov_capture_error
        ENCODING
            UTF-8
    )

    if(NOT mc_lab_core_lcov_capture_result EQUAL 0)
        message(
            FATAL_ERROR
            "LCOV could not capture GCC coverage data.\n"
            "${mc_lab_core_lcov_capture_error}"
        )
    endif()

    set(mc_lab_core_lcov_source_patterns)

    foreach(
        mc_lab_core_coverage_source_path
        IN LISTS MC_LAB_CORE_COVERAGE_SOURCE_PATHS
    )
        if(IS_DIRECTORY "${mc_lab_core_coverage_source_path}")
            list(
                APPEND
                mc_lab_core_lcov_source_patterns
                "${mc_lab_core_coverage_source_path}/*"
            )
        endif()
    endforeach()

    if(NOT mc_lab_core_lcov_source_patterns)
        message(
            FATAL_ERROR
            "None of the configured production coverage paths exists."
        )
    endif()

    execute_process(
        COMMAND
            "${MC_LAB_CORE_LCOV}"
            --extract
            "${mc_lab_core_unfiltered_coverage_info}"
            ${mc_lab_core_lcov_source_patterns}
            --output-file
            "${mc_lab_core_coverage_info}"
            --branch-coverage
        RESULT_VARIABLE
            mc_lab_core_lcov_extract_result
        ERROR_VARIABLE
            mc_lab_core_lcov_extract_error
        ENCODING
            UTF-8
    )

    if(NOT mc_lab_core_lcov_extract_result EQUAL 0)
        message(
            FATAL_ERROR
            "LCOV could not restrict the report to first-party production "
            "paths.\n${mc_lab_core_lcov_extract_error}"
        )
    endif()

    execute_process(
        COMMAND
            "${MC_LAB_CORE_LCOV}"
            --summary
            "${mc_lab_core_coverage_info}"
            --branch-coverage
        RESULT_VARIABLE
            mc_lab_core_lcov_summary_result
        OUTPUT_VARIABLE
            mc_lab_core_lcov_summary_output
        ERROR_VARIABLE
            mc_lab_core_lcov_summary_error
        ENCODING
            UTF-8
    )

    if(NOT mc_lab_core_lcov_summary_result EQUAL 0)
        message(
            FATAL_ERROR
            "LCOV could not generate the text summary.\n"
            "${mc_lab_core_lcov_summary_error}"
        )
    endif()

    file(
        WRITE
        "${mc_lab_core_coverage_summary}"
        "${mc_lab_core_lcov_summary_output}"
        "${mc_lab_core_lcov_summary_error}"
    )

    execute_process(
        COMMAND
            "${MC_LAB_CORE_GENHTML}"
            --branch-coverage
            --output-directory
            "${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}/html"
            "${mc_lab_core_coverage_info}"
        RESULT_VARIABLE
            mc_lab_core_genhtml_result
        ERROR_VARIABLE
            mc_lab_core_genhtml_error
        ENCODING
            UTF-8
    )

    if(NOT mc_lab_core_genhtml_result EQUAL 0)
        message(
            FATAL_ERROR
            "genhtml could not generate the HTML report.\n"
            "${mc_lab_core_genhtml_error}"
        )
    endif()
endif()

if(
    NOT EXISTS "${mc_lab_core_coverage_info}"
    OR NOT EXISTS "${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}/html/index.html"
)
    message(
        FATAL_ERROR
        "Coverage reporting completed without producing all canonical "
        "artifacts."
    )
endif()

file(
    SIZE
    "${mc_lab_core_coverage_info}"
    mc_lab_core_coverage_info_size
)

if(mc_lab_core_coverage_info_size EQUAL 0)
    message(
        FATAL_ERROR
        "The generated LCOV report is empty."
    )
endif()

message(
    STATUS
    "Coverage report generated:\n"
    "  LCOV: ${mc_lab_core_coverage_info}\n"
    "  HTML: ${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}/html/index.html\n"
    "  Summary: ${mc_lab_core_coverage_summary}"
)
