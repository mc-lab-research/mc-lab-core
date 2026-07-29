# CMake preset architecture

The mcLab preset architecture provides explicit local workflows for supported
host environments, compiler families, build configurations, and quality tools.

> The preset catalog describes the build configurations provided by the
> repository. Official platform support is established only by the active CI
> matrix and published project status.

## Scope and source of truth

This document describes the presets committed in the repository. It does not
replace the preset files themselves.

The sources of truth are:

- `CMakePresets.json` and `cmake/presets/` for available preset definitions;
- CMake configure-time validation for environment and toolchain contracts;
- CI workflows for combinations that are continuously validated;
- module-specific documentation for warnings, sanitizers, analysis,
  formatting, and configuration summaries.

## Files

- `CMakePresets.json` is the stable project entry point.
- `cmake/presets/base.json` contains only host-independent defaults,
  Debug/Release configuration layers, and shared test behavior.
- `cmake/presets/hosts.json` contains all operating-system and activated-shell
  visibility guards for configure, build, and test presets.
- `cmake/presets/compilers.json` contains logical compiler selectors resolved
  by the active host or vendor development environment.
- `cmake/presets/generators.json` separates Ninja from the optional Visual
  Studio 2026 multi-configuration backend.
- `cmake/presets/platforms.json` contains the Debug and Release platform matrix.
- `cmake/presets/profiles.json` contains the hidden development layer and the
  host-independent release compatibility workflow.
- `cmake/presets/quality/formatting.json` encapsulates the formatting targets.
- `cmake/presets/quality/tidy.json` contains the platform-aware static-analysis
  build presets.
- `cmake/presets/quality/asan.json` contains the instrumented AddressSanitizer
  configure, build, test, and workflow presets.
- `cmake/presets/quality/coverage.json` contains the instrumented coverage
  pipelines.
- `cmake/presets/quality/commit-gate.json` composes formatting, Release builds
  with warnings-as-errors, and tests for each platform.

All files use CMake Presets schema version 6 and require CMake 3.25 or newer.

Preset includes are directional. `formatting.json` owns only reusable target
mixins and has no platform dependency. `commit-gate.json` includes both
`platforms.json` and `formatting.json` because its build and workflow presets
reference definitions from both files. Including those files side by side only
from `CMakePresets.json` would not make them reachable from each other.

The platform include chain is similarly layered:

```text
base.json <- hosts.json <- platforms.json
                            ^ compilers.json
                            ^ generators.json
```

Only `hosts.json` includes `base.json`, because its test guards inherit the
host-independent `test-base` policy. Compiler and generator selectors remain
independent mixins consumed directly by `platforms.json`.

The target-scoped compiler warning policy and its extension rules are
documented in [`cmake/modules/warnings.md`](cmake/modules/warnings.md).
The runtime instrumentation contract is documented in
[`cmake/modules/sanitizers.md`](cmake/modules/sanitizers.md).
The target registration and clang-tidy execution model are documented in
[`cmake/modules/static_analysis.md`](cmake/modules/static_analysis.md).
The Git-backed formatting and staged-file safety contract are documented in
[`cmake/modules/formatting.md`](cmake/modules/formatting.md).
The final configure-time state and optional-tool reporting contract are
documented in
[`cmake/modules/config_summary.md`](cmake/modules/config_summary.md).
The Windows shell activation and toolchain-validation contract is documented
in
[`development/windows-build-environments.md`](development/windows-build-environments.md).

## Configured platform matrix

| Environment | Compiler presets | Architectures | Configurations |
| --- | --- | --- | --- |
| Windows | MSVC, clang-cl, MSYS2 UCRT64 GCC, MSYS2 CLANG64 | Native x64 host and x64 target | Debug, Release |
| Linux | GCC, Clang | Native | Debug, Release |
| macOS | AppleClang | Native | Debug, Release |

Ubuntu, Debian, and Fedora use the same Linux presets. Distribution-specific
validation belongs in the CI runner or container matrix rather than in
duplicate CMake preset aliases.

The table above is a configured preset matrix, not a claim that every
combination is already continuously validated. The active CI workflows are
the source of truth for validated combinations.

AppleClang is the portable macOS default. Non-Apple LLVM and GNU GCC
installations commonly use package-manager-specific, versioned paths and belong
in an uncommitted `CMakeUserPresets.json`.

The explicit compiler matrix uses Ninja. Windows also provides one
`windows-visualstudio` configure preset backed by the multi-configuration
Visual Studio 2026 generator. The `windows-visualstudio-debug` and
`windows-visualstudio-release` build and test presets select their
configuration inside the shared `build/windows-visualstudio` tree.

The shared Visual Studio configuration enables the warnings-as-errors policy
only for `Release`. CMake generator expressions therefore add `/WX` to Release
compilations without making interactive Debug builds blocking.

All Windows configure presets inherit explicit x64 host and target contracts
from `host-windows`. Here, `x64`, `AMD64`, and `x86_64` identify the same
vendor-independent x86-64 architecture. The launcher and configure-time
validation reject x86 32-bit, ARM, and ARM64 hosts and targets before an
incompatible compiler can reuse an existing build tree. Cross-compiling or
emulating x64 from ARM hardware is intentionally outside the declared Windows
preset contract.

The standalone
`cmake/tests/windows_architecture_contract_tests.cmake` regression script keeps
six focused host, target, and Visual Studio platform cases. It requires neither
`project()` nor a compiler and is intended for the build-infrastructure CI,
alongside preset-graph validation.

Windows configure presets form a discoverable host-level catalog: every
Windows configure preset is visible on Windows, even before its vendor
environment or compiler has been installed. Selecting an unavailable preset
starts an environment preflight whose failure explains both supported recovery
paths: install or repair the missing toolchain, or launch the required shell
and retry the exact preset when the toolchain is already present.

Build and test presets remain environment-aware. Ninja-based Visual Studio
presets require `VCINSTALLDIR` and `VSCMD_VER`; MSYS2 presets require
`MSYSTEM=UCRT64` or `MSYSTEM=CLANG64`. The Visual Studio generator build and
test presets remain available on every Windows host because the generator
initializes its own toolset.

Workflow presets cannot carry a `condition` in CMake Presets schema version 6.
They remain listed globally, while their configure step is disabled outside the
matching host environment.

## Quality profiles

- Formatting runs the `format-check` or `format-staged` target in an existing
  platform build tree.
- Tidy runs the always-registered `static-analysis` target in compatible
  clang-cl, Clang, or AppleClang Debug platform builds.
- ASan enables `MC_LAB_CORE_SANITIZER=ADDRESS` in an isolated,
  platform-derived build.
- Coverage enables `MC_LAB_CORE_ENABLE_COVERAGE` in an isolated,
  platform-derived build and runs reset, tests, report generation, and policy
  validation.
- Each platform commit gate checks formatting, builds its Release configuration
  with warnings-as-errors, and runs the complete test suite in that same
  platform build tree.

Explicit ASan presets use `RelWithDebInfo` and are provided for MSVC with
Ninja, Visual Studio, clang-cl, MSYS2 CLANG64, Linux GCC/Clang, and
AppleClang. MSYS2 UCRT64 GCC ASan is intentionally omitted from the guaranteed
matrix because availability and runtime packaging vary between distributions.

The coverage presets are Windows clang-cl, Linux GCC, Linux Clang, and macOS
AppleClang. A CI configuration may use Linux Clang as the canonical threshold
while the Windows and macOS workflows remain available for local
platform-specific validation. Debian, Fedora, and Ubuntu can invoke the Linux
presets from separate runners or containers.

## Workflow behavior

A workflow does not select another hidden platform preset. Each step names the
exact configure, build, or test preset that it uses:

| Workflow pattern | Configure | Build or analysis | Test | Build directory |
| --- | --- | --- | --- | --- |
| `<platform>-commit-gate` | `<platform>-release` | `format-check`, then `<platform>-release` | `<platform>-release` | `build/<platform>-release` |
| `windows-visualstudio-commit-gate` | `windows-visualstudio` | `format-check`, then `windows-visualstudio-release` | `windows-visualstudio-release` | `build/windows-visualstudio` |
| `<platform>-tidy` | `<platform>-debug` | `static-analysis` | — | `build/<platform>-debug` |
| `<platform>-asan` | `<platform>-asan` | instrumented default build | `<platform>-asan` | `build/<platform>-asan` |
| `<platform>-coverage` | `<platform>-coverage` | build, reset, report, check | `<platform>-coverage` | `build/<platform>-coverage` |
| `release` | `release` | `release` | `release` | `build/release` |

Every workflow invocation executes its configure step first, but it reuses the
same CMake cache and build directory. No preset sets `cleanFirst`, so Ninja
performs an incremental build and rebuilds only targets affected by changes.
The test step does not compile; it runs the tests from the build produced by the
preceding build step.

The quality workflows name a concrete platform contract. Compiler executable
names are resolved by the activated vendor environment, then validated against
the expected compiler family, frontend, ABI, and distribution. The generic
`release` workflow remains as a compatibility shortcut.

## Usage

List the presets available on the current host:

```sh
cmake --list-presets=configure
cmake --list-presets=build
cmake --list-presets=test
cmake --list-presets=workflow
```

Configure, build, and test an explicit toolchain:

```sh
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-debug
ctest --preset linux-clang-debug
```

Configure, build, and test one Visual Studio configuration:

```sh
cmake --preset windows-visualstudio
cmake --build --preset windows-visualstudio-debug
ctest --preset windows-visualstudio-debug
```

The same configured tree can then build and test Release without another
configure step:

```sh
cmake --build --preset windows-visualstudio-release
ctest --preset windows-visualstudio-release
```

Open a Visual Studio developer environment and run a blocking commit gate:

```powershell
./launch-dev-shell.ps1 msvc
cmake --workflow --preset windows-msvc-commit-gate
```

The preset defines the gate but does not install a Git hook. Invoke this command
from the repository's pre-commit or pre-push integration, and later repeat it as a
required CI check when server-side enforcement is introduced.

Run static analysis in a concrete platform build:

```sh
cmake --workflow --preset linux-clang-tidy
cmake --workflow --preset windows-clang64-tidy
```

Run the complete coverage pipeline:

```sh
cmake --workflow --preset linux-clang-coverage
cmake --workflow --preset windows-clangcl-coverage
cmake --workflow --preset macos-appleclang-coverage
```

## Requirements

- The published Windows preset matrix requires a native x86-64 Windows host and
  targets x86-64. x86 32-bit and ARM/ARM64 hosts and targets are outside the declared preset
  contract.
  The launcher, Visual Studio generator platform, MSYS2 distribution, and
  configure-time validation enforce this contract.
- Ninja must be available in the active environment for the explicit compiler
  matrix.
- Open Ninja-based MSVC and clang-cl environments with
  `./launch-dev-shell.ps1 msvc`.
- clang-cl presets require the Microsoft C++ toolchain and Windows SDK, with
  clang-cl supplied by Visual Studio or an LLVM installation.
- The `windows-visualstudio-*` presets require Visual Studio 2026 and CMake
  4.2 or newer. They select the native x64 host toolset without requiring
  Ninja.
- `windows-ucrt64-gcc-*` presets require
  `./launch-dev-shell.ps1 ucrt64` and the MSYS2 UCRT64 GCC toolchain.
- `windows-clang64-*` presets require
  `./launch-dev-shell.ps1 clang64` and the MSYS2 CLANG64 toolchain.
- Linux GCC presets recommend `build-essential`, CMake, and Ninja on Debian or
  Ubuntu, or `gcc-c++`, CMake, and Ninja on Fedora.
- Linux Clang presets recommend Clang, CMake, and Ninja through the matching
  distribution package manager.
- macOS AppleClang presets require the Apple command-line developer tools,
  CMake, and Ninja.
- Tidy presets require clang-tidy. Formatting presets require clang-format,
  Git, and the repository `.clang-format` policy. These tools are validated
  only when their corresponding quality target is invoked.
- The top-level CMake project must consume `MC_LAB_CORE_ENABLE_COVERAGE` and
  add the compiler-specific coverage instrumentation. It must provide the
  `coverage-reset`, `coverage-report`, and `coverage-check` targets.

`CMakeUserPresets.json` should be ignored by version control and used for IDE
preferences and experimental profiles. Windows compiler discovery belongs to
the activated vendor environment rather than machine-specific compiler paths.
