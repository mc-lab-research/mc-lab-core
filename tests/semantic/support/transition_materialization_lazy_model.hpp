#ifndef MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_LAZY_MODEL_HPP
#define MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_LAZY_MODEL_HPP

#include "transition_materialization_models.hpp"

#include <cstddef>
#include <iterator>
#include <optional>

namespace mc_lab::tests::semantic::research {

/**
 * SUBJECT-LAZY-LTS-01 ephemeral witness.
 *
 * Produced fresh, by value, on every dereference. It is not stored anywhere
 * else and has no address that survives the dereference that created it.
 */
struct lazy_transition_witness {
  reference_label label;
  int target;
};

/**
 * Computes the transition at `step` outgoing from `source`, or nothing if
 * `step` is past the local image. This is the single point that encodes the
 * reference labelled transition relation; it is evaluated on demand and
 * never materializes a collection of transitions.
 */
[[nodiscard]] inline auto compute_lazy_transition(int source, int step)
    -> std::optional<lazy_transition_witness> {
  switch (source) {
  case 0:
    switch (step) {
    case 0:
      return lazy_transition_witness{reference_label::advance, 1};
    case 1:
      return lazy_transition_witness{reference_label::bypass, 2};
    default:
      return std::nullopt;
    }
  case 1:
    switch (step) {
    case 0:
      return lazy_transition_witness{reference_label::reset, 0};
    default:
      return std::nullopt;
    }
  default:
    return std::nullopt;
  }
}

/**
 * SUBJECT-LAZY-LTS-01: genuinely single-pass generated outgoing-transition
 * range.
 *
 * The range owns the only traversal state (the queried source and a cursor).
 * Its iterator is a thin handle pointing back at the range, so copies of the
 * iterator alias the same cursor: advancing one copy advances what every
 * other copy observes. This is deliberate single-pass aliasing, not an
 * oversight - it is what makes the range non-forward without relying on
 * incidental omissions.
 */
class lazy_outgoing_transitions {
public:
  [[nodiscard]] explicit lazy_outgoing_transitions(int source) noexcept
      : source_{source} {}

  struct sentinel {};

  class iterator {
  public:
    using iterator_concept = std::input_iterator_tag;
    using value_type = lazy_transition_witness;
    using difference_type = std::ptrdiff_t;

    iterator() noexcept = default;

    [[nodiscard]] explicit iterator(lazy_outgoing_transitions* range) noexcept
        : range_{range} {}

    [[nodiscard]] auto operator*() const -> lazy_transition_witness {
      return *compute_lazy_transition(range_->source_, range_->step_);
    }

    auto operator++() -> iterator& {
      ++range_->step_;
      return *this;
    }

    void operator++(int) { ++*this; }

    [[nodiscard]] friend auto operator==(const iterator& it, sentinel) -> bool {
      return !compute_lazy_transition(it.range_->source_, it.range_->step_)
                  .has_value();
    }

  private:
    lazy_outgoing_transitions* range_ = nullptr;
  };

  [[nodiscard]] auto begin() -> iterator { return iterator{this}; }

  [[nodiscard]] auto end() -> sentinel { return {}; }

private:
  int source_;
  int step_ = 0;
};

/**
 * SUBJECT-LAZY-LTS-01 model: semantically equivalent to
 * stored_reference_system, exposed entirely through generated ephemeral
 * witnesses. It stores no transition, no local image, and no complete
 * transition collection - only the model member functions needed to compute
 * a witness's label and target for whatever transition form the range
 * actually produces.
 */
struct lazy_reference_system {
  [[nodiscard]] auto outgoing_transitions(const int& source) const
      -> lazy_outgoing_transitions {
    return lazy_outgoing_transitions{source};
  }

  [[nodiscard]] auto target(
      const lazy_transition_witness& witness) const noexcept -> int {
    return witness.target;
  }

  [[nodiscard]] auto transition_label(const lazy_transition_witness& witness)
      const noexcept -> reference_label {
    return witness.label;
  }
};

}  // namespace mc_lab::tests::semantic::research

#endif  // MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_LAZY_MODEL_HPP
