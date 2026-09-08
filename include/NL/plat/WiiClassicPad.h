#ifndef NL_PLAT_WII_CLASSIC_PAD_H
#define NL_PLAT_WII_CLASSIC_PAD_H

#include "NL/plat/PlatPadStatus.h"
#include "NL/plat/WiiPad.h"
#include "NL/platpad.h"
#include "NL/plat/DPDData.h"

class WiiClassicPad;
extern nlArrayAllocator<WiiClassicPad> gWiiClassicPadAllocator;

class WiiClassicPad : public PadBackend
{
public:
    WiiClassicPad(int padIndex);
    virtual ~WiiClassicPad() { }
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
    virtual int GetClassID() { return gWiiClassicPadClassID; }

    void UpdateState(float dt);

    static void* operator new(unsigned long)
    {
        return gWiiClassicPadAllocator.Allocate();
    }

    static void operator delete(void* ptr)
    {
        gWiiClassicPadAllocator.DeleteEntry(static_cast<WiiClassicPad*>(ptr));
    }

    /* 0x01C */ WiiClassicPadStatus* mCurrentStatus;
    /* 0x020 */ WiiClassicPadStatus* mPreviousStatus;
    /* 0x024 */ WiiClassicPadStatus mStatusBuffers[2];
    /* 0x19C */ nlVector2 mAnalogLeft;
    /* 0x1A4 */ nlVector2 mAnalogRight;
    /* 0x1AC */ float mButtonStateTime[16];
    /* 0x1EC */ DPDData mDPDData;
}; // size 0x20C

#endif // NL_PLAT_WII_CLASSIC_PAD_H
