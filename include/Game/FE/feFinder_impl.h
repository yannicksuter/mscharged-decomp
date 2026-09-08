#ifndef _FEFINDER_IMPL_H_
#define _FEFINDER_IMPL_H_

#include "Game/FE/feFinder.h"

template <typename T, int N>
template <typename U>
inline T* FEFinder<T, N>::Find(U* pTopLevel, InlineHasher Level1, InlineHasher Level2, InlineHasher Level3,
    InlineHasher Level4, InlineHasher Level5, InlineHasher Level6)
{
    TLInstance* pResult = _Find(pTopLevel, Level1.m_Hash, Level2.m_Hash, Level3.m_Hash, Level4.m_Hash, Level5.m_Hash, Level6.m_Hash);
    if (pResult == 0)
        return 0;
    return (T*)pResult;
}

template <typename T, int N>
template <typename U>
T* FEFinder<T, N>::FindOrDefault(U* pTopLevel, InlineHasher Level1, InlineHasher Level2, InlineHasher Level3,
    InlineHasher Level4, InlineHasher Level5, InlineHasher Level6)
{
    T* pResult = Find(pTopLevel, Level1.m_Hash, Level2.m_Hash, Level3.m_Hash, Level4.m_Hash, Level5.m_Hash, Level6.m_Hash);
    if (pResult == 0)
        pResult = (T*)FEGetDefaultInstance((eTimeLineAssetType)N);
    return pResult;
}

#endif // _FEFINDER_IMPL_H_
