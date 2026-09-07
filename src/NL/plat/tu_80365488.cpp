#include "NL/plat/tu_80365488.h"
#include "NL/plat/tu_80364118.h"

extern int lbl_806E1E20;

static unsigned char lbl_80587F00[8 * sizeof(Class_80365488)];
nlArrayAllocator<Class_80365488> lbl_806E2290(reinterpret_cast<Class_80365488*>(lbl_80587F00), 8);
int* lbl_806E2298;
int lbl_806E229C = lbl_806E1E20++;

Class_80365488::Class_80365488(int padIndex)
    : PadBackend(padIndex)
    , mUnidentified01C(&mUnidentified024[0])
    , mUnidentified020(&mUnidentified024[1])
{
    fn_80364604(&mUnidentified1EC);
    for (int i = 0; i < 16; ++i)
        mUnidentified1AC[i] = 0.0f;
    Update(0.0f);
    Update(0.0f);
}

void Class_80365488::Update(float dt)
{
    if (!fn_80365E84(this))
    {
        PlatPadStatus_80375EE0* temp = mUnidentified01C;
        mUnidentified01C = mUnidentified020;
        mUnidentified020 = temp;
        *mUnidentified01C = *fn_80375EE0(g_pPlatPadManager, m_padIndex);

        signed char x = mUnidentified01C->wpad.clLStickX;
        signed char y = mUnidentified01C->wpad.clLStickY;
        fn_80364118(&x, &y);
        mUnidentified01C->wpad.clLStickX = x;
        mUnidentified01C->wpad.clLStickY = y;
        x = mUnidentified01C->wpad.clRStickX;
        y = mUnidentified01C->wpad.clRStickY;
        fn_80364118(&x, &y);
        mUnidentified01C->wpad.clRStickX = x;
        mUnidentified01C->wpad.clRStickY = y;

        fn_80365758(dt);
        PadBackend::Update(dt);
    }
}

void Class_80365488::fn_80365758(float dt)
{
    fn_80364650(&mUnidentified1EC, &mUnidentified01C->kpad);
    WPADCLStatus* status = &mUnidentified01C->wpad;
    mUnidentified19C.x = status->clLStickX / 56.0f;
    mUnidentified19C.y = status->clLStickY / 56.0f;
    mUnidentified1A4.x = status->clRStickX / 56.0f;
    mUnidentified1A4.y = status->clRStickY / 56.0f;
    if (m_isLeftAnalogToDPadMapEnabled)
    {
        status->clButton |= fn_80364434(mUnidentified19C.x, mUnidentified19C.y, 2, 0x8000, 0x4000, 1);
    }

    int changed = mUnidentified01C->wpad.clButton ^ mUnidentified020->wpad.clButton;
    for (int i = 0; i < 16; ++i)
    {
        if (changed & GetButtonMask(i))
            mUnidentified1AC[i] = 0.0f;
        else
            mUnidentified1AC[i] += dt;
    }
}

bool Class_80365488::IsConnected()
{
    return true;
}

bool Class_80365488::IsPressed(int button, bool remap)
{
    if (remap)
        button = lbl_806E2298[button];
    return (button & mUnidentified01C->wpad.clButton) != 0;
}

float Class_80365488::GetPressure(int button, bool remap)
{
    if (remap)
        button = lbl_806E2298[button];
    return (button & mUnidentified01C->wpad.clButton) ? 1.0f : 0.0f;
}

float Class_80365488::GetPressureDerivative(int button, bool remap)
{
    return 0.0f;
}

bool Class_80365488::PlatJustPressed(int button, bool remap)
{
    if (remap)
        button = lbl_806E2298[button];
    return (button & mUnidentified01C->wpad.clButton) && !(button & mUnidentified020->wpad.clButton);
}

bool Class_80365488::PlatJustReleased(int button, bool remap)
{
    if (remap)
        button = lbl_806E2298[button];
    return !(button & mUnidentified01C->wpad.clButton) && (button & mUnidentified020->wpad.clButton);
}

int Class_80365488::GetButtonIndex(int button, bool remap)
{
    if (remap)
        button = lbl_806E2298[button];
    int buttonIndex = 0;
    switch (button)
    {
    case 0x1:
        buttonIndex = 0;
        break;
    case 0x2:
        buttonIndex = 1;
        break;
    case 0x4:
        buttonIndex = 2;
        break;
    case 0x8:
        buttonIndex = 3;
        break;
    case 0x10:
        buttonIndex = 4;
        break;
    case 0x20:
        buttonIndex = 5;
        break;
    case 0x40:
        buttonIndex = 6;
        break;
    case 0x80:
        buttonIndex = 7;
        break;
    case 0x100:
        buttonIndex = 8;
        break;
    case 0x200:
        buttonIndex = 9;
        break;
    case 0x400:
        buttonIndex = 10;
        break;
    case 0x800:
        buttonIndex = 11;
        break;
    case 0x1000:
        buttonIndex = 12;
        break;
    case 0x2000:
        buttonIndex = 13;
        break;
    case 0x4000:
        buttonIndex = 14;
        break;
    case 0x8000:
        buttonIndex = 15;
        break;
    }
    return buttonIndex;
}

int Class_80365488::GetButtonMask(int buttonIndex)
{
    int button = 0;
    switch (buttonIndex)
    {
    case 0:
        button = 0x1;
        break;
    case 1:
        button = 0x2;
        break;
    case 2:
        button = 0x4;
        break;
    case 3:
        button = 0x8;
        break;
    case 4:
        button = 0x10;
        break;
    case 5:
        button = 0x20;
        break;
    case 6:
        button = 0x40;
        break;
    case 7:
        button = 0x80;
        break;
    case 8:
        button = 0x100;
        break;
    case 9:
        button = 0x200;
        break;
    case 10:
        button = 0x400;
        break;
    case 11:
        button = 0x800;
        break;
    case 12:
        button = 0x1000;
        break;
    case 13:
        button = 0x2000;
        break;
    case 14:
        button = 0x4000;
        break;
    case 15:
        button = 0x8000;
        break;
    }
    return button;
}

float Class_80365488::GetButtonStateTime(int button, bool remap)
{
    return mUnidentified1AC[GetButtonIndex(button, remap)];
}

float Class_80365488::AnalogLeftX()
{
    return mUnidentified19C.x;
}

float Class_80365488::AnalogLeftY()
{
    return mUnidentified19C.y;
}

float Class_80365488::AnalogRightX()
{
    return mUnidentified1A4.x;
}

float Class_80365488::AnalogRightY()
{
    return mUnidentified1A4.y;
}

bool Class_80365488::RumbleActive()
{
    return false;
}

void Class_80365488::StartRumble(float fDuration, float fIntensity, float fFrequency)
{
    WPADControlMotor(m_padIndex, WPAD_MOTOR_RUMBLE);
}

void Class_80365488::StopRumble()
{
    WPADControlMotor(m_padIndex, WPAD_MOTOR_STOP);
}

int Class_80365488::UnidentifiedClassID()
{
    return lbl_806E229C;
}

Class_80365488::~Class_80365488()
{
}
