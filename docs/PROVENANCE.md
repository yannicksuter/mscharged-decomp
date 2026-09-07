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
| RVL DWC 1.4.1 final and debug libraries (2007-05-11) | `dwc_match.c` identity, retained surface, data layout, compiler revision, and symbol and type evidence. No original SDK binary or source is distributed. |
| [NitroDWC 2.2 patch-plus8 (2008-06-30)](https://twlsdk.randommeaninglesscharacters.com/download/NitroDWC/NitroDWC-2_2plus8-080630.zip) | `dwc_match.c` structural donor and later-version comparison. |
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
| [Jim Conger's C++ Blowfish, June 1996 archive](https://www.schneier.com/wp-content/uploads/2015/12/bfsh-con.zip) | Retained CBlowFish implementation, original names and P/S tables in `NL/blowfish.cpp`. |
| [zlib 1.2.2](https://zlib.net/fossils/zlib-1.2.2.tar.gz) | Pristine upstream decompression and checksum sources. |

`libs/RVL_SDK/include/revolution/bte.h` declares the handful of Broadcom BTE
types and entry points the SDK's WPAD and WUD headers refer to (`BD_ADDR`,
`BT_HDR`, `GKI_getbuf`, and the `BTA_Hh*` calls). Their names and widths follow
Broadcom's published BTE/Bluedroid headers.

`Game/Effects/EmitterCallbacks.cpp` contains the emitter updates at
`0x801BE428..0x801BEF44`. The predecessor's `Game/CharacterTriggers.cpp`
supplies the names `UpdateEmitterFromCharacter`,
`UpdateEmitterPoseFromCharacter`, and `UpdateEmitterFromBall`. The filename
and other callback names are descriptive reconstructions from R4QE01 behavior
and callers, rather than recovered retail symbols. These callbacks follow
replay characters, ball trails, ball landing positions, character joints, and
impostor models. The backward and forward character callbacks are used by
`wario_ignition` and `bowserjr_shriek_mouth`, respectively; the head callback
updates `bowser_smoke`.

`Game/GL/GLCompactColourMeshWriter.cpp` contains the mesh writer at
`0x802A7C90..0x802A7F60`. Its class and filename describe the vertex format:
signed 16-bit XYZ with six fractional bits, signed 16-bit UV with ten
fractional bits, and RGBA8 colour. The material program `0x4ED6C66F` and
impostor sprite callers establish that format. These are inferred names,
not recovered retail symbols. Packet finalization takes the allocator as
its third argument, as confirmed by the implementation at `0x8036E438`
and the model loader.

`Game/GL/MeshWriter.cpp` contains the writer at
`0x802A7F80..0x802A81F8` for its float XYZ and signed 16-bit UV streams.
Material `0xEE9D919D` uses twelve fractional UV bits and a uniform colour
parameter. The net mesh and electric fence callers confirm those formats.
The existing semantic API name `MeshWriter` is retained.

`Game/GL/GLTexturedColourMeshWriter.cpp` names the writer at
`0x802A8218..0x802A84E8` for float XYZ, signed 16-bit UV with ten
fractional bits, and RGBA8 colour. Material `0xD3E572DA`, flare rendering,
particle rendering and polygon drawing establish those formats. The class
and filename are descriptive inferences; the predecessor supplies the
`Begin`, `End`, `Colour`, `Texcoord` and `Vertex` API ancestry.

`Game/GL/GLFloatTexturedColourMeshWriter.cpp` names the writer at
`0x802A8508..0x802A87D8` for float XYZ, float UV and RGBA8 colour.
Material `0x0027BCF6` and the polygon drawing caller establish those formats.
The class and filename are descriptive inferences.

`Game/GL/GLColourMeshWriter.cpp` names the writer at
`0x802A87F8..0x802A8A70` for float XYZ and RGBA8 colour without UVs.
Material `0xD701656B` and the modeled transition outline caller establish
those formats. The class and filename are descriptive inferences.

`Game/GL/GLShadowBlendMeshWriter.cpp` contains the writer at
`0x802A8A90..0x802A8D60`. The name describes its use by
`RenderShadowVolumeBlend` with material program `0x386ECBDD`, float XYZ,
RGBA8 colour, and float UV streams. The class and filename are descriptive
inferences; the predecessor supplies the mesh-writer API ancestry.

`NL/plat/TransportSocket.cpp` contains the NLG socket wrapper at
`0x80374D68..0x803751D4`. Retail error messages identify `TransportSocket`;
the descriptive function names follow its open, bind, close, nonblocking,
connect, send, broadcast, and receive operations. The session and statistics
clients share the same four-byte socket state. Original function spellings
and C/C++ linkage are not recoverable from the stripped binary.

`NL/plat/nlFlash.cpp` contains the Wii NAND wrapper and `FlashMemoryTask`
at `0x80376888..0x80376F3C`. The retail task name is "Flash Memory".
Its task update defers completed NAND callbacks; the save/load and buffered
writer callers establish the file operations. The class, function, and file
names describe that behavior and are inferred. The shared task base and the
canonical RVL NAND types retain their existing definitions.

`NL/nlBufferedWriter.cpp` contains the buffered writer at
`0x802B77B0..0x802B7BDC`. It writes to debug files or NAND, retains incomplete
32-byte NAND blocks, and pads the final block when finishing. Transport logs
and recorded network sessions share its 24-byte state. The type, operation,
and file names are descriptive inferences; finishing the writer does not
close its caller-owned file handle.

`NL/glx/glxModel.cpp` contains the platform model hooks at
`0x8036E430..0x8036E4C4`. The predecessor supplies the matrix-access names
and expressions. `glplatFinalizePacket` prepares material state and creates
a display list for permanent packets using the supplied allocator. The
empty `glplatOnPacketCloned` hook is called after duplicating packet and
material data. These hook and file names are inferred from their callers;
the latter is not a resource-release operation.

`NL/gl/glView.cpp` retains the predecessor's `glViewProjectPoint`,
`gl_ViewReset`, and `gl_ViewStartup` names. Charged replaces the indexed view
array with a view hierarchy, adds the orthographic projection path, and resets
the per-view sorter trees each frame. Retail callers establish the additional
projection helper parameters; those functions retain address-based names.

`NL/tu_802A99D8.cpp` contains the incremental inflate wrapper used by the
chunked asynchronous file loader. Its stream layout and allocator callbacks
use the existing zlib 1.2.2 declarations. The stripped executable does not
preserve the wrapper's original file, class, or method names.

`Game/Render/tu_80271960.cpp` includes the static initializer at
`0x80271BE0`, its constructor-table entry, texture handles and pooled strings.
The initializer and fade renderer share the `"global/white"` literal at
`0x80520F88`; the compiler reproduces that sharing, string order and small-data
layout when they occupy one translation unit. The texture declarations retain
the `LightTexture`, `BlackTexture` and `WhiteTexture` names and source form
from the predecessor's `Game/Render/ShootToScoreArrow.cpp`, which also supplies
the renderer's ancestry. The original Charged filename and class name remain
unidentified. The initializer's shared guarded object at `0x806E0B80` still
lacks an identified type and header, so the unit remains `NonMatching`.

`Game/Audio/AudioResourcePlatform.cpp` owns the resource loaders,
report writer, static initialization, accessor, tree methods and destructors at
`0x8035DE28..0x8035EF08`. The traversal and report share a 32-entry resident-bank
tree keyed by memory-loader pointers, using `nlStaticArrayAllocator` and
`Function2`. Each entry contains a 32-byte bank name and its size. The report writer's file
handle is initialized after the tree; the initializer also requests the same guarded template statics
used by other audio consumers. The stream-reader allocation and release paths
reference the 16-entry stream-block allocator at `0x806E220C`.

The portable bank-loader interface is separate from the platform loaders, and
its chunk parser is defined as a member in `unclassified/tu_802EE964.cpp`. The
manager accessor belongs to the audio interface header, and the platform
loaders declare their own destructors. With file analysis, explicit inlining
and per-header text sections, these definitions reproduce the loader vtable
order and place the header-defined routines after static initialization. The
unreferenced `blr` at `0x8035E63C` is the final instruction
of `AudioMemoryLoader::ParseChunk`, not a separate empty function.
All 19 methods now match, and the normal build links the unit from C++ while
reproducing the retail DOL. Report content is written by an internal inline
routine, with file opening and closing in `DumpAudioBankMemory`. The shared AVL
tree uses its entry-deletion operation for both individual removal and clearing.
These definitions also reproduce the retained helper order and initializer
references. Audio filenames and descriptive identifiers are reconstruction
names; no original debug identifiers survive in R4QE01.

The resource loader retains the predecessor's three-argument
`SPInitSoundTable` call, including the silence-buffer base in `r5`. The Wii
implementation overwrites that register and does not use the third argument.
The shared declaration preserves this compatibility, and `SPSoundTable` has a
struct tag so game headers can forward-declare the SDK table type.

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

`NL/blowfish.cpp` retains the constructor, block encipher, key initialization
and padded encoding paths from Jim Conger's C++ conversion, including its
output-length helper. The source archive SHA-256 is
`62cb58180760d91bbf8876c66ca7e41150d0bc054dbaf8bfeb1fd1596df4b2f3`.
R4QE01 uses the project's aligned allocator and retains the donor's DCBA
bitfield order on PowerPC. The key packing, S-box byte selection and padding
behavior follow the game; they are not changed to a different Blowfish
implementation's byte order. Unretained decryption/destruction bodies are
not imported. The original source attribution is preserved.

The repository's [CC0 license](../LICENSE) applies only to contributions whose
authors have the right to make that dedication. Nintendo, Next Level Games,
and third-party names, trademarks, game content, and other proprietary
material are not granted or licensed by this repository.
