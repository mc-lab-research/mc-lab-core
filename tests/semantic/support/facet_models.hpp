#ifndef MC_LAB_TESTS_SEMANTIC_SUPPORT_FACET_MODELS_HPP
#define MC_LAB_TESTS_SEMANTIC_SUPPORT_FACET_MODELS_HPP

#include <array>
#include <ranges>
#include <span>
#include <string_view>

namespace mc_lab::tests::semantic::models {

/** Stored transition witness used by the explicit reference model. */
struct transition {
    int destination;
    std::string_view label;
};

/**
 * Small stored model exercising borrowed ranges and all structural facets.
 * It is a compile-time architecture fixture, not a production SemTL model.
 */
struct explicit_system {
    std::array<int, 2> initial{0, 1};
    std::array<transition, 2> outgoing{{{1, "advance"}, {0, "reset"}}};
    std::array<std::string_view, 1> propositions{"ready"};

    /** Returns a borrowed view of the stored initial-state set. */
    [[nodiscard]] constexpr auto initial_states() const noexcept -> std::span<const int> {
        return initial;
    }

    /** Exposes stored witnesses by reference without allocating or copying. */
    [[nodiscard]] constexpr auto
    outgoing_transitions(const int&) const noexcept -> std::span<const transition> {
        return outgoing;
    }

    /** The target is a reference into the stored transition witness. */
    [[nodiscard]] constexpr auto target(const transition& edge) const noexcept -> const int& {
        return edge.destination;
    }

    /** Returns the neutral label attached to a stored witness. */
    [[nodiscard]] constexpr auto
    transition_label(const transition& edge) const noexcept -> std::string_view {
        return edge.label;
    }

    /** Returns the set-shaped proposition view for the supplied state. */
    [[nodiscard]] constexpr auto
    atomic_propositions(const int&) const noexcept -> std::span<const std::string_view> {
        return propositions;
    }
};

/** Generated unlabelled model whose iterator yields transition values. */
struct lazy_system {
    [[nodiscard]] constexpr auto initial_states() const noexcept { return std::array{0}; }

    [[nodiscard]] constexpr auto outgoing_transitions(const int& source) const noexcept {
        return std::views::iota(source + 1, source + 2);
    }

    [[nodiscard]] constexpr auto target(const int transition) const noexcept -> int {
        return transition;
    }
};

/** Fixture used solely to check negative `noexcept` propagation. */
struct potentially_throwing_system {
    [[nodiscard]] auto initial_states() const { return std::array{0}; }
};

/** Valid relation whose members intentionally lack `noexcept`. */
struct potentially_throwing_relation {
    std::array<transition, 1> outgoing{{{1, "advance"}}};

    [[nodiscard]] auto outgoing_transitions(const int&) const -> std::span<const transition> {
        return outgoing;
    }

    [[nodiscard]] auto target(const transition& edge) const -> int { return edge.destination; }
};

}  // namespace mc_lab::tests::semantic::models

#endif  // MC_LAB_TESTS_SEMANTIC_SUPPORT_FACET_MODELS_HPP
