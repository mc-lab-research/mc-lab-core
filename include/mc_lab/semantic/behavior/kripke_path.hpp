#ifndef MC_LAB_SEMANTIC_BEHAVIOR_KRIPKE_PATH_HPP
#define MC_LAB_SEMANTIC_BEHAVIOR_KRIPKE_PATH_HPP

#include <mc_lab/semantic/behavior/execution_range.hpp>
#include <mc_lab/semantic/system/kripke_structure.hpp>

/**
 * @file
 * Defines range-shaped carriers for Kripke paths and initial executions.
 *
 * These concepts validate representation compatibility. Infinitude,
 * adjacency, and initiality are semantic requirements on range values and
 * cannot in general be inferred from a C++ range type.
 */
namespace mc_lab::semantic {

/**
 * A state-sequence carrier compatible with paths of a Kripke structure.
 *
 * A value `path` represents a Kripke path only when it obeys the additional
 * semantic laws:
 * - `path` is infinite;
 * - every value denotes a valid state of `system`;
 * - each pair of consecutive states belongs to the transition relation.
 *
 * The first state may be any valid state. A finite range can satisfy this
 * compile-time carrier concept, but represents only a finite path fragment,
 * not a complete Kripke path.
 *
 * Infinitude is intentionally not approximated with
 * `std::unreachable_sentinel_t`: that sentinel is one possible C++
 * representation of an infinite sequence, not its semantic definition.
 */
template <class System, class Path>
concept KripkePathRange = KripkeStructure<System> && ExecutionRange<Path, state_t<System>>;

/**
 * A state-sequence carrier compatible with initial Kripke executions.
 *
 * In addition to the `KripkePathRange` laws, a value `execution` represents an
 * execution of the whole structure only when its first state belongs to
 * `initial_states(system)`.
 *
 * `KripkePathRange` and `KripkeExecutionRange` have the same syntactic range
 * requirements. Their distinction is semantic: a path may start anywhere,
 * whereas an execution starts in `I`.
 */
template <class System, class Execution>
concept KripkeExecutionRange = KripkePathRange<System, Execution>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_BEHAVIOR_KRIPKE_PATH_HPP
