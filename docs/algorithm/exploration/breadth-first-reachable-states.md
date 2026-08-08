# Breadth-first reachable-state enumeration

`breadth_first_reachable_states(system)` materializes the states reachable
from a system's initial-state set. It is the first algorithm built across the
SemTL architectural boundary:

```text
semantic structure -> exploration algorithm -> owned result representation
```

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

`state_t<System>` must be copy-constructible and equality-comparable.

Copy construction is required because the returned vector owns its states.
The algorithm also copies the state currently being expanded before appending
new states: a `std::vector` reallocation must not invalidate a source observed
by a lazy `post` or outgoing-transition range.

Equality defines computational state identity. If two values denote the same
formal state, they must compare equal for exploration to collapse them.

The v0.1 implementation uses a linear search in the discovered vector. For
`I` enumerated initial values, `V` reachable states, and `E` enumerated local
successor occurrences, this intentionally simple registry may require
`O((I + E) * V)` equality comparisons in the worst case. Hashing, indexing,
symbolic registries, and caller-supplied discovery policies remain future
algorithmic capabilities.

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
intentionally discarded after their targets are observed. That is correct for
a result containing only reachable states.

Future algorithms producing paths, executions, traces, or counterexamples
must retain transition evidence. They may therefore require
`TransitionRelation` even when `Post` is also available.
