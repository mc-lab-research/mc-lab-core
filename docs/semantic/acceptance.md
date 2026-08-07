# Acceptance conditions

Acceptance connects a system's structural transition model to an
interpretation of its finite or infinite behaviors. It is therefore neither a
structural facet nor a computational capability.

```text
system structure
    -> associated acceptance condition
    -> interpretation of a finite or infinite run
```

## Association owned by the system

A system exposes its condition object through the member-backed CPO:

```cpp
acceptance_condition(system)
```

The corresponding public vocabulary is:

```cpp
HasAcceptanceCondition<System>
acceptance_condition_t<System>
```

`HasAcceptanceCondition` establishes only that a logically const system is
associated with an object. It deliberately says nothing about the object's
acceptance semantics. The CPO may return an owned value or a reference into
the system; generic code preserves that result category.

## Semantics owned by the condition object

Condition concepts apply to `acceptance_condition_t<System>`, not directly to
`System`:

```cpp
FinalStateAcceptanceCondition<Condition, State>
BuchiAcceptanceCondition<Condition, State>
```

Both current conditions use an accepting-state set and therefore share the
structural contract:

```cpp
AcceptingStateSet<Condition, State>
```

The public operation is consequently:

```cpp
accepting_states(condition)
```

It is intentionally not `accepting_states(system)`. A finite automaton
interprets membership in that set at the final state of a finite execution. A
Buchi automaton interprets the same shape recurrently over an infinite run.
Their structural representation can coincide while their behavioral laws
remain distinct.

## Not every condition is an accepting-state set

`AcceptingStateSet` is a reusable representation contract, not the universal
foundation of acceptance. Future condition concepts may require richer local
structure:

- generalized Buchi: a family of accepting sets;
- parity: a priority assignment;
- Rabin: pairs of state sets;
- Streett: request-response pairs of state sets.

Those objects will still be associated with a system through the same
`acceptance_condition` CPO. They need not provide `accepting_states`.

## Laws and computation

The concepts validate availability and type compatibility. They cannot prove:

- that every accepting state belongs to the associated system's state domain;
- that a range denotes exactly the mathematical accepting set;
- that a finite execution ends in an accepting state;
- that a Buchi run visits an accepting state infinitely often;
- that a word has an accepting execution.

The first two are semantic conformance laws. The remaining properties belong
to behavior interpretation and algorithms. None justifies adding enumeration
or search operations to the acceptance-condition concepts.
