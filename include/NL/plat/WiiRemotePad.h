#ifndef NL_PLAT_WII_REMOTE_PAD_H
#define NL_PLAT_WII_REMOTE_PAD_H

#include "NL/plat/WiiPad.h"
#include "NL/plat/PlatPadStatus.h"
#include "NL/plat/WiiPad.h"
#include "NL/platpad.h"
#include "NL/plat/DPDData.h"

class WiiRemotePad;
extern nlArrayAllocator<WiiRemotePad> gWiiRemotePadAllocator;

class WiiRemotePad : public PadBackend
{
public:
    WiiRemotePad(int padIndex);
    virtual ~WiiRemotePad() { }
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
    virtual int GetClassID() { return gWiiRemotePadClassID; }

    void UpdateState(float dt);

    static void* operator new(unsigned long)
    {
        return gWiiRemotePadAllocator.Allocate();
    }

    static void operator delete(void* ptr)
    {
        gWiiRemotePadAllocator.DeleteEntry(static_cast<WiiRemotePad*>(ptr));
    }

    /* 0x01C */ WiiRemotePadStatus* mCurrentStatus;
    /* 0x020 */ WiiRemotePadStatus* mPreviousStatus;
    /* 0x024 */ WiiRemotePadStatus mStatusBuffers[2];
    /* 0x184 */ float mButtonStateTime[11];
    /* 0x1B0 */ DPDData mDPDData;
}; // size 0x1D0

#endif // NL_PLAT_WII_REMOTE_PAD_H
