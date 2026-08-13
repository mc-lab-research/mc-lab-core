#ifndef MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_MODELS_HPP
#define MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_MODELS_HPP

#include <array>
#include <span>

namespace mc_lab::tests::semantic::research {

/** Labels in the finite ARC-001 reference transition system. */
enum class reference_label : unsigned char { advance, bypass, reset };

/** Stable transition witness stored by the calibration subject. */
struct stored_transition {
  reference_label label;
  int target;
};

/**
 * SUBJECT-EXPLICIT-LTS-01: stored calibration representation of the reference
 * labelled transition system.
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
    return transition.target;
  }

  [[nodiscard]] constexpr auto transition_label(
      const stored_transition& transition) const noexcept -> reference_label {
    return transition.label;
  }
};

}  // namespace mc_lab::tests::semantic::research

#endif  // MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_MODELS_HPP
