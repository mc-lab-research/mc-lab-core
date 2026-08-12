# mcLab

**Architectural R&D for a generic Modern C++ executable semantic substrate.**

[![Windows MSVC](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/windows-msvc.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/windows-msvc.yml)
[![Linux GCC](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/linux-gcc.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/linux-gcc.yml)
[![macOS AppleClang](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/macos-appleclang.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/macos-appleclang.yml)
[![Quality Gate](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/quality-gate.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/quality-gate.yml)
[![CodeQL](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/codeql.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/codeql.yml)
[![Coverage](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/coverage.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/coverage.yml)
[![OpenSSF Scorecard](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/scorecard.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/scorecard.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Codacy Badge](https://app.codacy.com/project/badge/Coverage/46c1e2f0645f417c954e885bed9e445c)](https://app.codacy.com/gh/mc-lab-research/mc-lab-core/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_coverage)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/46c1e2f0645f417c954e885bed9e445c)](https://app.codacy.com/gh/mc-lab-research/mc-lab-core/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![CodeFactor](https://www.codefactor.io/repository/github/mc-lab-research/mc-lab-core/badge)](https://www.codefactor.io/repository/github/mc-lab-research/mc-lab-core)

mcLab investigates whether Modern C++ can support reusable computation over
executable semantic structures without binding semantic meaning to one storage
model, class hierarchy, language, or analysis technique.

The current strategic thesis is:

> **mcLab aims to establish a generic Modern C++ computational substrate for
> executable semantic structures.**

This is a research thesis, not an established project claim. The project uses
implementation to produce architectural evidence, including evidence that may
narrow, redesign, or reject an abstraction.

## SemTL research programme

The **Semantic Template Library (SemTL)** is mcLab's central architectural R&D
programme. It investigates a generic-programming architecture organized around:

```text
Semantic structures
    -> semantic operations
    -> semantic laws, where justified
    -> reusable generic algorithms
```

The decisive question is not whether several C++ state types fit one template.
SemTL investigates genericity across two independent dimensions:

- **formalism genericity:** how far an architectural vocabulary remains useful
  as semantic meaning changes;
- **materialization genericity:** how far consumers remain reusable across
  stored, lazy, generated, viewed, composed, external, or runtime
  representations.

Simulation, exploration, composition, conformance checking, evidence
construction, and minimal verification act as architectural probes. Their
current purpose is to test semantic abstractions, not to turn mcLab into another
model checker or simulator.

The current structural vocabulary and its boundaries are described in
[SemTL architectural boundary](docs/semantic/architecture.md). Public contracts
remain experimental and are expected to change as evidence accumulates.

## Evidence-driven development

Architecturally significant questions are governed by the
[SemTL architectural research system](docs/research/README.md):

- the [operational protocol](docs/research/protocol.md) defines the lightweight
  R0-R3 research discipline;
- the [research state and Claims Ledger](docs/research/state.md) record active
  work, queued questions, supported claims, and evidence gaps;
- an Architectural Research Case preserves the chain from competing hypotheses
  through observations, claim impact, and decisions;
- an architectural decision record is created only when evidence changes the
  architecture subsequent work should follow.

The active pilot is
[ARC-001: explicit versus lazy transition materialization](docs/research/cases/ARC-001.md).
It tests whether one transition consumer can operate across stored and genuinely
single-pass lazy witnesses without hidden materialization or semantic loss.

## Architecture

The current semantic dependency boundary is:

```text
Semantic structure -> Algorithm -> Behavior -> Representation
```

Semantic structures describe the system. Algorithms consume the minimum
structure and computational capabilities they require. Executions, runs,
traces, and counterexamples are algorithmic results rather than structural
facets. Containers, ranges, views, generators, and other C++ mechanisms are
representations and do not define semantic roles by themselves.

At repository scale, the dependency direction remains:

```text
Tools -> Extensions -> Core
```

- **Core** provides language-independent semantic and computational
  foundations.
- **Extensions** add concrete languages, models, analyses, or adapters.
- **Tools** assemble these capabilities into workflows.

The Core is headless and independent from graphical interfaces, particular
source languages, and specific AI frameworks.

## Applications and future options

Potential consumers and applications include:

- simulation and execution;
- state-space exploration and verification;
- semantic composition and transformation;
- specification diagnosis, strengthening, and synthesis;
- modeling-language, IR, and external-representation adapters;
- and formal modeling and assurance of autonomous or agentic systems.

These possibilities do not currently define mcLab's primary identity. They are
validation environments or future options whose priority depends on evidence
from the SemTL substrate investigation.

## Engineering

mcLab is developed as a portable C++20 codebase with CMake, CTest, continuous
integration, static analysis, and code coverage.

C++20 is the current engineering baseline. Its adequacy for executable semantic
computing is itself under investigation. C++23 and C++26 mechanisms are
evaluated only when a demonstrated semantic or architectural requirement gives
them a concrete role.

The active CI workflows define the platforms and toolchains currently validated
by the project; see [CI architecture and governance](docs/ci/README.md).

## Status

mcLab is in an early architectural-research phase. The current focus is a small,
understandable, testable semantic core and decision-grade evidence about its
genericity, semantic integrity, composability, extension cost, and C++ language
requirements.

Public APIs and architectural boundaries are not stable. A working library is
an engineering artifact; it does not by itself establish that the SemTL thesis
is valid.

## Contributing

mcLab is not yet open to external contributions while its foundations are being established.
See [CONTRIBUTING.md](CONTRIBUTING.md) for the current contribution status and
the project's future contribution principles.

## License

mcLab is released under the [MIT License](LICENSE).
