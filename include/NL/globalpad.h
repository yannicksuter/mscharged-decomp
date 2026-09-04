#ifndef NL_GLOBALPAD_H
#define NL_GLOBALPAD_H

#include "NL/nlMath.h"
#include "types.h"

class PadBackend;

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
    virtual int Unidentified24(int button, bool remap) = 0;
    virtual int Unidentified28(int buttonIndex) = 0;
    virtual float GetButtonStateTime(int button, bool remap) = 0;
    virtual float AnalogLeftX() = 0;
    virtual float AnalogLeftY() = 0;
    virtual float AnalogRightX() = 0;
    virtual float AnalogRightY() = 0;
    virtual bool RumbleActive() = 0;
    virtual void StartRumble(float fDuration, float fIntensity, float fFrequency) = 0;
    virtual void StopRumble() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual int UnidentifiedClassID() = 0;

    void DisableLeftAnalogToDPadMap()
    {
        m_isLeftAnalogToDPadMapEnabled = false;
    }

    void EnableLeftAnalogToDPadMap()
    {
        m_isLeftAnalogToDPadMapEnabled = true;
    }

    /* 0x04 */ u8 mUnidentified004[0x14];
    /* 0x18 */ bool m_isLeftAnalogToDPadMapEnabled;
    /* 0x19 */ u8 mUnidentified019[3];
    /* 0x1C */ PadBackend* mBackend;
}; // size: 0x20

#endif // NL_GLOBALPAD_H
