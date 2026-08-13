#ifndef MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_LAZY_MODEL_HPP
#define MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_LAZY_MODEL_HPP

#include "transition_materialization_models.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>

namespace mc_lab::tests::semantic::research {

/**
 * Current-witness storage owned by the range for exactly one occurrence at a
 * time. This is not a transition collection - it is overwritten in place
 * every time the traversal position changes.
 */
struct lazy_current_witness {
  reference_label label;
  int target;
};

/**
 * Computes the transition at `step` outgoing from `source`, or nothing if
 * `step` is past the local image. Evaluated on demand; never materializes a
 * collection of transitions.
 */
[[nodiscard]] inline auto compute_lazy_transition(int source, int step)
    -> std::optional<lazy_current_witness> {
  switch (source) {
  case 0:
    switch (step) {
    case 0:
      return lazy_current_witness{reference_label::advance, 1};
    case 1:
      return lazy_current_witness{reference_label::bypass, 2};
    default:
      return std::nullopt;
    }
  case 1:
    switch (step) {
    case 0:
      return lazy_current_witness{reference_label::reset, 0};
    default:
      return std::nullopt;
    }
  default:
    return std::nullopt;
  }
}

/**
 * SUBJECT-LAZY-LTS-01 (A02): genuinely single-pass generated
 * outgoing-transition range whose dereference result is an ephemeral proxy.
 *
 * The range owns the queried source, the traversal cursor, storage for the
 * current occurrence only, and a generation token. It never owns or caches
 * the complete local image or stable occurrence descriptors.
 */
class lazy_outgoing_transitions {
public:
  [[nodiscard]] explicit lazy_outgoing_transitions(int source) noexcept
      : source_{source} {
    refresh_current();
  }

  struct sentinel {};

  /**
   * Ephemeral proxy: a non-owning handle to the range's current-witness
   * state, plus the generation observed at dereference. It carries no
   * copied label or target - reading through it after the range has moved
   * on is a stale access, which `is_current()` can detect and which the
   * accessors below refuse via assertion.
   */
  class proxy {
  public:
    [[nodiscard]] auto is_current() const noexcept -> bool {
      return range_ != nullptr && range_->generation_ == generation_;
    }

  private:
    friend class iterator;
    friend struct lazy_reference_system;

    [[nodiscard]] explicit proxy(const lazy_outgoing_transitions* range,
                                 std::uint64_t generation) noexcept
        : range_{range},
          generation_{generation} {}

    [[nodiscard]] auto read() const -> const lazy_current_witness& {
      assert(is_current()
             && "SUBJECT-LAZY-LTS-01 proxy read after invalidation");
      return *range_->current_;
    }

    const lazy_outgoing_transitions* range_ = nullptr;
    std::uint64_t generation_ = 0;
  };

  class iterator {
  public:
    using iterator_concept = std::input_iterator_tag;
    using value_type = proxy;
    using difference_type = std::ptrdiff_t;

    iterator() noexcept = default;

    [[nodiscard]] explicit iterator(lazy_outgoing_transitions* range) noexcept
        : range_{range} {}

    [[nodiscard]] auto operator*() const -> proxy {
      return proxy{range_, range_->generation_};
    }

    auto operator++() -> iterator& {
      range_->advance();
      return *this;
    }

    void operator++(int) { ++*this; }

    [[nodiscard]] friend auto operator==(const iterator& it, sentinel) -> bool {
      return !it.range_->current_.has_value();
    }

  private:
    lazy_outgoing_transitions* range_ = nullptr;
  };

  [[nodiscard]] auto begin() -> iterator { return iterator{this}; }

  [[nodiscard]] auto end() -> sentinel { return {}; }

private:
  friend class proxy;
  friend class iterator;

  void refresh_current() { current_ = compute_lazy_transition(source_, step_); }

  void advance() {
    ++step_;
    current_.reset();
    ++generation_;
    refresh_current();
  }

  int source_;
  int step_ = 0;
  std::uint64_t generation_ = 0;
  std::optional<lazy_current_witness> current_;
};

/**
 * SUBJECT-LAZY-LTS-01 model (A02): semantically equivalent to
 * stored_reference_system, exposed entirely through the ephemeral proxy
 * above. `target` and `transition_label` read only current range-owned
 * state through the proxy's handle; they never hold a copy of the label or
 * target themselves.
 */
struct lazy_reference_system {
  [[nodiscard]] auto outgoing_transitions(const int& source) const
      -> lazy_outgoing_transitions {
    return lazy_outgoing_transitions{source};
  }

  [[nodiscard]] auto target(
      const lazy_outgoing_transitions::proxy& witness) const -> int {
    return witness.read().target;
  }

  [[nodiscard]] auto transition_label(
      const lazy_outgoing_transitions::proxy& witness) const
      -> reference_label {
    return witness.read().label;
  }
};

}  // namespace mc_lab::tests::semantic::research

#endif  // MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_LAZY_MODEL_HPP
