#ifndef MC_LAB_ALGORITHM_EXPLORATION_LINEAR_VISITED_REGISTRY_HPP
#define MC_LAB_ALGORITHM_EXPLORATION_LINEAR_VISITED_REGISTRY_HPP

#include <mc_lab/algorithm/exploration/visited_registry.hpp>

#include <algorithm>
#include <concepts>
#include <span>
#include <utility>
#include <vector>

/**
 * @file
 * Defines the linear-search `VisitedRegistry` policy.
 */
namespace mc_lab::algorithm::exploration {

/**
 * A `VisitedRegistry` that records discovered states in a plain vector and
 * tests membership with a linear scan.
 *
 * Complexity: for `n` states already recorded, `try_insert` performs up to
 * `n` equality comparisons before inserting. Recording `V` distinct states
 * over a full exploration therefore costs `O(V^2)` equality comparisons in
 * the worst case. A caller expecting a large reachable set should supply a
 * different `VisitedRegistry` policy (for example, a hash-based one), which
 * this library does not yet provide.
 *
 * `State` must be equality-comparable and copy-constructible: this policy,
 * not the generic exploration kernels, is what actually needs to compare
 * and store states, so both requirements are imposed here rather than
 * globally.
 *
 * Because a caller pairs this registry with `explore_states` or
 * `explore_transition_relation` through `try_insert` alone, and both kernels
 * call `try_insert` exactly when a state is genuinely new, the sequence of
 * successful insertions this registry accumulates is precisely the
 * breadth-first discovery order established by the paired FIFO frontier and
 * kernel discovery protocol. `states()` and `extract()` expose that sequence
 * directly, so a caller that only wants the discovered-state list (such as
 * `breadth_first_reachable_states`) does not need a second, separately
 * accumulated copy of it.
 */
template <class State>
requires std::equality_comparable<State> && std::copy_constructible<State>
class linear_visited_registry {
  public:
    [[nodiscard]] constexpr auto try_insert(const State& state) -> bool {
        if (std::ranges::find(discovered_, state) != discovered_.end()) {
            return false;
        }
        discovered_.push_back(state);
        return true;
    }

    /** The recorded states, in the order `try_insert` first accepted them. */
    [[nodiscard]] constexpr auto states() const noexcept -> std::span<const State> {
        return discovered_;
    }

    /** Moves the recorded states out, in the order `try_insert` first accepted them. */
    [[nodiscard]] constexpr auto extract() && -> std::vector<State> {
        return std::move(discovered_);
    }

  private:
    std::vector<State> discovered_;
};

static_assert(VisitedRegistry<linear_visited_registry<int>, int>);

}  // namespace mc_lab::algorithm::exploration

#endif  // MC_LAB_ALGORITHM_EXPLORATION_LINEAR_VISITED_REGISTRY_HPP
