# Windows build environments

mcLab uses the activation mechanisms supplied by MSYS2 and Visual Studio. The
repository does not copy vendor environment variables into presets and does not
modify the global Windows `PATH`.

A valid development environment supplies a coherent compiler, linker, runtime,
headers, libraries, and SDK. The selected CMake preset declares the intended
platform and verifies that CMake resolved the matching toolchain.

> This document describes the Windows environments and presets currently
> provided by the repository. The active CI matrix is the source of truth for
> combinations that are continuously validated.

## Open an environment

Run one of these commands from a PowerShell terminal in the repository:

```powershell
./launch-dev-shell.ps1 ucrt64
./launch-dev-shell.ps1 clang64
./launch-dev-shell.ps1 msvc
```

Each command opens a new interactive terminal rooted at the repository. It
delegates to:

- `msys2_shell.cmd -defterm -here -no-start -ucrt64`;
- `msys2_shell.cmd -defterm -here -no-start -clang64`; or
- the latest `Launch-VsDevShell.ps1` installation containing the Microsoft C++
  build tools.

The launcher prints the recommended configure, build, and test commands for
the selected environment. The Visual Studio shell also points to the clang-cl
and quality alternatives exposed by the preset catalog.

The launcher is a convenience and onboarding tool. The CMake presets and their
configure-time validation remain authoritative.

The script uses `vswhere.exe` for Visual Studio discovery and accepts
prerelease installations. It never chooses a compiler itself.

Before opening an MSYS2 shell, the script checks the environment-specific
compiler, CMake, and Ninja executables. Missing components do not prevent the
shell from opening because that shell is required to install them. The script
prints one `pacman -S --needed ...` command containing only the missing package
groups.

Use `-WhatIf` to verify discovery without opening a terminal:

```powershell
./launch-dev-shell.ps1 ucrt64 -WhatIf
./launch-dev-shell.ps1 msvc -WhatIf
```

## Environment-to-preset mapping

The following table maps each configured activation mechanism to the matching
preset family. It describes repository configuration, not CI validation status.

| Environment | Configure preset | Generator | Host / target | ABI/runtime |
| --- | --- | --- | --- | --- |
| MSYS2 UCRT64 | `windows-ucrt64-gcc-debug` | Ninja | x64 / x64 | MinGW-w64 / UCRT / libstdc++ |
| MSYS2 CLANG64 | `windows-clang64-debug` | Ninja | x64 / x64 | MinGW-w64 / UCRT / libc++ |
| Visual Studio Developer PowerShell | `windows-msvc-debug` | Ninja | x64 / x64 | Microsoft ABI / UCRT |
| Visual Studio Developer PowerShell | `windows-clangcl-debug` | Ninja | x64 / x64 | Microsoft ABI / UCRT |
| Ordinary Windows terminal | `windows-visualstudio-2022` | Visual Studio 2022 | x64 / x64 | Microsoft ABI / UCRT |
| Ordinary Windows terminal | `windows-visualstudio-2026` | Visual Studio 2026 | x64 / x64 | Microsoft ABI / UCRT |

Choose the year-qualified Visual Studio preset matching the installed IDE.
Visual Studio 2022 presets require CMake 3.25 or newer; Visual Studio 2026
presets require CMake 4.2 or newer. Each generator discovers and initializes
its own toolset. The Ninja-based MSVC and clang-cl presets intentionally
require an activated Developer PowerShell.

## Architecture contract

In this project, `x64`, `AMD64`, and `x86_64` all designate the same x86-64
instruction set. The contract is architecture-based and does not restrict the
processor vendor.

The declared Windows preset contract requires a native x64 host and produces
x64 targets. Here, unsupported `x86` means the 32-bit architecture. ARM and
ARM64 hardware are outside this contract, including configurations that emulate
or cross-compile an x64 target. This is an explicit build policy, not an
accidental consequence of the current terminal:

- `host-windows` sets `MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE=x64` and
  `MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE=x64` for every Windows platform and
  quality configuration;
- `launch-dev-shell.ps1` verifies the native operating-system architecture
  before discovering MSYS2 or Visual Studio;
- `launch-dev-shell.ps1` encapsulates the declared architecture and opens the
  Visual Studio shell with native x64 host and target toolsets;
- preflight validates `VSCMD_ARG_TGT_ARCH` for Visual Studio environments and
  `MSYSTEM_CARCH=x86_64` for MSYS2 environments; and
- configure validation checks the native host processor, produced pointer
  size, Visual Studio generator platform, and compiler architecture when CMake
  exposes it.

The existing directory names are therefore unambiguous:
`build/windows-msvc-debug` can contain only the declared x64 configuration.
An x86 32-bit, ARM, or ARM64 environment fails before compiler detection
instead of silently reusing an incompatible cache. Supporting one of those
architectures in the future would require an explicit project policy change,
architecture-qualified presets and build directories, and dedicated test and
CI coverage.

The contract has a standalone regression suite:

```sh
cmake -P cmake/tests/windows_architecture_test.cmake
```

Its six focused cases cover a native AMD64 host, the portable `x86_64` target
alias, rejection of an ARM64 host, rejection of x86 and ARM64 targets, and
rejection of the Visual Studio ARM64 generator platform. The
build-infrastructure CI is intended to invoke this script alongside
preset-graph validation rather than create a dedicated architecture workflow.

## Preset visibility

Configure presets are the toolchain discovery catalog. Every Windows configure
preset is visible on a Windows host, regardless of the currently activated
shell. Build and test presets are filtered using the parent process
environment:

| Active process environment | Visible configure presets | Visible build and test presets |
| --- | --- | --- |
| Standard Windows | All Windows configure presets | Visual Studio generator presets |
| Visual Studio Developer | All Windows configure presets | Visual Studio generator, MSVC, and clang-cl presets |
| MSYS2 UCRT64 | All Windows configure presets | Visual Studio generator and UCRT64 GCC presets |
| MSYS2 CLANG64 | All Windows configure presets | Visual Studio generator and CLANG64 presets |

The configure catalog retains the `host-windows` operating-system condition, so
it does not offer Windows toolchains on Linux or macOS. On Windows hardware
outside the declared contract, configure presets remain discoverable and
produce the explicit host-architecture diagnostic when selected. Build and
test conditions require both `VCINSTALLDIR` and `VSCMD_VER` for Ninja-based
Microsoft toolchains, or the
exact `MSYSTEM` value `UCRT64` or `CLANG64` for MSYS2.

`windows-visualstudio-2022`, `windows-visualstudio-2026`, and their build and
test presets remain visible in every Windows environment because the Visual
Studio generators perform their own toolset discovery.

CMake workflow presets do not support `condition` in schema version 6. Workflow
names therefore remain globally visible, but invoking one outside its required
environment fails because its first configure preset is disabled.

An IDE evaluates these conditions from the environment inherited by the IDE
process, not from an unrelated integrated terminal. Launch or reload the IDE
from the selected development environment when its preset picker must reflect
that environment for build and test presets.

## Configure failure as onboarding

Windows configure presets are intentionally discoverable before every required
toolchain is installed. This makes configuration diagnostics part of the
onboarding workflow.

When preflight fails, the diagnostic presents two recovery paths:

1. Install or repair the missing vendor toolchain, CMake, and Ninja
   components.
2. If the toolchain is already installed, launch the required repository shell
   and retry the exact selected preset with `--fresh`.

For example, selecting `windows-ucrt64-gcc-debug` from ordinary PowerShell
recommends:

```powershell
./launch-dev-shell.ps1 ucrt64
cmake --fresh --preset windows-ucrt64-gcc-debug
```

The diagnostic first includes the MSYS2 download URL and complete `pacman`
command, then the short path for an existing installation. MSVC and clang-cl
failures similarly list the required Visual Studio workload and components
before providing the developer-shell command.

## Configure, build, and test

From the newly opened terminal:

```sh
cmake --preset windows-ucrt64-gcc-debug
cmake --build --preset windows-ucrt64-gcc-debug
ctest --preset windows-ucrt64-gcc-debug
```

Replace the preset with the one corresponding to the active environment.

Visual Studio uses a year-qualified multi-configuration configure preset. For
Visual Studio 2022:

```sh
cmake --preset windows-visualstudio-2022
cmake --build --preset windows-visualstudio-2022-debug
ctest --preset windows-visualstudio-2022-debug
cmake --build --preset windows-visualstudio-2022-release
ctest --preset windows-visualstudio-2022-release
```

Use the corresponding `windows-visualstudio-2026`,
`windows-visualstudio-2026-debug`, and `windows-visualstudio-2026-release`
names for Visual Studio 2026. There is no unqualified compatibility alias.
Debug and Release for each year share that year's
`build/windows-visualstudio-<year>` tree. Release enables `/WX` for first-party
targets; Debug reports the same warnings without making them fatal.

The Release commit gate uses the same environment:

```sh
cmake --workflow --preset windows-ucrt64-gcc-commit-gate
```

## Validation and failure behavior

Platform presets set `MC_LAB_CORE_EXPECTED_TOOLCHAIN`,
`MC_LAB_CORE_EXPECTED_HOST_ARCHITECTURE`, and
`MC_LAB_CORE_EXPECTED_TARGET_ARCHITECTURE`. Validation runs in two phases:

1. Before C++ compiler detection, the active shell, target architecture, and
   required executable names are checked.
2. After `project()`, the resolved compiler identity, command-line frontend,
   generator, target system, target architecture, and MSYS2 distribution path
   are checked.

No compiler fallback is permitted. For example,
`windows-clang64-debug` cannot silently use an LLVM installation targeting the
Microsoft ABI, even when `clang++` exists on the global `PATH`.

If a build tree was previously configured with another compiler, create a fresh
cache after opening the correct environment:

```sh
cmake --fresh --preset windows-clang64-debug
```

## Toolchain installation

Opening an MSYS2 environment and installing its compiler are separate actions.
Install missing toolchains from the corresponding MSYS2 terminal:

```sh
# UCRT64
pacman -S --needed \
  mingw-w64-ucrt-x86_64-toolchain \
  mingw-w64-ucrt-x86_64-cmake \
  mingw-w64-ucrt-x86_64-ninja

# CLANG64
pacman -S --needed \
  mingw-w64-clang-x86_64-toolchain \
  mingw-w64-clang-x86_64-cmake \
  mingw-w64-clang-x86_64-ninja
```

Use the environment-specific CMake and Ninja packages. Importing the complete
Windows `PATH` into MSYS2 could expose a compiler or build tool for another ABI,
so it is not part of the declared mcLab workflow.

Visual Studio environments require:

- **Desktop development with C++**;
- MSVC x64/x86 build tools;
- a Windows SDK;
- C++ CMake tools for Windows;
- C++ Clang tools for Windows when using the clang-cl presets.

Visual Studio Installer can be found at:
<https://visualstudio.microsoft.com/downloads/>.

The launcher reports when `vswhere`, the C++ component, or
`Launch-VsDevShell.ps1` is unavailable and repeats this installation guidance.

## Non-standard installations

MSYS2 is discovered in this order:

1. `MC_LAB_CORE_MSYS2_ROOT`;
2. `MSYS2_ROOT`;
3. an `msys2_shell.cmd` already visible on `PATH`;
4. `<SystemDrive>\msys64`;
5. `<SystemDrive>\tools\msys64`.

Set `MC_LAB_CORE_MSYS2_ROOT` for an installation elsewhere. This setting locates
the official launcher; it is not used to synthesize an MSYS2 environment.

Visual Studio is always discovered with the installer-provided `vswhere.exe`.
This avoids hard-coding a product year, edition, preview channel, or installation
directory.
