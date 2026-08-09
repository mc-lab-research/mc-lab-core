#ifndef MC_LAB_ALGORITHM_EXPLORATION_VISITED_REGISTRY_HPP
#define MC_LAB_ALGORITHM_EXPLORATION_VISITED_REGISTRY_HPP

#include <concepts>

/**
 * @file
 * Defines the discovery/deduplication policy protocol consumed by the
 * generic exploration kernels.
 *
 * A `VisitedRegistry` records which states an exploration has already
 * discovered, independent of the order they are examined in (that is the
 * separate `Frontier` policy's role).
 *
 * A model exposes this protocol through one member operation:
 *
 * @code
 * struct visited {
 *   auto try_insert(const State& state) -> bool;
 * };
 * @endcode
 *
 * This is an algorithmic policy protocol, not a `mc_lab::semantic` structural
 * facet, for the same reason `Frontier` is not: it describes algorithm-owned
 * auxiliary state, a representation choice of how discovery is tracked, not
 * a property of the system under exploration.
 */
namespace mc_lab::algorithm::exploration {

/**
 * A policy recording which states have already been discovered.
 *
 * Compile-time contract:
 * - `visited.try_insert(state)`, called with `const State&`, is contextually
 *   convertible to `bool`.
 *
 * Semantic contract, not enforceable by the compiler:
 * - `try_insert(state)` returns `true` and records `state` exactly when no
 *   previously recorded state compares equal to it;
 * - `try_insert(state)` returns `false`, and leaves the registry's recorded
 *   set unchanged, when an equal state was already recorded;
 * - the registry alone decides equality; callers never supply a comparator.
 *
 * This concept imposes nothing on `State` beyond what `try_insert` requires
 * for a *given* `V`. A concrete policy such as `linear_visited_registry<State>`
 * may itself require `State` to be equality-comparable and copy-constructible
 * for its own storage and comparisons; those requirements belong to the
 * policy, not to this protocol concept or to the generic exploration kernels
 * that consume it.
 */
template <class V, class State>
concept VisitedRegistry = requires(V& visited, const State& state) {
    { visited.try_insert(state) } -> std::convertible_to<bool>;
};

}  // namespace mc_lab::algorithm::exploration

#endif  // MC_LAB_ALGORITHM_EXPLORATION_VISITED_REGISTRY_HPP
