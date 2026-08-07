#ifndef MC_LAB_SEMANTIC_DETAIL_TRANSITION_LABEL_CPO_HPP
#define MC_LAB_SEMANTIC_DETAIL_TRANSITION_LABEL_CPO_HPP

#include <utility>

namespace mc_lab::semantic::detail {
/**
 * Member-backed implementation of the public `transition_label` CPO.
 *
 * Forwarding the witness is important: an outgoing range may expose a stored
 * transition by reference or generate a transition value during iteration.
 */
struct transition_label_fn {
    template <class System, class Transition>
    requires requires(System&& system, Transition&& transition) {
        std::forward<System>(system).transition_label(std::forward<Transition>(transition));
    }
    [[nodiscard]] constexpr decltype(auto)
    operator()(System&& system, Transition&& transition) const noexcept(noexcept(
        std::forward<System>(system).transition_label(std::forward<Transition>(transition)))) {
        return std::forward<System>(system).transition_label(std::forward<Transition>(transition));
    }
};

}  // namespace mc_lab::semantic::detail

#endif  // MC_LAB_SEMANTIC_DETAIL_TRANSITION_LABEL_CPO_HPP
