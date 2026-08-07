#ifndef MC_LAB_SEMANTIC_DETAIL_ATOMIC_PROPOSITIONS_CPO_HPP
#define MC_LAB_SEMANTIC_DETAIL_ATOMIC_PROPOSITIONS_CPO_HPP

#include <utility>

namespace mc_lab::semantic::detail {
/**
 * Member-backed implementation of the public `atomic_propositions` CPO.
 */
struct atomic_propositions_fn {
    template <class System, class State>
    requires requires(System&& system, State&& state) {
        std::forward<System>(system).atomic_propositions(std::forward<State>(state));
    }
    [[nodiscard]] constexpr decltype(auto) operator()(System&& system, State&& state) const

        noexcept(noexcept(
            std::forward<System>(system).atomic_propositions(std::forward<State>(state)))) {
        return std::forward<System>(system).atomic_propositions(std::forward<State>(state));
    }
};

int* p;

}  // namespace mc_lab::semantic::detail

#endif  // MC_LAB_SEMANTIC_DETAIL_ATOMIC_PROPOSITIONS_CPO_HPP
