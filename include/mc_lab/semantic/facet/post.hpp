#ifndef MC_LAB_SEMANTIC_FACET_POST_HPP
#define MC_LAB_SEMANTIC_FACET_POST_HPP

#include <mc_lab/semantic/detail/post_cpo.hpp>

#include <concepts>
#include <ranges>
#include <type_traits>
#include <utility>

/**
 * @file
 * Defines direct, state-oriented access to successor states.
 *
 * `Post` models the local semantic operation:
 *
 * @code
 * State -> Range<State>
 * @endcode
 *
 * It is independent from `TransitionRelation`, which instead exposes
 * transition witnesses and their targets. A system may provide either facet
 * or both without one refining the other.
 *
 * In SemTL v0.1, a model supplies a logically const member operation:
 *
 * @code
 * struct system {
 *   auto post(const state& source) const;
 * };
 * @endcode
 *
 * Generic code invokes the public CPO below rather than the member directly.
 */
namespace mc_lab::semantic {

/** Returns a range representing the formal successor set of `state`. */
inline constexpr detail::post_fn post{};

namespace detail {

template <class System, class State>
concept PostCallable = requires(const std::remove_cvref_t<System>& system,
                                const std::remove_cvref_t<State>& state) { post(system, state); };

template <class System, class State>
using post_range_for_t = decltype(post(std::declval<const std::remove_cvref_t<System>&>(),
                                       std::declval<const std::remove_cvref_t<State>&>()));

template <class System, class State>
concept PostInputRange =
    PostCallable<System, State> && std::ranges::input_range<post_range_for_t<System, State>>;

template <class System, class State>
requires PostInputRange<System, State>
using post_reference_for_t = std::ranges::range_reference_t<post_range_for_t<System, State>>;

template <class System, class State>
concept PostStateCompatible =
    PostInputRange<System, State>
    && std::same_as<
        std::remove_cvref_t<std::ranges::range_value_t<post_range_for_t<System, State>>>,
        std::remove_cvref_t<State>>;

}  // namespace detail

/** Exact range type returned for the supplied system and state domain. */
template <class System, class State>
requires detail::PostCallable<System, State>
using post_range_for_t = detail::post_range_for_t<System, State>;

/** Reference, value, or proxy observed while iterating successor states. */
template <class System, class State>
requires detail::PostInputRange<System, State>
using post_reference_for_t = detail::post_reference_for_t<System, State>;

/**
 * A model exposing the direct successor-state image of `State`.
 *
 * Compile-time contract:
 * - `post(system, state)` is callable with `const System&` and `const State&`;
 * - the result models `std::ranges::input_range`;
 * - after removing cv/ref qualifiers, its value type is exactly `State`.
 *
 * Semantic laws for a query from `source`:
 * - every produced value denotes a valid successor state of `source`;
 * - the range is sound and complete for the represented successor set;
 * - iteration order has no semantic meaning;
 * - duplicate occurrences do not change the represented set.
 *
 * The range may be empty, owning, borrowed, lazy, generated, or single-pass.
 * The concept implies neither determinism, finiteness, global state-space
 * enumeration, nor persistent ownership of observed states.
 *
 * `Post` does not require `InitialStateSet`: an algorithm that already owns a
 * state may query its successors without requiring an initial-state domain.
 * It also does not require or synthesize transition witnesses.
 */
template <class System, class State>
concept Post = detail::PostStateCompatible<System, State>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_FACET_POST_HPP
