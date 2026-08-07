#include <mc_lab/semantic/behavior/execution.hpp>
#include <mc_lab/semantic/behavior/kripke_path.hpp>
#include <mc_lab/semantic/system/kripke_structure.hpp>
#include <mc_lab/semantic/system/transition_system.hpp>

#include "support/facet_models.hpp"
#include "support/malformed_facet_models.hpp"

#include <array>
#include <concepts>
#include <ranges>

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
// Execution carriers: finite and infinite ranges use the same state domain.
// -----------------------------------------------------------------------------

using finite_execution = std::array<int, 3>;
using empty_execution = std::array<int, 0>;
using wrong_state_execution = std::array<short, 3>;
using infinite_execution = decltype(std::views::iota(0));

static_assert(sem::ExecutionRange<finite_execution, int>);
static_assert(sem::ExecutionRange<const finite_execution&, int>);
static_assert(sem::ExecutionRange<infinite_execution, int>);

// Emptiness is a value-level semantic law, not a property of the carrier type.
static_assert(sem::ExecutionRange<empty_execution, int>);

static_assert(!sem::ExecutionRange<wrong_state_execution, int>);
static_assert(!sem::ExecutionRange<int, int>);

static_assert(std::same_as<sem::execution_state_t<finite_execution>, int>);
static_assert(std::same_as<sem::execution_state_reference_t<finite_execution>, int&>);
static_assert(std::same_as<sem::execution_state_reference_t<const finite_execution>, const int&>);

static_assert(sem::TransitionSystemExecutionRange<models::explicit_system, finite_execution>);
static_assert(sem::TransitionSystemExecutionRange<models::lazy_system, infinite_execution>);
static_assert(!sem::TransitionSystemExecutionRange<models::explicit_system, wrong_state_execution>);
static_assert(
    !sem::TransitionSystemExecutionRange<models::potentially_throwing_system, finite_execution>);

// -----------------------------------------------------------------------------
// Kripke behavioral carriers: paths and initial executions share a state type.
// -----------------------------------------------------------------------------

static_assert(sem::KripkePathRange<models::explicit_system, infinite_execution>);
static_assert(sem::KripkeExecutionRange<models::explicit_system, infinite_execution>);
static_assert(!sem::KripkePathRange<models::explicit_system, wrong_state_execution>);
static_assert(!sem::KripkePathRange<models::lazy_system, infinite_execution>);

// This checks carrier compatibility only. A finite value is a path fragment,
// not a complete Kripke path under the documented semantic requirements.
static_assert(sem::KripkePathRange<models::explicit_system, finite_execution>);

int main() { return 0; }
