#ifndef MC_LAB_SEMANTIC_FACET_INITIAL_STATE_SET_HPP
#define MC_LAB_SEMANTIC_FACET_INITIAL_STATE_SET_HPP

#include <mc_lab/semantic/detail/initial_states_cpo.hpp>

#include <ranges>
#include <type_traits>
#include <utility>

/**
 * @file
 * Defines local access to a system's formal set of initial states.
 *
 * In SemTL v0.1, a model provides a logically const member operation:
 *
 * @code
 * struct model {
 *   auto initial_states() const;
 * };
 * @endcode
 *
 * Generic code uses the public `initial_states(model)` CPO below rather than
 * calling that member. No base class, registration, ADL customization, or
 * SemTL traits specialization is involved.
 *
 * The standard `remove_cvref_t` transformation below only makes `Model`,
 * `Model&`, and `const Model&` refer to the same model type. It is type
 * normalization, not a user-customizable traits mechanism.
 */
namespace mc_lab::semantic {

/**
 * Returns a range representing the system's formal set of initial states.
 */
inline constexpr detail::initial_states_fn initial_states{};

namespace detail {

template <class System>
concept InitialStatesCallable =
    requires(const std::remove_cvref_t<System>& system) { initial_states(system); };

template <class System>
using initial_state_range_t =
    decltype(initial_states(std::declval<const std::remove_cvref_t<System>&>()));

// Keeping this check separate makes a missing operation reject the concept
// before range aliases are inspected, yielding a local constraint failure.
template <class System>
concept InitialStatesInputRange =
    InitialStatesCallable<System> && std::ranges::input_range<initial_state_range_t<System>>;

}  // namespace detail

/** Exact range type returned for logical-const access to the system. */
template <class System>
requires detail::InitialStatesCallable<System>
using initial_state_range_t = detail::initial_state_range_t<System>;

/** Reference, value, or proxy observed while iterating initial states. */
template <class System>
requires detail::InitialStatesInputRange<System>
using initial_state_reference_t = std::ranges::range_reference_t<initial_state_range_t<System>>;

/** Stable state value type represented by the initial-state range. */
template <class System>
requires detail::InitialStatesInputRange<System>
using state_t = std::ranges::range_value_t<initial_state_range_t<System>>;

/**
 * A model exposing its formal initial-state set as a single-pass range.
 *
 * Compile-time contract:
 * - the public `initial_states` CPO is callable with `const System&`;
 * - its result models `std::ranges::input_range`.
 *
 * Semantic laws, not enforceable by the compiler:
 * - the range represents exactly the formal initial-state set;
 * - every element denotes a valid state of the model;
 * - iteration order has no semantic meaning;
 * - duplicate occurrences do not change the represented set.
 *
 * Ownership and lifetime:
 * - the range may be an owning value or a view into system-owned storage;
 * - the system and any referenced storage must outlive range consumption;
 * - no global enumeration of the complete state space is implied.
 */
template <class System>
concept InitialStateSet = detail::InitialStatesInputRange<System>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_FACET_INITIAL_STATE_SET_HPP
