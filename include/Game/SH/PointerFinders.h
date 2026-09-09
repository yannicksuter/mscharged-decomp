#ifndef GAME_SH_POINTER_FINDERS_H
#define GAME_SH_POINTER_FINDERS_H

#include "Game/FE/feFinder.h"

TLComponentInstance* FindPointerComponent(TLInstance* instance, InlineHasher level1,
    InlineHasher level2 = InlineHasher(0UL), InlineHasher level3 = InlineHasher(0UL),
    InlineHasher level4 = InlineHasher(0UL), InlineHasher level5 = InlineHasher(0UL),
    InlineHasher level6 = InlineHasher(0UL));
TLComponentInstance* FindOptionalPointerComponent(TLInstance* instance, InlineHasher level1,
    InlineHasher level2 = InlineHasher(0UL), InlineHasher level3 = InlineHasher(0UL),
    InlineHasher level4 = InlineHasher(0UL), InlineHasher level5 = InlineHasher(0UL),
    InlineHasher level6 = InlineHasher(0UL));
TLInstance* FindPointerLayer(TLSlide* slide, InlineHasher level1,
    InlineHasher level2 = InlineHasher(0UL), InlineHasher level3 = InlineHasher(0UL),
    InlineHasher level4 = InlineHasher(0UL), InlineHasher level5 = InlineHasher(0UL),
    InlineHasher level6 = InlineHasher(0UL));

#endif // GAME_SH_POINTER_FINDERS_H
