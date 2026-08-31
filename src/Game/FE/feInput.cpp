#include "Game/FE/feInput.h"

#include "NL/nlMemory.h"

#include <math.h>
#include <string.h>

struct FEPadData
{
    /* 0x00 */ float fButtonInitialDelay[13];
    /* 0x34 */ float fButtonRepeatRate[13];
    /* 0x68 */ float fButtonTimeSinceLastRepeat[13];
    /* 0x9C */ bool bIsPressed[13];
}; // size 0xAC

extern void* lbl_806E1E28;
extern "C" cGlobalPad* fn_802C082C(void* manager, int index);

FEInput* g_pFEInput = 0;
FEPadData g_aFEPadData[4];

static cGlobalPad* GetPad(int pad)
{
    return fn_802C082C(lbl_806E1E28, pad);
}

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
        return GetPad(pad);
    }
    return 0;
}

bool FEInput::IsConnected(eFEINPUT_PAD pad)
{
    if (pad != FE_ALL_PADS)
    {
        return GetPad(pad)->IsConnected();
    }

    for (int i = 0; i < 4; i++)
    {
        if (GetPad(i)->IsConnected())
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
            int buttonIndex = GetPad(i)->Unidentified24(button, remap);
            if ((mUnidentified024[i] & (1 << buttonIndex)) != 0)
            {
                return true;
            }
        }
        return false;
    }

    int buttonIndex = GetPad(pad)->Unidentified24(button, remap);
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
            return GetPad(pad)->IsPressed(button, remap);
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

            cGlobalPad* pPad = GetPad(pad);
            bool ispressed = pPad->IsPressed(button, remap);
            int buttonIndex = pPad->Unidentified24(button, remap);
            if (ispressed && !g_aFEPadData[pad].bIsPressed[buttonIndex])
            {
                ispressed = false;
            }
            if (IsButtonDisabled(pad, button, remap))
            {
                ispressed = false;
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
            return GetPad(pad)->PlatJustPressed(button, remap);
        }

        for (int i = 0; i < 4; i++)
        {
            if (m_bEnableInput[i] && GetPad(i)->PlatJustPressed(button, remap)
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
        return GetPad(pad)->PlatJustPressed(button, remap);
    }

    for (int i = 0; i < 4; i++)
    {
        if (GetPad(i)->PlatJustPressed(button, remap)
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
            return GetPad(pad)->PlatJustReleased(button, remap);
        }

        for (int i = 0; i < 4; i++)
        {
            if (m_bEnableInput[i] && GetPad(i)->PlatJustReleased(button, remap)
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

void FEInput::Update(float)
{
    for (int padIndex = 0; padIndex < 4; padIndex++)
    {
        mUnidentified024[padIndex] = 0;
        for (int buttonIndex = 0; buttonIndex < 13; buttonIndex++)
        {
            FEPadData& data = g_aFEPadData[padIndex];
            cGlobalPad* pPad = GetPad(padIndex);
            int button = pPad->Unidentified28(buttonIndex);
            data.bIsPressed[buttonIndex] = false;

            if (pPad->IsPressed(button, false))
            {
                if (pPad->PlatJustPressed(button, false))
                {
                    data.fButtonTimeSinceLastRepeat[buttonIndex]
                        = pPad->GetButtonStateTime(button, false);
                    data.bIsPressed[buttonIndex] = true;
                }
                else
                {
                    float buttonStateTime = pPad->GetButtonStateTime(button, false);
                    float diff = buttonStateTime - data.fButtonInitialDelay[buttonIndex];
                    bool bShouldRepeat = true;
                    if (!(diff > 0.0001f) && !(fabs(diff) <= 0.0001f))
                    {
                        bShouldRepeat = false;
                    }
                    if (bShouldRepeat)
                    {
                        float diff2 = buttonStateTime
                            - data.fButtonTimeSinceLastRepeat[buttonIndex]
                            - data.fButtonRepeatRate[buttonIndex];
                        bool bShouldRepeat2 = true;
                        if (!(diff2 > 0.0001f) && !(fabs(diff2) <= 0.0001f))
                        {
                            bShouldRepeat2 = false;
                        }
                        if (bShouldRepeat2)
                        {
                            data.fButtonTimeSinceLastRepeat[buttonIndex] = buttonStateTime;
                            data.bIsPressed[buttonIndex] = true;
                        }
                    }
                }
            }
            else
            {
                data.fButtonTimeSinceLastRepeat[buttonIndex] = 0.0f;
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
        int buttonIndex = GetPad(pad)->Unidentified24(button, true);
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
        GetPad(pad)->EnableLeftAnalogToDPadMap();
    }
    else
    {
        GetPad(pad)->DisableLeftAnalogToDPadMap();
    }
}

FEInput::~FEInput()
{
}
