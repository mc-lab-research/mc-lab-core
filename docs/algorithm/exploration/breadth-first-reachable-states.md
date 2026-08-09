# Breadth-first reachable-state enumeration

`breadth_first_reachable_states(system)` materializes the states reachable
from a system's initial-state set. It is the first algorithm built across the
SemTL architectural boundary:

```text
semantic structure -> exploration algorithm -> owned result representation
```

It is a thin, concrete assembly of the reusable pieces documented in
[generic-exploration.md](generic-exploration.md): the `explore_states`/
`explore_transition_relation` kernels, the `fifo_frontier` and
`linear_visited_registry` policies, and a no-op observer. This document
covers what is specific to this one convenience API; the callback order,
policy protocols, lifetime rules, and complexity of the underlying pieces
are specified once, in that other document, not repeated here.

Include it with:

```cpp
#include <mc_lab/algorithm/exploration/breadth_first_reachable_states.hpp>
```

Then call:

```cpp
const auto reachable =
    mc_lab::algorithm::exploration::breadth_first_reachable_states(system);
```

## Minimal structural inputs

The system must expose `InitialStateSet`. Local evolution may be supplied in
either of two independent forms:

```text
InitialStateSet + Post
InitialStateSet + TransitionRelation
```

With `Post`, successor states are consumed directly:

```cpp
post(system, source)
```

Without `Post`, transition witnesses are projected to successor states:

```cpp
for (auto&& transition : outgoing_transitions(system, source)) {
    auto&& successor = target(system, transition);
}
```

When both facets are available, the algorithm deliberately selects `Post`.
It is the most direct observation for a state-only result and avoids an
ambiguous overload. This priority does not introduce refinement between the
facets.

A conforming model that exposes both facets obeys their set-equivalence law:

```text
post(system, source)
    = {
          target(system, transition)
          | transition in outgoing_transitions(system, source)
      }
```

The two observations may enumerate their elements in different orders and a
transition relation may contain several witnesses with the same target.

## Result and discovery order

The result is:

```cpp
std::vector<state_t<System>>
```

Every discovered state occurs once according to state equality. States appear
in breadth-first discovery order:

1. distinct initial states, in their supplied order;
2. distinct states first reached at depth one;
3. distinct states first reached at depth two;
4. and so on.

This order is useful computationally but is not part of the source system's
mathematical semantics. In particular, iteration order and duplicate
occurrences in structural set ranges have no semantic meaning.

## Reachability law

When the algorithm terminates, its vector represents the least set `R` such
that:

```text
initial_states(system) is a subset of R
```

and:

```text
source in R and successor in post(source) implies successor in R
```

For a transition relation, `successor` is the target of an outgoing
transition. Equivalently, `R` is the least fixed point:

```text
R = I union successors(R)
```

## Computational requirements

`state_t<System>` must be copy-constructible and equality-comparable. Both
requirements come from the concrete policies this algorithm fixes —
`fifo_frontier` and `linear_visited_registry` — not from the generic kernels
themselves, which impose nothing beyond what the policy they are actually
given requires. See each policy's own documentation, and
[generic-exploration.md](generic-exploration.md), for exactly why.

Equality defines computational state identity. If two values denote the same
formal state, they must compare equal for exploration to collapse them.

`linear_visited_registry`'s linear-scan complexity, and why its recorded
order already is breadth-first discovery order (so this algorithm reads its
result back from the registry instead of accumulating a second copy), are
documented in [generic-exploration.md](generic-exploration.md). Hashing,
indexing, symbolic registries, and caller-supplied discovery policies remain
future algorithmic capabilities, available by driving `explore_states`/
`explore_transition_relation` directly with a different `VisitedRegistry`.

## Termination

The algorithm terminates when:

- the reachable state set is finite;
- the initial-state range terminates;
- every consumed successor or outgoing-transition range terminates.

Neither `InitialStateSet`, `Post`, nor `TransitionRelation` promises finiteness.
These are preconditions of complete materialized exploration, not laws of the
structural facets.

## Transition evidence

When exploration uses `TransitionRelation`, transition witnesses are
intentionally discarded after their targets are observed: the no-op observer
this algorithm passes to `explore_transition_relation` records nothing. That
is correct for a result containing only reachable states.

Future algorithms producing paths, executions, traces, or counterexamples
must retain transition evidence. They may therefore drive
`explore_transition_relation` directly, with an observer that keeps what it
is given, even on a system where `Post` is also available — see
[generic-exploration.md](generic-exploration.md)'s closing section.
