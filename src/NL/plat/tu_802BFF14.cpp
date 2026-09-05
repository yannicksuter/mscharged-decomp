#include "NL/globalpad.h"

extern int lbl_806E1E20;

cGlobalPad::cGlobalPad(int padIndex)
    : PadBackend(padIndex)
    , mBackend(0)
{
}

cGlobalPad::~cGlobalPad()
{
    if (mBackend != 0)
        delete mBackend;
}

void cGlobalPad::Update(float deltaTime)
{
    if (mBackend != 0)
    {
        if (m_isLeftAnalogToDPadMapEnabled)
            mBackend->EnableLeftAnalogToDPadMap();
        else
            mBackend->DisableLeftAnalogToDPadMap();
        mBackend->Update(deltaTime);
    }
    PadBackend::Update(deltaTime);
}

bool cGlobalPad::IsConnected()
{
    return mBackend != 0 && mBackend->IsConnected();
}

bool cGlobalPad::IsPressed(int button, bool remap)
{
    if (mBackend != 0)
        return mBackend->IsPressed(button, remap);
    return false;
}

float cGlobalPad::GetPressure(int button, bool remap)
{
    if (mBackend != 0)
        return mBackend->GetPressure(button, remap);
    return 0.0f;
}

float cGlobalPad::GetPressureDerivative(int button, bool remap)
{
    if (mBackend != 0)
        return mBackend->GetPressureDerivative(button, remap);
    return 0.0f;
}

bool cGlobalPad::PlatJustPressed(int button, bool remap)
{
    if (mBackend != 0)
        return mBackend->PlatJustPressed(button, remap);
    return false;
}

bool cGlobalPad::PlatJustReleased(int button, bool remap)
{
    if (mBackend != 0)
        return mBackend->PlatJustReleased(button, remap);
    return false;
}

int cGlobalPad::GetButtonIndex(int button, bool remap)
{
    if (mBackend != 0)
        return mBackend->GetButtonIndex(button, remap);
    return 0;
}

int cGlobalPad::GetButtonMask(int buttonIndex)
{
    if (mBackend != 0)
        return mBackend->GetButtonMask(buttonIndex);
    return 0;
}

float cGlobalPad::GetButtonStateTime(int button, bool remap)
{
    if (mBackend != 0)
        return mBackend->GetButtonStateTime(button, remap);
    return 0.0f;
}

float cGlobalPad::AnalogLeftX()
{
    if (mBackend != 0)
        return mBackend->AnalogLeftX();
    return 0.0f;
}

float cGlobalPad::AnalogLeftY()
{
    if (mBackend != 0)
        return mBackend->AnalogLeftY();
    return 0.0f;
}

float cGlobalPad::AnalogRightX()
{
    if (mBackend != 0)
        return mBackend->AnalogRightX();
    return 0.0f;
}

float cGlobalPad::AnalogRightY()
{
    if (mBackend != 0)
        return mBackend->AnalogRightY();
    return 0.0f;
}

bool cGlobalPad::RumbleActive()
{
    if (mBackend != 0)
        return mBackend->RumbleActive();
    return false;
}

void cGlobalPad::StartRumble(float fDuration, float fIntensity, float fFrequency)
{
    if (mBackend != 0)
        mBackend->StartRumble(fDuration, fIntensity, fFrequency);
}

void cGlobalPad::StopRumble()
{
    if (mBackend != 0)
        mBackend->StopRumble();
}

int cGlobalPad::sUnidentifiedClassID = lbl_806E1E20++;
