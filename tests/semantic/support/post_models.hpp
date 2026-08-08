#ifndef MC_LAB_TESTS_SEMANTIC_SUPPORT_POST_MODELS_HPP
#define MC_LAB_TESTS_SEMANTIC_SUPPORT_POST_MODELS_HPP

#include <array>
#include <ranges>

namespace mc_lab::tests::semantic::post_models {

/**
 * Deterministic state evolution used to drive the state-oriented `Post`
 * facet without manufacturing transition witnesses.
 */
struct iterated_map {
    [[nodiscard]] constexpr auto initial_states() const noexcept { return std::array{0}; }

    [[nodiscard]] constexpr auto post(const int& state) const noexcept {
        return std::array{state + 1};
    }
};

/**
 * Lazy nondeterministic successor generation over a small state domain.
 * Filtering an iota view proves that `Post` does not require stored ranges or
 * deterministic `State -> State` evolution.
 */
struct generated_branching_system {
    [[nodiscard]] constexpr auto initial_states() const noexcept { return std::array{0}; }

    [[nodiscard]] constexpr auto post(const int& source) const {
        return std::views::iota(1, 5) | std::views::filter([source](const int candidate) {
                   switch (source) {
                   case 0:
                       return candidate <= 2;
                   case 1:
                       return candidate == 3;
                   case 2:
                       return candidate >= 3;
                   default:
                       return false;
                   }
               });
    }
};

/** Valid local `Post` facet without initial-state access. */
struct post_only_system {
    [[nodiscard]] constexpr auto post(const int& state) const noexcept { return std::array{state}; }
};

/** No member provides direct successor-state access. */
struct missing_post {};

/** The member exists, but its result is not a range. */
struct non_range_post {
    [[nodiscard]] constexpr auto post(const int&) const noexcept -> int { return 0; }
};

/** A convertible but distinct successor value type is rejected in v0.1. */
struct incompatible_post {
    [[nodiscard]] constexpr auto post(const int&) const noexcept { return std::array<short, 1>{0}; }
};

/** A non-const member cannot satisfy the logical-const observation contract. */
struct non_const_post {
    [[nodiscard]] constexpr auto post(const int& state) noexcept { return std::array{state}; }
};

/** A free function alone must not satisfy the member-only v0.1 protocol. */
struct adl_only_post {};

[[nodiscard]] constexpr auto post(const adl_only_post&, const int& state) noexcept {
    return std::array{state};
}

}  // namespace mc_lab::tests::semantic::post_models

#endif  // MC_LAB_TESTS_SEMANTIC_SUPPORT_POST_MODELS_HPP
