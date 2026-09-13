# Source provenance

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
