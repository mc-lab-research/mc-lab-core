# ARC-001 maintained research copies. These targets reproduce the accepted
# experiment but do not independently define the supported production API.

function(mc_lab_core_configure_arc_001_target target)
    target_link_libraries(${target} PRIVATE MC_LAB_CORE::Semantic)
    set_target_properties(${target} PROPERTIES FOLDER "tests/research/arc-001")
    mc_lab_core_enable_warnings(${target})
    mc_lab_core_enable_sanitizers(${target})
    mc_lab_core_enable_coverage(${target})
    mc_lab_core_register_static_analysis_target(${target})
endfunction()

add_executable(
    mc_lab_core_research_arc_001_calibration_tests
    common/transition_materialization_calibration_test.cpp
)
mc_lab_core_configure_arc_001_target(
    mc_lab_core_research_arc_001_calibration_tests
)
add_test(
    NAME mc_lab_core.research.arc_001.calibration
    COMMAND mc_lab_core_research_arc_001_calibration_tests
)
set_tests_properties(
    mc_lab_core.research.arc_001.calibration
    PROPERTIES
        LABELS "research;arc-001;calibration"
        TIMEOUT 10
)

add_executable(
    mc_lab_core_research_arc_001_a01_owning_value_tests
    a01-owning-value/transition_materialization_lazy_test.cpp
)
mc_lab_core_configure_arc_001_target(
    mc_lab_core_research_arc_001_a01_owning_value_tests
)
add_test(
    NAME mc_lab_core.research.arc_001.a01_owning_value
    COMMAND mc_lab_core_research_arc_001_a01_owning_value_tests
)
set_tests_properties(
    mc_lab_core.research.arc_001.a01_owning_value
    PROPERTIES
        LABELS "research;arc-001;a01;exploratory;inconclusive"
        TIMEOUT 10
)

add_executable(
    mc_lab_core_research_arc_001_a02_ephemeral_proxy_tests
    a02-ephemeral-proxy/transition_materialization_lazy_test.cpp
)
mc_lab_core_configure_arc_001_target(
    mc_lab_core_research_arc_001_a02_ephemeral_proxy_tests
)
add_test(
    NAME mc_lab_core.research.arc_001.a02_ephemeral_proxy
    COMMAND mc_lab_core_research_arc_001_a02_ephemeral_proxy_tests
)
set_tests_properties(
    mc_lab_core.research.arc_001.a02_ephemeral_proxy
    PROPERTIES
        LABELS "research;arc-001;a02;controlled;supported"
        TIMEOUT 10
)
