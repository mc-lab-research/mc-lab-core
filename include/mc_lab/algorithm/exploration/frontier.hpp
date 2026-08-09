#ifndef MC_LAB_ALGORITHM_EXPLORATION_FRONTIER_HPP
#define MC_LAB_ALGORITHM_EXPLORATION_FRONTIER_HPP

#include <concepts>
#include <utility>

/**
 * @file
 * Defines the exploration-order policy protocol consumed by the generic
 * exploration kernels.
 *
 * A `Frontier` holds states awaiting examination and decides the order in
 * which they are examined. It carries no discovery/deduplication
 * responsibility of its own: that is the separate `VisitedRegistry`
 * policy's role, so an exploration kernel always pairs a `Frontier` with a
 * `VisitedRegistry`.
 *
 * A model exposes this protocol through three ordinary member operations:
 *
 * @code
 * struct frontier {
 *   auto empty() const -> bool;
 *   void push(State state);
 *   auto pop() -> State;
 * };
 * @endcode
 *
 * The generic exploration kernels call `push` with an lvalue: after deciding
 * a state is newly discovered, they push it and then still pass that same
 * value to the observer's tree-edge and discovery callbacks. `push` must
 * therefore be usable without consuming its argument's later usability; a
 * by-value parameter copy-constructed from that lvalue satisfies this
 * naturally. This is why `State` must be copy-constructible for any
 * `Frontier` a kernel actually uses, even though this protocol concept adds
 * no separate constraint of its own beyond what the expression below
 * requires for a *given* `F`.
 *
 * This is an algorithmic policy protocol, not a `mc_lab::semantic` structural
 * facet: it describes an algorithm-owned auxiliary object, a representation
 * choice of how exploration order is tracked, not a property of the semantic
 * system being explored. It is therefore deliberately not exposed through a
 * member-only CPO the way SemTL facets are; direct member calls are the
 * whole protocol here.
 */
namespace mc_lab::algorithm::exploration {

/**
 * A policy governing the order in which discovered states are examined.
 *
 * Compile-time contract:
 * - `frontier.empty()` is contextually convertible to `bool`;
 * - `frontier.push(state)` accepts an rvalue `State`;
 * - `frontier.pop()` returns exactly `State`.
 *
 * Semantic contract, not enforceable by the compiler:
 * - `pop()` must only be called when `empty()` is `false`;
 * - `pop()` removes and returns one previously pushed state, chosen
 *   according to the policy's own ordering discipline (for example,
 *   first-in-first-out for `fifo_frontier`);
 * - a `Frontier` does not decide whether a state has already been seen; a
 *   caller pairs it with a `VisitedRegistry` for that decision.
 *
 * This concept intentionally imposes nothing on `State` beyond what these
 * three expressions require for a *given* `F`. A concrete policy such as
 * `fifo_frontier<State>` may itself require `State` to be copy-constructible
 * for its own storage; that requirement belongs to the policy, not to this
 * protocol concept or to the generic exploration kernels that consume it.
 */
template <class F, class State>
concept Frontier = requires(F& frontier, State state) {
    { frontier.empty() } -> std::convertible_to<bool>;
    frontier.push(state);
    { frontier.pop() } -> std::same_as<State>;
};

}  // namespace mc_lab::algorithm::exploration

#endif  // MC_LAB_ALGORITHM_EXPLORATION_FRONTIER_HPP
