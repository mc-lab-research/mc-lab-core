---
id: ARC-###
title: Replace with a discriminating architectural question
tier: R2
lifecycle: OPEN
evidence_disposition: UNTESTED
evidence_posture: EXPLORATORY
freeze: NOT-FROZEN
human_owner: unassigned
human_experiment_owner: unassigned
human_challenger: unassigned
---

# ARC-### - Title

Use only the sections that materially protect evidence, clarify responsibility,
or enable the decision. Keep case-specific technical detail here rather than
creating parallel registries or administrative objects.

## Human ownership and AI support

**Human ARC Owner:** Name the human accountable for the case. An unassigned
case cannot be frozen or closed.

**Human Experiment Owner:** Name the human responsible for controlled
execution, frozen-contract compliance, stopping conditions, and evidence
preservation. An unassigned experiment cannot enter controlled execution.

**Independent Human Challenger:** Assign according to research significance.
AI-generated challenge material may assist this person but does not satisfy the
independence requirement.

**AI advisory support:** Optionally identify material assistance from GPT Work,
Codex, Claude, or another AI system. This records provenance, never ownership,
responsibility, approval, or adjudication. Every AI contribution remains a
proposal until accepted by the applicable named human.

## Question and decision

**Question:**

**Why it matters:**

**Decision enabled:**

**Scope and exclusions:**

**Properties under investigation:** Name only the applicable P1-P11 properties
and state their operational meaning in this case.

## Competing hypotheses

### H-A - Name

**Statement and mechanism:**

### H-B - Name

**Statement and mechanism:**

Add another hypothesis only when it changes the discriminator or decision.

## Experimental design

**Subjects and materializations:**

**Consumer and semantic purpose:**

**Minimum requirements:**

**Explicit non-requirements:**

**Technical discriminator:**

**Semantic-integrity guardrails:**

**Architectural-complexity guardrails:**

### Expected observations

**Supports H-A when:**

**Supports H-B when:**

**Inconclusive when:**

## Lightweight freeze

The freeze lives here and uses Git as its immutable reference.

**Freeze commit:** PENDING.

**Frozen consumer:**

**Frozen oracle:**

**Frozen requirements:**

**Explicit non-requirements:**

**Permitted adaptations:**

**Prohibited adaptations:**

**Status:** NOT-FROZEN.

**Human ARC Owner assessment:** PENDING.

**Project Principal approval:** PENDING.

## Experiment and evidence

**Procedure:**

**Evidence and reproduction locations:**

### First Failure Preservation Rule

When the controlled discriminator first fails, the Human Experiment Owner must
stop controlled execution and preserve the original frozen configuration,
source revisions, command, environment, exact diagnostic or semantic mismatch,
raw output, smallest faithful reproduction, and observation before repair or
redesign. AI support may assist preservation but cannot authorize repair.

### Observation

PENDING.

### First discriminating failure

PENDING.

### Counterevidence and unexpected observations

PENDING.

## Challenge and interpretation

**Independent Human Challenger:** Assign according to research significance.
Independent human challenge is mandatory for R3 and for an R2 result that could
materially affect an architectural Claim. An AI-only review does not satisfy
this requirement.

**Could another explanation produce the result?**

**Was the experiment discriminating and the hold-out genuinely adversarial?**

**Were semantics preserved, the consumer unchanged, and hidden
canonicalization absent?**

**Interpretation:**

**Credible alternative interpretation:**

## Claim impact and decision

**Affected Claim and demonstrated domain:**

**What remains unproven:**

**Claim strengthened, narrowed, contradicted, or unchanged:**

**Architectural action:** KEEP / NARROW / MOVE / MERGE / SPECIALIZE / DELETE /
REDESIGN / INCONCLUSIVE

**ADR required:** Only when evidence establishes or changes governing
architecture.

**Next question:**
