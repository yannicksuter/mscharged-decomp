# Source provenance

`Replay::Record` in `Game/Replay.h` follows the frame-chain construction in
[Super Mario Strikers' replay implementation](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/include/Game/Replay.h).
R4QE01 establishes the additional per-frame state word and the Charged frame
pool layout used by this implementation.

`Game/GoalieTweaks.cpp` reconstructs the initialization at R4QE01 address
`0x800278C8` from its binding calls, member offsets, strings and float constants.
It uses the shared `TweakFloatBinding` API, preserves the repeated jogging-speed
binding, and initializes maximum shot fatigue to zero before binding the
remaining settings.

`NL/nlIntersection.cpp` reconstructs the two-dimensional segment intersection
routine at R4QE01 address `0x802B6BC8`. Its determinant arithmetic, tolerance,
output parameters and polygon/wall callers establish the API and vector width.
The file and function names describe that behavior; original Charged spellings
are unknown.

`Game/Render/TimedObject.cpp` reconstructs the timed-object manager at
`0x80188884`: construction registers an enabled object with zero elapsed time,
the render task advances enabled objects until expiry, and the manager owns
the objects and list nodes. `TimedObject`, its manager/list and member names
describe those R4QE01 behaviors; original Charged spellings are unknown.

`Game/AI/Scripts/FuzzyAIRuntime.cpp` reconstructs the 305-case R4QE01
interpreter dispatcher from its jump table, stack operations, and called APIs.
The stack-based call convention is also present in the predecessor's
[generated interpreter](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/ReplayChoreo_interp.cpp).
`Variant::GetPlayer` preserves the predecessor's
[accessor name and field](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/include/Game/AI/Variant.h);
`GetTeam` exposes the corresponding team field used by R4QE01's runtime.
These source names do not establish original Charged spellings.

The light setup in `NL/glx/glxLight.cpp` follows the GX lighting operations in
[Super Mario Strikers' glxSend](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/NL/glx/glxSend.cpp).
R4QE01 supplies the indexed light cache, directional scale, point attenuation,
and retained interfaces. `nlFloatColourToByte` retains the predecessor's
[colour-conversion API name](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/include/NL/nlColour.h),
with Charged's truncation after multiplication by 255 instead of the
predecessor's clamped 255.5 conversion. The reference project's licensing
notice below also applies to this ancestry.

`RVL_SDK/hbm/nw4hbm/db/db_DbgPrintBase.cpp` uses the unreferenced helper from
[Twilight Princess's HBM reconstruction](https://github.com/zeldaret/tp/blob/c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0/libs/revolution/src/homebuttonLib/nw4hbm/db/db_DbgPrintBase.cpp)
to emit the shared `nw4hbm::ut::Color` destructor at its retained link position.
This is a dummy emitter; the original stripped debug-print implementation has
not been reconstructed. The helper is discarded by the linker, leaving only
the 64-byte destructor used by other HBM objects.

The Home Button Menu `RemoteSpk::spInstance` ownership and spelling follow
the corresponding [Kirby's Epic Yarn HBM source](https://github.com/Swiftshine/key/blob/904ed70ca1db76e2bf4dbde1d2a5c4adbda9bbb4/src/hbm/homebutton/HBMRemoteSpk.cpp).
R4QE01's speaker constructor, destructor and callbacks establish the singleton's
lifetime and uses; the controller and speaker small-data sections align separately.

`NL/plat/nlFileCache.cpp` and its header reconstruct R4QE01's asynchronous
NAND file cache from the cache diagnostics, file callbacks, task registration,
and request state transitions. `nlFileCache`, the public file-cache functions,
and the record and callback names describe those observed roles; their original
Charged spellings are unknown. `g_bDisableAllFileCaching` and `FileCacheTask`
survive in retail strings. Fields and request states without an established
meaning retain neutral names.

`Game/SH/SHStadiumSelect.cpp` reconstructs the R4QE01 stadium-selection
scene from its presentation transitions, stadium database calls, pointer
callbacks and preview-movie control. `StadiumSelectScene`, `MoviePlayerControl`
and their operation names describe those observed roles. The file name,
`STADIUM_ORDER`, `OnSelectStadium`, `mStadiumIndex`, `m_pTicker` and matching
movie-field names follow the corresponding Super Mario Strikers interfaces.
The Wii scene layout, 17-entry display order and movie state machine follow
R4QE01; these names are not claims about original Charged symbols.

`Game/Weather.cpp` and `Game/Weather.h` reconstruct R4QE01's weather manager
and stadium weather effects. The weather class names and serialized field
spellings survive in synchronization-logging strings. Manager, operation and
tuning-variable names describe the observed behavior; their original source
spellings are unknown.

The spatial, interception, possession and timing questions in
`Game/AI/Scripts/ScriptQuestions.cpp` use
[Super Mario Strikers' script questions](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/AI/Scripts/ScriptQuestions.cpp)
as an ancestry reference. Charged's cached intercept positions, controller-aware
facing, power-up availability, shot and pass obstruction weights, skill-shot
cases and tweak registration follow R4QE01 rather than predecessor offsets or
state IDs. `Goalie::IsBusy` and `IsRecovering` retain the predecessor interface
with Charged's fire-timer condition. The target-ball field of
`DesireRunToTarget` follows its variant initialization and ball-owner accesses.
New accessors expose these existing fields; their spellings are not claimed as
original Charged symbols.

`cCharacter::GetPosition`, `cPlayer::HasBall` and the vector-form defensive-zone
query retain their predecessor interfaces. The formation-position overloads
follow Charged's position-taking query and its single-player wrappers.
Shot-meter time and duration accessors expose the existing R4QE01 fields; the
charging predicate follows the shared ACTIVE/STS_ACTIVE test in player input
and user-control processing, excluding the transition state. Planar net-post
construction and vector-to-angle evaluation use the existing math interfaces.

ScriptQuestions' retained definition order and confidence-range placement follow
R4QE01's code and interleaved literal/aggregate sequence. With the configured
GC/3.0a5 compiler, ordinary inlining plus file-level IPA preserves that sequence
and the expansions of later-defined queries. This is a reconstruction of this
translation unit's emission behavior, not identification of the original
compiler options for the game. Unused predecessor-only confidence storage is
not retained.

`Game/Physics/PhysicsWaluigiWall.cpp` reconstructs the Charged-specific Waluigi
wall ability from R4QE01's collision dispatch, segment geometry, event
callbacks and effect strings. `PhysicsWaluigiWall`, `WaluigiWallManager`, their
member names and the tuning-variable names describe the observed behavior;
they are not claims about original source spellings. Address suffixes remain
on damage settings whose triggers are not yet established.
The shared `AvoidablePolygon::Update` geometry is used by both its existing
constructors and the wall's endpoint update. Its 2D negation, rotation and
scale-add helpers, and the wall's vector-projection helper in `NL/nlMath.h`,
reconstruct the corresponding R4QE01 arithmetic; their spellings follow the
existing vector API rather than recovered symbols. The matrix-owned `GetRow_`
accessors expose the existing row storage, complementing `SetRow_`, and are
used for initial wall placement and subsequent endpoint updates.
The `cBall::GetPosition`
accessor follows
[Super Mario Strikers' ball interface](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/include/Game/Ball.h),
with the Charged field layout.

The bounding-box helpers in `Game/Drawable/DrawableModel.cpp` retain the
corresponding component, function and type names from
[Super Mario Strikers' DrawableModel](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/Drawable/DrawableModel.cpp).
R4QE01 instead traverses packet vertex indices directly, looks up the position
stream by ID, and decodes its compact coordinates as signed bytes scaled by
1/128. The const `glModelPacketGetStream` overload names the retained
stream-by-ID lookup using the existing model API terminology; its original
Charged spelling is not known.

The player animation, ball possession, controller selection and head-tracking
routines in `Game/Player.cpp` use the corresponding
[Super Mario Strikers player implementation](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/Player.cpp)
and shared headers as ancestry references. Player state layout, sixteen input
timers, power-up events and camera-flash interactions follow R4QE01. The
reference project's licensing notice below describes its source origins and
applicable rights.

The navigation, save blending, passing and loose-ball decisions in
`Game/Goalie.cpp` use the corresponding
[Super Mario Strikers goalie implementation](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/Goalie.cpp)
and shared math and animation interfaces as ancestry references. R4QE01
determines the retained state layout, collision responses, Mega Strike and
Striker Challenge behavior, event payloads and synchronization fields. The net
dimension accessors follow the predecessor's
[shared net interface](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/include/Game/Net.h),
including the object-receiver calls used by its fielder collision response. The
reference project's licensing notice below describes its source origins and
applicable rights.

The popup layout and string-formatting operations in `Game/FE/fePopupMenu.cpp`
use the corresponding
[Super Mario Strikers popup implementation](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/FE/fePopupMenu.cpp)
and shared string headers as ancestry references. The popup table, pointer
callbacks, network messages and allocation behavior follow R4QE01. The reference
project's licensing notice below describes its source origins and applicable
rights.

The recursive instance walk in `Game/FE/tlSlide.cpp` and
`TLInstance::GetType` follow the corresponding
[Super Mario Strikers slide implementation](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/FE/tlSlide.cpp)
and [instance accessor](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/include/Game/FE/feImage.h).
R4QE01 determines the retained recursion, time advancement, play-mode dispatch
and slide initialization. The reference project's licensing notice below
describes its source origins and applicable rights.

`FEPresentation::SetActiveSlide` retains the hash overload from the
[Super Mario Strikers presentation interface](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/FE/fePresentation.cpp).
The string overload delegates its hash lookup to that operation. The boolean
reset argument and conditional timer reset follow R4QE01. The reference
project's licensing notice below describes its source origins and applicable
rights.

The rematch actions in `Game/SH/SHPausePostGame.cpp` use the corresponding
[Super Mario Strikers routines](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/SH/SHPausePostGame.cpp)
as an ancestry reference. The network state, timer, statistics layout and article
formatting follow the R4QE01 executable.

The `FERender` component and slide rendering helpers, and its current asset-colour
calculation, follow the corresponding routines in
[smstrikers-decomp at c62fc6a0](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/FE/feRender.cpp).
The Charged implementation passes an explicit transformation matrix, as required
by R4QE01's renderer. The reference project's
[licensing notice](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/LICENSE)
describes its source origins and applicable rights.

The HUD overlay retains corresponding frontend layouts and operations from
[smstrikers-decomp's HUD overlay](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/FE/Overlay/OverlayHandlerHUD.cpp),
with Charged's component organization, assets and behavior reconstructed from
R4QE01. The `DoubleHighlite` and `SingleHighlite` declarations in `feHelpFuncs.h`
retain the predecessor's
[shared frontend header](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/include/Game/FE/feHelpFuncs.h)
surface, including its slide-name constants. Function declarations are available
separately in `feHelpFuncs_decl.h` for callers that do not use the header static
storage. The reference licensing notice above also applies to this source
ancestry.

The goal overlay uses the corresponding
[Super Mario Strikers goal overlay](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/FE/Overlay/OverlayHandlerGoal.cpp)
as an ancestry reference for score, clock, highlight and winner text, including
the sniper-trophy query. Event callbacks, Mega Strike scoring, series results,
Striker Challenges and cup selection follow R4QE01. The float `std::ceil` and
`std::floor` overloads retain the predecessor's C++ math interface and use the
double C routines with the rounding conversion observed in R4QE01.
The shared wide-string `LexicalCast` copy specialization follows the
predecessor's `NL/nlLexicalCast.h` and the retained R4QE01 reference-counted copy.

`Game/SH/SHNetworkStart.cpp` reconstructs the LAN lobby menu from R4QE01's
presentation strings, listener callbacks, controller input and ranking payloads.
Its menu and callable interfaces use the corresponding Super Mario Strikers
`Game/FE/feMenu.h`, `NL/nlBind.h` and `NL/nlFunction.h` as ancestry references.
The bound callback values, function-memory allocation and retained member layouts
follow R4QE01. Pointer-listener consumers construct their bound member callables
directly; the menu callbacks use the generic `Bind` wrapper.

`Game/FE/feCamera.cpp` owns the staged 3D front-end world load and the
presentation-camera stack helpers at `0x801FE81C..0x801FEBCC`. Async-loading
step labels identify the begin and finish operations; the FE environment and
tweak paths, stadium loaders, model manager and lighting setup establish the
world lifecycle. Presentation and Hall of Fame callers establish the camera
operations. `SetGameObjectLightTexture` names the paired setter for the
existing getter and its shared storage. These names describe the observed
behavior and do not claim original Charged spellings. The reference project's
licensing notice above also applies to the shared source ancestry.
Cup scheduling and advancement in `Game/DB/GameProgress.cpp` use the
corresponding `GameInfoManager` routines from
[Super Mario Strikers](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/GameInfo.cpp)
as ancestry references, together with its cup and statistics interfaces.
Charged's cup phases, three-sidekick records, packed history and challenge-unlock
mappings follow R4QE01. The reference licensing notice above also applies
to this source ancestry.

`Game/Render/MegastrikeBackgroundOverlay.cpp` reconstructs R4QE01's full-screen
Mega Strike background fade. The render layer, character texture format,
team-indexed character lookup and intensity arithmetic establish its role.
The source, class, global and field names describe that behavior; they do not
claim original source spellings. Character data uses the existing shared
`cCharacter`, `CharacterInfo` and `g_pCharacters` interfaces.

`Game/Render/ShadowVolume.cpp` reconstructs R4QE01's shadow-volume target,
model submission and full-screen blend. Its descriptive source and API names
follow the retained render-target strings, raster states, view-layer callers
and mesh-writer streams; they do not claim original source spellings.

`Game/Font/FontLoading.cpp` reconstructs R4QE01's localization and frontend,
in-game and Strikers 101 font loading. The helper and local names follow the
corresponding [Super Mario Strikers transition code](https://github.com/yannicksuter/smstrikers-decomp/blob/c62fc6a0ed90f4eacc8cca9b231f1d4aaf16ad0b/src/Game/TransitionTask.cpp);
font resource paths and loading checks follow R4QE01. Each four-byte font
language code is copied from its default record before per-call adjustment;
the separate records and their byte copies follow the retained R4QE01 accesses.
The Strikers 101 mode field name follows the corresponding predecessor GameInfoManager field and
its retained Charged uses. The loading API and source filename are descriptive
names, not claims about original Charged spellings. The reference licensing
notice above also applies to this source ancestry.

The snapshot replay traversal and `FloatCompressor` implementation use the
corresponding Super Mario Strikers `Game/RenderSnapshot.h`,
`Game/Replay.h` and `Game/Compressor.h` as ancestry references.
Charged's drawable field order, intervals, scalar ranges and quaternion packing
follow R4QE01's paired save/load routines. The quaternion proxy's name is a
neutral reconstruction; no original Charged type spelling is claimed.
The reference licensing notice above also applies to this source ancestry.

`Game/FE/Overlay/OverlayHandlerPIP.cpp` reconstructs R4QE01's frontend
picture-in-picture overlay. The `target/pip` texture, aspect-ratio slides and
NIS camera countdown establish its role and the descriptive class and field
names. NIS overlay state zero selects this display mode; the mode query retains
the unsigned Boolean result consumed by the overlay. These names describe the
retained behavior and do not claim original Charged spellings.


DrawableBall's shared replay packet retains the predecessor's position/velocity
compression sequence. Its packed flags, scale, trail count and conditional trail
positions/orientations follow the paired R4QE01 save/load bodies.


DrawableCharacter's replay traversal and the polymorphic pose-pointer stream
follow the corresponding Super Mario Strikers `DrawableCharacter.cpp`,
`Replay.h` and `LoadFrame.h`. R4QE01 supplies the additional character flags,
mega-form packet, head-angle packing and pose-accumulator path. The accumulator
packet retains R4QE01's quaternion, translation and scale loops, including its
use of translation identity flags to gate scale values. The reference licensing
notice above also applies to this source ancestry.

The pose replay dispatch and node serializers follow the predecessor's
`DrawableCharacter.cpp`, `PoseNode.h` and `Game/SAnim` node headers. R4QE01
establishes the fifth pose type (`cPN_8030E550`), the animation controller's
packed mirror flag, and the value/reference distinction between SaveFrame and
LoadFrame pointer transfers. The node packet stores the child count and
recursively visits each child before its type-specific fields. The reference
licensing notice above also applies to this source ancestry.

`NL/nlPolygonRegion.cpp` reconstructs the vertex allocation, bounding-sphere
update and two-dimensional containment routines at `0x802B68E8..0x802B6BC8`.
Its descriptive class and method names follow those operations and the polygon
constructed by the player effect at `0x80098AA0`; they do not claim original
Charged source spellings. The unused sixteen bytes at object offset `0x10`
remain unidentified.

`Game/FE/Overlay/OverlayHandlerMegaStrikeMeter.cpp` reconstructs the frontend
Mega Strike meter display. Its event callers identify the start and first/second
meter results; the numbered slides and position offsets establish the display
fields. The descriptive names do not claim original Charged spellings.

`Game/FE/Overlay/OverlayHandlerSuperAbility.cpp` reconstructs the Super Ability
announcement. Its activation caller supplies the team, and its resource/image
paths, text component and countdown establish the descriptive class, field and
global names. These names do not claim original Charged spellings.

`Game/SH/SHStrikerTimesChallenge.cpp` implements the Striker Times challenge
introduction and result scene. Its base class, challenge article keys,
asynchronous settings load and popup option/callback mapping establish the
descriptive scene, field and callback names. They do not claim original
Charged spellings.

`Game/Render/YoshiEggObject.cpp` implements the Yoshi egg gameplay object.
Its PhysicsYoshiEgg member, DrawableYoshiEgg consumer, activation caller and
`yoshi_egg_trail` effect establish the descriptive class and method names.
Collision-normal/depth inputs identify its pending displacement, while radius
transitions and rolling/height calculations identify the remaining fields and
scalars. These names do not claim original Charged spellings.

`Game/Render/KoopaShellObject.cpp` implements the Koopa shell gameplay object.
Its physics and drawable consumers, activation callers and `koopa_shell_trail`
effect establish the descriptive class, method and parameter names. The
initial velocity and hidden position constants follow their constructor and
deactivation uses. These names do not claim original Charged spellings.
The adjacent retained Jumbotron stubs and singleton live in
`Game/Render/Jumbotron.cpp`; their interface and layout follow the predecessor,
while their empty bodies follow R4QE01.

## Controller map overlay

`Game/FE/Overlay/OverlayHandlerControllerMap.cpp` reconstructs R4QE01 code at
0x801FA28C-0x801FADB4. The controller-map resource, localized texture paths,
Done-button callbacks and pointer input identify the overlay and its fields.
The descriptive class and member names are reconstruction names. Callback
ownership uses the existing NLG Function/Bind implementation.

`Game/Camera/noisefilter.cpp` reconstructs the camera noise filter at
`0x800EF9F0..0x800F0240`. Its integer noise samples, cosine interpolation,
amplitude decay and view-matrix translation establish the descriptive filter,
operation and field names. Camera-manager and replay-choreography callers
establish filter slot one and the embedded object. These names do not claim
original Charged source spellings.

## Sound cue handle

`Game/Audio/XSoundCueHandle.cpp` reconstructs the cue playback handle at
`0x802F1758..0x802F2188`. The constructor diagnostic string preserves the class
name `XSoundCueHandle`; its base constructor, virtual table, callers and sound
instance operations establish the interface and descriptive member names.
`Release` at `0x800ED8C8` remains with its existing source input. Other member
and field names are reconstruction names, rather than recovered source symbols.

The RPC controller and sound instance share the node pool at `0x8057FA10`.
Their construction, insertion and cleanup paths establish the borrowed-pool
list type in `Game/Audio/RpcList_802EFB70.h`, using the existing NLG list and
allocator templates. The shared entry-return helper is at `0x802F076C`.

`Game/Physics/PhysicsWall.cpp` follows the predecessor's PhysicsWall class and
plane-construction formula. R4QE01's sideline creation and goalie-plane base
construction corroborate that interface; its object type is `0x17`, compared
with `0x19` in the predecessor.

`Game/Physics/PhysicsTriggerVolume.cpp` follows the predecessor linker map's
`PhysicsTriggerVolume.o` attribution. R4QE01's crowd-riot caller constructs the
sphere as a callback-driven collision volume, establishing the class and member
roles. The co-emitted patch table and its consumers establish the patch type,
effect, collision mask, sound, gravity, friction and bounce names. The
`CollisionChainCrowd` event registration establishes the queue operation used
when the volume, shells or bananas contact Chain Chomp.

`Game/FE/MatchSummary.cpp` reconstructs the reusable match-summary display at
`0x80209584..0x8020A38C`. The game-results and post-game callers, seven statistic
rows and captain-image paths establish its role. The paired player statistics,
`DisplayMatchSummary` operation and column buffers follow the predecessor's
SummaryOverlay ancestry; Charged embeds this separate object without overlay
inheritance. Other class, method and field names describe the reconstructed
behavior and do not claim original Charged spellings.

`NL/nlDebugViews.cpp` owns the two debug rendering view pointers at
`0x806E1DB8` and `0x806E1DBC`. Startup passes layers `eCLV_Debug` and
`eCLV_DebugSquare` to the setters at `0x802B7798` and `0x802B77A8`; the
getter at `0x802B77A0` supplies font, graph and shape rendering consumers.
The square-view setter and private pointer names describe this observed role;
no original Charged source filename or spelling is claimed.

`NL/nlAllocatorStack.cpp` implements the current-allocator push and pop at
`0x8036D6F8` and `0x8036D71C`. The screenshot path brackets its image-buffer
allocation with these operations on `VirtualAllocator`; the stack storage and
initial standard-allocator entry remain in `NL/nlMemory.cpp`. The operation
names describe the retained behavior rather than original Charged spellings.

`NL/plat/nlMemory.cpp` contains the Wii memory-arena setup at `0x80372980`
and the standard-allocator alignment wrapper at `0x80372B30`. The former
initializes the MEM2 and MEM1 allocator regions; the latter supplies the
32-byte alignment used by the graphics FIFO caller. The platform-independent
allocation entry points remain in `NL/nlMemory.cpp`, with their common
interface in `NL/nlMemory.h`.

`NL/nlRegistry.h`, `NL/nlRegistry.inl`, `NL/nlRegistryLookup.cpp` and
`NL/nlRegistryOwner.cpp` reconstruct the dynamic and packed registry family at
`0x802BE64C..0x802BFF14`. R4QE01's list operations, packed type words,
hash-sorted entries, image relocation, allocator callbacks and audio-resource
tree consumer establish the common registry roles. The linked owner unit and
standalone binary search retain their separate translation-unit boundaries.
Names such as `RegistryOwner`, `RegistryIterator`, `MovePrevious` and
`GetValueSlot` describe observed behavior; `AlwaysTrue`, `AlwaysFalse` and
`GetRootAlias` deliberately make no stronger claim about otherwise uncalled
virtual slots. None of these descriptive names claim original Charged
spellings.

`Game/Task/ParticleUpdateCallbacks.cpp` installs the update, render and
pre-update callbacks used by `ParticleUpdateTask`. R4QE01's callback
assignments establish their roles, while the predecessor's particle task
corroborates the task-state and world-render gates. The source, function and
global names describe that retained behavior and do not claim original Charged
spellings.
## MSL integer absolute value

The `_abs` inline helper in `libs/MSL_C/include/stdlib.h` follows the
MSL `arith.h` implementation in [Pikmin 1 Wii](https://github.com/projectPiki/pik1wii/blob/7170ba8a1eafd6dd3e30923c827b20f4a0df735e/include/PowerPC_EABI_Support/MSL_C/MSL_Common/arith.h)
and [Pikmin 2 Wii](https://github.com/projectPiki/pik2wii/blob/8784a059f1c3213ef5adf031fa1bd8e415723ede/include/PowerPC_EABI_Support/MSL_C/MSL_Common/arith.h).
Its conditional expression reproduces the repeated integer
absolute-value evaluations in DesireMegaStrike at R4QE01 `0x800B93C4`.
The separately available compiler intrinsic `__abs` does not reproduce that
caller's evaluation boundaries.

The marking calculations in `Game/AI/DesireMark.cpp` use the predecessor's
`cFielder::DesireMark` as algorithm and naming ancestry. R4QE01 supplies the
variant-based updates, defensive-position transition, multiple-mark loop,
tuning references and script calls. The transition result uses the existing
variant constructor with no duration overrides, preserving its metadata copy.

`Game/AI/DesireShoot.cpp` uses the predecessor's `cFielder::DesireWindupShot`
as ancestry for the ball-loss and shot-meter transition flow. R4QE01 supplies
the variant updates, transition identifiers, character filter, goalie-distance
checks, and `Windup/Skillshot` probability gate.
