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

// SUBJECT-LAZY-LTS-01 (A02) frozen acceptance checks.
static_assert(std::ranges::input_range<lazy_range_t>);
static_assert(!std::ranges::forward_range<lazy_range_t>);
static_assert(!std::ranges::sized_range<lazy_range_t>);
static_assert(!std::ranges::borrowed_range<lazy_range_t>);

// The range's actual reference form is the ephemeral proxy.
static_assert(
    std::same_as<witness_t, research::lazy_outgoing_transitions::proxy>);

// The relevant SemTL concepts hold for the lazy subject's actual proxy form.
static_assert(sem::TransitionRelation<system_t, int>);
static_assert(sem::TransitionLabelling<system_t, witness_t>);

// CON-SUCCESSORS-01 remains source-unchanged; this only checks that it still
// compiles and produces the same owning observation type for the lazy
// subject as it does for the stored subject.
static_assert(std::same_as<decltype(research::observe_labelled_successors(
                               std::declval<const system_t&>(),
                               std::declval<const int&>())),
                           std::vector<observation_t>>);

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
 * Frozen A02 negative/lifetime demonstration, following the pseudocode in
 * ARC-001-A02-ephemeral-proxy-freeze.md exactly:
 *
 *   auto iterator = range.begin();
 *   auto original_proxy = *iterator;
 *   auto iterator_copy = iterator;
 *
 *   assert(original_proxy.is_current());
 *   observe_label(original_proxy);
 *   observe_target(original_proxy);
 *
 *   ++iterator_copy;
 *
 *   assert(!original_proxy.is_current());
 *
 * Neither this function nor the frozen consumer calls `target` or
 * `transition_label` on `original_proxy` after the advancement below - that
 * would be a stale access, and the proxy's private reader asserts against
 * exactly that.
 */
[[nodiscard]] auto proxy_becomes_stale_after_shared_advancement() -> bool {
  const research::lazy_reference_system system{};
  auto range = system.outgoing_transitions(0);

  auto iterator = range.begin();
  auto original_proxy = *iterator;
  auto iterator_copy = iterator;

  if (!original_proxy.is_current()) {
    return false;
  }

  const auto observed_label = sem::transition_label(system, original_proxy);
  const auto observed_target = sem::target(system, original_proxy);
  if (observed_label != research::reference_label::advance
      || observed_target != 1) {
    return false;
  }

  ++iterator_copy;  // advances the shared cursor through either handle.

  if (original_proxy.is_current()) {
    return false;  // the pre-advancement observation must now be stale.
  }

  // Dereferencing either handle after advancement must observe the new
  // shared position (source 0's second transition), not the sentinel.
  const auto after_first = *iterator;
  const auto after_second = *iterator_copy;
  if (!after_first.is_current() || !after_second.is_current()) {
    return false;
  }

  const auto next_label = sem::transition_label(system, after_second);
  const auto next_target = sem::target(system, after_second);
  return next_label == research::reference_label::bypass && next_target == 2;
}

int main() try {
  const system_t lazy{};

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
