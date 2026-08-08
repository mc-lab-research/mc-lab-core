# SemTL architectural boundary

SemTL separates structural semantics from the behaviors produced by
algorithms. This dependency direction is a hard design boundary for the current
architecture:

```text
Semantic structure
    -> consumed by algorithms
Algorithms
    -> produce behaviors
Behaviors
    -> encoded by C++ representations
```

Compactly:

```text
Structure -> Algorithm -> Behavior -> Representation
```

The layers may use the same C++ mechanisms, including ranges and views, without
acquiring the same semantic role.

## Semantic structure

The structural core describes what a system exposes and the laws that give a
recognized formalism its meaning.

Current public categories are:

```text
semantic/
├── facet/
├── acceptance/
├── system/
└── view/
```

Future structural support may add:

```text
semantic/
├── capability/
└── conformance/
```

Their roles are distinct:

- `facet`: one atomic, local dimension of semantic structure;
- `acceptance`: an association to a condition object and the condition's
  structural acceptance vocabulary;
- `system`: a recognized formalism composed directly from atomic facets and
  acceptance, with its own laws;
- `view`: an algebraic adapter that still exposes semantic structure;
- `capability`: computational access offered by a representation, such as
  indexing or finite enumeration;
- `conformance`: future validation utilities for documented semantic laws.

SemTL does not introduce a separate public `law/` category. Laws are documented
with the abstraction whose meaning they constrain; reusable checkers belong to
future `conformance/` utilities when their required capabilities are explicit.

Examples of structural facets are:

```cpp
InitialStateSet<System>
Post<System, State>
TransitionRelation<System, State>
TransitionLabelling<System, Transition>
AtomicPropositionLabelling<System, State>
```

Acceptance remains separate from facets:

```cpp
HasAcceptanceCondition<System>
FinalStateAcceptanceCondition<Condition, State>
BuchiAcceptanceCondition<Condition, State>
```

The word "capability" is reserved for computational access in the public
taxonomy. `Post` and `TransitionRelation` are structural facets even though a
concrete representation computes their returned ranges.

## Algorithms

Algorithms consume the minimum structural facets and computational
capabilities needed for their task. Areas include:

```text
algorithm/
├── simulation/
├── execution/
├── exploration/
├── acceptance/
└── verification/
```

Examples include selecting a transition, unfolding successors, exploring a
reachable graph, validating a realized path, finding an accepting run, or
constructing a counterexample.

The first implemented exploration algorithm is
`breadth_first_reachable_states`. It consumes `InitialStateSet` with either
`Post` or `TransitionRelation`; it does not require a recognized-system
concept. Its owned `std::vector` result and equality-based discovery registry
are algorithmic representation choices, not new semantic structure.

The same system may feed several algorithms and therefore produce several
kinds of result. A `TransitionSystem` does not intrinsically contain a selected
execution merely because a simulator can construct one.

## Behaviors

Executions, runs, paths, trajectories, traces, streams, orbits, and
counterexamples are semantic results realized by algorithms. Their
mathematical definitions remain part of each formalism's documentation, but a
mathematical notion does not automatically require a structural C++ concept.

SemTL currently publishes no production `behavior/` API, `ExecutionRange`
concept, `execution(system)` CPO, or `executions(system)` enumeration.

In particular, structural concepts must not depend on behavioral results:

```text
TransitionSystem -X-> ExecutionRange
Post             -X-> Execution
FiniteAutomaton  -X-> concrete run container
```

Acceptance structure follows the same boundary. Conditions may expose final or
accepting-state information, while operations such as `accepts(word)`,
`is_accepting(execution)`, and `find_accepting_run()` are future algorithms.

## Representations

A concrete behavior may be stored or exposed through a container, borrowed
range, lazy view, generator, compressed trace, or another C++ representation.
Changing that representation must not redefine the source system's semantics.

Ranges remain central in both structural and algorithmic code. Their role is
determined by the operation that returns them:

```cpp
post(system, state)
```

returns a range representing the local successor structure, whereas a future:

```cpp
execute(configured_system)
```

could return a range representing one behavior selected by an algorithm. The
shared range mechanism does not collapse those meanings.

For the first future transition-oriented behavioral result, the provisional
v0.1 representation choice is:

```cpp
std::variant<State, Transition>
```

in a range whose observations conceptually alternate:

```text
State(q0), Transition(t0), State(q1), Transition(t1), State(q2), ...
```

This is deliberately deferred. It is an algorithm-result representation, not a
contract of `Post`, `TransitionRelation`, or an automaton concept. Labels,
weights, costs, probabilities, and other observations will be projected from
the retained transition witnesses rather than duplicated as execution-specific
facets.

## Classification test

Evaluate each proposed abstraction with three questions.

### Is it intrinsic local structure of the system?

Examples include initial states, direct successors, transition witnesses,
transition labels, state propositions, and acceptance conditions. These belong
to the structural semantic layer.

### Is it obtained by selecting, unfolding, or exploring that structure?

Examples include a run, execution, trajectory, trace, exploration tree, and
counterexample. These are behaviors produced by algorithms.

### Is it only how C++ exposes or stores the result?

Examples include `std::variant`, `std::vector`, `std::span`, a view, or a
generator. These are representations.

Do not promote an algorithm-produced behavior or one of its representations
into a structural facet or recognized-system requirement.
