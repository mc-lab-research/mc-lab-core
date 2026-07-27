# Contributing to mcLab

Thank you for your interest in mcLab.

mcLab is being developed as a portable C++20 platform for executable semantics, behavioral exploration, and formal verification research.

## Current status

> **mcLab is not yet open to external code contributions.**

The project is still establishing its foundational architecture, public terminology, build policies, and first semantic contracts. These elements are expected to change while the initial vertical implementation is completed.

For now, please do not submit unsolicited pull requests, large implementation proposals, architectural refactorings, or new extension frameworks.

This is a temporary restriction. It avoids asking contributors to work against unstable foundations or reviewing changes whose architectural context is still moving.

## What is welcome today

Constructive interest in the project is appreciated.

Where the corresponding repository channels are available, the following are welcome:

- questions about the project vision;
- reports of factual documentation errors;
- feedback on clarity and accessibility;
- references to relevant research or tools;
- expressions of interest in future contribution.

Please distinguish feedback from an implementation proposal. External code changes are not being accepted at this stage.

## When contributions will open

mcLab intends to welcome external contributions after the project has:

- a stable initial package and dependency structure;
- documented public contracts and terminology;
- a reliable multi-platform build;
- an established test and quality workflow;
- a first end-to-end executable-semantics example;
- clear issue, review, and contribution processes.

This document will be updated when the repository is ready to accept pull requests.

## Project direction

mcLab is not intended to become another monolithic model checker.

It is being designed as a modular platform in which languages, semantic models, analysis domains, exploration strategies, properties, verification procedures, and tools can evolve independently.

The intended architectural dependency direction is:

**Tools → Extensions → Core**

The Core remains language-independent and headless. Concrete languages, checkers, graphical interfaces, and project-specific analyses belong in extensions or tools.

The broader project vision is described in [README.md](README.md).

## Future contribution principles

Once external contributions are accepted, changes will be expected to preserve the following principles:

- keep the Core language-independent and headless;
- preserve the dependency direction from tools to extensions to Core;
- justify abstractions with concrete semantic, algorithmic, or tooling needs;
- keep public APIs portable, focused, and testable;
- accompany behavioral changes with appropriate tests;
- validate semantic and algebraic laws through conformance tests;
- distinguish implemented behavior from planned capabilities;
- keep the normal build simple and reproducible;
- maintain both scientific clarity and software-engineering quality.

Small, focused contributions will be preferred over broad speculative frameworks.

## Future contribution areas

Expected contribution areas include:

- reusable Core algorithms and contracts;
- language frontends;
- semantic domains, facets, and profiles;
- exploration strategies;
- verification procedures;
- variability-aware analyses;
- command-line and Studio tooling;
- portability, CI, testing, and static analysis;
- documentation and tutorials;
- educational examples;
- reproducible research artifacts.

These areas describe the long-term scope of the project. They are not yet open work items.
