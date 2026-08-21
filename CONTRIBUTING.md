# Contributing to mscharged-decomp

Thanks for your interest in contributing. This is a community-driven matching
decompilation of *Mario Strikers Charged* for Nintendo Wii. The goal is to
reconstruct readable source code that compiles to byte-for-byte identical game
code.

- Discord: [join the server](https://discord.gg/hKx3FJJgrV) to ask questions,
  coordinate work, or get unstuck.
- Issues and pull requests: use the repository's GitHub project once it is
  published.

## Table of contents

- [Where the project stands](#where-the-project-stands)
- [Before you start](#before-you-start)
- [Development setup](#development-setup)
- [How to contribute](#how-to-contribute)
- [Pull request guidelines](#pull-request-guidelines)
- [Code style](#code-style)
- [Working on matches](#working-on-matches)
- [Header and module organization](#header-and-module-organization)
- [Helpful tooling](#helpful-tooling)
- [Code of conduct](#code-of-conduct)

## Where the project stands

The project is at its initial reconstruction stage. The `R4QE01` revision has
been analyzed, its generated symbols and splits are checked in, and the
baseline build reproduces the original DOL. Most of the useful reverse-
engineering work is still ahead:

- Identify translation-unit and library boundaries.
- Replace generated split objects with matching C and C++ source.
- Correct symbol names, function signatures, types, and data structures.
- Document compiler, ABI, and engine behavior as it is discovered.

Changes that establish reliable structure for later matching work are just as
valuable as completed function matches.

## Before you start

- Read the [README](README.md) for the supported version and build workflow.
- Produce a clean local build before submitting a pull request.
- Coordinate non-trivial or overlapping work on Discord when practical.
- Never commit disc images, game assets, executable files, extracted DOL data,
  copied assembly, or other copyrighted Nintendo or Next Level Games material.
  In particular, keep `orig/` and `build/` artifacts out of commits.

## Development setup

At minimum, you need:

- Python 3
- Ninja
- A legally obtained `R4QE01` revision 1 disc image in `orig/R4QE01/`

From the repository root, run:

```sh
python3 configure.py
ninja
```

The build downloads its pinned tools when needed. After configuration,
`objdiff.json` is generated at the repository root; see
[Diffing](README.md#diffing) for its use.

## How to contribute

Useful contributions include:

- Reconstructing functions and data with verified matches.
- Improving symbols, splits, object boundaries, and library identification.
- Correcting structures, types, function signatures, and class layouts.
- Establishing headers and module organization as source files are added.
- Recording technical discoveries in [FunFacts.md](FunFacts.md).
- Improving repository tooling and documentation.
- Filing reproducible issues for build or configuration problems.

If you are new to decompilation, ask on Discord for a suitable starting point.

## Pull request guidelines

Keep pull requests small, focused, and reviewable:

- Submit one logical change per pull request.
- Use a title that identifies the affected module or symbol.
- Explain what changed, why it is correct, and how you verified it.
- Include relevant objdiff results or screenshots when useful.
- Make sure `ninja` completes successfully.
- State clearly when work is partial or non-matching.
- Avoid unrelated formatting, renaming, or cleanup.
- Call out changes to shared headers, structure layouts, or build configuration.

## Code style

- Run [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) on changed C
  and C++ files. The repository's `.clang-format` is the source of truth.
- Follow conventions in the surrounding code for naming, include order, and
  file layout.
- Do not refactor working code for personal style: source shape affects matching.
- Use comments to explain non-obvious compiler, ABI, or scheduling behavior,
  rather than narrating the code.
- Keep generated configuration edits deliberate and reviewable.

## Working on matches

- Use `objdiff` throughout the edit-build-diff cycle.
- Verify code and data matches rather than relying only on a successful link.
- Check types, inlines, and surrounding declarations when register allocation or
  instruction scheduling is unexpectedly different.
- Treat suspiciously easy matches carefully and compare their behavior and
  disassembly.
- Partial matches are welcome when they capture real structure; identify them
  clearly in the pull request and source when appropriate.

## Header and module organization

The source tree will evolve as translation units and ownership become clearer.
Changes that place declarations in the correct module, eliminate incorrect
dependencies, or establish accurate visibility are welcome. Keep structural
reorganization separate from unrelated matching changes whenever possible.

## Helpful tooling

- [`objdiff`](https://github.com/encounter/objdiff) is the primary tool for
  comparing reconstructed objects with the target; see
  [Diffing](README.md#diffing).
- [`decomp.me`](https://decomp.me) is useful for iterating on an individual
  function in isolation.
- [`decomp-toolkit`](https://github.com/encounter/decomp-toolkit) provides the
  project analysis, splitting, and verification pipeline.

## Code of conduct

Be kind, patient, and assume good faith. This is a hobby project maintained in
contributors' spare time. Keep discussions technical and respectful, both in
pull requests and on Discord.

Thanks for contributing; every reliable discovery moves the project forward.
