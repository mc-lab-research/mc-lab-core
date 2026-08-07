# Kripke structures, paths, and traces

This document defines the first state-labelled SemTL system and distinguishes
its structural contract from its behavioral laws.

## Structure

A Kripke structure is a tuple:

```text
K = (S, R, I, AP, L)
```

where:

- `S` is the state domain;
- `R` is a binary relation contained in `S × S`;
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

`KripkeStructure` is a system concept, not a new facet and not a base class.
It composes the three structural facets directly. It does not use
`TransitionSystem` as a prerequisite and does not require transition labels:
Kripke transitions carry neither actions nor symbols.

This direct composition is semantically important. A recognized system is not
merely shorthand for a conjunction of facets: it also introduces laws and a
behavioral interpretation. Reusing another system concept would therefore
suggest specialization of that formalism and inheritance of its laws. SemTL
instead lets each system compose the facets it needs and state its own laws.

## Totality law

The relation of a Kripke structure is total:

```text
for every s in S, there exists t in S such that s R t
```

This is a semantic law rather than a facet. It exposes no new operation and
cannot generally be checked from the type alone. A future conformance utility
may verify it when a concrete representation also offers an enumerable state
space, but enumeration is not part of `KripkeStructure`.

Totality means that every finite path fragment can be extended. Consequently,
every maximal execution is infinite. It does not make finite prefixes invalid.

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

Thus:

```text
Paths_K(s) = {
    pi in S^omega |
    pi(0) = s and pi(i) R pi(i+1) for every i
}
```

A finite sequence obeying the adjacency condition is a finite path fragment,
not a complete Kripke path under this convention.

`KripkePathRange<System, Path>` checks that a range can carry states from the
same domain as a Kripke structure. A particular value represents a path only
if it is infinite and obeys the state-validity and adjacency laws.

SemTL does not identify infinity with `std::unreachable_sentinel_t`. That
sentinel is useful for some lazy ranges, but other representations can also
denote infinite mathematical objects.

## Initial executions

An execution of the whole structure is a path whose first state is initial:

```text
Exec(K) = union of Paths_K(s0), for every s0 in I
```

Equivalently:

```text
Exec(K) = {
    pi in S^omega |
    pi(0) is in I and pi(i) R pi(i+1) for every i
}
```

`KripkeExecutionRange<System, Execution>` gives this role a distinct public
name. It shares the compile-time range requirements of `KripkePathRange`; the
initial-state requirement is a semantic law over a concrete range value.

## Traces

The path contains states. Applying `L` pointwise produces its observable trace:

```text
trace_K(pi) = L(pi(0)) L(pi(1)) L(pi(2)) ...
```

This trace is an omega word over `2^AP`:

```text
trace_K(pi) is in (2^AP)^omega
```

Different state paths may produce the same trace. SemTL therefore keeps paths
and traces as distinct behavioral objects. This increment defines path
carriers but does not yet introduce a trace-projection CPO or an `OmegaWord`
representation.

## What the compiler can and cannot establish

The concepts establish:

- availability and type compatibility of the structural facets;
- one shared normalized state domain;
- range-shaped access to path states.

The following remain semantic laws:

- totality of `R`;
- validity of each state value;
- adjacency of consecutive path states;
- infinitude of a complete path;
- membership of an execution's first state in `I`;
- correctness of the trace induced by `L`.

These laws belong to documentation and future conformance utilities. They do
not justify adding state enumeration or behavior enumeration to the system
concept.
