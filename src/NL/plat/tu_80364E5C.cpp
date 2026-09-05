#include "NL/plat/tu_80364E5C.h"
#include "NL/plat/tu_80364118.h"

extern int lbl_806E1E20;

static unsigned char lbl_80586F70[8 * sizeof(Class_80364E5C)];
nlArrayAllocator<Class_80364E5C> lbl_806E2280(reinterpret_cast<Class_80364E5C*>(lbl_80586F70), 8);
int* lbl_806E2288;
int lbl_806E228C = lbl_806E1E20++;

Class_80364E5C::Class_80364E5C(int padIndex)
    : PadBackend(padIndex)
    , mUnidentified01C(&mUnidentified024[0])
    , mUnidentified020(&mUnidentified024[1])
{
    fn_80364604(&mUnidentified1D0);
    for (int i = 0; i < 13; ++i)
        mUnidentified19C[i] = 0.0f;
    Update(0.0f);
    Update(0.0f);
}

void Class_80364E5C::Update(float dt)
{
    if (!fn_80365E84(this))
    {
        PlatPadStatus_80375ED4* temp = mUnidentified01C;
        mUnidentified01C = mUnidentified020;
        mUnidentified020 = temp;
        *mUnidentified01C = *fn_80375ED4(lbl_806E2478, m_padIndex);
        fn_80364118(&mUnidentified01C->wpad.fsStickX, &mUnidentified01C->wpad.fsStickY);
        fn_80364650(&mUnidentified1D0, &mUnidentified01C->kpad);

        WPADFSStatus* status = &mUnidentified01C->wpad;
        mUnidentified194.x = status->fsStickX / 56.0f;
        mUnidentified194.y = status->fsStickY / 56.0f;
        if (m_isLeftAnalogToDPadMapEnabled)
        {
            status->button |= fn_80364434(mUnidentified194.x, mUnidentified194.y, 1, 2, 4, 8);
        }

        int changed = mUnidentified01C->wpad.button ^ mUnidentified020->wpad.button;
        for (int i = 0; i < 13; ++i)
        {
            if (changed & GetButtonMask(i))
                mUnidentified19C[i] = 0.0f;
            else
                mUnidentified19C[i] += dt;
        }
        PadBackend::Update(dt);
    }
}

bool Class_80364E5C::IsConnected()
{
    return true;
}

bool Class_80364E5C::IsPressed(int button, bool remap)
{
    if (remap)
        button = lbl_806E2288[button];
    return (button & mUnidentified01C->wpad.button) != 0;
}

float Class_80364E5C::GetPressure(int button, bool remap)
{
    if (remap)
        button = lbl_806E2288[button];
    return (button & mUnidentified01C->wpad.button) ? 1.0f : 0.0f;
}

float Class_80364E5C::GetPressureDerivative(int button, bool remap)
{
    return 0.0f;
}

bool Class_80364E5C::PlatJustPressed(int button, bool remap)
{
    if (remap)
        button = lbl_806E2288[button];
    return (button & mUnidentified01C->wpad.button) && !(button & mUnidentified020->wpad.button);
}

bool Class_80364E5C::PlatJustReleased(int button, bool remap)
{
    if (remap)
        button = lbl_806E2288[button];
    return !(button & mUnidentified01C->wpad.button) && (button & mUnidentified020->wpad.button);
}

int Class_80364E5C::GetButtonIndex(int button, bool remap)
{
    return fn_80364298(remap ? lbl_806E2288[button] : button);
}

int Class_80364E5C::GetButtonMask(int buttonIndex)
{
    return fn_803643A8(buttonIndex);
}

float Class_80364E5C::GetButtonStateTime(int button, bool remap)
{
    return mUnidentified19C[fn_80364298(remap ? lbl_806E2288[button] : button)];
}

float Class_80364E5C::AnalogLeftX()
{
    return mUnidentified194.x;
}

float Class_80364E5C::AnalogLeftY()
{
    return mUnidentified194.y;
}

float Class_80364E5C::AnalogRightX()
{
    return 0.0f;
}

float Class_80364E5C::AnalogRightY()
{
    return 0.0f;
}

bool Class_80364E5C::RumbleActive()
{
    return false;
}

void Class_80364E5C::StartRumble(float fDuration, float fIntensity, float fFrequency)
{
    WPADControlMotor(m_padIndex, WPAD_MOTOR_RUMBLE);
}

void Class_80364E5C::StopRumble()
{
    WPADControlMotor(m_padIndex, WPAD_MOTOR_STOP);
}

int Class_80364E5C::UnidentifiedClassID()
{
    return lbl_806E228C;
}

Class_80364E5C::~Class_80364E5C()
{
}
