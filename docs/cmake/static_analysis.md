# Static-analysis policy

`cmake/modules/static_analysis.cmake` defines the first-party clang-tidy
workflow.

Static analysis is exposed as an explicit `static-analysis` build target and
runs against the compile database of a concrete platform build. It is
deliberately not attached through `CMAKE_CXX_CLANG_TIDY`.

This separation ensures that:

- ordinary platform builds do not require clang-tidy;
- analysis uses the exact commands of a concrete compiler configuration;
- analysis can fail independently from compilation; and
- the complete analysis pass can report failures across multiple translation
  units.

The configured presets do not by themselves establish official platform
support. The active CI matrix is the source of truth for combinations that are
continuously validated.

## Naming scope

The `MC_LAB_CORE_*` variables and `mc_lab_core_*` functions in this document
reflect the current CMake implementation. They should be renamed only together
with the corresponding build code.

## Public API

| API | Role |
| --- | --- |
| `mc_lab_core_register_static_analysis_target(target)` | Registers one first-party target for source discovery |
| `mc_lab_core_finalize_static_analysis()` | Creates the repository-level `static-analysis` target |
| `MC_LAB_CORE_CLANG_TIDY` | Advanced cache path to the clang-tidy executable |
| `MC_LAB_CORE_CLANG_TIDY_CONFIG` | Advanced cache path to the `.clang-tidy` policy |

A component registers its owning target after declaring it:

```cmake
add_library(
    mc_lab_core_semantic
    STATIC
    semantic.cpp
)

target_link_libraries(
    mc_lab_core_semantic
    PUBLIC
        MC_LAB_CORE::ProjectOptions
)

mc_lab_core_enable_warnings(mc_lab_core_semantic)
mc_lab_core_enable_sanitizers(mc_lab_core_semantic)
mc_lab_core_register_static_analysis_target(mc_lab_core_semantic)
```

The repository root finalizes the registry once, after adding all first-party
components and tests:

```cmake
mc_lab_core_finalize_static_analysis()
```

Registration and finalization are idempotent.

## Registration contract

The registration helper:

- rejects missing targets;
- rejects aliases and directs the caller to the owning target;
- rejects imported targets;
- accepts executables, compiled libraries, object libraries, and interface
  libraries; and
- records each owning target only once.

Registration is metadata-only. It does not invoke clang-tidy and does not modify
normal compilation.

## Source discovery

Finalization discovers translation units from the registered target graph. It:

- reads each target's declared sources;
- includes existing `.cc`, `.cpp`, `.cxx`, and `.c++` files;
- excludes generated files;
- excludes files outside `PROJECT_SOURCE_DIR`;
- ignores generator expressions that cannot be resolved during configuration;
- verifies that selected source files exist;
- removes duplicates; and
- sorts the final manifest deterministically.

Headers are analyzed when they are included by a registered translation unit.
Third-party include directories should be marked `SYSTEM` so compiler and
analysis diagnostics remain outside the first-party quality boundary.

The generated source manifest is written below:

```text
<build>/cmake/static-analysis/sources.cmake
```

## Target dependencies

Before invoking clang-tidy, `static-analysis` builds every registered compiled
target. Interface libraries are not built.

This dependency ensures that target-generated headers, response files, and
other build-time prerequisites exist before analysis begins. The dependency is
incremental, so already current targets are not rebuilt unnecessarily.

The project currently disables CMake C++ module scanning globally. If modules
are introduced later, their analysis and generated metadata will require a
separate reviewed policy.

## Compile database

clang-tidy requires an accurate `compile_commands.json`.

The canonical Tidy workflows therefore reuse concrete Ninja build trees:

```text
windows-clangcl-tidy  -> build/windows-clangcl-debug
windows-clang64-tidy  -> build/windows-clang64-debug
linux-clang-tidy      -> build/linux-clang-debug
macos-appleclang-tidy -> build/macos-appleclang-debug
```

The workflow builds only the `static-analysis` target after configuration. The
normal default build remains unchanged.

Visual Studio generators are not used for the canonical Tidy workflow because
they do not provide the compile database through
`CMAKE_EXPORT_COMPILE_COMMANDS`.

## Tool discovery and failure behavior

Configuration searches for `clang-tidy` beside the selected C++ compiler and
then on `PATH`. The result is stored in the advanced
`MC_LAB_CORE_CLANG_TIDY` cache entry.

Missing analysis tooling never blocks platform configuration or ordinary
builds.

Invoking `static-analysis` performs strict workflow validation and fails with
an actionable diagnostic when:

- clang-tidy is unavailable;
- `.clang-tidy` is missing;
- `compile_commands.json` is unavailable;
- the generated source manifest is missing;
- no first-party translation unit was registered; or
- a clang-tidy invocation returns a non-zero status.

The runner analyzes every registered translation unit even after an earlier
failure, then emits one aggregate failure summary.

## Policy ownership

The runner passes only execution context:

```text
--config-file=<repository policy>
--quiet
-p=<platform build tree>
```

The repository `.clang-tidy` file owns the enabled checks and check-specific
options. The CMake module and runner must not duplicate that policy.

The runner itself does not add `--warnings-as-errors`. Therefore, a policy that
intends every enabled diagnostic to block integration must encode
`WarningsAsErrors` in `.clang-tidy`.

Likewise, any restriction of diagnostics to first-party headers must be encoded
through `HeaderFilterRegex` or equivalent `.clang-tidy` policy. These settings
must not be claimed by this document unless they are present in the committed
configuration.

The `.clang-tidy` file is intended to be the shared source of truth for:

- the explicit CMake target;
- direct command-line invocations;
- IDE integrations; and
- future CI workflows.

Compiler warnings and clang-tidy remain separate controls:

```text
warnings.cmake        -> diagnostics emitted by the selected compiler
static_analysis.cmake -> semantic and structural checks from clang-tidy
```

## Changing the policy

A clang-tidy policy change should:

1. explain the defect class or maintainability rule being introduced;
2. update `.clang-tidy`, including any explicit exclusions;
3. fix existing first-party findings in the same change;
4. keep the selected clang-tidy version explicit;
5. avoid broad check families without a reviewed false-positive policy; and
6. validate every affected Tidy workflow.

Suppressions should be narrow and documented. Prefer a local
`NOLINTNEXTLINE(check-name)` with a reason over broad file-level or global
exclusions.

## Usage

Run a complete workflow:

```sh
cmake --workflow --preset windows-clangcl-tidy
cmake --workflow --preset windows-clang64-tidy
cmake --workflow --preset linux-clang-tidy
cmake --workflow --preset macos-appleclang-tidy
```

Or run the target after configuring the matching platform tree:

```sh
cmake --preset linux-clang-debug
cmake --build --preset linux-clang-tidy
```

The `Static analysis` GitHub Actions workflow runs the canonical Linux Clang,
macOS AppleClang, and Windows clang-cl presets from a clean checkout. It
reports the CMake, compiler, LLVM, and clang-tidy versions before running the
same command shown above. Ordinary builds remain independent from the
clang-tidy tool version.
