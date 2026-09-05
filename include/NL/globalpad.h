#ifndef NL_GLOBALPAD_H
#define NL_GLOBALPAD_H

#include "NL/platpad.h"

class cGlobalPad : public PadBackend
{
public:
    cGlobalPad(int padIndex);
    virtual ~cGlobalPad();
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
    virtual void StartRumble(float fDuration, float fIntensity, float fFrequency);
    virtual void StopRumble();
    virtual void Update(float deltaTime);
    virtual int UnidentifiedClassID() { return sUnidentifiedClassID; }

    static int sUnidentifiedClassID;

    /* 0x1C */ PadBackend* mBackend;
}; // size: 0x20

class PadManager_802C06D4
{
public:
    PadManager_802C06D4();
    void fn_802C06D8(int padCount, int padSetCount);
    void Update(float deltaTime);
    cGlobalPad* GetPad(int idx);
    void fn_802C084C(int padSet);

    /* 0x00 */ int mUnidentified000;
    /* 0x04 */ int mUnidentified004;
    /* 0x08 */ int mUnidentified008;
    /* 0x0C */ cGlobalPad** m_aPads;
}; // size: 0x10

extern PadManager_802C06D4* lbl_806E1E28;

#endif // NL_GLOBALPAD_H
