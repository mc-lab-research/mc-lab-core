include_guard(GLOBAL)

# ==============================================================================
# First-Party Static Analysis Registry
# ==============================================================================

# Static analysis is an explicit build target, not a compiler launcher attached
# to every normal compilation. This separation provides three guarantees:
#
#   * ordinary platform builds do not require clang-tidy;
#   * analysis runs against the exact compile database of a concrete platform;
#   * the quality workflow can fail independently with complete diagnostics.
#
# Components register their owning targets as they are declared. Finalization
# happens once, after the complete product and test target graph is known.
#
# Public API:
#
#   mc_lab_core_register_static_analysis_target(<target>)
#       Registers one first-party target for source discovery.
#
#   mc_lab_core_finalize_static_analysis()
#       Creates the repository-level `static-analysis` build target.

get_filename_component(
    mc_lab_core_compiler_directory
    "${CMAKE_CXX_COMPILER}"
    DIRECTORY
)

find_program(
    MC_LAB_CORE_CLANG_TIDY
    NAMES
        clang-tidy
    HINTS
        "${mc_lab_core_compiler_directory}"
    DOC
        "clang-tidy executable used by the MC-LAB-CORE static-analysis target"
)

set(
    MC_LAB_CORE_CLANG_TIDY_CONFIG
    "${PROJECT_SOURCE_DIR}/.clang-tidy"
    CACHE FILEPATH
    "clang-tidy configuration used by the MC-LAB-CORE static-analysis target"
)

mark_as_advanced(
    MC_LAB_CORE_CLANG_TIDY
    MC_LAB_CORE_CLANG_TIDY_CONFIG
)

# Register one first-party target for static analysis.
#
# Registration is metadata-only: it never invokes clang-tidy and never changes
# normal compilation. Aliases and imported targets are rejected so source
# ownership remains explicit. Repeated registration is idempotent.
function(mc_lab_core_register_static_analysis_target target_name)
    if(NOT TARGET "${target_name}")
        message(
            FATAL_ERROR
            "mc_lab_core_register_static_analysis_target(): "
            "'${target_name}' is not a target."
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
            "mc_lab_core_register_static_analysis_target(): "
            "'${target_name}' is an alias. Register "
            "'${mc_lab_core_target_alias}' instead."
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
            "mc_lab_core_register_static_analysis_target(): "
            "'${target_name}' is imported. Static analysis is restricted "
            "to first-party targets."
        )
    endif()

    get_target_property(
        mc_lab_core_static_analysis_registered
        "${target_name}"
        MC_LAB_CORE_STATIC_ANALYSIS_REGISTERED
    )

    if(mc_lab_core_static_analysis_registered)
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
            "mc_lab_core_register_static_analysis_target(): target "
            "'${target_name}' has unsupported type "
            "'${mc_lab_core_target_type}'."
        )
    endif()

    set_property(
        GLOBAL
        APPEND
        PROPERTY MC_LAB_CORE_STATIC_ANALYSIS_TARGETS "${target_name}"
    )

    set_property(
        TARGET "${target_name}"
        PROPERTY MC_LAB_CORE_STATIC_ANALYSIS_REGISTERED TRUE
    )
endfunction()

# Create the repository-level static-analysis target after all components have
# registered. The generated manifest is deterministic, contains only existing
# first-party C++ translation units, and excludes generated sources.
function(mc_lab_core_finalize_static_analysis)
    get_property(
        mc_lab_core_static_analysis_finalized
        GLOBAL
        PROPERTY MC_LAB_CORE_STATIC_ANALYSIS_FINALIZED
    )

    if(mc_lab_core_static_analysis_finalized)
        return()
    endif()

    if(TARGET static-analysis)
        message(
            FATAL_ERROR
            "mc_lab_core_finalize_static_analysis(): target "
            "'static-analysis' already exists."
        )
    endif()

    get_property(
        mc_lab_core_registered_targets
        GLOBAL
        PROPERTY MC_LAB_CORE_STATIC_ANALYSIS_TARGETS
    )

    set(mc_lab_core_static_analysis_sources)
    set(mc_lab_core_static_analysis_build_targets)

    foreach(
        mc_lab_core_registered_target
        IN LISTS mc_lab_core_registered_targets
    )
        if(NOT TARGET "${mc_lab_core_registered_target}")
            message(
                FATAL_ERROR
                "Static-analysis target '${mc_lab_core_registered_target}' "
                "disappeared before finalization."
            )
        endif()

        get_target_property(
            mc_lab_core_registered_target_type
            "${mc_lab_core_registered_target}"
            TYPE
        )

        # clang-cl compile database entries can reference response files such
        # as CMake's C++ module-scanning *.modmap artifacts. Those files are
        # materialized by the owning target's normal build, not by configure.
        # Building registered compiled targets first also guarantees that
        # generated headers and other target-level prerequisites are ready.
        if(
            NOT mc_lab_core_registered_target_type STREQUAL
                "INTERFACE_LIBRARY"
        )
            list(
                APPEND
                mc_lab_core_static_analysis_build_targets
                "${mc_lab_core_registered_target}"
            )
        endif()

        get_target_property(
            mc_lab_core_target_source_directory
            "${mc_lab_core_registered_target}"
            SOURCE_DIR
        )

        get_target_property(
            mc_lab_core_target_sources
            "${mc_lab_core_registered_target}"
            SOURCES
        )

        if(NOT mc_lab_core_target_sources)
            continue()
        endif()

        foreach(
            mc_lab_core_target_source
            IN LISTS mc_lab_core_target_sources
        )
            # Generator expressions and target-object references cannot be
            # resolved reliably during configure-time source discovery.
            if(mc_lab_core_target_source MATCHES "^\\$<")
                continue()
            endif()

            get_source_file_property(
                mc_lab_core_source_is_generated
                "${mc_lab_core_target_source}"
                TARGET_DIRECTORY "${mc_lab_core_registered_target}"
                GENERATED
            )

            if(mc_lab_core_source_is_generated)
                continue()
            endif()

            cmake_path(
                ABSOLUTE_PATH
                mc_lab_core_target_source
                BASE_DIRECTORY "${mc_lab_core_target_source_directory}"
                NORMALIZE
                OUTPUT_VARIABLE mc_lab_core_source_absolute
            )

            get_filename_component(
                mc_lab_core_source_extension
                "${mc_lab_core_source_absolute}"
                EXT
            )

            string(
                TOLOWER
                "${mc_lab_core_source_extension}"
                mc_lab_core_source_extension
            )

            if(NOT mc_lab_core_source_extension MATCHES
                "^\\.(cc|cpp|cxx|c\\+\\+)$"
            )
                continue()
            endif()

            file(
                RELATIVE_PATH
                mc_lab_core_source_relative
                "${PROJECT_SOURCE_DIR}"
                "${mc_lab_core_source_absolute}"
            )

            if(
                mc_lab_core_source_relative STREQUAL ".."
                OR mc_lab_core_source_relative MATCHES "^\\.\\.[/\\\\]"
            )
                continue()
            endif()

            if(NOT EXISTS "${mc_lab_core_source_absolute}")
                message(
                    FATAL_ERROR
                    "Registered static-analysis source does not exist: "
                    "${mc_lab_core_source_absolute}"
                )
            endif()

            list(
                APPEND
                mc_lab_core_static_analysis_sources
                "${mc_lab_core_source_absolute}"
            )
        endforeach()
    endforeach()

    list(
        REMOVE_DUPLICATES
        mc_lab_core_static_analysis_sources
    )

    list(
        SORT
        mc_lab_core_static_analysis_sources
    )

    list(
        REMOVE_DUPLICATES
        mc_lab_core_static_analysis_build_targets
    )

    list(
        SORT
        mc_lab_core_static_analysis_build_targets
    )

    set(
        mc_lab_core_static_analysis_directory
        "${CMAKE_BINARY_DIR}/cmake/static-analysis"
    )

    file(
        MAKE_DIRECTORY
        "${mc_lab_core_static_analysis_directory}"
    )

    set(
        mc_lab_core_static_analysis_manifest
        "${mc_lab_core_static_analysis_directory}/sources.cmake"
    )

    file(
        WRITE
        "${mc_lab_core_static_analysis_manifest}"
        "set(MC_LAB_CORE_STATIC_ANALYSIS_SOURCES\n"
    )

    foreach(
        mc_lab_core_static_analysis_source
        IN LISTS mc_lab_core_static_analysis_sources
    )
        string(
            REPLACE
            "\""
            "\\\""
            mc_lab_core_static_analysis_source_escaped
            "${mc_lab_core_static_analysis_source}"
        )

        file(
            APPEND
            "${mc_lab_core_static_analysis_manifest}"
            "    \"${mc_lab_core_static_analysis_source_escaped}\"\n"
        )
    endforeach()

    file(
        APPEND
        "${mc_lab_core_static_analysis_manifest}"
        ")\n"
    )

    add_custom_target(
        static-analysis
        COMMAND
            "${CMAKE_COMMAND}"
            "-DMC_LAB_CORE_CLANG_TIDY=${MC_LAB_CORE_CLANG_TIDY}"
            "-DMC_LAB_CORE_CLANG_TIDY_CONFIG=${MC_LAB_CORE_CLANG_TIDY_CONFIG}"
            "-DMC_LAB_CORE_COMPILE_DATABASE=${CMAKE_BINARY_DIR}"
            "-DMC_LAB_CORE_SOURCE_MANIFEST=${mc_lab_core_static_analysis_manifest}"
            -P
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../scripts/run_static_analysis.cmake"
        DEPENDS
            ${mc_lab_core_static_analysis_build_targets}
        COMMENT
            "Building registered targets and running clang-tidy"
        USES_TERMINAL
        VERBATIM
    )

    set_target_properties(
        static-analysis
        PROPERTIES
            FOLDER "quality"
    )

    set_property(
        GLOBAL
        PROPERTY MC_LAB_CORE_STATIC_ANALYSIS_FINALIZED TRUE
    )
endfunction()
