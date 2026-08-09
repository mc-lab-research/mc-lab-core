# Generic exploration: kernels, policies, and observers

This document defines the reusable exploration machinery under
`mc_lab::algorithm::exploration`: the `explore_states` and
`explore_transition_relation` kernels, the `Frontier` and `VisitedRegistry`
policy protocols they are parameterized over, and the `StateObserver`/
`TransitionObserver` protocols notified as they run.
`breadth_first_reachable_states` (documented separately in
[breadth-first-reachable-states.md](breadth-first-reachable-states.md)) is
the first concrete algorithm assembled from these pieces, not a separate
design.

## Roles

```text
System            -> what is explored: exposes InitialStateSet with either
                      Post or TransitionRelation (mc_lab::semantic facets)
Frontier           -> algorithm-owned: decides exploration order
VisitedRegistry     -> algorithm-owned: decides deduplication
Observer            -> caller-owned: notified of every event; builds
                        whatever result (or side effect) the caller wants
```

`Frontier`, `VisitedRegistry`, and the observer protocols live under
`include/mc_lab/algorithm/exploration/`, not under `semantic/capability/`.
They are policies and protocols of this algorithm, not capabilities of the
source system: a `Frontier` describes how *this traversal* orders its work,
not anything the system under exploration exposes. Placing them under
`semantic/` would misclassify an algorithmic representation choice as
semantic structure, exactly the confusion
[architecture.md](../../semantic/architecture.md)'s classification test
warns against.

## Why facets, not a recognized system concept

`explore_states` requires `InitialStateSet<System>` and
`Post<System, state_t<System>>` directly. `explore_transition_relation`
requires `InitialStateSet<System>` and
`TransitionRelation<System, state_t<System>>` directly. Neither requires
`TransitionSystem` or any other recognized system concept. A recognized
system concept would demand structure the kernel does not use (for example,
an automaton's acceptance condition) and would tie a general-purpose
traversal to one formalism's laws. Algorithms consume the minimal facets
their task actually needs, matching how a forward traversal is described
throughout this codebase.

## `explore_states` versus `explore_transition_relation`

They are two independent kernels over two independent, atomic facets, not
one kernel with an optional mode:

- `explore_states` consumes `Post`: successor *states* are the only thing it
  ever observes. It cannot retain transition evidence because none was ever
  produced.
- `explore_transition_relation` consumes `TransitionRelation`: it observes
  transition *witnesses* and projects each one to a target state through
  `target`. An observer given the witness can retain whatever evidence it
  carries (labels, weights, provenance, ...).

Neither refines the other and neither is unified with fake or optional
callbacks: a `StateObserver` has `examine_successor`/`tree_edge`; a
`TransitionObserver` has `examine_transition`/`tree_transition`. Post never
synthesizes a transition witness to satisfy a transition-shaped callback.

## Exact callback sequence

Both kernels share one discovery discipline, differing only in what a
"successor" observation is.

Initial-state seeding, for each `s0` in `initial_states(system)`, in range
order:

```text
candidate = State(s0)
if visited.try_insert(candidate):
    frontier.push(candidate)
    observer.discover_state(candidate)
```

Main loop, while `frontier` is not empty:

```text
source = frontier.pop()
observer.examine_state(source)
for each successor observation (successor state, or transition witness
        projected through target), in range order:
    observer.examine_successor(source, successor)          # or examine_transition(source, transition, target)
    candidate = State(successor)                            # or State(target)
    if visited.try_insert(candidate):
        frontier.push(candidate)
        observer.tree_edge(source, candidate)               # or tree_transition(source, transition, candidate)
        observer.discover_state(candidate)
observer.finish_state(source)
```

Load-bearing details:

- `examine_successor`/`examine_transition` fire for *every* observation,
  whether or not it turns out to be a new state. This is the projection
  point, not a discovery event.
- `try_insert` gates everything after it. When it returns `false` (already
  visited), nothing further happens for that observation.
- When it returns `true`, the order is always `frontier.push`, then
  `tree_edge`/`tree_transition`, then `discover_state`, in that relative
  order: the state is already in the frontier by the time either observer
  callback for it runs. This is a deliberate public contract, not an
  incidental implementation detail, and it must not silently drift from
  this document or from the tests that pin it down.
- `finish_state(source)` fires exactly once per popped state, after every
  one of its successor observations has been examined.
- For `explore_transition_relation`, `target(system, transition)` is
  computed exactly once per examined transition witness; its single result
  feeds `examine_transition`, the `try_insert` check, and — materialized —
  both `tree_transition` and `discover_state`.

## Proxy and witness lifetime

`examine_successor`'s `successor` argument and `examine_transition`'s
`transition`/`target` arguments may be ephemeral references or proxies
produced by the underlying `post`/`outgoing_transitions`/`target` ranges.
They are valid only for the duration of that call, unless the concrete
system model documents a stronger guarantee. An observer must not persist
one implicitly, for example by binding it to a member reference, beyond the
callback. Every other observer argument — `source`, and every value passed
to `discover_state`, `tree_edge`/`tree_transition`, `examine_state`, and
`finish_state` — is an owned `State` value that remains valid beyond the
callback, because each is the `candidate` the kernel already materialized
for the `try_insert` check.

## Termination

A call terminates when the reachable state set is finite and every consumed
`initial_states`/`post`/`outgoing_transitions` range terminates. Neither
`InitialStateSet`, `Post`, nor `TransitionRelation` promises that; it is a
precondition of a terminating call, not a law of those facets.

## Exception safety

Neither kernel is `noexcept`: both allocate through whatever `frontier`,
`visited`, and `observer` actually do. The v0.1 guarantee is basic: if the
system, frontier, visited registry, or observer throws, the objects the
caller supplied may be left partially modified (for example, `visited` may
already contain states that were never pushed to `frontier` if `push`
itself throws after `try_insert` succeeded). Neither kernel performs
rollback. Callers that need a stronger guarantee must provide policies that
offer one themselves.

## `fifo_frontier`: vector-plus-cursor tradeoff

`fifo_frontier<State>` stores pushed states in an appended `std::vector`
read through an advancing cursor, not a two-ended queue. This keeps every
operation `constexpr`-usable (`std::deque` is not constexpr-friendly on this
library's target standard) and matches the discovery-order storage the
algorithm this policy was extracted from already used.

The cost: popped elements are never reclaimed. Peak storage is proportional
to the *total* number of states ever pushed over the frontier's lifetime,
not to the maximum number of states simultaneously live in the frontier.
This is materially different from a `std::queue` backed by `std::deque`,
which reclaims a dequeued element's storage; `fifo_frontier` must not be
described as memory-equivalent to that. No compaction is planned for this
increment without a concrete need, since compaction introduces its own
invalidation and complexity decisions. See `fifo_frontier.hpp`'s own
documentation for the exact class invariant and reuse-after-drain behavior.

## `linear_visited_registry`: complexity

`try_insert` performs a linear scan: up to `n` equality comparisons against
`n` already-recorded states before inserting. Recording `V` distinct states
over a full exploration therefore costs `O(V^2)` equality comparisons in the
worst case. A caller expecting a large reachable set should supply a
different `VisitedRegistry` (for example, hash-based), which this library
does not yet provide.

Because both kernels call `try_insert` exactly when a state is genuinely
new — the same condition that gates `discover_state` — the sequence of
successful insertions `linear_visited_registry` accumulates *is* the
breadth-first discovery order the paired FIFO frontier establishes. Its
`states()`/`extract()` expose that sequence directly.

## How `breadth_first_reachable_states` is assembled

```cpp
fifo_frontier<State> frontier;
linear_visited_registry<State> visited;
detail::no_op_observer<State> observer;   // satisfies both observer protocols, records nothing

if constexpr (semantic::Post<System, State>) {
    explore_states(system, frontier, visited, observer);
} else {
    explore_transition_relation(system, frontier, visited, observer);
}

return std::move(visited).extract();
```

No separate result-accumulating observer is needed: `linear_visited_registry`
already holds the discovered states in discovery order, so the result is
read back from it instead of duplicated into a second vector.

`Post` is preferred when both facets are available because it is the most
direct observation for a state-only result, and because the choice must be
unambiguous rather than attempting both. This does not make `Post` a
refinement of `TransitionRelation`, or vice versa; a model may reasonably
provide both, and [post.md](../../semantic/post.md) documents the
set-equivalence law such a model must obey.

Algorithms that need transition evidence — labels, weights, provenance, or
anything else attached to a specific transition witness rather than to a
target state — must drive `explore_transition_relation` directly, even on a
system that also exposes `Post`, because `explore_states` structurally
cannot observe or retain what `Post` never produced.
