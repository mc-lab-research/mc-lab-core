#ifndef MC_LAB_TESTS_SEMANTIC_SUPPORT_LABELLED_SUCCESSOR_CONSUMER_HPP
#define MC_LAB_TESTS_SEMANTIC_SUPPORT_LABELLED_SUCCESSOR_CONSUMER_HPP

#include <mc_lab/semantic/facet/transition_labelling.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>
#include <type_traits>
#include <utility>
#include <vector>

namespace mc_lab::tests::semantic::research {

/**
 * Consumer-side semantic observation used by the frozen oracle.
 *
 * This owning copy is deliberately different from model-side transition
 * materialization. The consumer forgets witness representation and lifetime
 * only after it has observed the current label and target. It does not require
 * the subject to store transition objects or keep a witness alive afterward.
 */
template <class Label, class State> struct labelled_successor {
  Label label;
  State target;

  [[nodiscard]] constexpr auto operator==(
      const labelled_successor&) const noexcept -> bool = default;
};

/**
 * CON-SUCCESSORS-01: observe every local labelled transition exactly once.
 *
 * The witness is used only inside its range iteration step. The consumer does
 * not retain it, inspect its address, count the range in advance, or make a
 * second pass. Only the semantic label and target are copied into the result.
 *
 * This source was frozen at
 * f969fa8aa63a9a172e6feb2eeb32636aab24f387. The maintained copy is documented
 * here after adjudication; the immutable commit remains the authority for the
 * exact controlled observation.
 */
template <class System, class State>
requires mc_lab::semantic::TransitionRelation<System, State>
         && mc_lab::semantic::TransitionLabelling<
             System,
             mc_lab::semantic::transition_reference_for_t<System, State>>
[[nodiscard]] auto observe_labelled_successors(const System& system,
                                               const State& source) {
  namespace sem = mc_lab::semantic;

  using witness_t = sem::transition_reference_for_t<System, State>;
  using label_t = sem::transition_label_for_t<System, witness_t>;
  using state_t = std::remove_cvref_t<sem::target_result_t<System, State>>;

  // Vector ownership belongs to the observer and oracle boundary. It cannot
  // grant stable identity or multipass behavior to the model's witnesses.
  std::vector<labelled_successor<label_t, state_t>> observations;
  for (auto&& witness : sem::outgoing_transitions(system, source)) {
    // Both semantic facts are read before range-for advances. This ordering is
    // what permits A02's current-generation proxy to become stale immediately
    // after the iteration step without changing the consumer.
    observations.push_back(
        {sem::transition_label(system, witness), sem::target(system, witness)});
  }
  return observations;
}

}  // namespace mc_lab::tests::semantic::research

#endif  // MC_LAB_TESTS_SEMANTIC_SUPPORT_LABELLED_SUCCESSOR_CONSUMER_HPP
