#ifndef GAME_DET_INPUT_H
#define GAME_DET_INPUT_H

#include "NL/nlMath.h"
#include "types.h"

class cGlobalPad;

class DetInput
{
public:
    DetInput()
        : m_pPrevInput(0)
        , m_pMyUser(0)
    {
        Reset();
    }

    void Reset();
    u8 GetConnectionStatus();
    int GetControllerType();
    nlVector3* GetRemoteAcceleration();
    nlVector3* GetFreestyleAcceleration();
    bool IsPressed(int button, bool remap);
    bool JustPressed(int button, bool remap);
    bool JustReleased(int button, bool remap);
    float AnalogLeftX() { return m_AnalogLeftX; }
    float AnalogLeftY() { return m_AnalogLeftY; }
    float AnalogRightX() { return m_AnalogRightX; }
    float AnalogRightY() { return m_AnalogRightY; }
    void UpdatePolarAnalog();
    void UpdateButtonStateTicks();
    int GetButtonStateTicks(int button, bool remap);
    void ResetButtonStateTicks(int button, bool remap);
    void CopyState(const DetInput& input);
    void ReadFromPad(cGlobalPad* pad);
    int GetPadID() const;

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

#endif // GAME_DET_INPUT_H
