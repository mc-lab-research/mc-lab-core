# Configuration summary

`cmake/modules/config_summary.cmake` provides an opt-in, human-readable summary
of the effective top-level mcLab build configuration.

The summary is intended to answer three questions immediately:

1. Which platform, generator, configuration, and compiler did CMake select?
2. Which build policies are active in this build tree?
3. Which optional quality tools and repository targets are available?

The summary reports configured state. It does not establish official platform
support; the active CI matrix remains the source of truth for continuously
validated combinations.

## Public API

```cmake
include(config_summary)

if(PROJECT_IS_TOP_LEVEL)
    mc_lab_core_print_configuration_summary()
endif()
```

Including the module does not print anything by itself. The root build must call
`mc_lab_core_print_configuration_summary()` explicitly after the relevant
first-party and quality targets have been created.

The function is idempotent. Calling it more than once during one configure
operation produces only one summary.

The function returns without output when mcLab is consumed as a subproject. The
parent project therefore retains ownership of its configure log and may provide
its own aggregate summary.

The `mc_lab_core_*` function and variable names in this document reflect the
current CMake implementation. They should be renamed only together with the
corresponding build code.

## Output sections

### Platform and toolchain

This section reports:

- host operating system and processor;
- target operating system and processor;
- whether CMake is cross-compiling;
- generator, generator platform, and generator toolset;
- the selected single-configuration build type or available
  multi-configuration choices;
- compiler identity and version;
- compiler command-line frontend;
- resolved compiler executable;
- declared toolchain, host-architecture, and target-architecture contracts; and
- the activated MSYS2 or Visual Studio development environment, when relevant.

The frontend distinction is significant on Windows. For example, clang-cl may
be reported as:

```text
Clang 22.1.8 (MSVC-style command line)
```

This explains why the compiler consumes MSVC-compatible switches even though
its compiler identity is Clang.

For multi-configuration generators, the summary lists the configurations known
to the build tree. The concrete configuration remains a build-time choice:

```text
multi-config [Debug, Release]
```

### Build contract

This section reports the effective project policies:

- C++20 with compiler extensions disabled;
- test integration;
- compile-database request;
- position-independent code;
- compiler warnings-as-errors;
- selected sanitizer; and
- coverage instrumentation.

The compile-database row describes a request, not a guarantee made by every
CMake generator. Ninja and Makefiles support `compile_commands.json`. For other
generators, the summary adds:

```text
ON (generator support required)
```

This is particularly relevant to Visual Studio generators, while the
clang-tidy workflow uses a separate Ninja-based preset.

### Quality infrastructure

Optional quality tools are reported as resolved executable paths when
available. A missing tool is presented as:

```text
not found (quality target only)
```

This wording is intentional. clang-format, clang-tidy, and Git are not required
for an ordinary C++ build. Their dedicated targets perform strict validation
when invoked.

The section also reports:

- `.clang-format` and `.clang-tidy` policy paths;
- first-party formatting roots;
- the number of targets registered for static analysis; and
- quality targets that exist in the current configuration.

Coverage targets therefore appear only in an instrumented coverage build.
Formatting targets appear only for a top-level project. The summary describes
the generated target graph rather than a planned feature list.

### Directories

The final section reports the source, build, and installation directories. This
is useful when several concrete platform presets are configured concurrently:

```text
build/linux-gcc-debug
build/linux-clang-debug
build/windows-clangcl-release
```

Each summary identifies the exact build tree whose cache and generated targets
are being updated.

## Observational contract

The module is deliberately read-only. Printing the summary:

- does not create or modify cache entries;
- does not change compiler or linker flags;
- does not execute optional external tools;
- does not eagerly validate quality policies;
- does not create quality targets; and
- does not turn missing optional tooling into a configure failure.

Strict validation remains owned by the corresponding workflow:

| Concern | Strict validation point |
| --- | --- |
| Compiler warnings | Compilation of a first-party target |
| AddressSanitizer | Sanitizer configuration and instrumented build |
| clang-format | `format-check` or `format-staged` |
| clang-tidy | `static-analysis` |
| Coverage | Coverage configuration, report, and threshold targets |

This boundary keeps the summary trustworthy: it reports state but never changes
that state to improve its own output.

## Placement in the root build

For complete output, call `mc_lab_core_print_configuration_summary()` at the
end of the root `CMakeLists.txt`, after:

1. every first-party component and test directory has been added;
2. static-analysis registration has been finalized;
3. coverage targets have been finalized when coverage is enabled; and
4. formatting targets have been created for the top-level project.

Calling the function earlier produces an incomplete quality-target list and may
report an incorrect static-analysis registration count.

A correct root integration is therefore:

```cmake
if(PROJECT_IS_TOP_LEVEL)
    include(config_summary)
    mc_lab_core_print_configuration_summary()
endif()
```

## Diagnostic use

The summary is emitted with CMake `STATUS` messages, so it remains readable in
local terminals and CI configure logs without terminal-specific control
sequences.

When a CI job behaves differently from a local preset, compare the summary
sections before investigating source code. Differences in generator, compiler
frontend, configuration, warnings-as-errors, sanitizer, or coverage are usually
visible immediately.
