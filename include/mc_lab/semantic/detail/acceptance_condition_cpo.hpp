#ifndef MC_LAB_SEMANTIC_DETAIL_ACCEPTANCE_CONDITION_CPO_HPP
#define MC_LAB_SEMANTIC_DETAIL_ACCEPTANCE_CONDITION_CPO_HPP

#include <utility>

namespace mc_lab::semantic::detail {

/** Member-backed implementation of the public `acceptance_condition` CPO. */
struct acceptance_condition_fn {
    template <class System>
    requires requires(System&& system) { std::forward<System>(system).acceptance_condition(); }
    [[nodiscard]] constexpr decltype(auto) operator()(System&& system) const
        noexcept(noexcept(std::forward<System>(system).acceptance_condition())) {
        return std::forward<System>(system).acceptance_condition();
    }
};

}  // namespace mc_lab::semantic::detail

#endif  // MC_LAB_SEMANTIC_DETAIL_ACCEPTANCE_CONDITION_CPO_HPP
