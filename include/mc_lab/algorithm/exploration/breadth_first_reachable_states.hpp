#ifndef MC_LAB_ALGORITHM_EXPLORATION_BREADTH_FIRST_REACHABLE_STATES_HPP
#define MC_LAB_ALGORITHM_EXPLORATION_BREADTH_FIRST_REACHABLE_STATES_HPP

#include <mc_lab/algorithm/exploration/explore_states.hpp>
#include <mc_lab/algorithm/exploration/explore_transition_relation.hpp>
#include <mc_lab/algorithm/exploration/fifo_frontier.hpp>
#include <mc_lab/algorithm/exploration/linear_visited_registry.hpp>
#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/post.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>

#include <concepts>
#include <vector>

/**
 * @file
 * Defines materialized breadth-first enumeration of reachable states.
 *
 * This algorithm is now a thin, concrete configuration of the generic
 * `explore_states`/`explore_transition_relation` kernels: it supplies
 * `fifo_frontier` for exploration order and `linear_visited_registry` for
 * deduplication, and passes a private no-op observer that satisfies each
 * kernel's contract without recording anything of its own. The returned
 * vector is extracted directly from `linear_visited_registry`, whose
 * recorded insertion order already *is* breadth-first discovery order (see
 * that policy's own documentation), rather than accumulated a second time
 * through an observer callback. The generic kernels have no knowledge of
 * vectors or of this algorithm; they only drive whatever `Frontier`,
 * `VisitedRegistry`, and observer they are given.
 */
namespace mc_lab::algorithm::exploration {

namespace detail {

template <class System>
concept BreadthFirstReachabilityInput =
    semantic::InitialStateSet<System> && std::copy_constructible<semantic::state_t<System>>
    && std::equality_comparable<semantic::state_t<System>>
    && (semantic::Post<System, semantic::state_t<System>>
        || semantic::TransitionRelation<System, semantic::state_t<System>>);

/**
 * An observer that records nothing. Satisfies both `StateObserver` and
 * `TransitionObserver`, so the same type drives either kernel; the result
 * is instead read back from the `VisitedRegistry` policy.
 */
template <class State>
class no_op_observer {
  public:
    constexpr void discover_state(const State&) const noexcept {}
    constexpr void examine_state(const State&) const noexcept {}

    template <class Successor>
    constexpr void examine_successor(const State&, Successor&&) const noexcept {}

    constexpr void tree_edge(const State&, const State&) const noexcept {}

    template <class Transition, class Target>
    constexpr void examine_transition(const State&, Transition&&, Target&&) const noexcept {}

    template <class Transition>
    constexpr void tree_transition(const State&, Transition&&, const State&) const noexcept {}

    constexpr void finish_state(const State&) const noexcept {}
};

}  // namespace detail

/**
 * Materializes every state reachable from a system's initial-state set.
 *
 * The algorithm consumes `InitialStateSet` together with either of the atomic
 * local-evolution facets:
 *
 * - `Post` supplies successor states directly, via the generic
 *   `explore_states` kernel;
 * - otherwise, `TransitionRelation` supplies witnesses projected through
 *   `target`, via the generic `explore_transition_relation` kernel.
 *
 * If both facets are available, `Post` is selected. This state-only algorithm
 * deliberately discards transition witnesses; algorithms constructing paths,
 * executions, or counterexamples must retain those witnesses separately (for
 * example, by driving `explore_transition_relation` directly with an
 * observer that keeps them).
 *
 * The returned vector contains each state once, in breadth-first discovery
 * order: the order the underlying kernel's `try_insert` calls first accepted
 * it (see `linear_visited_registry`). That order reflects the ranges
 * supplied by the selected facet and is not part of the system's
 * mathematical semantics. Equality must represent computational state
 * identity.
 *
 * The operation terminates when the reachable state set and every consumed
 * local range are finite. These are algorithm preconditions, not laws imposed
 * on the semantic facets.
 *
 * `State` is copy-constructible because `fifo_frontier` and
 * `linear_visited_registry` both require it for their own storage (see
 * their documentation), and because the returned vector owns its states.
 */
template <class System>
requires detail::BreadthFirstReachabilityInput<System>
[[nodiscard]] constexpr auto
breadth_first_reachable_states(const System& system) -> std::vector<semantic::state_t<System>> {
    using State = semantic::state_t<System>;

    fifo_frontier<State> frontier;
    linear_visited_registry<State> visited;
    detail::no_op_observer<State> observer;

    // `Post` is the most direct observation for a state-only result. The
    // branch also makes dispatch unambiguous when a model exposes both
    // facets.
    if constexpr (semantic::Post<System, State>) {
        explore_states(system, frontier, visited, observer);
    } else {
        explore_transition_relation(system, frontier, visited, observer);
    }

    return std::move(visited).extract();
}

}  // namespace mc_lab::algorithm::exploration

#endif  // MC_LAB_ALGORITHM_EXPLORATION_BREADTH_FIRST_REACHABLE_STATES_HPP
