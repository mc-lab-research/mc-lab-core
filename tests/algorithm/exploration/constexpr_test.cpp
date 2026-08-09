#include <mc_lab/algorithm/exploration/breadth_first_reachable_states.hpp>
#include <mc_lab/algorithm/exploration/fifo_frontier.hpp>

#include "support/reachability_models.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>

/**
 * @file
 * Proves, rather than merely infers from `constexpr` declarations, that the
 * exploration policies and `breadth_first_reachable_states` are usable
 * during constant evaluation. Every check here runs inside a `consteval`
 * function invoked from a `static_assert`; if this file compiles, the
 * checks below already passed at compile time. None of the constructed
 * `std::vector`/`fifo_frontier` values need to escape constant evaluation:
 * they are created, inspected, and destroyed entirely inside each
 * `consteval` function.
 */

namespace exploration = mc_lab::algorithm::exploration;
namespace models = mc_lab::tests::algorithm::exploration::models;

// -----------------------------------------------------------------------------
// fifo_frontier, exercised directly, including a reuse-after-partial-drain
// sequence, inside constant evaluation.
// -----------------------------------------------------------------------------
consteval auto constexpr_fifo_frontier_is_correct() -> bool {
    exploration::fifo_frontier<int> frontier;
    frontier.push(1);
    frontier.push(2);
    frontier.push(3);

    if (frontier.empty()) {
        return false;
    }
    if (frontier.pop() != 1 || frontier.pop() != 2) {
        return false;
    }

    frontier.push(4);  // push while the frontier still holds one pending state
    if (frontier.pop() != 3 || frontier.pop() != 4) {
        return false;
    }
    return frontier.empty();
}

static_assert(constexpr_fifo_frontier_is_correct());

// -----------------------------------------------------------------------------
// breadth_first_reachable_states, both dispatch branches, inside constant
// evaluation.
// -----------------------------------------------------------------------------
consteval auto constexpr_bfs_post_is_correct() -> bool {
    const models::post_only_system system{};
    const auto result = exploration::breadth_first_reachable_states(system);
    return std::ranges::equal(result, std::array{0, 1, 2, 3, 4});
}

static_assert(constexpr_bfs_post_is_correct());

consteval auto constexpr_bfs_transition_relation_is_correct() -> bool {
    const models::transition_relation_only_system system{};
    const auto result = exploration::breadth_first_reachable_states(system);
    return std::ranges::equal(result, std::array{0, 1, 2});
}

static_assert(constexpr_bfs_transition_relation_is_correct());

int main() { return EXIT_SUCCESS; }
