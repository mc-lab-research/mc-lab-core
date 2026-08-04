include_guard(GLOBAL)

# ==============================================================================
# Target-Scoped Coverage Instrumentation and Reporting
# ==============================================================================

# Coverage is an explicit, isolated quality configuration. First-party targets
# opt into instrumentation through one helper, while repository-level report
# targets are created once after the complete product and test graph is known.
#
# Supported backends:
#
#   LLVM
#       Clang and clang-cl source-based coverage using llvm-profdata and
#       llvm-cov. This is the Windows clang-cl, Linux Clang, and macOS
#       AppleClang contract.
#
#   GCOV
#       GCC arc instrumentation using gcov, LCOV, and genhtml. This is the
#       Linux GCC contract.
#
# Both backends produce the same canonical artifact:
#
#   <build>/coverage/coverage.info
#
# Public API:
#
#   mc_lab_core_enable_coverage(<target>)
#       Applies coverage instrumentation to one first-party target.
#
#   mc_lab_core_finalize_coverage()
#       Creates coverage-reset, coverage-report, and coverage-check after all
#       first-party targets have registered.

set(
    MC_LAB_CORE_COVERAGE_LINE_THRESHOLD
    "90"
    CACHE STRING
    "Minimum first-party line coverage percentage"
)

set(
    MC_LAB_CORE_COVERAGE_BRANCH_THRESHOLD
    "90"
    CACHE STRING
    "Minimum first-party branch coverage percentage"
)

set(
    MC_LAB_CORE_COVERAGE_FUNCTION_THRESHOLD
    "90"
    CACHE STRING
    "Minimum first-party function coverage percentage"
)

set(
    MC_LAB_CORE_COVERAGE_SOURCE_PATHS
    "include;src;tools"
    CACHE STRING
    "Repository-relative production paths included in coverage policy"
)

set(
    MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY
    "${CMAKE_BINARY_DIR}/coverage"
    CACHE PATH
    "Directory containing MC-LAB-CORE coverage profiles and reports"
)

set(
    MC_LAB_CORE_COVERAGE_EXCLUDE_REGEX
    "([/\\\\](tests|third_party|external|vendor|_deps|CMakeFiles)[/\\\\])|(^/usr/)|(^/opt/)|([/\\\\]Program Files[/\\\\])"
    CACHE STRING
    "Path regular expression excluded from LLVM coverage reports"
)

mark_as_advanced(
    MC_LAB_CORE_COVERAGE_EXCLUDE_REGEX
    MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY
    MC_LAB_CORE_COVERAGE_SOURCE_PATHS
)

# The policy target always exists. It is empty in ordinary builds, allowing
# component CMake files to call mc_lab_core_enable_coverage() unconditionally.
if(TARGET mc_lab_core_coverage OR TARGET MC_LAB_CORE::Coverage)
    message(
        FATAL_ERROR
        "coverage.cmake cannot create MC_LAB_CORE::Coverage because the "
        "target already exists."
    )
endif()

add_library(
    mc_lab_core_coverage
    INTERFACE
)

add_library(
    MC_LAB_CORE::Coverage
    ALIAS
    mc_lab_core_coverage
)

set(
    MC_LAB_CORE_COVERAGE_BACKEND
    "OFF"
    CACHE INTERNAL
    "Coverage backend selected for the current MC-LAB-CORE build"
    FORCE
)

if(MC_LAB_CORE_ENABLE_COVERAGE)
    if(CMAKE_CONFIGURATION_TYPES)
        message(
            FATAL_ERROR
            "Coverage requires a single-configuration generator. Use the "
            "Ninja-based windows-clangcl-coverage, linux-gcc-coverage, "
            "linux-clang-coverage, or macos-appleclang-coverage preset."
        )
    endif()

    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(
            FATAL_ERROR
            "Coverage requires CMAKE_BUILD_TYPE=Debug so source-level results "
            "remain stable and easy to interpret. Selected build type: "
            "'${CMAKE_BUILD_TYPE}'."
        )
    endif()

    if(
        DEFINED MC_LAB_CORE_SANITIZER
        AND NOT MC_LAB_CORE_SANITIZER STREQUAL "OFF"
    )
        message(
            FATAL_ERROR
            "Coverage and runtime sanitizers must use separate build trees. "
            "MC_LAB_CORE_SANITIZER is '${MC_LAB_CORE_SANITIZER}'."
        )
    endif()

    foreach(
        mc_lab_core_coverage_threshold
        IN ITEMS
            MC_LAB_CORE_COVERAGE_LINE_THRESHOLD
            MC_LAB_CORE_COVERAGE_BRANCH_THRESHOLD
            MC_LAB_CORE_COVERAGE_FUNCTION_THRESHOLD
    )
        if(
            NOT "${${mc_lab_core_coverage_threshold}}" MATCHES "^[0-9]+$"
            OR "${${mc_lab_core_coverage_threshold}}" GREATER 100
        )
            message(
                FATAL_ERROR
                "${mc_lab_core_coverage_threshold} must be an integer from "
                "0 through 100; received "
                "'${${mc_lab_core_coverage_threshold}}'."
            )
        endif()
    endforeach()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(
            MC_LAB_CORE_COVERAGE_BACKEND
            "GCOV"
            CACHE INTERNAL
            "Coverage backend selected for the current MC-LAB-CORE build"
            FORCE
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|AppleClang)$")
        set(
            MC_LAB_CORE_COVERAGE_BACKEND
            "LLVM"
            CACHE INTERNAL
            "Coverage backend selected for the current MC-LAB-CORE build"
            FORCE
        )
    else()
        message(
            FATAL_ERROR
            "Coverage is supported with GNU GCC, Clang, AppleClang, and "
            "clang-cl. Compiler '${CMAKE_CXX_COMPILER_ID}' is not supported "
            "by the coverage module."
        )
    endif()

    get_filename_component(
        mc_lab_core_compiler_directory
        "${CMAKE_CXX_COMPILER}"
        DIRECTORY
    )

    if(MC_LAB_CORE_COVERAGE_BACKEND STREQUAL "LLVM")
        set(
            mc_lab_core_llvm_tool_hints
            "${mc_lab_core_compiler_directory}"
        )

        if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            find_program(
                MC_LAB_CORE_XCRUN
                NAMES
                    xcrun
                DOC
                    "xcrun executable used to locate Apple LLVM coverage tools"
            )

            mark_as_advanced(
                MC_LAB_CORE_XCRUN
            )

            if(MC_LAB_CORE_XCRUN)
                foreach(
                    mc_lab_core_apple_llvm_tool
                    IN ITEMS
                        llvm-profdata
                        llvm-cov
                )
                    execute_process(
                        COMMAND
                            "${MC_LAB_CORE_XCRUN}"
                            --find
                            "${mc_lab_core_apple_llvm_tool}"
                        RESULT_VARIABLE
                            mc_lab_core_xcrun_result
                        OUTPUT_VARIABLE
                            mc_lab_core_xcrun_tool
                        ERROR_QUIET
                        OUTPUT_STRIP_TRAILING_WHITESPACE
                        ENCODING
                            UTF-8
                    )

                    if(
                        mc_lab_core_xcrun_result EQUAL 0
                        AND EXISTS "${mc_lab_core_xcrun_tool}"
                    )
                        get_filename_component(
                            mc_lab_core_xcrun_tool_directory
                            "${mc_lab_core_xcrun_tool}"
                            DIRECTORY
                        )

                        list(
                            APPEND
                            mc_lab_core_llvm_tool_hints
                            "${mc_lab_core_xcrun_tool_directory}"
                        )
                    endif()
                endforeach()

                list(
                    REMOVE_DUPLICATES
                    mc_lab_core_llvm_tool_hints
                )
            endif()
        endif()

        find_program(
            MC_LAB_CORE_LLVM_PROFDATA
            NAMES
                llvm-profdata-22
                llvm-profdata-18
                llvm-profdata
            HINTS
                ${mc_lab_core_llvm_tool_hints}
            DOC
                "llvm-profdata executable used to merge MC-LAB-CORE profiles"
        )

        find_program(
            MC_LAB_CORE_LLVM_COV
            NAMES
                llvm-cov-22
                llvm-cov-18
                llvm-cov
            HINTS
                ${mc_lab_core_llvm_tool_hints}
            DOC
                "llvm-cov executable used to report MC-LAB-CORE coverage"
        )

        mark_as_advanced(
            MC_LAB_CORE_LLVM_COV
            MC_LAB_CORE_LLVM_PROFDATA
        )

        set(
            mc_lab_core_coverage_raw_profile_pattern
            "${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}/raw/%m_%p.profraw"
        )

        target_compile_options(
            mc_lab_core_coverage
            INTERFACE
                "-fprofile-instr-generate=${mc_lab_core_coverage_raw_profile_pattern}"
                -fcoverage-mapping
        )

        # The link flag selects the LLVM profile runtime. clang-cl with Ninja
        # may invoke lld-link directly; LLVM's linker accepts this driver-style
        # option and resolves the matching compiler-rt profile library.
        target_link_options(
            mc_lab_core_coverage
            INTERFACE
                -fprofile-instr-generate
        )
    else()
        find_program(
            MC_LAB_CORE_GCOV
            NAMES
                gcov
            HINTS
                "${mc_lab_core_compiler_directory}"
            DOC
                "gcov executable matching the GCC coverage compiler"
        )

        find_program(
            MC_LAB_CORE_LCOV
            NAMES
                lcov
            DOC
                "LCOV executable used to capture GCC coverage"
        )

        find_program(
            MC_LAB_CORE_GENHTML
            NAMES
                genhtml
            DOC
                "genhtml executable used to create the GCC HTML report"
        )

        mark_as_advanced(
            MC_LAB_CORE_GCOV
            MC_LAB_CORE_GENHTML
            MC_LAB_CORE_LCOV
        )

        target_compile_options(
            mc_lab_core_coverage
            INTERFACE
                --coverage
                -fprofile-abs-path
        )

        target_link_options(
            mc_lab_core_coverage
            INTERFACE
                --coverage
        )
    endif()
endif()

# Apply the coverage policy to one owning first-party target.
#
# Registration is idempotent and target-scoped. Aliases and imported targets
# are rejected so coverage instrumentation cannot leak into external code.
function(mc_lab_core_enable_coverage target_name)
    if(NOT TARGET "${target_name}")
        message(
            FATAL_ERROR
            "mc_lab_core_enable_coverage(): '${target_name}' is not a target."
        )
    endif()

    get_target_property(
        mc_lab_core_target_alias
        "${target_name}"
        ALIASED_TARGET
    )

    if(mc_lab_core_target_alias)
        message(
            FATAL_ERROR
            "mc_lab_core_enable_coverage(): '${target_name}' is an alias. "
            "Configure '${mc_lab_core_target_alias}' instead."
        )
    endif()

    get_target_property(
        mc_lab_core_target_imported
        "${target_name}"
        IMPORTED
    )

    if(mc_lab_core_target_imported)
        message(
            FATAL_ERROR
            "mc_lab_core_enable_coverage(): '${target_name}' is imported. "
            "Coverage is restricted to first-party targets."
        )
    endif()

    get_target_property(
        mc_lab_core_coverage_enabled
        "${target_name}"
        MC_LAB_CORE_COVERAGE_ENABLED
    )

    if(mc_lab_core_coverage_enabled)
        return()
    endif()

    get_target_property(
        mc_lab_core_target_type
        "${target_name}"
        TYPE
    )

    if(NOT mc_lab_core_target_type MATCHES
        "^(EXECUTABLE|STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY|OBJECT_LIBRARY|INTERFACE_LIBRARY)$"
    )
        message(
            FATAL_ERROR
            "mc_lab_core_enable_coverage(): target '${target_name}' has "
            "unsupported type '${mc_lab_core_target_type}'."
        )
    endif()

    if(mc_lab_core_target_type STREQUAL "INTERFACE_LIBRARY")
        target_link_libraries(
            "${target_name}"
            INTERFACE
                MC_LAB_CORE::Coverage
        )
    else()
        target_link_libraries(
            "${target_name}"
            PRIVATE
                MC_LAB_CORE::Coverage
        )
    endif()

    set_property(
        TARGET "${target_name}"
        PROPERTY MC_LAB_CORE_COVERAGE_ENABLED TRUE
    )

    if(NOT MC_LAB_CORE_ENABLE_COVERAGE)
        return()
    endif()

    set_property(
        GLOBAL
        APPEND
        PROPERTY MC_LAB_CORE_COVERAGE_TARGETS "${target_name}"
    )

    if(mc_lab_core_target_type MATCHES
        "^(EXECUTABLE|SHARED_LIBRARY|MODULE_LIBRARY)$"
    )
        set_property(
            GLOBAL
            APPEND
            PROPERTY MC_LAB_CORE_COVERAGE_OBJECT_TARGETS "${target_name}"
        )
    endif()
endfunction()

# Create repository-level coverage workflow targets after every production and
# test target has registered.
function(mc_lab_core_finalize_coverage)
    if(NOT MC_LAB_CORE_ENABLE_COVERAGE)
        return()
    endif()

    if(NOT PROJECT_IS_TOP_LEVEL)
        message(
            FATAL_ERROR
            "mc_lab_core_finalize_coverage() is a repository-level operation "
            "and requires MC-LAB-CORE to be the top-level project."
        )
    endif()

    get_property(
        mc_lab_core_coverage_finalized
        GLOBAL
        PROPERTY MC_LAB_CORE_COVERAGE_FINALIZED
    )

    if(mc_lab_core_coverage_finalized)
        return()
    endif()

    foreach(
        mc_lab_core_reserved_coverage_target
        IN ITEMS
            coverage-reset
            coverage-report
            coverage-check
    )
        if(TARGET "${mc_lab_core_reserved_coverage_target}")
            message(
                FATAL_ERROR
                "mc_lab_core_finalize_coverage(): target "
                "'${mc_lab_core_reserved_coverage_target}' already exists."
            )
        endif()
    endforeach()

    get_property(
        mc_lab_core_coverage_targets
        GLOBAL
        PROPERTY MC_LAB_CORE_COVERAGE_TARGETS
    )

    get_property(
        mc_lab_core_coverage_object_targets
        GLOBAL
        PROPERTY MC_LAB_CORE_COVERAGE_OBJECT_TARGETS
    )

    if(NOT mc_lab_core_coverage_targets)
        message(
            FATAL_ERROR
            "Coverage is enabled, but no first-party target registered. Call "
            "mc_lab_core_enable_coverage() for every production and test "
            "target."
        )
    endif()

    if(NOT mc_lab_core_coverage_object_targets)
        message(
            FATAL_ERROR
            "Coverage reporting requires at least one registered executable, "
            "shared library, or module library."
        )
    endif()

    list(
        REMOVE_DUPLICATES
        mc_lab_core_coverage_targets
    )

    list(
        REMOVE_DUPLICATES
        mc_lab_core_coverage_object_targets
    )

    list(
        SORT
        mc_lab_core_coverage_targets
    )

    list(
        SORT
        mc_lab_core_coverage_object_targets
    )

    # Keep every material coverage artifact inside the selected build tree.
    cmake_path(
        ABSOLUTE_PATH
        MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY
        BASE_DIRECTORY "${CMAKE_BINARY_DIR}"
        NORMALIZE
        OUTPUT_VARIABLE mc_lab_core_coverage_report_directory
    )

    file(
        RELATIVE_PATH
        mc_lab_core_coverage_report_relative
        "${CMAKE_BINARY_DIR}"
        "${mc_lab_core_coverage_report_directory}"
    )

    if(
        mc_lab_core_coverage_report_relative STREQUAL "."
        OR mc_lab_core_coverage_report_relative STREQUAL ".."
        OR mc_lab_core_coverage_report_relative MATCHES "^\\.\\.[/\\\\]"
    )
        message(
            FATAL_ERROR
            "MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY must be a dedicated "
            "directory inside CMAKE_BINARY_DIR; received "
            "'${MC_LAB_CORE_COVERAGE_REPORT_DIRECTORY}'."
        )
    endif()

    set(mc_lab_core_coverage_source_paths)

    foreach(
        mc_lab_core_coverage_source_path
        IN LISTS MC_LAB_CORE_COVERAGE_SOURCE_PATHS
    )
        if(
            mc_lab_core_coverage_source_path STREQUAL ""
            OR IS_ABSOLUTE "${mc_lab_core_coverage_source_path}"
        )
            message(
                FATAL_ERROR
                "MC_LAB_CORE_COVERAGE_SOURCE_PATHS must contain non-empty "
                "repository-relative paths."
            )
        endif()

        cmake_path(
            NORMAL_PATH
            mc_lab_core_coverage_source_path
            OUTPUT_VARIABLE mc_lab_core_coverage_source_path_normalized
        )

        if(
            mc_lab_core_coverage_source_path_normalized STREQUAL "."
            OR mc_lab_core_coverage_source_path_normalized STREQUAL ".."
            OR mc_lab_core_coverage_source_path_normalized MATCHES
                "^\\.\\.[/\\\\]"
        )
            message(
                FATAL_ERROR
                "Coverage source path "
                "'${mc_lab_core_coverage_source_path}' escapes or selects the "
                "repository root."
            )
        endif()

        list(
            APPEND
            mc_lab_core_coverage_source_paths
            "${mc_lab_core_coverage_source_path_normalized}"
        )
    endforeach()

    list(
        REMOVE_DUPLICATES
        mc_lab_core_coverage_source_paths
    )

    list(
        SORT
        mc_lab_core_coverage_source_paths
    )

    file(
        MAKE_DIRECTORY
        "${mc_lab_core_coverage_report_directory}"
    )

    set(
        mc_lab_core_coverage_object_manifest
        "${mc_lab_core_coverage_report_directory}/objects.cmake"
    )

    set(
        mc_lab_core_coverage_object_manifest_content
        "set(MC_LAB_CORE_COVERAGE_OBJECTS\n"
    )

    foreach(
        mc_lab_core_coverage_object_target
        IN LISTS mc_lab_core_coverage_object_targets
    )
        string(
            APPEND
            mc_lab_core_coverage_object_manifest_content
            "    \"$<TARGET_FILE:${mc_lab_core_coverage_object_target}>\"\n"
        )
    endforeach()

    string(
        APPEND
        mc_lab_core_coverage_object_manifest_content
        ")\n"
    )

    file(
        GENERATE
        OUTPUT
            "${mc_lab_core_coverage_object_manifest}"
        CONTENT
            "${mc_lab_core_coverage_object_manifest_content}"
    )

    set(
        mc_lab_core_coverage_source_manifest
        "${mc_lab_core_coverage_report_directory}/source-paths.cmake"
    )

    file(
        WRITE
        "${mc_lab_core_coverage_source_manifest}"
        "set(MC_LAB_CORE_COVERAGE_SOURCE_PATHS\n"
    )

    foreach(
        mc_lab_core_coverage_source_path
        IN LISTS mc_lab_core_coverage_source_paths
    )
        cmake_path(
            APPEND
            PROJECT_SOURCE_DIR
            "${mc_lab_core_coverage_source_path}"
            OUTPUT_VARIABLE mc_lab_core_coverage_source_path_absolute
        )

        cmake_path(
            NORMAL_PATH
            mc_lab_core_coverage_source_path_absolute
            OUTPUT_VARIABLE mc_lab_core_coverage_source_path_absolute
        )

        string(
            REPLACE
            "\""
            "\\\""
            mc_lab_core_coverage_source_path_escaped
            "${mc_lab_core_coverage_source_path_absolute}"
        )

        file(
            APPEND
            "${mc_lab_core_coverage_source_manifest}"
            "    \"${mc_lab_core_coverage_source_path_escaped}\"\n"
        )
    endforeach()

    file(
        APPEND
        "${mc_lab_core_coverage_source_manifest}"
        ")\n"
    )

    set(
        mc_lab_core_coverage_info
        "${mc_lab_core_coverage_report_directory}/coverage.info"
    )

    set(
        mc_lab_core_coverage_report_byproducts
        "${mc_lab_core_coverage_report_directory}/coverage-summary.txt"
        "${mc_lab_core_coverage_report_directory}/html/index.html"
    )

    if(MC_LAB_CORE_COVERAGE_BACKEND STREQUAL "LLVM")
        list(
            APPEND
            mc_lab_core_coverage_report_byproducts
            "${mc_lab_core_coverage_report_directory}/coverage.profdata"
        )
    else()
        list(
            APPEND
            mc_lab_core_coverage_report_byproducts
            "${mc_lab_core_coverage_report_directory}/coverage-unfiltered.info"
        )
    endif()

    add_custom_target(
        coverage-reset
        COMMAND
            "${CMAKE_COMMAND}"
            "-DMC_LAB_CORE_COVERAGE_BACKEND=${MC_LAB_CORE_COVERAGE_BACKEND}"
            "-DMC_LAB_CORE_BINARY_DIRECTORY=${CMAKE_BINARY_DIR}"
            "-DMC_LAB_CORE_COVERAGE_REPORT_DIRECTORY=${mc_lab_core_coverage_report_directory}"
            -P
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../scripts/reset_coverage.cmake"
        COMMENT
            "Removing stale runtime coverage data and reports"
        USES_TERMINAL
        VERBATIM
    )

    add_custom_command(
        OUTPUT
            "${mc_lab_core_coverage_info}"
        COMMAND
            "${CMAKE_COMMAND}"
            "-DMC_LAB_CORE_COVERAGE_BACKEND=${MC_LAB_CORE_COVERAGE_BACKEND}"
            "-DMC_LAB_CORE_BINARY_DIRECTORY=${CMAKE_BINARY_DIR}"
            "-DMC_LAB_CORE_SOURCE_DIRECTORY=${PROJECT_SOURCE_DIR}"
            "-DMC_LAB_CORE_COVERAGE_REPORT_DIRECTORY=${mc_lab_core_coverage_report_directory}"
            "-DMC_LAB_CORE_COVERAGE_OBJECT_MANIFEST=${mc_lab_core_coverage_object_manifest}"
            "-DMC_LAB_CORE_COVERAGE_SOURCE_MANIFEST=${mc_lab_core_coverage_source_manifest}"
            "-DMC_LAB_CORE_COVERAGE_EXCLUDE_REGEX=${MC_LAB_CORE_COVERAGE_EXCLUDE_REGEX}"
            "-DMC_LAB_CORE_LLVM_PROFDATA=${MC_LAB_CORE_LLVM_PROFDATA}"
            "-DMC_LAB_CORE_LLVM_COV=${MC_LAB_CORE_LLVM_COV}"
            "-DMC_LAB_CORE_GCOV=${MC_LAB_CORE_GCOV}"
            "-DMC_LAB_CORE_LCOV=${MC_LAB_CORE_LCOV}"
            "-DMC_LAB_CORE_GENHTML=${MC_LAB_CORE_GENHTML}"
            -P
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../scripts/generate_coverage_report.cmake"
        DEPENDS
            ${mc_lab_core_coverage_object_targets}
        BYPRODUCTS
            ${mc_lab_core_coverage_report_byproducts}
        COMMENT
            "Generating normalized LCOV and HTML coverage reports"
        USES_TERMINAL
        VERBATIM
    )

    add_custom_target(
        coverage-report
        DEPENDS
            "${mc_lab_core_coverage_info}"
    )

    add_custom_target(
        coverage-check
        COMMAND
            "${CMAKE_COMMAND}"
            "-DMC_LAB_CORE_COVERAGE_INFO=${mc_lab_core_coverage_info}"
            "-DMC_LAB_CORE_COVERAGE_SOURCE_MANIFEST=${mc_lab_core_coverage_source_manifest}"
            "-DMC_LAB_CORE_COVERAGE_LINE_THRESHOLD=${MC_LAB_CORE_COVERAGE_LINE_THRESHOLD}"
            "-DMC_LAB_CORE_COVERAGE_BRANCH_THRESHOLD=${MC_LAB_CORE_COVERAGE_BRANCH_THRESHOLD}"
            "-DMC_LAB_CORE_COVERAGE_FUNCTION_THRESHOLD=${MC_LAB_CORE_COVERAGE_FUNCTION_THRESHOLD}"
            -P
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../scripts/check_coverage.cmake"
        DEPENDS
            "${mc_lab_core_coverage_info}"
        COMMENT
            "Checking first-party coverage thresholds"
        USES_TERMINAL
        VERBATIM
    )

    set_target_properties(
        coverage-reset
        coverage-report
        coverage-check
        PROPERTIES
            FOLDER "quality"
    )

    set_property(
        GLOBAL
        PROPERTY MC_LAB_CORE_COVERAGE_FINALIZED TRUE
    )
endfunction()
