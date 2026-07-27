# Formatting policy

`cmake/modules/formatting.cmake` defines the repository-level clang-format
workflow for mcLab. Formatting is exposed through explicit build targets and is
never attached to normal C++ compilation.

This separation keeps ordinary builds available when Git or clang-format is
absent, while giving local commit gates and future CI workflows one
deterministic formatting contract.

This document describes the formatting workflow currently implemented in the
repository. The presence of CI-oriented targets does not imply that a remote CI
check is already active.

## Naming scope

The `MC_LAB_CORE_*` cache variables and the current `tools` formatting root
reflect the build implementation committed today. They should be renamed in
this document only when the corresponding CMake variables and repository paths
are migrated in the same change.

## Targets and configuration

| Interface | Role |
| --- | --- |
| `format-check` | Checks tracked and non-ignored untracked first-party C/C++ files without modifying them |
| `format-staged` | Formats staged first-party C/C++ files and stages the result |
| `MC_LAB_CORE_CLANG_FORMAT` | Advanced cache path to the clang-format executable |
| `MC_LAB_CORE_CLANG_FORMAT_CONFIG` | Advanced cache path to the `.clang-format` policy |
| `MC_LAB_CORE_GIT` | Advanced cache path to Git |
| `MC_LAB_CORE_FORMAT_PATHS` | Advanced list of repository-relative first-party roots |

The default formatting roots are:

```text
include
src
tests
tools
```

Missing roots are harmless. This allows the policy to cover future public
headers without requiring placeholder directories.

The formatting targets are created only when mcLab is the top-level CMake
project. When mcLab is consumed through `add_subdirectory()`, the parent project
retains ownership of repository-wide quality targets and names.

## Source ownership

Git is the source of truth for formatting inputs. This avoids recursive
filesystem globbing over build trees, downloaded dependencies, IDE metadata, or
other unowned files.

`format-check` asks Git for every tracked file and every non-ignored untracked
file below the configured roots. This ensures a newly created source cannot
escape the local commit gate merely because it has not yet been staged.

`format-staged` is narrower. It asks only for files added, copied, modified, or
renamed in the index. Deleted files contain no content to format and are
ignored.

The following case-insensitive extensions are supported:

```text
.c .cc .cpp .cxx .c++
.h .hh .hpp .hxx .h++
.inc .inl .ipp .tpp
.ixx .cppm
```

Ignored files remain outside `format-check`; Git ownership rules continue to
exclude build outputs and generated dependencies. A non-ignored new C/C++ file
is checked immediately and becomes eligible for `format-staged` after it is
added to the index.

Repository file names participating in this workflow must not contain control
characters or semicolons. Spaces and Unicode names are supported. This
restriction gives CMake, Git, Ninja, Visual Studio, and shell-independent
scripts one portable path contract.

## `format-check`

For each discovered first-party file, the runner invokes:

```text
clang-format
  --style=file:<repository .clang-format>
  --fallback-style=none
  --Werror
  --dry-run
  <source>
```

The target checks every file even after an earlier violation, then emits an
aggregate failure summary. It never writes to the source tree or Git index.

An empty full-repository input is treated as an error. This prevents a
misconfigured path list from producing a false-green commit gate.

`format-check` evaluates working-tree files. Locally, the commit gate therefore
checks the same source content that the following build and tests consume.

## `format-staged`

`format-staged` is an explicit mutating convenience target:

1. Discover staged first-party C/C++ files.
2. Validate `.clang-format`.
3. Check every candidate for additional unstaged modifications.
4. Format each safe working-tree file in place.
5. Stage the formatted result with Git.

If a candidate has both staged and unstaged changes, the operation stops before
modifying any source:

```text
format-staged did not modify any file because these staged files also
contain unstaged changes:
  src/example.cpp
```

This rule prevents two destructive outcomes:

- overwriting a developer's unstaged work; and
- silently including previously unstaged changes in the next commit.

The developer can stage the complete file, temporarily shelve the unstaged
portion, or format the file manually before retrying.

If no staged C/C++ file exists, `format-staged` succeeds as a no-op.

## Policy ownership

The repository `.clang-format` file is the single source of truth. The CMake
module and runner do not reproduce style options. They select the exact policy
file and disable fallback styles.

The committed policy targets clang-format 22.1, C++20, four-space indentation,
a 100-column limit, and LF line endings. Public includes below
`mc_lab_core/` form the first include category, ahead of local quoted headers
and standard-library or external headers.

The same policy can be consumed by:

- the `format-check` and `format-staged` targets;
- IDE integrations;
- direct clang-format commands; and
- future CI workflows.

A formatting policy change should be reviewed as a dedicated tooling change.
It should explain the intended rule, identify the selected clang-format
version, and format all existing first-party files in the same increment.

When CI formatting checks are introduced, they must pin the clang-format family
selected by the repository policy. Ordinary platform builds remain independent
of that tool version.

## Tool discovery and failure behavior

Configuration searches for `clang-format` beside the selected C++ compiler and
then on `PATH`. The versioned executable name `clang-format-22` is also
recognized. Git is discovered separately.

Tool absence never blocks configuration or an ordinary build. Invoking a
formatting target performs strict validation and fails with an actionable
message when:

- clang-format is unavailable;
- Git is unavailable;
- `.clang-format` is missing or invalid;
- the source directory is not inside a Git worktree;
- repository discovery fails; or
- a formatting invocation returns a non-zero status.

The path to each tool or policy can be overridden in
`CMakeUserPresets.json` without changing the shared repository presets.

## Preset integration

Formatting does not create a dedicated configure tree. Hidden build-target
mixins bind the targets to an existing concrete platform build.

The implemented local commit-gate sequence is:

```text
configure concrete Release platform
  -> format-check
  -> Release build with warnings-as-errors
  -> tests
```

For example:

```sh
cmake --workflow --preset linux-clang-commit-gate
```

To format already staged files after configuring a concrete platform:

```sh
cmake --build --preset linux-clang-debug --target format-staged
```

The presets also expose reusable hidden target mixins for composing additional
local or CI workflows without duplicating target names.

## Local and future CI responsibilities

Project developers can use `format-staged` for remediation and a platform
commit gate for feedback.

When branch protection and CI are introduced, the required formatting job
should execute `format-check`. This prevents the shared policy from depending
on an optional local Git hook.

Formatting is intended to remain platform-independent. A violation should have
the same result on Windows, Linux, and macOS when local and CI environments use
the pinned clang-format family.
