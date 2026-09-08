#ifndef NL_PLAT_DPD_DATA_H
#define NL_PLAT_DPD_DATA_H

#include "NL/nlMath.h"

struct KPADStatus;

struct DPDData
{
    DPDData();
    int GetPosition(nlVector2* position, unsigned short* angle);
    void Update(KPADStatus* status);

    /* 0x00 */ int mValidFlag;
    /* 0x04 */ nlVector2 mRawPosition;
    /* 0x0C */ unsigned short mAngle;
    /* 0x0E */ unsigned char mUnidentified00E[2];
    /* 0x10 */ nlVector2 mVector;
    /* 0x18 */ nlVector2 mPosition;
}; // size 0x20

#endif // NL_PLAT_DPD_DATA_H
