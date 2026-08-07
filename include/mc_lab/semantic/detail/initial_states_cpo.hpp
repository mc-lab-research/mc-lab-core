#ifndef MC_LAB_SEMANTIC_DETAIL_INITIAL_STATES_CPO_HPP
#define MC_LAB_SEMANTIC_DETAIL_INITIAL_STATES_CPO_HPP

#include <utility>

namespace mc_lab::semantic::detail {
/**
 * Member-backed implementation of the public `initial_states` CPO.
 *
 * Direct member access remains confined to the CPO implementation layer. The
 * public facade isolates concepts and algorithms from the v0.1 dispatch
 * policy.
 */
struct initial_states_fn {
    template <class System>
    requires requires(System&& system) { std::forward<System>(system).initial_states(); }
    [[nodiscard]] constexpr decltype(auto) operator()(System&& system) const
        noexcept(noexcept(std::forward<System>(system).initial_states())) {
        return std::forward<System>(system).initial_states();
    }
};

}  // namespace mc_lab::semantic::detail

#endif  // MC_LAB_SEMANTIC_DETAIL_INITIAL_STATES_CPO_HPP
