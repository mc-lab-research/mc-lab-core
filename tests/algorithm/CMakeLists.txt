# ==============================================================================
# Algorithm Tests
# ==============================================================================

add_executable(
    mc_lab_core_breadth_first_reachable_states_tests
    exploration/breadth_first_reachable_states_test.cpp
)

target_link_libraries(
    mc_lab_core_breadth_first_reachable_states_tests
    PRIVATE
        MC_LAB_CORE::Algorithm
)

set_target_properties(
    mc_lab_core_breadth_first_reachable_states_tests
    PROPERTIES
        FOLDER "tests/algorithm/exploration"
)

mc_lab_core_enable_warnings(mc_lab_core_breadth_first_reachable_states_tests)
mc_lab_core_enable_sanitizers(mc_lab_core_breadth_first_reachable_states_tests)
mc_lab_core_enable_coverage(mc_lab_core_breadth_first_reachable_states_tests)
mc_lab_core_register_static_analysis_target(mc_lab_core_breadth_first_reachable_states_tests)

add_test(
    NAME mc_lab_core.algorithm.exploration.breadth_first_reachable_states
    COMMAND mc_lab_core_breadth_first_reachable_states_tests
)

set_tests_properties(
    mc_lab_core.algorithm.exploration.breadth_first_reachable_states
    PROPERTIES
        LABELS "algorithm;exploration;breadth-first;reachability"
        TIMEOUT 10
)
