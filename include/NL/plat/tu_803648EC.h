#ifndef NL_PLAT_TU_803648EC_H
#define NL_PLAT_TU_803648EC_H

#include "NL/plat/PlatPadStatus.h"
#include "NL/platpad.h"
#include "NL/plat/tu_80364604.h"

class Class_803648EC;
extern nlArrayAllocator<Class_803648EC> lbl_806E2270;
extern int* lbl_806E2278;

class Class_803648EC : public PadBackend
{
public:
    Class_803648EC(int padIndex);
    virtual ~Class_803648EC();
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
    virtual int UnidentifiedClassID();

    static void* operator new(unsigned long)
    {
        return lbl_806E2270.Allocate();
    }

    static void operator delete(void* ptr)
    {
        lbl_806E2270.DeleteEntry(static_cast<Class_803648EC*>(ptr));
    }

    /* 0x01C */ PlatPadStatus_80375EC8* mUnidentified01C;
    /* 0x020 */ PlatPadStatus_80375EC8* mUnidentified020;
    /* 0x024 */ PlatPadStatus_80375EC8 mUnidentified024[2];
    /* 0x184 */ float mUnidentified184[11];
    /* 0x1B0 */ UnidentifiedPointerData mUnidentified1B0;
}; // size 0x1D0

#endif // NL_PLAT_TU_803648EC_H
