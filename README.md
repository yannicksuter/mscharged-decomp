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
> This repository does **not** provide a new way to play *Mario Strikers Charged*. It is not a modern recompilation ("recomp") or a port. For completed versions, 100% code and linking means the reconstructed source compiles into an executable byte-for-byte identical to the original retail Nintendo Wii release.
>
> Recompilations and ports based on this work are maintained separately and are outside the scope of this repository. Please do not ask for recompilations or ports in the decompilation Discord, as producing and supporting them is not the focus of this community.

A work-in-progress matching decompilation of *Mario Strikers Charged* for
Nintendo Wii, developed by Next Level Games and published by Nintendo.

> [!NOTE]
> Version **1.0.0** of the [native PC port](https://github.com/yannicksuter/mscharged-port) has been released and is available now. Based on this decompilation, the port uses the decompiled source directly and does not rely on recompilation ("recomp"). This approach enables bug fixes and modding.

This repository does **not** contain game assets or the original executable.
You must provide your own legally obtained copy of the game.

## Supported versions

| Regional Wii release | Version | Region and title | Revision | Status | `main.dol` SHA-1 |
| --- | --- | --- | --- | --- | --- |
| [2007-05-25](https://www.nintendo.com/en-gb/Games/Wii/Mario-Strikers-Charged-Football-281892.html) | `R4QP01_rev1` | Europe - *Mario Strikers Charged Football* | Rev 1 | **Complete: 100% code / 100% linked** | `0a066b755b469648a371d1cfc2d1bcad315b2b4a` |
| [2007-05-25](https://www.nintendo.com/en-gb/Games/Wii/Mario-Strikers-Charged-Football-281892.html) | `R4QP01_rev2` | Europe - *Mario Strikers Charged Football* | Rev 2 | **Complete: 100% code / 100% linked** | `c39e1eda1942aa2fc96a6a71bfef051698e23516` |
| [2007-07-30](https://www.nintendo.co.jp/ir/pdf/2008/080125e.pdf#page=8) | `R4QE01` | USA - *Mario Strikers Charged* | Rev 1 | **Complete: 100% code / 100% linked** | `e96d2298067b70652752145b4e63644e6e1b1560` |
| [2007-09-20](https://www.nintendo.co.jp/wii/softlist/n2007_2006.html) | `R4QJ01` | Japan - *Mario Strikers Charged* | Rev 0 | **Complete: 100% code / 100% linked** | `74be723403329158b1645d9961a3cd6be755cdb8` |
| [2010-03-18](https://www.inven.co.kr/webzine/news/?news=26630) | `R4QK01` | Korea - *Mario Power Soccer* | Rev 0 | **Complete: 100% code / 100% linked** | `c2f893aa71911c4ad4dcead1474e517e8e598fcb` |

Dates are the original regional Wii releases, sorted chronologically. Separate
release dates for the EU disc revisions have not been verified; both EU rows
show the regional launch date. The linked Nintendo release information and
Nintendo Korea announcement provide the date sources.

`R4QE01` remains the default. All five versions link entirely from reconstructed
source and reproduce their retail executables. Each version has its own
symbols, splits, progress report and retail DOL hash. The revision numbers
above are the revision bytes recorded in the disc headers.

The former `R4QP01` configuration is now named `R4QP01_rev2`. For an existing
checkout, move the local files from `orig/R4QP01` into `orig/R4QP01_rev2` and
configure again. Revision 1 needs its own disc image; the two EU revisions are
not interchangeable.

[comparison.md](comparison.md) compares the five builds in detail: disc and
system data, build stamps, SDK versions, executable layout, version-specific
code and disc assets. It shows that EU revision 2 takes part of the US fixes,
that the Japanese build is US code with interface fixes, and that the 2010
Korean build is a separate rebuild with its own title logo and goalie tuning.

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

Place a supported disc image in `orig/<version>`: `orig/R4QE01` for USA,
`orig/R4QP01_rev1` for EU revision 1, `orig/R4QP01_rev2` for EU revision 2,
`orig/R4QJ01` for Japan, or `orig/R4QK01` for Korea.
ISO, RVZ, WIA, WBFS, CISO, NFS, GCZ, and TGC images
are supported. Use the disc revision listed above.

Then configure and build:

```sh
python3 configure.py
ninja
```

To select another version:

```sh
python3 configure.py --version R4QP01_rev1
ninja

python3 configure.py --version R4QP01_rev2
ninja

python3 configure.py --version R4QJ01
ninja

python3 configure.py --version R4QK01
ninja
```

Running `python3 configure.py` without `--version` selects USA again. Build
outputs are kept separately in `build/<version>`; `build.ninja` and
`objdiff.json` describe the currently selected version.

On the first build, decomp-toolkit extracts `sys/main.dol` from the disc image
and performs the initial analysis. Once extraction succeeds, the disc image can
be removed from `orig/<version>` to save space.

For USA, Europe and Japan, Ninja also generates the disc-message image include
from that version's `main.dol`. The generated `DiscMessageImages.inc` stays in
the build directory and is ignored by Git, including when using `--build-dir`.
Korea uses its separate source implementation in
`src/RVL_SDK/dvd/R4QK01/dvdFatal.c`.

The reconstructed `config/<version>/symbols.txt` and
`config/<version>/splits.txt` are checked into the repository. Disc images,
extracted game files, split objects and build outputs must remain untracked.
A successful USA build ends with:

```text
build/R4QE01/main.dol: OK
```

Each version checks its own `build/<version>/main.dol` in the same way.
The build prints code matching and source-linking percentages and writes
`build/<version>/report.json`. A verified DOL can still contain extracted
objects; the linking percentage counts only code linked from source.

To generate a linker map for the reconstructed ELF, configure with `--map`:

```sh
python3 configure.py --map
ninja
```

The result is written to `build/<version>/main.elf.MAP`. It describes the
reconstructed link rather than an original development map and is intentionally
ignored by Git.

## Diffing

After configuration, open the project directory in
[objdiff](https://github.com/encounter/objdiff). It reads the generated
`objdiff.json` automatically and rebuilds changed objects with Ninja.

Some retained template definitions are provided by another compiled source
object. `objdiff_weak_providers` lists those sources for a unit. Ninja creates
its comparison object in `build/<version>/compare/`, preserving the local
object's definitions and adding missing weak definitions from the providers.
The ordinary compiled objects remain the linker inputs. This accounts for
shared method matches; source-linking a TU still requires matching its layout.

## Source review and regional verification

Matching the executable and reviewing the source are separate checks. A matching
DOL verifies the retained instructions and data; it does not establish original
names, types or source organization. Source review continues after a version
reaches 100% matching and linking.

For a source cleanup or regional change:

1. Build the unchanged revision and preserve its report and affected compiled
   objects as the baseline.
2. Read the implementation, declarations, callers and referenced data. Check
   behavior, ABI, layout, lifetime and ownership against the relevant retail
   executable. Use descriptive names supported by those uses; keep an uncertain
   identity neutral. Reconstructed names are not recovered original symbols.
3. Update definitions, owning headers, consumers and the applicable regional
   symbol files together. A file move also updates its build entry and splits.
   Compiler-generated pools, alignment gaps and anonymous labels do not need
   invented source names. Keep changes to algorithms separate from naming work.
4. Rebuild every affected consumer, including indirect users of shared headers.
   For a naming cleanup, compare allocated bytes, section alignment, symbol
   positions and bindings, and relocation targets after accounting for the
   intended renames. Preserve each existing function and data result, rather
   than relying on an aggregate percentage.
5. Verify that the linker consumes the compiled source objects, generate fresh
   reports, and require the retail DOL hash for every affected version. Shared
   source changes require all five regional checks; USA and both EU revisions
   must preserve their established behavior and matching results.

With the extracted originals available for every version, run the build matrix
from the repository root:

```sh
set -e
for version in R4QE01 R4QP01_rev1 R4QP01_rev2 R4QJ01 R4QK01; do
    python3 configure.py --version "$version"
    ninja all_source
    ninja
done
```

Keep comparison reports and build artifacts under the ignored build directory.
Document the affected scope and verification results with the change. Low-level
SDK and runtime code can use assembly; a byte match alone does not prove its
historical source form or justify a construct used only to force code generation.

## Contributing

Contributions are welcome. Read [CONTRIBUTING.md](CONTRIBUTING.md) before
starting work so that changes follow the repository's matching, style, and
asset-handling rules.

## Related projects

[smstrikers-decomp](https://github.com/yannicksuter/smstrikers-decomp) is the
sibling decompilation project for *Super Mario Strikers*, the Nintendo GameCube
predecessor first released in November 2005.

## Acknowledgements

This project is based on
[encounter/dtk-template](https://github.com/encounter/dtk-template) and uses
[decomp-toolkit](https://github.com/encounter/decomp-toolkit). Thanks to the
GameCube/Wii decompilation community for the tools and documentation that make
projects like this possible.

## License

The repository's original source contributions are dedicated to the public
domain under [CC0 1.0](LICENSE). Third-party source retains its own terms; see
[LICENSES](LICENSES).
Game assets and other copyrighted material are not covered and must not be
committed.
