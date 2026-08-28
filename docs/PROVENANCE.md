# Source provenance

This project reconstructs source code from the behavior and machine code in a
legally obtained copy of *Mario Strikers Charged*. It does not distribute game
assets, Nintendo SDK binaries, extracted assembly, or original proprietary
source code.

The project was built with or adapted source from the following projects:

| Project | Use in this project |
| --- | --- |
| [encounter/dtk-template](https://github.com/encounter/dtk-template) | Project and build-system foundation. |
| [SMGCommunity/Petari at `e789e52`](https://github.com/SMGCommunity/Petari/tree/e789e52e7774cf52cdf093002fc32035b6eab65d) | CodeWarrior runtime and RVL SDK reconstructions. |
| [HaydnTrigg/Castaway at `f5c5dbcf`](https://github.com/HaydnTrigg/Castaway/tree/f5c5dbcf2b4a20eb797b1aa6eb03f6cf09a4dbbe) | RVL SDK reconstructions. |
| [ThePlayerRolo/LegoCloneWarsWii at `b0ed795`](https://github.com/ThePlayerRolo/LegoCloneWarsWii/tree/b0ed795a586d1d921c536c991bf250bec39ae811) | CodeWarrior runtime reconstruction. |
| [koopthekoopa/wii-ipl at `0b0cedd`](https://github.com/koopthekoopa/wii-ipl/tree/0b0cedd44d83fa187eb3f52748c3eecf0840865b) | RVL SDK reconstructions. |
| [doldecomp/ogws at `a764c49`](https://github.com/doldecomp/ogws/tree/a764c49183610bce30ec79ca83e228b86d2ca0ab) | RVL SDK reconstructions at the same library revision as R4QE01. |
| [doldecomp/Rhae at `35929a0`](https://github.com/doldecomp/Rhae/tree/35929a0bfcca6c55c6305a72cc86caef54203c1f) | Expanded RVL SDK AXFX reverb reconstructions. |
| [doldecomp/mkw at `94585b8`](https://github.com/doldecomp/mkw/tree/94585b8a8fd7a2a52f30640ccff316e57880b6c1) | DWC allocator and initialization reconstructions, plus DWC GHTTP, common-utility, friend-unit, login-unit, main-unit, and transport-unit symbol and boundary references. |
| [DarkRTA/rb3 at `ababaa9`](https://github.com/DarkRTA/rb3/tree/ababaa9bb6a669af06aa7def7dd735c4f5265061) | DWC report and initialization structural references. |
| [zeldaret/oot-vc at `9c18537`](https://github.com/zeldaret/oot-vc/tree/9c1853710bcf6b598b0710022fdb87b9a8e99e2c) | RVL SDK and Home Button Menu (`nw4hbm`) reconstructions. |
| [projectPiki/pik1wii at `96c1af6`](https://github.com/projectPiki/pik1wii/tree/96c1af63ed75996755e53bb85decce3c890e5ca4) | RVL SDK WPAD encryption and HID parser reconstructions. |
| [conhlee/rhf at `4429cbc`](https://github.com/conhlee/rhf/tree/4429cbcb000abff644683d98701f6ab8c1671e10) | RVL SDK `DVDSetAutoInvalidation` reconstruction. |
| [zeldaret/tp at `ecafc81`](https://github.com/zeldaret/tp/tree/ecafc81d53cadf3c29ac01b03170cbe9bc37c686) | RVL SDK reconstructions. |
| [yannicksuter/smstrikers-decomp at `6123c35`](https://github.com/yannicksuter/smstrikers-decomp/tree/6123c3546baf59aa5d564e98fdd76cf16443c80f) | Predecessor game code, Dolphin SDK lineage, and Next Level Games' ODE extensions. |
| Open Dynamics Engine 0.5 (`ode-040529`) | Baseline for ODE-derived source. |

`libs/RVL_SDK/include/revolution/bte.h` declares the handful of Broadcom BTE
types and entry points the SDK's WPAD and WUD headers refer to (`BD_ADDR`,
`BT_HDR`, `GKI_getbuf`, and the `BTA_Hh*` calls). Their names and widths follow
Broadcom's published BTE/Bluedroid headers.

`src/RVL_SDK/bte/` and `libs/RVL_SDK/include/private/bte/` do vendor Broadcom
source. Both trees carry Broadcom's original copyright notice and its
Apache-2.0 licence header, and each file records the Bluedroid path it came
from. That material is licensed under
[Apache-2.0](../LICENSES/Apache-2.0.txt) and is not covered by the CC0 terms
below.

`src/RVL_SDK/gamespy/` vendors the GameSpy SDK, the middleware behind the
game's online matchmaking and profile services. IGN Entertainment released it
in 2011 under a three-clause BSD licence, reproduced in
[LICENSES/BSD-3-Clause-GameSpy.txt](../LICENSES/BSD-3-Clause-GameSpy.txt) and
kept alongside the source as `license.txt`. Its headers sit beside its sources
and are included by bare name, so the library's own directory layout is
preserved rather than folded into a shared include root.

Referenced decompilation material remains subject to its source project's
terms; material explicitly published under CC0 1.0 remains CC0. ODE-derived
source retains the
[ODE BSD-style license](../LICENSES/ODE-BSD.txt). GameSpy source retains its
[three-clause BSD licence](../LICENSES/BSD-3-Clause-GameSpy.txt). Other third-party material
retains its respective upstream terms.

External source is accepted only after comparison against R4QE01. A unit is
marked `Matching` only when its code and owned data agree and the complete
build reproduces the original `main.dol` hash.

The repository's [CC0 license](../LICENSE) applies only to contributions whose
authors have the right to make that dedication. Nintendo, Next Level Games,
and third-party names, trademarks, game content, and other proprietary
material are not granted or licensed by this repository.
