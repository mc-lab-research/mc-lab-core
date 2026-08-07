# Transition systems and behavioral semantics

This document defines the first recognized SemTL system and connects its C++
contracts to the mathematical notions of paths, executions, and behaviors.

## Transition-system structure

A transition system is a tuple:

```text
TS = (S, trans, I)
```

where:

- `S` is a set of states;
- `trans` is a binary transition relation contained in `S × S`;
- `I` is a subset of `S` containing the initial states.

SemTL recognizes this formalism with:

```cpp
template <class System>
concept TransitionSystem =
    InitialStateSet<System>
    && TransitionRelation<System, state_t<System>>;
```

The mapping is:

| Mathematical component | SemTL contract |
|---|---|
| state domain `S` | `state_t<System>` plus the documented valid-state laws |
| initial set `I` | `InitialStateSet<System>` |
| relation `trans` | `TransitionRelation<System, state_t<System>>` |

The C++ state type is a carrier for the semantic state domain. It does not imply
that every bit pattern or constructible value denotes a valid state, and it
does not enumerate `S`.

Consequently, `TransitionSystem` does not require:

- transition or state labels;
- acceptance semantics;
- finiteness;
- deterministic transitions;
- global state-space enumeration;
- predecessor or index access.

Those roles belong to other facets, acceptance conditions, system formalisms,
or computational capabilities.

## Paths

For a state `s` in `S`, `paths(TS, s)` is the set of all non-empty,
potentially infinite sequences:

```text
π = s0 s1 ...
```

such that:

```text
s0 = s
si trans s(i+1) for every adjacent pair
```

A path may terminate after finitely many states or continue indefinitely. The
basic definition does not require a maximal path: a finite prefix that obeys
the relation is still a path.

## Executions and behaviors

An execution, also called a behavior, is a path whose first state is initial.
The behavioral semantics of `TS` is:

```text
⟦TS⟧ = ⋃ paths(TS, s0), for every s0 in I
```

Equivalently, a sequence belongs to `⟦TS⟧` exactly when:

1. it is non-empty;
2. its first state belongs to `I`;
3. every consecutive state pair belongs to `trans`.

## C++ execution carriers

`ExecutionRange<Execution, State>` describes a range-shaped carrier for a
potentially finite or infinite state sequence:

```cpp
template <class Execution, class State>
concept ExecutionRange =
    std::ranges::input_range<Execution>
    && std::same_as<
        std::remove_cvref_t<execution_state_t<Execution>>,
        std::remove_cvref_t<State>>;
```

`TransitionSystemExecutionRange<System, Execution>` connects that carrier to
the state domain of a transition system:

```cpp
template <class System, class Execution>
concept TransitionSystemExecutionRange =
    TransitionSystem<System>
    && ExecutionRange<Execution, state_t<System>>;
```

These concepts validate only type-level compatibility. They do not prove that
a particular range value belongs to `⟦TS⟧`. Non-emptiness, initiality, and
transition adjacency are semantic laws over values.

This distinction is the same one used by structural facets: C++ concepts check
available expressions and type relationships, while documentation and future
conformance utilities address semantic laws.

## Finite and infinite representations

A finite container can carry a finite execution:

```cpp
std::array<int, 3> finite{0, 1, 2};
```

A lazy range with an unreachable sentinel can carry an infinite execution:

```cpp
auto infinite = std::views::iota(0);
```

Both can satisfy `ExecutionRange<..., int>`. Whether their values form valid
executions depends on the associated transition system.

No `executions(system)` CPO is defined. Requiring such an operation would turn
the mathematical power set of behaviors into an enumeration obligation, which
is impossible or undesirable for many infinite, lazy, symbolic, or composed
systems. Exploration and conformance algorithms may provide computational
access separately when a representation offers sufficient capabilities.
