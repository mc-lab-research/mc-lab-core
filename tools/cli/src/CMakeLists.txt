# ==============================================================================
# MC-LAB-CORE CLI Executable
# ==============================================================================

add_executable(
    mc_lab_core_cli
    main.cpp
)

add_executable(
    MC_LAB_CORE::CLI
    ALIAS
    mc_lab_core_cli
)

# Keep the CMake target globally unique while exposing a concise user-facing
# executable name in build and installation artifacts.
set_target_properties(
    mc_lab_core_cli
    PROPERTIES
        OUTPUT_NAME "mc-lab"
        FOLDER "tools/cli"
)

target_compile_definitions(
    mc_lab_core_cli
    PRIVATE
        MC_LAB_CORE_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(
    mc_lab_core_cli
    PRIVATE
        MC_LAB_CORE::ProjectOptions
)

# Quality policies are target-scoped. Applying each helper here keeps
# third-party dependencies outside the repository's warning, instrumentation,
# and analysis contracts.
mc_lab_core_enable_warnings(mc_lab_core_cli)
mc_lab_core_enable_sanitizers(mc_lab_core_cli)
mc_lab_core_enable_coverage(mc_lab_core_cli)
mc_lab_core_register_static_analysis_target(mc_lab_core_cli)

install(
    TARGETS
        mc_lab_core_cli
    RUNTIME
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT
        cli
)
