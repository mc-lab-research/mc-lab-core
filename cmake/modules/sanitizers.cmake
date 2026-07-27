include_guard(GLOBAL)

# ==============================================================================
# First-Party Runtime Sanitizer Policy
# ==============================================================================

# Sanitizers instrument object code and may add runtime libraries to final
# binaries. Keep that behavior target-scoped:
#
#   * first-party targets opt in explicitly;
#   * ordinary Debug and Release builds remain uninstrumented;
#   * third-party code is not rebuilt with repository flags accidentally; and
#   * final executables make their sanitizer runtime dependency visible.
#
# Public API:
#
#   MC_LAB_CORE::Sanitizers
#       Read-only alias for advanced target composition.
#
#   mc_lab_core_enable_sanitizers(<target>)
#       Preferred API for instrumenting a first-party target.
#
# MC_LAB_CORE_SANITIZER is configured by the platform-derived quality presets.
# The policy target always exists so component CMakeLists files do not need
# option-dependent branches.

set(
    mc_lab_core_supported_sanitizers
    OFF
    ADDRESS
)

if(NOT MC_LAB_CORE_SANITIZER IN_LIST mc_lab_core_supported_sanitizers)
    list(
        JOIN
        mc_lab_core_supported_sanitizers
        ", "
        mc_lab_core_supported_sanitizers_text
    )

    message(
        FATAL_ERROR
        "Unsupported MC_LAB_CORE_SANITIZER='${MC_LAB_CORE_SANITIZER}'. "
        "Supported values: ${mc_lab_core_supported_sanitizers_text}."
    )
endif()

add_library(
    mc_lab_core_sanitizers
    INTERFACE
)

add_library(
    MC_LAB_CORE::Sanitizers
    ALIAS
    mc_lab_core_sanitizers
)

if(MC_LAB_CORE_SANITIZER STREQUAL "ADDRESS")
    if(MSVC)
        # MSVC and clang-cl Debug configurations normally enable /RTC. Microsoft
        # documents every /RTC variant as incompatible with AddressSanitizer.
        # The repository ASan presets therefore expose RelWithDebInfo only.
        if(CMAKE_CONFIGURATION_TYPES)
            if("Debug" IN_LIST CMAKE_CONFIGURATION_TYPES)
                message(
                    FATAL_ERROR
                    "AddressSanitizer cannot instrument an MSVC-frontend "
                    "multi-config build containing Debug because /RTC is "
                    "incompatible with /fsanitize=address. Use the "
                    "windows-*-asan preset, which exposes RelWithDebInfo only."
                )
            endif()
        elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
            message(
                FATAL_ERROR
                "AddressSanitizer cannot instrument an MSVC-frontend Debug "
                "build because /RTC is incompatible with "
                "/fsanitize=address. Use the matching windows-*-asan preset."
            )
        endif()

        if(
            CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"
            AND MSVC_VERSION LESS 1929
        )
            message(
                FATAL_ERROR
                "MC-LAB-CORE requires MSVC 19.29 or newer for "
                "AddressSanitizer."
            )
        endif()

        target_compile_options(
            mc_lab_core_sanitizers
            INTERFACE
                $<$<COMPILE_LANGUAGE:CXX>:/fsanitize=address>
        )

        # Native MSVC records the required ASan libraries in instrumented
        # objects. clang-cl does not provide the same automatic runtime link
        # when CMake invokes link.exe or lld-link directly, as Ninja does.
        # Resolve compiler-rt from the active compiler installation rather than
        # assuming a versioned LLVM directory.
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            execute_process(
                COMMAND
                    "${CMAKE_CXX_COMPILER}"
                    -print-resource-dir
                RESULT_VARIABLE
                    mc_lab_core_clang_resource_result
                OUTPUT_VARIABLE
                    mc_lab_core_clang_resource_directory
                ERROR_VARIABLE
                    mc_lab_core_clang_resource_error
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )

            if(NOT mc_lab_core_clang_resource_result EQUAL 0)
                message(
                    FATAL_ERROR
                    "Unable to locate the clang-cl resource directory: "
                    "${mc_lab_core_clang_resource_error}"
                )
            endif()

            file(
                TO_CMAKE_PATH
                "${mc_lab_core_clang_resource_directory}"
                mc_lab_core_clang_resource_directory
            )

            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(mc_lab_core_asan_architecture "x86_64")
            else()
                set(mc_lab_core_asan_architecture "i386")
            endif()

            set(
                mc_lab_core_asan_runtime_directory
                "${mc_lab_core_clang_resource_directory}/lib/windows"
            )

            set(
                mc_lab_core_asan_runtime_import_library
                "${mc_lab_core_asan_runtime_directory}/clang_rt.asan_dynamic-${mc_lab_core_asan_architecture}.lib"
            )

            set(
                mc_lab_core_asan_runtime_dll
                "${mc_lab_core_asan_runtime_directory}/clang_rt.asan_dynamic-${mc_lab_core_asan_architecture}.dll"
            )

            if(
                CMAKE_MSVC_RUNTIME_LIBRARY
                AND NOT CMAKE_MSVC_RUNTIME_LIBRARY MATCHES "DLL"
            )
                set(mc_lab_core_asan_uses_dynamic_crt FALSE)

                set(
                    mc_lab_core_asan_runtime_thunk
                    "${mc_lab_core_asan_runtime_directory}/clang_rt.asan_static_runtime_thunk-${mc_lab_core_asan_architecture}.lib"
                )
            else()
                set(mc_lab_core_asan_uses_dynamic_crt TRUE)

                set(
                    mc_lab_core_asan_runtime_thunk
                    "${mc_lab_core_asan_runtime_directory}/clang_rt.asan_dynamic_runtime_thunk-${mc_lab_core_asan_architecture}.lib"
                )
            endif()

            foreach(
                mc_lab_core_asan_runtime_file
                IN ITEMS
                    "${mc_lab_core_asan_runtime_import_library}"
                    "${mc_lab_core_asan_runtime_thunk}"
                    "${mc_lab_core_asan_runtime_dll}"
            )
                if(NOT EXISTS "${mc_lab_core_asan_runtime_file}")
                    message(
                        FATAL_ERROR
                        "The clang-cl AddressSanitizer runtime is incomplete. "
                        "Missing: ${mc_lab_core_asan_runtime_file}"
                    )
                endif()
            endforeach()

            target_link_libraries(
                mc_lab_core_sanitizers
                INTERFACE
                    "${mc_lab_core_asan_runtime_import_library}"
            )

            target_link_options(
                mc_lab_core_sanitizers
                INTERFACE
                    "/WHOLEARCHIVE:${mc_lab_core_asan_runtime_thunk}"
            )

            if(mc_lab_core_asan_uses_dynamic_crt)
                target_link_options(
                    mc_lab_core_sanitizers
                    INTERFACE
                        /INCLUDE:__asan_seh_interceptor
                )
            endif()

            set_property(
                GLOBAL
                PROPERTY
                    MC_LAB_CORE_ASAN_RUNTIME_DLL
                    "${mc_lab_core_asan_runtime_dll}"
            )
        endif()

        # /DEBUG commonly implies incremental linking. ASan does not support
        # incremental links, so override the linker policy explicitly.
        target_link_options(
            mc_lab_core_sanitizers
            INTERFACE
                /INCREMENTAL:NO
        )
    elseif(
        WIN32
        AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
    )
        # Runtime packaging varies between MinGW-w64 GCC distributions. The
        # supported Windows matrix uses MSVC, clang-cl, or MinGW-w64 Clang.
        message(
            FATAL_ERROR
            "AddressSanitizer with MinGW-w64 GCC is outside the supported "
            "MC-LAB-CORE matrix. Use windows-clang64-asan instead."
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(
            mc_lab_core_sanitizers
            INTERFACE
                $<$<COMPILE_LANGUAGE:CXX>:-fsanitize=address>
                $<$<COMPILE_LANGUAGE:CXX>:-fno-omit-frame-pointer>
                $<$<COMPILE_LANGUAGE:CXX>:-fno-optimize-sibling-calls>
        )

        # The compiler driver must perform the final link so it can select the
        # matching AddressSanitizer runtime.
        target_link_options(
            mc_lab_core_sanitizers
            INTERFACE
                -fsanitize=address
        )
    else()
        message(
            FATAL_ERROR
            "AddressSanitizer is not implemented for "
            "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}."
        )
    endif()
endif()

# Instrument one first-party target with the selected sanitizer policy.
#
# Usage:
#   mc_lab_core_enable_sanitizers(my_first_party_target)
#
# Every compiled first-party library should call this helper so its own object
# code is instrumented. Every final executable and shared/module library should
# also call it so the sanitizer runtime and linker policy are present.
#
# The function rejects aliases and imported targets, supports header-only
# first-party INTERFACE libraries, and is idempotent.
function(mc_lab_core_enable_sanitizers target_name)
    if(NOT TARGET "${target_name}")
        message(
            FATAL_ERROR
            "mc_lab_core_enable_sanitizers(): '${target_name}' is not a target."
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
            "mc_lab_core_enable_sanitizers(): '${target_name}' is an alias. "
            "Apply sanitizers to '${mc_lab_core_target_alias}' instead."
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
            "mc_lab_core_enable_sanitizers(): '${target_name}' is imported. "
            "Sanitizer policy is restricted to first-party targets."
        )
    endif()

    get_target_property(
        mc_lab_core_sanitizers_enabled
        "${target_name}"
        MC_LAB_CORE_SANITIZERS_ENABLED
    )

    if(mc_lab_core_sanitizers_enabled)
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
                MC_LAB_CORE::Sanitizers
        )
    elseif(
        mc_lab_core_target_type MATCHES
        "^(EXECUTABLE|STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY|OBJECT_LIBRARY)$"
    )
        target_link_libraries(
            "${target_name}"
            PRIVATE
                MC_LAB_CORE::Sanitizers
        )
    else()
        message(
            FATAL_ERROR
            "mc_lab_core_enable_sanitizers(): target '${target_name}' has "
            "unsupported type '${mc_lab_core_target_type}'."
        )
    endif()

    set_property(
        TARGET "${target_name}"
        PROPERTY MC_LAB_CORE_SANITIZERS_ENABLED TRUE
    )

    # clang-cl's ASan runtime is a DLL. Keep execution hermetic by placing the
    # runtime next to each final first-party binary instead of depending on a
    # machine-specific PATH entry.
    get_property(
        mc_lab_core_asan_runtime_dll
        GLOBAL
        PROPERTY MC_LAB_CORE_ASAN_RUNTIME_DLL
    )

    if(
        mc_lab_core_asan_runtime_dll
        AND mc_lab_core_target_type MATCHES
            "^(EXECUTABLE|SHARED_LIBRARY|MODULE_LIBRARY)$"
    )
        add_custom_command(
            TARGET "${target_name}"
            POST_BUILD
            COMMAND
                "${CMAKE_COMMAND}"
                -E copy_if_different
                "${mc_lab_core_asan_runtime_dll}"
                "$<TARGET_FILE_DIR:${target_name}>"
            COMMENT
                "Deploying the clang-cl AddressSanitizer runtime for ${target_name}"
            VERBATIM
        )
    endif()
endfunction()
