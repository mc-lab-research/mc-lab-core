#ifndef MC_LAB_ALGORITHM_EXPLORATION_EXPLORE_TRANSITION_RELATION_HPP
#define MC_LAB_ALGORITHM_EXPLORATION_EXPLORE_TRANSITION_RELATION_HPP

#include <mc_lab/algorithm/exploration/frontier.hpp>
#include <mc_lab/algorithm/exploration/visited_registry.hpp>
#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>

#include <utility>

/**
 * @file
 * Defines the generic transition-witness exploration kernel.
 */
namespace mc_lab::algorithm::exploration {

/**
 * An observer of a `TransitionRelation`-based exploration, notified as
 * `explore_transition_relation` discovers and examines states.
 *
 * Compile-time contract, for a system modelling
 * `TransitionRelation<System, State>`:
 * - `discover_state(state)`, `examine_state(state)`, and `finish_state(state)`
 *   each accept `const State&`;
 * - `examine_transition(source, transition, target)` accepts `const State&`
 *   for `source`, the exact transition-witness form
 *   `outgoing_transitions(system, source)` yields, and the exact form
 *   `target(system, transition)` yields;
 * - `tree_transition(source, transition, target)` accepts `const State&`,
 *   that same transition-witness form, and `const State&` for `target`.
 *
 * Callback order is documented precisely on `explore_transition_relation`
 * itself.
 *
 * Observation lifetime: `source`, and every value passed to `discover_state`,
 * `examine_state`, and `finish_state`, are owned `State` values that remain
 * valid beyond the callback. The `transition` argument, in both
 * `examine_transition` and `tree_transition`, and the `target` argument of
 * `examine_transition`, may instead be ephemeral references or proxies
 * produced by `outgoing_transitions`'s and `target`'s underlying ranges;
 * they are valid only for the duration of that call, unless the concrete
 * system model documents a stronger guarantee. An observer must not retain
 * them implicitly, for example by binding one to a member reference, beyond
 * the callback. The `target` argument of `tree_transition` is instead the
 * owned `State` value that was just inserted into the visited registry.
 */
template <class Observer, class System, class State>
concept TransitionObserver = semantic::TransitionRelation<System, State>
    && requires(Observer& observer,
                const State& state,
                semantic::transition_reference_for_t<System, State> transition,
                semantic::target_result_t<System, State> target) {
           observer.discover_state(state);
           observer.examine_state(state);
           observer.examine_transition(state, transition, target);
           observer.tree_transition(state, transition, state);
           observer.finish_state(state);
       };

/**
 * Explores a system's reachable states through `TransitionRelation`, driving
 * `frontier`, `visited`, and `observer`. Produces no materialized result of
 * its own; callers that want one, or that need transition evidence such as
 * labels, weights, or provenance, supply an `observer` that retains it.
 *
 * Requires `InitialStateSet<System>` and
 * `TransitionRelation<System, state_t<System>>` directly, not a recognized
 * system concept such as `TransitionSystem`: algorithms consume the minimal
 * facets their task actually needs.
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
 *     for each transition in outgoing_transitions(system, source), in range order:
 *         target = target(system, transition)   // computed exactly once per transition
 *         observer.examine_transition(source, transition, target)
 *         candidate = State(target)
 *         if visited.try_insert(candidate):
 *             frontier.push(candidate)
 *             observer.tree_transition(source, transition, candidate)
 *             observer.discover_state(candidate)
 *     observer.finish_state(source)
 * @endcode
 *
 * `examine_transition` is called once per transition witness examined,
 * whether or not its target was already visited: it is the projection
 * point described in `TransitionRelation`'s own documentation, not a
 * discovery event. `tree_transition` and `discover_state` are called only
 * when `visited.try_insert` reports a genuinely new target state, for that
 * state alone, always in that relative order: the state is already pushed
 * onto `frontier` by the time either callback runs. `finish_state(source)`
 * is called exactly once per popped state, after every one of its outgoing
 * transitions has been examined.
 *
 * `target(system, transition)` is called exactly once per examined
 * transition witness. Its single result feeds `examine_transition`, the
 * `try_insert` membership check, and — materialized into `candidate` — both
 * `tree_transition` and `discover_state` when the target is new.
 *
 * A source with several transitions sharing one target (parallel
 * transitions) causes `examine_transition` once per witness but
 * `discover_state`/`tree_transition` at most once for that target.
 *
 * Structure versus algorithm versus result versus representation:
 * `InitialStateSet` and `TransitionRelation` are this system's structural
 * facets, i.e. what it exposes; `explore_transition_relation` is the
 * algorithm, i.e. how that structure is traversed; `frontier`, `visited`,
 * and `observer` are algorithm-owned representations, i.e. how the
 * traversal's own bookkeeping and any result are stored, supplied by the
 * caller and never part of the system's semantics.
 *
 * Termination: this call terminates when the reachable state set is finite
 * and every consumed `initial_states`/`outgoing_transitions` range
 * terminates. Neither `InitialStateSet` nor `TransitionRelation` promises
 * that; it is a precondition of a terminating call, not a law of those
 * facets.
 *
 * Exception safety: this function allocates through `frontier`, `visited`,
 * and `observer`, so it is not `noexcept`. If any of those policies throws,
 * whatever it had already recorded remains recorded; this function performs
 * no rollback.
 */
template <class System, class FrontierPolicy, class VisitedPolicy, class ObserverPolicy>
requires semantic::InitialStateSet<System>
    && semantic::TransitionRelation<System, semantic::state_t<System>>
    && Frontier<FrontierPolicy, semantic::state_t<System>>
    && VisitedRegistry<VisitedPolicy, semantic::state_t<System>>
    && TransitionObserver<ObserverPolicy, System, semantic::state_t<System>>
constexpr void explore_transition_relation(const System& system,
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

        for (auto&& transition : semantic::outgoing_transitions(system, source)) {
            // `target` is computed exactly once per transition witness; its
            // result feeds examine_transition, the membership check, and,
            // materialized, both tree_transition and discover_state.
            decltype(auto) target_state = semantic::target(system, transition);
            observer.examine_transition(source, transition, target_state);
            State candidate(target_state);
            if (visited.try_insert(candidate)) {
                frontier.push(candidate);
                observer.tree_transition(source, transition, candidate);
                observer.discover_state(candidate);
            }
        }

        observer.finish_state(source);
    }
}

}  // namespace mc_lab::algorithm::exploration

#endif  // MC_LAB_ALGORITHM_EXPLORATION_EXPLORE_TRANSITION_RELATION_HPP
