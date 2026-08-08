#include <mc_lab/semantic/system/buchi_automaton.hpp>
#include <mc_lab/semantic/system/finite_automaton.hpp>
#include <mc_lab/semantic/system/kripke_structure.hpp>
#include <mc_lab/semantic/system/transition_system.hpp>

#include "support/facet_models.hpp"
#include "support/malformed_facet_models.hpp"

#include <concepts>
#include <string_view>

namespace sem = mc_lab::semantic;
namespace models = mc_lab::tests::semantic::models;
namespace malformed = mc_lab::tests::semantic::malformed;

// -----------------------------------------------------------------------------
// System recognition: initial states and the relation share one state domain.
// -----------------------------------------------------------------------------

static_assert(sem::TransitionSystem<models::explicit_system>);
static_assert(sem::TransitionSystem<models::lazy_system>);
static_assert(sem::TransitionSystem<const models::explicit_system&>);

// Initial-state access alone does not define a transition system.
static_assert(!sem::TransitionSystem<models::potentially_throwing_system>);

// A transition relation alone does not define a transition system.
static_assert(!sem::TransitionSystem<models::potentially_throwing_relation>);
static_assert(!sem::TransitionSystem<malformed::missing_label>);

// -----------------------------------------------------------------------------
// Kripke recognition: state propositions extend the minimal system structure.
// -----------------------------------------------------------------------------

static_assert(sem::KripkeStructure<models::explicit_system>);
static_assert(sem::KripkeStructure<const models::explicit_system&>);

// The generated system has no atomic-proposition labelling facet.
static_assert(!sem::KripkeStructure<models::lazy_system>);
static_assert(!sem::KripkeStructure<models::potentially_throwing_system>);
static_assert(!sem::KripkeStructure<malformed::missing_initial_states>);

// -----------------------------------------------------------------------------
// Finite-automaton recognition: atomic structural facets are joined to an
// associated final-state acceptance condition.
// -----------------------------------------------------------------------------

static_assert(sem::FiniteAutomaton<models::explicit_system>);
static_assert(sem::FiniteAutomaton<const models::explicit_system&>);
static_assert(std::same_as<sem::symbol_t<models::explicit_system>, std::string_view>);

// The generated system has an acceptance condition but no transition labelling.
static_assert(!sem::FiniteAutomaton<models::lazy_system>);
static_assert(!sem::FiniteAutomaton<models::potentially_throwing_system>);

// Initial states and labelled transitions alone omit acceptance association.
static_assert(!sem::FiniteAutomaton<malformed::missing_acceptance_relation>);

// -----------------------------------------------------------------------------
// Buchi-automaton recognition: structurally identical to FiniteAutomaton, but
// a distinct concept since each formalism states its own acceptance laws.
// -----------------------------------------------------------------------------

static_assert(sem::BuchiAutomaton<models::explicit_system>);
static_assert(sem::BuchiAutomaton<const models::explicit_system&>);

// The generated system has an acceptance condition but no transition labelling.
static_assert(!sem::BuchiAutomaton<models::lazy_system>);
static_assert(!sem::BuchiAutomaton<models::potentially_throwing_system>);

// Initial states and labelled transitions alone omit acceptance association.
static_assert(!sem::BuchiAutomaton<malformed::missing_acceptance_relation>);

int main() { return 0; }
