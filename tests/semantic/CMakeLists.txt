# ==============================================================================
# SemTL Contract Tests
# ==============================================================================

add_executable(
    mc_lab_core_semantic_facet_tests
    facet_concepts_test.cpp
)

target_link_libraries(
    mc_lab_core_semantic_facet_tests
    PRIVATE
        MC_LAB_CORE::Semantic
)

set_target_properties(
    mc_lab_core_semantic_facet_tests
    PROPERTIES
        FOLDER "tests/semantic"
)

mc_lab_core_enable_warnings(mc_lab_core_semantic_facet_tests)
mc_lab_core_enable_sanitizers(mc_lab_core_semantic_facet_tests)
mc_lab_core_enable_coverage(mc_lab_core_semantic_facet_tests)
mc_lab_core_register_static_analysis_target(mc_lab_core_semantic_facet_tests)

add_test(
    NAME mc_lab_core.semantic.facet_concepts
    COMMAND mc_lab_core_semantic_facet_tests
)

set_tests_properties(
    mc_lab_core.semantic.facet_concepts
    PROPERTIES
        LABELS "semantic;concepts"
        TIMEOUT 10
)

add_executable(
    mc_lab_core_semantic_system_behavior_tests
    system_behavior_concepts_test.cpp
)

target_link_libraries(
    mc_lab_core_semantic_system_behavior_tests
    PRIVATE
        MC_LAB_CORE::Semantic
)

set_target_properties(
    mc_lab_core_semantic_system_behavior_tests
    PROPERTIES
        FOLDER "tests/semantic"
)

mc_lab_core_enable_warnings(mc_lab_core_semantic_system_behavior_tests)
mc_lab_core_enable_sanitizers(mc_lab_core_semantic_system_behavior_tests)
mc_lab_core_enable_coverage(mc_lab_core_semantic_system_behavior_tests)
mc_lab_core_register_static_analysis_target(mc_lab_core_semantic_system_behavior_tests)

add_test(
    NAME mc_lab_core.semantic.system_behavior_concepts
    COMMAND mc_lab_core_semantic_system_behavior_tests
)

set_tests_properties(
    mc_lab_core.semantic.system_behavior_concepts
    PROPERTIES
        LABELS "semantic;concepts;system;behavior"
        TIMEOUT 10
)
