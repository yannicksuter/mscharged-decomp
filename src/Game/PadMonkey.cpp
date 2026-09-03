#include "Game/PadMonkey.h"

#include "NL/nlMath.h"
#include "types.h"

#include <string.h>

extern int lbl_806E1E20;

PadMonkey::PadMonkey(int padIndex)
    : PadBackend(padIndex)
    , m_isConnected(false)
    , m_prevPressurePtr(0)
    , m_currPressurePtr(0)
    , m_unk_0x24(0.0f)
    , m_unk_0x28(0.0f)
    , m_unk_0x2C(0.0f)
    , m_unk_0x30(0.0f)
    , m_unk_0x34(100.0f)
    , m_buttonChance(0)
{
}

void PadMonkey::Update(float dt)
{
    float* p = m_prevPressurePtr;
    m_prevPressurePtr = m_currPressurePtr;
    m_currPressurePtr = p;

    float r = nlRandomf(100.0f, &nlDefaultSeed);
    m_isConnected = m_unk_0x34 > r;

    if (m_isConnected)
    {
        for (int i = 0; i < GetButtonCount(); ++i)
        {
            const float pick = nlRandomf(100.0f, &nlDefaultSeed);
            if (m_buttonChance[i] > pick)
            {
                m_currPressurePtr[i]
                    = 0.5f + 0.5f * nlRandomf(1.0f, &nlDefaultSeed);
            }
            else
            {
                m_currPressurePtr[i] = 0.0f;
            }
        }

        m_unk_0x24 = nlRandomf(2.0f, &nlDefaultSeed) - 1.0f;
        m_unk_0x28 = nlRandomf(2.0f, &nlDefaultSeed) - 1.0f;
        m_unk_0x2C = nlRandomf(2.0f, &nlDefaultSeed) - 1.0f;
        m_unk_0x30 = nlRandomf(2.0f, &nlDefaultSeed) - 1.0f;
    }
    else
    {
        memset(m_currPressurePtr, 0, GetButtonCount() * sizeof(float));
        m_unk_0x24 = 0.0f;
        m_unk_0x28 = 0.0f;
        m_unk_0x2C = 0.0f;
        m_unk_0x30 = 0.0f;
    }

    PadBackend::Update(dt);
}

bool PadMonkey::IsConnected()
{
    return m_isConnected;
}

bool PadMonkey::IsPressed(int button, bool remap)
{
    return m_currPressurePtr[GetButtonIndex(button, remap)] >= 0.5f;
}

float PadMonkey::GetPressure(int button, bool remap)
{
    return m_currPressurePtr[GetButtonIndex(button, remap)];
}

float PadMonkey::GetPressureDerivative(int, bool)
{
    return 0.0f;
}

bool PadMonkey::PlatJustPressed(int button, bool remap)
{
    const int idx = GetButtonIndex(button, remap);

    bool pressed = false;
    if (m_currPressurePtr[idx] >= 0.5f && m_prevPressurePtr[idx] < 0.5f)
    {
        pressed = true;
    }
    return pressed;
}

bool PadMonkey::PlatJustReleased(int button, bool remap)
{
    const int idx = GetButtonIndex(button, remap);

    bool released = false;
    if (m_currPressurePtr[idx] < 0.5f && m_prevPressurePtr[idx] >= 0.5f)
    {
        released = true;
    }
    return released;
}

float PadMonkey::GetButtonStateTime(int button, bool remap)
{
    return 0.0f;
}

float PadMonkey::AnalogLeftX()
{
    return m_unk_0x24;
}

float PadMonkey::AnalogLeftY()
{
    return m_unk_0x28;
}

float PadMonkey::AnalogRightX()
{
    return m_unk_0x2C;
}

float PadMonkey::AnalogRightY()
{
    return m_unk_0x30;
}

bool PadMonkey::RumbleActive()
{
    return false;
}

void PadMonkey::StartRumble(float, float, float)
{
}

void PadMonkey::StopRumble()
{
}

void PadMonkey::SetButtonChance(int button, float pct)
{
    m_buttonChance[GetButtonIndex(button, false)] = pct;
}

int PadMonkey::sClassID = lbl_806E1E20++;
