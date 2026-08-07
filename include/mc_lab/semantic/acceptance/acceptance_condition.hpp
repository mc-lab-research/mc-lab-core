#ifndef MC_LAB_SEMANTIC_ACCEPTANCE_ACCEPTANCE_CONDITION_HPP
#define MC_LAB_SEMANTIC_ACCEPTANCE_ACCEPTANCE_CONDITION_HPP

#include <mc_lab/semantic/detail/acceptance_condition_cpo.hpp>

#include <type_traits>
#include <utility>

/**
 * @file
 * Defines the association between a system and its acceptance condition.
 *
 * Acceptance is not a structural facet of a system. A system associates an
 * independent condition object through a logically const member operation:
 *
 * @code
 * struct system {
 *   auto acceptance_condition() const;
 * };
 * @endcode
 *
 * Generic code uses the public CPO. The condition object's own concept then
 * determines how finite or infinite executions are interpreted.
 */
namespace mc_lab::semantic {

/** Returns the acceptance-condition object associated with a system. */
inline constexpr detail::acceptance_condition_fn acceptance_condition{};

namespace detail {

template <class System>
concept AcceptanceConditionCallable =
    requires(const std::remove_cvref_t<System>& system) { acceptance_condition(system); };

template <class System>
requires AcceptanceConditionCallable<System>
using acceptance_condition_result_t =
    decltype(acceptance_condition(std::declval<const std::remove_cvref_t<System>&>()));

template <class System>
concept AcceptanceConditionObject = AcceptanceConditionCallable<System>
    && std::is_object_v<std::remove_cvref_t<acceptance_condition_result_t<System>>>;

}  // namespace detail

/** Stable type of the condition object associated with a system. */
template <class System>
requires detail::AcceptanceConditionCallable<System>
using acceptance_condition_t = std::remove_cvref_t<detail::acceptance_condition_result_t<System>>;

/**
 * A system associated with an acceptance-condition object.
 *
 * This concept establishes only the association. It does not prescribe the
 * condition's representation or claim final-state, Buchi, generalized Buchi,
 * parity, Rabin, or Streett semantics. Those contracts apply to the returned
 * condition object through separate acceptance-condition concepts.
 *
 * The CPO may return an owned value or a reference into the system. Callers
 * must respect the lifetime guaranteed by that result.
 */
template <class System>
concept HasAcceptanceCondition = detail::AcceptanceConditionObject<System>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_ACCEPTANCE_ACCEPTANCE_CONDITION_HPP
