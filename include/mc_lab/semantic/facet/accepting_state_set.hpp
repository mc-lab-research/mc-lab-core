#ifndef MC_LAB_SEMANTIC_FACET_ACCEPTING_STATE_SET_HPP
#define MC_LAB_SEMANTIC_FACET_ACCEPTING_STATE_SET_HPP

#include <mc_lab/semantic/detail/accepting_states_cpo.hpp>

#include <concepts>
#include <ranges>
#include <type_traits>
#include <utility>

/**
 * @file
 * Defines local access to a system's formal set of accepting states.
 *
 * In SemTL v0.1, a model provides a logically const member operation:
 *
 * @code
 * struct model {
 *   auto accepting_states() const;
 * };
 * @endcode
 *
 * Generic code uses the public `accepting_states(model)` CPO below rather
 * than calling that member. No base class, registration, ADL customization,
 * or SemTL traits specialization is involved.
 *
 * The explicit `State` concept argument keeps this facet independent from
 * `InitialStateSet<System>`, for the same reason `TransitionRelation` keeps
 * the relation independent from initial-state access: an algorithm that
 * already possesses a state domain can check accepting-state membership
 * without requiring a system to also expose initial states. A recognized
 * system such as `FiniteAutomaton` checks both facets against the same
 * normalized state domain, `state_t<System>`.
 */
namespace mc_lab::semantic {

/**
 * Returns a range representing the system's formal set of accepting states.
 */
inline constexpr detail::accepting_states_fn accepting_states{};

namespace detail {

template <class System>
concept AcceptingStatesCallable =
    requires(const std::remove_cvref_t<System>& system) { accepting_states(system); };

template <class System>
using accepting_state_range_t =
    decltype(accepting_states(std::declval<const std::remove_cvref_t<System>&>()));

// Keeping this check separate makes a missing operation reject the concept
// before range aliases are inspected, yielding a local constraint failure.
template <class System>
concept AcceptingStatesInputRange =
    AcceptingStatesCallable<System> && std::ranges::input_range<accepting_state_range_t<System>>;

// v0.1 accepts cv/ref variations of exactly the requested state type. It does
// not infer compatibility through conversions or shared common-reference
// types, which could silently mix distinct state domains.
template <class System, class State>
concept AcceptingStatesCompatible = AcceptingStatesInputRange<System>
    && std::same_as<std::remove_cvref_t<std::ranges::range_value_t<accepting_state_range_t<System>>>,
                    std::remove_cvref_t<State>>;

}  // namespace detail

/** Exact range type returned for logical-const access to the system. */
template <class System>
requires detail::AcceptingStatesCallable<System>
using accepting_state_range_t = detail::accepting_state_range_t<System>;

/** Reference, value, or proxy observed while iterating accepting states. */
template <class System>
requires detail::AcceptingStatesInputRange<System>
using accepting_state_reference_t = std::ranges::range_reference_t<accepting_state_range_t<System>>;

/**
 * A model exposing its formal accepting-state set as a single-pass range over
 * the supplied state domain.
 *
 * Compile-time contract:
 * - the public `accepting_states` CPO is callable with `const System&`;
 * - its result models `std::ranges::input_range`;
 * - after removing cv/ref qualifiers, the range's value type is exactly
 *   `State`.
 *
 * Semantic laws, not enforceable by the compiler:
 * - the range represents exactly the formal accepting-state set;
 * - every element denotes a valid state of the model;
 * - iteration order has no semantic meaning;
 * - duplicate occurrences do not change the represented set.
 *
 * Ownership and lifetime:
 * - the range may be an owning value or a view into system-owned storage;
 * - the system and any referenced storage must outlive range consumption;
 * - no global enumeration of the complete state space is implied.
 */
template <class System, class State>
concept AcceptingStateSet = detail::AcceptingStatesCompatible<System, State>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_FACET_ACCEPTING_STATE_SET_HPP
