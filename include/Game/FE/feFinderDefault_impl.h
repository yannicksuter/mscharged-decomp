#ifndef _FEFINDER_DEFAULT_IMPL_H_
#define _FEFINDER_DEFAULT_IMPL_H_

#include "Game/FE/feFinder.h"

template <typename T, int N>
template <typename U>
inline T* FEFinder<T, N>::FindOrDefault(U* pTopLevel, InlineHasher Level1, InlineHasher Level2, InlineHasher Level3,
    InlineHasher Level4, InlineHasher Level5, InlineHasher Level6)
{
    T* pResult = FindChecked(pTopLevel, (unsigned long)Level1, (unsigned long)Level2, (unsigned long)Level3, (unsigned long)Level4, (unsigned long)Level5, (unsigned long)Level6);
    return pResult == 0 ? (T*)FEGetDefaultInstance((eTimeLineAssetType)N) : pResult;
}

#endif // _FEFINDER_DEFAULT_IMPL_H_
