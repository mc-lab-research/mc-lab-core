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

add_executable(
    mc_lab_core_exploration_policies_tests
    exploration/policies_test.cpp
)

target_link_libraries(
    mc_lab_core_exploration_policies_tests
    PRIVATE
        MC_LAB_CORE::Algorithm
)

set_target_properties(
    mc_lab_core_exploration_policies_tests
    PROPERTIES
        FOLDER "tests/algorithm/exploration"
)

mc_lab_core_enable_warnings(mc_lab_core_exploration_policies_tests)
mc_lab_core_enable_sanitizers(mc_lab_core_exploration_policies_tests)
mc_lab_core_enable_coverage(mc_lab_core_exploration_policies_tests)
mc_lab_core_register_static_analysis_target(mc_lab_core_exploration_policies_tests)

add_test(
    NAME mc_lab_core.algorithm.exploration.policies
    COMMAND mc_lab_core_exploration_policies_tests
)

set_tests_properties(
    mc_lab_core.algorithm.exploration.policies
    PROPERTIES
        LABELS "algorithm;exploration;policy;frontier;visited"
        TIMEOUT 10
)

add_executable(
    mc_lab_core_explore_states_tests
    exploration/explore_states_test.cpp
)

target_link_libraries(
    mc_lab_core_explore_states_tests
    PRIVATE
        MC_LAB_CORE::Algorithm
)

set_target_properties(
    mc_lab_core_explore_states_tests
    PROPERTIES
        FOLDER "tests/algorithm/exploration"
)

mc_lab_core_enable_warnings(mc_lab_core_explore_states_tests)
mc_lab_core_enable_sanitizers(mc_lab_core_explore_states_tests)
mc_lab_core_enable_coverage(mc_lab_core_explore_states_tests)
mc_lab_core_register_static_analysis_target(mc_lab_core_explore_states_tests)

add_test(
    NAME mc_lab_core.algorithm.exploration.explore_states
    COMMAND mc_lab_core_explore_states_tests
)

set_tests_properties(
    mc_lab_core.algorithm.exploration.explore_states
    PROPERTIES
        LABELS "algorithm;exploration;kernel;post;observer"
        TIMEOUT 10
)

add_executable(
    mc_lab_core_explore_transition_relation_tests
    exploration/explore_transition_relation_test.cpp
)

target_link_libraries(
    mc_lab_core_explore_transition_relation_tests
    PRIVATE
        MC_LAB_CORE::Algorithm
)

set_target_properties(
    mc_lab_core_explore_transition_relation_tests
    PROPERTIES
        FOLDER "tests/algorithm/exploration"
)

mc_lab_core_enable_warnings(mc_lab_core_explore_transition_relation_tests)
mc_lab_core_enable_sanitizers(mc_lab_core_explore_transition_relation_tests)
mc_lab_core_enable_coverage(mc_lab_core_explore_transition_relation_tests)
mc_lab_core_register_static_analysis_target(mc_lab_core_explore_transition_relation_tests)

add_test(
    NAME mc_lab_core.algorithm.exploration.explore_transition_relation
    COMMAND mc_lab_core_explore_transition_relation_tests
)

set_tests_properties(
    mc_lab_core.algorithm.exploration.explore_transition_relation
    PROPERTIES
        LABELS "algorithm;exploration;kernel;transition-relation;observer"
        TIMEOUT 10
)

add_executable(
    mc_lab_core_exploration_constexpr_tests
    exploration/constexpr_test.cpp
)

target_link_libraries(
    mc_lab_core_exploration_constexpr_tests
    PRIVATE
        MC_LAB_CORE::Algorithm
)

set_target_properties(
    mc_lab_core_exploration_constexpr_tests
    PROPERTIES
        FOLDER "tests/algorithm/exploration"
)

mc_lab_core_enable_warnings(mc_lab_core_exploration_constexpr_tests)
mc_lab_core_enable_sanitizers(mc_lab_core_exploration_constexpr_tests)
mc_lab_core_enable_coverage(mc_lab_core_exploration_constexpr_tests)
mc_lab_core_register_static_analysis_target(mc_lab_core_exploration_constexpr_tests)

add_test(
    NAME mc_lab_core.algorithm.exploration.constexpr
    COMMAND mc_lab_core_exploration_constexpr_tests
)

set_tests_properties(
    mc_lab_core.algorithm.exploration.constexpr
    PROPERTIES
        LABELS "algorithm;exploration;constexpr"
        TIMEOUT 10
)
