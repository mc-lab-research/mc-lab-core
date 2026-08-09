# Buchi automata and infinite-word acceptance

This document defines the `BuchiAutomaton` structural formalism and records its
mathematical run and acceptance semantics without imposing a concrete
execution representation.

## Structure

A Buchi automaton is a tuple:

```text
A = (Q, Sigma, delta, Q0, F)
```

where:

- `Q` is the state domain;
- `Sigma` is the alphabet;
- `delta` is contained in `Q x Sigma x Q`;
- `Q0` is the set of initial states;
- `F` is the accepting-state set represented by the associated condition.

SemTL recognizes this formalism with:

```cpp
template <class System>
concept BuchiAutomaton =
    InitialStateSet<System>
    && TransitionRelation<System, state_t<System>>
    && TransitionLabelling<
        System,
        transition_reference_for_t<System, state_t<System>>>
    && HasAcceptanceCondition<System>
    && BuchiAcceptanceCondition<
        acceptance_condition_t<System>,
        state_t<System>>;
```

`BuchiAutomaton` composes its atomic structural facets directly. It does not
refine `FiniteAutomaton`, `TransitionSystem`, or another system concept. Its
acceptance interpretation and laws are its own even where structural
requirements coincide.

## Runs and transition evidence

A run on the infinite word `sigma_0 sigma_1 ...` contains states and the
successive labelled transition choices:

```text
q0 --sigma_0--> q1 --sigma_1--> q2 ...
```

such that `q0` belongs to `Q0` and:

```text
(q_i, sigma_i, q_(i+1)) is in delta for every i >= 0
```

A state sequence alone is not a complete representation of a run. For:

```text
q0 --a--> q1
q0 --b--> q1
```

the projection `q0, q1` loses the symbol that was read. A future concrete
execution representation must therefore preserve the relevant transition
evidence. The current v0.1 candidate is a single range whose observations
alternate `State` and `Transition`, represented using
`std::variant<State, Transition>`.

That representation is intentionally not part of the structural semantic API
yet. Run construction, selection, persistence, filtering, masking, and
validation belong to later algorithms and views. Labels, costs, weights, and
probabilities will be projected from retained transition witnesses through
their structural facets rather than duplicated into execution-specific
facets.

## Acceptance

The system exposes its condition through:

```cpp
acceptance_condition(system)
```

For this formalism, the returned object models:

```cpp
BuchiAcceptanceCondition<Condition, State>
```

Its current representation is an `AcceptingStateSet<Condition, State>`
observed through `accepting_states(condition)`. More expressive acceptance
formalisms may use different condition structures.

Writing `inf(run)` for the set of states that occur infinitely often, a run is
accepting exactly when:

```text
inf(run) intersect F != {}
```

The language of `A` is the set of infinite words for which an accepting run
exists. This is a behavioral law, not an enumeration operation required from
every representation.

## Structural guarantees and semantic laws

The C++ concept establishes availability and type compatibility of initial
states, labelled transition witnesses, and the associated Buchi condition over
one state domain.

The following remain semantic laws:

- validity of initial, accepting, source, and target states;
- soundness and completeness of `delta`, `Q0`, and `F`;
- infinitude and adjacency of a complete run;
- preservation of the symbols read by its transition evidence;
- correctness of `inf(run)`;
- existence of an accepting run for a particular infinite word.

Those laws belong to documentation and future conformance and algorithmic
utilities. They do not justify adding execution or language enumeration to the
system concept.
