# Direct successor-state semantics with `Post`

`Post<System, State>` is the atomic structural facet for discrete evolution
expressed directly as successor states:

```text
State -> Range<State>
```

It complements rather than replaces the transition-oriented facet:

```text
TransitionRelation
    State -> Range<Transition>
    Transition -> target State
```

## Independent semantic information

A model may expose `Post`, `TransitionRelation`, or both. Neither concept
refines the other.

For a deterministic map such as `f(x) = x + 1`, the natural model is:

```text
post(0) = {1}
post(1) = {2}
post(2) = {3}
```

Inventing transition descriptors would add no semantic information. The TDD
fixture `iterated_map` therefore exposes `InitialStateSet` and `Post`, but no
`TransitionRelation`.

`Post` remains range-based so it also represents nondeterminism. The
`generated_branching_system` fixture lazily generates:

```text
post(0) = {1, 2}
post(1) = {3}
post(2) = {3, 4}
post(3) = {}
post(4) = {}
```

No transition witness is introduced by either fixture.

## Evidence preserved by `TransitionRelation`

For:

```text
q0 --a--> q1
q0 --b--> q1
```

`TransitionRelation` may expose two distinct witnesses while `Post(q0)` denotes
the singleton successor set `{q1}`. Transition witnesses preserve evidence
that direct successor states intentionally omit, including labels and future
weights, costs, probabilities, durations, or rewards.

A future view may project `TransitionRelation` through `target` to provide
`Post`. That adapter is optional and does not redefine either facet. The
reverse direction cannot reconstruct arbitrary lost transition evidence.

## Coherence when both facets exist

If one model exposes both facets, it obeys the semantic set-equivalence law:

```text
post(system, source)
    = {
          target(system, transition)
          | transition in outgoing_transitions(system, source)
      }
```

This is not ordered range equality. Transition multiplicity may collapse when
several witnesses share one target, and iteration order has no semantic
meaning.

The law is documented now but has no generic checker in this increment. A
future utility belongs under `semantic/conformance/` and will require explicit
computational support for finite materialization and state equality or
indexing.

## Not a system or capability

`Post` is a local semantic facet. It does not introduce initial states,
behavioral semantics, or a recognized system formalism. A possible
`SuccessorSystem` remains deferred until concrete behavioral laws justify that
public system concept.

Likewise, `Post` is not a computational capability in SemTL's taxonomy.
Capabilities describe representation access such as state-space enumeration,
indexing, or predecessor queries. `Post` describes the formal local evolution
itself.

## Execution boundary

SemTL currently publishes no C++ execution carrier. Mathematical executions
remain documented with their formalisms, while construction, selection,
validation, filtering, masking, and persistence belong to later algorithms and
views.

For a future transition-rich execution representation, the current v0.1
direction is one observation range alternating states and transition witnesses
with `std::variant<State, Transition>`. Labels, weights, costs, and
probabilities will be projected from the retained transitions rather than
introduced as separate execution facets. This deferred choice does not affect
the structural `Post` contract.
