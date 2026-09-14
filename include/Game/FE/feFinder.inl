#ifndef _FEFINDER_INL_
#define _FEFINDER_INL_

#include "Game/FE/feInlineHasher.inl"
#include "Game/FE/feFinder_impl.h"

inline TLTextInstance* FEFindTextInstance(TLInstance* pTopLevel, InlineHasher Level1,
    InlineHasher Level2, InlineHasher Level3, InlineHasher Level4,
    InlineHasher Level5, InlineHasher Level6)
{
    return FEFinder<TLTextInstance, TLAT_TEXT>::FindChecked(pTopLevel,
        (unsigned long)Level1, (unsigned long)Level2, (unsigned long)Level3,
        (unsigned long)Level4, (unsigned long)Level5, (unsigned long)Level6);
}

#endif // _FEFINDER_INL_
