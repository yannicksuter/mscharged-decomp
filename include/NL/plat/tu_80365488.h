#ifndef NL_PLAT_TU_80365488_H
#define NL_PLAT_TU_80365488_H

#include "NL/plat/PlatPadStatus.h"
#include "NL/platpad.h"
#include "NL/plat/tu_80364604.h"

class Class_80365488;
extern nlArrayAllocator<Class_80365488> lbl_806E2290;
extern int* lbl_806E2298;

class Class_80365488 : public PadBackend
{
public:
    Class_80365488(int padIndex);
    virtual ~Class_80365488();
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

    void fn_80365758(float dt);

    static void* operator new(unsigned long)
    {
        return lbl_806E2290.Allocate();
    }

    static void operator delete(void* ptr)
    {
        lbl_806E2290.DeleteEntry(static_cast<Class_80365488*>(ptr));
    }

    /* 0x01C */ PlatPadStatus_80375EE0* mUnidentified01C;
    /* 0x020 */ PlatPadStatus_80375EE0* mUnidentified020;
    /* 0x024 */ PlatPadStatus_80375EE0 mUnidentified024[2];
    /* 0x19C */ nlVector2 mUnidentified19C;
    /* 0x1A4 */ nlVector2 mUnidentified1A4;
    /* 0x1AC */ float mUnidentified1AC[16];
    /* 0x1EC */ UnidentifiedPointerData mUnidentified1EC;
}; // size 0x20C

#endif // NL_PLAT_TU_80365488_H
