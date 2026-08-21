# Mario Strikers Charged

A work-in-progress matching decompilation of *Mario Strikers Charged* for
Nintendo Wii, developed by Next Level Games and published by Nintendo.

This repository does **not** contain game assets or assembly. You must provide
your own legally obtained copy of the game.

## Supported version

- `R4QE01`: USA, disc revision 1
- `main.dol` SHA-1: `e96d2298067b70652752145b4e63644e6e1b1560`

Other regions and revisions are not configured.

## Project status

The initial Wii project configuration and DOL analysis are complete. The
generated splits rebuild the original `main.dol` byte-for-byte; source
reconstruction is at an early stage. The disc contains no separate REL or RSO
modules.

The bootstrap linker/compiler default is Metrowerks CodeWarrior `GC/3.0a5`.
Exact compiler revisions are still being determined per library and translation
unit. The first matched `Runtime.PPCEABI.H` units reproduce with `GC/3.0a3`,
`GC/3.0a5`, and `GC/3.0a5.2`.

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
domain under [CC0 1.0](LICENSE). Game assets and other copyrighted material are
not covered and must not be committed.
