# Kripke structures, paths, and traces

This document defines the state-labelled Kripke formalism and distinguishes
its structural contract from its mathematical behaviors.

## Structure

A Kripke structure is a tuple:

```text
K = (S, R, I, AP, L)
```

where:

- `S` is the state domain;
- `R` is a binary relation contained in `S x S`;
- `I` is the set of initial states;
- `AP` is the domain of atomic propositions;
- `L : S -> 2^AP` labels each state with the propositions true there.

SemTL recognizes this composition with:

```cpp
template <class System>
concept KripkeStructure =
    InitialStateSet<System>
    && TransitionRelation<System, state_t<System>>
    && AtomicPropositionLabelling<System, state_t<System>>;
```

`KripkeStructure` is a system concept, not a facet or base class. It composes
its atomic facets directly and does not refine `TransitionSystem`. A recognized
formalism introduces its own semantic laws and behavioral interpretation, so
sharing structure does not establish specialization between systems.

## Totality law

The relation is total:

```text
for every s in S, there exists t in S such that s R t
```

Totality is a semantic law rather than a facet. It exposes no new operation and
cannot generally be checked from the type alone. A future conformance utility
may verify it when a concrete representation also offers the required finite
enumeration capabilities.

Totality means every finite path fragment can be extended. Consequently, every
maximal execution is infinite; finite prefixes remain valid fragments.

## Paths

A complete path starting in `s` is an infinite state sequence:

```text
pi = s0 s1 s2 ...
```

such that:

```text
s0 = s
si R s(i+1) for every i in N
```

A finite sequence satisfying adjacency is a finite path fragment, not a
complete Kripke path under this convention.

## Initial executions

An execution of the complete structure is a path whose first state belongs to
`I`. Equivalently:

```text
Exec(K) = {
    pi in S^omega |
    pi(0) is in I and pi(i) R pi(i+1) for every i
}
```

These paths and executions are mathematical behaviors. SemTL currently
publishes no C++ path or execution carrier for them. Concrete generation,
selection, persistence, and validation belong to later algorithms and
representations.

## Traces

Applying `L` pointwise to a path produces its observable trace:

```text
trace_K(pi) = L(pi(0)) L(pi(1)) L(pi(2)) ...
```

The trace is an omega word over `2^AP`. Different state paths may produce the
same trace, so paths and traces remain distinct mathematical objects. This
structural increment introduces neither a path representation nor a
trace-projection CPO.

## What the compiler can and cannot establish

The concept establishes availability and type compatibility of the three
structural facets over one normalized state domain.

The following remain semantic laws:

- totality of `R`;
- validity of every state value;
- adjacency of consecutive path states;
- infinitude of a complete path;
- initiality of an execution's first state;
- correctness of the trace induced by `L`.

These laws belong to documentation and future conformance utilities. They do
not justify state-space or behavior enumeration in the system concept.
