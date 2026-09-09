#ifndef NL_PLAT_WII_FREESTYLE_PAD_H
#define NL_PLAT_WII_FREESTYLE_PAD_H

#include "NL/plat/WiiPad.h"
#include "NL/plat/PlatPadStatus.h"
#include "NL/plat/WiiPad.h"
#include "NL/platpad.h"
#include "NL/plat/DPDData.h"

class WiiFreestylePad;
extern nlArrayAllocator<WiiFreestylePad> gWiiFreestylePadAllocator;

class WiiFreestylePad : public PadBackend
{
public:
    WiiFreestylePad(int padIndex);
    virtual ~WiiFreestylePad() { }
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
    virtual void Update(float dt);
    virtual int GetClassID() { return gWiiFreestylePadClassID; }

    static void* operator new(unsigned long)
    {
        return gWiiFreestylePadAllocator.Allocate();
    }

    static void operator delete(void* ptr)
    {
        gWiiFreestylePadAllocator.DeleteEntry(static_cast<WiiFreestylePad*>(ptr));
    }

    /* 0x01C */ WiiFreestylePadStatus* mCurrentStatus;
    /* 0x020 */ WiiFreestylePadStatus* mPreviousStatus;
    /* 0x024 */ WiiFreestylePadStatus mStatusBuffers[2];
    /* 0x194 */ nlVector2 mAnalogLeft;
    /* 0x19C */ float mButtonStateTime[13];
    /* 0x1D0 */ DPDData mDPDData;
}; // size 0x1F0

#endif // NL_PLAT_WII_FREESTYLE_PAD_H
