#ifndef MC_LAB_SEMANTIC_VIEW_SYNCHRONOUS_COMPOSITION_HPP
#define MC_LAB_SEMANTIC_VIEW_SYNCHRONOUS_COMPOSITION_HPP

#include <mc_lab/semantic/facet/initial_state_set.hpp>
#include <mc_lab/semantic/facet/transition_relation.hpp>

#include <concepts>
#include <cstddef>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

/**
 * @file
 * Defines strict, variadic synchronous composition as a semantic system view.
 *
 * Every global step makes every component take one local transition. Choices
 * are independent: the operation neither observes nor synchronizes transition
 * labels. The resulting relation is the direct product of the component
 * relations.
 */
namespace mc_lab::semantic {

namespace detail {

template <class System>
using synchronous_state_t = state_t<std::remove_cvref_t<System>>;

template <class System>
using synchronous_transition_t =
    transition_value_for_t<std::remove_cvref_t<System>, synchronous_state_t<System>>;

template <class System>
using synchronous_transition_reference_t =
    transition_reference_for_t<std::remove_cvref_t<System>, synchronous_state_t<System>>;

template <class System>
concept StableSynchronousOperand = InitialStateSet<std::remove_cvref_t<System>>
    && TransitionRelation<std::remove_cvref_t<System>, synchronous_state_t<System>>
    && std::copy_constructible<synchronous_state_t<System>>
    && std::copy_constructible<synchronous_transition_t<System>>
    && std::constructible_from<synchronous_state_t<System>,
                               initial_state_reference_t<std::remove_cvref_t<System>>>
    && std::constructible_from<synchronous_transition_t<System>,
                               synchronous_transition_reference_t<System>>
    && requires(const std::remove_cvref_t<System>& system,
                const synchronous_transition_t<System>& transition) {
           requires std::same_as<
               std::remove_cvref_t<decltype(mc_lab::semantic::target(system, transition))>,
               synchronous_state_t<System>>;
       };

}  // namespace detail

/**
 * A non-owning view of the strict synchronous product of two or more systems.
 *
 * For operands `S1, ..., Sn`, the view exposes flat product states and product
 * transition witnesses:
 *
 * @code
 * using state_type = std::tuple<state_t<S1>, ..., state_t<Sn>>;
 * using transition_type = std::tuple<transition_t<S1>, ..., transition_t<Sn>>;
 * @endcode
 *
 * Minimal structural contract:
 * - every operand models `InitialStateSet`;
 * - every operand models `TransitionRelation` on its own state domain.
 *
 * No transition-labelling or state-labelling facet is required or propagated.
 * Labels carried by operands play no role in this algebraic operation.
 *
 * Semantic laws:
 * - initial states are the Cartesian product of all component initial sets;
 * - every global step advances every component exactly once;
 * - local transition choices are independent;
 * - the global relation contains every tuple of locally enabled transitions
 *   and no other transition;
 * - if one component has no outgoing transition, the product is blocked.
 *
 * Lifetime:
 * - the view stores pointers to lvalue operands and never owns them;
 * - every operand must outlive the view;
 * - `synchronous_compose` rejects temporary systems.
 *
 * Evaluation model and v0.1 representation restriction:
 * - the complete product state space and relation are never materialized;
 * - each observation locally materializes only the range it returns;
 * - local materialization may allocate and propagate allocation exceptions;
 * - state and transition values must therefore be copy constructible;
 * - this stability requirement belongs to the view representation, not to the
 *   mathematical synchronous-product operation.
 */
template <class... Systems>
requires(sizeof...(Systems) >= 2) && (detail::StableSynchronousOperand<Systems> && ...)
class synchronous_composition_view {
  public:
    using state_type = std::tuple<detail::synchronous_state_t<Systems>...>;
    using transition_type = std::tuple<detail::synchronous_transition_t<Systems>...>;

    constexpr explicit synchronous_composition_view(const Systems&... systems) noexcept
        : systems_{std::addressof(systems)...} {}

    /** Returns a stable local materialization of the Cartesian initial set. */
    [[nodiscard]] auto initial_states() const -> std::vector<state_type> {
        std::vector<state_type> result;
        append_initial_states<0>(std::tuple<>{}, result);
        return result;
    }

    /** Returns the Cartesian product of all locally enabled transitions. */
    [[nodiscard]] auto
    outgoing_transitions(const state_type& source) const -> std::vector<transition_type> {
        std::vector<transition_type> result;
        append_transitions<0>(source, std::tuple<>{}, result);
        return result;
    }

    /** Applies each component target operation to its local witness. */
    [[nodiscard]] auto target(const transition_type& transition) const -> state_type {
        return target_impl(transition, std::index_sequence_for<Systems...>{});
    }

  private:
    template <std::size_t Index>
    [[nodiscard]] constexpr auto system() const noexcept -> const auto& {
        return *std::get<Index>(systems_);
    }

    template <std::size_t Index, class Prefix>
    void append_initial_states(Prefix prefix, std::vector<state_type>& result) const {
        if constexpr (Index == sizeof...(Systems)) {
            result.emplace_back(std::move(prefix));
        } else {
            auto states = mc_lab::semantic::initial_states(system<Index>());
            using component_state_type = std::tuple_element_t<Index, state_type>;

            for (auto&& state : states) {
                append_initial_states<Index + 1>(
                    std::tuple_cat(prefix, std::tuple<component_state_type>{state}), result);
            }
        }
    }

    template <std::size_t Index, class Prefix>
    void append_transitions(const state_type& source,
                            Prefix prefix,
                            std::vector<transition_type>& result) const {
        if constexpr (Index == sizeof...(Systems)) {
            result.emplace_back(std::move(prefix));
        } else {
            auto transitions =
                mc_lab::semantic::outgoing_transitions(system<Index>(), std::get<Index>(source));
            using component_transition_type = std::tuple_element_t<Index, transition_type>;

            for (auto&& transition : transitions) {
                append_transitions<Index + 1>(
                    source,
                    std::tuple_cat(prefix, std::tuple<component_transition_type>{transition}),
                    result);
            }
        }
    }

    template <std::size_t... Index>
    [[nodiscard]] auto
    target_impl(const transition_type& transition, std::index_sequence<Index...>) const
        -> state_type {
        return state_type{
            mc_lab::semantic::target(system<Index>(), std::get<Index>(transition))...};
    }

    std::tuple<const Systems*...> systems_;
};

/**
 * Constructs a non-owning strict synchronous-composition view.
 *
 * The lvalue-reference parameters deliberately reject temporaries. All
 * operands participate in every global step; no labels are inspected.
 */
template <class... Systems>
requires(sizeof...(Systems) >= 2)
    && (detail::StableSynchronousOperand<std::remove_cvref_t<Systems>> && ...)
[[nodiscard]] constexpr auto synchronous_compose(Systems&... systems) {
    return synchronous_composition_view<std::remove_cvref_t<Systems>...>{systems...};
}

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_VIEW_SYNCHRONOUS_COMPOSITION_HPP
