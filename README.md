Mario Strikers Charged  
[![Build Status]][actions] [![Code Progress]][progress] [![Link Progress]][progress] [![Discord Badge]][discord]
=============

<!--
Replace with your repository's URL.
-->
[Build Status]: https://github.com/yannicksuter/mscharged-decomp/actions/workflows/build.yml/badge.svg
[actions]: https://github.com/yannicksuter/mscharged-decomp/actions/workflows/build.yml
<!--
decomp.dev progress badges
See https://decomp.dev/api for an API overview.
-->
[Code Progress]: https://decomp.dev/yannicksuter/mscharged-decomp.svg?mode=shield&measure=code&label=Code
[Link Progress]: https://decomp.dev/yannicksuter/mscharged-decomp.svg?mode=shield&measure=complete_code_percent&label=Linked
[Fuzzy Progress]: https://decomp.dev/yannicksuter/mscharged-decomp.svg?mode=shield&measure=fuzzy_match_percent&label=Fuzzy
[progress]: https://decomp.dev/yannicksuter/mscharged-decomp
<!--
Replace with your Discord server's ID and invite URL.
-->
[Discord Badge]: https://img.shields.io/discord/727908905392275526?color=%237289DA&logo=discord&logoColor=%23FFFFFF
[discord]: https://discord.gg/hKx3FJJgrV

> [!IMPORTANT]
> This repository does **not** provide a new way to play *Super Mario Strikers*. It is not a modern recompilation ("recomp") or a port. Its 100% code and linking status refers to a high-fidelity reconstruction of what the original source code may have looked like—one that compiles into an executable byte-for-byte identical to the original retail GameCube release.
>
> Recompilations and ports may emerge elsewhere from this work—and we look forward to seeing them—but they are outside the scope of this repository. Please do not ask for recompilations or ports in the decompilation Discord, as producing and supporting them is not the focus of this community.

A work-in-progress matching decompilation of *Mario Strikers Charged* for
Nintendo Wii, developed by Next Level Games and published by Nintendo.

This repository does **not** contain game assets or assembly. You must provide
your own legally obtained copy of the game.

## Supported version

- `R4QE01`: USA, disc revision 1
- `main.dol` SHA-1: `e96d2298067b70652752145b4e63644e6e1b1560`

Other regions and revisions are not configured.

Decompilation
=============

Decompilation is the process of reverse-engineering compiled machine code back into human-readable source code. Unlike disassembly, which produces assembly language, decompilation aims to reconstruct high-level code (like C or C++) that closely matches what the original developers wrote. This process involves analyzing the binary executable, understanding its structure and behavior, and translating it back into source code that compiles to produce identical machine code. In this project, the goal is not just a close match, but a **100% match**—the decompiled source code must compile to produce byte-for-byte identical machine code to the original. This is why diffing (see the [Diffing](#diffing) section below) is an essential piece of the process, as it allows us to verify that our decompiled code produces exactly the same binary output as the original game. Decompilation projects like this one enable deeper understanding of game mechanics, facilitate modding and preservation, and serve as valuable learning resources for understanding how games were built.

Progress
========

![progress overview](https://decomp.dev/yannicksuter/mscharged-decomp/R4QE01.svg?mode=overview)

Track the project decompilation progress and explore the interactive graph on [decomp.dev](https://decomp.dev/yannicksuter/mscharged-decomp).


## Dependencies

- Python 3
- Ninja

The build downloads pinned versions of decomp-toolkit, the compiler, binutils,
objdiff-cli, and wibo when needed.

## Building

Place a supported disc image in `orig/R4QE01`. ISO, RVZ, WIA, WBFS, CISO, NFS,
GCZ, and TGC images are supported.

Then configure and build:

```sh
python3 configure.py
ninja
```

On the first build, decomp-toolkit extracts `sys/main.dol` from the disc image
and performs the initial analysis. Once extraction succeeds, the disc image can
be removed from `orig/R4QE01` to save space.

The generated `config/R4QE01/symbols.txt` and `config/R4QE01/splits.txt` are
checked into the repository. A successful build ends with:

```text
build/R4QE01/main.dol: OK
```

To generate a linker map for the reconstructed ELF, configure with `--map`:

```sh
python3 configure.py --map
ninja
```

The result is written to `build/R4QE01/main.elf.MAP`. It describes the
reconstructed link rather than an original development map and is intentionally
ignored by Git.

## Diffing

After configuration, open the project directory in
[objdiff](https://github.com/encounter/objdiff). It reads the generated
`objdiff.json` automatically and rebuilds changed objects with Ninja.

## Contributing

Contributions are welcome. Read [CONTRIBUTING.md](CONTRIBUTING.md) before
starting work so that changes follow the repository's matching, style, and
asset-handling rules.

Interesting technical discoveries can be recorded in
[FunFacts.md](FunFacts.md).

## Related projects

[smstrikers-decomp](https://github.com/yannicksuter/smstrikers-decomp) is the
sibling decompilation project for *Super Mario Strikers*, the Nintendo GameCube
predecessor first released in November 2005.

## Acknowledgements

This project is based on
[encounter/dtk-template](https://github.com/encounter/dtk-template) and uses
[decomp-toolkit](https://github.com/encounter/decomp-toolkit). Thanks to the
GameCube/Wii decompilation community for the tools and documentation that make
projects like this possible. The initial CodeWarrior runtime sources were
adapted from [Petari](https://github.com/SMGCommunity/Petari). See
[docs/PROVENANCE.md](docs/PROVENANCE.md) for the pinned reference revisions and
the reconstructed source areas adapted from them.

## License

The repository's original source contributions are dedicated to the public
domain under [CC0 1.0](LICENSE). Third-party source retains its own terms; see
[LICENSES](LICENSES) and the [source provenance record](docs/PROVENANCE.md).
Game assets and other copyrighted material are not covered and must not be
committed.
