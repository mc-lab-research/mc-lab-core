# ==============================================================================
# MC-LAB-CORE Algorithms
# ==============================================================================

add_library(
    mc_lab_core_algorithm
    INTERFACE
)

add_library(
    MC_LAB_CORE::Algorithm
    ALIAS
    mc_lab_core_algorithm
)

target_link_libraries(
    mc_lab_core_algorithm
    INTERFACE
        MC_LAB_CORE::Semantic
)

target_sources(
    mc_lab_core_algorithm
    INTERFACE
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS
            "${PROJECT_SOURCE_DIR}/include"
        FILES
            "${PROJECT_SOURCE_DIR}/include/mc_lab/algorithm/exploration/breadth_first_reachable_states.hpp"
)

set_target_properties(
    mc_lab_core_algorithm
    PROPERTIES
        FOLDER "algorithm"
)
