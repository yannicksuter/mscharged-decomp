#ifndef UNCLASSIFIED_TU_80331BE4_H
#define UNCLASSIFIED_TU_80331BE4_H

#include "NL/nlMath.h"
#include "types.h"

class cGlobalPad;

class DetInput
{
public:
    DetInput();

    u8 fn_80331BE4();
    u8 fn_80331BEC();
    nlVector3* fn_80331BF4();
    nlVector3* fn_80331BFC();
    bool JustPressed(int button, bool remap);
    bool JustReleased(int button, bool remap);
    void fn_80331D80();
    void fn_80331DC8();
    int fn_80331ECC(int button, bool remap);
    void fn_80331F9C(int button, bool remap);
    void fn_80332110(const DetInput& input);
    void fn_8033222C(cGlobalPad* pad);
    int fn_80332748();

    /* 0x00 */ float m_AnalogLeftX;
    /* 0x04 */ float m_AnalogLeftY;
    /* 0x08 */ float m_AnalogRightX;
    /* 0x0C */ float m_AnalogRightY;
    /* 0x10 */ u8 m_nConnected;
    /* 0x11 */ u8 mPadding11;
    /* 0x12 */ u16 m_ButtonBitfield;
    /* 0x14 */ u8 m_LeftTrigger;
    /* 0x15 */ u8 m_RightTrigger;
    /* 0x16 */ u8 mPadding16[2];
    /* 0x18 */ nlVector3 m_v3RevRemoteAccel;
    /* 0x24 */ nlVector3 m_v3RevFreeStyleAccel;
    /* 0x30 */ u8 m_nRevDPDNumTargets;
    /* 0x31 */ u8 mPadding31[3];
    /* 0x34 */ nlVector2 m_v2RevDPDCoord;
    /* 0x3C */ DetInput* m_pPrevInput;
    /* 0x40 */ void* m_pMyUser;
    /* 0x44 */ nlPolar m_PolarAnalogLeft;
    /* 0x4C */ nlPolar m_PolarAnalogRight;
    /* 0x54 */ int m_buttonStateTicks[13];
    /* 0x88 */ u16 m_aRemapAngle;
}; // size: 0x8C

extern "C" bool fn_80331C04(DetInput* input, int button, bool remap);

#endif // UNCLASSIFIED_TU_80331BE4_H
