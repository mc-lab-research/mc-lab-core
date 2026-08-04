# mcLab

**A portable C++20 platform for executable semantics and formal verification of AI agents.**

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/4bc2b7e139cd428da402cffb07a3c13e)](https://app.codacy.com/gh/mc-lab-research/mc-lab-core?utm_source=github.com&utm_medium=referral&utm_content=mc-lab-research/mc-lab-core&utm_campaign=Badge_Grade)
[![Windows MSVC](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/windows-msvc.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/windows-msvc.yml)
[![Linux GCC](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/linux-gcc.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/linux-gcc.yml)
[![macOS AppleClang](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/macos-appleclang.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/macos-appleclang.yml)
[![Quality Gate](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/quality-gate.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/quality-gate.yml)
[![CodeQL](https://github.com/mc-lab-research/mc-lab-core/workflows/CodeQL/badge.svg)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/323700157)
[![Coverage](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/coverage.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/coverage.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Coverage/46c1e2f0645f417c954e885bed9e445c)](https://app.codacy.com/gh/mc-lab-research/mc-lab-core/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_coverage)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/46c1e2f0645f417c954e885bed9e445c)](https://app.codacy.com/gh/mc-lab-research/mc-lab-core/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![CodeFactor](https://www.codefactor.io/repository/github/mc-lab-research/mc-lab-core/badge)](https://www.codefactor.io/repository/github/mc-lab-research/mc-lab-core)

mcLab explores software foundations for defining, executing, and analyzing formal semantics.

The project is modular and does not assume a single language, semantic model, or verification technique.

## Research direction

One research direction is the formal modeling and verification of AI agents.

This includes studying how agent state, observations, decisions, actions, tool interactions, and traces may be represented using explicit semantic models.

mcLab also investigates a **Semantic Template Library**: reusable semantic structures for recurring system and agent concepts.

**Source or agent model → Semantic templates → Semantic IR → Execution → Analysis**

This direction is exploratory. Its abstractions, interfaces, and terminology are expected to evolve.

## Architecture

**Tools → Extensions → Core**

- **Core** provides language-independent semantic and execution foundations.
- **Extensions** add concrete languages, models, analyses, or adapters.
- **Tools** assemble these capabilities into workflows.

The Core is headless and independent from graphical interfaces and specific AI frameworks.

## Engineering

mcLab is developed as a portable C++20 codebase with CMake, CTest, continuous integration, static analysis, and code coverage.

The active CI workflows define the platforms and toolchains currently validated by the project; see [CI architecture and governance](docs/ci/README.md).

## Status

mcLab is in an early foundational phase.

The current focus is a small, understandable, and testable core. Public APIs and architectural boundaries are not yet stable.

## Contributing

mcLab is not yet open to external contributions while its foundations are being established.
See [CONTRIBUTING.md](CONTRIBUTING.md) for the current contribution status and the projectâ€™s future contribution principles.

## License

mcLab is released under the [MIT License](LICENSE).
