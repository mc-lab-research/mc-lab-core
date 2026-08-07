# Variadic synchronous composition view

This document defines the first SemTL algebraic composition operation. It is a
strict synchronous, or lockstep, product of transition relations. It does not
synchronize transition labels.

## Operands

Let `n >= 2` systems expose only the following structural facets:

```cpp
InitialStateSet<System>
TransitionRelation<System, state_t<System>>
```

No recognized system concept is required. In particular, the operation does
not consume `TransitionSystem`, `KripkeStructure`, or any future automaton
concept. System concepts introduce formalism-specific laws; the algebraic view
needs only the local structures used by the product.

Transition labelling and atomic-proposition labelling are neither required nor
propagated. If an operand happens to expose labels, this operation ignores
them completely.

## Product states and initial states

For state domains `S1, ..., Sn`, the product state domain is:

```text
S1 × ... × Sn
```

The C++ carrier is a flat tuple:

```cpp
std::tuple<state_t<S1>, ..., state_t<Sn>>
```

The initial set is the Cartesian product:

```text
I1 × ... × In
```

The view never enumerates the complete product state domain. A call to
`initial_states()` constructs only the Cartesian product of the initial ranges
returned by the operands.

## Product relation

For a product state:

```text
s = (s1, ..., sn)
```

each component independently chooses one locally enabled transition:

```text
ti in outgoing_i(si)
```

The global witness is the flat tuple:

```text
t = (t1, ..., tn)
```

and its target is:

```text
target(t) = (target_1(t1), ..., target_n(tn))
```

Equivalently, the product relation contains:

```text
(s1, ..., sn) -> (s1', ..., sn')
```

exactly when:

```text
si -> si' for every component i
```

There is no comparison between local transition labels. Every component fires
once during the same global step, but their transition choices are otherwise
independent.

If component `i` exposes `ki` local transitions, the product exposes:

```text
k1 × ... × kn
```

global transitions. This combinatorial growth is part of the mathematical
product, not an artifact of the variadic implementation.

If one component has no outgoing transition, the Cartesian product is empty
and the global state is blocked. The view does not insert implicit stuttering
or self-loops.

## Public operation and view

The variadic construction operation is:

```cpp
auto product = synchronous_compose(system_1, system_2, ..., system_n);
```

It returns:

```cpp
synchronous_composition_view<System1, System2, ..., SystemN>
```

At least two operands are required. The implementation constructs one flat
variadic view; it does not fold a binary operation and therefore does not
produce nested product states.

## Ownership and lifetime

The v0.1 view is non-owning. It stores pointers to the supplied systems and
accepts lvalues only:

```cpp
auto product = synchronous_compose(left, right);
```

Temporary operands are rejected because they would leave dangling pointers:

```cpp
// Rejected by the function signature.
auto product = synchronous_compose(make_left(), make_right());
```

Every operand must outlive the composition view and all observations made
through it.

## Local materialization

The view does not materialize the global state space or transition graph.
However, v0.1 locally materializes the result of each call to:

```cpp
initial_states(product)
outgoing_transitions(product, state)
```

as an owning range of stable tuple values. This prevents returned product
witnesses from dangling when an operand range yields ephemeral references or
proxies.

Local materialization allocates storage and may propagate allocation failures.
Neither observation is specified as `noexcept`; callers that require an error
boundary must handle exceptions according to their own execution environment.

Consequently, the first implementation requires component state and transition
value types to be copy constructible. This is a representation restriction of
the view, not a semantic requirement of synchronous composition or of the
component facets.

A future cartesian-product range may relax the restriction after its iterator,
proxy, and lifetime contracts are designed explicitly.

## Deliberate non-goals

This increment does not provide:

- transition-label synchronization;
- interleaving transitions;
- synchronization participant groups;
- implicit stuttering for blocked components;
- propagation or union of atomic propositions;
- a materialized global state space;
- a recognized system concept for the result.

Later algebraic views may add those operations independently without changing
the minimal direct-product semantics defined here.
