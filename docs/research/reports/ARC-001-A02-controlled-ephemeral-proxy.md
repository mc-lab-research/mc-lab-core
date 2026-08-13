# ARC-001 A02 controlled experiment - ephemeral-proxy witnesses

**Status:** Accepted Controlled evidence supporting H-A within the frozen
ARC-001 scope. Sami Lazreg approved the evidence on 2026-08-13 as Human ARC
Owner, Human Experiment Owner, and Project Principal after Codex completed the
design/apparatus review and independent validation. This report does not close
[ARC-001](../cases/ARC-001.md), authorize a foundational representation-
independence Claim, require an ADR, or authorize production promotion. A
separate subsequent Project Principal action authorized this bounded result as
the revised `CL-001` wording in [state.md](../state.md).

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
- **A02 comment-only revision:** `fe44470b4acbcd69ef4731d8d346d55b8e935c4f`
  (no behavior change; this is the commit Codex independently reproduced the
  commit-gate and clang-tidy workflows against)
- **A02 final evidence-tip commit:** `40c50553c1d64a6ed4e74faa951843966d609ca1`
  (authorized narrow evidence-completion revision - see "Evidence-completion
  revision" below)
- **Consumer:** `CON-SUCCESSORS-01`,
  [`CON-SUCCESSORS-01 maintained copy`](../../../tests/research/arc-001/common/support/labelled_successor_consumer.hpp)
- **Oracle:**
  [`frozen oracle maintained copy`](../../../tests/research/arc-001/common/transition_materialization_calibration_test.cpp)
- **Controlled subject (A02):** `SUBJECT-LAZY-LTS-01`, ephemeral-proxy form,
  [`A02 model maintained copy`](../../../tests/research/arc-001/a02-ephemeral-proxy/support/transition_materialization_lazy_model.hpp)
- **Controlled test:**
  [`A02 test maintained copy`](../../../tests/research/arc-001/a02-ephemeral-proxy/transition_materialization_lazy_test.cpp)
  (maintained registration `mc_lab_core.research.arc_001.a02_ephemeral_proxy`,
  labels `research;arc-001;a02;controlled;supported`, in
  [`tests/research/arc-001/CMakeLists.txt`](../../../tests/research/arc-001/CMakeLists.txt))

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
[`transition_materialization_lazy_model.hpp`](../../../tests/research/arc-001/a02-ephemeral-proxy/support/transition_materialization_lazy_model.hpp)

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
[`transition_materialization_lazy_test.cpp`](../../../tests/research/arc-001/a02-ephemeral-proxy/transition_materialization_lazy_test.cpp):

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

All compiled successfully at the fix commit. These `static_assert`s establish
two different things, and it is worth keeping them distinct:

- The first six only check *declarations and return types* -
  `outgoing_transition_range_t`, `transition_reference_for_t`, and the
  `TransitionRelation` / `TransitionLabelling` concepts are all evaluated
  from function signatures and type traits. None of them requires the
  compiler to actually generate code for `CON-SUCCESSORS-01`'s body.
- The seventh (`decltype(research::observe_labelled_successors(...))`)
  forces genuine **instantiation** of `observe_labelled_successors`'s
  template body against the proxy type - i.e. the compiler must actually
  compile the frozen consumer's `for (auto&& witness : ...) { ... }` loop,
  including its calls to `transition_label`/`target` on the named lvalue
  `witness`, to determine the `decltype`. If the named-lvalue discriminator
  described in ARC-001.md were going to fire, it would fire here, at
  compile time, not merely fail one of the declaration-level checks above.
- The *runtime* calls in `main()` and in
  `proxy_becomes_stale_after_shared_advancement()` are a second, independent
  instantiation and execution of that same consumer body (and of the proxy's
  `target`/`transition_label` accessors) against live objects - they are not
  redundant with the compile-time check, since a body can compile
  successfully yet still misbehave at runtime (for example, by reading
  through a dangling or stale handle). Both layers passing is what the
  "Second observation" section below reports.

## First discriminating failure (preserved)

**Occurred at commit `d64232f9f6b7ca1e8d679bf2a188d7fa28580084`, before any
repair.**

Command: `cmake --workflow --preset windows-clangcl-commit-gate --fresh`

Diagnostic (verbatim):

```text
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
  current proxy; at the fix commit (`ff4963f`) this was checked only via
  `is_current()`. See "Evidence-completion revision" below for the
  authorized follow-up that adds a semantic read.
- Neither this test nor the unmodified consumer calls `target` or
  `transition_label` on `original_proxy` after it goes stale; the private
  `read()` accessor's `assert(is_current())` was never triggered during this
  run.

## Codex independent review

Codex independently reviewed the proxy implementation, ancestry, frozen-file
integrity, first-failure preservation, cache boundary, and named-lvalue
behavior, and reported all as passing. Codex independently reproduced both
required workflows at remote tip `fe44470b4acbcd69ef4731d8d346d55b8e935c4f`
(the comment-only revision - see Identifiers). This is a second,
independent execution of the same commands recorded in "Commands and raw
results" below, not merely a re-read of this report.

## Evidence-completion revision

Sami Lazreg authorized one narrow evidence-completion revision after
Codex's review, on the existing append-only branch (no rewrite of prior
history):

- **Change:** in `proxy_becomes_stale_after_shared_advancement()`,
  `after_first` and `after_second` (the two proxies obtained by
  dereferencing `iterator` and `iterator_copy` after `++iterator_copy`) are
  now read semantically through `sem::transition_label` / `sem::target`, not
  only checked via `is_current()`. Both are required to report
  `(bypass, 2)`. `original_proxy` is still never read semantically after it
  becomes stale.
- **Rationale:** `is_current()` alone does not distinguish a correct
  implementation from one that reports a matching generation but happens to
  serve stale or incorrect data through `read()`; an actual semantic read
  through both handles is what demonstrates the shared cursor moved to the
  right position, observably, through either handle.
- **Commit sequence:** the test change was committed
  (`40c50553c1d64a6ed4e74faa951843966d609ca1`) *before* either workflow was
  run, per instruction. No frozen file, and no other part of the lazy
  model, was touched (`git diff` against both the frozen-file set and
  `transition_materialization_lazy_model.hpp` is empty for this revision).
- **Result:** both workflows passed cleanly at `40c5055` - no failure
  occurred, so no First Failure Preservation stop applied to this revision.
  See "Commands and raw results" below for the exact re-run.

### Storage and cache inspection

Manual source inspection of
[`transition_materialization_lazy_model.hpp`](../../../tests/research/arc-001/a02-ephemeral-proxy/support/transition_materialization_lazy_model.hpp):

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

```powershell
cmake --workflow --preset windows-clangcl-commit-gate --fresh
```

At `d64232f`: formatting check passed (33 files); build failed with the
private-constructor diagnostic above; workflow stopped.

At `ff4963f`: formatting check passed (33 files); build succeeded with
warnings-as-errors on; all 13 registered tests passed, including
`mc_lab_core.semantic.transition_materialization_calibration` (frozen,
unaffected) and `mc_lab_core.semantic.transition_materialization_lazy`
(A02, controlled).

At `fe44470` (comment-only revision): identical result to `ff4963f` -
formatting check passed, build succeeded, 13/13 tests passed. This is also
the commit Codex independently reproduced this workflow against.

At `40c5055` (evidence-completion revision, run `--fresh` after committing
the test change): formatting check passed (33 files); build succeeded with
warnings-as-errors on; all 13 registered tests passed, including
`mc_lab_core.semantic.transition_materialization_lazy` with the new
semantic reads of `after_first`/`after_second` exercised and passing.

```powershell
cmake --workflow --preset windows-clangcl-tidy --fresh
```

At `ff4963f`, `fe44470`, and `40c5055`: `clang-tidy passed for
7 translation unit(s)` each time, including
[`A02 maintained test`](../../../tests/research/arc-001/a02-ephemeral-proxy/transition_materialization_lazy_test.cpp),
zero findings.

## Deviations from the frozen procedure

- None to the frozen contracts, consumer, or oracle, at any point.
- One first-failure cycle occurred exactly as the protocol anticipates:
  compile failure at `d64232f` → stop, preserve, push, report → explicit
  human authorization ("fix the code") → single-line fix at `ff4963f` →
  re-run from a clean build (`--fresh`) → pass. No source was edited between
  the failure and its being preserved and reported.
- A second, narrower cycle followed Codex's independent review: an
  authorized evidence-completion revision (`40c5055`) was committed before
  either workflow was re-run, both workflows passed with `--fresh`, and no
  failure occurred - so no First Failure Preservation stop applied to this
  revision.

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

- Evidence disposition: **SUPPORTED** within the frozen ARC-001 scope.
- Evidence posture achieved: **Controlled**, accepted by Sami Lazreg on
  2026-08-13 after Codex's review and independent gate reproduction.
- Claude supplied implementation and experiment support but did not approve or
  adjudicate the evidence.
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
  at the fix commit, at the comment-only revision (independently reproduced
  by Codex), and at the evidence-completion revision `40c5055`.
- After the authorized evidence-completion revision, both fresh proxies
  obtained post-advancement (`after_first` and `after_second`) were read
  semantically and both agreed with the oracle (`bypass`, `2`), not merely
  checked via `is_current()`.

### Interpretation (accepted by the Human ARC Owner)

Within this run's bounds, the observation supports **H-A**: a transition
relation can expose local transition witnesses through a genuinely
ephemeral, non-owning proxy - carrying no copied semantic data - to
`CON-SUCCESSORS-01` without requiring stored transition objects, stable
addresses, or a consumer/concept/CPO change.

### Credible alternative interpretation

As in A01: the absence of a named-lvalue discriminating failure may reflect
the specific `const&` accessor signature chosen rather than an
architectural property of the contract itself.

The `d64232f` friend-declaration failure is not treated as evidence, general
or otherwise, of ephemeral-proxy architectural friction. It was a
class-member-declaration-order name-lookup outcome specific to this
apparatus's particular arrangement of nested types, independently confirmed
by Codex's review; it says nothing about whether ephemeral-proxy designs are
harder to write correctly in general, and this report does not draw that
inference from a single instance.

### Proportionate challenge

Codex, the designer and reviewer of this configuration, independently reviewed
the proxy implementation, ancestry, frozen-file integrity, first-failure
preservation, cache boundary, named-lvalue behavior, and the evidence-
completion revision at `40c5055` (all reported as passing). Codex independently
reproduced both required workflows before and after that completion. This
report's own "credible alternative interpretation" above is an additional,
narrower check; per the disclosed exception, none of this - Codex's review
included - is independent *human* review.

### Bounded Claim impact

This accepted result supports the canonical bounded `CL-001` wording in
[state.md](../state.md):

> Within the ARC-001 finite labelled-transition domain,
> `CON-SUCCESSORS-01` operated unchanged across stable stored transition
> witnesses and a single-pass generated ephemeral-proxy representation,
> without requiring a persistent transition collection or
> stronger-than-`input_range` traversal.

It does not authorize "SemTL is representation-independent," and cannot by
itself promote a foundational or strategic Claim per the disclosed
exception.

### Human decision

On 2026-08-13, Sami Lazreg, acting as Human ARC Owner, Human Experiment Owner,
and Project Principal:

- accepted A02 as **Controlled evidence supporting H-A within the frozen
  ARC-001 scope**;
- accepted Codex's final review of ancestry, preservation, frozen-file
  equality, proxy structure, invalidation, storage boundaries, classification,
  and independently reproduced validation at the final executable evidence;
- retained A01 as INCONCLUSIVE for the controlled result and Exploratory for
  its generated by-value observation;
- made no Claims Ledger change and retained the disclosed AI Challenge Advisor
  Claim-promotion ceiling;
- determined that acceptance alone requires no ADR because no governing public
  architecture changed;
- did not close ARC-001 or authorize experimental-to-production promotion.

Claude did not approve or adjudicate this evidence.

### Subsequent Project Principal Claim decision

The human evidence-acceptance step above made no Claims Ledger change. In a
separate subsequent action on 2026-08-13, Sami Lazreg, acting as Project
Principal, authorized the bounded wording quoted above as the revised canonical
`CL-001` Claim. Its Controlled evidence is this report, human-acceptance commit
`fa29517dbf9ddc138395f329cfefc0ba9bb167f8`, and annotated result tag
`arc-001-a02-supports-ha`.

This authorization does not approve `CL-003`, a foundational or strategic
representation-independence Claim, an ADR, ARC closure, or production
promotion.

## Experimentator completion note

As Implementation and Controlled-Experiment Support (not Human ARC Owner,
Human Experiment Owner, approver, adjudicator, or Independent Human
Challenger), Claude considers its assigned A02 implementation and
controlled-execution work finished as of this commit:

- `SUBJECT-LAZY-LTS-01` was implemented per the frozen A02 configuration,
  the first discriminating failure was preserved before any repair, the
  authorized fix and the authorized evidence-completion revision were both
  committed before their respective workflow runs, and both required
  workflows passed at every revision that was run to completion.
- Extensive comments were added to every file Claude authored on this
  branch (the lazy model and its test) and, separately, to the A01 branch's
  equivalent files; no frozen file was modified on either branch.
- All commits described in this report are pushed to
  `experiment/arc-001/a02-ephemeral-proxy` on `origin`, append-only, with no
  rewrite, squash, force-push, or deletion of any frozen evidence history or
  tag.

This note does not close ARC-001, does not adjudicate between H-A and H-B,
does not approve or finalize the evidence, and does not authorize any
architectural, Claim, or ADR decision - those remain solely for Sami Lazreg
(and, per the process already exercised above, Codex's independent review)
to determine.
