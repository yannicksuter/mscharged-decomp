#ifndef _FEFINDER_H_
#define _FEFINDER_H_

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/tlSlide.h"

template <class T>
inline T* FindItemByHashID(T* list, unsigned long hashID)
{
    if (list == 0)
        return 0;
    T* curr = list->m_next;
    for (;;)
    {
        unsigned long id = curr->m_hash;
        T* next = curr->m_next;
        if (hashID == id)
            return curr;
        if (curr == list)
            break;
        curr = next;
    }
    return 0;
}

static inline TLInstance* FEGetChildren(TLSlide* p)
{
    return p->m_instances;
}

static inline TLInstance* FEGetChildren(TLInstance* p)
{
    return p->pChildren;
}

struct InlineHasher
{
    unsigned long m_Hash;
    InlineHasher(unsigned long h);
    InlineHasher(const char* string);
};

extern "C" TLInstance* fn_8030677C(FEPresentation* pPresentation, unsigned long Level1, unsigned long Level2,
    unsigned long Level3, unsigned long Level4, unsigned long Level5, unsigned long Level6);
extern "C" TLInstance* fn_803068F8(TLInstance* pInstance, unsigned long Level1, unsigned long Level2,
    unsigned long Level3, unsigned long Level4, unsigned long Level5, unsigned long Level6);
extern "C" TLInstance* fn_803068F4(TLInstance* pInstance, unsigned long Level1, unsigned long Level2,
    unsigned long Level3, unsigned long Level4, unsigned long Level5, unsigned long Level6);

template <typename T, int N>
struct FEFinder
{
    static inline T* Find(FEPresentation* pTopLevel, const unsigned long Level1, const unsigned long Level2,
        const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6);

    template <typename U>
    static T* Find(U* pTopLevel, InlineHasher Level1, InlineHasher Level2 = InlineHasher(0UL), InlineHasher Level3 = InlineHasher(0UL),
        InlineHasher Level4 = InlineHasher(0UL), InlineHasher Level5 = InlineHasher(0UL), InlineHasher Level6 = InlineHasher(0UL));

    template <typename U>
    static inline T* Find(U* pTopLevel, const unsigned long Level1, const unsigned long Level2,
        const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6);

    template <typename U>
    static inline TLInstance* _Find(U* pTopLevel, const unsigned long Level1, const unsigned long Level2,
        const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6);
};

template <typename T, int N>
inline T* FEFinder<T, N>::Find(FEPresentation* pTopLevel, const unsigned long Level1, const unsigned long Level2,
    const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6)
{
    TLInstance* pResult = fn_8030677C(pTopLevel, Level1, Level2, Level3, Level4, Level5, Level6);
    if (pResult == 0)
        return 0;
    return (T*)pResult;
}

template <typename T, int N>
template <typename U>
inline T* FEFinder<T, N>::Find(U* pTopLevel, const unsigned long Level1, const unsigned long Level2,
    const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6)
{
    TLInstance* pResult = fn_803068F4((TLInstance*)pTopLevel, Level1, Level2, Level3, Level4, Level5, Level6);
    if (pResult == 0)
        return 0;
    return (T*)pResult;
}

template <typename T, int N>
template <typename U>
T* FEFinder<T, N>::Find(U* pTopLevel, InlineHasher Level1, InlineHasher Level2, InlineHasher Level3,
    InlineHasher Level4, InlineHasher Level5, InlineHasher Level6)
{
    TLInstance* pResult = _Find(pTopLevel, Level1.m_Hash, Level2.m_Hash, Level3.m_Hash, Level4.m_Hash, Level5.m_Hash, Level6.m_Hash);
    if (pResult == 0)
        return 0;
    return (T*)pResult;
}

template <typename T, int N>
template <typename U>
inline TLInstance* FEFinder<T, N>::_Find(U* pTopLevel, const unsigned long Level1, const unsigned long Level2,
    const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6)
{
    if (pTopLevel == 0)
        return 0;
    TLInstance* pChild = FindItemByHashID(FEGetChildren(pTopLevel), Level1);
    if (pChild == 0)
        return 0;
    if (Level2 == 0)
        return pChild;
    return fn_803068F8(pChild, Level2, Level3, Level4, Level5, Level6, 0);
}

#endif // _FEFINDER_H_
