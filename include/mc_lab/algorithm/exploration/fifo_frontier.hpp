#ifndef MC_LAB_ALGORITHM_EXPLORATION_FIFO_FRONTIER_HPP
#define MC_LAB_ALGORITHM_EXPLORATION_FIFO_FRONTIER_HPP

#include <mc_lab/algorithm/exploration/frontier.hpp>

#include <concepts>
#include <cstddef>
#include <utility>
#include <vector>

/**
 * @file
 * Defines the first-in-first-out `Frontier` policy used for breadth-first
 * exploration.
 */
namespace mc_lab::algorithm::exploration {

/**
 * A `Frontier` that examines states in the order they were pushed.
 *
 * Representation: an appended `std::vector<State>` (`storage_`) read through
 * an advancing cursor (`cursor_`), rather than a two-ended queue such as
 * `std::deque`.
 *
 * - `push` copy-constructs its argument and appends it with `push_back`;
 *   pushed states are therefore always added at the end of `storage_`.
 * - `pop` moves the element at `storage_[cursor_]` out by value and
 *   advances `cursor_`; it never returns a reference into `storage_`.
 * - `empty()` is exactly `cursor_ == storage_.size()`.
 * - the class invariant `cursor_ <= storage_.size()` holds at every
 *   observable point: `cursor_` only ever advances by one inside `pop`,
 *   which is only called when `empty()` is `false`, i.e. when
 *   `cursor_ < storage_.size()`.
 *
 * This representation keeps every operation usable in a `constexpr` context
 * (a two-ended queue is not constexpr-friendly on this library's target
 * standard) and matches the discovery-order storage already used by the
 * algorithm this policy was extracted from.
 *
 * Memory tradeoff, stated precisely: popped elements are not reclaimed. They
 * remain physically stored in `storage_` until the `fifo_frontier` itself is
 * destroyed; there is no compaction in this increment, and none is planned
 * without a concrete need, since compaction would introduce its own
 * invalidation and complexity decisions. Consequently, peak storage is
 * proportional to the *total* number of states ever pushed over the
 * frontier's lifetime, not to the maximum number of states simultaneously
 * live in the frontier (its "width"). This is materially different from a
 * `std::queue` backed by `std::deque`, which reclaims a dequeued element's
 * storage and whose memory footprint tracks the live queue size. For
 * exploration, where the total pushed count is already bounded by the
 * reachable state count that the caller is materializing, this is an
 * acceptable v0.1 tradeoff, not a claim of equivalence to a reclaiming queue.
 *
 * References obtained from `storage_` are not stable across `push`: like any
 * `std::vector`, appending may reallocate and invalidate every previously
 * obtained reference, pointer, or iterator into it. `pop` returning `State`
 * by value, never by reference, is what makes this safe to use across a
 * `push` that may reallocate.
 *
 * A drained `fifo_frontier` remains usable: `push` after `empty()` becomes
 * `true` continues to append at the end of `storage_` and is popped in the
 * same first-in-first-out order as before draining.
 *
 * `State` must be copy-constructible: `push` copies its argument (see
 * `Frontier`'s own documentation for why the generic kernels call `push`
 * with an lvalue), and the returned-by-value `pop` additionally requires it
 * to be move-constructible, which `copy_constructible` already implies. This
 * requirement belongs to this policy's own storage, not to the `Frontier`
 * protocol or to the generic exploration kernels, which impose nothing on
 * `State` beyond what the policy they are actually given requires.
 */
template <class State>
requires std::copy_constructible<State>
class fifo_frontier {
  public:
    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return cursor_ == storage_.size();
    }

    constexpr void push(State state) { storage_.push_back(std::move(state)); }

    [[nodiscard]] constexpr auto pop() -> State { return std::move(storage_[cursor_++]); }

  private:
    std::vector<State> storage_;
    std::size_t cursor_ = 0;
};

static_assert(Frontier<fifo_frontier<int>, int>);

}  // namespace mc_lab::algorithm::exploration

#endif  // MC_LAB_ALGORITHM_EXPLORATION_FIFO_FRONTIER_HPP
