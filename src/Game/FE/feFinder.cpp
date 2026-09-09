#include "Game/FE/feFinder.h"
#include "Game/FE/tlComponent.h"

void* FEFindInstance(FEPresentation* pPresentation, unsigned long Level1, unsigned long Level2,
    unsigned long Level3, unsigned long Level4, unsigned long Level5, unsigned long Level6)
{
    if (pPresentation == 0)
        return 0;

    TLSlide* pChild = FindItemByHashID(pPresentation->m_slides, Level1);
    if (pChild == 0)
        return FEFinder<void, TLAT_UNKNOWN>::_Find(pPresentation->m_currentSlide,
            Level1, Level2, Level3, Level4, Level5, Level6);
    if (Level2 == 0)
        return pChild;
    return FEFinder<void, TLAT_UNKNOWN>::_Find(pChild,
        Level2, Level3, Level4, Level5, Level6, 0);
}

void* FEFindInstance(TLInstance* pInstance, unsigned long Level1, unsigned long Level2,
    unsigned long Level3, unsigned long Level4, unsigned long Level5, unsigned long Level6)
{
    return FEFindInstanceRecursive(pInstance, Level1, Level2, Level3, Level4, Level5, Level6);
}

void* FEFindInstanceRecursive(TLInstance* pInstance, unsigned long Level1, unsigned long Level2,
    unsigned long Level3, unsigned long Level4, unsigned long Level5, unsigned long Level6)
{
    if (pInstance == 0)
        return 0;

    eTimeLineAssetType type = pInstance->m_type;
    void* pChild;
    if (type == TLAT_COMPONENT)
    {
        TLComponent* pComponent = (TLComponent*)pInstance->m_component;
        pChild = FindItemByHashID(pComponent->pChildren, Level1);
        if (pChild == 0)
            return FEFinder<void, TLAT_UNKNOWN>::_Find(
                ((TLComponentInstance*)pInstance)->GetActiveSlide(),
                Level1, Level2, Level3, Level4, Level5, Level6);
    }
    else
    {
        pChild = FindItemByHashID(pInstance->pChildren, Level1);
    }

    if (pChild == 0 || Level2 == 0)
        return pChild;
    if (type == TLAT_COMPONENT)
        return FEFinder<void, TLAT_UNKNOWN>::_Find((TLSlide*)pChild,
            Level2, Level3, Level4, Level5, Level6, 0);
    return FEFindInstanceRecursive((TLInstance*)pChild, Level2, Level3, Level4, Level5, Level6, 0);
}
