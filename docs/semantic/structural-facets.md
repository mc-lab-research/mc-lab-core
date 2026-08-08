# SemTL structural facets

This document defines the first representation-independent contracts of the
Semantic Template Library. A structural facet describes a local piece of the
formal model. It is not a storage format, a graph container, a runtime
interface, or membership in a nominal class hierarchy.

## Public namespace and headers

All public symbols use the flat namespace `mc_lab::semantic`. The `facet/`
directory organizes headers; it is not part of that namespace.

```cpp
#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/post.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>
#include <mc_lab/semantic/facet/transition_labelling.hpp>
#include <mc_lab/semantic/facet/atomic_proposition_labelling.hpp>
```

## CPO architecture in v0.1

SemTL exposes semantic operations as stateless, `inline constexpr`
customization-point objects (CPOs). Generic code always calls the public CPO:

```cpp
auto&& initial = mc_lab::semantic::initial_states(system);
auto&& outgoing =
    mc_lab::semantic::outgoing_transitions(system, state);
auto&& destination = mc_lab::semantic::target(system, transition);
```

For v0.1, each CPO has exactly one dispatch route: a model member operation.

| Public CPO call | Member selected by the v0.1 implementation |
|---|---|
| `initial_states(system)` | `system.initial_states()` |
| `post(system, state)` | `system.post(state)` |
| `outgoing_transitions(system, state)` | `system.outgoing_transitions(state)` |
| `target(system, transition)` | `system.target(transition)` |
| `transition_label(system, transition)` | `system.transition_label(transition)` |
| `atomic_propositions(system, state)` | `system.atomic_propositions(state)` |

The direct member calls are confined to the CPO implementations under
`include/mc_lab/semantic/detail/`. Concepts, algorithms, views, tests, and user
code use the public objects. Consequently, the public semantic vocabulary does
not depend on the current dispatch policy.

There is deliberately no ADL fallback, `tag_invoke`, user-specialized traits
class, inheritance requirement, or virtual dispatch in this increment. If a
second customization route becomes useful later, it can be implemented behind
the existing CPOs without changing their callers. That is an extension point,
not a v0.1 feature.

### Model-side protocol

A model may provide only the facets that describe it. A model exercising all
current structural operations has the following shape:

```cpp
struct model {
    auto initial_states() const;
    auto post(const state& source) const;
    auto outgoing_transitions(const state& source) const;
    auto target(const transition& witness) const;
    auto transition_label(const transition& witness) const;
    auto atomic_propositions(const state& value) const;
};
```

These declarations are not a required base class. Their actual parameter and
result types follow from the model's ranges and the facet contracts below.

### Forwarding contract

Every CPO uses a constrained call operator and `std::forward`. It therefore
preserves:

- the cv/ref qualification of the system and operation arguments;
- lvalue and rvalue categories;
- references in the member's result through `decltype(auto)`;
- the member operation's exception specification through conditional
  `noexcept`.

Facet concepts observe their systems through `const System&`. A model that
claims a structural facet must therefore make the corresponding observation
available on a logically const system. The CPO itself still forwards the exact
category supplied by other callers.

### Type normalization is not traits customization

SemTL uses standard transformations such as `std::remove_cvref_t` and standard
range aliases such as `std::ranges::range_value_t`. They normalize expression
types and inspect the ranges actually returned by CPO calls. They do not form a
`semantic_traits` customization mechanism and cannot make a type satisfy a
facet when the public operation is unavailable.

## `InitialStateSet<System>`

Public operation:

```cpp
mc_lab::semantic::initial_states(system)
```

The result must model `std::ranges::input_range`. It may be an owning range, a
borrowed view into system storage, or a generated single-pass range.

Associated types:

```cpp
initial_state_range_t<System>
initial_state_reference_t<System>
state_t<System>
```

`state_t<System>` is the range value type, so it remains well-defined when a
runtime range is empty.

Semantic laws:

- the range denotes exactly the formal initial-state set;
- every element denotes a valid state;
- order has no semantic meaning;
- duplicate occurrences do not change the represented set.

This facet neither exposes nor implies enumeration of the complete state
space.

## `Post<System, State>`

Public operation:

```cpp
mc_lab::semantic::post(system, state)
```

`Post` exposes direct state-oriented evolution:

```text
State -> Range<State>
```

The explicit `State` argument keeps the facet independent from
`InitialStateSet<System>`. The result must be an `std::ranges::input_range`
whose normalized value type is exactly `State`. It may be empty, owning,
borrowed, lazy, generated, or single-pass.

Associated types:

```cpp
post_range_for_t<System, State>
post_reference_for_t<System, State>
```

Semantic laws for a query from `source`:

- every value denotes a valid successor state of `source`;
- the range is sound and complete for the represented successor set;
- order has no semantic meaning;
- duplicate occurrences do not change the represented set.

`Post` supports zero, one, or many successors and implies neither determinism
nor global state-space enumeration. It creates no transition witness.

`Post` and `TransitionRelation` are independent atomic facets. A model may
expose either or both. When both exist, their successor sets obey a documented
coherence law; see [post.md](post.md).

## `TransitionRelation<System, State>`

Public operations:

```cpp
mc_lab::semantic::outgoing_transitions(system, source)
mc_lab::semantic::target(system, witness)
```

The explicit `State` concept argument keeps the relation independent from
`InitialStateSet<System>`. An algorithm that already has a state can inspect
local transition witnesses without requiring initial-state access or global
state-space enumeration.

`outgoing_transitions` returns an `std::ranges::input_range` of local transition
witnesses. A witness describes one occurrence in the relation; it is not the
target state itself.

Associated types:

```cpp
outgoing_transition_range_t<System, State>
transition_reference_for_t<System, State>
transition_value_for_t<System, State>
target_result_t<System, State>
```

`transition_reference_for_t` is the exact type produced by dereferencing the
range iterator. Despite its name, this type may be a reference, a value, or a
range proxy. `target` is checked against that exact form.

`transition_value_for_t` is the stable value type reported by the range. The
distinction prevents the concept from materializing or copying a witness merely
to inspect its target.

For v0.1, removing cv/ref qualifiers from `target_result_t<System, State>` must
produce exactly the normalized `State` type. Consequently, `State`, `State&`,
`const State&`, and `State&&` results are compatible. A distinct type that is
merely convertible to `State` is rejected. Proxy-state support may be designed
later if a concrete model demonstrates that need.

Semantic laws for a query from `source`:

- every returned witness denotes a formal transition whose source is
  `source`;
- `target(system, witness)` denotes that transition's formal target;
- the range is sound and complete for the represented local image.

The concept does not require a `source` operation because the source is already
known from the `outgoing_transitions` query.

## Transition witnesses and lifetime

A range may expose stored witnesses by reference or generate witnesses by
value. It may also be single-pass. SemTL therefore distinguishes:

- a **transition witness**, valid while consumed according to its producing
  range and iterator contracts;
- persistent **transition evidence**, suitable for executions, predecessor
  maps, diagnostics, or counterexamples.

The structural relation facet defines witnesses only. It does not guarantee
that a witness remains valid after its owning range is destroyed or after a
single-pass iterator advances.

Generic code should preserve an operation's result category:

```cpp
decltype(auto) outgoing =
    mc_lab::semantic::outgoing_transitions(system, state);

for (auto&& witness : outgoing) {
    decltype(auto) destination = mc_lab::semantic::target(
        system, std::forward<decltype(witness)>(witness));
}
```

Using plain `auto` for an operation that returns a borrowed range by reference
may copy that result.

## `TransitionLabelling<System, Transition>`

Public operation:

```cpp
mc_lab::semantic::transition_label(system, witness)
```

`Transition` should normally be the exact reference, value, or proxy produced
by an outgoing-transition range. The normalized result must be an object type;
an accidental `void` operation does not define a semantic label.

The name is deliberately neutral. Recognized system concepts may later expose
domain aliases such as:

```cpp
action_t<LabelledTransitionSystem>
symbol_t<FiniteAutomaton>
```

Both aliases refer to the same structural label type. They do not add separate
`action(...)` or `symbol(...)` customization mechanisms.

Semantic law:

- the result denotes exactly the formal label carried by the witness.

## Facets remain atomic

SemTL deliberately defines no `LabelledTransitionRelation` composite. There is
one transition relation; labels, weights, costs, probabilities, durations, and
other observations may independently describe its witnesses. Creating one
relation concept per combination would privilege labels and cause a
combinatorial family of weak shortcuts.

An algorithm or recognized system that needs labelled transitions states the
two atomic requirements directly:

```cpp
TransitionRelation<System, State>
    && TransitionLabelling<
           System,
           transition_reference_for_t<System, State>>
```

The same rule applies when future facets observe weights or probabilities.
Cross-facet laws, such as normalization of outgoing probabilities, belong to
the recognized system and its conformance contract rather than to a composite
relation facet.

## `AtomicPropositionLabelling<System, State>`

Public operation:

```cpp
mc_lab::semantic::atomic_propositions(system, state)
```

The result must model `std::ranges::input_range`. It represents the formal
state-labelling function:

```text
L : State -> 2^AP
```

Associated types:

```cpp
atomic_proposition_range_t<System, State>
atomic_proposition_for_t<System, State>
```

Semantic laws:

- the range represents exactly `L(state)`;
- order has no semantic meaning;
- duplicate occurrences do not change the represented proposition set.

This operation is intentionally distinct from `transition_label`.

## What CPOs and concepts do not prove

A CPO standardizes access to an operation. A concept validates expressions and
type relationships. Neither proves:

- soundness or completeness of a transition relation;
- validity of every initial or target state;
- correctness of state or transition labelling;
- witness lifetime beyond the producing range's guarantees;
- finiteness, determinism, or computational complexity;
- set equality or absence of duplicate range elements.

These properties remain documented semantic laws. Reference models can receive
law-oriented runtime tests, and reusable conformance utilities may later test
laws when a representation supplies the required computational capabilities.
Those capabilities remain separate from the structural facets.
