# CI governance baseline

This document records the CI governance baseline reviewed for **mcLab** and the evidence used to validate each control.
It covers the tracked workflow inventory, related GitHub-managed automation, trigger conditions, governance controls, and the required check names intended for the main branch ruleset.

Reviewed commit: `5537a78`.
Validated on **2026-08-04**.

## Workflow inventory

The repository currently tracks **11** GitHub Actions workflow files under `.github/workflows/`.
These are distinct from GitHub-managed services such as Dependabot, which influence CI governance but are not tracked workflow YAML files.

### Tracked repository workflows

| Workflow | File | Responsibility | Classification |
| --- | --- | --- | --- |
| Quality Gate | [`quality-gate.yml`](../../.github/workflows/quality-gate.yml) | Enforces the Linux / GCC commit gate preset — build, test, and formatting verification in one step. This is the primary blocking gate for all merges to `master`. | **Required** |
| Linux GCC | [`linux-gcc.yml`](../../.github/workflows/linux-gcc.yml) | Validates Debug and Release builds on Ubuntu 24.04 with GCC. | **Required** |
| AddressSanitizer | [`asan.yml`](../../.github/workflows/asan.yml) | Runs the canonical Linux / GCC AddressSanitizer build and test suite to catch memory safety regressions. | **Required** |
| macOS AppleClang | [`macos-appleclang.yml`](../../.github/workflows/macos-appleclang.yml) | Validates Debug and Release builds on macOS 15 with AppleClang to ensure cross-platform portability. | **Required** |
| Static analysis | [`static-analysis.yml`](../../.github/workflows/static-analysis.yml) | Runs `clang-tidy` static analysis on Linux (Clang), macOS (AppleClang), and Windows (clang-cl). | **Required** (canonical Linux / Clang job); **Optional** (macOS and Windows jobs) |
| Windows MSVC | [`windows-msvc.yml`](../../.github/workflows/windows-msvc.yml) | Validates Debug and Release builds on Windows 2022 with Visual Studio 2022 / MSVC. | **Required** |
| Coverage | [`coverage.yml`](../../.github/workflows/coverage.yml) | Measures and enforces code coverage thresholds across Linux GCC, Linux Clang, macOS AppleClang, and Windows clang-cl. Publishes HTML coverage reports as workflow artifacts and uploads the canonical Linux/GCC LCOV report to Codacy on pushes to `master`. | **Advisory** |
| Native builds | [`native-builds.yml`](../../.github/workflows/native-builds.yml) | Runs a broader build and test matrix — Ubuntu GCC, Ubuntu Clang, macOS AppleClang, and Windows Visual Studio — on every branch push and pull request. Partially overlaps with the individual required workflows but provides additional toolchain and OS coverage. | **Advisory** |
| AddressSanitizer matrix | [`asan-matrix.yml`](../../.github/workflows/asan-matrix.yml) | On-demand, selectable multi-platform AddressSanitizer validation covering Linux GCC, Linux Clang, macOS AppleClang, Windows MSVC, Windows Visual Studio 2022, Windows Visual Studio 2026, Windows clang-cl, and Windows MSYS2 CLANG64. Triggered manually via `workflow_dispatch`. | **Manual** |
| Repository visualization | [`repository-visualizer.yml`](../../.github/workflows/repository-visualizer.yml) | Generates a repository structure SVG on a weekly schedule or manual dispatch and publishes it as an artifact without pushing changes back to the repository. | **Advisory** |
| OpenSSF Scorecard | [`scorecard.yml`](../../.github/workflows/scorecard.yml) | Runs the OpenSSF Scorecard security governance scan on pushes to `master`, weekly schedule, or manual dispatch, then uploads SARIF to GitHub Security. | **Advisory** |

### GitHub-managed automation related to CI governance

| Automation | Source | Scope | Classification | Evidence |
| --- | --- | --- | --- | --- |
| Dependabot version updates | [`.github/dependabot.yml`](../../.github/dependabot.yml) interpreted by GitHub | Weekly GitHub Actions dependency update checks for workflow action references under `.github/workflows/` | **GitHub-managed advisory automation** | Configuration file plus closed follow-up issue [#52](https://github.com/mc-lab-research/mc-lab-core/issues/52), which records GitHub acceptance and a successful update-check validation |

### Classification definitions

| Classification | Meaning |
| --- | --- |
| **Required** | The check is intended to be configured as a required status check in the main branch ruleset. A pull request cannot be merged to `master` until this check passes. |
| **Optional** | The workflow runs on every pull request but is not a required status check. Failure is visible but does not block merge. |
| **Advisory** | The workflow provides additional confidence or scheduled governance validation but is not part of the pull request critical path. |
| **Manual** | The workflow is available on demand and is not expected to run automatically on the merge path. |

## Trigger matrix

| Workflow | `pull_request` | `push` to `master` | `push` (all branches) | `schedule` | `workflow_dispatch` |
| --- | :---: | :---: | :---: | :---: | :---: |
| Quality Gate | ✅ | ✅ | — | — | ✅ |
| Linux GCC | ✅ | ✅ | — | — | ✅ |
| AddressSanitizer | ✅ | ✅ | — | — | ✅ |
| macOS AppleClang | ✅ | ✅ | — | — | ✅ |
| Static analysis | ✅ | ✅ | — | — | ✅ |
| Windows MSVC | ✅ | ✅ | — | — | ✅ |
| Coverage | ✅ | ✅ | — | — | ✅ |
| Native builds | ✅ | — | ✅ | — | ✅ |
| AddressSanitizer matrix | — | — | — | — | ✅ |
| Repository visualization | — | — | — | ✅ | ✅ |
| OpenSSF Scorecard | — | ✅ | — | ✅ | ✅ |

> **Note — Native builds push scope.** `native-builds.yml` triggers on every push to any branch, not only `master`.
> This is intentional: the broader matrix provides early cross-toolchain feedback on feature branches.
> Because its check names differ from the individual required workflows, it does not interfere with ruleset enforcement.

## Check names and ruleset alignment

The following check names are emitted by GitHub Actions job `name:` fields and are the intended required status checks for the `master` branch ruleset.
Issue [#53](https://github.com/mc-lab-research/mc-lab-core/issues/53) tracks live ruleset verification and evidence capture; until that closes, the table below should be treated as the repository-declared target set rather than a completed live-settings audit.

### Intended required checks (merge gate for `master`)

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

### Advisory checks (broader validation, scheduled governance, or non-merge-path evidence)

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
| `Generate repository visualization` | Repository visualization | Scheduled/manual artifact generation only. |
| `Scorecard analysis` | OpenSSF Scorecard | Scheduled/manual or post-merge governance evidence; SARIF uploaded to GitHub Security. |

## Governance controls

### Permissions — least privilege with justified exceptions

| Control | Evidence | Status |
| --- | --- | --- |
| Repository build/test workflows default to read-only repository access | Every tracked workflow except `scorecard.yml` declares top-level `permissions: contents: read`. | Validated |
| Scorecard permission elevation is documented and scoped | `scorecard.yml` declares top-level `permissions: read-all`, then limits the `analysis` job to `contents: read`, `actions: read`, `security-events: write`, and `id-token: write` for Scorecard metadata access, OIDC-backed behavior, and SARIF upload. | Validated |
| No other workflow elevates write permissions | No workflow besides Scorecard requests write access to GitHub resources. `repository-visualizer.yml` also disables credential persistence in `actions/checkout`. | Validated |

### External action pinning — immutable SHAs

All third-party GitHub Actions references are pinned to full 40-character commit SHAs.
Version tags appear as comments to assist human readability and Dependabot updates.

| Action reference | Workflow(s) | SHA |
| --- | --- | --- |
| `actions/checkout` | Quality Gate, Linux GCC, AddressSanitizer, macOS AppleClang, Static analysis, Windows MSVC, Coverage, Native builds, AddressSanitizer matrix, Repository visualization, OpenSSF Scorecard | `3d3c42e5aac5ba805825da76410c181273ba90b1` (v7.0.1) |
| `actions/upload-artifact` | Coverage, Repository visualization | `043fb46d1a93c77aae656e7c1c64a875d1fc6a0a` (v7.0.1) |
| `msys2/setup-msys2` | AddressSanitizer matrix | `66cd2cce69caa17b53920067426061ca1de3a884` (v2) |
| `githubocto/repo-visualizer` | Repository visualization | `a999615bdab757559bf94bda1fe6eef232765f85` (v0.9.1) |
| `ossf/scorecard-action` | OpenSSF Scorecard | `f49aabe0b5af0936a0987cfb85d86b75731b0186` (v2.4.1) |
| `github/codeql-action/upload-sarif` | OpenSSF Scorecard | `60168efe1c415ce0f5521ea06d5c2062adbeed1b` (v3.28.17) |

| Control | Evidence | Status |
| --- | --- | --- |
| All external Actions pinned to immutable commit SHAs | Every external action reference across the 11 tracked workflow files is pinned to a full 40-character SHA with a version comment. | Validated |
| No floating tag or branch references | No tracked workflow references an Action by a mutable tag (`@v4`, `@main`, etc.) without an accompanying SHA. | Validated |

### Dependabot maintenance

| Control | Evidence | Status |
| --- | --- | --- |
| Dependabot is configured to monitor GitHub Actions references | [`.github/dependabot.yml`](../../.github/dependabot.yml) declares `package-ecosystem: github-actions` at directory `"/"` with a weekly Monday 06:00 Europe/Paris schedule. | Validated |
| GitHub accepted the configuration and completed an update-check validation | Closed follow-up issue [#52](https://github.com/mc-lab-research/mc-lab-core/issues/52) records acceptance, workflow coverage, label verification, and successful service-side validation. | Validated |
| Dependabot metadata policy is bounded and documented | The configuration sets commit prefix `chore(ci)`, labels `dependencies` and `ci`, and `open-pull-requests-limit: 5`. | Validated |

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
| Repository visualization | 10 | Single-job scheduled/manual workflow. |
| OpenSSF Scorecard | 20 | Single-job scheduled/manual or post-merge workflow. |

| Control | Evidence | Status |
| --- | --- | --- |
| All jobs have explicit `timeout-minutes` | Every job across all 11 tracked workflow files declares `timeout-minutes`. | Validated |

### Concurrency — cancellation of obsolete runs where appropriate

Branch-scoped workflows generally configure concurrency and cancel obsolete in-progress runs, but scheduled governance workflows are more selective.

| Workflow | Concurrency group key | `cancel-in-progress` |
| --- | --- | --- |
| Quality Gate | `quality-gate-${{ github.workflow }}-${{ github.ref }}` | `true` |
| Linux GCC | `linux-gcc-${{ github.workflow }}-${{ github.ref }}` | `true` |
| AddressSanitizer | `asan-${{ github.workflow }}-${{ github.ref }}` | `true` |
| macOS AppleClang | `macos-appleclang-${{ github.workflow }}-${{ github.ref }}` | `true` |
| Static analysis | `static-analysis-${{ github.workflow }}-${{ github.ref }}` | `true` |
| Windows MSVC | `windows-msvc-${{ github.workflow }}-${{ github.ref }}` | `true` |
| Coverage | `coverage-${{ github.workflow }}-${{ github.ref }}` | `true` |
| Native builds | `native-builds-${{ github.workflow }}-${{ github.ref }}` | `true` |
| AddressSanitizer matrix | `asan-matrix-${{ github.ref }}` | `true` |
| Repository visualization | `repository-visualization-${{ github.workflow }}` | `false` |
| OpenSSF Scorecard | _none_ | _not declared_ |

| Control | Evidence | Status |
| --- | --- | --- |
| Branch-scoped build/test workflows cancel obsolete in-progress runs | All branch-scoped tracked workflows except Scorecard use top-level `concurrency` with `cancel-in-progress: true`. | Validated |
| Scheduled governance workflows intentionally differ | `repository-visualizer.yml` serializes runs without cancellation, and `scorecard.yml` does not declare an explicit `concurrency` block. | Validated |

## Governance review summary

The following table records the outcome of the CI governance review conducted for the **mcLab v0.1.0** engineering milestone.

| Item | Status | Notes |
| --- | --- | --- |
| Every tracked workflow has a documented responsibility | ✅ Validated | See [Tracked repository workflows](#tracked-repository-workflows) above. |
| GitHub-managed automation is distinguished from tracked workflow files | ✅ Validated | Dependabot is recorded separately from `.github/workflows/*.yml`. |
| Every check is classified as required, optional, advisory, or manual | ✅ Validated | See [Check names and ruleset alignment](#check-names-and-ruleset-alignment) above. |
| Required workflows are configured to run on every PR | ✅ Validated | Nine intended required checks across six workflows trigger on `pull_request`. |
| Workflow and job permissions follow least privilege with justified exceptions | ✅ Validated | Scorecard is the documented exception; other tracked workflows stay read-only. |
| All external Actions are pinned to full SHAs | ✅ Validated | Six unique external actions are pinned to full 40-character commit SHAs. |
| Dependabot evidence uses service-side validation | ✅ Validated | Follow-up issue #52 records GitHub acceptance and successful update-check validation. |
| Timeouts are explicitly configured | ✅ Validated | All jobs carry `timeout-minutes`. |
| Concurrency claims match actual YAML | ✅ Validated | Repository visualization and Scorecard exceptions are explicitly documented. |
| Intended ruleset check names are documented | ✅ Validated | Exact target check names are listed in [Intended required checks](#intended-required-checks-merge-gate-for-master). |
| Live ruleset evidence is fully closed | ⚠️ Pending follow-up | Issue #53 remains open; maintainers still need to confirm the live `master` ruleset matches the intended set and capture blocking evidence. |
| CI governance documented and reviewed | ✅ Validated | This document, reconciled with [`docs/ci/README.md`](README.md). |

## Follow-up actions

1. **Ruleset alignment confirmation.** Complete issue [#53](https://github.com/mc-lab-research/mc-lab-core/issues/53) by capturing the live `master` ruleset, confirming the nine intended required checks match exactly, and recording the merge-blocking evidence.
2. **Failure mode verification.** A maintainer should deliberately break one required check on a test branch, open a pull request, and confirm that GitHub blocks the merge. The outcome should be recorded on issue #53.
3. **Downstream documentation cleanup.** Other security/reference documents that still describe pending Scorecard or broader Dependabot/security state should be reconciled against the latest accepted evidence in follow-up documentation work.
