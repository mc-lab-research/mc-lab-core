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
    mc_lab_core_semantic_post_tests
    post_concepts_test.cpp
)

target_link_libraries(
    mc_lab_core_semantic_post_tests
    PRIVATE
        MC_LAB_CORE::Semantic
)

set_target_properties(
    mc_lab_core_semantic_post_tests
    PROPERTIES
        FOLDER "tests/semantic"
)

mc_lab_core_enable_warnings(mc_lab_core_semantic_post_tests)
mc_lab_core_enable_sanitizers(mc_lab_core_semantic_post_tests)
mc_lab_core_enable_coverage(mc_lab_core_semantic_post_tests)
mc_lab_core_register_static_analysis_target(mc_lab_core_semantic_post_tests)

add_test(
    NAME mc_lab_core.semantic.post_concepts
    COMMAND mc_lab_core_semantic_post_tests
)

set_tests_properties(
    mc_lab_core.semantic.post_concepts
    PROPERTIES
        LABELS "semantic;concepts;facet;post"
        TIMEOUT 10
)

add_executable(
    mc_lab_core_semantic_acceptance_tests
    acceptance_concepts_test.cpp
)

target_link_libraries(
    mc_lab_core_semantic_acceptance_tests
    PRIVATE
        MC_LAB_CORE::Semantic
)

set_target_properties(
    mc_lab_core_semantic_acceptance_tests
    PROPERTIES
        FOLDER "tests/semantic"
)

mc_lab_core_enable_warnings(mc_lab_core_semantic_acceptance_tests)
mc_lab_core_enable_sanitizers(mc_lab_core_semantic_acceptance_tests)
mc_lab_core_enable_coverage(mc_lab_core_semantic_acceptance_tests)
mc_lab_core_register_static_analysis_target(mc_lab_core_semantic_acceptance_tests)

add_test(
    NAME mc_lab_core.semantic.acceptance_concepts
    COMMAND mc_lab_core_semantic_acceptance_tests
)

set_tests_properties(
    mc_lab_core.semantic.acceptance_concepts
    PROPERTIES
        LABELS "semantic;concepts;acceptance"
        TIMEOUT 10
)

add_executable(
    mc_lab_core_semantic_system_tests
    system_concepts_test.cpp
)

target_link_libraries(
    mc_lab_core_semantic_system_tests
    PRIVATE
        MC_LAB_CORE::Semantic
)

set_target_properties(
    mc_lab_core_semantic_system_tests
    PROPERTIES
        FOLDER "tests/semantic"
)

mc_lab_core_enable_warnings(mc_lab_core_semantic_system_tests)
mc_lab_core_enable_sanitizers(mc_lab_core_semantic_system_tests)
mc_lab_core_enable_coverage(mc_lab_core_semantic_system_tests)
mc_lab_core_register_static_analysis_target(mc_lab_core_semantic_system_tests)

add_test(
    NAME mc_lab_core.semantic.system_concepts
    COMMAND mc_lab_core_semantic_system_tests
)

set_tests_properties(
    mc_lab_core.semantic.system_concepts
    PROPERTIES
        LABELS "semantic;concepts;system"
        TIMEOUT 10
)

add_executable(
    mc_lab_core_semantic_synchronous_composition_tests
    synchronous_composition_test.cpp
)

target_link_libraries(
    mc_lab_core_semantic_synchronous_composition_tests
    PRIVATE
        MC_LAB_CORE::Semantic
)

set_target_properties(
    mc_lab_core_semantic_synchronous_composition_tests
    PROPERTIES
        FOLDER "tests/semantic"
)

mc_lab_core_enable_warnings(mc_lab_core_semantic_synchronous_composition_tests)
mc_lab_core_enable_sanitizers(mc_lab_core_semantic_synchronous_composition_tests)
mc_lab_core_enable_coverage(mc_lab_core_semantic_synchronous_composition_tests)
mc_lab_core_register_static_analysis_target(mc_lab_core_semantic_synchronous_composition_tests)

add_test(
    NAME mc_lab_core.semantic.synchronous_composition
    COMMAND mc_lab_core_semantic_synchronous_composition_tests
)

set_tests_properties(
    mc_lab_core.semantic.synchronous_composition
    PROPERTIES
        LABELS "semantic;view;composition;synchronous"
        TIMEOUT 10
)
