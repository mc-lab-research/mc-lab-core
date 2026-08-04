# ==============================================================================
# MC-LAB-CORE CLI Product Tests
# ==============================================================================

# These tests exercise the installed-product boundary: the executable must
# start successfully and expose stable discovery/version entry points. Detailed
# frontend and runtime behavior remains owned by the corresponding component
# test suites.

add_test(
    NAME mc_lab_core.cli.no_arguments
    COMMAND mc_lab_core_cli
)

add_test(
    NAME mc_lab_core.cli.help
    COMMAND mc_lab_core_cli --help
)

add_test(
    NAME mc_lab_core.cli.short_help
    COMMAND mc_lab_core_cli -h
)

add_test(
    NAME mc_lab_core.cli.version
    COMMAND mc_lab_core_cli --version
)

add_test(
    NAME mc_lab_core.cli.unsupported_option
    COMMAND mc_lab_core_cli --unsupported
)

add_test(
    NAME mc_lab_core.cli.extra_arguments
    COMMAND mc_lab_core_cli --help unexpected
)

set_tests_properties(
    mc_lab_core.cli.no_arguments
    mc_lab_core.cli.help
    mc_lab_core.cli.short_help
    mc_lab_core.cli.version
    PROPERTIES
        LABELS "cli;smoke"
        TIMEOUT 10
)

set_tests_properties(
    mc_lab_core.cli.no_arguments
    mc_lab_core.cli.help
    mc_lab_core.cli.short_help
    PROPERTIES
        PASS_REGULAR_EXPRESSION "Usage: mc-lab \\[option\\]"
)

set_tests_properties(
    mc_lab_core.cli.version
    PROPERTIES
        PASS_REGULAR_EXPRESSION "MC-LAB-CORE CLI 0\\.1\\.0"
)

set_tests_properties(
    mc_lab_core.cli.unsupported_option
    mc_lab_core.cli.extra_arguments
    PROPERTIES
        LABELS "cli;negative"
        TIMEOUT 10
        WILL_FAIL TRUE
        PASS_REGULAR_EXPRESSION "mc-lab: unsupported command line"
)
