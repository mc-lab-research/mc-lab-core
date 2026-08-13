---
case: ARC-001
report_kind: pre-freeze-calibration
date: 2026-08-13
status: PASS
evidence_posture: EXPLORATORY
controlled_experiment_started: false
baseline_commit: 16da2757aca384d715347c9839d8a73113540266
freeze_commit: f969fa8aa63a9a172e6feb2eeb32636aab24f387
working_branch: research/arc-001-calibration
---

# ARC-001 pre-freeze calibration report

## Executive summary

The stored calibration subject, `SUBJECT-EXPLICIT-LTS-01`, satisfies the
current transition-relation and transition-labelling contracts. The proposed
`CON-SUCCESSORS-01` consumer observes its complete labelled transition relation
in one pass and agrees with the order-independent semantic oracle for a
branching state, a single-successor state, and a terminal state.

This is a **pre-freeze calibration result only**. It demonstrates that the
stored subject, consumer, and oracle form a viable freeze candidate. It does
not test generated ephemeral witnesses, support or contradict H-A or H-B,
change CL-001, authorize a freeze, or begin the controlled experiment.

## Purpose and boundary

This calibration performs ARC-001 procedure steps 1 through 4:

1. define a finite labelled-LTS oracle;
2. provide the stored calibration subject;
3. implement the proposed single-pass consumer;
4. validate that consumer and oracle on the stored subject.

The following controlled-experiment activities were deliberately excluded:

- freezing the contracts or recording a freeze commit;
- implementing `SUBJECT-LAZY-LTS-01`;
- comparing stable and ephemeral witness representations;
- changing the public concepts or customization-point objects;
- interpreting the result in favor of either competing hypothesis;
- updating the Claims Ledger or making an architectural decision.

## Calibration artifacts

### CON-SUCCESSORS-01

The proposed consumer is defined in
[`labelled_successor_consumer.hpp`](../../../tests/research/arc-001/common/support/labelled_successor_consumer.hpp).
For a supplied system and source state it:

- obtains the local range through `outgoing_transitions(system, source)`;
- performs one range-for pass;
- observes `transition_label(system, witness)` and
  `target(system, witness)` while the current witness is valid;
- copies only the label and target into an observation result;
- does not retain the witness, inspect its address, query the range size,
  perform a second pass, or branch on the model representation.

The observation vector belongs to the consumer-side test evidence. It is not a
transition collection stored or cached by the semantic model.

### SUBJECT-EXPLICIT-LTS-01

The stored calibration model is defined in
[`transition_materialization_models.hpp`](../../../tests/research/arc-001/common/support/transition_materialization_models.hpp).
It stores transition records in fixed arrays and exposes them through borrowed
`std::span<const stored_transition>` ranges. Its iteration witness is
`const stored_transition&`, which is stable for the lifetime of the model.

### Reference labelled transition relation

The proposed finite oracle uses the state domain `{0, 1, 2}` and the relation:

| Source | Label | Target |
|---:|---|---:|
| 0 | `advance` | 1 |
| 0 | `bypass` | 2 |
| 1 | `reset` | 0 |

State `2` has no outgoing transition. The three queried states deliberately
exercise branching, singleton, and empty local images.

Enumeration order is not assigned semantic meaning. The oracle sorts the
copied observations before comparison and therefore compares the represented
labelled relation rather than container order.

### Calibration test

[`transition_materialization_calibration_test.cpp`](../../../tests/research/arc-001/common/transition_materialization_calibration_test.cpp)
contains the compile-time contract checks and runtime oracle comparison. The
test is registered as
`mc_lab_core.semantic.transition_materialization_calibration` with the labels
`semantic`, `research`, `arc-001`, and `calibration`.

## Frozen-contract candidate

The following items are candidates for human review and a later freeze; this
report does not freeze them:

- the three-state labelled transition relation above;
- the complete source of `CON-SUCCESSORS-01`;
- the order-independent oracle comparison;
- the requirement that the consumer makes one pass and uses a witness only
  during its current iteration step;
- the explicit non-requirements already stated in ARC-001;
- the existing public CPO vocabulary:
  `outgoing_transitions`, `transition_label`, and `target`.

## Environment

| Item | Recorded value |
|---|---|
| Operating environment | Windows AMD64 |
| Language mode | C++20, compiler extensions disabled |
| Compiler | clang-cl 22.1.8 |
| CMake | 4.4.0 |
| Ninja | 1.13.2 |
| Baseline commit | `16da2757aca384d715347c9839d8a73113540266` |
| Working branch | `research/arc-001-calibration` |
| Release preset | `windows-clangcl-release` |
| Static-analysis preset | `windows-clangcl-tidy` |

The baseline commit identifies the governed repository context from which the
calibration began. The accepted calibration artifacts and prospective challenge
resolutions were committed as
`f969fa8aa63a9a172e6feb2eeb32636aab24f387`, which is the recorded ARC-001
freeze boundary and is not a controlled result commit.

## Execution and results

### Commit gate

The repository commit-gate workflow was executed with:

```powershell
cmake --workflow --preset windows-clangcl-commit-gate --fresh
```

Result:

- configuration completed successfully;
- all 31 first-party C/C++ files passed the formatting check;
- the warnings-as-errors release build completed successfully;
- all 12 registered tests passed;
- `mc_lab_core.semantic.transition_materialization_calibration` passed.

### Static analysis

The static-analysis workflow was executed with:

```powershell
cmake --workflow --preset windows-clangcl-tidy --fresh
```

Final result: clang-tidy passed for all 6 registered translation units.

### Semantic observation

For each queried source, `CON-SUCCESSORS-01` produced the oracle relation:

| Source | Observed normalized labelled targets | Result |
|---:|---|---|
| 0 | `{(advance, 1), (bypass, 2)}` | PASS |
| 1 | `{(reset, 0)}` | PASS |
| 2 | `{}` | PASS |

No transition label or occurrence was omitted or duplicated in the stored
calibration subject.

## Non-discriminating validation issue

The first static-analysis run reported that allocation from the consumer's
observation vector could escape `main` as an exception. The test harness was
changed to catch unexpected exceptions and return a failing test status. The
static-analysis workflow then passed.

This was a test-harness robustness issue during pre-freeze calibration. It did
not involve the ephemeral subject, a witness-lifetime assumption, a stronger
range requirement, a semantic mismatch, or a change to consumer iteration
logic. It is therefore not a discriminating failure under ARC-001's First
Failure Preservation Rule.

## Interpretation limits

The calibration establishes only that:

- the proposed reference relation is implemented by the stored subject;
- the proposed consumer compiles against a stored borrowed range;
- the consumer and oracle agree for the three representative source states;
- the proposed artifacts pass the repository's current build, test, format,
  and static-analysis gates.

It does **not** establish that:

- the consumer accepts a genuinely single-pass range;
- an ephemeral value or proxy remains usable for both label and target
  observation;
- the current concept and CPO constraints are sufficient for the lazy subject;
- no hidden forward-range or witness-lifetime assumption exists;
- H-A is supported or H-B is contradicted;
- CL-001 should be strengthened;
- the minimum `input_range` posture should be retained.

Those are controlled-experiment questions and remain untested.

## Threats and review questions

- The three-state relation is a newly proposed oracle and requires human
  review before it can be frozen.
- The stored model cannot pressure single-pass invalidation or ephemeral proxy
  behavior.
- The owning observation vector could be mistaken for model materialization;
  the architectural boundary must remain explicit during review.
- The generic consumer assumes that labels and target states can be copied into
  evidence values. That is permitted by ARC-001's observation-output language,
  but should be confirmed as part of the freeze review.
- Range-for binds the current observation to a named local `witness`. The lazy
  subject must show whether its actual proxy form supports both semantic calls
  during that iteration step without extending witness lifetime.
- The small reference system tests semantic shape, not scalability or
  performance.

## Governance status and next gate

Project Principal Sami Lazreg appointed himself Human ARC Owner and Human
Experiment Owner and approved the governed baseline and ARC-001 freeze on
2026-08-13. No independent human Challenger was available. Sami Lazreg
therefore authorized the protocol's disclosed R2 AI Challenge Advisor
exception. The AI review is advisory and is not represented as human or
independent; ARC-001 alone consequently cannot promote a foundational or
strategic representation-independence Claim.

The AI Challenge Advisor identified two material pre-freeze ambiguities:

1. the declared concepts may test a value or proxy as an rvalue while the
   consumer calls the CPOs with its named `witness` as an lvalue;
2. the lazy subject characteristics did not operationally fix its range
   categories, invalidation behavior, proxy form, or cache boundary.

Both findings were accepted. ARC-001 now prospectively classifies the first as
a discriminating failure of the current contract and freezes objective
acceptance checks and storage boundaries for the second. Additional advisory
findings bound the case to copyable `int` states and `reference_label` labels,
multiset-sensitive order-independent comparison, the three declared source
states, and the recorded compiler and standard-library environment.

The next authorized sequence is:

1. commit the accepted calibration artifacts and prospective challenge
   resolutions;
2. record that exact immutable revision as the freeze commit;
3. validate the recorded frozen baseline;
4. only then implement `SUBJECT-LAZY-LTS-01` and begin controlled execution.

## Evidence disposition

**Exploratory pre-freeze calibration.** The stored calibration passed, but no
controlled architectural evidence or Claim impact is asserted.
