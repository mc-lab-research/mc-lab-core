# CI architecture and governance

This document is the central source of truth for mcLab continuous integration (CI) architecture, responsibilities, execution policies, and local reproduction.

Related governance audit: [CI governance baseline](governance.md).

## CI model

mcLab CI separates:

- **Tracked repository workflows**: workflow files versioned under `.github/workflows/`.
- **GitHub-managed automation**: repository services such as Dependabot that act on CI configuration but are not tracked workflow YAML files.
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

### Tracked repository workflows

| Workflow | Concern | Trigger | Status | Platform | Toolchain | Local reproduction | Artifacts |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Quality Gate | quality | `pull_request`, `push` (`master`), `workflow_dispatch` | **Required** | Ubuntu 24.04 | GCC + Ninja | `cmake --workflow --preset linux-gcc-commit-gate --fresh` | none |
| Linux GCC | build/test | `push` (`master`), `workflow_dispatch` | **Post-merge validation** | Ubuntu 24.04 | GCC + Ninja | `cmake --workflow --preset linux-gcc-debug --fresh` and `cmake --workflow --preset linux-gcc-release --fresh` | none |
| macOS AppleClang | build/test, portability | `push` (`master`), `workflow_dispatch` | **Post-merge validation** | macOS 15 | AppleClang + Ninja | `cmake --workflow --preset macos-appleclang-debug --fresh` and `cmake --workflow --preset macos-appleclang-release --fresh` | none |
| Windows MSVC | build/test, portability | `push` (`master`), `workflow_dispatch` | **Post-merge validation** | Windows 2022 | Visual Studio 2022 / MSVC | `cmake --workflow --preset windows-visualstudio-2022-debug --fresh` and `cmake --workflow --preset windows-visualstudio-2022-release --fresh` | none |
| AddressSanitizer | sanitizer | `pull_request`, `push` (`master`), `workflow_dispatch` | **Required** (canonical) | Ubuntu 24.04 | GCC AddressSanitizer | `cmake --workflow --preset linux-gcc-asan --fresh` | none |
| Static analysis | quality | `pull_request`, `push` (`master`), `workflow_dispatch` | **Required** (Linux canonical), **Optional** (macOS, Windows jobs) | Ubuntu 24.04, macOS 15, Windows 2022 | clang-tidy (Clang/AppleClang/clang-cl) | `cmake --workflow --preset linux-clang-tidy --fresh`, `cmake --workflow --preset macos-appleclang-tidy --fresh`, `cmake --workflow --preset windows-clangcl-tidy --fresh` | none |
| Coverage | coverage | `pull_request`, `push` (`master`), `workflow_dispatch` | **Advisory** (Linux GCC canonical threshold job) | Ubuntu 24.04, macOS 15, Windows 2022 | GCC/lcov, LLVM coverage tools, clang-cl | `cmake --workflow --preset linux-gcc-coverage --fresh`, `cmake --workflow --preset linux-clang-coverage --fresh`, `cmake --workflow --preset macos-appleclang-coverage --fresh`, `cmake --workflow --preset windows-clangcl-coverage --fresh` | `coverage-*` artifacts (coverage reports, 14-day retention) |
| Release portability | build/test, release portability | `pull_request`, `workflow_dispatch` | **Required** | Ubuntu 24.04, macOS 15, Windows 2022 | GCC, AppleClang, Visual Studio 2022 / MSVC | `cmake --workflow --preset linux-gcc-release --fresh`, `cmake --workflow --preset macos-appleclang-release --fresh`, `cmake --workflow --preset windows-visualstudio-2022-release --fresh` | none |
| Native builds | build/test, compiler diversity | `push` (all branches), `workflow_dispatch` | **Advisory** (Linux Clang only) | Ubuntu 24.04 | Clang + Ninja | `cmake --workflow --preset linux-clang-debug --fresh`, `cmake --workflow --preset linux-clang-release --fresh` | none |
| AddressSanitizer matrix | sanitizer, portability | `workflow_dispatch` (manual target input) | **Manual** (exhaustive matrix) | Ubuntu 24.04, macOS 15, Windows 2022, Windows 2025-vs2026 | GCC, Clang, AppleClang, MSVC, Visual Studio 2022/2026, clang-cl, MSYS2 CLANG64 | `cmake --workflow --preset linux-gcc-asan --fresh`, `linux-clang-asan`, `macos-appleclang-asan`, `windows-msvc-asan`, `windows-visualstudio-2022-asan`, `windows-visualstudio-2026-asan`, `windows-clangcl-asan`, `windows-clang64-asan` | none |
| Repository visualization | repository documentation | weekly schedule, `workflow_dispatch` | **Advisory** | Ubuntu 24.04 | Repo Visualizer | no direct local equivalent (GitHub-hosted workflow) | `repository-visualization` artifact (SVG, 14-day retention) |
| OpenSSF Scorecard | security governance | `push` (`master`), `schedule`, `workflow_dispatch` | **Advisory** | Ubuntu 24.04 | OpenSSF Scorecard action | no direct CMake equivalent (GitHub-hosted security workflow) | SARIF uploaded to GitHub Security dashboard |

### GitHub-managed automation related to CI governance

| Automation | Source | Scope | Evidence |
| --- | --- | --- | --- |
| Dependabot version updates | GitHub-managed service using [`.github/dependabot.yml`](../../.github/dependabot.yml) | Weekly GitHub Actions dependency update checks for workflow action references | Repository configuration file plus closed follow-up issue [#52](https://github.com/mc-lab-research/mc-lab-core/issues/52) documenting GitHub acceptance and a successful update-check validation |

## Validation categories

### Canonical quality gates (required)

Expected required checks for merge to `master`:

- `Linux GCC Commit Gate`
- `Linux GCC / Release`
- `Linux / GCC / AddressSanitizer (canonical)`
- `macOS AppleClang / Release`
- `Linux / Clang / Static analysis (canonical)`
- `Windows MSVC / Release`

These names come from job `name:` fields and must stay aligned with the live branch ruleset configuration for `master`.

### Continuously validated configurations

Automatically validated on pull requests:

- Release portability: Linux GCC Release, macOS AppleClang Release, and Windows MSVC Release
- Linux GCC AddressSanitizer
- Linux Clang static analysis (canonical)
- Additional optional/advisory jobs in static analysis and coverage
Automatically validated outside pull requests:

- Native builds on every branch push
- Repository visualization on weekly schedule or manual dispatch
- OpenSSF Scorecard on pushes to `master`, weekly schedule, or manual dispatch

### Manual exhaustive matrices

- `AddressSanitizer matrix` is intentionally manual and supports targeted or full exhaustive ASan coverage.

The advisory `Native builds` workflow retains the Linux Clang Debug/Release pair for compiler diversity on every branch push. The `Release portability` workflow validates Linux GCC, macOS AppleClang, and Windows MSVC Release builds on pull requests. The dedicated platform workflows retain Debug/Release validation after merge to `master` and for manual runs.

### Configured but not continuously validated presets

CMake presets include additional configurations not always run in required CI (for example: Windows Visual Studio 2026 ASan, Windows MSYS2 CLANG64 ASan, and non-canonical coverage/static-analysis variants). They are available for manual validation and troubleshooting.

### Unsupported or intentionally excluded combinations

- Windows x86 (32-bit), ARM, and ARM64 host/target combinations are outside the declared preset contract.
- MSYS2 UCRT64 GCC ASan is intentionally omitted from guaranteed ASan CI.
- Non-Apple LLVM/GCC macOS variants are not part of official continuously validated support.

## Triggers and execution policy

- **Automatic (`pull_request`)**: merge-path quality and release-portability checks.
- **Automatic (`push` to `master`)**: post-merge validation and governance/security workflows.
- **Automatic (`push` all branches)**: advisory Linux Clang `Native builds` feedback.
- **Manual (`workflow_dispatch`)**: maintainers can run any enabled workflow on demand.
- **Scheduled**: `OpenSSF Scorecard` and `Repository visualization` run weekly.
- **GitHub-managed scheduled service**: Dependabot evaluates the configured `github-actions` ecosystem weekly on Monday at 06:00 Europe/Paris according to [`.github/dependabot.yml`](../../.github/dependabot.yml).

## Artifacts and reports

- `Coverage` uploads per-platform coverage directories as artifacts (`coverage-*`).
- The canonical Linux/GCC LCOV report is uploaded to Codacy only after a successful push to `master`, using the `CODACY_PROJECT_TOKEN` repository secret.
- `Repository visualization` uploads `repository-visualization`, an SVG artifact that is not committed to the repository.
- `OpenSSF Scorecard` uploads SARIF to GitHub Security.
- Other workflows are pass/fail validations without retained artifacts.

## Permissions and security expectations

- Baseline expectation for repository workflows: top-level `permissions: contents: read`.
- Exception: `OpenSSF Scorecard` declares top-level `permissions: read-all`, then narrows the executing job to `contents: read`, `actions: read`, `security-events: write`, and `id-token: write` so the workflow can read workflow metadata, mint an OIDC token, and upload SARIF to GitHub Security.
- `actions/checkout` uses `persist-credentials: false` in the `Release portability`, `OpenSSF Scorecard`, and `Repository visualization` workflows.
- Third-party actions must be pinned to immutable full commit SHAs.

## Action pinning policy

All third-party actions in active workflows are pinned to full 40-character commit SHAs (for example `actions/checkout`, `actions/upload-artifact`, `ossf/scorecard-action`, `github/codeql-action/upload-sarif`, `githubocto/repo-visualizer`, `msys2/setup-msys2`). Floating tags (`@vX`, `@main`) are not acceptable for CI governance.

## Timeout and concurrency policy

- Every workflow job declares `timeout-minutes` to prevent runaway jobs.
- Most branch-scoped workflows define `concurrency` with `cancel-in-progress: true` so superseded branch runs are cancelled.
- `Repository visualization` uses workflow-level concurrency with `cancel-in-progress: false`, preserving at most one scheduled/manual generation run at a time instead of cancelling an in-flight artifact build.
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
  4. confirm required check names still match the live ruleset configuration.
- Use the manual `AddressSanitizer matrix` workflow for targeted cross-platform incident reproduction.
