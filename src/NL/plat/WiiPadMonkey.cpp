#include "Game/PadMonkey.h"
#include "NL/plat/WiiPad.h"

#include <string.h>

WiiPadMonkey::WiiPadMonkey(int padIndex)
    : PadMonkey(padIndex)
{
    m_prevPressurePtr = &m_prevPressure[0];
    m_currPressurePtr = &m_currPressure[0];
    m_buttonChance = &m_buttonChances[0];

    memset(m_prevPressurePtr, 0, GetButtonCount() * sizeof(float));
    memset(m_currPressurePtr, 0, GetButtonCount() * sizeof(float));

    for (int i = 0; i < GetButtonCount(); ++i)
    {
        m_buttonChance[i] = 0.0f;
    }
}

int WiiPadMonkey::GetButtonIndex(int button, bool remap)
{
    return GetPadButtonIndex(remap ? gWiiFreestyleButtonRemap[button] : button);
}

int WiiPadMonkey::GetButtonMask(int buttonIndex)
{
    return GetPadButtonMask(buttonIndex);
}

int WiiPadMonkey::GetButtonCount()
{
    return 16;
}

WiiPadMonkey::~WiiPadMonkey()
{
}
