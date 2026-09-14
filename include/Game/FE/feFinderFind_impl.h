#ifndef _FEFINDER_FIND_IMPL_H_
#define _FEFINDER_FIND_IMPL_H_

#include "Game/FE/feFinder.h"

template <typename T, int N>
template <typename U>
inline T* FEFinder<T, N>::Find(U* pTopLevel, InlineHasher Level1, InlineHasher Level2, InlineHasher Level3,
    InlineHasher Level4, InlineHasher Level5, InlineHasher Level6)
{
    return FindChecked(pTopLevel, (unsigned long)Level1, (unsigned long)Level2, (unsigned long)Level3, (unsigned long)Level4, (unsigned long)Level5, (unsigned long)Level6);
}

template <typename T, int N>
inline T* FEFinder<T, N>::Find(TLSlide* pTopLevel, const InlineHasher& Level1, const InlineHasher& Level2,
    const InlineHasher& Level3, const InlineHasher& Level4, const InlineHasher& Level5, const InlineHasher& Level6)
{
    return Find(pTopLevel, (unsigned long)Level1, (unsigned long)Level2, (unsigned long)Level3,
        (unsigned long)Level4, (unsigned long)Level5, (unsigned long)Level6);
}

#endif // _FEFINDER_FIND_IMPL_H_
