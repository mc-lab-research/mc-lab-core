#ifndef MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_LAZY_MODEL_HPP
#define MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_LAZY_MODEL_HPP

#include "../../common/support/transition_materialization_models.hpp"

#include <cstddef>
#include <iterator>
#include <optional>

namespace mc_lab::tests::semantic::research {

// SUBJECT-LAZY-LTS-01 (A01 revision) ephemeral witness.
//
// This is the by-value form: every dereference constructs a brand-new
// `lazy_transition_witness` from scratch and returns it directly, so
// `label` and `target` are always copies, never a view into anything the
// range itself is still holding. There is no address here that "goes
// stale" the way a handle-based proxy would - each witness is simply its
// own independent value, valid for as long as the caller keeps it around.
//
// (The A02 revision of this experiment replaces this by-value witness with
// a genuinely ephemeral proxy - a non-owning handle plus a generation
// token, with no copied label/target fields at all - specifically because
// this by-value design was found not to satisfy the "ephemeral proxy with
// no copied semantic data" requirement. That revision lives on the
// `experiment/arc-001/a02-ephemeral-proxy` branch. This file is preserved
// here, unmodified in behavior, as the reviewable A01 attempt.)
struct lazy_transition_witness {
  reference_label label;
  int target;
};

// Pure function that computes "the transition at position `step` outgoing
// from `source`" on demand. This is the single place the reference
// labelled transition relation is encoded:
//
//   0 --advance--> 1
//   0 --bypass-->  2
//   1 --reset-->   0
//   2                 (no outgoing transitions)
//
// Returning std::nullopt means "step is past source's local image" - which
// is also how the range below knows it has reached its sentinel. Because
// every call recomputes its result from (source, step) with no memoization
// and no side effects, nothing about the transition relation is ever
// stored as a collection anywhere in this header - each witness is
// generated fresh, one at a time, in traversal order.
[[nodiscard]] inline auto compute_lazy_transition(int source, int step)
    -> std::optional<lazy_transition_witness> {
  switch (source) {
  case 0:
    // Source 0 has two outgoing transitions, selected by step 0 / step 1.
    switch (step) {
    case 0:
      return lazy_transition_witness{reference_label::advance, 1};
    case 1:
      return lazy_transition_witness{reference_label::bypass, 2};
    default:
      return std::nullopt;
    }
  case 1:
    // Source 1 has exactly one outgoing transition, at step 0.
    switch (step) {
    case 0:
      return lazy_transition_witness{reference_label::reset, 0};
    default:
      return std::nullopt;
    }
  default:
    // Source 2 (and anything else) has no outgoing transitions at all.
    return std::nullopt;
  }
}

// SUBJECT-LAZY-LTS-01 (A01 revision): a genuinely single-pass, generated
// outgoing-transition range.
//
// The range's only traversal state is the queried source and a cursor
// (`step_`); it never stores the complete local image of outgoing
// transitions, only enough to know "which transition comes next." Its
// iterator holds nothing but a pointer back to the range, so copies of the
// iterator all observe and advance the *same* cursor - incrementing one
// copy changes what every other copy (and the range itself) reports as the
// current position. This shared-cursor aliasing is what makes the range
// genuinely single-pass, and is demonstrated directly by
// `copied_iterators_share_traversal_state()` in the accompanying test file,
// independently of whether the frozen consumer happens to exercise it.
class lazy_outgoing_transitions {
public:
  [[nodiscard]] explicit lazy_outgoing_transitions(int source) noexcept
      : source_{source} {}

  // Empty sentinel type. There is no independent "end position" for this
  // range beyond "compute_lazy_transition(source_, step_) came back empty" -
  // see the hidden-friend `operator==` on `iterator` below.
  struct sentinel {};

  class iterator {
  public:
    // Declaring `iterator_concept = std::input_iterator_tag` explicitly is
    // what makes `std::ranges::forward_range<lazy_outgoing_transitions>`
    // false: C++20 range-concept detection looks for a
    // `forward_iterator_tag` (or stronger) here, and `input_iterator_tag`
    // is not derived from it. This is a positive declaration of
    // "single-pass only," not merely the absence of extra iterator
    // machinery.
    using iterator_concept = std::input_iterator_tag;
    using value_type = lazy_transition_witness;
    using difference_type = std::ptrdiff_t;

    // Default-constructible because `std::weakly_incrementable` (required
    // by `std::input_iterator`) requires it, even though a
    // default-constructed iterator (`range_ == nullptr`) is not meant to be
    // dereferenced.
    iterator() noexcept = default;

    [[nodiscard]] explicit iterator(lazy_outgoing_transitions* range) noexcept
        : range_{range} {}

    // Computes and returns a brand-new witness for the range's *current*
    // (source_, step_) pair every time this is called - there is nothing
    // cached here to return a reference to.
    [[nodiscard]] auto operator*() const -> lazy_transition_witness {
      return *compute_lazy_transition(range_->source_, range_->step_);
    }

    // Advances the range's shared cursor directly - this is the only place
    // the traversal position changes, and it is visible through every
    // iterator copy that points at the same range.
    auto operator++() -> iterator& {
      ++range_->step_;
      return *this;
    }

    // Post-increment only needs to be a valid, discarded-value expression
    // per `weakly_incrementable` - it does not need to return a copy of the
    // prior position, and does not.
    void operator++(int) { ++*this; }

    // Sentinel comparison, implemented as a hidden friend. "At the
    // sentinel" means "there is no transition at the current (source,
    // step)," recomputed on demand rather than tracked as a separate flag.
    //
    // Delegates to the private `at_end()` member below rather than reaching
    // into `range_->source_` / `range_->step_` directly here: a hidden
    // friend is not itself a member of `iterator`, so it does not inherit
    // the nested-class-to-enclosing-class private-access grant that an
    // actual `iterator` member function gets. GCC and Clang accept the
    // direct access as an extension; MSVC does not. Reading the private
    // state through a genuine member function keeps this portable.
    [[nodiscard]] friend auto operator==(const iterator& it, sentinel) -> bool {
      return it.at_end();
    }

  private:
    // Member of `iterator`, and therefore entitled to the private-access
    // grant nested classes receive on their enclosing class - this is
    // where `source_` / `step_` are actually read.
    [[nodiscard]] auto at_end() const -> bool {
      return !compute_lazy_transition(range_->source_, range_->step_)
                  .has_value();
    }

    lazy_outgoing_transitions* range_ = nullptr;
  };

  // Returns a fresh handle onto this range's shared cursor. Because
  // `iterator` stores only a pointer back to `*this`, there is no
  // independent "starting position" snapshot taken here - `begin()` and any
  // later `begin()` call (nothing prevents more than one) all observe
  // whatever the range's cursor currently is.
  [[nodiscard]] auto begin() -> iterator { return iterator{this}; }

  [[nodiscard]] auto end() -> sentinel { return {}; }

private:
  int source_;
  int step_ = 0;
};

// SUBJECT-LAZY-LTS-01 model (A01 revision): semantically equivalent to
// stored_reference_system (SUBJECT-EXPLICIT-LTS-01) for the reference
// labelled transition relation, but exposed entirely through the generated,
// by-value witnesses above instead of a stored `const stored_transition&`.
//
// `target` and `transition_label` both take `const lazy_transition_witness&`,
// a signature that accepts the witness both as the named lvalue the
// unchanged consumer actually passes (`witness` in
// `for (auto&& witness : ...)`) and as the rvalue-forwarded form the
// TransitionRelation/TransitionLabelling concept checks probe internally.
struct lazy_reference_system {
  // Every call starts a fresh range at step 0 for the given source; no
  // state is retained between calls to `outgoing_transitions`.
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
