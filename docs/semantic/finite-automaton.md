# Finite automata and their language

This document defines the third recognized SemTL system, `FiniteAutomaton`,
and connects its C++ contracts to the mathematical notion of an accepted
language.

## Structure

A finite automaton is a tuple:

```text
A = (Q, Sigma, delta, Q0, F)
```

where:

- `Q` is a set of states;
- `Sigma` is the alphabet;
- `delta` is contained in `Q x Sigma x Q`, the transition relation;
- `Q0` is a subset of `Q` containing the initial states;
- `F` is a subset of `Q` containing the accepting states.

SemTL recognizes this composition with:

```cpp
template <class System>
concept FiniteAutomaton =
    InitialStateSet<System>
    && TransitionRelation<System, state_t<System>>
    && TransitionLabelling<
        System,
        transition_reference_for_t<System, state_t<System>>>
    && HasAcceptanceCondition<System>
    && FinalStateAcceptanceCondition<
        acceptance_condition_t<System>,
        state_t<System>>;
```

| Mathematical component | SemTL contract |
|---|---|
| state domain `Q` | `state_t<System>` plus the documented valid-state laws |
| alphabet `Sigma` | `symbol_t<System>`, an alias over `transition_label_for_t` |
| initial set `Q0` | `InitialStateSet<System>` |
| relation `delta` | `TransitionRelation<System, state_t<System>>` |
| symbols on `delta` | `TransitionLabelling<System, transition_reference_for_t<...>>` |
| acceptance association | `HasAcceptanceCondition<System>` |
| final-state interpretation | `FinalStateAcceptanceCondition<acceptance_condition_t<System>, state_t<System>>` |

Like `KripkeStructure`, `FiniteAutomaton` composes its facets directly rather
than requiring `TransitionSystem` as a prerequisite. Reusing another system
concept would suggest specialization of that formalism and inheritance of its
laws; `FiniteAutomaton` instead states its own.

## Acceptance association and final-state condition

Acceptance is not a structural facet of the automaton. The system associates
an independent condition object through:

```cpp
mc_lab::semantic::acceptance_condition(system)
```

`acceptance_condition_t<System>` is the normalized type of that object. For a
finite automaton, it must model:

```cpp
FinalStateAcceptanceCondition<acceptance_condition_t<System>, state_t<System>>
```

The current final-state condition is represented by an
`AcceptingStateSet<Condition, State>`, whose range is obtained with:

```cpp
mc_lab::semantic::accepting_states(condition)
```

This accepting set belongs to the condition, not to the system. The
distinction permits future conditions such as parity, Rabin, or Streett to
associate richer objects without pretending that every acceptance semantics
is founded on one set of states.

Semantic laws:

- the condition's range denotes exactly the formal accepting-state set `F`;
- every element denotes a valid state;
- order has no semantic meaning;
- duplicate occurrences do not change the represented set.

## `symbol_t<System>`

`Sigma` is not a new facet. It is the structural label type already exposed
by `TransitionLabelling`, reached through the transition form yielded by
`outgoing_transitions`:

```cpp
template <class System>
using symbol_t =
    transition_label_for_t<System, transition_reference_for_t<System, state_t<System>>>;
```

This mirrors the `action_t`/`symbol_t` domain-alias pattern anticipated in
[structural-facets.md](structural-facets.md): both name the same neutral
`transition_label` result under a formalism-specific term, without
introducing a second customization mechanism.

## Language

A word over `Sigma` is a finite sequence `sigma_0 ... sigma_k`. `A` accepts
the word exactly when there exists an execution:

```text
q_0 q_1 ... q_(k+1)
```

such that:

```text
q_0 is in Q0
(q_i, sigma_i, q_(i+1)) is in delta, for every i in [0, k]
q_(k+1) is in F
```

The language of `A` is the set of all words it accepts:

```text
[[A]] = { sigma_0 ... sigma_k | such an execution exists }
```

This definition is recorded here as a semantic law, not as a computational
capability. `FiniteAutomaton` defines no `accepts(system, word)` or
`language(system)` CPO in this increment, for the same reason
[transition-system.md](transition-system.md) defines no `executions(system)`
CPO: turning membership in `[[A]]`, or the full language, into a required
operation would impose an enumeration or search obligation on every model,
which is unnecessary for a structural recognition concept and undesirable for
infinite, lazy, symbolic, or composed representations. Word-acceptance
checking and language computation remain separate algorithmic capabilities
that may be built on top of `FiniteAutomaton` once a concrete representation
supplies the needed computational capacity.

## What the compiler can and cannot establish

The concept establishes:

- availability and type compatibility of the structural facets;
- one shared normalized state domain across `Q0`, `delta`, and `F`;
- the structural symbol type `Sigma` carried by each transition.

The following remain semantic laws:

- validity of each initial, accepting, source, and target state value;
- soundness and completeness of `delta`, `Q0`, and `F` for the formal model;
- correctness of the label produced by `transition_label` as an element of
  `Sigma`;
- existence of an accepting execution for any specific word, and membership
  of a word in `[[A]]`.

These laws belong to documentation and future conformance and computational
utilities. They do not justify adding state, transition, or language
enumeration to the system concept.
