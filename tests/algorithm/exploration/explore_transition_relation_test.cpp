#include <mc_lab/algorithm/exploration/explore_transition_relation.hpp>
#include <mc_lab/algorithm/exploration/fifo_frontier.hpp>
#include <mc_lab/algorithm/exploration/linear_visited_registry.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>

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
//
// explore_transition_relation's own requires-clause conjoins
// Frontier<FrontierPolicy, State>, VisitedRegistry<VisitedPolicy, State>, and
// TransitionObserver<ObserverPolicy, System, State> directly, so proving each
// protocol concept rejects an incomplete policy (below) is exactly the
// rejection explore_transition_relation itself performs.
// -----------------------------------------------------------------------------

namespace {

struct observer_missing_tree_transition {
    void discover_state(const int&) {}
    void examine_state(const int&) {}
    void examine_transition(const int&, const models::transition&, const int&) {}
    void finish_state(const int&) {}
};

}  // namespace

static_assert(!exploration::TransitionObserver<observer_missing_tree_transition,
                                               models::transition_relation_only_system,
                                               int>);
static_assert(exploration::TransitionObserver<models::recording_observer,
                                              models::transition_relation_only_system,
                                              int>);

// explore_transition_relation's requires-clause also conjoins
// semantic::TransitionRelation<System, state_t<System>> directly. A system
// with no transition relation at all (initial_states_only_system) and a
// system with only Post (post_only_system) both fail that conjunct, so
// neither can satisfy explore_transition_relation's constraints.
static_assert(!sem::TransitionRelation<models::initial_states_only_system, int>);
static_assert(!sem::TransitionRelation<models::post_only_system, int>);

namespace {

// -----------------------------------------------------------------------------
// Exploration via TransitionRelation, target projection, and parallel
// transitions sharing one target: transitions_relation_only_system has two
// witnesses from state 0, both targeting state 1.
// -----------------------------------------------------------------------------
auto test_target_projection_and_parallel_transitions() -> bool {
    exploration::fifo_frontier<int> frontier;
    exploration::linear_visited_registry<int> visited;
    models::recording_observer observer;

    exploration::explore_transition_relation(
        models::transition_relation_only_system{}, frontier, visited, observer);

    const std::vector<std::string> expected{
        "discover_state:0",
        "examine_state:0",
        "examine_transition:0->1#1",
        "tree_transition:0->1#1",
        "discover_state:1",
        "examine_transition:0->1#1",
        "finish_state:0",
        "examine_state:1",
        "examine_transition:1->2#2",
        "tree_transition:1->2#2",
        "discover_state:2",
        "finish_state:1",
        "examine_state:2",
        "finish_state:2",
    };
    if (observer.log != expected) {
        std::fputs(
            "target projection / parallel-transition callback sequence did not match expectations\n",
            stderr);
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

    exploration::explore_transition_relation(
        models::transition_cycle_system{}, frontier, visited, observer);

    const std::vector<std::string> expected{
        "discover_state:0",
        "examine_state:0",
        "examine_transition:0->1#1",
        "tree_transition:0->1#1",
        "discover_state:1",
        "finish_state:0",
        "examine_state:1",
        "examine_transition:1->0#0",
        "finish_state:1",
    };
    if (observer.log != expected) {
        std::fputs("cycle did not produce the expected callback sequence\n", stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// finish_state is called exactly once per popped state, after every outgoing
// transition of that state has been examined.
// -----------------------------------------------------------------------------
auto test_finish_state_event() -> bool {
    exploration::fifo_frontier<int> frontier;
    exploration::linear_visited_registry<int> visited;
    models::recording_observer observer;

    exploration::explore_transition_relation(
        models::transition_relation_only_system{}, frontier, visited, observer);

    int finish_zero_index = -1;
    int last_examine_from_zero_index = -1;
    for (std::size_t i = 0; i < observer.log.size(); ++i) {
        if (observer.log[i] == "finish_state:0") {
            finish_zero_index = static_cast<int>(i);
        }
        if (observer.log[i].rfind("examine_transition:0->", 0) == 0) {
            last_examine_from_zero_index = static_cast<int>(i);
        }
    }
    if (finish_zero_index == -1 || last_examine_from_zero_index == -1
        || finish_zero_index < last_examine_from_zero_index) {
        std::fputs("finish_state:0 did not occur after every transition from state 0\n", stderr);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// target(system, transition) is called exactly once per examined transition
// witness, not once for examine_transition and again for the membership
// check or tree_transition.
// -----------------------------------------------------------------------------
auto test_target_called_once_per_transition() -> bool {
    exploration::fifo_frontier<int> frontier;
    exploration::linear_visited_registry<int> visited;
    models::recording_observer observer;
    models::target_call_counting_system system;

    exploration::explore_transition_relation(system, frontier, visited, observer);

    // state 0 has exactly two outgoing transitions; target() must be called
    // exactly once per witness, i.e. exactly twice in total.
    if (system.target_call_count != 2) {
        std::fputs("target() was not called exactly once per examined transition\n", stderr);
        return false;
    }
    return true;
}

auto run_tests() -> int {
    if (!test_target_projection_and_parallel_transitions() || !test_cycle()
        || !test_finish_state_event() || !test_target_called_once_per_transition()) {
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
