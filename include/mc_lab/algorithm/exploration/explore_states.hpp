#ifndef MC_LAB_ALGORITHM_EXPLORATION_EXPLORE_STATES_HPP
#define MC_LAB_ALGORITHM_EXPLORATION_EXPLORE_STATES_HPP

#include <mc_lab/algorithm/exploration/frontier.hpp>
#include <mc_lab/algorithm/exploration/visited_registry.hpp>
#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/post.hpp>

#include <utility>

/**
 * @file
 * Defines the generic direct-successor exploration kernel.
 */
namespace mc_lab::algorithm::exploration {

/**
 * An observer of a `Post`-based exploration, notified as `explore_states`
 * discovers and examines states.
 *
 * Compile-time contract, for a system modelling `Post<System, State>`:
 * - `discover_state(state)`, `examine_state(state)`, and `finish_state(state)`
 *   each accept `const State&`;
 * - `examine_successor(source, successor)` accepts `const State&` for
 *   `source` and, for `successor`, the exact reference or proxy form
 *   `post(system, source)`'s range yields;
 * - `tree_edge(source, successor)` accepts two `const State&`.
 *
 * Callback order is documented precisely on `explore_states` itself.
 *
 * Observation lifetime: `source`, and every value passed to `discover_state`,
 * `tree_edge`, `examine_state`, and `finish_state`, are owned `State` values
 * that remain valid beyond the callback. The `successor` argument of
 * `examine_successor` may instead be an ephemeral reference or proxy
 * produced by `post`'s underlying range; it is valid only for the duration
 * of that call, unless the concrete system model documents a stronger
 * guarantee. An observer must not retain it implicitly, for example by
 * binding it to a member reference, beyond the callback.
 */
template <class Observer, class System, class State>
concept StateObserver = semantic::Post<System, State>
    && requires(Observer& observer,
                const State& state,
                semantic::post_reference_for_t<System, State> successor) {
           observer.discover_state(state);
           observer.examine_state(state);
           observer.examine_successor(state, successor);
           observer.tree_edge(state, state);
           observer.finish_state(state);
       };

/**
 * Explores a system's reachable states through `Post`, driving `frontier`,
 * `visited`, and `observer`. Produces no materialized result of its own;
 * callers that want one supply an `observer` that builds it.
 *
 * Requires `InitialStateSet<System>` and `Post<System, state_t<System>>`
 * directly, not a recognized system concept such as `TransitionSystem`:
 * algorithms consume the minimal facets their task actually needs.
 *
 * Exact callback sequence, in this order:
 *
 * @code
 * for each state s0 in initial_states(system), in range order:
 *     candidate = State(s0)
 *     if visited.try_insert(candidate):
 *         frontier.push(candidate)
 *         observer.discover_state(candidate)
 *
 * while not frontier.empty():
 *     source = frontier.pop()
 *     observer.examine_state(source)
 *     for each successor in post(system, source), in range order:
 *         observer.examine_successor(source, successor)
 *         candidate = State(successor)
 *         if visited.try_insert(candidate):
 *             frontier.push(candidate)
 *             observer.tree_edge(source, candidate)
 *             observer.discover_state(candidate)
 *     observer.finish_state(source)
 * @endcode
 *
 * `examine_successor` is called once per successor observation, whether or
 * not that successor was already visited. `tree_edge` and `discover_state`
 * are called only when `visited.try_insert` reports a genuinely new state,
 * for that state alone, always in that relative order: the state is already
 * pushed onto `frontier` by the time either callback runs. `finish_state(source)`
 * is called exactly once per popped state, after every one of its successors
 * has been examined.
 *
 * Structure versus algorithm versus result versus representation:
 * `InitialStateSet` and `Post` are this system's structural facets, i.e.
 * what it exposes; `explore_states` is the algorithm, i.e. how that
 * structure is traversed; `frontier`, `visited`, and `observer` are
 * algorithm-owned representations, i.e. how the traversal's own bookkeeping
 * and any result are stored, supplied by the caller and never part of the
 * system's semantics.
 *
 * Termination: this call terminates when the reachable state set is finite
 * and every consumed `initial_states`/`post` range terminates. Neither
 * `InitialStateSet` nor `Post` promises that; it is a precondition of a
 * terminating call, not a law of those facets.
 *
 * Exception safety: this function allocates through `frontier`, `visited`,
 * and `observer`, so it is not `noexcept`. If any of those policies throws,
 * whatever it had already recorded remains recorded; this function performs
 * no rollback.
 */
template <class System, class FrontierPolicy, class VisitedPolicy, class ObserverPolicy>
requires semantic::InitialStateSet<System> && semantic::Post<System, semantic::state_t<System>>
    && Frontier<FrontierPolicy, semantic::state_t<System>>
    && VisitedRegistry<VisitedPolicy, semantic::state_t<System>>
    && StateObserver<ObserverPolicy, System, semantic::state_t<System>>
constexpr void explore_states(const System& system,
                              FrontierPolicy& frontier,
                              VisitedPolicy& visited,
                              ObserverPolicy& observer) {
    using State = semantic::state_t<System>;

    for (auto&& initial : semantic::initial_states(system)) {
        State candidate(initial);
        if (visited.try_insert(candidate)) {
            frontier.push(candidate);
            observer.discover_state(candidate);
        }
    }

    while (!frontier.empty()) {
        State source = frontier.pop();
        observer.examine_state(source);

        for (auto&& successor : semantic::post(system, source)) {
            observer.examine_successor(source, successor);
            State candidate(successor);
            if (visited.try_insert(candidate)) {
                frontier.push(candidate);
                observer.tree_edge(source, candidate);
                observer.discover_state(candidate);
            }
        }

        observer.finish_state(source);
    }
}

}  // namespace mc_lab::algorithm::exploration

#endif  // MC_LAB_ALGORITHM_EXPLORATION_EXPLORE_STATES_HPP
