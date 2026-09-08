#include "NL/plat/WiiRemotePad.h"
#include "NL/plat/WiiPad.h"
#include "NL/plat/PlatPadManager.h"
#include "NL/plat/SwappablePad.h"

static unsigned char sWiiRemotePadStorage[8 * sizeof(WiiRemotePad)];
nlArrayAllocator<WiiRemotePad> gWiiRemotePadAllocator(reinterpret_cast<WiiRemotePad*>(sWiiRemotePadStorage), 8);
int* gWiiRemoteButtonRemap;
int gWiiRemotePadClassID = gNextPadClassID++;

WiiRemotePad::WiiRemotePad(int padIndex)
    : PadBackend(padIndex)
    , mCurrentStatus(&mStatusBuffers[0])
    , mPreviousStatus(&mStatusBuffers[1])
{
    for (int i = 0; i < 11; ++i)
        mButtonStateTime[i] = 0.0f;
    Update(0.0f);
    Update(0.0f);
}

void WiiRemotePad::Update(float dt)
{
    if (!UpdatePadBackend(this))
    {
        WiiRemotePadStatus* temp = mCurrentStatus;
        mCurrentStatus = mPreviousStatus;
        mPreviousStatus = temp;
        *mCurrentStatus = *g_pPlatPadManager->GetRemoteStatus(m_padIndex);
        UpdateState(dt);
        PadBackend::Update(dt);
    }
}

void WiiRemotePad::UpdateState(float dt)
{
    mDPDData.Update(&mCurrentStatus->kpad);

    int changed = mCurrentStatus->wpad.button ^ mPreviousStatus->wpad.button;
    for (int i = 0; i < 11; ++i)
    {
        if (changed & GetButtonMask(i))
            mButtonStateTime[i] = 0.0f;
        else
            mButtonStateTime[i] += dt;
    }
}

bool WiiRemotePad::IsConnected()
{
    return true;
}

bool WiiRemotePad::IsPressed(int button, bool remap)
{
    if (remap)
        button = gWiiRemoteButtonRemap[button];
    return (button & mCurrentStatus->wpad.button) != 0;
}

float WiiRemotePad::GetPressure(int button, bool remap)
{
    if (remap)
        button = gWiiRemoteButtonRemap[button];
    return (button & mCurrentStatus->wpad.button) ? 1.0f : 0.0f;
}

float WiiRemotePad::GetPressureDerivative(int button, bool remap)
{
    return 0.0f;
}

bool WiiRemotePad::PlatJustPressed(int button, bool remap)
{
    if (remap)
        button = gWiiRemoteButtonRemap[button];
    return (button & mCurrentStatus->wpad.button) && !(button & mPreviousStatus->wpad.button);
}

bool WiiRemotePad::PlatJustReleased(int button, bool remap)
{
    if (remap)
        button = gWiiRemoteButtonRemap[button];
    return !(button & mCurrentStatus->wpad.button) && (button & mPreviousStatus->wpad.button);
}

int WiiRemotePad::GetButtonIndex(int button, bool remap)
{
    return GetWiiButtonIndex(remap ? gWiiRemoteButtonRemap[button] : button);
}

int WiiRemotePad::GetButtonMask(int buttonIndex)
{
    return GetWiiButtonMask(buttonIndex);
}

float WiiRemotePad::GetButtonStateTime(int button, bool remap)
{
    return mButtonStateTime[GetWiiButtonIndex(remap ? gWiiRemoteButtonRemap[button] : button)];
}

float WiiRemotePad::AnalogLeftX()
{
    return 0.0f;
}

float WiiRemotePad::AnalogLeftY()
{
    return 0.0f;
}

float WiiRemotePad::AnalogRightX()
{
    return 0.0f;
}

float WiiRemotePad::AnalogRightY()
{
    return 0.0f;
}

bool WiiRemotePad::RumbleActive()
{
    return false;
}

void WiiRemotePad::StartRumble(float fDuration, float fIntensity, float fFrequency)
{
    WPADControlMotor(m_padIndex, WPAD_MOTOR_RUMBLE);
}

void WiiRemotePad::StopRumble()
{
    WPADControlMotor(m_padIndex, WPAD_MOTOR_STOP);
}
