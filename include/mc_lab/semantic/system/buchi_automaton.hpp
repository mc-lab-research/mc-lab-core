#ifndef MC_LAB_SEMANTIC_SYSTEM_BUCHI_AUTOMATON_HPP
#define MC_LAB_SEMANTIC_SYSTEM_BUCHI_AUTOMATON_HPP

#include <mc_lab/semantic/acceptance/acceptance_condition.hpp>
#include <mc_lab/semantic/acceptance/buchi_acceptance.hpp>
#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/transition_labelling.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>

/**
 * @file
 * Recognizes the Buchi-automaton formalism over infinite words.
 *
 * A Buchi automaton is defined the same way as a finite automaton: a tuple
 * `A = (Q, Sigma, delta, Q0, F)`. Its structural components are therefore
 * identical to `FiniteAutomaton`. Only its language differs: a Buchi
 * automaton accepts infinite words under a recurrence condition on `F`,
 * rather than finite words under a reachability condition on `F`.
 */
namespace mc_lab::semantic {

namespace detail {

// The staged concepts keep dependent aliases behind the contracts that make
// them well-formed. Each requirement remains an atomic public contract.
template <class System>
concept BuchiAutomatonTransitionStructure =
    InitialStateSet<System> && TransitionRelation<System, state_t<System>>
    && TransitionLabelling<System, transition_reference_for_t<System, state_t<System>>>;

template <class System>
concept BuchiAutomatonAcceptance = HasAcceptanceCondition<System>
    && BuchiAcceptanceCondition<acceptance_condition_t<System>, state_t<System>>;

template <class System>
concept BuchiAutomatonComponents =
    BuchiAutomatonTransitionStructure<System> && BuchiAutomatonAcceptance<System>;

}  // namespace detail

/**
 * A system recognized directly as a Buchi automaton.
 *
 * Compile-time contract:
 * - identical in shape to `FiniteAutomaton`: `InitialStateSet<System>` exposes
 *   `Q0` and determines `Q`; `TransitionRelation` exposes the local image of
 *   `delta`; `TransitionLabelling` exposes `Sigma`; and
 *   `HasAcceptanceCondition` associates a condition object;
 * - `BuchiAcceptanceCondition<acceptance_condition_t<System>,
 *   state_t<System>>` gives that object recurrence semantics over `Q`.
 *
 * Semantic laws, not enforceable by the compiler: the same structural laws as
 * `FiniteAutomaton` (valid states, sound and complete `Q0`/`delta`/`F`), plus
 * the Buchi-specific acceptance law over infinite runs:
 *
 * A run of `A` on an infinite word `sigma_0 sigma_1 ...` is an infinite state
 * sequence `q_0 q_1 ...` such that `q_0` is in `Q0` and every
 * `(q_i, sigma_i, q_(i+1))` is in `delta`. Writing `inf(run)` for the set of
 * states that occur infinitely often in `run`, `A` accepts the word exactly
 * when some run on it satisfies:
 *
 * @code
 * inf(run) intersect F != {}
 * @endcode
 *
 * i.e. the run visits at least one accepting state infinitely often. The
 * language `[[A]]` is the set of infinite words accepted this way.
 *
 * As with `FiniteAutomaton`, this concept recognizes only the structural
 * formalism. `inf(run)` is a property of a specific infinite value, not a
 * type; checking it, checking word acceptance, or computing `[[A]]` are
 * separate computational capabilities, not part of this concept.
 *
 * `BuchiAutomaton` does not refine `FiniteAutomaton`, `TransitionSystem`, or
 * `KripkeStructure`, even though its structural requirements currently
 * coincide with `FiniteAutomaton`. Recognized systems compose facets
 * directly because each formalism also introduces its own semantic laws, and
 * a future increment could give either formalism additional structural
 * requirements without forcing the other to change.
 */
template <class System>
concept BuchiAutomaton = detail::BuchiAutomatonComponents<System>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_SYSTEM_BUCHI_AUTOMATON_HPP
