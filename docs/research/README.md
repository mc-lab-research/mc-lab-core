# SemTL architectural research

This directory contains the Git-native research-governance system used to
operate SemTL as architectural R&D.

The governing question is whether mcLab can establish a principled Modern C++
generic-programming architecture for executable semantic structures, within a
clearly demonstrated domain and at acceptable architectural and language cost.

## Start here

- [Operational protocol](protocol.md) defines how R2 and R3 architectural
  questions are investigated.
- [Research state](state.md) records current work in progress, the queued
  question backlog, and the Claims Ledger.
- [ARC-001](cases/ARC-001.md) is the first proposed controlled research case:
  explicit versus lazy transition materialization.
- [Research Case template](templates/research-case.md) is the copyable starting
  point for later R2 and R3 investigations.

## Authoritative living documents

The research system intentionally has only three authoritative living document
types:

1. `state.md` for programme-level WIP, backlog, and claims;
2. one `cases/ARC-###.md` file for each R2 or R3 investigation;
3. an evidence-aware ADR only when an experiment changes governing
   architecture.

Models, materializations, consumers, laws, language notes, hold-outs, and
evidence remain conceptually distinct. They normally live inside the Research
Case that investigates them rather than in separately synchronized registries.

Derived views such as a Genericity Matrix, property evidence map, subject
catalog, or language summary may be generated from case metadata when useful.
They are not independent sources of truth.

## Repository relationship

The current semantic design is documented under [`docs/semantic/`](../semantic/).
Those documents describe governing architecture and semantic contracts. This
directory records how research questions are challenged, what evidence exists,
what mcLab may currently claim, and why an architectural decision changes.

Code, tests, compiler output, and CI results are experiment evidence. A Research
Case links to them and records the distinction between:

```text
Observation -> Interpretation -> Claim impact -> Decision
```

Implementation alone is not a strategic conclusion.
