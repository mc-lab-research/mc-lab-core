#ifndef MC_LAB_SEMANTIC_BEHAVIOR_BUCHI_RUN_HPP
#define MC_LAB_SEMANTIC_BEHAVIOR_BUCHI_RUN_HPP

#include <mc_lab/semantic/behavior/execution_range.hpp>
#include <mc_lab/semantic/system/buchi_automaton.hpp>

/**
 * @file
 * Defines range-shaped carriers for Buchi runs and initial Buchi executions.
 *
 * These concepts validate representation compatibility only. Infinitude,
 * adjacency, initiality, and the recurrence acceptance condition are semantic
 * requirements on range values and cannot in general be inferred from a C++
 * range type.
 */
namespace mc_lab::semantic {

/**
 * A state-sequence carrier compatible with runs of a Buchi automaton.
 *
 * A value `run` represents a Buchi run only when it obeys the additional
 * semantic laws:
 * - `run` is infinite;
 * - every value denotes a valid state of `system`;
 * - each pair of consecutive states, together with the corresponding symbol
 *   of the word being read, belongs to the labelled transition relation.
 *
 * The first state may be any valid state. A finite range can satisfy this
 * compile-time carrier concept, but represents only a finite run fragment,
 * not a complete Buchi run.
 *
 * As with `KripkePathRange`, infinitude is intentionally not approximated
 * with `std::unreachable_sentinel_t`.
 */
template <class System, class Run>
concept BuchiRunRange = BuchiAutomaton<System> && ExecutionRange<Run, state_t<System>>;

/**
 * A state-sequence carrier compatible with initial Buchi executions.
 *
 * In addition to the `BuchiRunRange` laws, a value `execution` represents an
 * execution of the whole automaton only when its first state belongs to
 * `initial_states(system)`.
 *
 * `BuchiRunRange` and `BuchiExecutionRange` have the same syntactic range
 * requirements. Their distinction is semantic: a run may start anywhere,
 * whereas an execution starts in `Q0`. Neither concept checks the recurrence
 * acceptance condition `inf(run) intersect F != {}`; that remains a
 * value-level semantic law over a concrete, fully consumed infinite range.
 */
template <class System, class Execution>
concept BuchiExecutionRange = BuchiRunRange<System, Execution>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_BEHAVIOR_BUCHI_RUN_HPP
