#ifndef MC_LAB_SEMANTIC_ACCEPTANCE_BUCHI_ACCEPTANCE_HPP
#define MC_LAB_SEMANTIC_ACCEPTANCE_BUCHI_ACCEPTANCE_HPP

#include <mc_lab/semantic/acceptance/accepting_state_set.hpp>

/** @file Defines Buchi acceptance conditions for infinite runs. */
namespace mc_lab::semantic {

/**
 * An accepting-state-set condition interpreted recurrently over an infinite
 * run.
 *
 * The compile-time shape is an `AcceptingStateSet`. Its distinguishing law is
 * behavioral: a run is accepted exactly when it visits at least one state in
 * `accepting_states(condition)` infinitely often.
 */
template <class Condition, class State>
concept BuchiAcceptanceCondition = AcceptingStateSet<Condition, State>;

}  // namespace mc_lab::semantic

#endif  // MC_LAB_SEMANTIC_ACCEPTANCE_BUCHI_ACCEPTANCE_HPP
