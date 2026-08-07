#ifndef MC_LAB_TESTS_SEMANTIC_SUPPORT_MALFORMED_FACET_MODELS_HPP
#define MC_LAB_TESTS_SEMANTIC_SUPPORT_MALFORMED_FACET_MODELS_HPP

#include <array>
#include <span>
#include <string_view>

namespace mc_lab::tests::semantic::malformed {

/** No member provides access to initial states. */
struct missing_initial_states {};

/** An ADL customization alone is deliberately insufficient in v0.1. */
struct adl_only_initial_states {};

[[nodiscard]] constexpr auto initial_states(const adl_only_initial_states&) noexcept {
    return std::array{0};
}

/** No member provides the local image of the relation. */
struct missing_outgoing {};

/** The member exists, but its result is not an input range. */
struct non_range_outgoing {
    [[nodiscard]] constexpr auto outgoing_transitions(const int&) const noexcept -> int {
        return 0;
    }
};

/** Free functions alone must not reintroduce an ADL fallback in v0.1. */
struct adl_only_relation {};
struct adl_only_transition {};

[[nodiscard]] constexpr auto outgoing_transitions(const adl_only_relation&, const int&) noexcept {
    return std::array{adl_only_transition{}};
}

[[nodiscard]] constexpr auto
target(const adl_only_relation&, const adl_only_transition&) noexcept -> int {
    return 0;
}

struct missing_target_transition {};
struct missing_target {
    [[nodiscard]] constexpr auto outgoing_transitions(const int&) const noexcept {
        return std::array{missing_target_transition{}};
    }
};

/**
 * `short` converts to `int`, but it is not the relation's exact state type.
 */
struct incompatible_target_transition {};
struct incompatible_target {
    [[nodiscard]] constexpr auto outgoing_transitions(const int&) const noexcept {
        return std::array{incompatible_target_transition{}};
    }

    [[nodiscard]] constexpr auto
    target(const incompatible_target_transition&) const noexcept -> short {
        return 0;
    }
};

/** A sound unlabelled relation used to prove that labelling is optional. */
struct missing_label_transition {
    int destination;
};
struct missing_label {
    [[nodiscard]] constexpr auto outgoing_transitions(const int&) const noexcept {
        return std::array{missing_label_transition{0}};
    }

    [[nodiscard]] constexpr auto
    target(const missing_label_transition& edge) const noexcept -> int {
        return edge.destination;
    }
};

/** A non-const relation member cannot satisfy the const observation contract.
 */
struct non_const_outgoing {
    [[nodiscard]] constexpr auto outgoing_transitions(const int&) noexcept {
        return std::array{missing_label_transition{0}};
    }

    [[nodiscard]] constexpr auto
    target(const missing_label_transition& edge) const noexcept -> int {
        return edge.destination;
    }
};

/** The range yields lvalues, but `target` accepts only rvalue witnesses. */
struct incorrectly_qualified_target {
    std::array<missing_label_transition, 1> outgoing{{{0}}};

    [[nodiscard]] constexpr auto outgoing_transitions(const int&) const noexcept {
        return std::span<const missing_label_transition>{outgoing};
    }

    [[nodiscard]] constexpr auto target(missing_label_transition&& edge) const noexcept -> int {
        return edge.destination;
    }
};

/** An operation returning `void` does not define a semantic label value. */
struct void_label_transition {
    int destination;
};
struct void_label {
    constexpr void transition_label(const void_label_transition&) const noexcept {}
};

/** A free label customization must not satisfy the member-only protocol. */
struct adl_only_label {};
struct adl_only_label_transition {};

[[nodiscard]] constexpr auto
transition_label(const adl_only_label&, const adl_only_label_transition&) noexcept
    -> std::string_view {
    return "free";
}

/** Atomic-proposition access exists but fails the required range contract. */
struct non_range_propositions {
    [[nodiscard]] constexpr auto atomic_propositions(const int&) const noexcept -> int { return 0; }
};

/** A free proposition customization must not satisfy the v0.1 protocol. */
struct adl_only_propositions {};

[[nodiscard]] constexpr auto
atomic_propositions(const adl_only_propositions&, const int&) noexcept {
    return std::array{std::string_view{"free"}};
}

}  // namespace mc_lab::tests::semantic::malformed

#endif  // MC_LAB_TESTS_SEMANTIC_SUPPORT_MALFORMED_FACET_MODELS_HPP
