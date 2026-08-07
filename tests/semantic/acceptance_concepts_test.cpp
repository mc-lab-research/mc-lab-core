#include <mc_lab/semantic/acceptance/acceptance_condition.hpp>
#include <mc_lab/semantic/acceptance/accepting_state_set.hpp>
#include <mc_lab/semantic/acceptance/buchi_acceptance.hpp>
#include <mc_lab/semantic/acceptance/final_state_acceptance.hpp>

#include "support/facet_models.hpp"
#include "support/malformed_facet_models.hpp"

#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <span>
#include <type_traits>
#include <utility>

namespace sem = mc_lab::semantic;
namespace models = mc_lab::tests::semantic::models;
namespace malformed = mc_lab::tests::semantic::malformed;

using condition = models::accepting_state_condition;

static_assert(sem::HasAcceptanceCondition<models::explicit_system>);
static_assert(sem::HasAcceptanceCondition<const models::explicit_system&>);
static_assert(std::same_as<sem::acceptance_condition_t<models::explicit_system>, condition>);

static_assert(sem::AcceptingStateSet<condition, int>);
static_assert(sem::FinalStateAcceptanceCondition<condition, int>);
static_assert(sem::BuchiAcceptanceCondition<condition, int>);
static_assert(std::same_as<sem::accepting_state_range_t<condition>, std::span<const int>>);
static_assert(std::same_as<sem::accepting_state_reference_t<condition>, const int&>);

static_assert(!sem::HasAcceptanceCondition<malformed::missing_acceptance_condition>);
static_assert(!sem::HasAcceptanceCondition<malformed::void_acceptance_condition>);
static_assert(!sem::HasAcceptanceCondition<malformed::adl_only_acceptance_condition>);
static_assert(!sem::AcceptingStateSet<malformed::missing_accepting_states, int>);
static_assert(!sem::AcceptingStateSet<malformed::non_range_accepting_states, int>);
static_assert(!sem::AcceptingStateSet<malformed::incompatible_accepting_states, int>);

static_assert(std::is_empty_v<std::remove_cvref_t<decltype(sem::acceptance_condition)>>);
static_assert(std::is_empty_v<std::remove_cvref_t<decltype(sem::accepting_states)>>);
static_assert(noexcept(sem::acceptance_condition(std::declval<const models::explicit_system&>())));
static_assert(noexcept(sem::accepting_states(std::declval<const condition&>())));
static_assert(std::same_as<
              decltype(sem::acceptance_condition(std::declval<const models::explicit_system&>())),
              const condition&>);

int main() {
    const models::explicit_system system{};

    decltype(auto) associated = sem::acceptance_condition(system);
    decltype(auto) states = sem::accepting_states(associated);
    if (states.size() != 1 || states.front() != 1) {
        std::fputs("acceptance condition returned unexpected states\n", stderr);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
