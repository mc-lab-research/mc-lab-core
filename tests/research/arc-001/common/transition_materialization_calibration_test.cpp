// ARC-001 stored-subject calibration and frozen semantic oracle.
//
// This executable proves that CON-SUCCESSORS-01 and the expected relation are
// internally consistent for SUBJECT-EXPLICIT-LTS-01 before comparing them with
// a generated subject. It is a research control, not an ordinary production
// contract test. Exact freeze authority:
// f969fa8aa63a9a172e6feb2eeb32636aab24f387 (arc-001-freeze-v1).
#include "support/labelled_successor_consumer.hpp"
#include "support/transition_materialization_models.hpp"

#include <algorithm>
#include <array>
#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <mc_lab/semantic/facet/transition_labelling.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>
#include <ranges>
#include <span>
#include <type_traits>

namespace sem = mc_lab::semantic;
namespace research = mc_lab::tests::semantic::research;

using system_t = research::stored_reference_system;
using witness_t = sem::transition_reference_for_t<system_t, int>;
using observation_t =
    research::labelled_successor<research::reference_label, int>;

static_assert(sem::TransitionRelation<system_t, int>);
static_assert(sem::TransitionLabelling<system_t, witness_t>);
// A span is much stronger than the minimum input-range contract. This positive
// control intentionally supplies stable, sized, borrowed, multipass storage;
// A02 removes those properties while keeping the consumer and oracle fixed.
static_assert(
    std::ranges::input_range<sem::outgoing_transition_range_t<system_t, int>>);
static_assert(std::same_as<witness_t, const research::stored_transition&>);
static_assert(std::same_as<decltype(research::observe_labelled_successors(
                               std::declval<const system_t&>(),
                               std::declval<const int&>())),
                           std::vector<observation_t>>);

template <std::size_t Size>
[[nodiscard]] auto agrees_with_relation(
    std::vector<observation_t> actual,
    std::array<observation_t, Size> expected) -> bool {
  // Transition enumeration order is explicitly non-semantic in ARC-001. Sort
  // copies on the observer side so representation-specific ordering cannot
  // decide the outcome. Equality remains multiplicity-sensitive: omissions or
  // duplicates still fail even though order does not matter.
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

int main() try {
  const system_t stored{};

  // Exercise branching, singleton-successor, and terminal states. These are
  // the complete frozen query set; no conclusion covers arbitrary int states.
  const auto from_zero = research::observe_labelled_successors(stored, 0);
  const auto from_one = research::observe_labelled_successors(stored, 1);
  const auto from_two = research::observe_labelled_successors(stored, 2);

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
    // Any mismatch would invalidate the oracle/consumer calibration before a
    // lazy representation could be interpreted architecturally.
    std::fputs(
        "stored ARC-001 subject disagrees with the labelled-LTS oracle\n",
        stderr);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
} catch (...) {
  std::fputs("stored ARC-001 calibration raised an unexpected exception\n",
             stderr);
  return EXIT_FAILURE;
}
