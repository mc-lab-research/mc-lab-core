#ifndef MC_LAB_SEMANTIC_FACET_TRANSITION_RELATION_HPP
#define MC_LAB_SEMANTIC_FACET_TRANSITION_RELATION_HPP

#include <mc_lab/semantic/detail/transition_relation_cpos.hpp>

#include <concepts>
#include <ranges>
#include <type_traits>
#include <utility>

/**
 * @file
 * Defines local, representation-independent access to a transition relation.
 *
 * The state type is an explicit concept argument:
 *
 * @code
 * TransitionRelation<System, State>
 * @endcode
 *
 * This keeps the relation independent from `InitialStateSet`; algorithms that
 * already possess a state can query successors without requiring a system to
 * expose initial states or a globally enumerable state space.
 *
 * In SemTL v0.1, a model supplies member operations:
 *
 * @code
 * struct system {
 *   auto outgoing_transitions(const state&) const;
 *   auto target(const transition&) const;
 * };
 * @endcode
 *
 * Generic code invokes the public CPOs below, never these members directly.
 */
namespace mc_lab::semantic {

/**
 * Returns a range of local transition witnesses whose source is `state`.
 */
inline constexpr detail::outgoing_transitions_fn outgoing_transitions{};

/**
 * Returns the target state denoted by a local transition witness.
 */
inline constexpr detail::target_fn target{};

namespace detail {

template <class System, class State>
concept OutgoingTransitionsCallable =
    requires(const std::remove_cvref_t<System>& system, const std::remove_cvref_t<State>& state) {
        outgoing_transitions(system, state);
    };

template <class System, class State>
using outgoing_transition_range_t =
    decltype(outgoing_transitions(std::declval<const std::remove_cvref_t<System>&>(),
                                  std::declval<const std::remove_cvref_t<State>&>()));

template <class System, class State>
concept OutgoingTransitionsInputRange = OutgoingTransitionsCallable<System, State>
    && std::ranges::input_range<outgoing_transition_range_t<System, State>>;

template <class System, class State>
requires OutgoingTransitionsInputRange<System, State>
using transition_reference_for_t =
    std::ranges::range_reference_t<outgoing_transition_range_t<System, State>>;

template <class System, class State>
requires OutgoingTransitionsInputRange<System, State>
using transition_value_for_t =
    std::ranges::range_value_t<outgoing_transition_range_t<System, State>>;

// `target` is checked against the reference, value, or proxy actually produced
// by the transition range. No copy of a potentially move-only witness is
// required.
template <class System, class State>
concept TargetCallable = OutgoingTransitionsInputRange<System, State>
    && requires(const std::remove_cvref_t<System>& system,
                transition_reference_for_t<System, State> transition) {
           target(system, std::forward<transition_reference_for_t<System, State>>(transition));
       };

template <class System, class State>
requires TargetCallable<System, State>
using target_result_t = decltype(target(std::declval<const std::remove_cvref_t<System>&>(),
                                        std::declval<transition_reference_for_t<System, State>>()));

// v0.1 accepts cv/ref variations of exactly the requested state type. It does
// not infer compatibility through conversions or shared common-reference
// types, which could silently mix distinct state domains.
template <class System, class State>
concept StateCompatibleTarget = TargetCallable<System, State>
    && std::same_as<std::remove_cvref_t<target_result_t<System, State>>,
                    std::remove_cvref_t<State>>;

}  // namespace detail

/** Exact range type returned for the supplied system and state types. */
template <class System, class State>
requires detail::OutgoingTransitionsCallable<System, State>
using outgoing_transition_range_t = detail::outgoing_transition_range_t<System, State>;

/** Reference, value, or proxy observed while iterating transitions. */
template <class System, class State>
requires detail::OutgoingTransitionsInputRange<System, State>
using transition_reference_for_t = detail::transition_reference_for_t<System, State>;

/** Stable transition value type represented by the outgoing range. */
template <class System, class State>
requires detail::OutgoingTransitionsInputRange<System, State>
using transition_value_for_t = detail::transition_value_for_t<System, State>;

/** Exact result type returned by `target` for the range's transition form. */
template <class System, class State>
requires detail::TargetCallable<System, State>
using target_result_t = detail::target_result_t<System, State>;

/**
 * A model exposing the local image of a transition relation for `State`.
 *
 * Compile-time contract:
 * - the public `outgoing_transitions` CPO can call the corresponding member on
 *   `const System&` and `const State&`;
 * - the result is a single-pass input range;
 * - the public `target` CPO accepts the range's actual transition reference or
 *   value;
 * - after removing cv/ref qualifiers, the target result is exactly `State`.
 *
 * Semantic laws, not enforceable by the compiler:
 * - each returned witness represents a transition whose source is the queried
 *   state;
 * - `target(system, witness)` represents that transition's formal target;
 * - iteration is sound and complete for the represented local image.
 *
 * Witness lifetime:
 * - a witness is guaranteed only while its producing range permits access;
 * - incrementing a single-pass iterator may invalidate an earlier witness;
 * - the concept does not promise persistent transition evidence suitable for
 *   predecessor maps, executions, or counterexamples.
 */
template <class System, class State>
concept TransitionRelation = detail::StateCompatibleTarget<System, State>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_FACET_TRANSITION_RELATION_HPP
