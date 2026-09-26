#include "Game/FE/feInput.h"

#include "NL/globalpad.h"

#include "NL/nlMath.h"
#include "NL/nlMemory.h"

#include <string.h>

struct FEPadData
{
    /* 0x00 */ float fButtonInitialDelay[13];
    /* 0x34 */ float fButtonRepeatRate[13];
    /* 0x68 */ float fButtonTimeSinceLastRepeat[13];
    /* 0x9C */ bool bIsPressed[13];
}; // size 0xAC


FEInput* g_pFEInput = 0;
FEPadData g_aFEPadData[4];

void FEInput::Initialize()
{
    g_pFEInput = new ((u8*)nlMalloc(sizeof(FEInput), 8, false)) FEInput();
}

void FEInput::Reset(bool arg0)
{
    for (int i = 0; i < 4; i++)
    {
        m_bEnableInput[i] = true;
        mUnidentified024[i] = 0;
    }

    if (arg0)
    {
        m_InputLockDepth = 0;
        m_bInputAllowed = true;
    }

    EnableAnalogToDPadMapping(FE_ALL_PADS, false);
    memset(g_aFEPadData, 0, sizeof(g_aFEPadData));
}

cGlobalPad* FEInput::GetGlobalPad(eFEINPUT_PAD pad) const
{
    if (pad >= 4)
    {
        return 0;
    }
    if (!m_bInputAllowed)
    {
        return 0;
    }
    if (m_bEnableInput[pad])
    {
        return g_pPadManager->GetPad(pad);
    }
    return 0;
}

bool FEInput::IsConnected(eFEINPUT_PAD pad)
{
    if (pad != FE_ALL_PADS)
    {
        return g_pPadManager->GetPad(pad)->IsConnected();
    }

    for (int i = 0; i < 4; i++)
    {
        if (g_pPadManager->GetPad(i)->IsConnected())
        {
            return true;
        }
    }
    return false;
}

bool FEInput::IsButtonDisabled(eFEINPUT_PAD pad, int button, bool remap) const
{
    if (pad == FE_NO_PAD)
    {
        return false;
    }

    if (pad == FE_ALL_PADS)
    {
        for (int i = 0; i < 4; i++)
        {
            int buttonIndex = g_pPadManager->GetPad(i)->GetButtonIndex(button, remap);
            if ((mUnidentified024[i] & (1 << buttonIndex)) != 0)
            {
                return true;
            }
        }
        return false;
    }

    int buttonIndex = g_pPadManager->GetPad(pad)->GetButtonIndex(button, remap);
    return (mUnidentified024[pad] & (1 << buttonIndex)) != 0;
}

bool FEInput::IsPressed(eFEINPUT_PAD pad, int button, bool remap, eFEINPUT_PAD* pOutPad)
{
    if (m_bInputAllowed)
    {
        if (pad != FE_ALL_PADS)
        {
            if (!m_bEnableInput[pad])
            {
                return false;
            }
            if (IsButtonDisabled(pad, button, remap))
            {
                return false;
            }
            return g_pPadManager->GetPad(pad)->IsPressed(button, remap);
        }

        for (int i = 0; i < 4; i++)
        {
            if (m_bEnableInput[i] && IsPressed((eFEINPUT_PAD)i, button, remap, 0)
                && !IsButtonDisabled(pad, button, remap))
            {
                if (pOutPad)
                {
                    *pOutPad = (eFEINPUT_PAD)i;
                }
                return true;
            }
        }
    }
    return false;
}

bool FEInput::IsAutoPressed(eFEINPUT_PAD pad, int button, bool remap, eFEINPUT_PAD* pOutPad)
{
    if (m_bInputAllowed)
    {
        if (pad != FE_ALL_PADS)
        {
            if (!m_bEnableInput[pad])
            {
                return false;
            }

            bool ispressed = g_pPadManager->GetPad(pad)->IsPressed(button, remap);
            int buttonIndex
                = g_pPadManager->GetPad(pad)->GetButtonIndex(button, remap);
            if (ispressed)
            {
                if (!g_aFEPadData[pad].bIsPressed[buttonIndex])
                {
                    ispressed = false;
                }
                else if (IsButtonDisabled(pad, button, remap))
                {
                    ispressed = false;
                }
            }
            return ispressed;
        }

        for (int i = 0; i < 4; i++)
        {
            if (m_bEnableInput[i] && IsAutoPressed((eFEINPUT_PAD)i, button, remap, 0)
                && !IsButtonDisabled(pad, button, remap))
            {
                if (pOutPad)
                {
                    *pOutPad = (eFEINPUT_PAD)i;
                }
                return true;
            }
        }
    }
    return false;
}

bool FEInput::JustPressed(eFEINPUT_PAD pad, int button, bool remap, eFEINPUT_PAD* pOutPad)
{
    if (m_bInputAllowed)
    {
        if (pad != FE_ALL_PADS)
        {
            if (!m_bEnableInput[pad])
            {
                return false;
            }
            if (IsButtonDisabled(pad, button, remap))
            {
                return false;
            }
            return g_pPadManager->GetPad(pad)->PlatJustPressed(button, remap);
        }

        for (int i = 0; i < 4; i++)
        {
            if (m_bEnableInput[i] && g_pPadManager->GetPad(i)->PlatJustPressed(button, remap)
                && !IsButtonDisabled(pad, button, remap))
            {
                if (pOutPad)
                {
                    *pOutPad = (eFEINPUT_PAD)i;
                }
                return true;
            }
        }
    }
    return false;
}

bool FEInput::PlatJustPressed(eFEINPUT_PAD pad, int button, bool remap, eFEINPUT_PAD* pOutPad)
{
    if (pad != FE_ALL_PADS)
    {
        if (IsButtonDisabled(pad, button, remap))
        {
            return false;
        }
        return g_pPadManager->GetPad(pad)->PlatJustPressed(button, remap);
    }

    for (int i = 0; i < 4; i++)
    {
        if (g_pPadManager->GetPad(i)->PlatJustPressed(button, remap)
            && !IsButtonDisabled(pad, button, remap))
        {
            if (pOutPad)
            {
                *pOutPad = (eFEINPUT_PAD)i;
            }
            return true;
        }
    }
    return false;
}

bool FEInput::JustReleased(eFEINPUT_PAD pad, int button, bool remap, eFEINPUT_PAD* pOutPad)
{
    if (m_bInputAllowed)
    {
        if (pad != FE_ALL_PADS)
        {
            if (!m_bEnableInput[pad])
            {
                return false;
            }
            if (IsButtonDisabled(pad, button, remap))
            {
                return false;
            }
            return g_pPadManager->GetPad(pad)->PlatJustReleased(button, remap);
        }

        for (int i = 0; i < 4; i++)
        {
            if (m_bEnableInput[i] && g_pPadManager->GetPad(i)->PlatJustReleased(button, remap)
                && !IsButtonDisabled(pad, button, remap))
            {
                if (pOutPad)
                {
                    *pOutPad = (eFEINPUT_PAD)i;
                }
                return true;
            }
        }
    }
    return false;
}

void FEInput::EnableInputIfSceneHasFocus(BaseSceneHandler* pSceneHandler)
{
    int depth = m_InputLockDepth;
    if (depth == 0 || m_nExclusiveInputSceneHashIDStack[depth - 1].m_pBaseSceneHandler == pSceneHandler)
    {
        m_bInputAllowed = true;
        return;
    }
    m_bInputAllowed = false;
}

void FEInput::PushExclusiveInputLock(BaseSceneHandler* pRequestingSceneHandler, int customID)
{
    m_nExclusiveInputSceneHashIDStack[m_InputLockDepth].m_pBaseSceneHandler = pRequestingSceneHandler;
    m_nExclusiveInputSceneHashIDStack[m_InputLockDepth].m_customID = customID;
    m_InputLockDepth++;
}

void FEInput::PopExclusiveInputLock(BaseSceneHandler*)
{
    m_InputLockDepth--;
    InputLockEntry& entry = m_nExclusiveInputSceneHashIDStack[m_InputLockDepth];
    entry.m_pBaseSceneHandler = 0;
    entry.m_customID = -1;
}

bool FEInput::HasInputLock(BaseSceneHandler* pRequestingSceneHandler) const
{
    int depth = m_InputLockDepth;
    if (depth == 0)
    {
        return false;
    }
    return m_nExclusiveInputSceneHashIDStack[depth - 1].m_pBaseSceneHandler == pRequestingSceneHandler;
}

static inline bool IsAtLeast(float value, float threshold)
{
    return value - threshold > 0.0001f || nlNear(value, threshold);
}

void FEInput::Update(float)
{
    for (int i = 0; i < 4; i++)
    {
        mUnidentified024[i] = 0;
        for (int buttonindex = 0; buttonindex < 13; buttonindex++)
        {
            int button = g_pPadManager->GetPad(i)->GetButtonMask(buttonindex);
            g_aFEPadData[i].bIsPressed[buttonindex] = false;

            if (g_pPadManager->GetPad(i)->IsPressed(button, false))
            {
                if (g_pPadManager->GetPad(i)->PlatJustPressed(button, false))
                {
                    g_aFEPadData[i].fButtonTimeSinceLastRepeat[buttonindex]
                        = g_pPadManager->GetPad(i)->GetButtonStateTime(button, false);
                    g_aFEPadData[i].bIsPressed[buttonindex] = true;
                }
                else
                {
                    float ontime = g_pPadManager->GetPad(i)->GetButtonStateTime(button, false);
                    float lastrepeat = g_aFEPadData[i].fButtonTimeSinceLastRepeat[buttonindex];
                    float repeatrate = g_aFEPadData[i].fButtonRepeatRate[buttonindex];
                    if (IsAtLeast((double)ontime, g_aFEPadData[i].fButtonInitialDelay[buttonindex])
                        && IsAtLeast(ontime - lastrepeat, repeatrate))
                    {
                        g_aFEPadData[i].fButtonTimeSinceLastRepeat[buttonindex] = ontime;
                        g_aFEPadData[i].bIsPressed[buttonindex] = true;
                    }
                }
            }
            else
            {
                g_aFEPadData[i].fButtonTimeSinceLastRepeat[buttonindex] = 0.0f;
            }
        }
    }
}

void FEInput::SetAutoRepeatParams(
    eFEINPUT_PAD pad, int button, float initialdelay, float repeatrate)
{
    if (pad == FE_ALL_PADS)
    {
        for (int i = 0; i < 4; i++)
        {
            SetAutoRepeatParams((eFEINPUT_PAD)i, button, initialdelay, repeatrate);
        }
    }
    else
    {
        int buttonIndex = g_pPadManager->GetPad(pad)->GetButtonIndex(button, true);
        g_aFEPadData[pad].fButtonInitialDelay[buttonIndex] = initialdelay;
        g_aFEPadData[pad].fButtonRepeatRate[buttonIndex] = repeatrate;
    }
}

void FEInput::EnableAnalogToDPadMapping(eFEINPUT_PAD pad, bool enable)
{
    if (pad == FE_ALL_PADS)
    {
        for (int i = 0; i < 4; i++)
        {
            EnableAnalogToDPadMapping((eFEINPUT_PAD)i, enable);
        }
    }
    else if (enable)
    {
        g_pPadManager->GetPad(pad)->EnableLeftAnalogToDPadMap();
    }
    else
    {
        g_pPadManager->GetPad(pad)->DisableLeftAnalogToDPadMap();
    }
}

FEInput::~FEInput()
{
}
