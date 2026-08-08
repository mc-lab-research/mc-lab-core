#include <mc_lab/algorithm/exploration/breadth_first_reachable_states.hpp>
#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/post.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>

#include "support/reachability_models.hpp"

#include <algorithm>
#include <array>
#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <ranges>

namespace algo = mc_lab::algorithm::exploration;
namespace sem = mc_lab::semantic;
namespace models = mc_lab::tests::algorithm::exploration::models;

template <class System>
concept CanEnumerateReachableStates =
    requires(const System& system) { algo::breadth_first_reachable_states(system); };

static_assert(sem::InitialStateSet<models::post_only_system>);
static_assert(sem::Post<models::post_only_system, int>);
static_assert(!sem::TransitionRelation<models::post_only_system, int>);
static_assert(CanEnumerateReachableStates<models::post_only_system>);

static_assert(sem::InitialStateSet<models::transition_relation_only_system>);
static_assert(!sem::Post<models::transition_relation_only_system, int>);
static_assert(sem::TransitionRelation<models::transition_relation_only_system, int>);
static_assert(CanEnumerateReachableStates<models::transition_relation_only_system>);

static_assert(sem::Post<models::both_facets_system, int>);
static_assert(sem::TransitionRelation<models::both_facets_system, int>);
static_assert(CanEnumerateReachableStates<models::both_facets_system>);

static_assert(!CanEnumerateReachableStates<models::initial_states_only_system>);

namespace {

template <std::ranges::input_range Actual, std::ranges::input_range Expected>
[[nodiscard]] auto
expect_equal(const Actual& actual, const Expected& expected, const char* failure) -> bool {
    if (!std::ranges::equal(actual, expected)) {
        std::fputs(failure, stderr);
        std::fputc('\n', stderr);
        return false;
    }
    return true;
}

}  // namespace

namespace {

auto run_tests() -> int {
    const auto post_reachable = algo::breadth_first_reachable_states(models::post_only_system{});
    if (!expect_equal(post_reachable,
                      std::array{0, 1, 2, 3, 4},
                      "Post exploration did not preserve unique breadth-first discovery")) {
        return EXIT_FAILURE;
    }

    const auto transition_reachable =
        algo::breadth_first_reachable_states(models::transition_relation_only_system{});
    if (!expect_equal(transition_reachable,
                      std::array{0, 1, 2},
                      "Transition exploration did not project targets or remove duplicates")) {
        return EXIT_FAILURE;
    }

    const auto preferred_post = algo::breadth_first_reachable_states(models::both_facets_system{});
    if (!expect_equal(preferred_post,
                      std::array{0, 2, 1},
                      "Post was not preferred when both local-evolution facets were available")) {
        return EXIT_FAILURE;
    }

    const auto empty = algo::breadth_first_reachable_states(models::empty_system{});
    if (!empty.empty()) {
        std::fputs("An empty initial-state set produced reachable states\n", stderr);
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
