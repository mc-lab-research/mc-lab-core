# ==============================================================================
# MC-LAB-CORE CLI Product Tests
# ==============================================================================

# These tests exercise the installed-product boundary: the executable must
# start successfully and expose stable discovery/version entry points. Detailed
# frontend and runtime behavior remains owned by the corresponding component
# test suites.

add_test(
    NAME mc_lab_core.cli.help
    COMMAND mc_lab_core_cli --help
)

add_test(
    NAME mc_lab_core.cli.version
    COMMAND mc_lab_core_cli --version
)

set_tests_properties(
    mc_lab_core.cli.help
    mc_lab_core.cli.version
    PROPERTIES
        LABELS "cli;smoke"
        TIMEOUT 10
)
