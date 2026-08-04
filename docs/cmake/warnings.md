# Compiler warning policy

`cmake/modules/warnings.cmake` defines the warning policy for first-party mcLab
C++ code.

The policy is target-scoped. It does not use global compiler options and does
not modify imported or vendored dependencies.

The configured compiler matrix does not by itself establish official support.
The active CI matrix is the source of truth for compiler versions and
combinations that are continuously validated.

## Naming scope

The `MC_LAB_CORE_*` variables, targets, and functions in this document reflect
the current CMake implementation. They should be renamed only together with the
corresponding build code.

## Public API

The module exposes one implementation target, one read-only alias, and one
preferred helper:

| API | Role |
| --- | --- |
| `mc_lab_core_warnings` | Internal `INTERFACE` implementation target |
| `MC_LAB_CORE::Warnings` | Read-only alias for target composition |
| `mc_lab_core_enable_warnings(target)` | Applies the warning policy to a first-party target |

Component CMake files should normally call the helper:

```cmake
add_library(
    mc_lab_core_ir
    STATIC
    ir.cpp
)

target_link_libraries(
    mc_lab_core_ir
    PUBLIC
        MC_LAB_CORE::ProjectOptions
)

mc_lab_core_enable_warnings(mc_lab_core_ir)
```

The helper applies the warning target with `PRIVATE` scope to compiled targets.
For an `INTERFACE` library, it uses `INTERFACE` scope because that target has no
compilation of its own.

The helper:

- rejects missing targets;
- rejects aliases and directs the caller to the owning target;
- rejects imported targets;
- rejects unsupported target types; and
- is idempotent.

## Compiler policy

The warning set is selected from the detected compiler identity and command-line
frontend:

| Toolchain | Baseline |
| --- | --- |
| Native MSVC | `/W4`, `/permissive-`, and selected numbered diagnostics |
| clang-cl | `/W4`, `/permissive-`, and selected stable Clang diagnostics |
| GCC and MinGW-w64 GCC | Reviewed GNU-compatible warning set |
| Clang and MinGW-w64 Clang | Reviewed GNU-compatible warning set |
| AppleClang | Reviewed GNU-compatible warning set |

### Native MSVC

Native MSVC receives `/W4`, `/permissive-`, and an explicit set of numbered
warnings that are useful but not enabled at the selected baseline.

The policy intentionally avoids `/Wall`. That option includes compiler and
system-header diagnostics that are not consistently actionable as a portable
warnings-as-errors gate.

### clang-cl

clang-cl uses the Microsoft command-line frontend and therefore receives
`/W4` and `/permissive-`.

It also receives reviewed Clang diagnostics, including conversion,
sign-conversion, shadowing, virtual-dispatch, formatting, and implicit
fallthrough checks.

clang-cl does not receive the native MSVC numbered-warning list because it does
not guarantee compatibility with every MSVC warning number.

### GNU-compatible frontends

GCC, Clang, AppleClang, and their MinGW-w64 variants receive:

```text
-Wall
-Wextra
-Wpedantic
-Wconversion
-Wsign-conversion
-Wshadow
-Wnon-virtual-dtor
-Wold-style-cast
-Woverloaded-virtual
-Wdouble-promotion
-Wformat=2
-Wimplicit-fallthrough
```

The shared branch intentionally uses a reviewed intersection rather than
experimental or compiler-version-specific diagnostics.

### Unknown compilers

An unknown compiler remains configurable for experimentation, but CMake emits a
warning explaining that no explicit mcLab warning policy exists for that
compiler and version.

Such a compiler is outside the declared warning matrix until a reviewed policy
and validation workflow are added.

## Warnings as errors

`MC_LAB_CORE_COMPILER_WARNINGS_AS_ERRORS` controls whether enabled diagnostics
become errors.

`MC_LAB_CORE_WARNINGS_AS_ERRORS_CONFIGURATIONS` may restrict the policy to a
semicolon-separated configuration list:

```text
false                    -> warnings remain non-fatal
true + empty list        -> /WX or -Werror in every configuration
true + "Release"         -> /WX or -Werror only in Release
```

The repository presets establish this policy:

| Configuration | Behavior |
| --- | --- |
| Platform Debug | Warnings enabled, not treated as errors |
| Platform Release | Warnings treated as errors |
| Visual Studio multi-config tree | Warnings as errors restricted to Release |
| Platform commit gate | Uses the corresponding Release configuration |

The warning set remains the same between Debug and Release. The strict
configuration changes the consequence of a warning, not the classes of
diagnostics being evaluated.

Configuration restrictions are implemented with generator expressions, so
switching a Visual Studio configuration does not require reconfiguring the
build tree.

## Dependency boundary

Warnings-as-errors must apply only to code owned by the project.

Therefore:

- do not call `mc_lab_core_enable_warnings()` on imported targets;
- do not link `MC_LAB_CORE::Warnings` into third-party targets;
- mark third-party include directories as `SYSTEM` when consumed by first-party
  targets; and
- apply unavoidable suppressions as narrowly as possible to the owning source
  file or target.

A suppression should identify the compiler, diagnostic, reason, and removal
condition.

The helper enforces part of this boundary by rejecting aliases and imported
targets during configuration.

## Changing the warning set

A new warning should be added only when:

1. it detects a defect class relevant to mcLab;
2. it is supported by every compiler receiving that policy branch;
3. the existing first-party code is clean or fixed in the same change;
4. third-party headers remain outside the diagnostic boundary; and
5. Debug and warnings-as-errors builds have both been validated.

Compiler-specific warnings must remain inside their compiler branch. A warning
must not be added to the shared GNU-compatible branch based on support from only
one GCC or Clang version.

Changes to the warning policy should remain focused. They should not be hidden
inside an unrelated feature implementation.

## Validation

At minimum, a change to this module should configure and compile a first-party
probe with warnings-as-errors enabled.

The affected commit-gate workflows should then be exercised, for example:

```sh
cmake --workflow --preset windows-clangcl-commit-gate
cmake --workflow --preset windows-clang64-commit-gate
cmake --workflow --preset linux-gcc-commit-gate
cmake --workflow --preset linux-clang-commit-gate
cmake --workflow --preset macos-appleclang-commit-gate
```

Additional Windows workflows should be included when their policy branch is
affected:

```sh
cmake --workflow --preset windows-msvc-commit-gate
cmake --workflow --preset windows-visualstudio-2022-commit-gate
cmake --workflow --preset windows-visualstudio-2026-commit-gate
cmake --workflow --preset windows-ucrt64-gcc-commit-gate
```

When CI is introduced, platform-specific jobs remain authoritative because a
compiler accepting a warning switch does not guarantee identical diagnostic
behavior across compiler versions.
