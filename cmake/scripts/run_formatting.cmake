cmake_minimum_required(VERSION 3.25)

# ==============================================================================
# clang-format Target Runner
# ==============================================================================

# This script implements the runtime behavior of the `format-check` and
# `format-staged` targets. Repository discovery happens here, at build time, so
# a contributor can add or stage a file without reconfiguring the build tree.
#
# Git is the source of file ownership:
#
#   CHECK  -> tracked and non-ignored untracked files below first-party roots;
#   STAGED -> added, copied, modified, or renamed files in the Git index.
#
# Deleted files and non-C/C++ assets are ignored. The clang-format policy comes
# exclusively from the repository `.clang-format` file.

foreach(
    mc_lab_core_required_variable
    IN ITEMS
        MC_LAB_CORE_FORMAT_MODE
        MC_LAB_CORE_CLANG_FORMAT
        MC_LAB_CORE_CLANG_FORMAT_CONFIG
        MC_LAB_CORE_GIT
        MC_LAB_CORE_SOURCE_DIRECTORY
        MC_LAB_CORE_FORMAT_PATHS_MANIFEST
)
    if(
        NOT DEFINED "${mc_lab_core_required_variable}"
        OR "${${mc_lab_core_required_variable}}" STREQUAL ""
    )
        message(
            FATAL_ERROR
            "run_formatting.cmake requires "
            "${mc_lab_core_required_variable}."
        )
    endif()
endforeach()

if(NOT MC_LAB_CORE_FORMAT_MODE MATCHES "^(CHECK|STAGED)$")
    message(
        FATAL_ERROR
        "MC_LAB_CORE_FORMAT_MODE must be CHECK or STAGED; received "
        "'${MC_LAB_CORE_FORMAT_MODE}'."
    )
endif()

if(
    MC_LAB_CORE_CLANG_FORMAT MATCHES "-NOTFOUND$"
    OR NOT EXISTS "${MC_LAB_CORE_CLANG_FORMAT}"
)
    message(
        FATAL_ERROR
        "clang-format was not found. Install LLVM clang-format or configure "
        "MC_LAB_CORE_CLANG_FORMAT with its full path."
    )
endif()

if(
    MC_LAB_CORE_GIT MATCHES "-NOTFOUND$"
    OR NOT EXISTS "${MC_LAB_CORE_GIT}"
)
    message(
        FATAL_ERROR
        "Git was not found. Install Git or configure MC_LAB_CORE_GIT with "
        "its full path."
    )
endif()

if(NOT EXISTS "${MC_LAB_CORE_CLANG_FORMAT_CONFIG}")
    message(
        FATAL_ERROR
        "clang-format configuration not found: "
        "${MC_LAB_CORE_CLANG_FORMAT_CONFIG}"
    )
endif()

if(NOT IS_DIRECTORY "${MC_LAB_CORE_SOURCE_DIRECTORY}")
    message(
        FATAL_ERROR
        "MC-LAB-CORE source directory not found: "
        "${MC_LAB_CORE_SOURCE_DIRECTORY}"
    )
endif()

if(NOT EXISTS "${MC_LAB_CORE_FORMAT_PATHS_MANIFEST}")
    message(
        FATAL_ERROR
        "Formatting path manifest not found: "
        "${MC_LAB_CORE_FORMAT_PATHS_MANIFEST}"
    )
endif()

include("${MC_LAB_CORE_FORMAT_PATHS_MANIFEST}")

if(NOT MC_LAB_CORE_FORMAT_PATHS)
    message(
        FATAL_ERROR
        "The formatting path manifest does not contain any first-party paths."
    )
endif()

# Confirm that the configured source directory belongs to a Git worktree before
# interpreting any discovery output as repository-relative paths.
execute_process(
    COMMAND
        "${MC_LAB_CORE_GIT}"
        -C
        "${MC_LAB_CORE_SOURCE_DIRECTORY}"
        rev-parse
        --is-inside-work-tree
    RESULT_VARIABLE
        mc_lab_core_git_worktree_result
    OUTPUT_VARIABLE
        mc_lab_core_git_worktree_output
    ERROR_VARIABLE
        mc_lab_core_git_worktree_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ENCODING
        UTF-8
)

if(
    NOT mc_lab_core_git_worktree_result EQUAL 0
    OR NOT mc_lab_core_git_worktree_output STREQUAL "true"
)
    message(
        FATAL_ERROR
        "Formatting targets require a Git worktree rooted at or containing "
        "'${MC_LAB_CORE_SOURCE_DIRECTORY}'.\n"
        "${mc_lab_core_git_worktree_error}"
    )
endif()

# Validate the policy before a mutating STAGED run touches any source file.
# --assume-filename selects the C++ language even when no actual input file is
# provided to --dump-config.
execute_process(
    COMMAND
        "${MC_LAB_CORE_CLANG_FORMAT}"
        "--style=file:${MC_LAB_CORE_CLANG_FORMAT_CONFIG}"
        --fallback-style=none
        --Werror
        "--assume-filename=${MC_LAB_CORE_SOURCE_DIRECTORY}/src/format-probe.cpp"
        --dump-config
    RESULT_VARIABLE
        mc_lab_core_format_config_result
    OUTPUT_QUIET
    ERROR_VARIABLE
        mc_lab_core_format_config_error
    ENCODING
        UTF-8
)

if(NOT mc_lab_core_format_config_result EQUAL 0)
    message(
        FATAL_ERROR
        "Invalid clang-format policy "
        "'${MC_LAB_CORE_CLANG_FORMAT_CONFIG}'.\n"
        "${mc_lab_core_format_config_error}"
    )
endif()

if(MC_LAB_CORE_FORMAT_MODE STREQUAL "CHECK")
    execute_process(
        COMMAND
            "${MC_LAB_CORE_GIT}"
            -C
            "${MC_LAB_CORE_SOURCE_DIRECTORY}"
            -c
            core.quotepath=false
            ls-files
            --cached
            --others
            --exclude-standard
            --
            ${MC_LAB_CORE_FORMAT_PATHS}
        RESULT_VARIABLE
            mc_lab_core_git_files_result
        OUTPUT_VARIABLE
            mc_lab_core_git_files_output
        ERROR_VARIABLE
            mc_lab_core_git_files_error
        ENCODING
            UTF-8
    )
else()
    execute_process(
        COMMAND
            "${MC_LAB_CORE_GIT}"
            -C
            "${MC_LAB_CORE_SOURCE_DIRECTORY}"
            -c
            core.quotepath=false
            diff
            --cached
            --name-only
            --diff-filter=ACMR
            --relative
            --
            ${MC_LAB_CORE_FORMAT_PATHS}
        RESULT_VARIABLE
            mc_lab_core_git_files_result
        OUTPUT_VARIABLE
            mc_lab_core_git_files_output
        ERROR_VARIABLE
            mc_lab_core_git_files_error
        ENCODING
            UTF-8
    )
endif()

if(NOT mc_lab_core_git_files_result EQUAL 0)
    message(
        FATAL_ERROR
        "Git failed while discovering formatting inputs.\n"
        "${mc_lab_core_git_files_error}"
    )
endif()

# Git emits one repository-relative path per line. `core.quotepath=false`
# preserves Unicode file names; the project naming policy intentionally avoids
# control characters and semicolons because they cannot be represented safely
# as CMake list elements.
string(
    REPLACE
    "\r\n"
    "\n"
    mc_lab_core_git_files_output
    "${mc_lab_core_git_files_output}"
)

string(
    REGEX REPLACE
    "\n$"
    ""
    mc_lab_core_git_files_output
    "${mc_lab_core_git_files_output}"
)

if(mc_lab_core_git_files_output STREQUAL "")
    set(mc_lab_core_git_files)
else()
    string(
        REPLACE
        "\n"
        ";"
        mc_lab_core_git_files
        "${mc_lab_core_git_files_output}"
    )
endif()

set(mc_lab_core_format_files)

foreach(mc_lab_core_git_file IN LISTS mc_lab_core_git_files)
    if(
        mc_lab_core_git_file MATCHES "[\r\n;]"
        OR mc_lab_core_git_file MATCHES "^\""
    )
        message(
            FATAL_ERROR
            "Unsupported C/C++ repository path returned by Git: "
            "'${mc_lab_core_git_file}'. File names used by the formatting "
            "workflow must not contain control characters or semicolons."
        )
    endif()

    get_filename_component(
        mc_lab_core_git_file_extension
        "${mc_lab_core_git_file}"
        EXT
    )

    string(
        TOLOWER
        "${mc_lab_core_git_file_extension}"
        mc_lab_core_git_file_extension
    )

    if(NOT mc_lab_core_git_file_extension MATCHES
        "^\\.(c|cc|cpp|cxx|c\\+\\+|h|hh|hpp|hxx|h\\+\\+|inc|inl|ipp|tpp|ixx|cppm)$"
    )
        continue()
    endif()

    cmake_path(
        APPEND
        MC_LAB_CORE_SOURCE_DIRECTORY
        "${mc_lab_core_git_file}"
        OUTPUT_VARIABLE mc_lab_core_format_file_absolute
    )

    cmake_path(
        NORMAL_PATH
        mc_lab_core_format_file_absolute
        OUTPUT_VARIABLE mc_lab_core_format_file_absolute
    )

    file(
        RELATIVE_PATH
        mc_lab_core_format_file_relative
        "${MC_LAB_CORE_SOURCE_DIRECTORY}"
        "${mc_lab_core_format_file_absolute}"
    )

    if(
        mc_lab_core_format_file_relative STREQUAL ".."
        OR mc_lab_core_format_file_relative MATCHES "^\\.\\.[/\\\\]"
    )
        message(
            FATAL_ERROR
            "Git returned a formatting input outside PROJECT_SOURCE_DIR: "
            "${mc_lab_core_format_file_absolute}"
        )
    endif()

    # A path can remain in the index while its working-tree copy is deleted.
    # Deletions contain no content to format and are handled by the build/tests.
    if(NOT EXISTS "${mc_lab_core_format_file_absolute}")
        continue()
    endif()

    list(
        APPEND
        mc_lab_core_format_files
        "${mc_lab_core_format_file_absolute}"
    )
endforeach()

list(
    REMOVE_DUPLICATES
    mc_lab_core_format_files
)

list(
    SORT
    mc_lab_core_format_files
)

if(NOT mc_lab_core_format_files)
    if(MC_LAB_CORE_FORMAT_MODE STREQUAL "STAGED")
        message(
            STATUS
            "No staged first-party C/C++ files require formatting."
        )
        return()
    endif()

    message(
        FATAL_ERROR
        "No first-party C/C++ files were found below: "
        "${MC_LAB_CORE_FORMAT_PATHS}"
    )
endif()

list(
    LENGTH
    mc_lab_core_format_files
    mc_lab_core_format_file_count
)

if(MC_LAB_CORE_FORMAT_MODE STREQUAL "STAGED")
    # Formatting the working-tree copy of a partially staged file would either
    # overwrite unstaged work or accidentally include it in the next commit.
    # Perform this safety check for every candidate before changing any file.
    set(mc_lab_core_partially_staged_files)

    foreach(mc_lab_core_format_file IN LISTS mc_lab_core_format_files)
        file(
            RELATIVE_PATH
            mc_lab_core_format_file_relative
            "${MC_LAB_CORE_SOURCE_DIRECTORY}"
            "${mc_lab_core_format_file}"
        )

        execute_process(
            COMMAND
                "${MC_LAB_CORE_GIT}"
                -C
                "${MC_LAB_CORE_SOURCE_DIRECTORY}"
                diff
                --quiet
                --
                "${mc_lab_core_format_file_relative}"
            RESULT_VARIABLE
                mc_lab_core_git_unstaged_result
            ERROR_VARIABLE
                mc_lab_core_git_unstaged_error
            ENCODING
                UTF-8
        )

        if(mc_lab_core_git_unstaged_result EQUAL 1)
            list(
                APPEND
                mc_lab_core_partially_staged_files
                "${mc_lab_core_format_file_relative}"
            )
        elseif(NOT mc_lab_core_git_unstaged_result EQUAL 0)
            message(
                FATAL_ERROR
                "Git could not inspect unstaged changes for "
                "'${mc_lab_core_format_file_relative}'.\n"
                "${mc_lab_core_git_unstaged_error}"
            )
        endif()
    endforeach()

    if(mc_lab_core_partially_staged_files)
        string(
            JOIN
            "\n  "
            mc_lab_core_partially_staged_file_list
            ${mc_lab_core_partially_staged_files}
        )

        message(
            FATAL_ERROR
            "format-staged did not modify any file because these staged "
            "files also contain unstaged changes:\n"
            "  ${mc_lab_core_partially_staged_file_list}\n"
            "Stage or temporarily shelve those changes, then run "
            "format-staged again."
        )
    endif()

    set(mc_lab_core_format_index 0)

    foreach(mc_lab_core_format_file IN LISTS mc_lab_core_format_files)
        math(
            EXPR
            mc_lab_core_format_index
            "${mc_lab_core_format_index} + 1"
        )

        file(
            RELATIVE_PATH
            mc_lab_core_format_file_relative
            "${MC_LAB_CORE_SOURCE_DIRECTORY}"
            "${mc_lab_core_format_file}"
        )

        message(
            STATUS
            "[${mc_lab_core_format_index}/${mc_lab_core_format_file_count}] "
            "clang-format: ${mc_lab_core_format_file_relative}"
        )

        execute_process(
            COMMAND
                "${MC_LAB_CORE_CLANG_FORMAT}"
                "--style=file:${MC_LAB_CORE_CLANG_FORMAT_CONFIG}"
                --fallback-style=none
                --Werror
                -i
                "${mc_lab_core_format_file}"
            RESULT_VARIABLE
                mc_lab_core_clang_format_result
            ENCODING
                UTF-8
        )

        if(NOT mc_lab_core_clang_format_result EQUAL 0)
            message(
                FATAL_ERROR
                "clang-format failed while formatting "
                "'${mc_lab_core_format_file_relative}'. Earlier files may "
                "already have been formatted and staged."
            )
        endif()

        execute_process(
            COMMAND
                "${MC_LAB_CORE_GIT}"
                -C
                "${MC_LAB_CORE_SOURCE_DIRECTORY}"
                add
                --
                "${mc_lab_core_format_file_relative}"
            RESULT_VARIABLE
                mc_lab_core_git_add_result
            ERROR_VARIABLE
                mc_lab_core_git_add_error
            ENCODING
                UTF-8
        )

        if(NOT mc_lab_core_git_add_result EQUAL 0)
            message(
                FATAL_ERROR
                "clang-format updated "
                "'${mc_lab_core_format_file_relative}', but Git could not "
                "stage the result.\n"
                "${mc_lab_core_git_add_error}"
            )
        endif()
    endforeach()

    message(
        STATUS
        "Formatted and staged "
        "${mc_lab_core_format_file_count} first-party C/C++ file(s)."
    )

    return()
endif()

set(mc_lab_core_format_failures)
set(mc_lab_core_format_index 0)

foreach(mc_lab_core_format_file IN LISTS mc_lab_core_format_files)
    math(
        EXPR
        mc_lab_core_format_index
        "${mc_lab_core_format_index} + 1"
    )

    file(
        RELATIVE_PATH
        mc_lab_core_format_file_relative
        "${MC_LAB_CORE_SOURCE_DIRECTORY}"
        "${mc_lab_core_format_file}"
    )

    message(
        STATUS
        "[${mc_lab_core_format_index}/${mc_lab_core_format_file_count}] "
        "clang-format check: ${mc_lab_core_format_file_relative}"
    )

    execute_process(
        COMMAND
            "${MC_LAB_CORE_CLANG_FORMAT}"
            "--style=file:${MC_LAB_CORE_CLANG_FORMAT_CONFIG}"
            --fallback-style=none
            --Werror
            --dry-run
            "${mc_lab_core_format_file}"
        RESULT_VARIABLE
            mc_lab_core_clang_format_result
        ENCODING
            UTF-8
    )

    if(NOT mc_lab_core_clang_format_result EQUAL 0)
        list(
            APPEND
            mc_lab_core_format_failures
            "${mc_lab_core_format_file_relative}"
        )
    endif()
endforeach()

if(mc_lab_core_format_failures)
    list(
        LENGTH
        mc_lab_core_format_failures
        mc_lab_core_format_failure_count
    )

    string(
        JOIN
        "\n  "
        mc_lab_core_format_failure_list
        ${mc_lab_core_format_failures}
    )

    message(
        FATAL_ERROR
        "Formatting check failed for "
        "${mc_lab_core_format_failure_count} file(s):\n"
        "  ${mc_lab_core_format_failure_list}\n"
        "Run the format-staged target after staging the intended files."
    )
endif()

message(
    STATUS
    "Formatting check passed for "
    "${mc_lab_core_format_file_count} first-party C/C++ file(s)."
)
