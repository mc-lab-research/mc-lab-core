#ifndef MC_LAB_TESTS_ALGORITHM_EXPLORATION_SUPPORT_REACHABILITY_MODELS_HPP
#define MC_LAB_TESTS_ALGORITHM_EXPLORATION_SUPPORT_REACHABILITY_MODELS_HPP

#include <array>
#include <ranges>
#include <span>

namespace mc_lab::tests::algorithm::exploration::models {

struct transition {
    int destination;
};

/** Exercises direct successor-state exploration, convergence, cycles, and duplicates. */
struct post_only_system {
    [[nodiscard]] constexpr auto initial_states() const noexcept { return std::array{0, 0}; }

    [[nodiscard]] constexpr auto post(const int& source) const {
        return std::views::iota(0, 5) | std::views::filter([source](const int candidate) {
                   switch (source) {
                   case 0:
                       return candidate == 1 || candidate == 2;
                   case 1:
                       return candidate == 3;
                   case 2:
                       return candidate == 3 || candidate == 4;
                   case 3:
                       return candidate == 3;
                   default:
                       return false;
                   }
               });
    }
};

/** Exercises transition projection and parallel witnesses sharing one target. */
struct transition_relation_only_system {
    std::array<transition, 2> from_zero{{{1}, {1}}};
    std::array<transition, 1> from_one{{{2}}};

    [[nodiscard]] constexpr auto initial_states() const noexcept { return std::array{0}; }

    [[nodiscard]] constexpr auto
    outgoing_transitions(const int& source) const noexcept -> std::span<const transition> {
        if (source == 0) {
            return from_zero;
        }
        if (source == 1) {
            return from_one;
        }
        return {};
    }

    [[nodiscard]] constexpr auto target(const transition& edge) const noexcept -> const int& {
        return edge.destination;
    }
};

/** Coherent dual-facet fixture whose range orders make dispatch observable. */
struct both_facets_system {
    std::array<int, 2> post_successors{2, 1};
    std::array<transition, 2> transitions{{{1}, {2}}};

    [[nodiscard]] constexpr auto initial_states() const noexcept { return std::array{0}; }

    [[nodiscard]] constexpr auto post(const int& source) const noexcept {
        if (source == 0) {
            return std::span<const int>{post_successors};
        }
        return std::span<const int>{};
    }

    [[nodiscard]] constexpr auto
    outgoing_transitions(const int& source) const noexcept -> std::span<const transition> {
        if (source == 0) {
            return transitions;
        }
        return {};
    }

    [[nodiscard]] constexpr auto target(const transition& edge) const noexcept -> const int& {
        return edge.destination;
    }
};

struct empty_system {
    [[nodiscard]] constexpr auto initial_states() const noexcept { return std::array<int, 0>{}; }

    [[nodiscard]] constexpr auto post(const int&) const noexcept { return std::array<int, 0>{}; }
};

struct initial_states_only_system {
    [[nodiscard]] constexpr auto initial_states() const noexcept { return std::array{0}; }
};

}  // namespace mc_lab::tests::algorithm::exploration::models

#endif  // MC_LAB_TESTS_ALGORITHM_EXPLORATION_SUPPORT_REACHABILITY_MODELS_HPP
