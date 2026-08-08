#ifndef MC_LAB_ALGORITHM_EXPLORATION_BREADTH_FIRST_REACHABLE_STATES_HPP
#define MC_LAB_ALGORITHM_EXPLORATION_BREADTH_FIRST_REACHABLE_STATES_HPP

#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/post.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <ranges>
#include <utility>
#include <vector>

/**
 * @file
 * Defines materialized breadth-first enumeration of reachable states.
 */
namespace mc_lab::algorithm::exploration {

namespace detail {

template <class System>
concept BreadthFirstReachabilityInput =
    semantic::InitialStateSet<System> && std::copy_constructible<semantic::state_t<System>>
    && std::equality_comparable<semantic::state_t<System>>
    && (semantic::Post<System, semantic::state_t<System>>
        || semantic::TransitionRelation<System, semantic::state_t<System>>);

template <class State, class Candidate>
constexpr void append_if_new(std::vector<State>& discovered, Candidate&& candidate) {
    State materialized(std::forward<Candidate>(candidate));
    if (std::ranges::find(discovered, materialized) == discovered.end()) {
        discovered.push_back(std::move(materialized));
    }
}

template <class System, class State>
constexpr void
append_successors(const System& system, const State& source, std::vector<State>& discovered) {
    // `Post` is the most direct observation for a state-only result. The
    // branch also makes dispatch unambiguous when a model exposes both facets.
    if constexpr (semantic::Post<System, State>) {
        for (auto&& successor : semantic::post(system, source)) {
            append_if_new(discovered, std::forward<decltype(successor)>(successor));
        }
    } else {
        for (auto&& transition : semantic::outgoing_transitions(system, source)) {
            decltype(auto) successor =
                semantic::target(system, std::forward<decltype(transition)>(transition));
            append_if_new(discovered, std::forward<decltype(successor)>(successor));
        }
    }
}

}  // namespace detail

/**
 * Materializes every state reachable from a system's initial-state set.
 *
 * The algorithm consumes `InitialStateSet` together with either of the atomic
 * local-evolution facets:
 *
 * - `Post` supplies successor states directly;
 * - otherwise, `TransitionRelation` supplies witnesses projected through
 *   `target`.
 *
 * If both facets are available, `Post` is selected. This state-only algorithm
 * deliberately discards transition witnesses; algorithms constructing paths,
 * executions, or counterexamples must retain those witnesses separately.
 *
 * The returned vector contains each state once, in breadth-first discovery
 * order. That order reflects the ranges supplied by the selected facet and is
 * not part of the system's mathematical semantics. Equality must represent
 * computational state identity.
 *
 * The operation terminates when the reachable state set and every consumed
 * local range are finite. These are algorithm preconditions, not laws imposed
 * on the semantic facets.
 *
 * `State` is copy-constructible because the result owns its states and because
 * the currently expanded state is copied before the vector can reallocate.
 */
template <class System>
requires detail::BreadthFirstReachabilityInput<System>
[[nodiscard]] constexpr auto
breadth_first_reachable_states(const System& system) -> std::vector<semantic::state_t<System>> {
    using State = semantic::state_t<System>;

    std::vector<State> discovered;
    for (auto&& initial : semantic::initial_states(system)) {
        detail::append_if_new(discovered, std::forward<decltype(initial)>(initial));
    }

    for (std::size_t cursor = 0; cursor < discovered.size(); ++cursor) {
        // Keep a stable source while appending successors may reallocate the
        // vector. A lazy local range may continue observing this value.
        const State source = discovered[cursor];
        detail::append_successors(system, source, discovered);
    }

    return discovered;
}

}  // namespace mc_lab::algorithm::exploration

#endif  // MC_LAB_ALGORITHM_EXPLORATION_BREADTH_FIRST_REACHABLE_STATES_HPP
