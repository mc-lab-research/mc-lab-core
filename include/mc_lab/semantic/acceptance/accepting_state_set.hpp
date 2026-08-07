#ifndef MC_LAB_SEMANTIC_ACCEPTANCE_ACCEPTING_STATE_SET_HPP
#define MC_LAB_SEMANTIC_ACCEPTANCE_ACCEPTING_STATE_SET_HPP

#include <mc_lab/semantic/detail/accepting_states_cpo.hpp>

#include <concepts>
#include <ranges>
#include <type_traits>
#include <utility>

/**
 * @file
 * Defines accepting-state-set conditions independently from systems.
 *
 * This representation is useful for final-state and Buchi acceptance. It is
 * deliberately not the universal basis of acceptance: generalized Buchi,
 * parity, Rabin, and Streett conditions may expose richer structures.
 */
namespace mc_lab::semantic {

/** Returns the formal accepting-state set represented by a condition. */
inline constexpr detail::accepting_states_fn accepting_states{};

namespace detail {

template <class Condition>
concept AcceptingStatesCallable =
    requires(const std::remove_cvref_t<Condition>& condition) { accepting_states(condition); };

template <class Condition>
using accepting_state_range_t =
    decltype(accepting_states(std::declval<const std::remove_cvref_t<Condition>&>()));

template <class Condition>
concept AcceptingStatesInputRange = AcceptingStatesCallable<Condition>
    && std::ranges::input_range<accepting_state_range_t<Condition>>;

template <class Condition, class State>
concept AcceptingStatesCompatible =
    AcceptingStatesInputRange<Condition>
    && std::same_as<
        std::remove_cvref_t<std::ranges::range_value_t<accepting_state_range_t<Condition>>>,
        std::remove_cvref_t<State>>;

}  // namespace detail

/** Exact range type returned for logical-const access to the condition. */
template <class Condition>
requires detail::AcceptingStatesCallable<Condition>
using accepting_state_range_t = detail::accepting_state_range_t<Condition>;

/** Reference, value, or proxy observed while iterating accepting states. */
template <class Condition>
requires detail::AcceptingStatesInputRange<Condition>
using accepting_state_reference_t =
    std::ranges::range_reference_t<accepting_state_range_t<Condition>>;

/**
 * A condition object exposing an accepting-state set over `State`.
 *
 * Compile-time contract:
 * - `accepting_states(condition)` is available on `const Condition&`;
 * - its result models `std::ranges::input_range`;
 * - its normalized value type is exactly `State`.
 *
 * Semantic laws:
 * - the range denotes exactly the condition's formal accepting set;
 * - every element denotes a valid state in the associated system;
 * - iteration order and duplicate occurrences have no semantic meaning.
 *
 * The valid-state law relates a condition value to its associated system and
 * therefore remains a conformance obligation rather than a type constraint.
 */
template <class Condition, class State>
concept AcceptingStateSet = detail::AcceptingStatesCompatible<Condition, State>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_ACCEPTANCE_ACCEPTING_STATE_SET_HPP
