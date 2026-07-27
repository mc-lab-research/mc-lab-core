include_guard(GLOBAL)

# ==============================================================================
# First-Party Compiler Warning Policy
# ==============================================================================

# This module owns the warning policy for code maintained in this repository.
# It deliberately uses an opt-in INTERFACE target instead of global
# add_compile_options():
#
#   * first-party targets receive one consistent warning policy;
#   * imported and vendored dependencies retain their upstream policy;
#   * warnings-as-errors never turn dependency diagnostics into repository
#     failures; and
#   * every component CMakeLists.txt makes policy ownership visible.
#
# Public API:
#
#   MC_LAB_CORE::Warnings
#       Read-only alias for advanced target composition.
#
#   mc_lab_core_enable_warnings(<target>)
#       Preferred API for applying the policy to a first-party target.
#
# MC_LAB_CORE_COMPILER_WARNINGS_AS_ERRORS enables strict diagnostics.
# MC_LAB_CORE_WARNINGS_AS_ERRORS_CONFIGURATIONS may restrict that policy to a
# configuration list for multi-configuration generators. An empty list means
# every configuration. The warning set itself remains identical in all cases.
add_library(
    mc_lab_core_warnings
    INTERFACE
)

add_library(
    MC_LAB_CORE::Warnings
    ALIAS
    mc_lab_core_warnings
)

# Add the compiler-specific warnings-as-errors switch. Keeping the
# configuration filter inside a generator expression is essential for
# multi-configuration generators: one Visual Studio project can remain
# developer-friendly in Debug while enforcing the commit gate in Release.
function(
    mc_lab_core_add_warnings_as_errors
    option_name
)
    if(NOT MC_LAB_CORE_COMPILER_WARNINGS_AS_ERRORS)
        return()
    endif()

    if(MC_LAB_CORE_WARNINGS_AS_ERRORS_CONFIGURATIONS)
        foreach(
            mc_lab_core_warnings_configuration
            IN LISTS MC_LAB_CORE_WARNINGS_AS_ERRORS_CONFIGURATIONS
        )
            target_compile_options(
                mc_lab_core_warnings
                INTERFACE
                    "$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:${mc_lab_core_warnings_configuration}>>:${option_name}>"
            )
        endforeach()
    else()
        target_compile_options(
            mc_lab_core_warnings
            INTERFACE
                "$<$<COMPILE_LANGUAGE:CXX>:${option_name}>"
        )
    endif()
endfunction()

# MSVC is true for both the native Microsoft compiler and compilers using the
# Microsoft command-line frontend, notably clang-cl. Keep their baseline
# frontend flags together, then specialize diagnostics by compiler identity.
if(MSVC)
    target_compile_options(
        mc_lab_core_warnings
        INTERFACE
            $<$<COMPILE_LANGUAGE:CXX>:/W4>
            $<$<COMPILE_LANGUAGE:CXX>:/permissive->
    )

    # Native MSVC understands this complete numbered-warning policy. These
    # switches raise useful diagnostics normally hidden below /W4 without
    # enabling /Wall, whose implementation and system-header diagnostics are
    # unsuitable for a portable warnings-as-errors gate.
    #
    # clang-cl intentionally does not receive the numbered policy because it
    # does not guarantee compatibility with every MSVC warning number.
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(
            mc_lab_core_warnings
            INTERFACE
                $<$<COMPILE_LANGUAGE:CXX>:/w14242>
                $<$<COMPILE_LANGUAGE:CXX>:/w14254>
                $<$<COMPILE_LANGUAGE:CXX>:/w14263>
                $<$<COMPILE_LANGUAGE:CXX>:/w14265>
                $<$<COMPILE_LANGUAGE:CXX>:/w14287>
                $<$<COMPILE_LANGUAGE:CXX>:/w14289>
                $<$<COMPILE_LANGUAGE:CXX>:/w14296>
                $<$<COMPILE_LANGUAGE:CXX>:/w14311>
                $<$<COMPILE_LANGUAGE:CXX>:/w14545>
                $<$<COMPILE_LANGUAGE:CXX>:/w14546>
                $<$<COMPILE_LANGUAGE:CXX>:/w14547>
                $<$<COMPILE_LANGUAGE:CXX>:/w14549>
                $<$<COMPILE_LANGUAGE:CXX>:/w14555>
                $<$<COMPILE_LANGUAGE:CXX>:/w14619>
                $<$<COMPILE_LANGUAGE:CXX>:/w14640>
                $<$<COMPILE_LANGUAGE:CXX>:/w14826>
                $<$<COMPILE_LANGUAGE:CXX>:/w14905>
                $<$<COMPILE_LANGUAGE:CXX>:/w14906>
                $<$<COMPILE_LANGUAGE:CXX>:/w14928>
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # clang-cl accepts Clang diagnostic switches alongside the MSVC
        # frontend syntax. This keeps its policy close to Clang on Unix while
        # preserving Microsoft ABI and SDK compatibility.
        target_compile_options(
            mc_lab_core_warnings
            INTERFACE
                $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
                $<$<COMPILE_LANGUAGE:CXX>:-Wconversion>
                $<$<COMPILE_LANGUAGE:CXX>:-Wsign-conversion>
                $<$<COMPILE_LANGUAGE:CXX>:-Wshadow>
                $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
                $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
                $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
                $<$<COMPILE_LANGUAGE:CXX>:-Wformat=2>
                $<$<COMPILE_LANGUAGE:CXX>:-Wimplicit-fallthrough>
        )
    endif()

    # /WX is intentionally independent from the selected warning set.
    mc_lab_core_add_warnings_as_errors("/WX")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    # Use the common, stable intersection supported by the repository's GCC,
    # Clang, AppleClang, and MinGW toolchains. Compiler-specific experimental
    # warnings belong in a separate opt-in policy until the full matrix has
    # demonstrated that they are actionable and sufficiently stable.
    target_compile_options(
        mc_lab_core_warnings
        INTERFACE
            $<$<COMPILE_LANGUAGE:CXX>:-Wall>
            $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
            $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
            $<$<COMPILE_LANGUAGE:CXX>:-Wconversion>
            $<$<COMPILE_LANGUAGE:CXX>:-Wsign-conversion>
            $<$<COMPILE_LANGUAGE:CXX>:-Wshadow>
            $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
            $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
            $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
            $<$<COMPILE_LANGUAGE:CXX>:-Wdouble-promotion>
            $<$<COMPILE_LANGUAGE:CXX>:-Wformat=2>
            $<$<COMPILE_LANGUAGE:CXX>:-Wimplicit-fallthrough>
    )

    mc_lab_core_add_warnings_as_errors("-Werror")
else()
    # Unknown compilers remain usable for experimentation, but the warning
    # policy is explicitly reported as incomplete. The supported platform
    # matrix never takes this branch.
    message(
        WARNING
        "MC-LAB-CORE has no explicit warning policy for "
        "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}."
    )
endif()

# Apply the repository warning policy to one first-party target.
#
# Usage:
#   mc_lab_core_enable_warnings(my_first_party_target)
#
# The function is intentionally strict:
#
#   * aliases must be resolved to their owning target;
#   * imported targets are dependency boundaries and are rejected;
#   * INTERFACE libraries propagate the policy with INTERFACE scope;
#   * compilable targets consume it with PRIVATE scope; and
#   * repeated calls are idempotent.
function(mc_lab_core_enable_warnings target_name)
    if(NOT TARGET "${target_name}")
        message(
            FATAL_ERROR
            "mc_lab_core_enable_warnings(): '${target_name}' is not a target."
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
            "mc_lab_core_enable_warnings(): '${target_name}' is an alias. "
            "Apply warnings to '${mc_lab_core_target_alias}' instead."
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
            "mc_lab_core_enable_warnings(): '${target_name}' is imported. "
            "Warning policy is restricted to first-party targets."
        )
    endif()

    get_target_property(
        mc_lab_core_warnings_enabled
        "${target_name}"
        MC_LAB_CORE_WARNINGS_ENABLED
    )

    if(mc_lab_core_warnings_enabled)
        return()
    endif()

    get_target_property(
        mc_lab_core_target_type
        "${target_name}"
        TYPE
    )

    if(mc_lab_core_target_type STREQUAL "INTERFACE_LIBRARY")
        target_link_libraries(
            "${target_name}"
            INTERFACE
                MC_LAB_CORE::Warnings
        )
    elseif(
        mc_lab_core_target_type MATCHES
        "^(EXECUTABLE|STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY|OBJECT_LIBRARY)$"
    )
        target_link_libraries(
            "${target_name}"
            PRIVATE
                MC_LAB_CORE::Warnings
        )
    else()
        message(
            FATAL_ERROR
            "mc_lab_core_enable_warnings(): target '${target_name}' has "
            "unsupported type '${mc_lab_core_target_type}'."
        )
    endif()

    set_property(
        TARGET "${target_name}"
        PROPERTY MC_LAB_CORE_WARNINGS_ENABLED TRUE
    )
endfunction()
