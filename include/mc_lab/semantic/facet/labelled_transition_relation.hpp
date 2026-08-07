#ifndef MC_LAB_SEMANTIC_FACET_LABELLED_TRANSITION_RELATION_HPP
#define MC_LAB_SEMANTIC_FACET_LABELLED_TRANSITION_RELATION_HPP

#include <mc_lab/semantic/facet/transition_labelling.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>

/**
 * @file
 * Composes local transition access with structural transition labelling.
 *
 * This remains a facet: it requires neither initial states nor a recognized
 * system formalism. Algorithms may use it directly from a supplied state.
 */
namespace mc_lab::semantic {

/**
 * A transition relation whose iteration results expose labels through
 * `transition_label`.
 *
 * `TransitionLabelling` is intentionally instantiated with
 * `transition_reference_for_t`, not the stable transition value type. This
 * validates the references, values, or proxies that generic iteration really
 * observes and permits move-only transition witnesses without introducing
 * copies.
 */
template <class System, class State>
concept LabelledTransitionRelation = TransitionRelation<System, State>
    && TransitionLabelling<System, transition_reference_for_t<System, State>>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_FACET_LABELLED_TRANSITION_RELATION_HPP
