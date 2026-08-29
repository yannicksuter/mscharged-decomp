#ifndef _TLCOMPONENT_H_
#define _TLCOMPONENT_H_

#include "Game/FE/feLibObject.h"

class TLSlide;

class TLComponent : public FELibObject
{
public:
    TLComponent();

    void SetActiveSlide(const char* name, bool arg2, bool arg3);
    void SetActiveSlide(unsigned long hash, bool arg2, bool arg3);
    void SetActiveSlide(TLSlide* slide, bool arg2, bool arg3);

    TLSlide* GetActiveSlide() const
    {
        return m_pActiveSlide;
    }

    /* 0x78 */ TLSlide* pChildren;
    /* 0x7C */ TLSlide* m_pActiveSlide;
};

#endif // _TLCOMPONENT_H_
