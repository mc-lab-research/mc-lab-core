# ==============================================================================
# MC-LAB-CORE Semantic Template Library
# ==============================================================================

# SemTL starts as a header-only semantic contract library. Structural facets,
# recognized formalisms, and behavioral carriers remain zero-overhead type
# contracts. Concrete representations and algorithms stay in separate targets
# so consumers pay only for the abstractions they use.
add_library(
    mc_lab_core_semantic
    INTERFACE
)

add_library(
    MC_LAB_CORE::Semantic
    ALIAS
    mc_lab_core_semantic
)

target_link_libraries(
    mc_lab_core_semantic
    INTERFACE
        MC_LAB_CORE::ProjectOptions
)

target_include_directories(
    mc_lab_core_semantic
    INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_sources(
    mc_lab_core_semantic
    INTERFACE
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS
            "${PROJECT_SOURCE_DIR}/include"
        FILES
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/behavior/buchi_run.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/behavior/execution.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/behavior/execution_range.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/behavior/kripke_path.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/detail/accepting_states_cpo.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/detail/atomic_propositions_cpo.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/detail/initial_states_cpo.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/detail/transition_label_cpo.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/detail/transition_relation_cpos.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/facet/accepting_state_set.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/facet/atomic_proposition_labelling.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/facet/initial_state_set.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/facet/labelled_transition_relation.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/facet/transition_labelling.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/facet/transition_relation.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/system/buchi_automaton.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/system/finite_automaton.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/system/kripke_structure.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/system/transition_system.hpp"
)

set_target_properties(
    mc_lab_core_semantic
    PROPERTIES
        FOLDER "semantic"
)
