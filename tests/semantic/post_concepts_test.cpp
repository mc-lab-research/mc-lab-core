#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/post.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>
#include <mc_lab/semantic/system/transition_system.hpp>

#include "support/post_models.hpp"

#include <algorithm>
#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <ranges>
#include <type_traits>
#include <utility>

namespace sem = mc_lab::semantic;
namespace models = mc_lab::tests::semantic::post_models;

static_assert(sem::InitialStateSet<models::iterated_map>);
static_assert(sem::Post<models::iterated_map, int>);
static_assert(!sem::TransitionRelation<models::iterated_map, int>);
static_assert(!sem::TransitionSystem<models::iterated_map>);

static_assert(sem::InitialStateSet<models::generated_branching_system>);
static_assert(sem::Post<models::generated_branching_system, int>);
static_assert(!sem::TransitionRelation<models::generated_branching_system, int>);
static_assert(!sem::TransitionSystem<models::generated_branching_system>);

static_assert(sem::Post<models::post_only_system, int>);
static_assert(!sem::InitialStateSet<models::post_only_system>);

static_assert(!sem::Post<models::missing_post, int>);
static_assert(!sem::Post<models::non_range_post, int>);
static_assert(!sem::Post<models::incompatible_post, int>);
static_assert(!sem::Post<models::non_const_post, int>);
static_assert(!sem::Post<models::adl_only_post, int>);

static_assert(std::same_as<sem::post_range_for_t<models::iterated_map, int>, std::array<int, 1>>);
static_assert(std::same_as<sem::post_reference_for_t<models::iterated_map, int>, int&>);
static_assert(std::is_empty_v<std::remove_cvref_t<decltype(sem::post)>>);
static_assert(noexcept(sem::post(std::declval<const models::iterated_map&>(),
                                 std::declval<const int&>())));

int main() {
    const models::iterated_map system{};
    const int source = 3;
    decltype(auto) successors = sem::post(system, source);

    if (successors.size() != 1 || successors.front() != 4) {
        std::fputs("Post did not preserve deterministic state evolution\n", stderr);
        return EXIT_FAILURE;
    }

    const models::generated_branching_system branching{};
    auto successors0 = sem::post(branching, 0);
    auto successors1 = sem::post(branching, 1);
    auto successors2 = sem::post(branching, 2);
    auto successors3 = sem::post(branching, 3);
    auto successors4 = sem::post(branching, 4);

    if (!std::ranges::equal(successors0, std::array{1, 2})
        || !std::ranges::equal(successors1, std::array{3})
        || !std::ranges::equal(successors2, std::array{3, 4})
        || !std::ranges::equal(successors3, std::array<int, 0>{})
        || !std::ranges::equal(successors4, std::array<int, 0>{})) {
        std::fputs("Post did not preserve lazy nondeterministic successors\n", stderr);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
