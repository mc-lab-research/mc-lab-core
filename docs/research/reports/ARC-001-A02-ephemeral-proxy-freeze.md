---
case: ARC-001
attempt: A02
report_kind: controlled-attempt-configuration-freeze
date: 2026-08-13
status: FROZEN
evidence_disposition: UNTESTED
evidence_posture: CONTROLLED-TARGET
technical_freeze_commit: f969fa8aa63a9a172e6feb2eeb32636aab24f387
freeze_record_commit: 1dec900c899893537c9f1d87b3b26e272d22808f
attempt_configuration_commit: 2460f4d45f21fce49636d820c07c489d1b4598b6
attempt_configuration_tag: arc-001-a02-config-v1
human_experiment_owner: Sami Lazreg
experimentator: Claude
designer_and_reviewer: Codex
---

# ARC-001 A02 ephemeral-proxy configuration freeze

## Freeze record

Project Principal, Human ARC Owner, and Human Experiment Owner Sami Lazreg
approved configuration commit
`2460f4d45f21fce49636d820c07c489d1b4598b6` on 2026-08-13. That exact SHA is
the immutable A02 configuration authority. Annotated tag
`arc-001-a02-config-v1` is a human-readable aid and must not be moved.

Claude may implement and execute A02 only from the configuration-freeze record
that adds this approval. Any material configuration change requires a new
attempt or configuration version under the Git evidence protocol.

## Authorization and separation

On 2026-08-13, Human Experiment Owner Sami Lazreg authorized a new controlled
attempt after A01 was found nonconforming:

> Preserve the current commits as the nonconforming by-value attempt, correct
> only the lazy subject and negative test, and rerun the controlled experiment.

A01 remains preserved as an independently reviewable owning-value attempt. A02
derives from the ARC-001 freeze record and has no A01 implementation ancestry.
Claude is the experimentator. Codex designed this configuration and will review
the result, but must not implement or execute it.

## Objective

Implement `SUBJECT-LAZY-LTS-01` as a genuinely single-pass range whose
dereference result is an ephemeral proxy. Run the byte-for-byte unchanged
`CON-SUCCESSORS-01` consumer and frozen semantic oracle against it.

The attempt must not change the consumer, stored subject, oracle, reference
relation, public concepts, CPO implementations, expected classifications, or
semantic-integrity and architectural-complexity guardrails.

## Frozen proxy design

The outgoing range may own only the queried source, traversal cursor, current
generated witness storage for one occurrence, and a changing generation token.
It must not own or cache the complete local image or stable occurrence
descriptors.

Dereference returns a small proxy value containing only a non-owning handle to
the range's current-witness state and the generation observed at dereference.
The proxy must not contain copied label or target fields. Copying the proxy may
copy its handle and generation, but must not detach semantic facts from the
range.

Every increment that changes traversal position advances the cursor, clears or
invalidates the current witness, and changes the generation. Earlier proxies
then become stale. A test-only `is_current()` or equivalent generation check may
demonstrate this safely; neither the test nor consumer may call `target` or
`transition_label` on a stale proxy.

The model-local `target` and `transition_label` operations must accept the
actual named-lvalue proxy used by the unchanged consumer and read only current
range-owned state. If the concepts accept the subject but that lvalue call
fails, preserve and classify the failure prospectively under ARC-001.

## Required conformance checks

The attempt must prove at compile time that its outgoing range is an
`input_range`, is not a `forward_range`, `sized_range`, or `borrowed_range`, and
has the proxy as its actual `range_reference_t`. It must also satisfy
`TransitionRelation` and `TransitionLabelling` for the actual proxy form.

At runtime it must safely demonstrate:

```cpp
auto iterator = range.begin();
auto original_proxy = *iterator;
auto iterator_copy = iterator;

assert(original_proxy.is_current());
observe_label(original_proxy);
observe_target(original_proxy);

++iterator_copy;

assert(!original_proxy.is_current());
```

Dereferencing either iterator handle after advancement must observe the new
shared position when it is not at the sentinel. Copying label and target out
does not demonstrate that the proxy itself survived.

The unchanged consumer must produce the frozen multiplicity-sensitive,
order-independent observations:

```text
0: {(advance, 1), (bypass, 2)}
1: {(reset, 0)}
2: {}
```

## Change boundary

Claude may change only:

- `tests/semantic/support/transition_materialization_lazy_model.hpp`;
- `tests/semantic/transition_materialization_lazy_test.cpp`;
- the controlled report and raw reproduction evidence;
- test registration only if a distinct A02 target is necessary.

Claude must not change a frozen consumer, oracle, stored subject, public
concept, or CPO; weaken the proxy or invalidation rule; put label or target in
the proxy; cache the complete local image; add representation branching; or
implement ARC-002.

## First failure and validation

At the first compile-time, runtime, semantic, value-category, lifetime, or
guardrail failure, Claude must stop before repair and preserve the technical
freeze SHA, this configuration SHA, exact A02 revision, frozen-file equality,
environment, command, full diagnostic or mismatch, and neutral observation.
Further repair requires new authorization from Sami Lazreg.

Claude must record successful or failed execution of:

```powershell
cmake --workflow --preset windows-clangcl-commit-gate --fresh
cmake --workflow --preset windows-clangcl-tidy --fresh
```

## Prospective result classification

**Supports H-A within scope:** every proxy, storage, invalidation, range, and
semantic check conforms; the unchanged consumer and oracle pass; and no
prohibited machinery appears.

**Supports H-B or contradicts H-A:** a conforming proxy requires a frozen
consumer, concept, or CPO change; stronger range behavior; persistent storage;
representation branching; or triggers the frozen named-lvalue failure.

**Inconclusive:** the apparatus is not the frozen proxy subject, violates this
configuration, or exposes a separable compiler or apparatus defect.

After execution, Codex reviews ancestry, preservation, frozen-file equality,
proxy structure, invalidation, storage boundaries, independent gate results,
and classification. Sami Lazreg alone assesses evidence, approves Claim impact,
adjudicates ARC-001, and closes the case.
