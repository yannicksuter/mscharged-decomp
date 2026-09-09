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

`Game/GameInfo.h` adapts `UserInfo`, its option members, save ID and five
lifetime-counter names from the predecessor's same header. R4QE01 serializes
the smaller `0x80`-byte block at `GameInfoManager+0x9C`; its constructor and
save writer confirm the retained members. `GameplaySettings` keeps the
predecessor's `SkillLevel`, `GameTime` and skill-level enum. The home/away
powerup and special-shot flags follow the retail challenge settings and their
gameplay consumers. `WinBy`, `GameGoals` and `BestSeries` follow the recorded-game
diagnostics in `Game/NetworkSession.cpp`.

`Game/SH/SHOnlineFriendsChooseSides.cpp` names the scene constructed at
`0x8026B10C` for scene 56 and `art/fe/online_friends_choose_sides.fen`. Its
callback and member names describe the retail home/away selection, guest-player
mapping, and countdown behavior. `DoChangeSides` follows its retail diagnostic
string. `NetMessageSidesChanged` follows the session send diagnostics and
message-25 dispatch; the payload carries the machine, side, guest flag, and
host-response flag. These scene and message identities describe supported
roles; original debug spellings are unavailable.

`Game/FE/feTimer.h` describes the callback timer at `0x8030616C`. The
front-end consumers and retail timer-list operations establish its callback,
enabled state, duration, elapsed time, tick count, and intrusive next pointer.
`SetEnabled` resets elapsed time only when the enabled state changes; `Update`
invokes the callback at most once per call. The class name describes this role.

`Game/Audio/AudioSystem.cpp` is the reconstruction unit for
`0x802EBD54..0x802ED144`, including the listener, sound-handle, and callback
helpers first retained here. Its constructor, update/flush operations,
member-function pointers, and vtables connect the ordinary code to this tail.
The initializer at `0x802ECF90`, referenced by `.ctors:0x804DBA3C`, registers
the `audio/Stats` / `AllocatedCueCount` tweak used by this unit and closes the
range before `AudioResourceLoadOwner`. Its tweak and destructor-registration
storage is `0x8057F928..0x8057F960`; the shared sound pools themselves remain
with their earlier owner. The unit owns `.data:0x8052F400..0x8052F48A`,
`.sbss:0x806E2018..0x806E2020`, and the two float constants at
`0x806E65B0..0x806E65B8`, not the preceding transition constant or trailing
data alignment padding. The filename follows the existing class identity,
not recovered original filename metadata; the unit remains unreconstructed.

The former automatic boundary at `0x802F7E00` bisected a deferred helper
tail. `unclassified/tu_802F6BC4.cpp` groups `0x802F6BC4..0x802F841C`:
the ordinary callers use the AVL walks, searches, comparator, and callback
adapter after that boundary, while the initializer at `0x802F7D94`
(`.ctors:0x804DBA70`) registers the pool destructor at `0x802F83A8`.
Its member-function pointer and adapter table occupy
`0x8052F978..0x8052F998`; its registration record and pool occupy
`0x8057FBC8..0x8057FBF0`, including four bytes of interior alignment.

`unclassified/tu_802F841C.cpp` groups `0x802F841C..0x802F98F4`. Its
ordinary methods operate on the same two AVL trees and use the following
walks and callback adapters. The two empty `AudioEffectBase` methods at
`0x802F98EC` and `0x802F98F0` stay with this consumer's tail, not with the
following constructor. The associated member-function pointers and three
five-word adapter tables occupy `0x8052F998..0x8052F9EC`. The default value
at `0x806DF4F0` is one byte, as shown by the receiving node allocator's
`lbz`/`stb` at `0x802F6654..0x802F6658`; subsequent zero bytes are alignment,
not part of the value. These two unit names remain address-based because
the predecessor has no corresponding audio implementation establishing
original filenames or concrete template argument names. Both remain
unreconstructed.

The automatic function split at `0x80302A98..0x80302D04` contains guarded
initialization of fourteen FE default objects, reached from
`.ctors:0x804DBA84`; it is not a localization function. Its object storage
is `0x8057FFB0..0x80580720`, with fourteen guard bytes at
`0x806E2078..0x806E2086`. The predecessor's retail `MarioSoccerZ.MAP`
distinguishes `tlTextInstance.o` (`SetStringId`) from
`tlTextInstance_runtime.o` (`GetString`, `Render`, and the string/scissor
setters), supporting the existing two text-instance units. The initializer's
original filename and ownership relative to the preceding runtime unit are
not established; its automatic split is not evidence of a separate original
translation unit.

`SetPlayButtonBounds` and `SetDoneButtonBounds` in `Game/FE/feHelpFuncs.cpp`
follow the navigation controls bound by their callers: `Play_Now` (mask
`0x10`) and `done` (mask `0x20`). Their separate four-float bounds tables
remain as retained in R4QE01.

The character conversion, team/sidekick name, and mode-name helpers in
`feHelpFuncs.cpp` preserve the predecessor's API names and character-ID enum
parameter types. Charged's match-end/cup-win and score/winner-title callers distinguish
`GetLOCCharacterName` from `GetLOCTeamName`; both now return localization keys,
and the former no longer takes the predecessor's Super Team flags. The
`FECharacterSound` tables retain their donor names, while the sound getters
describe Charged's lookup-only behavior. Cheat name/description helpers and
their progress predicates follow retail localization keys and switch cases.
The character idle helpers follow the `fe_idle`/`fe_idle_action_01` animation
cycle. These added role names do not claim recovered debug spellings.

`Game/SH/SHOptionsCheatsList.cpp` describes scene 28, whose retail factory
loads `art/fe/options_cheats_list.fen` and constructs the `0x668`-byte scene at
`0x802582C4`. Its pointer callbacks, scroll offset, and localization lookups
establish the item and category names. The options scene constructs
`PowerupSettings` at offset `0x15DC` and passes that same member to the list;
the list therefore uses the shared settings type. Selection and preview
callers establish its three word-sized fields: custom powerups, environment
cheat, and player cheat. `mCustomPowerups` preserves the predecessor's field
name for that role; the added category and getter names describe Charged's
behavior without assuming the predecessor's smaller settings layout.

`Game/SH/SHGameplayOptions.h` describes the parent options scene constructed
at `0x8023483C` for scene 27 and `art/fe/options.fen`. Its retail gameplay
settings, series/goal/time controls, and diagnostics establish this role. The
constructor and destructor identify its 24 option buttons, three cheat
buttons, back button, and Done button; callers identify its shared page
controls and settings members. Its nine-slot vtable establishes the retained
lifecycle declarations. `UpdateCheatText` at `0x80238234` updates the three
cheat name/description pairs after a selection. The predecessor's gameplay
menu supplies the applicable `mSettings` name, while Charged's embedded
storage and pointer controls follow the retail layout. Unresolved members
retain neutral names.

`FEModelManager::GetModel` at `0x801C2FB4` searches the manager's handle list
using the lowercase hash of a model name. The presentation interpreter uses
that same manager and the owning `FEModelHandle` animation, transform,
position, visibility, and callback interfaces. Its model completion callback
restarts `fe_idle`. `GetCharacterTemplateInfo` at `0x8002600C` returns the
character-indexed template record, or the shared goalie record for indices
twenty and above. These names describe retail behavior.

`Game/FriendManager.cpp` describes the friend-list and invitation manager at
`0x801360A4`. Retail diagnostics provide the five `EFriendStatus` enumerators
and the status-setting operation names. The status payload stores the targeted
profile, game/powerup settings, network version, and stadium. Invitation screens
establish the return scene, saved ranked mode, and UTF-16 friend-code input.
`GetFriendManager` returns the manager allocated by the network task;
`FormatFriendKey` prints its two six-digit groups. The two manager pointers
retain distinct storage: `Reset(true)` also registers the instance pointer.
The class, file, and remaining role names are descriptive reconstructions.

`libs/RVL_SDK/include/revolution/bte.h` declares the handful of Broadcom BTE
types and entry points the SDK's WPAD and WUD headers refer to (`BD_ADDR`,
`BT_HDR`, `GKI_getbuf`, and the `BTA_Hh*` calls). Their names and widths follow
Broadcom's published BTE/Bluedroid headers.

`unclassified/tu_801B5EE8.cpp` reconstructs the GX setup routine at
`0x801B5EE8..0x801B6168`, called by material program support at `0x8028CC78`.
Its original filename and function name remain unidentified. The two flag
bytes control additional texture stages; a nonzero float enables the final
constant-colour blend. The predecessor's `NL/nlColour.h` supplies
`nlFloatColourSet` and the existing `ConvertColour` abstraction. The partition
also retains the adjacent initializer at `0x801B6168..0x801B6188`, its constructor
entry at `0x804DB750`, and the routine's float literals at `0x806E5420..0x806E5428`.
The initializer's original shared definition and inclusion trigger remain
unidentified, so it is not reconstructed and the complete unit remains
assembly-linked.

`unclassified/tu_801B535C.cpp` reconstructs the 0x4C-byte object used by the
snapshot renderer and the sphere at `0x801798A8`. Its orientation, position,
radius, activation flag, drawable, player reference, and pending displacement
are corroborated by construction, update, collision, and snapshot consumers.
No predecessor source identity has been established; the class and methods
retain address-based names. The vector constants at `0x804DCF98` and
`0x804DCFA4` remain owned by `unclassified/tu_801B298C.cpp`.
The partition includes the following initializer at `0x801B5DF8` and its
constructor entry at `0x804DB74C`. The initializer remains unreconstructed
because the shared pool specializations and their inclusion trigger are
unidentified; the complete unit remains assembly-linked.
The retail `0x801B59DC` path also queries `cFielder::IsFallenDown()` before
playing its sound, but the use of that result is unresolved; no discarded
query was added solely to reproduce the call.

`Game/FE/feFinder.cpp` reconstructs the shared presentation and recursive
instance lookups at `0x8030677C..0x80306B34`. A raw lookup can return a
`TLSlide` or a `TLInstance`; typed finder callers select the expected result
type. The circular-list traversal follows the predecessor's `feFinder.h`.
The finder range and the preceding timer family at
`0x8030616C..0x8030677C` are separated from the font-loading code beginning
at `0x80306B34`. These source groupings and the `Game/FE/feTimer.cpp` filename
are inferred from the retained behavior and calls; original object names
are unavailable.

`Game/GameInfo.h` reconstructs the 0x80-byte `UserInfo` settings record at
`GameInfoManager+0x9C`, using the predecessor's record and option names where
their roles agree with R4QE01. `Game/DB/UserOptions` uses `CheatSettings` for
the three-word custom-powerup, environment and player cheat selections;
its zero defaults and frontend string-key lookups distinguish it from the
predecessor's enabled-powerup bit fields. Gameplay settings contain paired
home/away powerup and megastrike enable flags, as established by the award
and shooting checks. The other new field and accessor names describe retained
behavior; they are not recovered retail debug identifiers.

`Game/TweakRegistry`, `Game/TweakNode`, `Game/TweakEntry` and
`Game/TweakNameRecycler` use descriptive reconstructed names for the
registry hierarchy, deferred registrations, string storage and value
lifetime states. The predecessor has no corresponding value hierarchy.
`NL/PointerEntryTable` replaces the address-based table identity; its
retained removal method searches entries by pointer equality. These names
describe R4QE01 behavior and do not establish original source filenames
or debug identifiers.

`Game/TweakConfig` names the configuration parser and synchronous file
and buffer-loading entry points by their retained behavior. Its
`Config::Parser` callbacks preserve the predecessor interface names,
while its loader names and filename are descriptive reconstructions.
The in-place parser is distinct from `Config::Parse`, which copies
the input and enforces the configuration table capacity.

`Game/TweakValue.h` names the pointer-backed float, int and bool values
`TweakFloatBinding`, `TweakIntBinding` and `TweakBoolBinding`, with
`TweakBindingBase` owning their shared binding operations. `TweakValueBool`
stores its value directly; `Game/TweakCallback` stores a callback and
registers it in the shared callback list. These are descriptive names
for the retained hierarchy, not recovered debug identifiers. The zero
literal used by `TweakBindingBase::Bind(path)` belongs to TweakValue at
`0x806E62D0..0x806E62D4`; the following four bytes are alignment padding.

`Game/TweakFileLoader`, `Game/GameTweaksManager` and
`Game/Render/StadiumTweaks` name the asynchronous configuration loader,
game tweak lifetime manager and stadium bindings by their retained
behavior. Their shared declarations use the same C++ types as their
consumers, including the terrain constructor and destructor.
`Game/TweakValueInt.h` and `Game/TweakValueFloat.h` declare the generic
owned numeric values used by the registry, gameplay and graphics.
These are descriptive reconstruction names, not recovered debug
identifiers or original source filenames.

`Game/TrophyInfo` provides separate cup-series and online cup-persona
trophy names. The persona localization keys and its paired stadium
table distinguish the persona ID from the arenas selected for
tournament games. `CupInterface::GetCupPersona` and the tournament
message fields follow these retail uses; their names are descriptive
reconstructions. `AsyncLoadingManager::LoadTrophyTemplates` restores
the retained cup and online-tournament loading branches through the
shared NPC and trophy interfaces. The predecessor trophy API has
different retained types and behavior.

`NPCManager` loads animation, hierarchy, texture and model resources
asynchronously into persistent or transient template collections.
The retained `PersistentResourcePool` string and loader calls identify
the persistent pool exposed by `AsyncLoadingManager`. The loading
callbacks, completion flags and collection names describe these retail
uses; the predecessor has a different fixed template table. Applicable
predecessor names and the shared `ChainChomp` constructor and collision
callback interfaces are preserved.

`Game/Render/DiddyBanana` is identified by the NPC template requested
by its loading consumer and its zip/unzip animation pair. The Birdo
egg owner now shares `BirdoEggObject` with its physics and drawable
consumers, replacing their separate partial layouts. Retail field
accesses establish the orientation, radius transition, lifetime,
shooter and velocity members. Its Show/Hide diagnostics and loading,
collision and rendering uses identify the ordinary C++ methods. The
filenames and otherwise unavailable identifiers are descriptive
reconstructions; these Wii objects have no applicable predecessor
implementation. BirdoEgg static-pool and literal ownership remain
incomplete.


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

`NL/InflateStream.cpp` contains the incremental inflate wrapper at
`0x802A99D8..0x802A9B8C`. Its stream layout and allocator callbacks use the
existing zlib 1.2.2 declarations. `NL/nlCompressedFile.cpp` contains the
asynchronous compressed-file loader at `0x802B3C28..0x802B41B4`: it reads the
uncompressed size, alternates two input buffers, and delivers the completed
output through the shared file callback. The effects, character, stadium,
crowd and NPC loaders share its C++ declaration and allocation type. These
filenames, classes and operation names are descriptive reconstructions;
the stripped executable does not preserve their original spellings.

`Game/Render/HomeButtonFade.cpp` includes the static initializer at
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

`Game/FE/feScrollBar.cpp` reconstructs the scroll control at
`0x8022F710..0x80231118`. Its two `FEButtonPointerRegion` members occupy
`0x4C..0x1B4`; array construction, destruction, callback contexts, and the
independent scene consumers establish their layout. The callback records at
`0x8051D67C..0x8051D6AC` identify four member functions, and the functor
vtable and three following methods use the shared `Function2`/`BindExp3`
implementation. The next function destroys a containing scene, not this
control. `FEScrollBar` and its method names describe the reconstructed behavior.
The predecessor's
`FEScrollText` is a different control and is not a source donor here. Binding
emission, finder register allocation in the range setter, and literal ordering
remain unresolved; the unit is not source-linked.

The shared `FEFinder` implementation uses the predecessor's `InlineHasher`
argument objects and inline finder structure. R4QE01's string constructor
at `0x801CA5A8` hashes with `nlStringLowerHash`, and its integer constructor
at `0x801CA5E0` stores the supplied hash. Their inline definitions and the
argument-object finder are available through `Game/FE/feFinder.inl`.
The scroll control's finder at `0x802308D0` and the online scene consumers
expand this implementation, while `ButtonComponent::SetState` retains calls
to both constructors and the shared finder instances under the same compiler
flags. The declaration/implementation separation represents this observed
visibility distinction, not recovered original header filenames. Recursive
descent still calls the shared implementation at `0x803068F8`; no local
recursive finder is instantiated.

`Game/FE/feScrollText.h` uses the predecessor's `FEScrollText` name and
applicable API and field names. The R4QE01 control at `0x801E2E30` retains
the wide-string message, text metrics, font and completion callback, with a
different 0x40-byte layout and additional scroll-axis, direction and loop
state. Its message setters at `0x801E3A88` and `0x801E3B60` use
`BasicString<unsigned short>`, and its updater moves the text instance's
position. The Striker Times headline and body values are also wide strings,
as shown by the cup-news formatting calls. The predecessor's larger text
buffer and original field offsets are not used for this Wii layout.

`NL/blowfish.cpp` retains the constructor, block encipher, key initialization
and padded encoding paths from Jim Conger's C++ conversion, including its
output-length helper. The source archive SHA-256 is
`62cb58180760d91bbf8876c66ca7e41150d0bc054dbaf8bfeb1fd1596df4b2f3`.
R4QE01 uses the project's aligned allocator and retains the donor's DCBA
bitfield order on PowerPC. The key packing, S-box byte selection and padding
behavior follow the game; they are not changed to a different Blowfish
implementation's byte order. Unretained decryption/destruction bodies are
not imported. The original source attribution is preserved.

`Game/Render/tu_801B43F8.cpp` reconstructs the movable NPC used by the
`CollisionWindDebrisPlayer` event. Its constructor, virtual table, collision
callback and independent `NPCManager` allocations establish a `0x98`-byte
object derived from `SkinAnimatedMovableNPC`. The shared base-class names
follow the predecessor; the derived class and unidentified methods retain
address-based names because no applicable predecessor class was found.
The player-event payload holds this NPC, rather than a `PhysicsObject`.
The physics queue's `CollisionDebrisBall` event carries the NPC pointer itself,
without a separately allocated payload.

`Game/Render/tu_801B532C.cpp` supplies the descriptor accessor and the
three retail records for `CowDebris`, `CatfishDebris` and `TractorDebris`.
The records contain the model name, physics radius and two values passed to
the NPC constructor; the first value is subsequently passed to `PlaySound`.
This descriptor unit is source-linked. The NPC implementation is not:
boolean forwarding, update code generation and object layout remain
unresolved. Its two vector constants still use the existing definitions in
`unclassified/tu_801B298C.cpp`; their original definition ownership has not
been established. The address-based source partition does not identify
original filenames. The trailing initializer at `0x801B523C` remains
unreconstructed: its guarded static storage is visible in retail, but its
original shared declaration and instantiation trigger are not established.

`Game/SH/SHPause.cpp` reconstructs the pause scene at `0x80239454` through
`0x8023B144`. The predecessor's `SHPause.cpp` supplies the `PauseMenuScene`,
quit-confirmation, transition, and static-state names where the Wii behavior
retains those operations. The Wii scene replaces the predecessor's menu-list
storage with seven pointer-input components and has no constructor context
argument. Retail constructor stores, callback families, and the vtable at
`0x8051DDCC` establish the `0x594`-byte layout and this partition. The remaining
callbacks retain address-based names. The source is not yet source-linked.

The online hub, friends list, and matchmaking draft screens occupy
`0x8023B144..0x8023E9F4`, `0x8023E9F4..0x802441B4`, and
`0x802441B4..0x80245DB4`. Their `SHOnlineHub`, `SHOnlineFriends`, and
`SHOnlineMatchmakingDraft` names describe the retail menu actions, localization
keys, and `FE_MATCHMAKING_DRAFT` audio context. These are descriptive source
names, not recovered debug identifiers. `SHOnlineInvitePlayers` keeps its
existing class identity under the corresponding source and header paths.

The friends, draft, and invitation screens share a `0x98`-byte
`FEOnlinePlayerRow`. Its reset stores and renderer establish the name, Mii data,
friend index, search/status selections, ranking metadata, captain and side,
visibility, guest, and cancel fields. The renderer belongs to this shared
interface. The component-root default finder at `0x80244158` uses the common
`FEFinder` template. Mii texture lookup and manager access at `0x80244080` and
`0x802441AC` use the same state owned by `MiiManager`; their inline definitions
are supplied through its implementation header. These interfaces preserve
the existing calling conventions and layouts. The scene units remain
`NonMatching` while their other code and object differences are unresolved.

The invitation callbacks and `SHOnlineLogin` use names describing their
pointer actions, lobby slots, login stages, and error handling. The login
scene at `0x80260990..0x80261940` registers the session's two-result listener
and uses the `FE_LOGIN` audio context. Its error-range table and the displayed
error code are shared through `Game/FE/feOnlineError.h`. The four-machine
friend-lobby flag follows the lobby's connection-capacity selection and the
invitation screen's host-start control. These are descriptive names based on
retail behavior; existing layouts and data ranges are retained.

The GameInfo accessor at `0x80077224` reads the existing singleton pointer.
It uses the common `nlSingleton<T>::GetInstance` definition, with no separate
accessor body. The implementation header is visible before derived-type
instantiation in the inline consumers and after the navigation scene's
methods, preserving its retained call under the configured compiler.

`SHNavigation` owns the literal pool at `0x806DE828..0x806DE907` and pointer
visibility state at `0x806E18F0`. Its slide names, button text identifiers,
and eight door names are ordinary string literals. Retail references and
null terminators establish their complete extents; the final byte before the
next pool is alignment padding. The previously split suffixes have no retail
references. Their apparent users were superseded renderer sources whose
small-data addresses did not use the retail r13 base. The configured
`GXMaterialProgram` implementations already own the corresponding state.

Navigation's button helpers use the shared argument-object finder with inline
hash constructors. The retail code retains a zero-level descent check in these
calls, while `SceneCreated` retains out-of-line constructor calls. Definition
visibility and file-level inlining preserve both forms. Literal first use also
places the home-button warning definition after the button-text helpers.
The text lookup at `0x802281EC` has a shared `FEFindTextInstance` declaration
used by the timer and online invitation preview. This descriptive interface
does not establish the original template identity of that retained entry.
Navigation's owned data matches; its remaining code differences keep the unit
`NonMatching`.

`SHOnlineInvitePreview` names the `0x3F8` scene at `0x8025BE74` that reads the
host invitation payload and displays six gameplay options. Its continue-button
callbacks, preview delay, text buffers, and literal names follow those retail
uses. The class has its own header; its layout and retained ranges are unchanged.
The wide-string formatter at `0x801CFE3C` follows the shared `Format` template:
it converts a pointer argument, replaces the numbered brace placeholder, and
returns the formatted string. The preview now uses that existing template with
`const unsigned short*`, replacing its local C-linkage declaration. This names
the interface without claiming an exact formatter body or a matching preview
object.

`SHOnlineInviteStatus` owns the `0x10C` invitation-status scene at `0x80262B54`
and scene ID 53. Its callers set the lobby, cancellation, or decline status
and return delay; the scene manages cancellation, pointer setup, and connection
error dismissal. The owner header exposes those fields directly to its callers.
The error callback descriptor at `0x80520640` contains the normal three-word
member-function pointer; the following four zero bytes are alignment padding.
The naming and header changes preserve the existing function bytes and data
ranges.

`Game/FE/feScrollText.h` restores the predecessor's `FEScrollText` name and
applicable message, update, metrics, and text-instance interfaces. The retail
methods at `0x801E2E30..0x801E45C0` establish a smaller Wii layout: a wide
`BasicString`, axis and direction controls, scrolling modes, offset, speed, and
the message-finished callback occupy `0x40` bytes. The constructor's halfword
copy and the message setter's wide-string assignment establish the string
type independently. `SHStrikerTimesBase` passes its wide headline string to this
interface. New Wii-specific method and field names describe their retail
roles; they are reconstruction names. The header restoration does not change
the containing aggregate's translation-unit boundaries or matching status.

`SHStrikerTimesBase` names the shared Striker Times scene at
`0x80269078..0x8026B10C`. Its resource path, localized logos, headline and
story pages, and game-summary presentation establish the role. Its derived
scenes use separate Done and Back completion hooks after the out slide.
The class, page state, callbacks, image and text fields, and literal names
describe those retail uses; the original class and file names are unavailable.
The rename preserves the existing layout, retained functions, data ranges,
and translation-unit boundaries.

`NL/gl/glMaterialProgram.h` describes the common 0x10-byte material program
interface used by startup, model loading, parameter lookup, and rendering.
These independent callers establish the seven virtual operations and the
shared hash, parameter-data size, and parameter count. `GLMaterialProgram`
is a descriptive reconstruction name. The lighting and skin-matrix helpers
under `NL/glx` likewise use behavior-supported filenames; the applicable
`glx_LoadDirectionalLight` and `glx_LoadSpecular` names follow the predecessor,
with the Wii argument and cache layouts established from R4QE01.

The repository's [CC0 license](../LICENSE) applies only to contributions whose
authors have the right to make that dedication. Nintendo, Next Level Games,
and third-party names, trademarks, game content, and other proprietary
material are not granted or licensed by this repository.

`cCameraManager::GetCamera<T>` in `Game/Camera/CameraMan.h` follows the
predecessor header's template, including its identifiers and circular-list
traversal. R4QE01's weather callback at `0x800AC6D8` embeds that lookup and
writes the gameplay camera's zoom override.

`Game/FE/feCupFlow.cpp` names the shared cup-page navigation, back-menu,
restart, news, award and trophy-visibility interfaces at `0x80206CE4..0x80209584`.
The retail scene and popup tables identify the associated standings, schedule,
Golden Boot, Brick Wall, rules and reward screens. Its team-slide helper returns
`"user"` in retail; its callers supply the team category. CupManager supplies
the record restoration, cup-series restart and round-news operations through
C++ member declarations. These file and operation names are descriptive
reconstructions, not identifiers recovered from debug information.

`BasicStadium` inherits `World` at offset zero. Its retail allocation at
`0x80276674` is `0xAC` bytes, and its constructor at `0x80278A2C` initializes
the base, a second drawable registry, the shadow light position and height,
time, and stadium/megastrike high-range tweak pointers. Its vtable at
`0x805223D0` distinguishes the inherited two-argument `Update` from the
three-argument stadium overload. Shadow consumers use this shared class layout.
The tournament-trophy loading interfaces at `0x80276D10..0x80276F5C` and
`SetWorldAnimation` at `0x80277BB4` use C++ declarations; the latter takes
two names and an `ePlayMode`. The World loading and effect-triggering calls
also use their actual member declarations. Operation and field names added
without a predecessor counterpart are descriptive reconstructions.

`Game/Render/StadiumLoading.cpp` names the reconstructed loading range at
`0x80276724..0x80276D10`. Its canonical header also declares the associated
resource callbacks, drawable lookup and stadium lifecycle interfaces. The
retail table at `0x80521F68` identifies 22 model resources, their instance
counts and captain/sidekick/stadium load conditions. Loaded data, sizes,
completion flags, effect requests and double buffers retain their existing
storage and ownership. These operation, file and data names describe retail
behavior; they are not recovered debug names. Existing split boundaries
and the source-link status are unchanged.
