---
principal_case: ARC-001
supporting_case: null
last_review: 2026-08-13
---

# SemTL research state

This file is the programme-level source of truth for research work in progress,
the queued question backlog, and claims mcLab may currently make about SemTL.

## Operating posture

**Strategic thesis:** mcLab aims to establish a generic Modern C++
computational substrate for executable semantic structures.

**Operating model:** Lean Research OS - high scientific discipline with
low-to-moderate operational overhead. Process is added only when it prevents a
demonstrated research failure or material coordination cost.

**Governance:** Human-owned and AI-advised. Named humans hold ownership,
responsibility, approval, and adjudication. GPT Work, Codex, Claude, and other
AI systems may advise or support those humans but cannot occupy an accountable
governance role. ARC-001 uses the disclosed R2 AI Challenge Advisor exception
because an independent human Challenger is unavailable; the AI is not recorded
as human or independent, and the case carries the protocol's Claim-promotion
limit.

**Current decision:** No M02 strategic outcome is authorized. Sami Lazreg is
Project Principal, Human ARC Owner, and Human Experiment Owner. On 2026-08-13,
he accepted ARC-001 A02 as Controlled evidence supporting H-A within the frozen
scope. A01 remains INCONCLUSIVE as the controlled attempt and Exploratory as
by-value engineering evidence. ARC-001 remains ACTIVE pending its separate
closure and next-question actions. No independent human Challenger is
available; the disclosed R2 AI Challenge Advisor exception and its
Claim-promotion ceiling remain in force.

**Human governance record:** Project Principal Sami Lazreg approved the governed
baseline and ARC-001 freeze, then accepted the bounded A02 controlled result on
2026-08-13 after Codex review and independent validation. In a separate
Project Principal action, he authorized the bounded Controlled wording for
`CL-001` below. This does not authorize a foundational representation-
independence Claim, require an ADR, close ARC-001, or promote experimental code.

## Active

| Case | Tier | Question | Phase | Evidence target |
| --- | --- | --- | --- | --- |
| [ARC-001](cases/ARC-001.md) | R2 | Can one single-pass local labelled-transition consumer remain source-unchanged when the same finite labelled transition relation is exposed through stored stable witnesses and generated ephemeral witnesses? | A02 accepted as supporting H-A within scope; closure pending. | Controlled |

## Supporting

None. The low-WIP rule permits at most one supporting R2 case.

## Queue

These are seed questions, not active workstreams.

| ID | Question | Likely tier | Activation dependency |
| --- | --- | --- | --- |
| RQ-SEM-001 | What is the minimal reusable semantic vocabulary? | R3 | Evidence from materialization and formalism cases |
| RQ-SEM-002 | Which operations belong to semantic structure, and which belong to algorithms? | R2 | A concrete disputed operation |
| RQ-SEM-003 | What is the correct unit of semantic capability? | R2 | ARC-001 outcome |
| RQ-FOR-001 | How far does formalism genericity extend? | R3 | Stable initial materialization contract |
| RQ-MAT-001 | How far does materialization genericity extend? | R3 | Several closed materialization cases, beginning with ARC-001 |
| RQ-ALG-001 | Which consumers are genuinely reusable? | R2/R3 | First algorithmic consumer beyond local enumeration |
| RQ-COMP-001 | Can composed semantic structures remain first-class inputs? | R3 | Stable local semantic capabilities |
| RQ-INT-001 | Which semantic distinctions must remain explicit? | R3 | Formalism challenge with a discriminating distinction |
| RQ-LAW-001 | Which semantic laws provide concrete reusable value? | R2 | A law linked to a real consumer or transformation |
| RQ-EXT-001 | How local are formalism and representation extensions? | R3 | Post-freeze extension subject |
| RQ-BOUND-001 | Where should static and runtime polymorphism meet? | R3 | A concrete runtime integration boundary |
| RQ-LANG-001 | Is Modern C++ adequate for the desired substrate? | R3 | Accumulated language notes from controlled cases |
| RQ-PRIOR-001 | What is SemTL's relationship to relevant prior architectures? | R3 | A shared architectural comparator scenario |
| RQ-SCOPE-001 | Where should SemTL stop generalizing? | R3 | Sufficient supporting and contradicting evidence |

## Deferred

- Broad formal-agent, synthesis, GUI, and multi-language platform research is
  outside the current SemTL substrate priority.
- miniLTS comparison is deferred until its exact source, version, and
  architectural intent are established.

## Claims Ledger

Claim wording may be authorized only within its recorded scope and after named
human approval is recorded. `Exploratory` evidence may motivate a controlled
case but does not establish a broad claim.

| ID | Claim wording | Scope and exclusions | Lifecycle | Disposition | Evidence | Confidence | Next challenge |
| --- | --- | --- | --- | --- | --- | --- | --- |
| CL-001 | Within the ARC-001 finite labelled-transition domain, `CON-SUCCESSORS-01` operated unchanged across stable stored transition witnesses and a single-pass generated ephemeral-proxy representation, without requiring a persistent transition collection or stronger-than-`input_range` traversal. | The ARC-001 three-state relation; copyable `int` states and `reference_label` labels; order-insensitive, multiplicity-sensitive observations; member-backed customization; Windows clang-cl 22.1.8 with MSVC STL. Excludes other consumers, persistent evidence, deferred replay, ordered occurrence identity, move-only semantic values, other formalisms, other proxy signatures, other toolchains, and general SemTL representation independence. | ACTIVE | SUPPORTED | Controlled: accepted [ARC-001 A02 report](reports/ARC-001-A02-controlled-ephemeral-proxy.md), human-acceptance commit `fa29517dbf9ddc138395f329cfefc0ba9bb167f8`, and annotated result tag `arc-001-a02-supports-ha`. A01 remains INCONCLUSIVE as the controlled attempt and Exploratory for its generated by-value observation. | High within the demonstrated domain; low outside it | A durable-evidence or deferred-replay consumer, followed by a different formalism or independently challenged representation hold-out |
| CL-002 | The current C++20 `Post` and `TransitionRelation` contracts are syntactically independent: a model may satisfy either without satisfying the other. | Current finite discrete semantic vocabulary and member-backed v0.1 customization route. Establishes contract separability only; it does not establish useful architectural leverage or coherence when both facets exist. | ACTIVE | SUPPORTED | Exploratory: [`post.hpp`](../../include/mc_lab/semantic/facet/post.hpp), [`transition_relation.hpp`](../../include/mc_lab/semantic/facet/transition_relation.hpp), and positive and negative assertions in [`post_concepts_test.cpp`](../../tests/semantic/post_concepts_test.cpp) and [`facet_concepts_test.cpp`](../../tests/semantic/facet_concepts_test.cpp). | High | A model exposing both facets, followed by a consumer or formalism that pressures the separation |
| CL-003 | SemTL is representation-independent. | No sufficiently broad demonstrated domain. This wording is not authorized. | ACTIVE | UNTESTED | Existing fixtures are exploratory only. | Low | Crossed controlled and challenged evidence |
| CL-004 | SemTL is an algebra of executable semantics. | No demonstrated domain. This wording is not authorized. | ACTIVE | UNTESTED | No law has yet demonstrated sufficient reusable leverage. | Very low | A law case tied to a reusable operation |
| CL-005 | Separating `Post` from `TransitionRelation` provides reusable architectural leverage across consumers or formalisms. | No demonstrated domain. This wording is not authorized; current evidence establishes only syntactic independence. | ACTIVE | UNTESTED | No unchanged consumer or formalism challenge currently discriminates the separated design from a simpler alternative. | Low | A consumer or formalism for which the separation changes reuse, semantic preservation, or extension locality |
| CL-006 | The current SemTL v0.1 structural-contract slice builds and its registered tests pass in C++20 mode with extensions disabled on Windows using clang-cl 22.1.8 and MSVC 19.51. | Code revision `d4937750baa1f56c55775c8a31c6f9904006d93c` on Windows AMD64. This is an engineering-validation claim. It excludes Linux and macOS replication, semantic-law validity, architectural genericity, compile-time scalability, and C++20 adequacy for SemTL as a whole. | ACTIVE | SUPPORTED | Engineering validation EV-2026-08-12-01 below: both commit gates built all five targets and passed all 11 registered tests. | High within scope | A public-contract or toolchain change, or independent non-Windows replication |

## Engineering validation record

### EV-2026-08-12-01 - Governance-baseline code validation

**Human evidence owner / approver:** PENDING; no named human has yet been
recorded. The compiler and test facts remain reproducible observations, but
their acceptance into governed research state is pending that attribution.

**Evidence class:** Engineering validation. These observations confirm the
health of the implementation artifacts within the recorded environment. They
do not count as Controlled architectural evidence, freeze ARC-001, validate a
semantic law, or strengthen CL-003 or CL-004.

**Code revision:** `d4937750baa1f56c55775c8a31c6f9904006d93c`

**Environment:** Windows AMD64; C++20; language extensions disabled.

| Gate | Toolchain or scope | Result |
| --- | --- | --- |
| `windows-clangcl-commit-gate` | clang-cl 22.1.8, warnings as errors | Formatting passed for 28 files; all five targets built; 11/11 registered tests passed. |
| `windows-msvc-commit-gate` | MSVC 19.51.36252, warnings as errors | Formatting passed for 28 files; all five targets built; 11/11 registered tests passed. |
| `windows-clangcl-tidy` | clang-tidy 22.1.8 | Static analysis passed for all five registered translation units. |
| `windows-clangcl-coverage` | LLVM source-based coverage | The configured instrumented surface reported 62/62 lines, 10/10 branches, and 11/11 functions; the 90% policy passed. |

**Interpretation:** The current structural-contract implementation is a healthy
exploratory slice and is compiler-portable across the two tested Windows C++20
toolchains. The coverage result is narrow: it measures eight instrumented CPO
and CLI source files, not compile-time concepts, semantic laws,
cross-representation equivalence, or architectural hypotheses.

**Reproduction:** From a Visual Studio developer environment, run:

```text
cmake --workflow --preset windows-clangcl-commit-gate --fresh
cmake --workflow --preset windows-msvc-commit-gate --fresh
cmake --workflow --preset windows-clangcl-tidy --fresh
cmake --workflow --preset windows-clangcl-coverage --fresh
```

## Known limitations and open uncertainties

- No controlled single-pass transition-materialization experiment with a
  frozen unchanged consumer.
- No adversarial materialization hold-out.
- No controlled formalism-genericity case.
- No controlled comparator experiment against runtime polymorphism, PINS,
  Boost.Graph, or Spot.
- No model exposing both `Post` and `TransitionRelation` and no executable
  conformance check for their documented coherence law.
- No law experiment demonstrating reusable computational leverage.
- No accumulated evidence capable of determining C++20/23/26 language
  adequacy for SemTL as a whole.
- No operational evidence yet demonstrates that a fuller Research OS, separate
  registries, or additional GitHub metadata would improve decisions.

## Last review

**Date:** 2026-08-13

**What changed:** Adopted the Lean Research OS posture and subsequently revised
the operational protocol to v0.8. Governance remains human-owned and
AI-advised. Sami Lazreg is Project Principal, Human ARC Owner, and Human
Experiment Owner for ARC-001. Because no independent human Challenger is
available, Sami Lazreg authorized the disclosed R2 AI Challenge Advisor
exception with its Claim-promotion ceiling. Added compact append-only Git
evidence rules after A01 demonstrated the need to separate materially distinct
attempts. A01 is INCONCLUSIVE as the controlled result and preserved as
Exploratory by-value engineering evidence. A02 was executed independently from
the ARC-001 freeze record and accepted as Controlled evidence supporting H-A
within scope. No Claims Ledger wording, ADR, ARC closure, or M02 strategic
decision was authorized by the evidence-acceptance step itself. In a subsequent
Project Principal action, the bounded demonstrated result was authorized as
the new `CL-001` wording; `CL-003` remains unauthorized.

**Cases closed:** None.

**Claims changed:** Strengthened CL-001 from design intent to the bounded
Controlled result demonstrated by ARC-001 A02. Retained CL-002's demonstrated
syntactic-independence fact, CL-005's untested architectural-leverage
proposition, and the environment-bounded engineering claim CL-006. CL-003 and
CL-004 remain unauthorized.

**Research integration validation:** The maintained ARC-001 hierarchy was
reconstructed on a clean branch from `origin/master`, not from an experiment
branch. The Windows clang-cl commit gate passed formatting for 35 files and all
14 registered tests; clang-tidy passed all 8 registered translation units. The
three research targets are labelled separately as calibration, A01
Exploratory/INCONCLUSIVE, and A02 Controlled/SUPPORTED.

**ADRs changed:** None. Current semantic documentation remains the governing
architecture until a controlled Research Case changes it.

**Project Principal approval for this review and retained Claim wording:** Sami
Lazreg, 2026-08-13. The bounded A02 result and the revised `CL-001` wording are
approved. No foundational or strategic representation-independence Claim is
approved.

**Next principal action:** Review and integrate the dedicated maintained
research hierarchy under
[`tests/research/arc-001/`](../../tests/research/arc-001/README.md), then close
ARC-001 separately when its next-question recommendation is recorded. The
accepted result remains fixed by annotated tag `arc-001-a02-supports-ha`.
Do not activate another principal case while ARC-001 remains active.
