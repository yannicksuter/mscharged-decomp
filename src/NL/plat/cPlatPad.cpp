#include "NL/platpad.h"

extern int lbl_806E1E20;

static unsigned char lbl_80585FF0[8 * sizeof(cPlatPad)];
nlArrayAllocator<cPlatPad> lbl_806E2260(reinterpret_cast<cPlatPad*>(lbl_80585FF0), 8);
int lbl_806E2268 = lbl_806E1E20++;

void cPlatPad::Update(float dt)
{
    if (!fn_80365E84(this))
        PadBackend::Update(dt);
}

bool cPlatPad::IsConnected()
{
    return false;
}

bool cPlatPad::IsPressed(int button, bool remap)
{
    return false;
}

float cPlatPad::GetPressure(int button, bool remap)
{
    return 0.0f;
}

float cPlatPad::GetPressureDerivative(int button, bool remap)
{
    return 0.0f;
}

bool cPlatPad::PlatJustPressed(int button, bool remap)
{
    return false;
}

bool cPlatPad::PlatJustReleased(int button, bool remap)
{
    return false;
}

int cPlatPad::GetButtonMask(int buttonIndex)
{
    return 0;
}

int cPlatPad::GetButtonIndex(int button, bool remap)
{
    return 0;
}

float cPlatPad::GetButtonStateTime(int button, bool remap)
{
    return 0.0f;
}

float cPlatPad::AnalogLeftX()
{
    return 0.0f;
}

float cPlatPad::AnalogLeftY()
{
    return 0.0f;
}

float cPlatPad::AnalogRightX()
{
    return 0.0f;
}

float cPlatPad::AnalogRightY()
{
    return 0.0f;
}

bool cPlatPad::RumbleActive()
{
    return false;
}

void cPlatPad::StartRumble(float fDuration, float fIntensity, float fFrequency)
{
}

void cPlatPad::StopRumble()
{
}

int cPlatPad::UnidentifiedClassID()
{
    return lbl_806E2268;
}

cPlatPad::~cPlatPad()
{
}
