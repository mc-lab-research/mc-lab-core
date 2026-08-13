// ARC-001 A02 controlled test: runs the byte-for-byte unchanged
// CON-SUCCESSORS-01 consumer and the frozen semantic oracle against
// SUBJECT-LAZY-LTS-01's ephemeral-proxy revision. Nothing in this file, or
// in transition_materialization_lazy_model.hpp, changes the consumer, the
// oracle, the stored subject, the public concepts, or the CPO
// implementations - see ARC-001-A02-ephemeral-proxy-freeze.md for the exact
// change boundary this file operates within.
#include "support/labelled_successor_consumer.hpp"
#include "support/transition_materialization_lazy_model.hpp"
#include "support/transition_materialization_models.hpp"

#include <algorithm>
#include <array>
#include <cassert>
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

// --- SUBJECT-LAZY-LTS-01 (A02) frozen acceptance checks -------------------
//
// These four assertions are the compile-time half of the A02 configuration's
// "Required conformance checks": the outgoing range must be an input_range,
// and must explicitly NOT be a forward_range, sized_range, or
// borrowed_range. Failing any one of these would already be a discriminating
// result before a single test runs.
static_assert(std::ranges::input_range<lazy_range_t>);
static_assert(!std::ranges::forward_range<lazy_range_t>);
static_assert(!std::ranges::sized_range<lazy_range_t>);
static_assert(!std::ranges::borrowed_range<lazy_range_t>);

// The range's actual reference form (what range_reference_t resolves to,
// and therefore what `witness` is bound to inside the frozen consumer's
// range-based for loop) is the ephemeral proxy type itself, not a plain
// value witness and not a stored reference. This is the A02-specific check
// that distinguishes this revision from A01, whose witness_t was a
// by-value struct carrying copied label/target fields.
static_assert(
    std::same_as<witness_t, research::lazy_outgoing_transitions::proxy>);

// The relevant SemTL concepts hold for the lazy subject's actual proxy
// form - i.e. the concept checks are exercised against the same type the
// consumer will actually see, not some other convenient stand-in.
static_assert(sem::TransitionRelation<system_t, int>);
static_assert(sem::TransitionLabelling<system_t, witness_t>);

// CON-SUCCESSORS-01 remains source-unchanged; this static_assert only
// checks that it still compiles and produces the same owning observation
// type for the lazy subject as it does for the stored subject. If the
// frozen consumer could not be instantiated against the proxy at all (for
// example because of the named-lvalue/value-category discriminator
// described in ARC-001.md), this line itself would fail to compile - which
// is exactly the failure mode ARC-001 asks us to preserve rather than
// paper over.
static_assert(std::same_as<decltype(research::observe_labelled_successors(
                               std::declval<const system_t&>(),
                               std::declval<const int&>())),
                           std::vector<observation_t>>);

// Multiplicity-sensitive, order-independent comparison against the
// expected labelled transitions - sorts both sides by (label, target) and
// compares element-wise, so neither a missing nor a duplicated transition
// can slip through, but the enumeration order (which ARC-001 declares
// non-semantic) is not required to match.
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

// Frozen A02 negative/lifetime demonstration, following the pseudocode in
// ARC-001-A02-ephemeral-proxy-freeze.md line for line:
//
//   auto iterator = range.begin();
//   auto original_proxy = *iterator;
//   auto iterator_copy = iterator;
//
//   assert(original_proxy.is_current());
//   observe_label(original_proxy);
//   observe_target(original_proxy);
//
//   ++iterator_copy;
//
//   assert(!original_proxy.is_current());
//
// This function runs entirely against source 0 (the branching state), so
// that "the new shared position after advancing" is a second, distinct
// transition (bypass -> 2) rather than the sentinel - which lets the test
// also confirm that dereferencing after advancement yields a fresh, live
// proxy, not just that the old one went stale.
//
// Crucially: neither this function nor the frozen consumer ever calls
// `target` or `transition_label` on `original_proxy` *after* the
// advancement below. Doing so would be exactly the "semantic access to a
// stale proxy" the A02 configuration prohibits, and the proxy's private
// `read()` method would fail its assertion if it were ever reached that
// way - this test is written so that assertion is never exercised, because
// its whole point is to observe staleness safely through `is_current()`,
// not to trigger the failure mode it demonstrates.
[[nodiscard]] auto proxy_becomes_stale_after_shared_advancement() -> bool {
  const research::lazy_reference_system system{};
  auto range = system.outgoing_transitions(0);

  // `iterator` and `iterator_copy` are two independent iterator objects,
  // but both hold a pointer to the *same* range, so they share one cursor.
  auto iterator = range.begin();
  auto original_proxy = *iterator;
  auto iterator_copy = iterator;

  // Before anything has been advanced, the proxy taken from the very first
  // dereference must agree that it is still current.
  if (!original_proxy.is_current()) {
    return false;
  }

  // Read label and target through the CPOs, exactly as CON-SUCCESSORS-01
  // does, while the proxy is still guaranteed valid. Source 0's first
  // transition is (advance, 1).
  const auto observed_label = sem::transition_label(system, original_proxy);
  const auto observed_target = sem::target(system, original_proxy);
  if (observed_label != research::reference_label::advance
      || observed_target != 1) {
    return false;
  }

  // Advance the *copy*, not the original iterator - this is the crux of
  // the "shared traversal state" acceptance check: because both iterators
  // point at the same range, this single increment must invalidate the
  // proxy obtained from the *other* iterator, not just from the one that
  // was actually incremented.
  ++iterator_copy;  // advances the shared cursor through either handle.

  // The pre-advancement observation must now be stale. If this were still
  // true, the proxy would be silently carrying its own private copy of
  // "current-ness" instead of genuinely deferring to the range, which
  // would defeat the whole point of the ephemeral-proxy design.
  if (original_proxy.is_current()) {
    return false;
  }

  // Dereferencing either handle after advancement - the one that was
  // incremented, and the one that was not - must observe the *new* shared
  // position (source 0's second transition), not the sentinel and not a
  // stale echo of the first transition. This is what "shared" traversal
  // state means in practice: there is exactly one logical cursor, visible
  // through both handles.
  const auto after_first = *iterator;
  const auto after_second = *iterator_copy;
  if (!after_first.is_current() || !after_second.is_current()) {
    return false;
  }

  // Both fresh proxies are read semantically here - not just checked for
  // is_current() - because "current" alone would not distinguish a correct
  // implementation from one that happened to leave stale data behind but
  // still reported a matching generation. Reading through both handles and
  // requiring (bypass, 2) from each is what actually proves the shared
  // cursor moved to the right position, observably, through either handle.
  const auto first_label = sem::transition_label(system, after_first);
  const auto first_target = sem::target(system, after_first);
  const auto second_label = sem::transition_label(system, after_second);
  const auto second_target = sem::target(system, after_second);

  return first_label == research::reference_label::bypass && first_target == 2
         && second_label == research::reference_label::bypass
         && second_target == 2;
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

  // Runtime half of the A02 "Required conformance checks": proves the
  // ephemeral-proxy invalidation rule holds, independently of the consumer
  // run above.
  if (!proxy_becomes_stale_after_shared_advancement()) {
    std::fputs("lazy ARC-001 subject did not demonstrate ephemeral-proxy "
               "invalidation\n",
               stderr);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
} catch (...) {
  std::fputs("lazy ARC-001 controlled experiment raised an unexpected "
             "exception\n",
             stderr);
  return EXIT_FAILURE;
}
