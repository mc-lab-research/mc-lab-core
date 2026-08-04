# CI architecture and governance

This document is the central source of truth for mcLab continuous integration (CI) architecture, responsibilities, execution policies, and local reproduction.

Related governance audit: [CI governance baseline](governance.md).

## CI model

mcLab CI separates:

- **Canonical quality gates**: required checks that must pass before merge to `master`.
- **Continuously validated configurations**: workflows that run automatically on pull requests and/or pushes.
- **Advisory coverage**: automatically executed checks that are informative but not merge-blocking.
- **Manual exhaustive matrices**: on-demand workflows for broader platform/toolchain validation.
- **Configured but not continuously validated presets**: available CMake presets that are not part of always-on CI.

## Pull-request and push execution

The CI matrix intentionally separates fast compiler-diversity feedback from
cross-platform release-portability validation:

| Event | Workflow | Configurations |
| --- | --- | --- |
| `push` (all branches) | Native builds | Linux Clang Debug and Release |
| `pull_request` | Release portability | Linux GCC Release, macOS AppleClang Release, Windows MSVC Release |
| `push` (`master`) | Dedicated platform workflows | Linux GCC, macOS AppleClang, and Windows MSVC Debug and Release |

`workflow_dispatch` remains available for each of these workflows. The
dedicated platform workflows are post-merge and manual exhaustive validation;
the Release portability workflow is the cross-platform Release check for pull
requests.

## Workflow inventory

| Workflow | Concern | Trigger | Status | Platform | Toolchain | Local reproduction | Artifacts |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Quality Gate | quality | `pull_request`, `push` (`master`), `workflow_dispatch` | **Required** | Ubuntu 24.04 | GCC + Ninja | `cmake --workflow --preset linux-gcc-commit-gate --fresh` | none |
| Linux GCC | build/test | `push` (`master`), `workflow_dispatch` | **Post-merge validation** | Ubuntu 24.04 | GCC + Ninja | `cmake --workflow --preset linux-gcc-debug --fresh` and `cmake --workflow --preset linux-gcc-release --fresh` | none |
| macOS AppleClang | build/test, portability | `push` (`master`), `workflow_dispatch` | **Post-merge validation** | macOS 15 | AppleClang + Ninja | `cmake --workflow --preset macos-appleclang-debug --fresh` and `cmake --workflow --preset macos-appleclang-release --fresh` | none |
| Windows MSVC | build/test, portability | `push` (`master`), `workflow_dispatch` | **Post-merge validation** | Windows 2022 | Visual Studio 2022 / MSVC | `cmake --workflow --preset windows-visualstudio-2022-debug --fresh` and `cmake --workflow --preset windows-visualstudio-2022-release --fresh` | none |
| AddressSanitizer | sanitizer | `pull_request`, `push` (`master`), `workflow_dispatch` | **Required** (canonical) | Ubuntu 24.04 | GCC AddressSanitizer | `cmake --workflow --preset linux-gcc-asan --fresh` | none |
| Static analysis | quality | `pull_request`, `push` (`master`), `workflow_dispatch` | **Required** (Linux canonical), **Optional** (macOS, Windows jobs) | Ubuntu 24.04, macOS 15, Windows 2022 | clang-tidy (Clang/AppleClang/clang-cl) | `cmake --workflow --preset linux-clang-tidy --fresh`, `cmake --workflow --preset macos-appleclang-tidy --fresh`, `cmake --workflow --preset windows-clangcl-tidy --fresh` | none |
| Coverage | coverage | `pull_request`, `push` (`master`), `workflow_dispatch` | **Advisory** (Linux GCC canonical threshold job) | Ubuntu 24.04, macOS 15, Windows 2022 | GCC/lcov, LLVM coverage tools, clang-cl | `cmake --workflow --preset linux-gcc-coverage --fresh`, `cmake --workflow --preset linux-clang-coverage --fresh`, `cmake --workflow --preset macos-appleclang-coverage --fresh`, `cmake --workflow --preset windows-clangcl-coverage --fresh` | `coverage-*` artifacts (coverage reports, 14-day retention) |
| Release portability | build/test, installation, release portability | `pull_request`, `workflow_dispatch` | **Required** | Ubuntu 24.04, macOS 15, Windows 2022 | GCC, AppleClang, Visual Studio 2022 / MSVC | `cmake --workflow --preset linux-gcc-release --fresh`, `cmake --workflow --preset macos-appleclang-release --fresh`, `cmake --workflow --preset windows-visualstudio-2022-release --fresh` | none |
| Native builds | build/test, compiler diversity | `push` (all branches), `workflow_dispatch` | **Advisory** (Linux Clang only) | Ubuntu 24.04 | Clang + Ninja | `cmake --workflow --preset linux-clang-debug --fresh`, `cmake --workflow --preset linux-clang-release --fresh` | none |
| AddressSanitizer matrix | sanitizer, portability | `workflow_dispatch` (manual target input) | **Manual** (exhaustive matrix) | Ubuntu 24.04, macOS 15, Windows 2022, Windows 2025-vs2026 | GCC, Clang, AppleClang, MSVC, Visual Studio 2022/2026, clang-cl, MSYS2 CLANG64 | `cmake --workflow --preset linux-gcc-asan --fresh`, `linux-clang-asan`, `macos-appleclang-asan`, `windows-msvc-asan`, `windows-visualstudio-2022-asan`, `windows-visualstudio-2026-asan`, `windows-clangcl-asan`, `windows-clang64-asan` | none |
| OpenSSF Scorecard | security governance | `push` (`master`), `schedule`, `workflow_dispatch` | **Advisory** | Ubuntu 24.04 | OpenSSF Scorecard action | no direct CMake equivalent (GitHub-hosted security workflow) | SARIF uploaded to GitHub Security dashboard |

## Validation categories

### Canonical quality gates (required)

Expected required checks for merge to `master`:

- `Linux GCC Commit Gate`
- `Linux GCC / Release`
- `Linux / GCC / AddressSanitizer (canonical)`
- `macOS AppleClang / Release`
- `Linux / Clang / Static analysis (canonical)`
- `Windows MSVC / Release`

These names come from job `name:` fields and must stay aligned with branch protection/ruleset configuration.

### Continuously validated configurations

Automatically validated on pull requests:

- Release portability: Linux GCC Release, macOS AppleClang Release, and Windows MSVC Release
- Linux GCC AddressSanitizer
- Linux Clang static analysis (canonical)
- Additional optional/advisory jobs in static analysis and coverage

### Manual exhaustive matrices

- `AddressSanitizer matrix` is intentionally manual and supports targeted or full exhaustive ASan coverage.

The advisory `Native builds` workflow retains the Linux Clang Debug/Release pair for compiler diversity on every branch push. The `Release portability` workflow validates Linux GCC, macOS AppleClang, and Windows MSVC Release builds on pull requests. The dedicated platform workflows retain Debug/Release validation after merge to `master` and for manual runs.

### Installed CLI contract

The v0.1.0 installed product is the `mc-lab` command-line executable. The
Release portability workflow installs the canonical Linux/GCC and
Windows/Visual Studio 2022 Release builds into fresh runner-temporary prefixes.
It then invokes the installed executable from the prefix, outside the build
tree, and verifies that:

- `--help` succeeds and reports the expected usage;
- `--version` succeeds and reports exactly `MC-LAB-CORE CLI 0.1.0`;
- an unsupported option fails; and
- the expected `bin/mc-lab` or `bin/mc-lab.exe` artifact exists.

Reproduce the validation locally after running the corresponding Release
workflow. Use an absolute, previously unused prefix:

```powershell
$installPrefix = Join-Path $PWD 'build/install-validation'
cmake --install build/windows-visualstudio-2022 --config Release --prefix $installPrefix
cmake "-DMC_LAB_CORE_INSTALL_PREFIX=$installPrefix" `
  '-DMC_LAB_CORE_EXPECTED_VERSION=0.1.0' `
  -P cmake/scripts/validate_installed_cli.cmake
```

```sh
install_prefix="$PWD/build/install-validation"
cmake --install build/linux-gcc-release --config Release --prefix "$install_prefix"
cmake -DMC_LAB_CORE_INSTALL_PREFIX="$install_prefix" \
  -DMC_LAB_CORE_EXPECTED_VERSION=0.1.0 \
  -P cmake/scripts/validate_installed_cli.cmake
```

The validator deliberately models only the current CLI contract. When mcLab
exports an installed CMake package or library, add a separate consumer project
and invoke it from this same isolated-install stage; do not make that future
contract depend on the repository build tree.

### Configured but not continuously validated presets

CMake presets include additional configurations not always run in required CI (for example: Windows Visual Studio 2026 ASan, Windows MSYS2 CLANG64 ASan, and non-canonical coverage/static-analysis variants). They are available for manual validation and troubleshooting.

### Unsupported or intentionally excluded combinations

- Windows x86 (32-bit), ARM, and ARM64 host/target combinations are outside the declared preset contract.
- MSYS2 UCRT64 GCC ASan is intentionally omitted from guaranteed ASan CI.
- Non-Apple LLVM/GCC macOS variants are not part of official continuously validated support.

## Triggers and execution policy

- **Automatic (`pull_request`)**: merge-path quality and portability checks.
- **Automatic (`push` to `master`)**: post-merge validation and governance/security workflows.
- **Automatic (`push` all branches)**: broader `Native builds` matrix feedback.
- **Manual (`workflow_dispatch`)**: maintainers can run any enabled workflow on demand.
- **Scheduled**: `OpenSSF Scorecard` runs weekly.

## Artifacts and reports

- `Coverage` uploads per-platform coverage directories as artifacts (`coverage-*`).
- `OpenSSF Scorecard` uploads SARIF to GitHub Security.
- Other workflows are pass/fail validations without retained artifacts.

## Permissions and security expectations

- Default expectation: least privilege (`permissions: contents: read`) for build/test workflows.
- Exception: `OpenSSF Scorecard` uses scoped extra permissions (`security-events: write`, `id-token: write`, `actions: read`) required for SARIF publication and OIDC-based behavior.
- `actions/checkout` uses `persist-credentials: false` in scorecard workflow.
- Third-party actions must be pinned to immutable full commit SHAs.

## Action pinning policy

All third-party actions in active workflows are pinned to full 40-character commit SHAs (for example `actions/checkout`, `actions/upload-artifact`, `ossf/scorecard-action`, `github/codeql-action/upload-sarif`, `msys2/setup-msys2`). Floating tags (`@vX`, `@main`) are not acceptable for CI governance.

## Timeout and concurrency policy

- Every workflow job declares `timeout-minutes` to prevent runaway jobs.
- Most workflows define `concurrency` with `cancel-in-progress: true` so superseded branch runs are cancelled.
- `OpenSSF Scorecard` currently has no explicit `concurrency` block and relies on workflow scheduling frequency and timeout.

## Skipped and unsupported job behavior

- If an optional/advisory job fails or is skipped, merge eligibility depends on required checks only.
- Coverage artifact upload steps are conditionally skipped when expected files are absent (guarded by `hashFiles(...)`).
- Manual matrix dispatch with an unsupported target fails during matrix resolution (`Unsupported target: ...`).
- Unsupported platform/toolchain combinations should fail fast during preset/toolchain contract validation rather than producing ambiguous results.

## Ownership and troubleshooting

- **Owners**: repository maintainers owning `.github/workflows/`, `cmake/presets/`, and repository rulesets.
- **When CI and local results differ**:
  1. run the exact `cmake --workflow --preset ... --fresh` command from this document;
  2. compare compiler/generator/environment summaries in logs;
  3. verify runner OS and tool versions;
  4. confirm required check names still match branch protection settings.
- Use the manual `AddressSanitizer matrix` workflow for targeted cross-platform incident reproduction.
