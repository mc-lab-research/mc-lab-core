# SemTL architectural research

This directory contains the Git-native **Lean Research OS** used to operate
SemTL as architectural R&D. It preserves falsification and evidence discipline
without turning the research model into a large administration system.

The governing question is whether mcLab can establish a principled Modern C++
generic-programming architecture for executable semantic structures, within a
clearly demonstrated domain and at acceptable architectural and language cost.

Governance is **human-owned and AI-advised**. Named humans own Research Cases,
experiments, freezes, challenges, evidence assessments, Claims, and decisions.
AI systems may propose, analyze, draft, implement, or execute under human
direction, but they are never owners, responsible parties, approvers,
adjudicators, or independent Challengers.

## Start here

- [Operational protocol](protocol.md) defines how R2 and R3 architectural
  questions are investigated.
- [Research state](state.md) records current work in progress, the queued
  question backlog, and the Claims Ledger.
- [ARC-001](cases/ARC-001.md) is the first proposed controlled research case:
  explicit versus lazy transition materialization.
- [ARC-002](cases/ARC-002.md) is a queued, non-active design draft concerning
  durable transition evidence and deferred execution replay. It cannot activate
  until ARC-001 closes.
- [Research Case template](templates/research-case.md) is the copyable starting
  point for later R2 and R3 investigations.

## Minimum persistent artifact set

The research system intentionally has only a small authoritative chain:

1. the Strategic Baseline and [`protocol.md`](protocol.md);
2. [`state.md`](state.md) for compact programme state, backlog, evidence, and
   Claims;
3. one `cases/ARC-###.md` file for each significant R2 or R3 investigation;
4. experiments, PRs, tests, diagnostics, and commits as evidence;
5. an evidence-aware ADR only when an experiment establishes or changes
   governing architecture.

Questions, hypotheses, properties, models, materializations, consumers, laws,
language notes, hold-outs, freezes, and evidence remain conceptually distinct.
They normally live inside `state.md` or the ARC that investigates them rather
than in separately synchronized registries.

Derived views such as a Genericity Matrix may be assembled from the ARC when
useful. They are not independent sources of truth, quantitative coverage
scores, or reasons to maximize tested cells.

GitHub is an operational interface for where work is. `state.md` and the ARC
remain authoritative for what is known. Do not duplicate epistemic state across
Project fields, labels, milestones, and documents without a demonstrated need.

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
