#ifndef _FEFINDER_INL_
#define _FEFINDER_INL_

#include "Game/FE/feFinder.h"
#include "NL/nlString.h"

inline InlineHasher::InlineHasher(unsigned long h)
    : m_Hash(h)
{
}

inline InlineHasher::InlineHasher(const char* string)
    : m_Hash(nlStringLowerHash(string))
{
}

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

#endif // _FEFINDER_INL_
