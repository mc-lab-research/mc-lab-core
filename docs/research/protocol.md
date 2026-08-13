# SemTL Architectural Research Protocol v0.5 Lean

This protocol governs SemTL architectural research during M02. It preserves
falsification, evidence, and claim discipline with the smallest practical
management surface. It is subordinate to the mcLab Strategic Baseline and does
not select a technical solution in advance.

## Research mandate

The current strategic thesis is:

> mcLab aims to establish a generic Modern C++ computational substrate for
> executable semantic structures.

M02 asks whether that substrate can be established within a demonstrated
domain and at acceptable architectural and language cost. M02 eventually ends
in **COMMIT**, **NARROW**, **REDESIGN**, or **REJECT / REFRAME**. "Library
implemented" is not a strategic conclusion.

## Selected operating posture: Lean Research OS

mcLab currently uses a **Lean Research OS**: high scientific discipline with
low-to-moderate operational overhead.

Research rigor comes from experimental structure and evidence quality, not the
number of registries, fields, labels, workflow states, issues, documents, or
approval checkpoints.

### Protocol Minimality

The Project Principal is the human Research Process Minimality Authority, with
GPT Work available as an advisor. Every persistent process element must prevent
a concrete failure or coordination cost by doing at least one of the following:

- preserving evidence or reproducibility;
- preventing retrospective experiment modification;
- separating observation from interpretation;
- preventing an unsupported claim;
- clarifying authority or responsibility;
- reducing material coordination cost;
- preserving architectural traceability;
- or enabling a decision that would otherwise be difficult.

If an element has no convincing justification, remove, merge, defer, or keep it
implicit.

> Do not materialize process before its necessity has been demonstrated.

Protocol evolution follows three questions:

1. Could the current protocol have prevented the observed failure?
2. If not, what is the smallest additional mechanism that would?
3. Does that mechanism justify permanent process complexity?

## Scientific core

Every significant investigation preserves:

```text
Question -> Hypothesis -> Experiment -> Evidence -> Decision
```

For a research-critical experiment, also preserve:

- a decision-relevant question;
- credible competing hypotheses;
- an explicit technical discriminator;
- a controlled freeze;
- the First Failure Preservation Rule;
- observation separated from interpretation;
- contradictory evidence and credible alternative explanations;
- independent challenge when significance warrants it;
- bounded claim wording;
- and traceability to an architectural decision when one changes.

The following rules are constitutional:

1. Architectural and strategic claims require evidence.
2. Frozen hypotheses, contracts, and expected observations are not rewritten
   after results are observed.
3. Counterevidence remains recoverable.
4. Formalism genericity and materialization genericity remain distinct.
5. Semantic integrity takes precedence over superficial uniformity.
6. Observation, interpretation, claim impact, and decision remain distinct.
7. A failed fit is recorded and analyzed before generalization or repair.
8. Genericity claims remain bounded by their demonstrated domain.

## Authority and handoffs

### Human accountability principle

Research governance is human-owned and AI-advised. Every Research Case,
controlled experiment, freeze, evidence assessment, Claim change, ADR, and
strategic decision has a named human owner or approver.

AI systems may support research by proposing questions, alternatives,
discriminators, implementations, challenge prompts, interpretations, and draft
records. They cannot be the owner, accountable or responsible party, approver,
adjudicator, experiment authority, or independent Challenger. AI output remains
advisory until a named human reviews and accepts it. If the required human role
is unassigned, the affected work cannot pass its governance checkpoint.

### Project Principal

The Project Principal is human and retains constitutional authority over
purpose, the Research Question and Hypothesis portfolio, major risk, protocol
adoption, strategic veto, termination, Research Case activation, experiment
freezes, accepted Claim wording, governing ADRs, and M02 decisions. The
Project Principal appoints a named Human ARC Owner, Human Experiment Owner, and
Independent Human Challenger when those roles are required.

### Human ARC Owner and Human Experiment Owner

The Human ARC Owner is accountable for the Research Case. The owner determines,
subject to Project Principal approval where required:

- what the case must learn and why it matters;
- which hypotheses and evidence could change the decision;
- whether the proposed design and discriminator answer the question;
- whether evidence and challenge have been assessed without exceeding the
  observed domain;
- and which bounded Claim update or recommendation to propose.

The Human Experiment Owner is responsible for controlled execution, frozen
contract compliance, evidence preservation, and stopping before unauthorized
repair. One person may hold both human roles when independence is not required
between them. Neither role may be assigned to an AI system.

### GPT Work: Research Operations Advisor

GPT Work supports the Project Principal and Human ARC Owner by:

- proposing Research Questions, hypotheses, and decision-relevant evidence;
- identifying strategic implications and credible alternative explanations;
- challenging whether each protocol element still deserves to exist;
- drafting evidence syntheses, bounded Claim wording, and recommendations; and
- checking that observation, interpretation, Claim impact, and decision remain
  distinct.

GPT Work does not own or activate the portfolio, adjudicate evidence, approve a
freeze, accept a Claim, close a Research Case, or make a strategic decision.

### Codex: Architecture and Design Advisor

Codex supports the Human ARC Owner by instantiating an active research question
as technical alternatives and a discriminating architecture experiment. Codex:

- formulates architectural alternatives;
- identifies technical assumptions;
- defines minimum semantic requirements and explicit non-requirements;
- proposes concepts, CPOs, views, adapters, type erasure, runtime boundaries,
  and competing designs;
- identifies the technical discriminator;
- defines the contracts that must be tested and frozen;
- inspects whether frozen contracts were respected;
- proposes interpretations of architectural consequences for human review;
- and drafts ADR proposals when governing architecture may change.

Codex may propose Research Questions and hypotheses. It does not own a design
decision, freeze, experiment, Claim, ADR, issue, or portfolio, and it cannot
approve or adjudicate them.

```text
Humans:   What must we learn, who owns the work, what evidence changes the
          decision, and what is accepted?
GPT Work: Which questions, hypotheses, evidence needs, and process changes
          should the human owners consider?
Codex:    Which alternatives instantiate the question, what separates them,
          and which contracts should be tested?
```

### Claude: Implementation and Experiment Support

Claude supports the Human Experiment Owner by:

- implementing experimental subjects and frozen consumers;
- executing controlled experiments under human direction;
- preserving the first discriminating failure;
- recording observations and reproduction evidence;
- and exposing accidental complexity, hidden assumptions, hidden
  materialization, coupling, semantic loss, lifetime friction, diagnostics,
  special cases, and compile-time costs.

Claude is not the developer of record, experiment owner, responsible party, or
result adjudicator. It does not independently authorize or perform repair after
an unexpected result, and it does not change a frozen question, hypothesis,
consumer, guardrail, or success condition to make an experiment pass. Any such
change requires explicit human direction after the original result is
preserved.

### Independent Human Challenger

The Challenger is a named human who is independent from the uninterrupted
design-to-implementation path. The role attacks the inference, not
implementation quality. An AI system may supply challenge questions or analysis
as advisory material, but an AI-only review does not satisfy this independence
requirement.

The Challenger asks:

- Could another explanation produce this result?
- Is the experiment actually discriminating?
- Is hidden canonicalization occurring?
- Were semantics preserved?
- Was the consumer really unchanged?
- Was the hold-out genuinely adversarial?
- Does the proposed Claim exceed the observed domain?

The Human ARC Owner assesses Challenger evidence and the Project Principal
approves any resulting Claim or governing decision. AI advisors may support
that assessment but do not adjudicate it.

### Mission Engineering

Mission Engineering is a human responsibility that turns active research intent
into bounded increments and execution order. AI systems may advise or assist
with planning, but they do not own the plan or change the question,
discriminator, freeze, or success conditions.

## Minimum persistent artifact set

The authoritative chain is:

```text
Strategic Baseline
    -> this protocol
    -> state.md
    -> one ARC-###.md per significant case
    -> experiments, PRs, and evidence
    -> ADR only when governing architecture changes
```

The persistent research artifacts are therefore:

1. [`state.md`](state.md): compact programme state, backlog, evidence posture,
   Claims, limitations, and next decision;
2. one `cases/ARC-###.md` file per R2 or R3 investigation;
3. code, tests, diagnostics, commits, CI results, and PRs as evidence;
4. an evidence-aware ADR only when the result establishes or changes governing
   architecture.

Do not create independent Question, Hypothesis, Claim, Property, Subject,
Consumer, Law, language-note, freeze, or evidence registries until demonstrated
scale or coordination pressure makes reconstruction materially difficult.

Derived tables, including a Genericity Matrix, are lightweight views. They do
not become independent sources of truth and must not produce a genericity score
or coverage percentage.

## Research significance and WIP

- **R0 - routine engineering:** normal engineering workflow; no ARC.
- **R1 - local reversible architecture:** record a short rationale in the
  issue, code, review, or relevant ADR; normal review is sufficient.
- **R2 - architectural hypothesis:** use one ARC when semantic capabilities,
  materialization, extension locality, semantic preservation, composition,
  ownership, language adequacy, or an architectural Claim is affected.
- **R3 - strategic investigation:** use one ARC with substantive prior-art and
  comparator analysis, an adversarial hold-out, independent challenge, and
  explicit M02 impact.

At the current scale, one principal R2 or R3 case may be active and one
supporting R2 may be active. A second concurrent R3 requires Project Principal
approval. Seed questions are a backlog, not concurrent workstreams.

Escalate ordinary work to R2 when a generic consumer gains model-specific
branching, a representation must be materialized, semantics are lost, a frozen
contract must change, or a public architectural Claim is affected. Escalate R2
to R3 when the thesis, supported domain, foundational architecture, language
baseline, or M02 outcome may materially change.

## Compact lifecycle and evidence vocabulary

ARC lifecycle:

- **OPEN**;
- **ACTIVE**;
- **CLOSED**;
- **SUPERSEDED**.

Evidence disposition records the result direction in the canonical ARC
frontmatter key `evidence_disposition`:

- **UNTESTED**;
- **SUPPORTED**;
- **CHALLENGED**;
- **CONTRADICTED**;
- **INCONCLUSIVE**.

Evidence posture records the strength of the evidence in the canonical ARC
frontmatter key `evidence_posture`:

- **Exploratory:** rationale, feasibility prototype, or development-model
  observation;
- **Controlled:** frozen discriminating experiment with predeclared outcomes;
- **Challenged:** adversarial hold-out, external grounding, or independent
  replication.

The two dimensions are not interchangeable. `SUPPORTED` with `EXPLORATORY`
posture, for example, records a promising but non-controlled observation. ARC
frontmatter and templates use the two canonical keys above; the ambiguous
single `evidence` key is not used.

These epistemic states live in `state.md` and the ARC, not in a parallel set of
GitHub workflow states.

## Architectural property vocabulary

Properties are domain-bounded vocabulary, not registry objects or KPIs.

- **P1 Representation independence:** a relevant consumer remains unchanged
  across materially different representations without hidden canonicalization.
- **P2 Formalism extensibility:** meaningfully new semantics do not require
  disproportionate changes to unrelated core abstractions.
- **P3 Algorithmic reuse:** one consumer is reused against explicit semantic
  requirements where legitimate.
- **P4 Compositionality:** composed or transformed structure remains a
  first-class input with explicit semantics.
- **P5 Semantic preservation:** every distinction needed by applicable
  consumers survives abstraction.
- **P6 Minimal structural assumptions:** a consumer requires only what its
  semantic logic needs.
- **P7 Extension locality:** change propagates according to semantic
  responsibility rather than implementation coupling.
- **P8 Law utility:** a law enables reusable checking, transformation,
  optimization, or defect detection.
- **P9 Language adequacy:** C++ expresses the architecture at acceptable
  conceptual, compilation, diagnostic, lifetime, and runtime cost.
- **P10 Static/runtime boundary adequacy:** static and runtime-selected models
  meet at explicit boundaries without unjustified erasure or rigidity.
- **P11 Capability orthogonality, candidate:** independent capabilities remain
  separate unless semantics demonstrate a real dependency.

An ARC names only the properties it actually pressures and states their local
operational meaning.

## Lean ARC lifecycle

The ARC is the primary research unit and normally contains:

```text
Question
    -> why it matters and decision enabled
    -> relevant properties
    -> competing hypotheses
    -> experimental design and discriminator
    -> frozen contract
    -> expected supporting, contradicting, and inconclusive observations
    -> observation and preserved first failure
    -> challenge or alternative explanation
    -> interpretation
    -> Claim impact
    -> decision
    -> next question
```

### Entry to controlled execution

Before the discriminator is executed, the ARC records:

- the question and credible alternatives;
- subjects and consumer;
- minimum requirements and explicit non-requirements;
- discriminator;
- supporting, contradicting, and inconclusive observations;
- semantic-integrity and complexity guardrails;
- and the lightweight freeze.

### Lightweight freeze

The freeze lives in the ARC and uses Git as the immutable reference. It records:

```text
Freeze commit
Frozen consumer
Frozen oracle
Frozen requirements
Explicit non-requirements
Permitted adaptations
Prohibited adaptations
Status
Human ARC Owner assessment
Project Principal approval
```

Do not duplicate freeze state through several documents, Project fields,
labels, and checklists. Add another mechanism only after a concrete integrity
or coordination failure demonstrates the need.

### First Failure Preservation Rule

When a controlled discriminator fails, the Human Experiment Owner stops
controlled execution before redesign or repair. Claude may support preservation
of:

- the freeze reference and original frozen configuration;
- consumer and subject revisions;
- command, environment, compiler, and standard library;
- exact diagnostic, runtime failure, or semantic mismatch;
- raw output and smallest faithful reproduction;
- and the observation without architectural interpretation.

The failing state remains recoverable in Git or a linked evidence artifact. A
repair, workaround, stronger requirement, cache, consumer change, or redesign
is attempted only after preservation and remains distinguishable from the
original result.

Preservation does not decide causality. Codex may propose an architectural
interpretation, GPT Work may advise on evidence and Claim impact, and the
Independent Human Challenger attacks the inference. The Human ARC Owner assesses
whether the result reflects the hypothesis, frozen contract, apparatus,
compiler, library, or another cause; the Project Principal approves any Claim
or governing decision.

### Stop and record

Stop before redesign when the discriminator is observed, a frozen contract
must change, hidden canonicalization or materialization appears, semantic
information is lost, model-specific branching is required, a guardrail is
exceeded, or the experiment no longer separates the alternatives.

### Closure

An ARC closes when it records observation, reproduction evidence, alternative
explanation, proportionate challenge, Claim impact, the decision or explicit
inconclusion, and the next-question recommendation.

## Proportional challenge

- **R0:** no Independent Human Challenger.
- **R1:** normal technical review is normally sufficient.
- **R2:** use an Independent Human Challenger when evidence could materially
  strengthen, narrow, or contradict an architectural Claim.
- **R3:** an Independent Human Challenger is mandatory.

ARC-001 retains an Independent Human Challenger because it tests a foundational
representation-independence hypothesis.

## Claims and decisions

After named Project Principal approval is recorded, the compact Claims section
in `state.md` authorizes current wording. Each Claim states its demonstrated
domain, material exclusions, counterevidence, current status, and next
discriminator. Keep it compact until interactions across cases make
reconstruction difficult.

Acceptable:

> Within the tested finite labelled-transition domain, the local consumer
> operated unchanged across the explicit stored and single-pass lazy
> representations.

Not authorized by one such case:

> SemTL is representation-independent.

Architectural actions include **KEEP**, **NARROW**, **MOVE**, **MERGE**,
**SPECIALIZE**, **DELETE**, **REDESIGN**, and explicit **INCONCLUSIVE**.

Create an ADR only when evidence establishes or changes governing architecture.
An executed experiment, fixture repair, or inconclusive apparatus failure does
not by itself require an ADR.

## Requirement-driven technical research

Keep language, law, comparator, hold-out, and subject notes inside the ARC
unless several cases need an independent lifecycle.

- C++ analysis begins from a semantic requirement and distinguishes language,
  library, compiler, portability, ergonomic, and architectural limitations.
- A candidate law supports an algebraic Claim only when it enables concrete
  reusable leverage.
- Comparators answer the same architectural question; they do not participate
  in feature-count competition.
- Generalization is assessed by useful semantic or representational reuse
  relative to added concepts, CPOs, lifetime machinery, specializations,
  diagnostics, compilation cost, and conceptual burden.

## Lean GitHub operating interface

GitHub answers **where the work is operationally**. The ARC answers **what is
currently known**.

### Project statuses

Use these eight operational states:

```text
INBOX -> BACKLOG -> DESIGN -> FROZEN -> EXPERIMENT -> REVIEW -> DONE
BLOCKED  (temporary operational state for any item that cannot advance)
```

`FROZEN` requires the ARC freeze record and Project Principal approval. A
passing CI run does not imply Controlled evidence, Claim support, or ARC
closure. `DONE` means that the tracked issue or pull request is operationally
complete; automation may set it when an item is closed or a pull request is
merged, but that transition never approves evidence, a Claim, an ARC closure,
or an ADR.

### Minimum Project fields

Use only:

- **Status**;
- **Research Case ID** (`ARC-001`, `ARC-002`, and so on);
- **Human owner**, implemented by the native GitHub assignee and never by an AI
  system;
- **Work type**, implemented by the native organization Issue Type rather than
  a duplicate Project field;
- **Architectural properties** (P1-P11) when useful;
- **Priority**, **Effort**, and **Complexity**, using the organization Issue
  fields already attached to the Project.

The native Issue Type mapping is:

- **Initiative** for the principal ARC issue;
- **Research** for a bounded investigation or question;
- **Proposal** for an architectural alternative;
- **Evaluation** for a characterization or comparison;
- **Validation** for controlled or reproduction evidence;
- **Decision** for human adjudication;
- **Specification** for a stabilized normative contract;
- **Feature**, **Task**, **Bug**, and **Document** for implementation, support,
  defect, and documentation work.

`Research -> Proposal -> Evaluation -> Validation -> Decision ->
Specification` is a useful path, not a mandatory issue chain. A negative,
contradictory, deferred, or inconclusive case may stop earlier once its evidence
and Claim impact are recorded. Priority, Effort, and Complexity support
operational planning only; they do not encode significance, evidence strength,
confidence, approval, or architectural value.

Add an Evidence field only if actual operation demonstrates that it reduces
coordination cost. Do not mirror the same state through fields, labels,
milestones, issue titles, and ARC metadata.

### Labels

Use labels for stable repository taxonomy, not a second Project ontology. A
small set is sufficient:

```text
research
semtl
architecture
experiment
```

Do not create `role:*`, `stage:*`, `property:*`, `evidence:*`, or `claim:*`
label families while those meanings already live in Project fields or ARCs.

### ARC issue decomposition

Prefer **one ARC = one principal issue plus N implementation PRs** when that is
sufficient. The issue may contain task groups for Design, Calibration, Freeze,
Experiment, Challenge, Evidence, and Adjudication.

When separate ownership, concurrency, or evidence traceability provides real
value, split ARC-001 into at most these initial operational units:

1. **Design & Calibration** - Human ARC Owner, advised by Codex and supported
   by Claude;
2. **Controlled Experiment** - Human Experiment Owner, supported by Claude;
3. **Challenge & Evidence Review** - Independent Human Challenger, with Codex
   and GPT Work available as advisors;
4. **Adjudication** - Human ARC Owner assessment, with Project Principal
   approval where required.

Before work passes its relevant governance checkpoint, every issue and PR has a
named human assignee. An unassigned intake item remains in `INBOX` or `BLOCKED`;
an unassigned ARC cannot be frozen or enter controlled execution. AI support
may be acknowledged in the issue or PR body, but it is not represented as
ownership and does not create a separate authority field.

This is a default, not a mandatory issue count.

## Scaling criteria

Expand toward a fuller Research OS only when concrete evidence shows that the
Lean system is inadequate, for example when:

- Claims become difficult to trace across many ARCs;
- several experiments depend on the same hypotheses;
- multiple researchers or teams work concurrently;
- property evidence becomes inconsistent across cases;
- external publication requires stronger provenance;
- parallel cases repeatedly create coordination failures;
- freeze integrity is lost despite the ARC and Git reference;
- or the GitHub operational view can no longer represent ownership and flow.

Extract only the abstraction needed to solve the demonstrated problem. Do not
pre-build a mature Research OS.

The operating objective is:

> Maximize strategically useful architectural information per unit of
> engineering effort.

Do not optimize for numbers of cases, experiments, Claims, issues, closed
items, properties covered, or Genericity Matrix cells.

## Protocol calibration

ARC-001 is the first Lean Research OS pilot. After it closes, review which
protocol elements prevented a real failure, which were unused, whether the
freeze and First Failure rule preserved evidence, and what can be deleted.

Unless ARC-001 exposes a material governance defect, protocol-design work then
stops and priority remains on architectural research.
