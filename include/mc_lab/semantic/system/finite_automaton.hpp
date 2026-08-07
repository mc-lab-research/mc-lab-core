#ifndef MC_LAB_SEMANTIC_SYSTEM_FINITE_AUTOMATON_HPP
#define MC_LAB_SEMANTIC_SYSTEM_FINITE_AUTOMATON_HPP

#include <mc_lab/semantic/facet/accepting_state_set.hpp>
#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/labelled_transition_relation.hpp>

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
 * SemTL obtains `Q` from `InitialStateSet<System>`, `delta` from
 * `LabelledTransitionRelation<System, state_t<System>>` (whose structural
 * label type is aliased below as `symbol_t`), and `F` from
 * `AcceptingStateSet<System, state_t<System>>`.
 */
namespace mc_lab::semantic {

namespace detail {

// The dependency order makes the state domain available before the labelled
// relation and accepting-state facets are formed. This is composition of
// contracts, not system inheritance.
template <class System>
concept FiniteAutomatonComponents =
    InitialStateSet<System> && LabelledTransitionRelation<System, state_t<System>>
    && AcceptingStateSet<System, state_t<System>>;

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
 * A labelled transition system recognized as a finite automaton.
 *
 * Compile-time contract:
 * - `InitialStateSet<System>` exposes `Q0` and determines `Q`;
 * - `LabelledTransitionRelation<System, state_t<System>>` exposes `delta` on
 *   `Q`, with transitions labelled by `Sigma` (`symbol_t<System>`);
 * - `AcceptingStateSet<System, state_t<System>>` exposes `F` on the same
 *   normalized state domain.
 *
 * Semantic laws, not enforceable by the compiler:
 * - every initial and accepting value denotes an element of `Q`;
 * - every transition source, target, and label denote elements of `Q`, `Q`,
 *   and `Sigma` respectively;
 * - the initial and accepting ranges represent exactly `Q0` and `F`;
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
