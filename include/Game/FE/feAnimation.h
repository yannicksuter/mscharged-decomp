#ifndef _FEANIMATION_H_
#define _FEANIMATION_H_

#include "types.h"

class TLInstance;

class FEAnimation
{
public:
    virtual ~FEAnimation() { }

    void Update(float fCurrentTime);

    /* 0x04 */ FEAnimation* m_next;
    /* 0x08 */ FEAnimation* m_prev;
    /* 0x0C */ TLInstance* m_pTLInstanceTarget;
    /* 0x10 */ u16 m_cast_type;
};

#endif // _FEANIMATION_H_
