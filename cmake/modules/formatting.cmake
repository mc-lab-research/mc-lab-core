include_guard(GLOBAL)

# ==============================================================================
# Repository Formatting Targets
# ==============================================================================

# Formatting is an explicit repository operation rather than a compilation
# property. Keeping it outside the normal target graph provides three useful
# guarantees:
#
#   * ordinary builds do not require clang-format or Git;
#   * every platform commit gate evaluates the same repository policy;
#   * formatting never runs implicitly while a compiler is producing objects.
#
# Two top-level targets are provided:
#
#   format-check
#       Verifies tracked and non-ignored untracked first-party C/C++ files
#       without modifying them.
#
#   format-staged
#       Formats staged first-party C/C++ files and stages the resulting content.
#       Files containing additional unstaged changes are rejected before any
#       source is modified.
#
# The targets are repository-wide and intentionally exist only when
# MC-LAB-CORE is configured as the top-level project. A parent project remains
# responsible for its own formatting policy and target names.

get_filename_component(
    mc_lab_core_compiler_directory
    "${CMAKE_CXX_COMPILER}"
    DIRECTORY
)

find_program(
    MC_LAB_CORE_CLANG_FORMAT
    NAMES
        clang-format
        clang-format-22
    HINTS
        "${mc_lab_core_compiler_directory}"
    DOC
        "clang-format executable used by the MC-LAB-CORE formatting targets"
)

find_program(
    MC_LAB_CORE_GIT
    NAMES
        git
    DOC
        "Git executable used to discover MC-LAB-CORE formatting inputs"
)

set(
    MC_LAB_CORE_CLANG_FORMAT_CONFIG
    "${PROJECT_SOURCE_DIR}/.clang-format"
    CACHE FILEPATH
    "clang-format configuration used by the MC-LAB-CORE formatting targets"
)

set(
    mc_lab_core_default_format_paths
    "include;src;tests;tools"
)

# Migrate the exact previous repository default without overwriting deliberate
# contributor overrides stored in CMakeUserPresets.json or the cache.
if(
    DEFINED MC_LAB_CORE_FORMAT_PATHS
    AND MC_LAB_CORE_FORMAT_PATHS STREQUAL "include;src;tests"
)
    set(
        MC_LAB_CORE_FORMAT_PATHS
        "${mc_lab_core_default_format_paths}"
        CACHE STRING
        "Repository-relative first-party paths covered by clang-format"
        FORCE
    )
else()
    set(
        MC_LAB_CORE_FORMAT_PATHS
        "${mc_lab_core_default_format_paths}"
        CACHE STRING
        "Repository-relative first-party paths covered by clang-format"
    )
endif()

mark_as_advanced(
    MC_LAB_CORE_CLANG_FORMAT
    MC_LAB_CORE_CLANG_FORMAT_CONFIG
    MC_LAB_CORE_FORMAT_PATHS
    MC_LAB_CORE_GIT
)

if(NOT PROJECT_IS_TOP_LEVEL)
    return()
endif()

if(TARGET format-check OR TARGET format-staged)
    message(
        FATAL_ERROR
        "formatting.cmake cannot create its repository targets because "
        "'format-check' or 'format-staged' already exists."
    )
endif()

if(NOT MC_LAB_CORE_FORMAT_PATHS)
    message(
        FATAL_ERROR
        "MC_LAB_CORE_FORMAT_PATHS must name at least one repository-relative "
        "first-party path."
    )
endif()

# Validate and normalize formatting roots at configure time. The runtime runner
# still discovers files through Git so additions, removals, and staging changes
# are observed without reconfiguring CMake.
set(mc_lab_core_format_paths)

foreach(mc_lab_core_format_path IN LISTS MC_LAB_CORE_FORMAT_PATHS)
    if(mc_lab_core_format_path STREQUAL "")
        message(
            FATAL_ERROR
            "MC_LAB_CORE_FORMAT_PATHS contains an empty path."
        )
    endif()

    if(IS_ABSOLUTE "${mc_lab_core_format_path}")
        message(
            FATAL_ERROR
            "MC_LAB_CORE_FORMAT_PATHS must contain repository-relative paths; "
            "'${mc_lab_core_format_path}' is absolute."
        )
    endif()

    cmake_path(
        NORMAL_PATH
        mc_lab_core_format_path
        OUTPUT_VARIABLE mc_lab_core_format_path_normalized
    )

    if(
        mc_lab_core_format_path_normalized STREQUAL "."
        OR mc_lab_core_format_path_normalized STREQUAL ".."
        OR mc_lab_core_format_path_normalized MATCHES "^\\.\\.[/\\\\]"
    )
        message(
            FATAL_ERROR
            "MC_LAB_CORE_FORMAT_PATHS entry "
            "'${mc_lab_core_format_path}' does not identify a first-party "
            "subdirectory inside PROJECT_SOURCE_DIR."
        )
    endif()

    list(
        APPEND
        mc_lab_core_format_paths
        "${mc_lab_core_format_path_normalized}"
    )
endforeach()

list(
    REMOVE_DUPLICATES
    mc_lab_core_format_paths
)

list(
    SORT
    mc_lab_core_format_paths
)

# A generated manifest avoids platform-specific command-line list escaping.
set(
    mc_lab_core_formatting_directory
    "${CMAKE_BINARY_DIR}/cmake/formatting"
)

file(
    MAKE_DIRECTORY
    "${mc_lab_core_formatting_directory}"
)

set(
    mc_lab_core_format_paths_manifest
    "${mc_lab_core_formatting_directory}/paths.cmake"
)

file(
    WRITE
    "${mc_lab_core_format_paths_manifest}"
    "set(MC_LAB_CORE_FORMAT_PATHS\n"
)

foreach(mc_lab_core_format_path IN LISTS mc_lab_core_format_paths)
    string(
        REPLACE
        "\""
        "\\\""
        mc_lab_core_format_path_escaped
        "${mc_lab_core_format_path}"
    )

    file(
        APPEND
        "${mc_lab_core_format_paths_manifest}"
        "    \"${mc_lab_core_format_path_escaped}\"\n"
    )
endforeach()

file(
    APPEND
    "${mc_lab_core_format_paths_manifest}"
    ")\n"
)

set(
    mc_lab_core_formatting_runner
    "${CMAKE_CURRENT_LIST_DIR}/../scripts/run_formatting.cmake"
)

add_custom_target(
    format-check
    COMMAND
        "${CMAKE_COMMAND}"
        "-DMC_LAB_CORE_FORMAT_MODE=CHECK"
        "-DMC_LAB_CORE_CLANG_FORMAT=${MC_LAB_CORE_CLANG_FORMAT}"
        "-DMC_LAB_CORE_CLANG_FORMAT_CONFIG=${MC_LAB_CORE_CLANG_FORMAT_CONFIG}"
        "-DMC_LAB_CORE_GIT=${MC_LAB_CORE_GIT}"
        "-DMC_LAB_CORE_SOURCE_DIRECTORY=${PROJECT_SOURCE_DIR}"
        "-DMC_LAB_CORE_FORMAT_PATHS_MANIFEST=${mc_lab_core_format_paths_manifest}"
        -P
        "${mc_lab_core_formatting_runner}"
    COMMENT
        "Checking first-party C/C++ formatting"
    USES_TERMINAL
    VERBATIM
)

add_custom_target(
    format-staged
    COMMAND
        "${CMAKE_COMMAND}"
        "-DMC_LAB_CORE_FORMAT_MODE=STAGED"
        "-DMC_LAB_CORE_CLANG_FORMAT=${MC_LAB_CORE_CLANG_FORMAT}"
        "-DMC_LAB_CORE_CLANG_FORMAT_CONFIG=${MC_LAB_CORE_CLANG_FORMAT_CONFIG}"
        "-DMC_LAB_CORE_GIT=${MC_LAB_CORE_GIT}"
        "-DMC_LAB_CORE_SOURCE_DIRECTORY=${PROJECT_SOURCE_DIR}"
        "-DMC_LAB_CORE_FORMAT_PATHS_MANIFEST=${mc_lab_core_format_paths_manifest}"
        -P
        "${mc_lab_core_formatting_runner}"
    COMMENT
        "Formatting and re-staging staged first-party C/C++ files"
    USES_TERMINAL
    VERBATIM
)

set_target_properties(
    format-check
    format-staged
    PROPERTIES
        FOLDER "quality"
)
