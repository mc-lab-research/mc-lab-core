#include <mc_lab/semantic/facet/atomic_proposition_labelling.hpp>
#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/transition_labelling.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>

#include "support/facet_models.hpp"
#include "support/malformed_facet_models.hpp"

#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

namespace sem = mc_lab::semantic;
namespace models = mc_lab::tests::semantic::models;
namespace malformed = mc_lab::tests::semantic::malformed;

// -----------------------------------------------------------------------------
// Stored model: every structural facet is available through borrowed ranges.
// -----------------------------------------------------------------------------

static_assert(sem::InitialStateSet<models::explicit_system>);
static_assert(sem::TransitionRelation<models::explicit_system, int>);
static_assert(sem::TransitionRelation<const models::explicit_system&, const int&>);
static_assert(sem::TransitionLabelling<models::explicit_system, const models::transition&>);
static_assert(sem::AtomicPropositionLabelling<models::explicit_system, int>);

static_assert(std::same_as<sem::state_t<models::explicit_system>, int>);
static_assert(std::same_as<sem::initial_state_reference_t<models::explicit_system>, const int&>);
static_assert(
    std::same_as<sem::transition_value_for_t<models::explicit_system, int>, models::transition>);
static_assert(std::same_as<sem::transition_reference_for_t<models::explicit_system, int>,
                           const models::transition&>);
static_assert(std::same_as<sem::target_result_t<models::explicit_system, int>, const int&>);
static_assert(
    std::same_as<sem::transition_label_for_t<models::explicit_system, const models::transition&>,
                 std::string_view>);
static_assert(
    std::same_as<sem::atomic_proposition_for_t<models::explicit_system, int>, std::string_view>);

// -----------------------------------------------------------------------------
// Generated model: the range itself and each witness are produced by value.
// Labelling is absent and must not leak into the base relation facet.
// -----------------------------------------------------------------------------

static_assert(sem::InitialStateSet<models::lazy_system>);
static_assert(sem::TransitionRelation<models::lazy_system, int>);
static_assert(!sem::TransitionLabelling<models::lazy_system, int>);
static_assert(std::same_as<sem::transition_value_for_t<models::lazy_system, int>, int>);
static_assert(std::same_as<sem::transition_reference_for_t<models::lazy_system, int>, int>);
static_assert(std::same_as<sem::target_result_t<models::lazy_system, int>, int>);

// -----------------------------------------------------------------------------
// Malformed models: each failure is local to the missing semantic role.
// -----------------------------------------------------------------------------

static_assert(!sem::InitialStateSet<malformed::missing_initial_states>);
static_assert(!sem::InitialStateSet<malformed::adl_only_initial_states>);
static_assert(!sem::TransitionRelation<malformed::missing_outgoing, int>);
static_assert(!sem::TransitionRelation<malformed::non_range_outgoing, int>);
static_assert(!sem::TransitionRelation<malformed::adl_only_relation, int>);
static_assert(!sem::TransitionRelation<malformed::missing_target, int>);
static_assert(!sem::TransitionRelation<malformed::incompatible_target, int>);
static_assert(!sem::TransitionRelation<malformed::non_const_outgoing, int>);
static_assert(!sem::TransitionRelation<malformed::incorrectly_qualified_target, int>);

static_assert(sem::TransitionRelation<malformed::missing_label, int>);
static_assert(!sem::TransitionLabelling<malformed::missing_label,
                                        const malformed::missing_label_transition&>);
static_assert(
    !sem::TransitionLabelling<malformed::void_label, const malformed::void_label_transition&>);
static_assert(!sem::TransitionLabelling<malformed::adl_only_label,
                                        const malformed::adl_only_label_transition&>);
static_assert(!sem::AtomicPropositionLabelling<malformed::non_range_propositions, int>);
static_assert(!sem::AtomicPropositionLabelling<malformed::adl_only_propositions, int>);

// -----------------------------------------------------------------------------
// CPO behavior: dispatch preserves result categories and exception contracts.
// -----------------------------------------------------------------------------

static_assert(std::is_empty_v<std::remove_cvref_t<decltype(sem::initial_states)>>);
static_assert(std::is_empty_v<std::remove_cvref_t<decltype(sem::outgoing_transitions)>>);
static_assert(std::is_empty_v<std::remove_cvref_t<decltype(sem::target)>>);
static_assert(std::is_empty_v<std::remove_cvref_t<decltype(sem::transition_label)>>);
static_assert(std::is_empty_v<std::remove_cvref_t<decltype(sem::atomic_propositions)>>);

static_assert(noexcept(sem::initial_states(std::declval<const models::explicit_system&>())));
static_assert(
    !noexcept(sem::initial_states(std::declval<const models::potentially_throwing_system&>())));
static_assert(noexcept(sem::outgoing_transitions(std::declval<const models::explicit_system&>(),
                                                 std::declval<const int&>())));
static_assert(noexcept(sem::target(std::declval<const models::explicit_system&>(),
                                   std::declval<const models::transition&>())));
static_assert(
    std::same_as<decltype(sem::initial_states(std::declval<const models::explicit_system&>())),
                 std::span<const int>>);
static_assert(
    std::same_as<decltype(sem::outgoing_transitions(std::declval<const models::explicit_system&>(),
                                                    std::declval<const int&>())),
                 std::span<const models::transition>>);
static_assert(std::same_as<decltype(sem::target(std::declval<const models::explicit_system&>(),
                                                std::declval<const models::transition&>())),
                           const int&>);
static_assert(
    std::same_as<decltype(sem::transition_label(std::declval<const models::explicit_system&>(),
                                                std::declval<const models::transition&>())),
                 std::string_view>);
static_assert(
    std::same_as<decltype(sem::atomic_propositions(std::declval<const models::explicit_system&>(),
                                                   std::declval<const int&>())),
                 std::span<const std::string_view>>);
static_assert(sem::TransitionRelation<models::potentially_throwing_relation, int>);
static_assert(!noexcept(sem::outgoing_transitions(
    std::declval<const models::potentially_throwing_relation&>(), std::declval<const int&>())));
static_assert(!noexcept(sem::target(std::declval<const models::potentially_throwing_relation&>(),
                                    std::declval<const models::transition&>())));
static_assert(noexcept(sem::transition_label(std::declval<const models::explicit_system&>(),
                                             std::declval<const models::transition&>())));
static_assert(noexcept(sem::atomic_propositions(std::declval<const models::explicit_system&>(),
                                                std::declval<const int&>())));

int main() {
    const models::explicit_system stored{};
    const int source = 0;

    decltype(auto) initial = sem::initial_states(stored);
    if (initial.size() != 2 || initial.front() != 0 || initial.back() != 1) {
        std::fputs("initial_states CPO returned unexpected states\n", stderr);
        return EXIT_FAILURE;
    }

    decltype(auto) outgoing = sem::outgoing_transitions(stored, source);
    if (outgoing.size() != 2) {
        std::fputs("outgoing_transitions CPO returned an unexpected range\n", stderr);
        return EXIT_FAILURE;
    }

    const auto& first = outgoing.front();
    if (sem::target(stored, first) != 1 || sem::transition_label(stored, first) != "advance") {
        std::fputs("transition target or label did not survive CPO dispatch\n", stderr);
        return EXIT_FAILURE;
    }

    decltype(auto) propositions = sem::atomic_propositions(stored, source);
    if (propositions.size() != 1 || propositions.front() != "ready") {
        std::fputs("atomic proposition labelling returned unexpected values\n", stderr);
        return EXIT_FAILURE;
    }

    const models::lazy_system generated{};
    decltype(auto) generated_outgoing = sem::outgoing_transitions(generated, source);
    const auto generated_witness = generated_outgoing.begin();
    if (generated_witness == generated_outgoing.end()
        || sem::target(generated, *generated_witness) != 1) {
        std::fputs("generated transition range failed CPO dispatch\n", stderr);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
