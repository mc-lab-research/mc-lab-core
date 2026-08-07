#ifndef MC_LAB_SEMANTIC_BEHAVIOR_EXECUTION_HPP
#define MC_LAB_SEMANTIC_BEHAVIOR_EXECUTION_HPP

#include <mc_lab/semantic/behavior/execution_range.hpp>
#include <mc_lab/semantic/system/transition_system.hpp>

/**
 * @file
 * Associates range-shaped execution carriers with transition systems.
 *
 * The system-independent `ExecutionRange` contract lives in
 * `execution_range.hpp`. This header adds only the semantic association with
 * the recognized `TransitionSystem` formalism.
 */
namespace mc_lab::semantic {

/**
 * An execution-range representation compatible with a transition system.
 *
 * For a value `execution` and a model `system`, membership in the behavioral
 * semantics of `system` additionally obeys these semantic laws:
 * - `execution` is non-empty;
 * - its first state belongs to `initial_states(system)`;
 * - every pair of consecutive states belongs to the transition relation.
 *
 * Therefore, if `execution = s0, s1, ...`, then `s0` is initial and each
 * `si -> s(i+1)` is a transition. The range may terminate or continue
 * indefinitely.
 *
 * The behavioral semantics is the mathematical set of all values obeying
 * those laws. This concept does not require the system to enumerate that set;
 * such access would be a separate computational capability or algorithm.
 */
template <class System, class Execution>
concept TransitionSystemExecutionRange =
    TransitionSystem<System> && ExecutionRange<Execution, state_t<System>>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_BEHAVIOR_EXECUTION_HPP
