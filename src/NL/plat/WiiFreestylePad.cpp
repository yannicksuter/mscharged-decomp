#include "NL/plat/WiiFreestylePad.h"
#include "NL/plat/WiiPad.h"
#include "NL/plat/PlatPadManager.h"
#include "NL/plat/SwappablePad.h"

static unsigned char sWiiFreestylePadStorage[8 * sizeof(WiiFreestylePad)];
nlArrayAllocator<WiiFreestylePad> gWiiFreestylePadAllocator(reinterpret_cast<WiiFreestylePad*>(sWiiFreestylePadStorage), 8);
int* gWiiFreestyleButtonRemap;
int gWiiFreestylePadClassID = gNextPadClassID++;

WiiFreestylePad::WiiFreestylePad(int padIndex)
    : PadBackend(padIndex)
    , mCurrentStatus(&mStatusBuffers[0])
    , mPreviousStatus(&mStatusBuffers[1])
{
    for (int i = 0; i < 13; ++i)
        mButtonStateTime[i] = 0.0f;
    Update(0.0f);
    Update(0.0f);
}

void WiiFreestylePad::Update(float dt)
{
    if (!UpdatePadBackend(this))
    {
        WiiFreestylePadStatus* temp = mCurrentStatus;
        mCurrentStatus = mPreviousStatus;
        mPreviousStatus = temp;
        *mCurrentStatus = *g_pPlatPadManager->GetFreestyleStatus(m_padIndex);
        ClampWiiStick(&mCurrentStatus->wpad.fsStickX, &mCurrentStatus->wpad.fsStickY);
        mDPDData.Update(&mCurrentStatus->kpad);

        WPADFSStatus* status = &mCurrentStatus->wpad;
        mAnalogLeft.x = status->fsStickX / 56.0f;
        mAnalogLeft.y = status->fsStickY / 56.0f;
        if (m_isLeftAnalogToDPadMapEnabled)
        {
            status->button |= MapWiiStickToDPad(mAnalogLeft.x, mAnalogLeft.y, 1, 2, 4, 8);
        }

        int changed = mCurrentStatus->wpad.button ^ mPreviousStatus->wpad.button;
        for (int i = 0; i < 13; ++i)
        {
            if (changed & GetButtonMask(i))
                mButtonStateTime[i] = 0.0f;
            else
                mButtonStateTime[i] += dt;
        }
        PadBackend::Update(dt);
    }
}

bool WiiFreestylePad::IsConnected()
{
    return true;
}

bool WiiFreestylePad::IsPressed(int button, bool remap)
{
    if (remap)
        button = gWiiFreestyleButtonRemap[button];
    return (button & mCurrentStatus->wpad.button) != 0;
}

float WiiFreestylePad::GetPressure(int button, bool remap)
{
    if (remap)
        button = gWiiFreestyleButtonRemap[button];
    return (button & mCurrentStatus->wpad.button) ? 1.0f : 0.0f;
}

float WiiFreestylePad::GetPressureDerivative(int button, bool remap)
{
    return 0.0f;
}

bool WiiFreestylePad::PlatJustPressed(int button, bool remap)
{
    if (remap)
        button = gWiiFreestyleButtonRemap[button];
    return (button & mCurrentStatus->wpad.button) && !(button & mPreviousStatus->wpad.button);
}

bool WiiFreestylePad::PlatJustReleased(int button, bool remap)
{
    if (remap)
        button = gWiiFreestyleButtonRemap[button];
    return !(button & mCurrentStatus->wpad.button) && (button & mPreviousStatus->wpad.button);
}

int WiiFreestylePad::GetButtonIndex(int button, bool remap)
{
    return GetWiiButtonIndex(remap ? gWiiFreestyleButtonRemap[button] : button);
}

int WiiFreestylePad::GetButtonMask(int buttonIndex)
{
    return GetWiiButtonMask(buttonIndex);
}

float WiiFreestylePad::GetButtonStateTime(int button, bool remap)
{
    return mButtonStateTime[GetWiiButtonIndex(remap ? gWiiFreestyleButtonRemap[button] : button)];
}

float WiiFreestylePad::AnalogLeftX()
{
    return mAnalogLeft.x;
}

float WiiFreestylePad::AnalogLeftY()
{
    return mAnalogLeft.y;
}

float WiiFreestylePad::AnalogRightX()
{
    return 0.0f;
}

float WiiFreestylePad::AnalogRightY()
{
    return 0.0f;
}

bool WiiFreestylePad::RumbleActive()
{
    return false;
}

void WiiFreestylePad::StartRumble(float fDuration, float fIntensity, float fFrequency)
{
    WPADControlMotor(m_padIndex, WPAD_MOTOR_RUMBLE);
}

void WiiFreestylePad::StopRumble()
{
    WPADControlMotor(m_padIndex, WPAD_MOTOR_STOP);
}
