#include "NL/plat/WiiClassicPad.h"
#include "NL/plat/WiiPad.h"
#include "NL/plat/PlatPadManager.h"
#include "NL/plat/SwappablePad.h"

static unsigned char sWiiClassicPadStorage[8 * sizeof(WiiClassicPad)];
nlArrayAllocator<WiiClassicPad> gWiiClassicPadAllocator(reinterpret_cast<WiiClassicPad*>(sWiiClassicPadStorage), 8);
int* gWiiClassicButtonRemap;
int gWiiClassicPadClassID = gNextPadClassID++;

WiiClassicPad::WiiClassicPad(int padIndex)
    : PadBackend(padIndex)
    , mCurrentStatus(&mStatusBuffers[0])
    , mPreviousStatus(&mStatusBuffers[1])
{
    for (int i = 0; i < 16; ++i)
        mButtonStateTime[i] = 0.0f;
    Update(0.0f);
    Update(0.0f);
}

void WiiClassicPad::Update(float dt)
{
    if (!UpdatePadBackend(this))
    {
        WiiClassicPadStatus* temp = mCurrentStatus;
        mCurrentStatus = mPreviousStatus;
        mPreviousStatus = temp;
        *mCurrentStatus = *g_pPlatPadManager->GetClassicStatus(m_padIndex);

        signed char x = mCurrentStatus->wpad.clLStickX;
        signed char y = mCurrentStatus->wpad.clLStickY;
        ClampWiiStick(&x, &y);
        mCurrentStatus->wpad.clLStickX = x;
        mCurrentStatus->wpad.clLStickY = y;
        x = mCurrentStatus->wpad.clRStickX;
        y = mCurrentStatus->wpad.clRStickY;
        ClampWiiStick(&x, &y);
        mCurrentStatus->wpad.clRStickX = x;
        mCurrentStatus->wpad.clRStickY = y;

        UpdateState(dt);
        PadBackend::Update(dt);
    }
}

void WiiClassicPad::UpdateState(float dt)
{
    mDPDData.Update(&mCurrentStatus->kpad);
    WPADCLStatus* status = &mCurrentStatus->wpad;
    mAnalogLeft.x = status->clLStickX / 56.0f;
    mAnalogLeft.y = status->clLStickY / 56.0f;
    mAnalogRight.x = status->clRStickX / 56.0f;
    mAnalogRight.y = status->clRStickY / 56.0f;
    if (m_isLeftAnalogToDPadMapEnabled)
    {
        status->clButton |= MapWiiStickToDPad(mAnalogLeft.x, mAnalogLeft.y, 2, 0x8000, 0x4000, 1);
    }

    int changed = mCurrentStatus->wpad.clButton ^ mPreviousStatus->wpad.clButton;
    for (int i = 0; i < 16; ++i)
    {
        if (changed & GetButtonMask(i))
            mButtonStateTime[i] = 0.0f;
        else
            mButtonStateTime[i] += dt;
    }
}

bool WiiClassicPad::IsConnected()
{
    return true;
}

bool WiiClassicPad::IsPressed(int button, bool remap)
{
    if (remap)
        button = gWiiClassicButtonRemap[button];
    return (button & mCurrentStatus->wpad.clButton) != 0;
}

float WiiClassicPad::GetPressure(int button, bool remap)
{
    if (remap)
        button = gWiiClassicButtonRemap[button];
    return (button & mCurrentStatus->wpad.clButton) ? 1.0f : 0.0f;
}

float WiiClassicPad::GetPressureDerivative(int button, bool remap)
{
    return 0.0f;
}

bool WiiClassicPad::PlatJustPressed(int button, bool remap)
{
    if (remap)
        button = gWiiClassicButtonRemap[button];
    return (button & mCurrentStatus->wpad.clButton) && !(button & mPreviousStatus->wpad.clButton);
}

bool WiiClassicPad::PlatJustReleased(int button, bool remap)
{
    if (remap)
        button = gWiiClassicButtonRemap[button];
    return !(button & mCurrentStatus->wpad.clButton) && (button & mPreviousStatus->wpad.clButton);
}

int WiiClassicPad::GetButtonIndex(int button, bool remap)
{
    if (remap)
        button = gWiiClassicButtonRemap[button];
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

int WiiClassicPad::GetButtonMask(int buttonIndex)
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

float WiiClassicPad::GetButtonStateTime(int button, bool remap)
{
    return mButtonStateTime[GetButtonIndex(button, remap)];
}

float WiiClassicPad::AnalogLeftX()
{
    return mAnalogLeft.x;
}

float WiiClassicPad::AnalogLeftY()
{
    return mAnalogLeft.y;
}

float WiiClassicPad::AnalogRightX()
{
    return mAnalogRight.x;
}

float WiiClassicPad::AnalogRightY()
{
    return mAnalogRight.y;
}

bool WiiClassicPad::RumbleActive()
{
    return false;
}

void WiiClassicPad::StartRumble(float fDuration, float fIntensity, float fFrequency)
{
    WPADControlMotor(m_padIndex, WPAD_MOTOR_RUMBLE);
}

void WiiClassicPad::StopRumble()
{
    WPADControlMotor(m_padIndex, WPAD_MOTOR_STOP);
}
