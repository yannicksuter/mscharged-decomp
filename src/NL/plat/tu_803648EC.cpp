#include "NL/plat/tu_803648EC.h"
#include "NL/plat/tu_80364118.h"

extern int lbl_806E1E20;

static unsigned char lbl_805860E0[8 * sizeof(Class_803648EC)];
nlArrayAllocator<Class_803648EC> lbl_806E2270(reinterpret_cast<Class_803648EC*>(lbl_805860E0), 8);
int* lbl_806E2278;
int lbl_806E227C = lbl_806E1E20++;

Class_803648EC::Class_803648EC(int padIndex)
    : PadBackend(padIndex)
    , mUnidentified01C(&mUnidentified024[0])
    , mUnidentified020(&mUnidentified024[1])
{
    fn_80364604(&mUnidentified1B0);
    for (int i = 0; i < 11; ++i)
        mUnidentified184[i] = 0.0f;
    Update(0.0f);
    Update(0.0f);
}

void Class_803648EC::Update(float dt)
{
    if (!fn_80365E84(this))
    {
        PlatPadStatus_80375EC8* temp = mUnidentified01C;
        mUnidentified01C = mUnidentified020;
        mUnidentified020 = temp;
        *mUnidentified01C = *fn_80375EC8(lbl_806E2478, m_padIndex);
        fn_80364650(&mUnidentified1B0, &mUnidentified01C->kpad);

        int changed = mUnidentified01C->wpad.button ^ mUnidentified020->wpad.button;
        for (int i = 0; i < 11; ++i)
        {
            if (changed & GetButtonMask(i))
                mUnidentified184[i] = 0.0f;
            else
                mUnidentified184[i] += dt;
        }
        PadBackend::Update(dt);
    }
}

bool Class_803648EC::IsConnected()
{
    return true;
}

bool Class_803648EC::IsPressed(int button, bool remap)
{
    if (remap)
        button = lbl_806E2278[button];
    return (button & mUnidentified01C->wpad.button) != 0;
}

float Class_803648EC::GetPressure(int button, bool remap)
{
    if (remap)
        button = lbl_806E2278[button];
    return (button & mUnidentified01C->wpad.button) ? 1.0f : 0.0f;
}

float Class_803648EC::GetPressureDerivative(int button, bool remap)
{
    return 0.0f;
}

bool Class_803648EC::PlatJustPressed(int button, bool remap)
{
    if (remap)
        button = lbl_806E2278[button];
    return (button & mUnidentified01C->wpad.button) && !(button & mUnidentified020->wpad.button);
}

bool Class_803648EC::PlatJustReleased(int button, bool remap)
{
    if (remap)
        button = lbl_806E2278[button];
    return !(button & mUnidentified01C->wpad.button) && (button & mUnidentified020->wpad.button);
}

int Class_803648EC::GetButtonIndex(int button, bool remap)
{
    return fn_80364298(remap ? lbl_806E2278[button] : button);
}

int Class_803648EC::GetButtonMask(int buttonIndex)
{
    return fn_803643A8(buttonIndex);
}

float Class_803648EC::GetButtonStateTime(int button, bool remap)
{
    return mUnidentified184[fn_80364298(remap ? lbl_806E2278[button] : button)];
}

float Class_803648EC::AnalogLeftX()
{
    return 0.0f;
}

float Class_803648EC::AnalogLeftY()
{
    return 0.0f;
}

float Class_803648EC::AnalogRightX()
{
    return 0.0f;
}

float Class_803648EC::AnalogRightY()
{
    return 0.0f;
}

bool Class_803648EC::RumbleActive()
{
    return false;
}

void Class_803648EC::StartRumble(float fDuration, float fIntensity, float fFrequency)
{
    WPADControlMotor(m_padIndex, WPAD_MOTOR_RUMBLE);
}

void Class_803648EC::StopRumble()
{
    WPADControlMotor(m_padIndex, WPAD_MOTOR_STOP);
}

int Class_803648EC::UnidentifiedClassID()
{
    return lbl_806E227C;
}

Class_803648EC::~Class_803648EC()
{
}
