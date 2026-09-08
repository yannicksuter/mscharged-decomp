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
    virtual int GetClassID() { return sClassID; }

    static int sClassID;

    /* 0x1C */ PadBackend* mBackend;
}; // size: 0x20

class PadManager
{
public:
    PadManager();
    void Initialize(int padCount, int padSetCount);
    void Update(float deltaTime);
    cGlobalPad* GetPad(int idx);
    void SetActivePadSet(int padSet);

    /* 0x00 */ int mPadCount;
    /* 0x04 */ int mPadSetCount;
    /* 0x08 */ int mActivePadSet;
    /* 0x0C */ cGlobalPad** m_aPads;
}; // size: 0x10

extern PadManager* g_pPadManager;

#endif // NL_GLOBALPAD_H
