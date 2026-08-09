# ==============================================================================
# MC-LAB-CORE Semantic Template Library
# ==============================================================================

# SemTL starts as a header-only structural semantic library. Atomic facets,
# acceptance conditions, recognized formalisms, and structural views remain
# zero-overhead contracts. Algorithms and their behavioral result
# representations stay outside this target.
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
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/acceptance/acceptance_condition.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/acceptance/accepting_state_set.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/acceptance/buchi_acceptance.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/acceptance/final_state_acceptance.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/detail/acceptance_condition_cpo.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/detail/accepting_states_cpo.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/detail/atomic_propositions_cpo.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/detail/initial_states_cpo.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/detail/post_cpo.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/detail/transition_label_cpo.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/detail/transition_relation_cpos.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/facet/atomic_proposition_labelling.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/facet/initial_state_set.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/facet/post.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/facet/transition_labelling.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/facet/transition_relation.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/system/buchi_automaton.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/system/finite_automaton.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/system/kripke_structure.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/system/transition_system.hpp"
            "${PROJECT_SOURCE_DIR}/include/mc_lab/semantic/view/synchronous_composition.hpp"
)

set_target_properties(
    mc_lab_core_semantic
    PROPERTIES
        FOLDER "semantic"
)
