#ifndef MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_MODELS_HPP
#define MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_MODELS_HPP

#include <array>
#include <span>

namespace mc_lab::tests::semantic::research {

/**
 * Semantic labels in the frozen three-state ARC-001 reference relation.
 * Keeping one copyable scalar label type constant prevents representation
 * mechanics from being confused with a difference in semantic value types.
 */
enum class reference_label : unsigned char { advance, bypass, reset };

/**
 * Stable occurrence object used only by SUBJECT-EXPLICIT-LTS-01.
 *
 * Its address and fields remain valid because it resides in a member array.
 * A02 intentionally does not provide this lifetime or representation.
 */
struct stored_transition {
  reference_label label;
  int target;
};

/**
 * SUBJECT-EXPLICIT-LTS-01: stored calibration representation of the reference
 * labelled transition system.
 *
 * The three arrays are an intentional positive control: they make the local
 * transition image explicit and yield stable `const stored_transition&`
 * witnesses through `std::span`. They are not a recommended universal model.
 * The controlled question is whether the same consumer also works when these
 * representation properties are removed.
 *
 * This apparatus was frozen at
 * f969fa8aa63a9a172e6feb2eeb32636aab24f387. Comments in this maintained copy
 * do not alter the immutable historical evidence.
 */
struct stored_reference_system {
  std::array<stored_transition, 2> from_zero{{
      {reference_label::advance, 1},
      {reference_label::bypass, 2},
  }};
  std::array<stored_transition, 1> from_one{{
      {reference_label::reset, 0},
  }};
  std::array<stored_transition, 0> from_two{};

  [[nodiscard]] constexpr auto outgoing_transitions(
      const int& source) const noexcept -> std::span<const stored_transition> {
    // Each span is a non-owning view over a complete member-backed local image.
    // The empty default case keeps queries outside the frozen {0,1,2} domain
    // harmless but does not extend ARC-001's demonstrated semantic domain.
    switch (source) {
    case 0:
      return from_zero;
    case 1:
      return from_one;
    case 2:
      return from_two;
    default:
      return {};
    }
  }

  [[nodiscard]] constexpr auto target(
      const stored_transition& transition) const noexcept -> const int& {
    // Returning a reference is safe only because this subject owns stable
    // transition objects. The consumer is frozen not to rely on that fact.
    return transition.target;
  }

  [[nodiscard]] constexpr auto transition_label(
      const stored_transition& transition) const noexcept -> reference_label {
    // Labels are copied into the observer result so the oracle compares
    // semantic facts rather than transition-object identity.
    return transition.label;
  }
};

}  // namespace mc_lab::tests::semantic::research

#endif  // MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_MODELS_HPP
