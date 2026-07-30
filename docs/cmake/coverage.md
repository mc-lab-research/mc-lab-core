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

The `Coverage` GitHub Actions workflow runs Linux GCC, Linux Clang, macOS
AppleClang, and Windows clang-cl for every pull request and every push to
`master`. Each job publishes its complete `coverage` directory for 14 days
under a distinct artifact name:

- `coverage-linux-gcc`;
- `coverage-linux-clang`;
- `coverage-macos-appleclang`;
- `coverage-windows-clangcl`.

An artifact step also runs after a failed quality gate when that job
successfully generated `coverage.info`, so a platform-specific regression can
be investigated.

Linux GCC is the canonical CI measurement and baseline. The Linux Clang,
Windows clang-cl, and macOS AppleClang jobs continuously validate portability
and provide GitHub-hosted diagnostics for their respective contributor
environments.

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
