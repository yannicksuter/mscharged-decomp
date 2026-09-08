#include "NL/platpad.h"
#include "NL/plat/SwappablePad.h"

static unsigned char sPlatPadStorage[8 * sizeof(cPlatPad)];
nlArrayAllocator<cPlatPad> gPlatPadAllocator(reinterpret_cast<cPlatPad*>(sPlatPadStorage), 8);
int gPlatPadClassID = gNextPadClassID++;

void cPlatPad::Update(float dt)
{
    if (!UpdatePadBackend(this))
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
