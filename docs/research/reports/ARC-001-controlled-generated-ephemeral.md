# ARC-001 controlled experiment - generated ephemeral witnesses

**Status:** Draft. Interpretation is AI-advised (Claude, Implementation and
Experiment Support). Human assessment, Claim-impact approval, and any
architectural decision are pending Human ARC Owner / Human Experiment Owner
Sami Lazreg and Project Principal Sami Lazreg. This report does not close
ARC-001, does not approve CL-001, and does not authorize an architectural
decision.

## Identifiers

- **Frozen commit (immutable experiment boundary):**
  `f969fa8aa63a9a172e6feb2eeb32636aab24f387`
- **Freeze-record commit:** `1dec900c899893537c9f1d87b3b26e272d22808f`
  (docs-only; verified to make no change to
  `tests/semantic/support/labelled_successor_consumer.hpp`,
  `tests/semantic/transition_materialization_calibration_test.cpp`,
  `tests/semantic/support/transition_materialization_models.hpp`,
  `include/mc_lab/semantic/facet/transition_relation.hpp`,
  `include/mc_lab/semantic/facet/transition_labelling.hpp`, or
  `include/mc_lab/semantic/detail/transition_relation_cpos.hpp`)
- **Experiment branch:** `experiment/arc-001-generated-ephemeral`
- **Experiment commit:** `2539f7f741f31f53ebcb7314dc9f9ac989a6ab16`,
  branched from `1dec900c899893537c9f1d87b3b26e272d22808f`
- **Consumer:** `CON-SUCCESSORS-01`,
  `tests/semantic/support/labelled_successor_consumer.hpp`
- **Oracle:** `tests/semantic/transition_materialization_calibration_test.cpp`
- **Controlled subject added:** `SUBJECT-LAZY-LTS-01`,
  `tests/semantic/support/transition_materialization_lazy_model.hpp`
- **Controlled test added:**
  `tests/semantic/transition_materialization_lazy_test.cpp`
  (registered as `mc_lab_core.semantic.transition_materialization_lazy`)

## Consumer unchanged - proof

```
git diff f969fa8aa63a9a172e6feb2eeb32636aab24f387 -- \
  tests/semantic/support/labelled_successor_consumer.hpp \
  tests/semantic/transition_materialization_calibration_test.cpp \
  tests/semantic/support/transition_materialization_models.hpp \
  include/mc_lab/semantic/facet/transition_relation.hpp \
  include/mc_lab/semantic/facet/transition_labelling.hpp \
  include/mc_lab/semantic/detail/transition_relation_cpos.hpp
```

Output: empty. `CON-SUCCESSORS-01`, the oracle, the stored subject, and the
frozen contracts are byte-for-byte unchanged in the experiment commit
relative to the frozen boundary. Only the following were added or touched,
none of them frozen files:

- `tests/semantic/support/transition_materialization_lazy_model.hpp` (new)
- `tests/semantic/transition_materialization_lazy_test.cpp` (new)
- `tests/semantic/CMakeLists.txt` (new build target/test registration only)

## SUBJECT-LAZY-LTS-01 design

- `lazy_reference_system::outgoing_transitions(const int&)` returns
  `lazy_outgoing_transitions`, a range that stores only the queried source
  and a cursor (`step_`); it does not store or construct any transition
  collection.
- `lazy_outgoing_transitions::iterator` holds a raw pointer back to the range
  object. Copies of the iterator alias the same range, so they share the
  same cursor: advancing one copy advances what any other copy observes.
- `operator*()` computes the current witness on demand via
  `compute_lazy_transition(source, step)`, a `switch`-based function that
  returns the label/target pair for the requested step or `std::nullopt`
  when the local image is exhausted. It returns a plain
  `lazy_transition_witness` by value - a fresh temporary each call, with no
  stable address and no persistent identity.
- The iterator declares `iterator_concept = std::input_iterator_tag`
  explicitly and provides no `iterator==iterator` comparison, so it is
  input-only by construction, not by incidental omission.
- No `size()`, no `enable_borrowed_range` specialization, no cache.

## Lazy-range category assertions (compile-time)

From `tests/semantic/transition_materialization_lazy_test.cpp`:

```cpp
static_assert(std::ranges::input_range<lazy_range_t>);
static_assert(!std::ranges::forward_range<lazy_range_t>);
static_assert(!std::ranges::sized_range<lazy_range_t>);
static_assert(!std::ranges::borrowed_range<lazy_range_t>);
static_assert(std::same_as<witness_t, research::lazy_transition_witness>);
static_assert(sem::TransitionRelation<system_t, int>);
static_assert(sem::TransitionLabelling<system_t, witness_t>);
static_assert(std::same_as<decltype(research::observe_labelled_successors(
                               std::declval<const system_t&>(),
                               std::declval<const int&>())),
                           std::vector<observation_t>>);
```

All of these compiled successfully, including the last one, which
instantiates the unmodified `CON-SUCCESSORS-01` body against
`SUBJECT-LAZY-LTS-01`.

## Storage and cache inspection

Manual source inspection of
`tests/semantic/support/transition_materialization_lazy_model.hpp`:

- `lazy_reference_system` holds no data members.
- `lazy_outgoing_transitions` holds exactly two `int`s: `source_` and
  `step_`.
- `lazy_outgoing_transitions::iterator` holds exactly one pointer back to
  the range.
- `compute_lazy_transition` is a pure function over `(source, step)`; it
  allocates nothing and retains nothing between calls.
- No `std::vector`, `std::array`, `std::span`, or other container of
  transitions appears anywhere in the header.
- No member or free function returns a stable reference into stored state;
  `target` and `transition_label` take `const lazy_transition_witness&` and
  read only the by-value witness passed to them.

No persistent transition collection or hidden complete cache was found.

## Runtime observations

`observe_labelled_successors` (unmodified `CON-SUCCESSORS-01`) run against
`SUBJECT-LAZY-LTS-01`:

- source `0`: `{(advance, 1), (bypass, 2)}` - agrees with the oracle.
- source `1`: `{(reset, 0)}` - agrees with the oracle.
- source `2`: `{}` - agrees with the oracle.

No label or transition occurrence was omitted or duplicated (order-
independent, multiplicity-sensitive comparison, same comparator as the
frozen oracle test).

A dedicated runtime check (`copied_iterators_share_traversal_state`) copies
an iterator, dereferences the copy, advances the original, and re-
dereferences the copy: the copy's observation changes and matches the
original's new position, demonstrating the frozen single-pass aliasing
requirement directly and independently of the consumer.

## Value-category outcome

The frozen consumer binds each observation to the named variable `witness`
(`for (auto&& witness : outgoing_transitions(system, source))`), which is a
named lvalue expression inside the loop body regardless of its declared
reference-collapsed type. `SUBJECT-LAZY-LTS-01`'s `target` and
`transition_label` members were implemented to take
`const lazy_transition_witness&`, which accepts both the lvalue form used by
the consumer and the rvalue-forwarded form used by the `TargetCallable` /
`TransitionLabelCallable` concept checks (which invoke the CPOs through
`std::forward<transition_reference_for_t<...>>`, and since that type is a
plain value type, the forward call casts to an rvalue).

Both value-category forms therefore succeed against the same accessor
signatures. **The prospectively classified named-lvalue/value-category
discriminator (ARC-001, "Prospective value-category classification") was
not triggered.** This is a design choice available to the ARC's permitted
"model-local member implementations behind current CPOs," not a change to
the consumer, a concept, or a CPO - the frozen negative-test intent (no
`forward_range`, `sized_range`, `borrowed_range`, stable address, or second
pass required) is what was tested, and no failure occurred while doing so.

## Environment

- OS: Windows NT 10.0.26200.0
- Compiler: `clang-cl`, clang version 22.1.8 (MSVC-compatible driver),
  target `x86_64-pc-windows-msvc`
- Standard library: MSVC STL (via clang-cl's default `/EHsc` MSVC-ABI mode)
- CMake: 4.4.0
- Ninja: 1.13.2
- Generator/preset: Ninja via `windows-clangcl-release` (commit-gate) and
  `windows-clangcl-debug` (tidy), toolchain contract `WINDOWS_CLANGCL`
- Build type: Release (commit-gate), Debug (tidy)
- C++ mode: C++20, extensions off, warnings as errors ON (commit-gate)
- Visual Studio: 18.8.12021.73 (Community), providing the x64 developer
  environment consumed by clang-cl

No portability claim is made beyond this single environment.

## Commands and raw results

```
cmake --workflow --preset windows-clangcl-commit-gate --fresh
```

Result: formatting check passed (33 files, after `clang-format -i` was
applied to the two new files); build succeeded with warnings-as-errors on;
all 13 registered tests passed, including
`mc_lab_core.semantic.transition_materialization_calibration` (frozen,
unaffected) and `mc_lab_core.semantic.transition_materialization_lazy`
(new, controlled).

```
cmake --workflow --preset windows-clangcl-tidy --fresh
```

Result: `clang-tidy passed for 7 translation unit(s)`, including
`tests/semantic/transition_materialization_lazy_test.cpp`, with zero
findings reported.

Raw command transcripts were produced in-session and are not separately
archived outside this report and the CI-equivalent local run; the commands
above are exactly reproducible against experiment commit
`2539f7f741f31f53ebcb7314dc9f9ac989a6ab16`.

## First discriminating failure

**None occurred.** No stop condition under the First Failure Preservation
Rule was reached during controlled execution. The controlled subject built,
satisfied every frozen acceptance check, and ran against the unmodified
consumer without any compile or runtime failure.

## Deviations from the frozen procedure

- None to the frozen contracts, consumer, or oracle.
- Operationally: the initial pre-implementation verification pass found that
  the frozen commits and `research/arc-001-calibration` branch were not yet
  present on the `mc-lab-research/mc-lab-core` remote, and that
  `docs/research/state.md` / `docs/research/cases/ARC-001.md` on `master`
  still recorded `freeze: NOT-FROZEN` with all research roles unassigned.
  Execution was paused and reported rather than proceeding. The branch and
  freeze-record commit were pushed afterward; re-verification confirmed the
  freeze, role assignments, and disclosed AI Challenge Advisor exception
  before implementation began. This is a procedural/environmental note, not
  a deviation from any frozen technical contract.
- An unrelated tooling error occurred once during implementation: a stray
  `git checkout <frozen-boundary-sha> -- .` was run against the wrong
  branch (`codex/semtl-semantic-architecture`, not the experiment branch),
  which staged unrelated file changes into that branch's working tree. This
  was caught immediately (that branch's `HEAD` matched `origin` exactly, so
  no work existed to lose) and reverted with `git reset --hard HEAD` before
  any further action. It never touched the experiment branch or any frozen
  file and produced no commit.

## Threats to validity

Threats already identified in `ARC-001.md` remain in force unchanged (single
formalism, oracle-copy vs. model-materialization distinction, custom-view
apparatus complexity, small-model scale, no persistent-trace test, held-
constant member-backed dispatch). Additionally for this run:

- The lazy subject's accessor signatures (`const lazy_transition_witness&`)
  were a specific, permitted design choice among several the ARC's
  "model-local member implementations" allowance would have accepted (e.g.,
  an rvalue-only signature was also possible and would very likely have
  triggered the prospectively classified named-lvalue discriminator
  instead). The result reported here is therefore evidence about *a*
  natural, non-adversarial ephemeral-proxy design that satisfies the frozen
  adversarial acceptance checks, not a proof that no compliant lazy design
  could trigger H-B. A credible alternative interpretation is that the
  const-reference accessor choice, not the single-pass/ephemeral property
  per se, is what avoided the discriminating failure.
- One environment only (Windows, clang-cl 22.1.8, MSVC STL) was exercised.
  `windows-clangcl-tidy` and `windows-clangcl-commit-gate` were both run,
  but no cross-compiler or cross-platform corroboration was obtained.

## Evidence disposition and posture

- Evidence disposition: **UNTESTED → this report's contents are proposed
  Controlled evidence**, pending Human ARC Owner assessment. Claude has not
  approved this evidence; only Sami Lazreg can do so.
- Evidence posture achieved: **Controlled** (own single-pass demonstration
  plus unmodified-consumer compile/run), subject to the threats above and to
  the disclosed R2 AI Challenge Advisor exception's Claim-promotion ceiling.
- The disclosed AI Challenge Advisor exception means no independent human
  Challenger reviewed this result. **This report and its AI-advised
  interpretation are not a substitute for human-independent review** and,
  per `ARC-001.md` and `state.md`, this case alone cannot support a
  foundational or strategic representation-independence Claim even if its
  result supports H-A.

## Observation → Interpretation → Alternative explanation → Challenge → Bounded Claim impact → Human decision

### Observation

- `CON-SUCCESSORS-01` remained byte-for-byte source-unchanged.
- `SUBJECT-LAZY-LTS-01` satisfied every frozen adversarial acceptance check
  (input_range; not forward/sized/borrowed; shared single-pass cursor state;
  ephemeral, addressless witness; label and target both readable from the
  same current witness before increment).
- Both `SUBJECT-EXPLICIT-LTS-01` and `SUBJECT-LAZY-LTS-01` satisfied
  `TransitionRelation` and `TransitionLabelling` for their actual witness
  forms.
- Normalized labelled observations from the lazy subject agreed with the
  frozen oracle for sources `0`, `1`, and `2`, with no omission or
  duplication.
- No persistent transition collection, hidden cache, or representation-
  specific branch was introduced in the consumer, concept, or CPO layer.
- The full commit-gate and clang-tidy workflows passed with zero findings.

### Interpretation (AI-advised, pending human review)

Within this run's bounds, the observation supports **H-A**: a transition
relation can expose local transition witnesses to `CON-SUCCESSORS-01`
without requiring stored transition objects, and the existing minimal
`input_range` contract did not need to change, strengthen, or grow
representation-specific branching to accommodate a genuinely single-pass,
ephemeral-proxy generated model.

### Credible alternative interpretation

The absence of a discriminating failure may reflect the specific accessor
signature chosen (`const&`) rather than an architectural property of the
contract itself; a differently-but-still-compliantly designed ephemeral
proxy (e.g., one exposing only rvalue-qualified accessors, which is at least
as natural a way to signal "this witness is not meant to be re-read") might
still trigger the prospectively classified named-lvalue discriminator. This
run does not rule that out, and does not itself decide whether such a design
would count as a "genuinely single-pass" model within the ARC's bounds
without further review.

### Proportionate challenge

The disclosed AI Challenge Advisor review already recorded in
`docs/research/reports/ARC-001-pre-freeze-calibration.md` covered the
prospective named-lvalue classification and the adversarial acceptance
checks before freeze. No new AI challenge pass was run against this specific
result in this report; the "credible alternative interpretation" above is
offered in its place as the required proportionate challenge for this
report, but it is not independent human review.

### Bounded Claim impact

If accepted by the Human ARC Owner, this result is evidence toward, but does
not by itself establish, the bounded wording proposed in `ARC-001.md`:

> Within the tested finite labelled-transition domain, the local
> successor-enumeration consumer operated unchanged across explicit stored
> and single-pass lazy transition representations without forcing
> transition materialization.

It does not authorize: "SemTL is representation-independent," and per the
disclosed R2 AI Challenge Advisor exception, this case alone cannot promote
a foundational or strategic representation-independence Claim regardless of
outcome.

### Human decision

**Pending.** Required from Sami Lazreg as Human ARC Owner / Human Experiment
Owner:

- Accept, request revision of, or reject this evidence and its H-A
  interpretation.
- Decide whether the "credible alternative interpretation" above (rvalue-
  only accessor variant) should be run as a follow-up controlled probe
  before the ARC's evidence disposition is finalized.
- Decide any `CL-001` wording update in `docs/research/state.md`.
- Decide whether an ADR is warranted (not indicated by this result alone,
  per `ARC-001.md`'s ADR-required condition).

Claude does not approve this evidence, does not close ARC-001, and does not
update `CL-001`.
