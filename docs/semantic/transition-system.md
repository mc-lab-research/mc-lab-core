# Transition systems and behavioral semantics

This document defines the minimal transition-system formalism while keeping
its mathematical behaviors separate from concrete execution representations.

## Structure

A transition system is a tuple:

```text
TS = (S, trans, I)
```

where:

- `S` is a set of states;
- `trans` is a binary relation contained in `S x S`;
- `I` is a subset of `S` containing the initial states.

SemTL recognizes this formalism with:

```cpp
template <class System>
concept TransitionSystem =
    InitialStateSet<System>
    && TransitionRelation<System, state_t<System>>;
```

| Mathematical component | SemTL contract |
|---|---|
| state domain `S` | `state_t<System>` plus the documented valid-state laws |
| initial set `I` | `InitialStateSet<System>` |
| relation `trans` | `TransitionRelation<System, state_t<System>>` |

The C++ state type carries the semantic state domain. It neither implies that
every representable C++ value is valid nor enumerates `S`.

`TransitionSystem` deliberately requires no labels, acceptance condition,
finiteness, determinism, global state-space enumeration, predecessor query, or
indexing. Those roles belong to other facets, acceptance conditions,
formalisms, or computational capabilities.

## Paths

For `s` in `S`, `paths(TS, s)` is the set of all non-empty, potentially
infinite sequences:

```text
pi = s0 s1 ...
```

such that:

```text
s0 = s
si trans s(i+1) for every adjacent pair
```

A path may terminate after finitely many states or continue indefinitely. The
basic definition does not require maximality: a finite prefix that obeys the
relation is still a path.

## Executions and behaviors

An execution, also called a behavior, is a path whose first state is initial.
The behavioral semantics of `TS` is the union of `paths(TS, s0)` over all
`s0` in `I`.

Equivalently, a sequence belongs to the semantics exactly when:

1. it is non-empty;
2. its first state belongs to `I`;
3. every consecutive state pair belongs to `trans`.

These are mathematical definitions and laws of the recognized formalism. They
do not require the structural C++ concept to construct or enumerate behaviors.

## Representation and algorithm boundary

SemTL currently publishes no structural `ExecutionRange` concept and no
`execution(system)` or `executions(system)` CPO.

A range is one possible access mechanism for a concrete behavior, but its
observations depend on the formalism and the algorithm. A state-oriented
evolution may be completely represented by a range of states. A
transition-rich execution may retain alternating state and transition
observations so labels, costs, probabilities, or other transition evidence can
later be projected without duplicating them into execution-specific facets.

The v0.1 candidate for such a heterogeneous observation range is
`std::variant<State, Transition>`. It is a deferred representation decision,
not part of the structural semantic API in this increment.

Construction, selection, validation, filtering, and projection of concrete
executions belong to later algorithms and views. Requiring all systems to
enumerate their executions would turn a mathematical power set into an
impossible or undesirable computational obligation for many infinite, lazy,
symbolic, nondeterministic, or composed systems.
