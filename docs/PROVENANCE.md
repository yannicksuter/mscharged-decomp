# Source provenance

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

`unclassified/tu_801FE81C.cpp` contains the adjacent frontend stadium-loading and
camera operations. Its separate state, constant pools and initializer follow the
retail layout; the address-based filename does not identify an original source
filename. The reference project's licensing notice above also applies to the
shared source ancestry.
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
