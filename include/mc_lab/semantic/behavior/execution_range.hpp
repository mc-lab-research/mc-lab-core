#ifndef MC_LAB_SEMANTIC_BEHAVIOR_EXECUTION_RANGE_HPP
#define MC_LAB_SEMANTIC_BEHAVIOR_EXECUTION_RANGE_HPP

#include <concepts>
#include <ranges>
#include <type_traits>

/**
 * @file
 * Defines the system-independent range carrier used for state executions.
 *
 * This header contains representation vocabulary only. It does not associate
 * the range with a recognized system formalism or import that formalism's
 * semantic laws.
 */
namespace mc_lab::semantic {

/** State value type carried by an execution range. */
template <class Execution>
requires std::ranges::input_range<Execution>
using execution_state_t = std::ranges::range_value_t<Execution>;

/** Reference, value, or proxy observed while consuming an execution range. */
template <class Execution>
requires std::ranges::input_range<Execution>
using execution_state_reference_t = std::ranges::range_reference_t<Execution>;

/**
 * A potentially finite or infinite input range carrying one state domain.
 *
 * The normalized range value type must be exactly `State`. References and
 * range proxies remain permitted through `execution_state_reference_t`.
 *
 * This concept checks representational compatibility only. In particular, it
 * cannot prove that a runtime range is non-empty, finite, infinite, or related
 * to a system.
 */
template <class Execution, class State>
concept ExecutionRange = std::ranges::input_range<Execution>
    && std::same_as<std::remove_cvref_t<execution_state_t<Execution>>, std::remove_cvref_t<State>>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_BEHAVIOR_EXECUTION_RANGE_HPP
