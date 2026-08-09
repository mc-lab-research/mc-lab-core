#ifndef MC_LAB_SEMANTIC_SYSTEM_KRIPKE_STRUCTURE_HPP
#define MC_LAB_SEMANTIC_SYSTEM_KRIPKE_STRUCTURE_HPP

#include <mc_lab/semantic/facet/atomic_proposition_labelling.hpp>
#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>

/**
 * @file
 * Recognizes state-labelled transition systems with Kripke semantics.
 *
 * A Kripke structure is a tuple `K = (S, R, I, AP, L)`, where `R` is a
 * total relation over `S` and `L : S -> 2^AP` labels each state with the
 * propositions true in that state. Transitions carry no action or symbol.
 */
namespace mc_lab::semantic {

namespace detail {

// The dependency order makes the state domain available before the labelling
// facet is formed. This is composition of contracts, not system inheritance.
template <class System>
concept KripkeStructureComponents =
    InitialStateSet<System> && TransitionRelation<System, state_t<System>>
    && AtomicPropositionLabelling<System, state_t<System>>;

}  // namespace detail

/**
 * A transition system whose states are labelled by atomic propositions.
 *
 * Compile-time contract:
 * - `InitialStateSet<System>` exposes `I` and determines `S`;
 * - `TransitionRelation<System, state_t<System>>` exposes `R` on `S`;
 * - `AtomicPropositionLabelling<System, state_t<System>>` exposes `L` on the
 *   same normalized state domain.
 *
 * Kripke-specific semantic law, not enforceable by the compiler:
 *
 * @code
 * for every valid state s, there exists a valid state t such that s R t
 * @endcode
 *
 * Thus `R` is total: no valid state is a deadlock. Every finite path fragment
 * can be extended, and every maximal execution is infinite.
 *
 * The concept deliberately requires neither action-labelled transitions nor
 * state-space enumeration. Enumeration is a computational capability that a
 * future conformance utility may use to check totality for finite models.
 *
 * `KripkeStructure` does not refine another system concept. Recognized systems
 * compose facets directly because each formalism also introduces its own
 * semantic laws.
 */
template <class System>
concept KripkeStructure = detail::KripkeStructureComponents<System>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_SYSTEM_KRIPKE_STRUCTURE_HPP
