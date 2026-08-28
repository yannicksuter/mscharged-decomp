#ifndef NL_GLOBALPAD_H
#define NL_GLOBALPAD_H

#include "NL/nlMath.h"
#include "types.h"

class cGlobalPad
{
public:
    virtual ~cGlobalPad();
    virtual bool IsConnected() = 0;
    virtual bool IsPressed(int button, bool remap) = 0;
    virtual float GetPressure(int button, bool remap) = 0;
    virtual float GetPressureDerivative(int button, bool remap) = 0;
    virtual bool PlatJustPressed(int button, bool remap) = 0;
    virtual bool PlatJustReleased(int button, bool remap) = 0;
    virtual float GetButtonStateTime(int button, bool remap) = 0;
    virtual float AnalogLeftX() = 0;
    virtual float AnalogLeftY() = 0;
    virtual float AnalogRightX() = 0;
    virtual float AnalogRightY() = 0;
    virtual void Unidentified38() = 0;
    virtual void Unidentified3C() = 0;
    virtual bool RumbleActive() = 0;
    virtual void StartRumble(float fDuration, float fIntensity, float fFrequency) = 0;
    virtual void StopRumble() = 0;

    bool JustPressed(int button, bool remap);
    bool JustReleased(int button, bool remap);
    int fn_80332748();

    /* 0x04 */ u8 mUnidentified004[0x40];
    /* 0x44 */ nlPolar m_polarAnalogLeft;
    /* 0x4C */ nlPolar m_polarAnalogRight;
    /* 0x54 */ u8 mUnidentified054[0x34];
    /* 0x88 */ u16 mUnidentified088;
};

#endif // NL_GLOBALPAD_H
