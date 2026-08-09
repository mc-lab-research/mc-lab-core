#ifndef MC_LAB_SEMANTIC_ACCEPTANCE_FINAL_STATE_ACCEPTANCE_HPP
#define MC_LAB_SEMANTIC_ACCEPTANCE_FINAL_STATE_ACCEPTANCE_HPP

#include <mc_lab/semantic/acceptance/accepting_state_set.hpp>

/** @file Defines final-state acceptance conditions for finite executions. */
namespace mc_lab::semantic {

/**
 * An accepting-state-set condition interpreted at the end of a finite run.
 *
 * The compile-time shape is an `AcceptingStateSet`. Its distinguishing law is
 * behavioral: a non-empty finite execution is accepted exactly when its last
 * state belongs to `accepting_states(condition)`.
 */
template <class Condition, class State>
concept FinalStateAcceptanceCondition = AcceptingStateSet<Condition, State>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_ACCEPTANCE_FINAL_STATE_ACCEPTANCE_HPP
