# ARC-001 A02 controlled experiment - ephemeral-proxy witnesses

**Status:** Draft. Interpretation is AI-advised (Claude, Implementation and
Experiment Support). Human assessment, Claim-impact approval, and any
architectural decision are pending Human ARC Owner / Human Experiment Owner
Sami Lazreg and Project Principal Sami Lazreg, and design/apparatus review by
Codex. This report does not close [ARC-001](../cases/ARC-001.md), does not
approve `CL-001` in [state.md](../state.md), and does not authorize an
architectural decision.

## Identifiers

- **Technical freeze commit (immutable):** `f969fa8aa63a9a172e6feb2eeb32636aab24f387`
- **ARC-001 freeze-record commit:** `1dec900c899893537c9f1d87b3b26e272d22808f`
- **A02 configuration commit (immutable):** `2460f4d45f21fce49636d820c07c489d1b4598b6`
  (tag `arc-001-a02-config-v1`)
- **A02 freeze-record / starting commit:** `07bde5bb89e8ab768a4170b34cc9b02632b85413`
- **A02 configuration document:**
  [`ARC-001-A02-ephemeral-proxy-freeze.md`](ARC-001-A02-ephemeral-proxy-freeze.md)
- **Experiment branch:** `experiment/arc-001/a02-ephemeral-proxy`
- **A02 pre-observation commit:** `d64232f9f6b7ca1e8d679bf2a188d7fa28580084`
  (first compile-time failure occurred here; preserved and pushed before any
  repair)
- **A02 fix commit:** `ff4963f9d3430a4d9d12a79650109e7c954c38bf`
  (authorized by Sami Lazreg after the failure was preserved; passing
  revision)
- **Consumer:** `CON-SUCCESSORS-01`,
  [`tests/semantic/support/labelled_successor_consumer.hpp`](../../../tests/semantic/support/labelled_successor_consumer.hpp)
- **Oracle:**
  [`tests/semantic/transition_materialization_calibration_test.cpp`](../../../tests/semantic/transition_materialization_calibration_test.cpp)
- **Controlled subject (A02):** `SUBJECT-LAZY-LTS-01`, ephemeral-proxy form,
  [`tests/semantic/support/transition_materialization_lazy_model.hpp`](../../../tests/semantic/support/transition_materialization_lazy_model.hpp)
- **Controlled test:**
  [`tests/semantic/transition_materialization_lazy_test.cpp`](../../../tests/semantic/transition_materialization_lazy_test.cpp)
  (registered as `mc_lab_core.semantic.transition_materialization_lazy`,
  labels `semantic;research;arc-001;experiment;a02`, in
  [`tests/semantic/CMakeLists.txt`](../../../tests/semantic/CMakeLists.txt))

Links above are repository-relative, in the same convention as
[ARC-001.md](../cases/ARC-001.md); they resolve against whatever ref you are
viewing this file on.

## Pre-implementation verification

Performed before writing any A02 code, per the configuration document:

1. **Starting branch/SHA:** `experiment/arc-001/a02-ephemeral-proxy` at
   `07bde5bb89e8ab768a4170b34cc9b02632b85413` - matches the branch tip on
   `origin` exactly.
2. **A02 configuration SHA/tag:** annotated tag `arc-001-a02-config-v1`
   dereferences to `2460f4d45f21fce49636d820c07c489d1b4598b6` - matches.
3. **Frozen technical files unchanged from `f969fa8...`:**
   `git diff f969fa8aa63a9a172e6feb2eeb32636aab24f387 origin/experiment/arc-001/a02-ephemeral-proxy --
   <consumer, oracle, stored subject, all frozen contract headers>` -
   empty.
4. **A01 non-ancestry:** `git merge-base --is-ancestor 2539f7f741f31f53ebcb7314dc9f9ac989a6ab16 origin/experiment/arc-001/a02-ephemeral-proxy`
   - exit 1 (not an ancestor), confirming A02 was not implemented from or
   merged with the A01 branch.

Ancestry of the branch: `16da275` (Lean Research OS) → `f969fa8` (technical
freeze) → `1dec900` (ARC-001 freeze record) → `2460f4d` (A02 configuration,
immutable) → `07bde5b` (A02 configuration freeze record, branch tip at
start).

## SUBJECT-LAZY-LTS-01 (A02) design

Source:
[`transition_materialization_lazy_model.hpp`](../../../tests/semantic/support/transition_materialization_lazy_model.hpp)

This differs materially from the A01 attempt (preserved, unmodified, on
`experiment/arc-001-generated-ephemeral`), which returned a plain
by-value witness carrying copied `label`/`target` fields - found
nonconforming to the ephemeral-proxy requirement.

- `lazy_outgoing_transitions` owns exactly: the queried `source_`, a cursor
  `step_`, current-witness storage for one occurrence (`current_`, an
  `std::optional<lazy_current_witness>`), and a generation counter
  (`generation_`, `std::uint64_t`). It never owns or caches the complete
  local image.
- `lazy_outgoing_transitions::proxy` contains only a non-owning
  `const lazy_outgoing_transitions*` handle and the `generation_` observed
  at dereference. **No label or target field is copied into the proxy.**
- `proxy::is_current()` is the public, test-visible validity check: it
  compares the proxy's stored generation against the range's current
  generation.
- `proxy`'s private `read()` accessor - reachable only from `iterator` and
  `lazy_reference_system` via friendship - returns a reference to the
  range's current-witness storage, guarded by
  `assert(is_current() && ...)`. Any attempt to read a stale proxy through
  the sanctioned channel fails a debug assertion rather than silently
  returning the wrong (or right-by-coincidence) data.
- `advance()` (called from `iterator::operator++()`) increments the cursor,
  **clears** `current_` (invalidating in-flight proxies before recomputing
  anything), increments `generation_`, then recomputes `current_` for the
  new position. Because `iterator` holds a pointer back to the shared range
  object, copies of the iterator alias the same cursor/generation state.
- `lazy_reference_system::target` / `transition_label` take
  `const lazy_outgoing_transitions::proxy&` and read only through
  `proxy::read()` - i.e., only current range-owned state, never a copy held
  by the proxy itself.

## Conformance checks (compile-time)

From
[`transition_materialization_lazy_test.cpp`](../../../tests/semantic/transition_materialization_lazy_test.cpp):

```cpp
static_assert(std::ranges::input_range<lazy_range_t>);
static_assert(!std::ranges::forward_range<lazy_range_t>);
static_assert(!std::ranges::sized_range<lazy_range_t>);
static_assert(!std::ranges::borrowed_range<lazy_range_t>);
static_assert(
    std::same_as<witness_t, research::lazy_outgoing_transitions::proxy>);
static_assert(sem::TransitionRelation<system_t, int>);
static_assert(sem::TransitionLabelling<system_t, witness_t>);
static_assert(std::same_as<decltype(research::observe_labelled_successors(
                               std::declval<const system_t&>(),
                               std::declval<const int&>())),
                           std::vector<observation_t>>);
```

All compiled successfully at the fix commit, including the last one, which
instantiates the unmodified `CON-SUCCESSORS-01` body against the proxy form.

## First discriminating failure (preserved)

**Occurred at commit `d64232f9f6b7ca1e8d679bf2a188d7fa28580084`, before any
repair.**

Command: `cmake --workflow --preset windows-clangcl-commit-gate --fresh`

Diagnostic (verbatim):

```
tests\semantic\support/transition_materialization_lazy_model.hpp(114,14): error: calling a private constructor of class 'mc_lab::tests::semantic::research::lazy_outgoing_transitions::proxy'
  114 |       return proxy{range_, range_->generation_};
      |              ^
tests\semantic\support/transition_materialization_lazy_model.hpp(87,28): note: declared private here
   87 |     [[nodiscard]] explicit proxy(const lazy_outgoing_transitions* range,
      |                            ^
1 error generated.
ninja: build stopped: subcommand failed.
```

**Neutral observation (as recorded before repair):** the failure was a
`private constructor` access error inside `iterator::operator*()`.
`proxy`'s constructor was `private` with `friend class iterator;` declared
inside `proxy`'s body, but `proxy` was defined before
`lazy_outgoing_transitions::iterator` in the enclosing class. At that point
in translation, the unqualified `friend class iterator;` did not yet have
`lazy_outgoing_transitions::iterator` in scope to bind to, so friendship did
not reach the actual nested type. This is a class-member-ordering /
name-lookup outcome in the apparatus code. It did not involve the frozen
consumer, oracle, concepts, CPOs, range category, or a named-lvalue /
value-category call - the compiler never reached `CON-SUCCESSORS-01`, the
`TransitionRelation` / `TransitionLabelling` checks, or any range-category
`static_assert`. It was not classified against H-A or H-B at the time, and
no source was touched before this was recorded and pushed.

**Preservation:** commit `d64232f` was committed and pushed to
`experiment/arc-001/a02-ephemeral-proxy` (`07bde5b..d64232f`) before the
build was run, and remains reachable in that branch's history unmodified,
unsquashed, and unforced.

**Repair authorization:** Sami Lazreg explicitly authorized the fix after
the failure was preserved and reported ("fix the code").

**Fix applied (commit `ff4963f9d3430a4d9d12a79650109e7c954c38bf`):** a single
forward declaration, `class iterator;`, added inside
`lazy_outgoing_transitions` before `proxy`, so the `friend class iterator;`
inside `proxy` binds to the correct nested type. No other line changed.

## Second observation (at the fix commit)

### Runtime observations

`observe_labelled_successors` (unmodified `CON-SUCCESSORS-01`) run against
the A02 `SUBJECT-LAZY-LTS-01`:

- source `0`: `{(advance, 1), (bypass, 2)}` - agrees with the oracle.
- source `1`: `{(reset, 0)}` - agrees with the oracle.
- source `2`: `{}` - agrees with the oracle.

### Proxy invalidation demonstration

Implemented exactly per the frozen pseudocode in the A02 configuration
document (`proxy_becomes_stale_after_shared_advancement` in the test file):

- `original_proxy.is_current()` is `true` immediately after dereference.
- `transition_label` and `target` are read from `original_proxy` while
  current, and agree with the oracle (`advance`, `1`).
- After `++iterator_copy` (a copy of the original iterator, advancing the
  range shared by both handles), `original_proxy.is_current()` becomes
  `false`.
- Dereferencing either `iterator` or `iterator_copy` afterward yields a new,
  current proxy observing the shared position's new state (`bypass`, `2`),
  not the sentinel and not the stale value.
- Neither this test nor the unmodified consumer calls `target` or
  `transition_label` on `original_proxy` after it goes stale; the private
  `read()` accessor's `assert(is_current())` was never triggered during this
  run.

### Storage and cache inspection

Manual source inspection of
[`transition_materialization_lazy_model.hpp`](../../../tests/semantic/support/transition_materialization_lazy_model.hpp):

- `proxy` holds exactly two members: `range_` (pointer) and `generation_`
  (`std::uint64_t`). No `label`/`target` fields.
- `lazy_outgoing_transitions` holds `source_`, `step_`, `generation_`, and
  `current_` (storage for exactly one occurrence).
- `compute_lazy_transition` is a pure function over `(source, step)`;
  nothing is retained across calls beyond what `current_` already holds for
  the active position.
- No `std::vector`, `std::array`, `std::span`, or other multi-element
  container of transitions appears anywhere in the header.
- No member or free function returns a stable reference into `current_`
  outside of the guarded, friend-only `proxy::read()`.

No persistent transition collection or hidden complete cache was found.

### Value-category outcome

As in A01, the frozen consumer binds each observation to the named lvalue
`witness`. `lazy_reference_system::target` / `transition_label` take
`const lazy_outgoing_transitions::proxy&`, which accepts both the lvalue
form the consumer uses and the rvalue-forwarded form the
`TargetCallable` / `TransitionLabelCallable` concept checks use. **The
prospectively classified named-lvalue/value-category discriminator was not
triggered.** This remains a design choice within the ARC's permitted
"model-local member implementations," not a change to the consumer, a
concept, or a CPO - see Threats to validity.

## Environment

- OS: Windows NT 10.0.26200.0
- Compiler: `clang-cl`, clang version 22.1.8 (MSVC-compatible driver),
  target `x86_64-pc-windows-msvc`
- Standard library: MSVC STL
- CMake: 4.4.0
- Ninja: 1.13.2
- Generator/preset: Ninja via `windows-clangcl-release` (commit-gate) and
  `windows-clangcl-debug` (tidy), toolchain contract `WINDOWS_CLANGCL`
- Build type: Release (commit-gate), Debug (tidy)
- C++ mode: C++20, extensions off, warnings as errors ON (commit-gate)
- Visual Studio: 18.8.12021.73 (Community)

No portability claim is made beyond this single environment.

## Commands and raw results

```
cmake --workflow --preset windows-clangcl-commit-gate --fresh
```

At `d64232f`: formatting check passed (33 files); build failed with the
private-constructor diagnostic above; workflow stopped.

At `ff4963f`: formatting check passed (33 files); build succeeded with
warnings-as-errors on; all 13 registered tests passed, including
`mc_lab_core.semantic.transition_materialization_calibration` (frozen,
unaffected) and `mc_lab_core.semantic.transition_materialization_lazy`
(A02, controlled).

```
cmake --workflow --preset windows-clangcl-tidy --fresh
```

At `ff4963f`: `clang-tidy passed for 7 translation unit(s)`, including
[`tests/semantic/transition_materialization_lazy_test.cpp`](../../../tests/semantic/transition_materialization_lazy_test.cpp),
zero findings.

## Deviations from the frozen procedure

- None to the frozen contracts, consumer, or oracle, at any point.
- One first-failure cycle occurred exactly as the protocol anticipates:
  compile failure at `d64232f` → stop, preserve, push, report → explicit
  human authorization ("fix the code") → single-line fix at `ff4963f` →
  re-run from a clean build (`--fresh`) → pass. No source was edited between
  the failure and its being preserved and reported.

## Threats to validity

Threats already identified in [ARC-001.md](../cases/ARC-001.md) and in the
A01 report remain in force (single formalism, oracle-copy vs.
model-materialization distinction, custom-view apparatus complexity,
small-model scale, no persistent-trace test, held-constant member-backed
dispatch). Additionally for this run:

- As in A01, the accessor signatures (`const proxy&`) are a specific,
  permitted design choice that accepts both value categories the consumer
  and concepts use. This run is evidence that a genuinely ephemeral,
  no-copied-fields proxy design can also avoid the named-lvalue
  discriminator - it does not establish that no compliant ephemeral-proxy
  design would trigger it.
- The first-failure/fix cycle involved a real compile-time stop, but its
  cause (friend-declaration lookup order) was apparatus-internal and
  unrelated to any ARC-001 hypothesis; Codex's review should independently
  confirm that characterization rather than take it on Claude's word alone.
- One environment only (Windows, clang-cl 22.1.8, MSVC STL) was exercised.

## Evidence disposition and posture

- Evidence disposition: **UNTESTED → this report's contents are proposed
  Controlled evidence**, pending Human ARC Owner assessment and Codex's
  design/apparatus review. Claude has not approved this evidence.
- Evidence posture achieved: **Controlled**, subject to the threats above.
- Per the disclosed R2 AI Challenge Advisor exception recorded in
  [ARC-001.md](../cases/ARC-001.md) and [state.md](../state.md), this case
  alone cannot support a foundational or strategic representation-
  independence Claim even if its result supports H-A.

## Observation → Interpretation → Alternative explanation → Challenge → Bounded Claim impact → Human decision

### Observation

- `CON-SUCCESSORS-01` remained byte-for-byte source-unchanged throughout.
- The A02 proxy contains no copied label or target field - only a
  non-owning handle and a generation token.
- The proxy became stale (per `is_current()`) after either copied iterator
  advanced the shared traversal position, and neither the test nor the
  consumer read a stale proxy's semantic content.
- `SUBJECT-LAZY-LTS-01` (A02 form) satisfied every frozen adversarial
  acceptance check (input_range; not forward/sized/borrowed; proxy as the
  actual `range_reference_t`).
- Normalized labelled observations agreed with the frozen oracle for
  sources `0`, `1`, and `2`, with no omission or duplication.
- One first-failure cycle occurred and was preserved before repair, per
  protocol; the underlying cause was apparatus-internal (friend-declaration
  lookup order), not a change to any frozen contract.
- The full commit-gate and clang-tidy workflows passed with zero findings
  at the fix commit.

### Interpretation (AI-advised, pending human and Codex review)

Within this run's bounds, the observation supports **H-A**: a transition
relation can expose local transition witnesses through a genuinely
ephemeral, non-owning proxy - carrying no copied semantic data - to
`CON-SUCCESSORS-01` without requiring stored transition objects, stable
addresses, or a consumer/concept/CPO change.

### Credible alternative interpretation

As in A01: the absence of a named-lvalue discriminating failure may reflect
the specific `const&` accessor signature chosen rather than an
architectural property of the contract itself. Separately, the fact that a
correct ephemeral-proxy implementation required one non-obvious C++
name-lookup fix (friend visibility across nested-class declaration order)
suggests this pattern carries real implementation friction even when it
does not surface as an ARC-001-relevant failure - a fact worth surfacing to
Codex's apparatus review even though it is not being classified as
discriminating here.

### Proportionate challenge

Codex is the designer and reviewer of this configuration and has not yet
reviewed this result. This report substitutes no AI challenge pass of its
own beyond the "credible alternative interpretation" above; per the
disclosed exception, none of this is independent human review.

### Bounded Claim impact

If accepted, this result is additional evidence toward, but does not by
itself establish, the same bounded wording proposed in
[ARC-001.md](../cases/ARC-001.md):

> Within the tested finite labelled-transition domain, the local
> successor-enumeration consumer operated unchanged across explicit stored
> and single-pass lazy transition representations without forcing
> transition materialization.

It does not authorize "SemTL is representation-independent," and cannot by
itself promote a foundational or strategic Claim per the disclosed
exception.

### Human decision

**Pending.** Required from Sami Lazreg as Human ARC Owner / Human Experiment
Owner, and from Codex as designer/reviewer:

- Codex's review of ancestry, preservation, frozen-file equality, proxy
  structure, invalidation, storage boundaries, independent gate results,
  and classification.
- Accept, request revision of, or reject this evidence and its H-A
  interpretation.
- Decide any `CL-001` wording update in [state.md](../state.md).
- Decide whether an ADR is warranted.
- Decide how A01 (preserved, nonconforming) and A02 (this attempt) are
  jointly represented in ARC-001's evidence record.

Claude does not approve this evidence, does not choose between H-A and H-B,
does not update `CL-001`, does not create an ADR, and does not close
[ARC-001](../cases/ARC-001.md).
