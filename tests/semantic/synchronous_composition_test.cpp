#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/transition_labelling.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>
#include <mc_lab/semantic/view/synchronous_composition.hpp>

#include "support/facet_models.hpp"

#include <array>
#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <tuple>
#include <type_traits>
#include <utility>

namespace sem = mc_lab::semantic;
namespace models = mc_lab::tests::semantic::models;

namespace {

struct blocked_system {
    [[nodiscard]] constexpr auto initial_states() const noexcept { return std::array{0}; }

    [[nodiscard]] constexpr auto outgoing_transitions(const int&) const noexcept {
        return std::array<int, 0>{};
    }

    [[nodiscard]] constexpr auto target(const int transition) const noexcept -> int {
        return transition;
    }
};

}  // namespace

template <class... Systems>
concept CanSynchronousCompose =
    requires(Systems&&... systems) { sem::synchronous_compose(std::forward<Systems>(systems)...); };

using model = models::explicit_system;
using composition = decltype(sem::synchronous_compose(
    std::declval<model&>(), std::declval<model&>(), std::declval<model&>()));
using product_state = std::tuple<int, int, int>;
using product_transition = std::tuple<models::transition, models::transition, models::transition>;
using heterogeneous_composition = decltype(sem::synchronous_compose(
    std::declval<model&>(), std::declval<models::lazy_system&>(), std::declval<model&>()));

static_assert(CanSynchronousCompose<model&, model&, model&>);
static_assert(CanSynchronousCompose<const model&, const model&, const model&>);
static_assert(!CanSynchronousCompose<model, model&, model&>);
static_assert(
    CanSynchronousCompose<models::lazy_system&, models::lazy_system&, models::lazy_system&>);

static_assert(std::same_as<typename composition::state_type, product_state>);
static_assert(std::same_as<typename composition::transition_type, product_transition>);
static_assert(std::same_as<typename heterogeneous_composition::transition_type,
                           std::tuple<models::transition, int, models::transition>>);
static_assert(sem::InitialStateSet<composition>);
static_assert(sem::TransitionRelation<composition, product_state>);
static_assert(!sem::TransitionLabelling<composition, product_transition>);
static_assert(std::same_as<sem::state_t<composition>, product_state>);
static_assert(
    std::same_as<sem::transition_value_for_t<composition, product_state>, product_transition>);

int run_tests() {
    const model first{};
    const model second{};
    const model third{};
    const auto product = sem::synchronous_compose(first, second, third);

    const auto initial = sem::initial_states(product);
    if (initial.size() != 8 || initial.front() != product_state{0, 0, 0}
        || initial.back() != product_state{1, 1, 1}) {
        std::fputs("synchronous initial-state product is incorrect\n", stderr);
        return EXIT_FAILURE;
    }

    const product_state source{0, 0, 0};
    const auto outgoing = sem::outgoing_transitions(product, source);
    if (outgoing.size() != 8) {
        std::fputs("synchronous transition product has an unexpected size\n", stderr);
        return EXIT_FAILURE;
    }

    std::array<bool, 8> destinations{};
    for (const auto& transition : outgoing) {
        const auto destination = sem::target(product, transition);
        const auto first_index = static_cast<std::size_t>(std::get<0>(destination));
        const auto second_index = static_cast<std::size_t>(std::get<1>(destination));
        const auto third_index = static_cast<std::size_t>(std::get<2>(destination));
        const auto index = first_index * 4U + second_index * 2U + third_index;
        destinations[index] = true;
    }

    for (const bool found : destinations) {
        if (!found) {
            std::fputs("synchronous product omitted a local transition combination\n", stderr);
            return EXIT_FAILURE;
        }
    }

    const blocked_system blocked{};
    const auto blocked_product = sem::synchronous_compose(first, blocked, third);
    if (!sem::outgoing_transitions(blocked_product, product_state{0, 0, 0}).empty()) {
        std::fputs("a blocked component did not block the synchronous product\n", stderr);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main() {
    try {
        return run_tests();
    } catch (const std::exception& error) {
        std::fprintf(stderr, "synchronous composition test threw: %s\n", error.what());
        return EXIT_FAILURE;
    }
}
