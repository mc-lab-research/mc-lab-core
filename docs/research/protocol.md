# SemTL Architectural Research Protocol v0.2 Operational

This protocol governs architectural research performed by SemTL during M02. It
preserves the distinction between research questions, hypotheses, observations,
claims, and decisions while keeping the physical documentation system small.

The protocol is subordinate to the mcLab Strategic Baseline. It does not define
SemTL's technical solution in advance.

## Research mandate

The current strategic thesis is:

> mcLab aims to establish a generic Modern C++ computational substrate for
> executable semantic structures.

M02 asks:

> Can a generic executable semantic substrate be established in Modern C++?

If evidence supports such a substrate, a second-order question asks what
architectural leverage it provides relative to simpler or existing
architectures. Novelty is not an experiment success condition. The strongest
architecture may reuse or combine established patterns.

M02 eventually terminates in one of:

- **COMMIT**;
- **NARROW**;
- **REDESIGN**;
- **REJECT / REFRAME**.

"Library implemented" is not a strategic conclusion.

## Constitutional research rules

The following rules apply regardless of experiment size:

1. Architectural and strategic claims require evidence.
2. A frozen hypothesis and its expected observations cannot be rewritten after
   results are observed.
3. Counterevidence remains recoverable.
4. Hold-outs attack named assumptions rather than merely add examples.
5. Genericity claims remain bounded by their demonstrated domain.
6. Formalism genericity and materialization genericity remain distinct.
7. Semantic integrity takes precedence over superficial uniformity.
8. C++ investigation starts from semantic requirements, not language features.
9. Observation, interpretation, claim impact, and decision remain distinct.
10. A failed fit is analyzed before the architecture is generalized to
    accommodate it.

Removing unsupported machinery is a valid positive result. An experiment may
justify deleting a concept or CPO, merging facets, moving an operation to an
algorithm, specializing a path, or narrowing the supported domain.

## Governance and handoffs

### Project Principal

The Project Principal retains constitutional authority over purpose, major
risk, governance changes, strategic veto, and termination.

### Strategic & Research Operations Authority

GPT Work governs:

- strategic alignment and research priority;
- this protocol and its proportional application;
- activation and closure of R2/R3 Research Cases;
- experiment intent, evidence requirements, and strategic gates;
- the Claims Ledger;
- evidence integration;
- and recommendations to COMMIT, NARROW, REDESIGN, or REJECT / REFRAME.

This authority adjudicates evidence; it does not make an architectural
proposition true by authority.

### Mission Engineering

Mission Engineering translates active research intent into bounded increments,
dependencies, and execution sequencing. It does not silently change a frozen
question, success condition, or experiment boundary.

### Design Authority

Design Authority proposes technical alternatives, including concepts, CPOs,
members, traits, views, adapters, type erasure, or runtime polymorphism. The
protocol defines what must be learned, not which implementation must win.

### Experiment implementation

Developers and implementation agents construct prototypes, subjects,
consumers, tests, and measurements. Unexpected coupling, materialization,
semantic loss, lifetime complexity, diagnostic failures, special cases, and
compile-time costs are research observations and must not be engineered around
silently.

### Challenge and review

Material conclusions should receive a distinct challenge. For R3, use a
separate reviewer, human, or agent where practical. GPT Work must not be the
only generator, implementer, and evaluator of strategically important evidence.

## Conceptual model and physical documents

SemTL research conceptually distinguishes:

```text
Question -> Hypothesis -> Property -> Subject -> Experiment -> Evidence
         -> Claim -> Decision
```

These conceptual entities do not require separate registries or files.

The authoritative living document types are:

1. [`state.md`](state.md), containing programme WIP, the queued backlog, and
   Claims Ledger;
2. one `cases/ARC-###.md` Research Case for each R2/R3 investigation;
3. an evidence-aware ADR only when an experiment changes governing
   architecture.

Code, tests, input models, diagnostics, and CI results are evidence artifacts.
The Research Case links them to the question and records their interpretation.

Create an independent model, consumer, law, or language record only when it
needs an independent lifecycle, such as reuse by several cases or separate
review authority.

## Derived views

The following are generated or assembled from case metadata when useful and
are not independently maintained sources of truth:

- Genericity Matrix;
- architectural-property evidence view;
- experimental-subject catalog;
- language-adequacy summary;
- candidate-law inventory;
- research dashboard.

A Genericity Matrix must distinguish planned, controlled support, challenged
support, contradicted, unsupported, and not applicable. It must not be reduced
to a coverage percentage.

## Research significance

### R0 - routine engineering

Examples include formatting, local refactoring, test-fixture cleanup, and an
implementation optimization behind unchanged contracts.

Use the normal engineering workflow. Do not open a Research Case.

### R1 - local reversible architectural choice

Examples include a private data structure, helper abstraction, or reversible
implementation mechanism behind a stable semantic contract.

Record a short rationale in the issue, change review, code, or relevant ADR.

### R2 - SemTL architectural hypothesis

Use R2 when a decision affects semantic capabilities, materialization
independence, formalism extension, consumer requirements, composition,
semantic preservation, ownership or borrowing contracts, customization,
extension locality, law utility, or an architectural claim.

Open one controlled Research Case.

### R3 - strategic architectural investigation

Use R3 when an issue could affect the SemTL strategic thesis, supported domain,
M02 outcome, foundational architecture, minimum C++ language level, broad
genericity claim, or major research commitment.

Use the same Research Case format with the additional R3 controls described
below.

### Escalation and stop rule

Escalate ordinary work to R2 when:

- a generic consumer gains model-specific branching;
- a representation must be materialized to conform;
- semantic information is lost;
- a second unrelated model needs the same workaround;
- a new generic concept, CPO, associated type, or specialization path is
  proposed;
- a frozen contract must change;
- or a SemTL claim is affected.

Escalate R2 to R3 when the result could materially narrow or reject the thesis,
change a foundational capability, change the language baseline, or requires a
strategic public claim.

When an escalation trigger occurs, normal implementation stops. Record the
observation before changing the architecture.

## Work-in-progress limit

At the current programme scale:

- one principal R2 or R3 Research Case may be active;
- one secondary supporting R2 case may be active;
- a second concurrent R3 case requires Project Principal approval.

The seed research questions are a backlog, not concurrent workstreams. Close,
supersede, defer, or block the principal case before activating the next one.

## Common lifecycle and evidence vocabulary

Use the lifecycle states:

- **OPEN**;
- **ACTIVE**;
- **CLOSED**;
- **SUPERSEDED**.

Use the evidence dispositions:

- **UNTESTED**;
- **SUPPORTED**;
- **CHALLENGED**;
- **CONTRADICTED**;
- **INCONCLUSIVE**.

For everyday research, classify evidence as:

- **Exploratory** - rationale, feasibility prototype, or development-model
  observation;
- **Controlled** - frozen, discriminating experiment with predeclared
  observations;
- **Challenged** - adversarial hold-out, external grounding, or independent
  replication.

Detailed E0-E5 grading is reserved for R3 or a strategic review where the
additional distinction affects the decision.

## Architectural properties

Property statements are always domain-bounded.

### Genericity family

- **P1 Representation independence:** relevant consumers remain unchanged
  across materially different representations without hidden canonicalization.
- **P2 Formalism extensibility:** a meaningfully new formalism can be added
  without disproportionate modification to unrelated core abstractions.
- **P3 Algorithmic reuse:** a consumer is implemented once against explicit
  semantic capabilities and reused where semantically legitimate.
- **P6 Minimal structural assumptions:** a consumer depends only on the
  semantic capabilities its logic requires.
- **P7 Extension locality:** changes propagate according to semantic
  responsibility rather than implementation coupling.
- **P11 Capability orthogonality, candidate:** mathematically independent
  capabilities remain architecturally independent unless a formalism
  introduces a genuine dependency.

### Semantic-integrity family

- **P4 Compositionality:** a transformed or composed structure remains
  consumable through the relevant abstractions with its semantics explicit.
- **P5 Semantic preservation:** generic abstraction retains every distinction
  required by applicable consumers.
- **P8 Law utility:** a law enables a reusable transformation, conformance
  check, optimization, defect detection, or other observable leverage.

### Language-architecture family

- **P9 Language adequacy:** Modern C++ expresses the architecture at acceptable
  conceptual, compilation, diagnostic, lifetime, and runtime cost.
- **P10 Static/runtime boundary adequacy:** static models, runtime-selected
  representations, external systems, and separately compiled consumers can
  meet at explicit boundaries without unjustified erasure or rigidity.

## Minimal R2 loop

An R2 case follows this loop:

```text
Question
    -> competing hypotheses
    -> smallest discriminating experiment
    -> freeze
    -> implementation
    -> observation
    -> challenge
    -> claim update
    -> decision
```

### Entry criteria

Before implementation reaches the decisive test, the case must state:

- decision-relevant question and scope;
- at least two credible hypotheses or alternatives;
- affected properties;
- discriminating subjects and consumer;
- relevant contracts to freeze;
- expected supporting, contradicting, and inconclusive observations;
- semantic-integrity guardrails;
- and architectural-complexity guardrails.

### Experiment stop conditions

Stop and record before redesign when:

- the discriminator has been observed;
- a frozen contract must change;
- hidden materialization or specialization appears;
- semantic information is lost;
- model-specific branching is required;
- a guardrail is exceeded;
- or the experiment no longer separates the alternatives.

### Exit criteria

An R2 case may close when it records:

- observation;
- evidence and reproduction location;
- interpretation and credible alternative interpretation;
- proportionate challenge;
- claim impact;
- architectural action or explicit inconclusion;
- and next-question recommendation.

The normal manual update path is:

1. update the Research Case;
2. update affected claims in `state.md`;
3. create or update an ADR only if governing architecture changes.

## R3 additions

An R3 case adds:

- substantive prior-art analysis;
- explicit comparator architectures;
- formal contract freeze;
- an adversarial hold-out;
- predeclared failure modes;
- H0-H5 modification classification;
- semantic-loss assessment;
- a distinct Challenger;
- external grounding or independent replication where practical;
- formal architectural review;
- and explicit M02 decision impact.

## Adversarial hold-outs

A hold-out attacks a named hypothesis after relevant contracts are frozen. It
normally remains a section inside the Research Case.

Required fields are:

- hypothesis and architectural assumption under attack;
- why the subject is discriminating;
- frozen contracts;
- expected failure modes;
- permitted adaptations;
- adaptations that would contradict the hypothesis;
- semantic information that must survive;
- result and modification classes;
- semantic loss;
- conclusion.

Classify modifications as:

- **H0 No architectural modification** - model-local implementation only;
- **H1 Predicted local adaptation** - an existing extension mechanism works;
- **H2 Local capability extension** - genuinely new semantics remain local;
- **H3 Core correction** - a frozen contract was incomplete or incorrect;
- **H4 Transversal redesign** - unrelated capabilities or consumers change;
- **H5 Semantic accommodation failure** - conformance requires semantic
  erasure, hidden specialization, invalid behavior, or forced canonicalization.

After observing the result, do not rewrite the original hypothesis, expected
observation, permitted adaptations, freeze, or disconfirming criteria. Test a
redesign in a successor case.

## Experimental subjects and consumers

Subjects normally live inside their Research Case. Give each a stable local ID,
type, experimental role, semantic characteristics, representation
characteristics, assumption tested, and provenance.

Roles include development, calibration, adversarial, formalism hold-out,
materialization hold-out, externally grounded, and independent replication.

A generic consumer is an architectural probe. Record:

- semantic purpose;
- minimum requirements and explicit non-requirements;
- applicable and negative-applicability cases;
- source changes;
- model-specific branches;
- specialization;
- forced materialization;
- and semantic information lost.

Meaningful reuse requires unchanged algorithmic logic, declared extension
points, preserved semantics, and correct rejection of non-applicable subjects.

## Requirement-driven C++ investigation

Language analysis stays inside the case that exposed it unless it becomes
independently significant.

Record:

```text
Semantic requirement
Desired abstraction
C++20 expression and mechanisms
Observed friction and compiler behavior
Diagnostics
Ownership and lifetime consequences
Compile-time and runtime consequences
Simpler architectural alternative
C++23 or C++26 alternative, only if relevant
Standardization and implementation maturity
Architectural consequence
Conclusion, confidence, and next experiment
```

Distinguish a language-standard limitation from a standard-library limitation,
compiler defect, implementation immaturity, portability problem, ergonomic
problem, or architectural limitation.

Create a dedicated language record only if the finding affects multiple cases,
may change the minimum baseline, or becomes strategically significant.

## Law research

A candidate law normally remains in its Research Case. Distinguish:

- mathematical law;
- executable conformance property;
- implementation invariant;
- optimization law;
- API convenience.

A law supports the stronger executable-semantic-algebra thesis only when it
enables concrete reusable leverage such as a transformation, conformance
check, optimization, or defect detection.

## Comparator experiments

Comparators answer a shared architectural question rather than participate in
feature-count competition.

- STL and Ranges are methodological references for requirements, views,
  borrowing, and algorithms independent from representation.
- Boost.Graph is a reference for capability factoring, associated types,
  external adaptation, and implicit structures.
- Spot is a domain-specialized formal-methods architecture showing what strong
  semantic specialization buys.
- PINS/LTSmin is a reference for comparatively minimal model/algorithm
  separation.
- Conventional runtime polymorphism is the control for significant static
  genericity choices.
- miniLTS is unavailable for comparative claims until its exact artifact,
  version, source, and intent are established.

For significant static mechanisms, ask both:

> Why is static genericity necessary here?

and:

> Could a simpler runtime-polymorphic boundary preserve the relevant property?

## Architectural budget and genericity leverage

Every generalization consumes architectural budget through concepts, CPOs,
associated types, adapters, lifetime machinery, specialization paths,
compilation cost, diagnostics, or conceptual burden.

Use this decision lens without turning it into a rigid metric:

```text
genericity leverage
    approximately equals
legitimate additional semantic or representational reuse
    divided by
additional architectural complexity
```

Assess leverage as high, positive, marginal, negative, or unresolved. A
high-complexity mechanism serving one marginal case faces a presumption against
inclusion. Consider narrowing, specializing, moving, merging, or deleting it.

Research Case decisions may use the actions ADD, KEEP, NARROW, MOVE, MERGE,
SPECIALIZE, DELETE, REDESIGN, or NO CHANGE.

## Claims and ADRs

The Claims Ledger in `state.md` authorizes current SemTL wording. Claims record
scope, exclusions where material, evidence references, counterevidence,
confidence, and review trigger.

Acceptable:

> The tested local successor consumer operated unchanged across the explicit
> stored and single-pass lazy LTS representations.

Unacceptable without much broader evidence:

> SemTL is representation-independent.

An ADR is required only when evidence changes governing architecture. It links
the question, alternatives, experiment, claim state, counterevidence, scope,
and review trigger. A closed experiment that changes no architectural decision
does not create an ADR.

## AI-assisted, human-governed operation

AI assistants may propose cases, hypotheses, prior-art leads, counterexamples,
experiment scaffolding, claim updates, ADR changes, and derived views. A normal
post-experiment proposal should touch only the case, affected claims, and an ADR
if needed.

Humans retain authority over freezes, interpretations, claim acceptance,
governing ADRs, and M02 strategic decisions. For R3, use a separate adversarial
review rather than one uninterrupted AI chain creating and approving its own
evidence.

## Protocol calibration

The first operational pilot is [ARC-001](cases/ARC-001.md). After it closes,
review:

- which fields improved the decision;
- which fields were unused;
- how many authoritative files changed;
- whether the freeze prevented accommodation;
- whether counterevidence remained visible;
- and what can be deleted from this protocol.

Unless the pilot reveals a material governance defect, protocol-design work
then stops and priority remains on architectural research.
