#include "revolution/types.h"
#include "NL/nlDLListContainer.inl"
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
        status->clButton |= MapWiiStickToDPad(mAnalogLeft.x, mAnalogLeft.y, WPAD_BUTTON_CL_LEFT, WPAD_BUTTON_CL_RIGHT, WPAD_BUTTON_CL_DOWN, WPAD_BUTTON_CL_UP);
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
    case WPAD_BUTTON_CL_UP:
        buttonIndex = 0;
        break;
    case WPAD_BUTTON_CL_LEFT:
        buttonIndex = 1;
        break;
    case WPAD_BUTTON_CL_ZR:
        buttonIndex = 2;
        break;
    case WPAD_BUTTON_CL_X:
        buttonIndex = 3;
        break;
    case WPAD_BUTTON_CL_A:
        buttonIndex = 4;
        break;
    case WPAD_BUTTON_CL_Y:
        buttonIndex = 5;
        break;
    case WPAD_BUTTON_CL_B:
        buttonIndex = 6;
        break;
    case WPAD_BUTTON_CL_ZL:
        buttonIndex = 7;
        break;
    case 0x100:
        buttonIndex = 8;
        break;
    case WPAD_BUTTON_CL_R:
        buttonIndex = 9;
        break;
    case WPAD_BUTTON_CL_PLUS:
        buttonIndex = 10;
        break;
    case WPAD_BUTTON_CL_HOME:
        buttonIndex = 11;
        break;
    case WPAD_BUTTON_CL_MINUS:
        buttonIndex = 12;
        break;
    case WPAD_BUTTON_CL_L:
        buttonIndex = 13;
        break;
    case WPAD_BUTTON_CL_DOWN:
        buttonIndex = 14;
        break;
    case WPAD_BUTTON_CL_RIGHT:
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
        button = WPAD_BUTTON_CL_UP;
        break;
    case 1:
        button = WPAD_BUTTON_CL_LEFT;
        break;
    case 2:
        button = WPAD_BUTTON_CL_ZR;
        break;
    case 3:
        button = WPAD_BUTTON_CL_X;
        break;
    case 4:
        button = WPAD_BUTTON_CL_A;
        break;
    case 5:
        button = WPAD_BUTTON_CL_Y;
        break;
    case 6:
        button = WPAD_BUTTON_CL_B;
        break;
    case 7:
        button = WPAD_BUTTON_CL_ZL;
        break;
    case 8:
        button = 0x100;
        break;
    case 9:
        button = WPAD_BUTTON_CL_R;
        break;
    case 10:
        button = WPAD_BUTTON_CL_PLUS;
        break;
    case 11:
        button = WPAD_BUTTON_CL_HOME;
        break;
    case 12:
        button = WPAD_BUTTON_CL_MINUS;
        break;
    case 13:
        button = WPAD_BUTTON_CL_L;
        break;
    case 14:
        button = WPAD_BUTTON_CL_DOWN;
        break;
    case 15:
        button = WPAD_BUTTON_CL_RIGHT;
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
