#ifndef NL_PLAT_TU_80364604_H
#define NL_PLAT_TU_80364604_H

#include "NL/nlMath.h"

struct KPADStatus;

struct UnidentifiedPointerData
{
    /* 0x00 */ int mCount;
    /* 0x04 */ nlVector2 mUnidentified004;
    /* 0x0C */ unsigned short mAngle;
    /* 0x0E */ unsigned char mUnidentified00E[2];
    /* 0x10 */ nlVector2 mUnidentified010;
    /* 0x18 */ nlVector2 mPosition;
}; // size 0x20

extern "C"
{
    void fn_80364604(UnidentifiedPointerData* data);
    int fn_80364630(UnidentifiedPointerData* data, nlVector2* position, unsigned short* angle);
    void fn_80364650(UnidentifiedPointerData* data, KPADStatus* status);
}

#endif // NL_PLAT_TU_80364604_H
