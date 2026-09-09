#ifndef _FEFINDER_H_
#define _FEFINDER_H_

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/tlSlide.h"

extern TLImageInstance gDefaultTLImageInstance;
extern TLComponentInstance gDefaultTLComponentInstance;
extern TLTextInstance gDefaultTLTextInstance;
extern TLInstance gDefaultTLGroupInstance;
extern TLInstance gDefaultTLLayerInstance;

inline TLInstance* FEGetDefaultInstance(eTimeLineAssetType type)
{
    switch (type)
    {
    case TLAT_LAYER:
        return &gDefaultTLLayerInstance;
    case TLAT_IMAGE:
        return &gDefaultTLImageInstance;
    case TLAT_TEXT:
        return &gDefaultTLTextInstance;
    case TLAT_COMPONENT:
        return &gDefaultTLComponentInstance;
    case TLAT_GROUP:
        return &gDefaultTLGroupInstance;
    default:
        return 0;
    }
}

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

TLTextInstance* FEFindTextInstance(TLSlide* pTopLevel, InlineHasher Level1,
    InlineHasher Level2 = InlineHasher(0UL), InlineHasher Level3 = InlineHasher(0UL),
    InlineHasher Level4 = InlineHasher(0UL), InlineHasher Level5 = InlineHasher(0UL),
    InlineHasher Level6 = InlineHasher(0UL));

void* FEFindInstance(FEPresentation* pPresentation, unsigned long Level1, unsigned long Level2,
    unsigned long Level3, unsigned long Level4, unsigned long Level5, unsigned long Level6);
void* FEFindInstanceRecursive(TLInstance* pInstance, unsigned long Level1, unsigned long Level2,
    unsigned long Level3, unsigned long Level4, unsigned long Level5, unsigned long Level6);
void* FEFindInstance(TLInstance* pInstance, unsigned long Level1, unsigned long Level2,
    unsigned long Level3, unsigned long Level4, unsigned long Level5, unsigned long Level6);

template <typename T, int N>
struct FEFinder
{
    static inline T* Find(FEPresentation* pTopLevel, const unsigned long Level1, const unsigned long Level2,
        const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6);

    static inline T* Find(TLSlide* pTopLevel, const unsigned long Level1, const unsigned long Level2,
        const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6);

    static inline T* Find(TLInstance* pTopLevel, const unsigned long Level1, const unsigned long Level2,
        const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6);

    template <typename U>
    static T* Find(U* pTopLevel, InlineHasher Level1, InlineHasher Level2 = InlineHasher(0UL), InlineHasher Level3 = InlineHasher(0UL),
        InlineHasher Level4 = InlineHasher(0UL), InlineHasher Level5 = InlineHasher(0UL), InlineHasher Level6 = InlineHasher(0UL));

    template <typename U>
    static inline T* FindOrDefault(U* pTopLevel, const unsigned long Level1, const unsigned long Level2,
        const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6);

    template <typename U>
    static T* FindOrDefault(U* pTopLevel, InlineHasher Level1, InlineHasher Level2 = InlineHasher(0UL), InlineHasher Level3 = InlineHasher(0UL),
        InlineHasher Level4 = InlineHasher(0UL), InlineHasher Level5 = InlineHasher(0UL), InlineHasher Level6 = InlineHasher(0UL));

    template <typename U>
    static inline T* Find(U* pTopLevel, const unsigned long Level1, const unsigned long Level2,
        const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6);

    static inline T* _Find(FEPresentation* pTopLevel, const unsigned long Level1, const unsigned long Level2,
        const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6);

    static inline T* _Find(TLComponentInstance* pTopLevel, const unsigned long Level1, const unsigned long Level2,
        const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6);

    template <typename U>
    static inline T* _Find(U* pTopLevel, const unsigned long Level1, const unsigned long Level2,
        const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6);
};

template <typename T, int N>
inline T* FEFinder<T, N>::Find(FEPresentation* pTopLevel, const unsigned long Level1, const unsigned long Level2,
    const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6)
{
    void* pResult = FEFindInstance(pTopLevel, Level1, Level2, Level3, Level4, Level5, Level6);
    if (pResult == 0)
        return 0;
    return (T*)pResult;
}

template <typename T, int N>
inline T* FEFinder<T, N>::Find(TLSlide* pTopLevel, const unsigned long Level1, const unsigned long Level2,
    const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6)
{
    void* pResult = _Find(pTopLevel, Level1, Level2, Level3, Level4, Level5, Level6);
    if (pResult == 0)
        return 0;
    return (T*)pResult;
}

template <typename T, int N>
inline T* FEFinder<T, N>::Find(TLInstance* pTopLevel, const unsigned long Level1, const unsigned long Level2,
    const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6)
{
    void* pResult = _Find(pTopLevel, Level1, Level2, Level3, Level4, Level5, Level6);
    if (pResult == 0)
        return 0;
    return (T*)pResult;
}

template <typename T, int N>
template <typename U>
inline T* FEFinder<T, N>::Find(U* pTopLevel, const unsigned long Level1, const unsigned long Level2,
    const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6)
{
    void* pResult = FEFindInstance((TLInstance*)pTopLevel, Level1, Level2, Level3, Level4, Level5, Level6);
    if (pResult == 0)
        return 0;
    return (T*)pResult;
}

template <typename T, int N>
template <typename U>
inline T* FEFinder<T, N>::_Find(U* pTopLevel, const unsigned long Level1, const unsigned long Level2,
    const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6)
{
    if (pTopLevel == 0)
        return 0;
    TLInstance* pChild = FindItemByHashID(FEGetChildren(pTopLevel), Level1);
    if (pChild == 0)
        return 0;
    if (Level2 == 0)
        return (T*)pChild;
    return (T*)FEFindInstanceRecursive(pChild, Level2, Level3, Level4, Level5, Level6, 0);
}

template <typename T, int N>
template <typename U>
inline T* FEFinder<T, N>::FindOrDefault(U* pTopLevel, const unsigned long Level1, const unsigned long Level2,
    const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6)
{
    T* pResult = Find(pTopLevel, Level1, Level2, Level3, Level4, Level5, Level6);
    return pResult == 0 ? (T*)FEGetDefaultInstance((eTimeLineAssetType)N) : pResult;
}

template <typename T, int N>
inline T* FEFinder<T, N>::_Find(FEPresentation* pTopLevel, const unsigned long Level1, const unsigned long Level2,
    const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6)
{
    return (T*)FEFindInstance(pTopLevel, Level1, Level2, Level3, Level4, Level5, Level6);
}

template <typename T, int N>
inline T* FEFinder<T, N>::_Find(TLComponentInstance* pTopLevel, const unsigned long Level1, const unsigned long Level2,
    const unsigned long Level3, const unsigned long Level4, const unsigned long Level5, const unsigned long Level6)
{
    return (T*)FEFindInstance((TLInstance*)pTopLevel, Level1, Level2, Level3, Level4, Level5, Level6);
}

#endif // _FEFINDER_H_
