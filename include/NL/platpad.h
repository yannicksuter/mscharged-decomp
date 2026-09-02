#ifndef NL_PLATPAD_H
#define NL_PLATPAD_H

#include "types.h"

class PadBackend
{
public:
    PadBackend(int padIndex)
        : mPadIndex(padIndex)
        , mUnidentified018(false)
    {
    }

    virtual ~PadBackend();
    virtual bool IsConnected();
    virtual bool IsPressed(int button, bool remap);
    virtual float GetPressure(int button, bool remap);
    virtual float GetPressureDerivative(int button, bool remap);
    virtual bool PlatJustPressed(int button, bool remap);
    virtual bool PlatJustReleased(int button, bool remap);
    virtual int GetButtonIndex(int button, bool remap);
    virtual int GetButtonMask(int buttonIndex);
    virtual float GetButtonStateTime(int button, bool remap);
    virtual float AnalogLeftX();
    virtual float AnalogLeftY();
    virtual float AnalogRightX();
    virtual float AnalogRightY();
    virtual bool RumbleActive();
    virtual void StartRumble(float duration, float intensity, float frequency);
    virtual void StopRumble();
    virtual void Update(float dt);

    /* 0x04 */ int mPadIndex;
    /* 0x08 */ u8 mUnidentified008[0x10];
    /* 0x18 */ bool mUnidentified018;
    /* 0x19 */ u8 mUnidentified019[3];
}; // size 0x1C

class cPlatPad;
extern cPlatPad* lbl_806E2260;

class cPlatPad : public PadBackend
{
public:
    cPlatPad(int padIndex)
        : PadBackend(padIndex)
    {
    }

    virtual ~cPlatPad();
    virtual bool IsConnected();
    virtual bool IsPressed(int button, bool remap);
    virtual float GetPressure(int button, bool remap);
    virtual float GetPressureDerivative(int button, bool remap);
    virtual bool PlatJustPressed(int button, bool remap);
    virtual bool PlatJustReleased(int button, bool remap);
    virtual int GetButtonIndex(int button, bool remap);
    virtual int GetButtonMask(int buttonIndex);
    virtual float GetButtonStateTime(int button, bool remap);
    virtual float AnalogLeftX();
    virtual float AnalogLeftY();
    virtual float AnalogRightX();
    virtual float AnalogRightY();
    virtual bool RumbleActive();
    virtual void StartRumble(float duration, float intensity, float frequency);
    virtual void StopRumble();
    virtual void Update(float dt);

    static void* operator new(unsigned long)
    {
        cPlatPad* pad = lbl_806E2260;
        if (pad == 0)
        {
            return 0;
        }
        lbl_806E2260 = *(cPlatPad**)pad;
        return pad;
    }
}; // size 0x1C

#endif // NL_PLATPAD_H
