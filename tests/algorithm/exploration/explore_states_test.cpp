#include <mc_lab/algorithm/exploration/explore_states.hpp>
#include <mc_lab/algorithm/exploration/fifo_frontier.hpp>
#include <mc_lab/algorithm/exploration/linear_visited_registry.hpp>
#include <mc_lab/semantic/facet/post.hpp>

#include "support/exploration_models.hpp"
#include "support/reachability_models.hpp"

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include <vector>

namespace exploration = mc_lab::algorithm::exploration;
namespace sem = mc_lab::semantic;
namespace models = mc_lab::tests::algorithm::exploration::models;

// -----------------------------------------------------------------------------
// Compile-time rejection of incomplete policies.
// -----------------------------------------------------------------------------

namespace {

struct frontier_missing_pop {
    [[nodiscard]] auto empty() const noexcept -> bool { return true; }
    void push(int) {}
};

struct visited_missing_try_insert {};

struct observer_missing_tree_edge {
    void discover_state(const int&) {}
    void examine_state(const int&) {}
    void examine_successor(const int&, const int&) {}
    void finish_state(const int&) {}
};

}  // namespace

static_assert(!exploration::Frontier<frontier_missing_pop, int>);
static_assert(!exploration::VisitedRegistry<visited_missing_try_insert, int>);
static_assert(
    !exploration::StateObserver<observer_missing_tree_edge, models::post_only_system, int>);

static_assert(
    exploration::StateObserver<models::recording_observer, models::post_only_system, int>);

// explore_states's requires-clause also conjoins semantic::Post<System,
// state_t<System>> directly. A system with no local-evolution facet at all
// (initial_states_only_system) and a system with only TransitionRelation
// (transition_relation_only_system) both fail that conjunct, so neither can
// satisfy explore_states's constraints.
static_assert(!sem::Post<models::initial_states_only_system, int>);
static_assert(!sem::Post<models::transition_relation_only_system, int>);

// explore_states's own requires-clause conjoins Frontier<FrontierPolicy, State>,
// VisitedRegistry<VisitedPolicy, State>, and StateObserver<ObserverPolicy, System, State>
// directly, so each rejection proved above is exactly the rejection explore_states
// itself performs; there is no separate policy-acceptance rule to test.

namespace {

// -----------------------------------------------------------------------------
// Discovery of initial states, including duplicates.
// -----------------------------------------------------------------------------
auto test_initial_state_discovery() -> bool {
    // post_only_system supplies the initial states {0, 0}: a duplicate.
    exploration::fifo_frontier<int> frontier;
    exploration::linear_visited_registry<int> visited;
    models::recording_observer observer;

    exploration::explore_states(models::post_only_system{}, frontier, visited, observer);

    // Exactly one discover_state:0, not two, despite the duplicate initial.
    int discover_zero_count = 0;
    for (const auto& entry : observer.log) {
        if (entry == "discover_state:0") {
            ++discover_zero_count;
        }
    }
    if (discover_zero_count != 1) {
        std::fputs("initial-state duplicate was discovered more than once\n", stderr);
        return false;
    }
    if (observer.log.front() != "discover_state:0") {
        std::fputs("the initial state was not discovered first\n", stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// Reflexive loop: a state whose only successor is itself.
// -----------------------------------------------------------------------------
auto test_reflexive_loop() -> bool {
    exploration::fifo_frontier<int> frontier;
    exploration::linear_visited_registry<int> visited;
    models::recording_observer observer;

    exploration::explore_states(models::self_loop_system{}, frontier, visited, observer);

    const std::vector<std::string> expected{
        "discover_state:0",
        "examine_state:0",
        "examine_successor:0->0",
        "finish_state:0",
    };
    if (observer.log != expected) {
        std::fputs("reflexive loop did not produce the expected callback sequence\n", stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// Cycle: 0 -> 1 -> 0 must terminate and discover both states exactly once.
// -----------------------------------------------------------------------------
auto test_cycle() -> bool {
    exploration::fifo_frontier<int> frontier;
    exploration::linear_visited_registry<int> visited;
    models::recording_observer observer;

    exploration::explore_states(models::cycle_system{}, frontier, visited, observer);

    const std::vector<std::string> expected{
        "discover_state:0",
        "examine_state:0",
        "examine_successor:0->1",
        "tree_edge:0->1",
        "discover_state:1",
        "finish_state:0",
        "examine_state:1",
        "examine_successor:1->0",
        "finish_state:1",
    };
    if (observer.log != expected) {
        std::fputs("cycle did not produce the expected callback sequence\n", stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// Convergence: two paths reach the same state, which is discovered once.
// -----------------------------------------------------------------------------
auto test_convergence() -> bool {
    exploration::fifo_frontier<int> frontier;
    exploration::linear_visited_registry<int> visited;
    models::recording_observer observer;

    exploration::explore_states(models::convergence_system{}, frontier, visited, observer);

    int discover_three_count = 0;
    int tree_edge_into_three_count = 0;
    for (const auto& entry : observer.log) {
        if (entry == "discover_state:3") {
            ++discover_three_count;
        }
        if (entry == "tree_edge:1->3" || entry == "tree_edge:2->3") {
            ++tree_edge_into_three_count;
        }
    }
    if (discover_three_count != 1 || tree_edge_into_three_count != 1) {
        std::fputs("convergent state was discovered or tree-edged more than once\n", stderr);
        return false;
    }
    // examine_transition/examine_successor still happens for the non-tree edge.
    bool saw_examine_from_both_sources = false;
    for (std::size_t i = 0; i + 1 < observer.log.size(); ++i) {
        if (observer.log[i] == "examine_successor:1->3"
            || observer.log[i] == "examine_successor:2->3") {
            saw_examine_from_both_sources = true;
        }
    }
    if (!saw_examine_from_both_sources) {
        std::fputs("expected at least one examine_successor call into the convergent state\n",
                   stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// Exact callback order across a branching, converging, self-looping system.
// -----------------------------------------------------------------------------
auto test_exact_callback_order() -> bool {
    exploration::fifo_frontier<int> frontier;
    exploration::linear_visited_registry<int> visited;
    models::recording_observer observer;

    exploration::explore_states(models::post_only_system{}, frontier, visited, observer);

    const std::vector<std::string> expected{
        "discover_state:0",       "examine_state:0",        "examine_successor:0->1",
        "tree_edge:0->1",         "discover_state:1",       "examine_successor:0->2",
        "tree_edge:0->2",         "discover_state:2",       "finish_state:0",
        "examine_state:1",        "examine_successor:1->3", "tree_edge:1->3",
        "discover_state:3",       "finish_state:1",         "examine_state:2",
        "examine_successor:2->3", "examine_successor:2->4", "tree_edge:2->4",
        "discover_state:4",       "finish_state:2",         "examine_state:3",
        "examine_successor:3->3", "finish_state:3",         "examine_state:4",
        "finish_state:4",
    };
    if (observer.log != expected) {
        std::fputs("exact callback order did not match the expected BFS visitor sequence\n",
                   stderr);
        return false;
    }
    return true;
}

auto run_tests() -> int {
    if (!test_initial_state_discovery() || !test_reflexive_loop() || !test_cycle()
        || !test_convergence() || !test_exact_callback_order()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

}  // namespace

int main() {
    try {
        return run_tests();
    } catch (const std::exception& error) {
        std::fprintf(stderr, "Unexpected exception: %s\n", error.what());
    } catch (...) {
        std::fputs("Unexpected non-standard exception\n", stderr);
    }
    return EXIT_FAILURE;
}
