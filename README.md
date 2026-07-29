# mcLab

**A portable C++20 platform for executable semantics and formal verification research.**

[![Coverage](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/coverage.yml/badge.svg?branch=master)](https://github.com/mc-lab-research/mc-lab-core/actions/workflows/coverage.yml)

mcLab brings high-quality software engineering infrastructure to the definition, execution, exploration, and checking of formal semantics.

It is being developed as an open foundation for researchers, engineers, educators, and students who want to build semantic models and verification tools without starting from a monolithic model checker.

## Vision

mcLab is not another fixed model checker.

It is a modular platform in which languages, semantic models, analysis domains, exploration strategies, properties, and verification procedures can evolve independently.

The long-term semantic pipeline is:

**Source language → Semantic IR → Executable semantics → Exploration → Verification**

mcLab aims to support several semantic families, including:

- explicit state-transition systems;
- featured and variability-aware systems;
- weighted systems;
- stochastic systems;
- timed systems;
- continuous and hybrid systems.

Variability is treated as a transversal capability that may be combined with other semantic families, including featured timed automata, featured Markov decision processes, and featured hybrid automata.

## Engineering principles

mcLab is being developed as a portable, high-quality C++20 codebase.

The repository provides or is establishing:

- Windows, Linux, and macOS build configurations;
- MSVC, GCC, Clang, ClangCL, and AppleClang toolchain configurations;
- target-based CMake;
- automated testing with CTest;
- CI-ready build and quality workflows;
- compiler warning policies;
- sanitizers;
- static analysis;
- [code coverage](docs/cmake/coverage.md);
- reproducible builds;
- explicit architectural boundaries.

The active CI matrix will define the operating-system and compiler combinations officially validated by the project.

The Core is headless and independent from graphical interfaces.

## Architecture

mcLab follows a strict dependency direction:

**Tools → Extensions → Core**

### Core

The Core provides language-independent foundations for semantic execution, exploration, observations, traces, diagnostics, and reusable analysis contracts.

### Extensions

Extensions add concrete capabilities such as:

- languages and frontends;
- semantic domains;
- semantic facets and profiles;
- exploration strategies;
- checkers;
- variability-aware analyses;
- visualizations.

### Tools

Tools assemble the platform for concrete workflows.

The repository currently contains an initial command-line bootstrap. The planned tool layer includes:

- the `mc-lab` command-line interface;
- mcLab Studio, an interactive workbench for semantic development, execution, visualization, and verification.

## Research and education

mcLab aims to connect high-level formal-methods research with high-level software engineering.

The platform is intended to support:

- executable research artifacts;
- semantic experimentation;
- reusable verification algorithms;
- comparison of semantic variants;
- variability-aware analysis;
- reproducible studies;
- formal-methods teaching;
- tutorials and student projects.

## Current status

mcLab is in its foundational phase.

The first milestones aim to establish a minimal, portable architecture for:

- semantic models;
- state-transition execution;
- bounded exploration;
- verification results;
- tests and quality infrastructure.

Advanced semantic families, family-based analyses, Studio features, and AI-assisted semantic reimplementation will be introduced incrementally.

## Contributing

mcLab is not yet open to external contributions while its foundational architecture and public contracts are being established.

See [CONTRIBUTING.md](CONTRIBUTING.md) for the current contribution status and the project’s future contribution principles.

## License

mcLab is released under the [MIT License](LICENSE).
