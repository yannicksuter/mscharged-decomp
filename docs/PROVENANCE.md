# Source provenance

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
