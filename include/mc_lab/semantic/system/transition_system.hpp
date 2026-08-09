#ifndef MC_LAB_SEMANTIC_SYSTEM_TRANSITION_SYSTEM_HPP
#define MC_LAB_SEMANTIC_SYSTEM_TRANSITION_SYSTEM_HPP

#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>

/**
 * @file
 * Recognizes the minimal, unlabelled transition-system formalism.
 *
 * Mathematically, a transition system is a tuple `TS = (S, trans, I)`:
 * - `S` is its state domain;
 * - `trans` is a binary relation over `S`;
 * - `I` is a subset of `S` containing its initial states.
 *
 * SemTL obtains the state type from `InitialStateSet<System>` and checks the
 * transition relation on that exact domain. The concept deliberately requires
 * neither labels, acceptance, nor a globally enumerable state space.
 */
namespace mc_lab::semantic {

namespace detail {

// Keeping the conjunction in dependency order prevents `state_t` from being
// formed before initial-state access has satisfied its range contract.
template <class System>
concept TransitionSystemStructure =
    InitialStateSet<System> && TransitionRelation<System, state_t<System>>;

}  // namespace detail

/**
 * A minimal transition system composed from structural semantic facets.
 *
 * Compile-time contract:
 * - `InitialStateSet<System>` exposes `I` and determines `state_t<System>`;
 * - `TransitionRelation<System, state_t<System>>` exposes `trans` on the same
 *   normalized state type.
 *
 * Semantic laws, not enforceable by the compiler:
 * - every initial value denotes an element of `S`;
 * - every transition source and target denote elements of `S`;
 * - the initial range represents exactly `I`;
 * - local transition queries are sound and complete for `trans`.
 *
 * `S` is a semantic domain, not an enumeration requirement. A model may have
 * an infinite or implicit state space and still satisfy `TransitionSystem`.
 * Computational facilities such as global enumeration, indexing, and
 * predecessor queries remain separate capabilities.
 *
 * This concept recognizes a system formalism; it is not a base class and does
 * not introduce a customization mechanism beyond its component facets.
 */
template <class System>
concept TransitionSystem = detail::TransitionSystemStructure<System>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_SYSTEM_TRANSITION_SYSTEM_HPP
