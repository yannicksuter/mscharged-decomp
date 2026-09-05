#ifndef NL_PLAT_TU_80364E5C_H
#define NL_PLAT_TU_80364E5C_H

#include "NL/plat/PlatPadStatus.h"
#include "NL/platpad.h"
#include "NL/plat/tu_80364604.h"

class Class_80364E5C;
extern nlArrayAllocator<Class_80364E5C> lbl_806E2280;
extern int* lbl_806E2288;

class Class_80364E5C : public PadBackend
{
public:
    Class_80364E5C(int padIndex);
    virtual ~Class_80364E5C();
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
        return lbl_806E2280.Allocate();
    }

    static void operator delete(void* ptr)
    {
        lbl_806E2280.DeleteEntry(static_cast<Class_80364E5C*>(ptr));
    }

    /* 0x01C */ PlatPadStatus_80375ED4* mUnidentified01C;
    /* 0x020 */ PlatPadStatus_80375ED4* mUnidentified020;
    /* 0x024 */ PlatPadStatus_80375ED4 mUnidentified024[2];
    /* 0x194 */ nlVector2 mUnidentified194;
    /* 0x19C */ float mUnidentified19C[13];
    /* 0x1D0 */ UnidentifiedPointerData mUnidentified1D0;
}; // size 0x1F0

#endif // NL_PLAT_TU_80364E5C_H
