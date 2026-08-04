# CI governance baseline

This document records the CI governance baseline reviewed for **mcLab** and the evidence used to validate each control.
It covers workflow inventory and classification, trigger conditions, governance controls, and the required check names used to configure the `master` branch ruleset.

Validated on **2026-08-03**.

## Workflow inventory

Nine GitHub Actions workflows are active in the repository.
Seven run on every pull request and push to `master`; one triggers on every push across all branches; one is on-demand only.

| Workflow | File | Responsibility | Classification |
| --- | --- | --- | --- |
| Quality Gate | [`quality-gate.yml`](../../.github/workflows/quality-gate.yml) | Enforces the Linux / GCC commit gate preset — build, test, and formatting verification in one step. This is the primary blocking gate for all merges to `master`. | **Required** |
| Linux GCC | [`linux-gcc.yml`](../../.github/workflows/linux-gcc.yml) | Validates Debug and Release builds on Ubuntu 24.04 with GCC. | **Required** |
| AddressSanitizer | [`asan.yml`](../../.github/workflows/asan.yml) | Runs the canonical Linux / GCC AddressSanitizer build and test suite to catch memory safety regressions. | **Required** |
| macOS AppleClang | [`macos-appleclang.yml`](../../.github/workflows/macos-appleclang.yml) | Validates Debug and Release builds on macOS 15 with AppleClang to ensure cross-platform portability. | **Required** |
| Static analysis | [`static-analysis.yml`](../../.github/workflows/static-analysis.yml) | Runs `clang-tidy` static analysis on Linux (Clang), macOS (AppleClang), and Windows (clang-cl). | **Required** (canonical Linux / Clang job); **Optional** (macOS and Windows jobs) |
| Windows MSVC | [`windows-msvc.yml`](../../.github/workflows/windows-msvc.yml) | Validates Debug and Release builds on Windows 2022 with Visual Studio 2022 / MSVC. | **Required** |
| Coverage | [`coverage.yml`](../../.github/workflows/coverage.yml) | Measures and enforces code coverage thresholds across Linux GCC, Linux Clang, macOS AppleClang, and Windows clang-cl. Publishes HTML coverage reports as workflow artifacts. | **Advisory** |
| Native builds | [`native-builds.yml`](../../.github/workflows/native-builds.yml) | Runs a broader build and test matrix — Ubuntu GCC, Ubuntu Clang, macOS AppleClang, and Windows Visual Studio — on every branch push and pull request. Partially overlaps with the individual required workflows but provides additional toolchain and OS coverage. | **Advisory** |
| AddressSanitizer matrix | [`asan-matrix.yml`](../../.github/workflows/asan-matrix.yml) | On-demand, selectable multi-platform AddressSanitizer validation covering Linux GCC, Linux Clang, macOS AppleClang, Windows MSVC, Windows clang-cl, and Windows CLANG64. Triggered manually via `workflow_dispatch`. | **Advisory** |

### Classification definitions

| Classification | Meaning |
| --- | --- |
| **Required** | The check is configured as a required status check in the `master` branch ruleset. A pull request cannot be merged to `master` until this check passes. |
| **Optional** | The workflow runs on every pull request but is not a required status check. Failure is visible but does not block merge. |
| **Advisory** | The workflow provides additional confidence or on-demand validation but is not part of the pull request critical path. |

## Trigger matrix

| Workflow | `pull_request` | `push` to `master` | `push` (all branches) | `workflow_dispatch` |
| --- | :---: | :---: | :---: | :---: |
| Quality Gate | ✅ | ✅ | — | ✅ |
| Linux GCC | ✅ | ✅ | — | ✅ |
| AddressSanitizer | ✅ | ✅ | — | ✅ |
| macOS AppleClang | ✅ | ✅ | — | ✅ |
| Static analysis | ✅ | ✅ | — | ✅ |
| Windows MSVC | ✅ | ✅ | — | ✅ |
| Coverage | ✅ | ✅ | — | ✅ |
| Native builds | ✅ | — | ✅ | ✅ |
| AddressSanitizer matrix | — | — | — | ✅ |

> **Note — Native builds push scope.** `native-builds.yml` triggers on every push to any branch, not only `master`.
> This is intentional: the broader matrix provides early cross-toolchain feedback on feature branches.
> Because its check names differ from the individual required workflows, it does not interfere with ruleset enforcement.

## Check names and ruleset alignment

The following check names are emitted by GitHub Actions and must match exactly the names configured as required status checks in the `master` branch ruleset.
GitHub Actions derives check names from job-level `name:` fields.

### Required checks (must pass before merge to `master`)

| Check name | Workflow | Job id |
| --- | --- | --- |
| `Linux GCC Commit Gate` | Quality Gate | `quality-gate` |
| `Linux GCC / Debug` | Linux GCC | `linux-gcc` (matrix) |
| `Linux GCC / Release` | Linux GCC | `linux-gcc` (matrix) |
| `Linux / GCC / AddressSanitizer (canonical)` | AddressSanitizer | `linux-gcc` |
| `macOS AppleClang / Debug` | macOS AppleClang | `macos-appleclang` (matrix) |
| `macOS AppleClang / Release` | macOS AppleClang | `macos-appleclang` (matrix) |
| `Linux / Clang / Static analysis (canonical)` | Static analysis | `linux-clang` |
| `Windows MSVC / Debug` | Windows MSVC | `windows-msvc` (matrix) |
| `Windows MSVC / Release` | Windows MSVC | `windows-msvc` (matrix) |

### Optional checks (run on PR but do not block merge)

| Check name | Workflow | Job id |
| --- | --- | --- |
| `macOS 15 / AppleClang / Static analysis` | Static analysis | `macos-appleclang` |
| `Windows 2022 / clang-cl / Static analysis` | Static analysis | `windows-clangcl` |

### Advisory checks (broader validation, not on the PR critical path)

| Check name | Workflow | Note |
| --- | --- | --- |
| `Linux / GCC / Coverage (canonical)` | Coverage | Coverage threshold enforced internally; report published as artifact. |
| `Linux / Clang / Coverage` | Coverage | |
| `macOS 15 / AppleClang / Coverage` | Coverage | |
| `Windows 2022 / clang-cl / Coverage` | Coverage | |
| `Ubuntu 24.04 / GCC / Debug` | Native builds | Broader matrix, complements Linux GCC required check. |
| `Ubuntu 24.04 / GCC / Release` | Native builds | |
| `Ubuntu 24.04 / Clang / Debug` | Native builds | |
| `Ubuntu 24.04 / Clang / Release` | Native builds | |
| `macOS 15 / AppleClang / Debug` | Native builds | |
| `macOS 15 / AppleClang / Release` | Native builds | |
| `Windows 2022 / Visual Studio 2022 / Debug` | Native builds | |
| `Windows 2022 / Visual Studio 2022 / Release` | Native builds | |

## Governance controls

### Permissions — least privilege

| Control | Evidence | Status |
| --- | --- | --- |
| All workflows declare `permissions: contents: read` | Every workflow file under `.github/workflows/` carries a top-level `permissions: contents: read` block. No job overrides elevate permissions further. | Validated |
| No write permissions granted without justification | No workflow writes to any GitHub resource (packages, pull requests, deployments). No elevated scope is present or needed. | Validated |

### External action pinning — immutable SHAs

All third-party GitHub Actions references are pinned to full 40-character commit SHAs.
Version tags appear as comments to assist human readability and Dependabot updates.

| Action reference | Workflow(s) | SHA |
| --- | --- | --- |
| `actions/checkout` | All build and analysis workflows | `3d3c42e5aac5ba805825da76410c181273ba90b1` (v7.0.1) |
| `actions/upload-artifact` | Coverage | `043fb46d1a93c77aae656e7c1c64a875d1fc6a0a` (v7.0.1) |
| `msys2/setup-msys2` | AddressSanitizer matrix | `66cd2cce69caa17b53920067426061ca1de3a884` (v2) |

| Control | Evidence | Status |
| --- | --- | --- |
| All external Actions pinned to immutable commit SHAs | The three external Actions referenced across all workflows are each pinned to a full 40-character SHA with a version comment. | Validated |
| No floating tag or branch references | No workflow references an Action by a mutable tag (`@v4`, `@main`, etc.) without an accompanying SHA. | Validated |

### Dependabot maintenance

| Control | Evidence | Status |
| --- | --- | --- |
| Dependabot configured for GitHub Actions | [`.github/dependabot.yml`](../../.github/dependabot.yml) schedules weekly Monday checks for the `github-actions` ecosystem at `"/"`. | Validated |
| Commit prefix and labels applied | Pull requests carry the `chore(ci)` prefix and the `dependencies` and `ci` labels. | Validated |
| Concurrent PR limit bounded | `open-pull-requests-limit: 5` prevents unbounded queuing of dependency update PRs. | Validated |

### Timeouts

All jobs carry an explicit `timeout-minutes` value, preventing runaway jobs from consuming runner minutes indefinitely.

| Workflow | Timeout (minutes) | Note |
| --- | --- | --- |
| Quality Gate | 20 | Single-job workflow. |
| Linux GCC | 20 | Applied per matrix job. |
| AddressSanitizer | 20 | Single-job workflow. |
| macOS AppleClang | 20 | Applied per matrix job. |
| Static analysis | 20 (Linux), 30 (macOS, Windows) | Longer timeout for macOS/Windows toolchain setup. |
| Windows MSVC | 30 | Accounts for Visual Studio toolchain startup. |
| Coverage | 20 (Linux), 30 (macOS, Windows) | Longer timeout for macOS/Windows coverage tooling. |
| Native builds | 20 (Linux/macOS), 30 (Windows) | Applied per matrix job. |
| AddressSanitizer matrix | 5 (resolve), 30 (validate) | 5-minute matrix resolution job; 30-minute per-platform job. |

| Control | Evidence | Status |
| --- | --- | --- |
| All jobs have explicit `timeout-minutes` | Every job across all nine workflow files declares a `timeout-minutes` field. | Validated |

### Concurrency — cancellation of obsolete runs

All workflows configure a concurrency group and set `cancel-in-progress: true`, so a new push to the same branch supersedes any in-progress run of the same workflow.

| Workflow | Concurrency group key |
| --- | --- |
| Quality Gate | `quality-gate-${{ github.workflow }}-${{ github.ref }}` |
| Linux GCC | `linux-gcc-${{ github.workflow }}-${{ github.ref }}` |
| AddressSanitizer | `asan-${{ github.workflow }}-${{ github.ref }}` |
| macOS AppleClang | `macos-appleclang-${{ github.workflow }}-${{ github.ref }}` |
| Static analysis | `static-analysis-${{ github.workflow }}-${{ github.ref }}` |
| Windows MSVC | `windows-msvc-${{ github.workflow }}-${{ github.ref }}` |
| Coverage | `coverage-${{ github.workflow }}-${{ github.ref }}` |
| Native builds | `native-builds-${{ github.workflow }}-${{ github.ref }}` |
| AddressSanitizer matrix | `asan-matrix-${{ github.ref }}` |

| Control | Evidence | Status |
| --- | --- | --- |
| All workflows cancel obsolete in-progress runs | Every workflow file has a top-level `concurrency` block with `cancel-in-progress: true`. | Validated |

## Governance review summary

The following table records the outcome of the CI governance review conducted for the **mcLab v0.1.0** engineering milestone.

| Item | Status | Notes |
| --- | --- | --- |
| Every workflow has a documented responsibility | ✅ Validated | See [Workflow inventory](#workflow-inventory) above. |
| Every check is classified as required, optional, or advisory | ✅ Validated | See [Check names and ruleset alignment](#check-names-and-ruleset-alignment) above. |
| Required checks run reliably on every PR | ✅ Validated | Nine required checks across six workflows trigger on `pull_request`. |
| Workflow and job permissions follow least privilege | ✅ Validated | All workflows declare `permissions: contents: read`. No elevated scopes. |
| All external Actions are pinned to full SHAs | ✅ Validated | Three Actions pinned to full 40-character commit SHAs. |
| Dependabot maintains Action references | ✅ Validated | Weekly GitHub Actions updates via [`.github/dependabot.yml`](../../.github/dependabot.yml). |
| Timeouts are explicitly configured | ✅ Validated | All jobs carry `timeout-minutes`. |
| Obsolete PR runs are cancelled | ✅ Validated | All workflows carry `concurrency: cancel-in-progress: true`. |
| Ruleset check names documented | ✅ Validated | Exact check names listed in [Required checks](#required-checks-must-pass-before-merge-to-master). Maintainers must confirm these match the names configured in the repository ruleset settings. |
| Failure modes deliberately tested | ⚠️ Maintainer confirmation required | Failure mode testing (deliberately breaking a required check and confirming the PR is blocked) must be confirmed by maintainers out-of-band. |
| CI governance documented and reviewed | ✅ Validated | This document. |

## Follow-up actions

1. **Ruleset alignment confirmation.** A maintainer must open the repository's branch ruleset configuration and verify that the nine required check names listed above exactly match the check names configured as required status checks. Any mismatch or stale entry must be corrected.
2. **Failure mode verification.** A maintainer should deliberately break one required check on a test branch, open a pull request, and confirm that GitHub blocks the merge. The outcome should be recorded as a comment on the related tracking issue.
3. **`native-builds.yml` push scope review.** The workflow currently triggers on every branch push rather than `master` only. Maintainers should confirm this scope is intentional or restrict the push trigger to `master` in a follow-up pull request.
