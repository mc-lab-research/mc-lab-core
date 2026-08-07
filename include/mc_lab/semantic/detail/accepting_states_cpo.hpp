#ifndef MC_LAB_SEMANTIC_DETAIL_ACCEPTING_STATES_CPO_HPP
#define MC_LAB_SEMANTIC_DETAIL_ACCEPTING_STATES_CPO_HPP

#include <utility>

namespace mc_lab::semantic::detail {
/**
 * Member-backed implementation of the public `accepting_states` CPO.
 *
 * Direct member access remains confined to the CPO implementation layer. The
 * public facade isolates concepts and algorithms from the v0.1 dispatch
 * policy.
 */
struct accepting_states_fn {
    template <class System>
    requires requires(System&& system) { std::forward<System>(system).accepting_states(); }
    [[nodiscard]] constexpr decltype(auto) operator()(System&& system) const
        noexcept(noexcept(std::forward<System>(system).accepting_states())) {
        return std::forward<System>(system).accepting_states();
    }
};

}  // namespace mc_lab::semantic::detail

#endif  // MC_LAB_SEMANTIC_DETAIL_ACCEPTING_STATES_CPO_HPP
