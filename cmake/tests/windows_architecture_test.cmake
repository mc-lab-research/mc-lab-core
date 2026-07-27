cmake_minimum_required(VERSION 3.25)

# Minimal regression suite for the supported Windows architecture contract:
# native x86-64 host, x86-64 target, and no x86 32-bit or ARM variants.
#
# Run from any directory with:
#
#   cmake -P <repository>/cmake/tests/windows_architecture_test.cmake
#
# The script reinvokes itself for negative cases so an expected FATAL_ERROR
# remains isolated from the test driver. It requires neither project() nor a
# compiler, making it suitable for a future general build-infrastructure CI
# job.

set(
    mc_lab_core_environment_validation_module
    "${CMAKE_CURRENT_LIST_DIR}/../modules/env_validation.cmake"
)

# Child-process mode: execute one contract check and return its process status
# to the parent driver.
if(DEFINED MC_LAB_CORE_ARCHITECTURE_TEST_MODE)
    set(MC_LAB_CORE_EXPECTED_TOOLCHAIN AUTO)
    set(MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE AUTO)
    set(MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE AUTO)

    include("${mc_lab_core_environment_validation_module}")

    if(MC_LAB_CORE_ARCHITECTURE_TEST_MODE STREQUAL "HOST")
        set(MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE x64)
        set(
            ENV{PROCESSOR_ARCHITEW6432}
            "${MC_LAB_CORE_ARCHITECTURE_TEST_VALUE}"
        )

        _mc_lab_core_require_host_architecture()
    elseif(MC_LAB_CORE_ARCHITECTURE_TEST_MODE STREQUAL "TARGET")
        set(MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE x64)
        set(CMAKE_SIZEOF_VOID_P 8)
        set(
            CMAKE_CXX_COMPILER_ARCHITECTURE_ID
            "${MC_LAB_CORE_ARCHITECTURE_TEST_VALUE}"
        )

        if(DEFINED MC_LAB_CORE_ARCHITECTURE_TEST_GENERATOR_PLATFORM)
            set(CMAKE_GENERATOR "Visual Studio 18 2026")
            set(
                CMAKE_GENERATOR_PLATFORM
                "${MC_LAB_CORE_ARCHITECTURE_TEST_GENERATOR_PLATFORM}"
            )
        else()
            set(CMAKE_GENERATOR Ninja)
            set(CMAKE_GENERATOR_PLATFORM "")
        endif()

        _mc_lab_core_require_target_architecture()
    else()
        message(
            FATAL_ERROR
            "Unknown architecture test mode "
            "'${MC_LAB_CORE_ARCHITECTURE_TEST_MODE}'."
        )
    endif()

    return()
endif()

function(
    mc_lab_core_expect_architecture_success
    case_name
)
    execute_process(
        COMMAND
            "${CMAKE_COMMAND}"
            ${ARGN}
            -P
            "${CMAKE_CURRENT_LIST_FILE}"
        RESULT_VARIABLE mc_lab_core_result
        OUTPUT_VARIABLE mc_lab_core_stdout
        ERROR_VARIABLE mc_lab_core_stderr
    )

    if(NOT mc_lab_core_result EQUAL 0)
        message(
            FATAL_ERROR
            "${case_name}: expected success, received "
            "${mc_lab_core_result}.\n"
            "${mc_lab_core_stdout}\n"
            "${mc_lab_core_stderr}"
        )
    endif()

    message(STATUS "PASS: ${case_name}")
endfunction()

function(
    mc_lab_core_expect_architecture_failure
    case_name
    expected_diagnostic
)
    execute_process(
        COMMAND
            "${CMAKE_COMMAND}"
            ${ARGN}
            -P
            "${CMAKE_CURRENT_LIST_FILE}"
        RESULT_VARIABLE mc_lab_core_result
        OUTPUT_VARIABLE mc_lab_core_stdout
        ERROR_VARIABLE mc_lab_core_stderr
    )

    string(
        CONCAT
        mc_lab_core_output
        "${mc_lab_core_stdout}"
        "\n"
        "${mc_lab_core_stderr}"
    )

    if(mc_lab_core_result EQUAL 0)
        message(
            FATAL_ERROR
            "${case_name}: expected the contract to reject this case."
        )
    endif()

    if(NOT mc_lab_core_output MATCHES "${expected_diagnostic}")
        message(
            FATAL_ERROR
            "${case_name}: expected diagnostic did not match "
            "'${expected_diagnostic}'.\n"
            "${mc_lab_core_output}"
        )
    endif()

    message(STATUS "PASS: ${case_name}")
endfunction()

# Two positive cases protect the canonical Windows label and the portable
# x86-64 alias used by non-MSVC toolchains.
mc_lab_core_expect_architecture_success(
    "native AMD64 host"
    -DMC_LAB_CORE_ARCHITECTURE_TEST_MODE=HOST
    -DMC_LAB_CORE_ARCHITECTURE_TEST_VALUE=AMD64
)

mc_lab_core_expect_architecture_success(
    "x86_64 target alias"
    -DMC_LAB_CORE_ARCHITECTURE_TEST_MODE=TARGET
    -DMC_LAB_CORE_ARCHITECTURE_TEST_VALUE=x86_64
)

# Four negative cases protect the actual policy boundaries.
mc_lab_core_expect_architecture_failure(
    "reject ARM64 host"
    "requires a native x86-64 host"
    -DMC_LAB_CORE_ARCHITECTURE_TEST_MODE=HOST
    -DMC_LAB_CORE_ARCHITECTURE_TEST_VALUE=ARM64
)

mc_lab_core_expect_architecture_failure(
    "reject x86 target"
    "Compiler architecture"
    -DMC_LAB_CORE_ARCHITECTURE_TEST_MODE=TARGET
    -DMC_LAB_CORE_ARCHITECTURE_TEST_VALUE=x86
)

mc_lab_core_expect_architecture_failure(
    "reject ARM64 target"
    "Compiler architecture"
    -DMC_LAB_CORE_ARCHITECTURE_TEST_MODE=TARGET
    -DMC_LAB_CORE_ARCHITECTURE_TEST_VALUE=ARM64
)

mc_lab_core_expect_architecture_failure(
    "reject Visual Studio ARM64 platform"
    "generator platform 'ARM64'"
    -DMC_LAB_CORE_ARCHITECTURE_TEST_MODE=TARGET
    -DMC_LAB_CORE_ARCHITECTURE_TEST_VALUE=x64
    -DMC_LAB_CORE_ARCHITECTURE_TEST_GENERATOR_PLATFORM=ARM64
)

message(
    STATUS
    "Windows x86-64 architecture contract regression suite passed."
)
