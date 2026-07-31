# Runtime sanitizer policy

`cmake/modules/sanitizers.cmake` defines target-scoped runtime instrumentation
for first-party mcLab C++ code.

Sanitized binaries are diagnostic artifacts. They are configured, built, and
tested separately from ordinary Debug and Release builds and from distributable
packages.

The configured preset catalog does not by itself establish official platform
support. The active CI matrix is the source of truth for combinations that are
continuously validated.

## Naming scope

The `MC_LAB_CORE_*` variables, targets, and functions in this document reflect
the current CMake implementation. They should be renamed only together with the
corresponding build code.

## Public API

| API | Role |
| --- | --- |
| `mc_lab_core_sanitizers` | Internal `INTERFACE` implementation target |
| `MC_LAB_CORE::Sanitizers` | Read-only alias for target composition |
| `mc_lab_core_enable_sanitizers(target)` | Applies the selected sanitizer policy to a first-party target |
| `MC_LAB_CORE_SANITIZER` | Cache selector for the active runtime sanitizer |

A compiled first-party target opts into the policy explicitly:

```cmake
add_library(
    mc_lab_core_runtime
    STATIC
    runtime.cpp
)

target_link_libraries(
    mc_lab_core_runtime
    PUBLIC
        MC_LAB_CORE::ProjectOptions
)

mc_lab_core_enable_warnings(mc_lab_core_runtime)
mc_lab_core_enable_sanitizers(mc_lab_core_runtime)
```

The helper is safe to call when sanitizers are disabled because the sanitizer
policy target then carries no instrumentation options.

Every final executable and every shared or module library must also call the
helper. Instrumenting only static or object libraries is insufficient because
the final link must include the sanitizer runtime and linker policy.

The helper:

- rejects missing, alias, imported, and unsupported target types;
- applies `INTERFACE` scope to header-only interface libraries;
- applies `PRIVATE` scope to compiled targets; and
- is idempotent.

## Supported selector values

`MC_LAB_CORE_SANITIZER` is a cache string with these values:

| Value | Behavior |
| --- | --- |
| `OFF` | No sanitizer instrumentation |
| `ADDRESS` | AddressSanitizer compile, link, and runtime deployment policy |

Unknown values stop configuration with an actionable diagnostic. Additional
sanitizers must be introduced as explicit policy branches rather than accepted
silently.

## Configured AddressSanitizer presets

The repository currently provides AddressSanitizer workflows for:

| Platform | Toolchain or environment | Workflow preset |
| --- | --- | --- |
| Windows | Native MSVC with Ninja | `windows-msvc-asan` |
| Windows | Visual Studio 2022 | `windows-visualstudio-2022-asan` |
| Windows | Visual Studio 2026 | `windows-visualstudio-2026-asan` |
| Windows | clang-cl with Ninja | `windows-clangcl-asan` |
| Windows | MSYS2 CLANG64 | `windows-clang64-asan` |
| Linux | GCC | `linux-gcc-asan` |
| Linux | Clang | `linux-clang-asan` |
| macOS | AppleClang | `macos-appleclang-asan` |

MSYS2 UCRT64 GCC is deliberately outside the current Windows AddressSanitizer
contract because runtime availability and packaging vary between
distributions. Use the CLANG64 workflow for a GNU-style Windows environment.

## Continuous integration

The `AddressSanitizer` workflow is the automatic sanitizer quality gate. It
runs `linux-gcc-asan` on `ubuntu-24.04` for every pull request and every push to
`master`. This canonical job keeps the required sanitizer path stable and
reasonably fast.

The `AddressSanitizer matrix` workflow is manual. Its `target` input runs either
the complete supported matrix or one selected platform and toolchain:

| Target | GitHub-hosted runner | Workflow preset |
| --- | --- | --- |
| `linux-gcc` | `ubuntu-24.04` | `linux-gcc-asan` |
| `linux-clang` | `ubuntu-24.04` | `linux-clang-asan` |
| `macos-appleclang` | `macos-15` | `macos-appleclang-asan` |
| `windows-msvc` | `windows-2022` | `windows-msvc-asan` |
| `windows-visualstudio-2022` | `windows-2022` | `windows-visualstudio-2022-asan` |
| `windows-visualstudio-2026` | `windows-2025-vs2026` | `windows-visualstudio-2026-asan` |
| `windows-clangcl` | `windows-2022` | `windows-clangcl-asan` |
| `windows-clang64` | `windows-2022` | `windows-clang64-asan` |

The matrix reports the selected compiler and runtime environment before running
the repository workflow preset. A configuration, build, CTest, or sanitizer
failure therefore fails the corresponding job without a CI-specific test path.

## Build isolation

AddressSanitizer changes object code, binary layout, linker behavior, runtime
dependencies, memory consumption, and execution speed. ASan therefore uses
dedicated build trees, for example:

```text
build/linux-clang-debug
build/linux-clang-asan
```

The ASan presets use `RelWithDebInfo`. This keeps debug information while
avoiding the MSVC `/RTC` flags normally associated with Debug configurations.

On MSVC-compatible frontends, AddressSanitizer is incompatible with `/RTC` and
incremental linking. The module therefore:

- rejects a single-configuration Debug ASan build;
- rejects a multi-configuration ASan build containing Debug; and
- adds `/INCREMENTAL:NO` to final links.

The Visual Studio ASan preset consequently exposes only `RelWithDebInfo`.

## Compiler and linker policy

| Frontend | Compile policy | Link and runtime policy |
| --- | --- | --- |
| Native MSVC | `/fsanitize=address` | `/INCREMENTAL:NO`; instrumented objects select the runtime |
| clang-cl | `/fsanitize=address` | Explicit compiler-rt import library and thunk, `/INCREMENTAL:NO`, runtime DLL deployment |
| GCC, Clang, AppleClang | `-fsanitize=address`, frame pointers, no sibling-call optimization | `-fsanitize=address` through the compiler driver |

For GNU-compatible frontends, the final link must run through the compiler
driver so it can select the matching AddressSanitizer runtime.

### clang-cl runtime resolution

With Ninja and clang-cl, CMake may invoke `link.exe` or `lld-link` directly.
The module therefore does not rely on implicit compiler-rt selection. It:

1. queries the active compiler with `-print-resource-dir`;
2. selects the runtime architecture from the configured pointer size;
3. selects the thunk matching the configured MSVC runtime;
4. verifies that the import library, thunk, and runtime DLL exist;
5. links the import library and whole-archive thunk explicitly;
6. forces the ASan SEH interceptor for a dynamic CRT; and
7. copies the runtime DLL beside each final first-party binary.

No LLVM installation directory or version is hard-coded.

### Native MSVC minimum

Native MSVC AddressSanitizer requires MSVC 19.29 or newer. Configuration fails
when an older native MSVC compiler is selected.

## Runtime behavior

The module does not define a global `ASAN_OPTIONS`. Runtime capabilities and
defaults differ between Windows, Linux, and macOS. Test presets or CI jobs may
set platform-specific options when a documented policy requires them.

For useful symbolized reports:

- build with debug information;
- keep `llvm-symbolizer` available for Clang-based workflows or set
  `ASAN_SYMBOLIZER_PATH`; and
- preserve the first failing report when diagnosing a test failure.

An AddressSanitizer failure gives the affected process a non-zero result, so
CTest and workflow presets naturally fail.

## Third-party code and suppressions

Do not apply the first-party sanitizer helper to imported targets.

A defect crossing an uninstrumented dependency boundary may still be observed
when it accesses instrumented memory, but accesses performed entirely inside
the dependency are not guaranteed to be checked.

If a dependency must be instrumented, make that decision explicitly in its
integration layer and verify that its build and runtime model support ASan.

Suppressions are a last resort. Each suppression must identify:

1. the owning dependency or platform runtime;
2. the reported defect or interceptor;
3. why the source cannot be fixed or upgraded immediately; and
4. the condition under which the suppression can be removed.

First-party memory defects must be fixed rather than suppressed.

## Validation

A sanitizer-policy change should:

1. configure the affected `*-asan` workflow;
2. build every affected first-party library and executable;
3. run the complete test suite;
4. confirm that a deliberate memory-safety probe is detected by module tests;
5. exercise every affected compiler family that the project claims to validate.

Examples:

```sh
cmake --workflow --preset windows-msvc-asan
cmake --workflow --preset windows-clangcl-asan
cmake --workflow --preset windows-clang64-asan
cmake --workflow --preset linux-gcc-asan
cmake --workflow --preset linux-clang-asan
cmake --workflow --preset macos-appleclang-asan
```

The Visual Studio workflow may also be exercised explicitly:

```sh
cmake --workflow --preset windows-visualstudio-2022-asan
cmake --workflow --preset windows-visualstudio-2026-asan
```
