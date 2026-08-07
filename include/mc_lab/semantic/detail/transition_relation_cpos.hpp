#ifndef MC_LAB_SEMANTIC_DETAIL_TRANSITION_RELATION_CPOS_HPP
#define MC_LAB_SEMANTIC_DETAIL_TRANSITION_RELATION_CPOS_HPP

#include <utility>

namespace mc_lab::semantic::detail {
/**
 * Member-backed implementation of the public `outgoing_transitions` CPO.
 *
 * SemTL v0.1 deliberately has one customization route for transition
 * relations. Keeping that route behind a CPO leaves generic callers
 * independent from the dispatch policy.
 */
struct outgoing_transitions_fn {
    template <class System, class State>
    requires requires(System&& system, State&& state) {
        std::forward<System>(system).outgoing_transitions(std::forward<State>(state));
    }
    [[nodiscard]] constexpr decltype(auto)
    operator()(System&& system, State&& state) const noexcept(
        noexcept(std::forward<System>(system).outgoing_transitions(std::forward<State>(state)))) {
        return std::forward<System>(system).outgoing_transitions(std::forward<State>(state));
    }
};

/** Member-backed implementation of the public `target` CPO. */
struct target_fn {
    template <class System, class Transition>
    requires requires(System&& system, Transition&& transition) {
        std::forward<System>(system).target(std::forward<Transition>(transition));
    }
    [[nodiscard]] constexpr decltype(auto)
    operator()(System&& system, Transition&& transition) const noexcept(
        noexcept(std::forward<System>(system).target(std::forward<Transition>(transition)))) {
        return std::forward<System>(system).target(std::forward<Transition>(transition));
    }
};

}  // namespace mc_lab::semantic::detail

#endif  // MC_LAB_SEMANTIC_DETAIL_TRANSITION_RELATION_CPOS_HPP
