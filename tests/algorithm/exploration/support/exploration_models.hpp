#ifndef MC_LAB_TESTS_ALGORITHM_EXPLORATION_SUPPORT_EXPLORATION_MODELS_HPP
#define MC_LAB_TESTS_ALGORITHM_EXPLORATION_SUPPORT_EXPLORATION_MODELS_HPP

#include "reachability_models.hpp"

#include <array>
#include <span>
#include <string>
#include <vector>

namespace mc_lab::tests::algorithm::exploration::models {

/** A single state whose only successor is itself. */
struct self_loop_system {
    std::array<int, 1> initial{0};
    std::array<int, 1> successors{0};

    [[nodiscard]] constexpr auto initial_states() const noexcept -> std::span<const int> {
        return initial;
    }

    [[nodiscard]] constexpr auto post(const int&) const noexcept -> std::span<const int> {
        return successors;
    }
};

/** A two-state cycle: 0 -> 1 -> 0. */
struct cycle_system {
    std::array<int, 1> initial{0};
    std::array<int, 1> from_zero{1};
    std::array<int, 1> from_one{0};

    [[nodiscard]] constexpr auto initial_states() const noexcept -> std::span<const int> {
        return initial;
    }

    [[nodiscard]] constexpr auto post(const int& source) const noexcept -> std::span<const int> {
        return source == 0 ? std::span<const int>{from_zero} : std::span<const int>{from_one};
    }
};

/** Two independent paths converge on state 3: 0 -> {1, 2}; 1 -> {3}; 2 -> {3}. */
struct convergence_system {
    std::array<int, 1> initial{0};
    std::array<int, 2> from_zero{1, 2};
    std::array<int, 1> from_one{3};
    std::array<int, 1> from_two{3};

    [[nodiscard]] constexpr auto initial_states() const noexcept -> std::span<const int> {
        return initial;
    }

    [[nodiscard]] constexpr auto post(const int& source) const noexcept -> std::span<const int> {
        if (source == 0) {
            return from_zero;
        }
        if (source == 1) {
            return from_one;
        }
        if (source == 2) {
            return from_two;
        }
        return {};
    }
};

/** A two-state transition-relation cycle: 0 -> 1 -> 0, via a single witness each. */
struct transition_cycle_system {
    std::array<int, 1> initial{0};
    std::array<transition, 1> from_zero{{{1}}};
    std::array<transition, 1> from_one{{{0}}};

    [[nodiscard]] constexpr auto initial_states() const noexcept -> std::span<const int> {
        return initial;
    }

    [[nodiscard]] constexpr auto
    outgoing_transitions(const int& source) const noexcept -> std::span<const transition> {
        return source == 0 ? std::span<const transition>{from_zero}
                           : std::span<const transition>{from_one};
    }

    [[nodiscard]] constexpr auto target(const transition& edge) const noexcept -> int {
        return edge.destination;
    }
};

/**
 * Counts how many times `target()` is actually invoked. Two witnesses from
 * state 0 both target state 1 (parallel transitions), so a caller that
 * fully explores this system and calls `target` exactly once per examined
 * transition witness observes `target_call_count == 2`; a caller that
 * accidentally recomputed the target (for example, once for
 * `examine_transition` and again for the membership check) would observe a
 * higher count. This makes single-computation a test-observable property
 * rather than something only code inspection can confirm.
 */
struct target_call_counting_system {
    std::array<int, 1> initial{0};
    std::array<transition, 2> from_zero{{{1}, {1}}};
    mutable int target_call_count = 0;

    [[nodiscard]] constexpr auto initial_states() const noexcept -> std::span<const int> {
        return initial;
    }

    [[nodiscard]] constexpr auto
    outgoing_transitions(const int& source) const noexcept -> std::span<const transition> {
        return source == 0 ? std::span<const transition>{from_zero} : std::span<const transition>{};
    }

    [[nodiscard]] constexpr auto target(const transition& edge) const noexcept -> int {
        ++target_call_count;
        return edge.destination;
    }
};

/**
 * Records every observer callback as a short, ordered log entry. Satisfies
 * both `StateObserver` and `TransitionObserver` so it can drive either
 * kernel in the same test.
 */
struct recording_observer {
    std::vector<std::string> log;

    void discover_state(const int& state) {
        log.push_back("discover_state:" + std::to_string(state));
    }

    void examine_state(const int& state) {
        log.push_back("examine_state:" + std::to_string(state));
    }

    void examine_successor(const int& source, const int& successor) {
        log.push_back("examine_successor:" + std::to_string(source) + "->"
                      + std::to_string(successor));
    }

    void tree_edge(const int& source, const int& successor) {
        log.push_back("tree_edge:" + std::to_string(source) + "->" + std::to_string(successor));
    }

    void examine_transition(const int& source, const transition& edge, const int& target) {
        log.push_back("examine_transition:" + std::to_string(source) + "->" + std::to_string(target)
                      + "#" + std::to_string(edge.destination));
    }

    void tree_transition(const int& source, const transition& edge, const int& target) {
        log.push_back("tree_transition:" + std::to_string(source) + "->" + std::to_string(target)
                      + "#" + std::to_string(edge.destination));
    }

    void finish_state(const int& state) { log.push_back("finish_state:" + std::to_string(state)); }
};

}  // namespace mc_lab::tests::algorithm::exploration::models

#endif  // MC_LAB_TESTS_ALGORITHM_EXPLORATION_SUPPORT_EXPLORATION_MODELS_HPP
