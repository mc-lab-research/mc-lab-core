#ifndef MC_LAB_SEMANTIC_SYSTEM_FINITE_AUTOMATON_HPP
#define MC_LAB_SEMANTIC_SYSTEM_FINITE_AUTOMATON_HPP

#include <mc_lab/semantic/acceptance/acceptance_condition.hpp>
#include <mc_lab/semantic/acceptance/final_state_acceptance.hpp>
#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/transition_labelling.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>

/**
 * @file
 * Recognizes the finite-automaton formalism over an alphabet-labelled
 * transition relation.
 *
 * A finite automaton is a tuple `A = (Q, Sigma, delta, Q0, F)`:
 * - `Q` is its state domain;
 * - `Sigma` is its alphabet;
 * - `delta` is a relation over `Q x Sigma x Q`;
 * - `Q0` is a subset of `Q` containing its initial states;
 * - `F` is a subset of `Q` containing its accepting states.
 *
 * SemTL obtains `Q` from `InitialStateSet<System>`, the local image of `delta`
 * from `TransitionRelation<System, state_t<System>>`, and its labels from the
 * independent `TransitionLabelling` facet. Acceptance is associated through
 * `acceptance_condition(system)` and interpreted as final-state acceptance.
 */
namespace mc_lab::semantic {

namespace detail {

// The staged concepts keep dependent aliases behind the contracts that make
// them well-formed. Each requirement remains an atomic public contract.
template <class System>
concept FiniteAutomatonTransitionStructure =
    InitialStateSet<System> && TransitionRelation<System, state_t<System>>
    && TransitionLabelling<System, transition_reference_for_t<System, state_t<System>>>;

template <class System>
concept FiniteAutomatonAcceptance = HasAcceptanceCondition<System>
    && FinalStateAcceptanceCondition<acceptance_condition_t<System>, state_t<System>>;

template <class System>
concept FiniteAutomatonComponents =
    FiniteAutomatonTransitionStructure<System> && FiniteAutomatonAcceptance<System>;

}  // namespace detail

/**
 * The alphabet symbol type carried by a finite automaton's transitions.
 *
 * This is a domain alias over the neutral `transition_label` result, in the
 * same spirit as `action_t` for a labelled transition system: it introduces
 * no separate customization mechanism, only a name.
 */
template <class System>
requires detail::FiniteAutomatonComponents<System>
using symbol_t =
    transition_label_for_t<System, transition_reference_for_t<System, state_t<System>>>;

/**
 * A system recognized directly as a finite automaton.
 *
 * Compile-time contract:
 * - `InitialStateSet<System>` exposes `Q0` and determines `Q`;
 * - `TransitionRelation<System, state_t<System>>` exposes the local image of
 *   `delta` on `Q`;
 * - `TransitionLabelling` assigns each transition its symbol in `Sigma`;
 * - `HasAcceptanceCondition<System>` associates an independent condition
 *   object with the system;
 * - `FinalStateAcceptanceCondition<acceptance_condition_t<System>,
 *   state_t<System>>` interprets that object over the same state domain.
 *
 * Semantic laws, not enforceable by the compiler:
 * - every initial and accepting value denotes an element of `Q`;
 * - every transition source, target, and label denote elements of `Q`, `Q`,
 *   and `Sigma` respectively;
 * - the initial range and acceptance condition represent exactly `Q0` and
 *   final-state acceptance by `F`;
 * - local transition queries are sound and complete for `delta`.
 *
 * Language, as a documented behavioral definition rather than an algorithm: a
 * word `sigma_0 ... sigma_k` is accepted by `A` exactly when there exists an
 * execution `q_0 ... q_(k+1)` such that `q_0` is in `Q0`, every
 * `(q_i, sigma_i, q_(i+1))` is in `delta` for `i` in `[0, k]`, and `q_(k+1)`
 * is in `F`. This concept recognizes only the structural formalism; checking
 * whether a concrete word is accepted, or computing the language `[[A]]`,
 * is a separate computational capability, consistent with SemTL deliberately
 * defining no `executions(system)` CPO for `TransitionSystem`.
 *
 * `FiniteAutomaton` does not refine `TransitionSystem` or `KripkeStructure`.
 * Recognized systems compose facets directly because each formalism also
 * introduces its own semantic laws.
 */
template <class System>
concept FiniteAutomaton = detail::FiniteAutomatonComponents<System>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_SYSTEM_FINITE_AUTOMATON_HPP
