# Validate the installed v0.1.0 command-line contract without referring to a
# target or executable in the build tree.

if(NOT DEFINED MC_LAB_CORE_INSTALL_PREFIX OR MC_LAB_CORE_INSTALL_PREFIX STREQUAL "")
    message(FATAL_ERROR "MC_LAB_CORE_INSTALL_PREFIX must name the isolated installation prefix")
endif()

if(NOT IS_ABSOLUTE "${MC_LAB_CORE_INSTALL_PREFIX}")
    message(FATAL_ERROR "MC_LAB_CORE_INSTALL_PREFIX must be an absolute path")
endif()

if(NOT DEFINED MC_LAB_CORE_EXPECTED_VERSION OR MC_LAB_CORE_EXPECTED_VERSION STREQUAL "")
    message(FATAL_ERROR "MC_LAB_CORE_EXPECTED_VERSION must name the expected product version")
endif()

if(WIN32)
    set(mc_lab_core_cli_name "mc-lab.exe")
else()
    set(mc_lab_core_cli_name "mc-lab")
endif()

set(mc_lab_core_cli "${MC_LAB_CORE_INSTALL_PREFIX}/bin/${mc_lab_core_cli_name}")

if(NOT EXISTS "${mc_lab_core_cli}" OR IS_DIRECTORY "${mc_lab_core_cli}")
    message(FATAL_ERROR "Installed CLI is missing or misplaced: ${mc_lab_core_cli}")
endif()

execute_process(
    COMMAND "${mc_lab_core_cli}" --help
    WORKING_DIRECTORY "${MC_LAB_CORE_INSTALL_PREFIX}"
    RESULT_VARIABLE mc_lab_core_help_result
    OUTPUT_VARIABLE mc_lab_core_help_output
    ERROR_VARIABLE mc_lab_core_help_error
)

if(NOT mc_lab_core_help_result EQUAL 0)
    message(FATAL_ERROR "Installed CLI --help failed: ${mc_lab_core_help_error}")
endif()

if(NOT mc_lab_core_help_output MATCHES "Usage: mc-lab \\[option\\]")
    message(FATAL_ERROR "Installed CLI --help did not report the expected usage")
endif()

execute_process(
    COMMAND "${mc_lab_core_cli}" --version
    WORKING_DIRECTORY "${MC_LAB_CORE_INSTALL_PREFIX}"
    RESULT_VARIABLE mc_lab_core_version_result
    OUTPUT_VARIABLE mc_lab_core_version_output
    ERROR_VARIABLE mc_lab_core_version_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT mc_lab_core_version_result EQUAL 0)
    message(FATAL_ERROR "Installed CLI --version failed: ${mc_lab_core_version_error}")
endif()

set(mc_lab_core_expected_version "MC-LAB-CORE CLI ${MC_LAB_CORE_EXPECTED_VERSION}")
if(NOT mc_lab_core_version_output STREQUAL mc_lab_core_expected_version)
    message(
        FATAL_ERROR
        "Installed CLI version mismatch: expected '${mc_lab_core_expected_version}', got '${mc_lab_core_version_output}'"
    )
endif()

execute_process(
    COMMAND "${mc_lab_core_cli}" --unsupported-installed-contract-probe
    WORKING_DIRECTORY "${MC_LAB_CORE_INSTALL_PREFIX}"
    RESULT_VARIABLE mc_lab_core_invalid_result
    OUTPUT_VARIABLE mc_lab_core_invalid_output
    ERROR_VARIABLE mc_lab_core_invalid_error
)

if(mc_lab_core_invalid_result EQUAL 0)
    message(FATAL_ERROR "Installed CLI accepted an invalid invocation")
endif()

message(STATUS "Installed CLI contract validated: ${mc_lab_core_cli}")
