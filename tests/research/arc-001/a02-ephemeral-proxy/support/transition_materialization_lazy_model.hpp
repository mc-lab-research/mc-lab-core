#ifndef MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_LAZY_MODEL_HPP
#define MC_LAB_TESTS_SEMANTIC_SUPPORT_TRANSITION_MATERIALIZATION_LAZY_MODEL_HPP

#include "../../common/support/transition_materialization_models.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>

namespace mc_lab::tests::semantic::research {

// Current-witness storage owned by the range for exactly one occurrence at
// a time. This is NOT a transition collection: the range keeps only one of
// these alive at once and overwrites it in place every time the traversal
// position changes (see lazy_outgoing_transitions::advance()). Reusing
// research::reference_label (defined in transition_materialization_models.hpp
// for the stored SUBJECT-EXPLICIT-LTS-01 subject) keeps the label type
// identical across both ARC-001 subjects, which the semantic oracle depends
// on for a fair comparison.
struct lazy_current_witness {
  reference_label label;
  int target;
};

// Pure function that computes "the transition at position `step` outgoing
// from `source`" on demand, with no side effects and no memoization. It is
// the single source of truth for the reference labelled transition
// relation:
//
//   0 --advance--> 1
//   0 --bypass-->  2
//   1 --reset-->   0
//   2                 (no outgoing transitions)
//
// Returning std::nullopt means "step is past the end of source's local
// image" - i.e. this is also how the range knows it has reached its
// sentinel. Because this function recomputes everything from (source, step)
// every time it is called, nothing about the transition relation is ever
// stored as a collection anywhere in this header.
[[nodiscard]] inline auto compute_lazy_transition(int source, int step)
    -> std::optional<lazy_current_witness> {
  switch (source) {
  case 0:
    // Source 0 has two outgoing transitions, selected by step 0 / step 1.
    switch (step) {
    case 0:
      return lazy_current_witness{reference_label::advance, 1};
    case 1:
      return lazy_current_witness{reference_label::bypass, 2};
    default:
      return std::nullopt;
    }
  case 1:
    // Source 1 has exactly one outgoing transition, at step 0.
    switch (step) {
    case 0:
      return lazy_current_witness{reference_label::reset, 0};
    default:
      return std::nullopt;
    }
  default:
    // Source 2 (and anything else) has no outgoing transitions at all.
    return std::nullopt;
  }
}

// SUBJECT-LAZY-LTS-01 (A02 revision): a genuinely single-pass, generated
// outgoing-transition range whose dereference result is an ephemeral proxy
// rather than a copied value.
//
// This is the corrected design after A01 was found nonconforming: A01's
// range dereferenced to a plain-value witness that copied `label` and
// `target` out of the range on every `operator*()` call, which is exactly
// what an "ephemeral proxy with no copied semantic fields" is supposed to
// avoid. Here, the range itself is the only thing that ever holds label/
// target data; the proxy holds nothing but a pointer back to the range plus
// a generation counter, so it can prove - and enforce - that it has gone
// stale once the range moves on.
//
// What the range is allowed to own (and does own): the queried source
// state, the traversal cursor (`step_`), storage for the *current*
// occurrence only (`current_`), and a generation token (`generation_`) that
// changes every time the traversal position changes. What it must never own
// (and does not): the complete local image (all outgoing transitions at
// once), or any stable, independently-addressable occurrence descriptor.
class lazy_outgoing_transitions {
public:
  // Constructing the range immediately computes the witness for step 0 (if
  // any) into `current_`, so that `begin() == end()` can be answered
  // correctly even before the first `operator*()` call, and so a range over
  // a source with zero outgoing transitions (source 2) starts out already
  // exhausted.
  [[nodiscard]] explicit lazy_outgoing_transitions(int source) noexcept
      : source_{source} {
    refresh_current();
  }

  // Empty sentinel type. Equality against it is implemented as a hidden
  // friend on `iterator` (see below) that asks "is there a current witness
  // left to visit?" rather than comparing two iterator positions - this
  // range has no notion of "the end position" independent of "the range
  // has been exhausted."
  struct sentinel {};

  // Forward-declared here, ahead of `proxy`, purely so that `proxy`'s
  // `friend class iterator;` declaration (below) resolves to *this* nested
  // type instead of implicitly declaring an unrelated `iterator` name in
  // the enclosing namespace. Without this forward declaration the friend
  // grant silently fails to reach the real iterator type, which is exactly
  // the bug fixed in commit ff4963f (a private-constructor access error
  // caught by the compiler, not a logic bug - but a real one).
  class iterator;

  // Ephemeral proxy: everything the frozen consumer (CON-SUCCESSORS-01)
  // actually touches when it names a witness `witness` and calls
  // `transition_label(system, witness)` / `target(system, witness)`.
  //
  // Deliberately minimal storage - exactly a non-owning pointer back to the
  // range plus the generation counter observed at the moment this proxy was
  // created. There is no `label` member and no `target` member here: those
  // live only in the range's `current_`, and this proxy is nothing more
  // than "a claim, as of generation N, that the range's current occurrence
  // is the one I'm pointing at." Once the range's own generation moves past
  // N, that claim is void, and `is_current()` will say so.
  class proxy {
  public:
    // The one piece of the proxy's contract that is safe to call at any
    // time, including after invalidation: "is this proxy still backed by
    // the range's current occurrence?" This is deliberately public (and
    // `noexcept`) because the A02 configuration requires a safe way to
    // observe staleness without ever touching the underlying semantic data
    // - this function never dereferences `range_->current_`, only compares
    // generation counters and a null check.
    [[nodiscard]] auto is_current() const noexcept -> bool {
      return range_ != nullptr && range_->generation_ == generation_;
    }

  private:
    // Only the iterator that creates proxies, and the model that reads
    // them through the public CPO surface, are allowed to see the
    // constructor or the semantic-reading `read()` method below. Nothing
    // else - including this proxy's own consumer-facing public API - can
    // construct one out of thin air or peek at stale data.
    friend class iterator;
    friend struct lazy_reference_system;

    [[nodiscard]] explicit proxy(const lazy_outgoing_transitions* range,
                                 std::uint64_t generation) noexcept
        : range_{range},
          generation_{generation} {}

    // The only way to reach the actual label/target data through a proxy.
    // Guarded by an assertion rather than silently returning whatever the
    // range happens to hold now: if this were ever reached on a stale
    // proxy, returning the range's *new* current witness instead of
    // failing loudly would be far more dangerous than a crash, because it
    // would look like a correct read of the wrong transition. Both the
    // frozen consumer and this file's own negative test are structured so
    // that this assertion is never triggered in a passing run - it exists
    // as a defense of the invalidation rule, not a code path that is
    // expected to fire.
    [[nodiscard]] auto read() const -> const lazy_current_witness& {
      assert(is_current()
             && "SUBJECT-LAZY-LTS-01 proxy read after invalidation");
      return *range_->current_;
    }

    const lazy_outgoing_transitions* range_ = nullptr;
    std::uint64_t generation_ = 0;
  };

  // Single-pass input iterator. It does not carry any traversal state of
  // its own beyond a pointer back to the shared range - "advancing" one
  // copy of the iterator advances the range's cursor, which every other
  // copy (and every proxy already handed out) observes through that same
  // shared pointer. This is what makes the negative test's "copy an
  // iterator, advance one copy, watch the other observation go stale"
  // scenario work: there is exactly one cursor, not one per iterator copy.
  class iterator {
  public:
    // `iterator_concept = std::input_iterator_tag` is declared explicitly
    // (rather than left to be deduced) so that this type is unambiguously
    // input-only. Because C++20 range-concept detection for
    // `std::ranges::forward_range` looks for a `forward_iterator_tag` (or
    // stronger) here, declaring `input_iterator_tag` is what makes
    // `!std::ranges::forward_range<lazy_range_t>` hold - it is a positive
    // declaration of "single-pass", not just the absence of extra
    // machinery.
    using iterator_concept = std::input_iterator_tag;
    using value_type = proxy;
    using difference_type = std::ptrdiff_t;

    // Default-constructible because `std::weakly_incrementable` (a
    // prerequisite of `std::input_iterator`) requires it, even though a
    // default-constructed iterator here (with `range_ == nullptr`) is not
    // meant to be dereferenced or incremented - it exists only to satisfy
    // the concept's syntactic requirement.
    iterator() noexcept = default;

    [[nodiscard]] explicit iterator(lazy_outgoing_transitions* range) noexcept
        : range_{range} {}

    // Builds a brand-new proxy stamped with the range's *current*
    // generation. Every dereference - even of the same iterator position
    // twice in a row without an intervening increment - produces a
    // distinct proxy object, but as long as the range hasn't advanced,
    // all such proxies share the same generation and are all still
    // "current."
    [[nodiscard]] auto operator*() const -> proxy {
      return proxy{range_, range_->generation_};
    }

    // Delegates to the range: this is where the shared cursor actually
    // moves, and where every proxy created before this call becomes
    // provably stale (see lazy_outgoing_transitions::advance()).
    auto operator++() -> iterator& {
      range_->advance();
      return *this;
    }

    // Post-increment is only required to be a valid, discarded-value
    // expression by weakly_incrementable - it is not required to return an
    // iterator copy of the old position, and does not: this type has no
    // "old position" independent of the shared range state to copy.
    void operator++(int) { ++*this; }

    // Sentinel comparison, implemented as a hidden friend (found only via
    // argument-dependent lookup on `iterator`/`sentinel`, keeping it out of
    // ordinary overload resolution elsewhere). "At the sentinel" means
    // "the range has no current witness left" - i.e. the last call to
    // refresh_current() came back empty - not any positional comparison.
    [[nodiscard]] friend auto operator==(const iterator& it, sentinel) -> bool {
      return !it.range_->current_.has_value();
    }

  private:
    lazy_outgoing_transitions* range_ = nullptr;
  };

  // Returns a fresh iterator handle onto this range. Because `iterator`
  // only stores a pointer back to `*this`, every call to begin() (there is
  // only ever one, from the range-based for loop's implicit expansion, but
  // nothing prevents more) yields a handle that observes the same shared
  // cursor - there is no independent "starting position" concept here.
  [[nodiscard]] auto begin() -> iterator { return iterator{this}; }

  [[nodiscard]] auto end() -> sentinel { return {}; }

private:
  // Only `proxy::read()`/`proxy::is_current()` and `iterator`'s methods
  // need to reach into this range's private state (`current_`,
  // `generation_`, `source_`, `step_`); nothing else, including the model
  // struct below, touches these fields directly - the model only ever goes
  // through a proxy's `read()`.
  friend class proxy;
  friend class iterator;

  // Recomputes `current_` for the range's present (source_, step_) pair.
  // This is the only place that ever writes to `current_` with fresh data;
  // it does not touch `generation_`, because "the current witness changed
  // because we just constructed the range or just advanced" is tracked
  // separately by advance() below.
  void refresh_current() { current_ = compute_lazy_transition(source_, step_); }

  // The single place where the traversal position actually changes, and
  // therefore the single place where "staleness" is created. The order of
  // operations here matters for what the invalidation rule actually means:
  //
  //   1. step_ moves to the next position;
  //   2. current_ is cleared *before* generation_ changes, so there is no
  //      window where a stale generation could still see the old witness;
  //   3. generation_ is incremented, which is what makes every proxy
  //      created before this call now disagree with range_->generation_
  //      and therefore report is_current() == false;
  //   4. only then is current_ repopulated for the *new* position, so that
  //      a fresh dereference immediately after this call (at the new,
  //      already-incremented generation) sees the new witness right away.
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

// SUBJECT-LAZY-LTS-01 model (A02 revision): semantically equivalent to
// stored_reference_system (SUBJECT-EXPLICIT-LTS-01) for the reference
// transition relation, but exposed entirely through the ephemeral proxy
// above instead of a stable, stored `const stored_transition&`.
//
// `target` and `transition_label` both take `const proxy&` - a signature
// that accepts the proxy both as the named lvalue the unchanged consumer
// actually passes (`witness` in `for (auto&& witness : ...)`) and as the
// rvalue-forwarded form the TransitionRelation/TransitionLabelling concept
// checks probe internally. Either way, all this model ever reads is
// `witness.read()`, i.e. whatever the range currently holds - never
// anything the proxy itself is carrying, because it carries nothing
// semantic at all.
struct lazy_reference_system {
  // Every call starts a fresh range at generation 0, step 0, with
  // `current_` already populated (or already empty, for source 2) by the
  // range's constructor.
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
