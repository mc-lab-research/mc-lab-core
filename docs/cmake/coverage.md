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
- system headers and installed SDK or toolchain sources; and
- GCC exception-only branch arcs identified by the compiler.

The GCC report excludes only compiler-marked exception arcs. For example, C
library output calls can acquire an exception edge in GCOV even though no
product exception path exists to exercise. Ordinary decision branches remain
in the report and must be covered by tests. The CLI tests assert help, version,
and invalid-invocation output in addition to their exit status so that the
measured paths have behavioral evidence.

The authoritative settings are
`MC_LAB_CORE_COVERAGE_SOURCE_PATHS` and
`MC_LAB_CORE_COVERAGE_EXCLUDE_REGEX` in
`cmake/modules/coverage.cmake`. The policy gate also rejects any normalized
report containing a source outside the configured first-party paths.

## Accepted baseline

The accepted baseline is `master` commit
`f8961dc2b0c8445fb3a50ba5f0ff46b902928fe1`. It was measured on 2026-08-03
by the canonical `linux-gcc-coverage` workflow on Ubuntu 24.04, using GCC/gcov
13.3.0 and the same normalized LCOV report and policy gate used by CI:

| Metric | Covered | Total | Baseline |
| --- | ---: | ---: | ---: |
| Lines | 20 | 20 | 100.00% |
| Branches | 14 | 16 | 87.50% |
| Functions | 3 | 3 | 100.00% |

The platform-independent LCOV representation makes metric names, source paths,
and report locations toolchain-neutral. Minor backend differences may occur as
compilers evolve, so the enforced policy uses stable whole-number floors rather
than exact snapshot equality. Linux GCC is the canonical CI baseline job; the
values above are its recorded evidence for the named commit.

This historical measurement predates the current 90% hard floors and was
evaluated against the then-active 80/70/80 policy. It records the initial
baseline only; it does not exempt current or future changes from the stricter
90/90/90 gate.

## Regression policy

The hard repository-wide minimums are:

- line coverage: 90%;
- branch coverage: 90%;
- function coverage: 90%.

Falling below any minimum fails the `Coverage` workflow and is not an
acceptable regression. The observed baseline is evidence, not an additional
100% hard gate: source growth can legitimately add uncovered defensive or
platform-specific paths while the repository remains at or above the 90%
floors.

A decline from the accepted baseline is intentional only when its pull request:

- identifies the newly uncovered behavior and why exercising it is currently
  impractical or disproportionate;
- links the generated canonical Linux GCC summary or artifact;
- adds the strongest practical alternative test evidence; and
- receives explicit approval from a repository maintainer responsible for the
  affected production area and coverage configuration.

After such a change merges, this section must be updated in a dedicated or
clearly identified follow-up to name the new commit, covered/total counts, and
percentages. Normal source growth therefore evolves the baseline through
reviewed evidence rather than silently redefining it. Improvements do not
automatically raise hard floors; thresholds are ratcheted upward only in a
separately approved policy change after the higher level has proved stable.
They must never be lowered merely to make a failing pull request pass.

Changing first-party source paths, exclusion expressions, generated-code
classification, report normalization, or metric calculation changes the
measurement boundary. Such a change requires explicit maintainer approval,
before/after reports, and a rationale in both the pull request and this
document. It must not be bundled as an unexplained fix for a regression.

Coverage percentages are a reachability signal, not evidence that assertions
are meaningful, edge cases are complete, or product behavior is correct.
Reviewers must consider test intent, assertions, negative cases, and relevant
integration evidence independently of the percentages.
