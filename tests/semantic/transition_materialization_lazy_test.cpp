// ARC-001 A01 controlled test: runs the byte-for-byte unchanged
// CON-SUCCESSORS-01 consumer and the frozen semantic oracle against
// SUBJECT-LAZY-LTS-01's by-value generated revision. Nothing in this file,
// or in transition_materialization_lazy_model.hpp, changes the consumer,
// the oracle, the stored subject, the public concepts, or the CPO
// implementations.
//
// This is the A01 (by-value witness) attempt. Its evidence was accepted
// alongside the A02 (ephemeral-proxy) revision on
// `experiment/arc-001/a02-ephemeral-proxy`, which corrected the witness
// design to carry a non-owning handle instead of copied label/target
// fields. Both revisions are preserved for comparison.
#include "support/labelled_successor_consumer.hpp"
#include "support/transition_materialization_lazy_model.hpp"
#include "support/transition_materialization_models.hpp"

#include <algorithm>
#include <array>
#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <mc_lab/semantic/facet/transition_labelling.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>
#include <ranges>
#include <type_traits>

namespace sem = mc_lab::semantic;
namespace research = mc_lab::tests::semantic::research;

using system_t = research::lazy_reference_system;
using lazy_range_t = sem::outgoing_transition_range_t<system_t, int>;
using witness_t = sem::transition_reference_for_t<system_t, int>;
using observation_t =
    research::labelled_successor<research::reference_label, int>;

// --- SUBJECT-LAZY-LTS-01 (A01) frozen acceptance checks --------------------
//
// The outgoing range must be an input_range, and must explicitly NOT be a
// forward_range, sized_range, or borrowed_range.
static_assert(std::ranges::input_range<lazy_range_t>);
static_assert(!std::ranges::forward_range<lazy_range_t>);
static_assert(!std::ranges::sized_range<lazy_range_t>);
static_assert(!std::ranges::borrowed_range<lazy_range_t>);

// The range's actual reference form is the value-typed ephemeral witness,
// not a stored reference - there is nothing stable to borrow. (This is the
// specific point on which A02's proxy-based witness differs: there,
// witness_t is a handle-plus-generation proxy rather than a plain value
// carrying copied label/target fields.)
static_assert(std::same_as<witness_t, research::lazy_transition_witness>);

// The relevant SemTL concepts hold for the lazy subject's actual witness
// form - the concept checks are exercised against the same type the
// consumer will actually see.
static_assert(sem::TransitionRelation<system_t, int>);
static_assert(sem::TransitionLabelling<system_t, witness_t>);

// CON-SUCCESSORS-01 remains source-unchanged; this static_assert only
// checks that it still compiles and produces the same owning observation
// type for the lazy subject as it does for the stored subject. Unlike the
// declaration-level checks above, this line forces the compiler to
// actually instantiate the consumer's template body - including its calls
// to `transition_label`/`target` on the named lvalue `witness` inside the
// range-based for loop - against this witness type.
static_assert(std::same_as<decltype(research::observe_labelled_successors(
                               std::declval<const system_t&>(),
                               std::declval<const int&>())),
                           std::vector<observation_t>>);

// Multiplicity-sensitive, order-independent comparison: sorts both sides by
// (label, target) and compares element-wise, so a missing or duplicated
// transition cannot slip through, while enumeration order (declared
// non-semantic by ARC-001) is not required to match.
template <std::size_t Size>
[[nodiscard]] auto agrees_with_relation(
    std::vector<observation_t> actual,
    std::array<observation_t, Size> expected) -> bool {
  const auto less = [](const observation_t& left, const observation_t& right) {
    if (left.label != right.label) {
      return left.label < right.label;
    }
    return left.target < right.target;
  };

  std::ranges::sort(actual, less);
  std::ranges::sort(expected, less);
  return std::ranges::equal(actual, expected);
}

/**
 * Demonstrates the frozen single-pass acceptance check directly: copied
 * iterators share traversal state, so advancing one invalidates what an
 * earlier copy observes. This is checked independently of the consumer.
 *
 * Because SUBJECT-LAZY-LTS-01 (A01) witnesses are plain values - not
 * handles into range-owned state - "invalidates" here means "the *next*
 * dereference through either handle now reports the new position," not
 * that the previously-obtained witness value itself changes or becomes
 * unsafe to read. `before_advance` remains a perfectly valid, independent
 * value even after `++first`; what this test demonstrates is that the
 * *range's cursor* is shared, which is the single-pass property that
 * matters for the frozen acceptance checks. (Contrast with the A02
 * revision, where the proxy witness itself becomes provably stale after
 * advancement - a stronger invalidation guarantee than this A01 attempt
 * provides, which is part of why A01 needed a follow-up revision.)
 */
[[nodiscard]] auto copied_iterators_share_traversal_state() -> bool {
  const research::lazy_reference_system system{};
  auto range = system.outgoing_transitions(0);

  // `first` and `second` are independent iterator objects, but both point
  // at the same range, so they share one cursor.
  auto first = range.begin();
  auto second = first;  // copy - must alias the same underlying cursor.

  const auto before_advance = *second;
  ++first;  // advances the shared cursor through either handle.
  const auto after_advance = *second;

  // The witness observed through `second` before the advance must differ
  // from the witness observed through the same handle afterward - proving
  // the cursor moved even though `second` itself was never incremented.
  const bool observation_changed =
      before_advance.label != after_advance.label
      || before_advance.target != after_advance.target;
  // And what `second` now reports must agree with what `first` (the handle
  // that was actually incremented) reports, since both are reading the
  // same shared cursor.
  const bool matches_first = after_advance.label == (*first).label
                             && after_advance.target == (*first).target;

  return observation_changed && matches_first;
}

int main() try {
  const system_t lazy{};

  // Run the byte-for-byte unmodified CON-SUCCESSORS-01 against all three
  // reference source states: branching (0), singleton (1), and terminal
  // (2), exactly as the frozen oracle test does for the stored subject.
  const auto from_zero = research::observe_labelled_successors(lazy, 0);
  const auto from_one = research::observe_labelled_successors(lazy, 1);
  const auto from_two = research::observe_labelled_successors(lazy, 2);

  const bool branching_state_agrees = agrees_with_relation(
      from_zero, std::array{
                     observation_t{research::reference_label::advance, 1},
                     observation_t{research::reference_label::bypass, 2},
                 });
  const bool single_successor_state_agrees = agrees_with_relation(
      from_one, std::array{
                    observation_t{research::reference_label::reset, 0},
                });
  const bool terminal_state_agrees =
      agrees_with_relation(from_two, std::array<observation_t, 0>{});

  if (!branching_state_agrees || !single_successor_state_agrees
      || !terminal_state_agrees) {
    std::fputs("lazy ARC-001 subject disagrees with the labelled-LTS oracle\n",
               stderr);
    return EXIT_FAILURE;
  }

  if (!copied_iterators_share_traversal_state()) {
    std::fputs("lazy ARC-001 subject did not demonstrate single-pass iterator "
               "aliasing\n",
               stderr);
    return EXIT_FAILURE;
  }

  // Negative check: the frozen compile-time assertions above already prove
  // the consumer's dependency surface is satisfied by an input_range that is
  // not forward, sized, or borrowed. Reaching this point at runtime, with
  // CON-SUCCESSORS-01 unmodified and no second pass taken over `lazy`,
  // confirms the consumer required no stronger range category and no
  // witness validity beyond a single dereference-then-advance step.

  return EXIT_SUCCESS;
} catch (...) {
  std::fputs("lazy ARC-001 controlled experiment raised an unexpected "
             "exception\n",
             stderr);
  return EXIT_FAILURE;
}
