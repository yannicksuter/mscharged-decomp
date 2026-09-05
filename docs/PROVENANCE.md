# Source provenance

This project reconstructs source code from the behavior and machine code in a
legally obtained copy of *Mario Strikers Charged*. It does not distribute game
assets, Nintendo SDK binaries, extracted assembly, or original proprietary
source code.

The project was built with or adapted source from the following projects:

| Project | Use in this project |
| --- | --- |
| [encounter/dtk-template](https://github.com/encounter/dtk-template) | Project and build-system foundation. |
| [SMGCommunity/Petari at `e789e52`](https://github.com/SMGCommunity/Petari/tree/e789e52e7774cf52cdf093002fc32035b6eab65d) | CodeWarrior runtime, MetroTRK, and RVL SDK reconstructions. |
| [HaydnTrigg/Castaway at `f5c5dbcf`](https://github.com/HaydnTrigg/Castaway/tree/f5c5dbcf2b4a20eb797b1aa6eb03f6cf09a4dbbe) | RVL SDK reconstructions. |
| [ThePlayerRolo/LegoCloneWarsWii at `b0ed795`](https://github.com/ThePlayerRolo/LegoCloneWarsWii/tree/b0ed795a586d1d921c536c991bf250bec39ae811) | CodeWarrior runtime reconstruction. |
| [koopthekoopa/wii-ipl at `0b0cedd`](https://github.com/koopthekoopa/wii-ipl/tree/0b0cedd44d83fa187eb3f52748c3eecf0840865b) | RVL SDK and Metrowerks Standard Library reconstructions. |
| [doldecomp/ogws at `a764c49`](https://github.com/doldecomp/ogws/tree/a764c49183610bce30ec79ca83e228b86d2ca0ab) | RVL SDK reconstructions at the same library revision as R4QE01. |
| [doldecomp/Rhae at `35929a0`](https://github.com/doldecomp/Rhae/tree/35929a0bfcca6c55c6305a72cc86caef54203c1f) | Expanded RVL SDK AXFX reverb reconstructions. |
| [doldecomp/mkw at `94585b8`](https://github.com/doldecomp/mkw/tree/94585b8a8fd7a2a52f30640ccff316e57880b6c1) | DWC allocator and initialization reconstructions, plus DWC GHTTP, common-utility, friend-unit, login-unit, main-unit, and transport-unit symbol and boundary references. |
| [OpenXRay/xray-15 at `ad7b68b`](https://github.com/OpenXRay/xray-15/tree/ad7b68b700d73abf6487675ab5905c06617a714a/cs/engine/xrGameSpy/gamespy) | Period GameSpy GHTTP, RSA, RC4, and large-integer source used to restore the retained encryption revision and translation-unit ownership. |
| [GameProgressive/UniSpySDK at `80c6b61`](https://github.com/GameProgressive/UniSpySDK/tree/80c6b61d57ddf9e3151d030ead5f3f23aadaec77) | Public GameSpy SDK 2.12.00 Revolution socket source used to restore the retained DNS-cache `gethostbyname` implementation and donor identifiers. |
| [ntrtwl/NitroDWC at `ba54898`](https://github.com/ntrtwl/NitroDWC/tree/ba548986500a6628ec37c0603bdb94245ff12a63) | GameSpy GP peer, search, transfer, and unique-nick source structure and donor identifiers, plus the QR2 receive-loop source structure. |
| [CommunityRTS/MinGenerals at `ec0378a`](https://github.com/CommunityRTS/MinGenerals/tree/ec0378a99bf79f5d5f43c8914ee4399db207a80b/libraries/gamespy/gamespy/gp) | Later public GameSpy unique-nick source used to identify the partner-ID request addition present in the target revision. |
| [DarkRTA/rb3 at `ababaa9`](https://github.com/DarkRTA/rb3/tree/ababaa9bb6a669af06aa7def7dd735c4f5265061) | DWC report and initialization structural references. |
| [pret/pokerevo at `ae02670`](https://github.com/pret/pokerevo/tree/ae02670ef33650e44313443dd95180f485150d84) | PowerPC DWC non-port, digest, and account function identities, plus NHTTP code-shape references. |
| [e-minence/black_white at `9699f2e`](https://github.com/e-minence/black_white/tree/9699f2e44036b044fc6f271becbaff979cee8679) | Later Nintendo NHTTP source used as a naming, constant, and revision-lineage reference. |
| RevoEX 1.0 patch-plus2 (2007-05-10) | Nintendo NET release/debug objects used to recover helper boundaries, names, source order, and compiler inlining behavior. |
| Revolution DWC 1.4.1 (2007-05-11) | Exact-date Final and Debug account, encoding-session, ranking-session, GameSpy socket, GameSpy GP, QR2, and GHTTP objects, public account/non-port headers, build flags, source paths, symbols, signatures, structure layouts, and C89 local-variable metadata used to restore account source structure, the separate `dwc_encsession.c`/`dwc_ranksession.c` ownership boundary, the GP search/transfer/unique object boundaries and revision, the QR2 receive-loop call boundary and IP-verification revision, the GHTTP posting-helper revision, and the Revolution `getlocalhost` implementation. |
| [pret/pokediamond at `038ccca`](https://github.com/pret/pokediamond/tree/038cccaed5de8f013875bc5d734f912d1de08e0f) | Nitro SDK CRC API, structure, and source reconstruction. |
| [RushRE/SonicRushAdventure-Decomp at `94de7ce`](https://github.com/RushRE/SonicRushAdventure-Decomp/tree/94de7ce8e95c1136f8d14e93f1dc3c07d10f20b9) | Nitro SDK random-number API and inline implementation reference. |
| [NicholasMoser/Naruto-GNT-Modding at `a16d1c0`](https://github.com/NicholasMoser/Naruto-GNT-Modding/tree/a16d1c03572832d168d096c405fa085b04d86050) | DWC `dwci_np_math.c` retail-map ownership and retained-function reference. |
| [MelgMKW/Pulsar at `820ad92`](https://github.com/MelgMKW/Pulsar/tree/820ad929c3c7141a0396692d8b0896d1546240fd) | Mario Kart Wii retail-map identities and ordering for DWC non-port and authentication helpers. |
| [doldecomp/brawl at `ec22759`](https://github.com/doldecomp/brawl/tree/ec2275908c1b22af4e47cc57bee51e070aff2adb) | DWC time-helper code-shape and `DWC_GetDateTime` identity reference. |
| [NitroDWC 1.2 patch-plus4 (2006-08-25)](https://twlsdk.randommeaninglesscharacters.com/download/NitroDWC/NitroDWC-1_2-patch-plus4-20060825.zip) | DWC account, NAS-time, and ranking API, structure, semantics, and donor naming reference. |
| [zeldaret/oot-vc at `9c18537`](https://github.com/zeldaret/oot-vc/tree/9c1853710bcf6b598b0710022fdb87b9a8e99e2c) | MetroTRK, RVL SDK, Home Button Menu (`nw4hbm`), and Metrowerks Standard Library reconstructions. |
| [zeldaret/ss at `9481f3a`](https://github.com/zeldaret/ss/tree/9481f3ab62d690bd6534cb0adf7e02db5579b333) | NintendoWare sound API, method-order, and debug-symbol references. |
| [robojumper/sdk_2009-12-11 at `19e2ec4`](https://github.com/robojumper/sdk_2009-12-11/tree/19e2ec48d61a84ee072dc4fb5ac1db9bfd08c766) | Matched later Home Button Menu binary-file APIs and shared endian-helper reconstruction. |
| [LoigiFan72/NW4C at `ad5d65b`](https://github.com/LoigiFan72/NW4C/tree/ad5d65b9600463dea25f66958874cf41acf3692f) | Independent NintendoWare binary-file API and assertion-ownership reference. |
| [projectPiki/pik1wii at `96c1af6`](https://github.com/projectPiki/pik1wii/tree/96c1af63ed75996755e53bb85decce3c890e5ca4) | RVL SDK WPAD encryption and HID parser reconstructions. |
| [conhlee/rhf at `4429cbc`](https://github.com/conhlee/rhf/tree/4429cbcb000abff644683d98701f6ab8c1671e10) | RVL SDK `DVDSetAutoInvalidation` reconstruction. |
| [zeldaret/tp at `ecafc81`](https://github.com/zeldaret/tp/tree/ecafc81d53cadf3c29ac01b03170cbe9bc37c686) | RVL SDK reconstructions. |
| [zcanann/FFCC-Decomp at `ff63985`](https://github.com/zcanann/FFCC-Decomp/tree/ff63985d02f959457ca7812e56c817ac61c72493) | Metrowerks Standard Library tokenizer structure and donor identifiers. |
| [yannicksuter/smstrikers-decomp at `6123c35`](https://github.com/yannicksuter/smstrikers-decomp/tree/6123c3546baf59aa5d564e98fdd76cf16443c80f) | Predecessor game code, Dolphin SDK lineage, and Next Level Games' ODE extensions. |
| Open Dynamics Engine 0.5 (`ode-040529`) | Baseline for ODE-derived source. |
| [zlib 1.2.2](https://zlib.net/fossils/zlib-1.2.2.tar.gz) | Pristine upstream decompression and checksum sources. |

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
