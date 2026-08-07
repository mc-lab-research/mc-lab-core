# Buchi automata, runs, and infinite-word acceptance

This document defines the fourth recognized SemTL system, `BuchiAutomaton`,
and connects its C++ contracts to the mathematical notion of infinite-word
acceptance.

## Structure

A Buchi automaton is defined the same way as a finite automaton: a tuple

```text
A = (Q, Sigma, delta, Q0, F)
```

with the same components as [finite-automaton.md](finite-automaton.md):
states `Q`, alphabet `Sigma`, transition relation `delta` contained in
`Q x Sigma x Q`, initial states `Q0 subseteq Q`, and accepting states
`F subseteq Q`. Only the language differs, so SemTL recognizes the same
structural composition under a distinct name:

```cpp
template <class System>
concept BuchiAutomaton =
    InitialStateSet<System>
    && LabelledTransitionRelation<System, state_t<System>>
    && AcceptingStateSet<System, state_t<System>>;
```

`BuchiAutomaton` does not refine `FiniteAutomaton`. The two concepts compose
the same facets directly, and each states its own laws, so that a future
increment can add structural requirements to one without forcing them onto
the other. This mirrors why `KripkeStructure` does not refine
`TransitionSystem` even though both are built from `InitialStateSet` and
`TransitionRelation`.

## Runs

A run of `A` on an infinite word `sigma_0 sigma_1 ...` is an infinite state
sequence:

```text
q_0 q_1 ...
```

such that:

```text
q_0 is in Q0
(q_i, sigma_i, q_(i+1)) is in delta, for every i >= 0
```

This is the same shape as a Kripke path, but built over the labelled relation
`delta` rather than an unlabelled, total `R`: each step is justified by one
symbol of the word being read, and `delta` need not be total.

SemTL gives run carriers a distinct public name:

```cpp
template <class System, class Run>
concept BuchiRunRange = BuchiAutomaton<System> && ExecutionRange<Run, state_t<System>>;

template <class System, class Execution>
concept BuchiExecutionRange = BuchiRunRange<System, Execution>;
```

As with `KripkePathRange`/`KripkeExecutionRange`, the two concepts share one
syntactic range requirement; their difference is the semantic initiality law
attached to `BuchiExecutionRange`. Neither infinitude, adjacency to `delta`,
nor initiality in `Q0` is enforceable from the range type alone.

## Acceptance

Write `inf(run)` for the set of states that occur infinitely often in a run:

```text
inf(run) = { q in Q | q = run(i) for infinitely many i }
```

`A` accepts the word `sigma_0 sigma_1 ...` exactly when it has a run `run` on
that word such that:

```text
inf(run) intersect F != {}
```

that is, the run visits at least one accepting state infinitely often. The
language of `A` is the set of all infinite words it accepts:

```text
[[A]] = { sigma_0 sigma_1 ... | such a run exists }
```

Unlike finite-automaton acceptance, which asks about the last state of a
finite execution, Buchi acceptance is a recurrence property of an entire
infinite run and cannot be reduced to a predicate on a single state. It is
recorded here as a semantic law, not as a computational capability.
`BuchiAutomaton` defines no `inf(run)`, `accepts(system, word)`, or
`language(system)` CPO in this increment, for the same reason
[finite-automaton.md](finite-automaton.md) defines none of its acceptance
operations: computing `inf(run)` in general requires consuming an infinite
range, which is a separate algorithmic capability (e.g. cycle detection on a
finite state space), not a structural requirement every model must satisfy.

## What the compiler can and cannot establish

The concepts establish:

- availability and type compatibility of the structural facets, identical to
  `FiniteAutomaton`;
- range-shaped access to run and execution states, identical in shape to
  `KripkePathRange`/`KripkeExecutionRange`.

The following remain semantic laws:

- validity of each initial, accepting, source, and target state value;
- soundness and completeness of `delta`, `Q0`, and `F`;
- infinitude of a complete run;
- membership of a run's first state in `Q0`;
- correctness of `inf(run)` for a specific run;
- existence of an accepting run for any specific infinite word, and
  membership of a word in `[[A]]`.

These laws belong to documentation and future conformance and computational
utilities. They do not justify adding state, transition, or language
enumeration to the system concept.
