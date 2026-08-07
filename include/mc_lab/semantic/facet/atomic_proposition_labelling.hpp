#ifndef MC_LAB_SEMANTIC_FACET_ATOMIC_PROPOSITION_LABELLING_HPP
#define MC_LAB_SEMANTIC_FACET_ATOMIC_PROPOSITION_LABELLING_HPP

#include <mc_lab/semantic/detail/atomic_propositions_cpo.hpp>

#include <ranges>
#include <type_traits>
#include <utility>

/**
 * @file
 * Defines the formal state-labelling function L : State -> 2^AP.
 *
 * In SemTL v0.1, a model supplies a logically const member operation:
 *
 * @code
 * struct system {
 *   auto atomic_propositions(const state&) const;
 * };
 * @endcode
 *
 * Generic code invokes the public CPO below rather than the member directly.
 * The result is range-shaped for computational access but has set semantics:
 * its order is not part of the formal model.
 */
namespace mc_lab::semantic {

/** Returns a range representing the propositions true in `state`. */
inline constexpr detail::atomic_propositions_fn atomic_propositions{};

namespace detail {

template <class System, class State>
concept AtomicPropositionsCallable =
    requires(const std::remove_cvref_t<System>& system, const std::remove_cvref_t<State>& state) {
        atomic_propositions(system, state);
    };

template <class System, class State>
using atomic_proposition_range_t =
    decltype(atomic_propositions(std::declval<const std::remove_cvref_t<System>&>(),
                                 std::declval<const std::remove_cvref_t<State>&>()));

template <class System, class State>
concept AtomicPropositionsInputRange = AtomicPropositionsCallable<System, State>
    && std::ranges::input_range<atomic_proposition_range_t<System, State>>;

}  // namespace detail

/** Exact range type returned for the supplied system and state types. */
template <class System, class State>
requires detail::AtomicPropositionsCallable<System, State>
using atomic_proposition_range_t = detail::atomic_proposition_range_t<System, State>;

/** Stable proposition value type represented by the returned range. */
template <class System, class State>
requires detail::AtomicPropositionsInputRange<System, State>
using atomic_proposition_for_t =
    std::ranges::range_value_t<atomic_proposition_range_t<System, State>>;

/**
 * A model exposing the propositions true in a supplied state.
 *
 * Compile-time contract:
 * - the public `atomic_propositions` CPO is callable with `const System&`
 *   and `const State&`;
 * - its result models `std::ranges::input_range`.
 *
 * Semantic laws, not enforceable by the compiler:
 * - the range represents exactly L(state);
 * - iteration order has no semantic meaning;
 * - duplicate occurrences do not change the represented proposition set.
 *
 * The range may own its values or borrow storage from the model. Any borrowed
 * storage must remain valid for the documented consumption period.
 */
template <class System, class State>
concept AtomicPropositionLabelling = detail::AtomicPropositionsInputRange<System, State>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_FACET_ATOMIC_PROPOSITION_LABELLING_HPP
