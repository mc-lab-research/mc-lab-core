# Code coverage

MC-LAB-CORE measures first-party production coverage with an isolated Debug
build. The same CMake workflow is used locally and in GitHub Actions so the
quality gate can be reproduced without CI-specific commands.

## Supported workflows

Run one of the coverage workflow presets from an activated toolchain
environment:

```text
cmake --workflow --preset windows-clangcl-coverage --fresh
cmake --workflow --preset linux-clang-coverage --fresh
cmake --workflow --preset linux-gcc-coverage --fresh
cmake --workflow --preset macos-appleclang-coverage --fresh
```

Each workflow configures and builds the instrumented targets, removes stale
runtime data, runs the test suite, generates the reports, and enforces the
repository thresholds.

On macOS, the Apple command-line developer tools, CMake, and Ninja are
required. The coverage module uses `xcrun` to locate the `llvm-profdata` and
`llvm-cov` binaries belonging to the selected Xcode toolchain.

The canonical outputs are written below the preset build directory:

- `coverage/coverage.info`: normalized LCOV data;
- `coverage/coverage-summary.txt`: human-readable summary;
- `coverage/html/index.html`: browsable source report.

The `Coverage` GitHub Actions workflow runs the Linux Clang preset for every
pull request and every push to `master`. It publishes the complete `coverage`
directory as the `coverage-report` artifact for 14 days. The artifact step also
runs after a failed quality gate when a report was successfully generated, so a
regression can be investigated.

Linux Clang is the canonical CI measurement. The Windows clang-cl and macOS
AppleClang workflows provide equivalent local platform coverage without adding
duplicate operating-system jobs to the required CI gate.

## Scope and exclusions

Coverage is target-scoped. A first-party target participates only when its
owning `CMakeLists.txt` calls `mc_lab_core_enable_coverage()`.

The report includes production sources under these repository-relative paths:

- `include`;
- `src`;
- `tools`.

Only paths that exist are selected. Tests exercise production code but test
sources are not part of the measured product surface.

Reports exclude:

- test sources;
- third-party, external, vendor, and dependency sources;
- CMake-generated files and compiler support files;
- system headers and installed SDK or toolchain sources.

The authoritative settings are
`MC_LAB_CORE_COVERAGE_SOURCE_PATHS` and
`MC_LAB_CORE_COVERAGE_EXCLUDE_REGEX` in
`cmake/modules/coverage.cmake`. The policy gate also rejects any normalized
report containing a source outside the configured first-party paths.

## Initial baseline

The initial baseline was established by this quality-evidence change, based on
`master` at commit `835e4fd`, with the `windows-clangcl-coverage` workflow and
LLVM 22.1.8:

| Metric | Covered | Total | Baseline |
| --- | ---: | ---: | ---: |
| Lines | 33 | 33 | 100.00% |
| Branches | 10 | 10 | 100.00% |
| Functions | 3 | 3 | 100.00% |

The platform-independent LCOV report is canonical. Minor backend differences
may occur as compilers evolve, so the enforced policy uses stable whole-number
floors rather than exact snapshot equality.

## Regression policy

Every pull request must meet all repository-wide minimums:

- line coverage: 80%;
- branch coverage: 70%;
- function coverage: 80%.

Falling below any minimum fails the `Coverage` workflow. New production
behavior should normally arrive with tests, and changes that intentionally
reduce coverage must explain why and update both the policy and this document
in the same review.

Thresholds are ratcheted upward deliberately after sustained coverage gains.
They must not be lowered merely to make a failing pull request pass.
