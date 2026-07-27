# ==============================================================================
# MC-LAB-CORE Build-Infrastructure Regression Tests
# ==============================================================================

# Keep CMake's own contract tests separate from product tests. These tests do
# not compile code; they protect the behavior and diagnostics of repository
# build infrastructure on every supported host.
add_test(
    NAME mc_lab_core.cmake.windows_architecture_contract
    COMMAND
        "${CMAKE_COMMAND}"
        -P
        "${CMAKE_CURRENT_LIST_DIR}/windows_architecture_test.cmake"
)

set_tests_properties(
    mc_lab_core.cmake.windows_architecture_contract
    PROPERTIES
        LABELS "cmake;infrastructure"
        TIMEOUT 30
)
