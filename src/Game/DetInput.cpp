#include <revolution/pad.h>
#include "Game/DetInput.h"

#include "Game/PadMonkey.h"
#include "NL/globalpad.h"
#include "NL/platpad.h"
#include "NL/plat/WiiPad.h"
#include "NL/plat/WiiRemotePad.h"
#include "NL/plat/WiiFreestylePad.h"
#include "NL/plat/GameCubePad.h"
#include "Game/NetworkInput.h"

u8 DetInput::GetConnectionStatus()
{
    return m_nConnected;
}

u8 DetInput::GetControllerType()
{
    return m_nConnected;
}

nlVector3* DetInput::GetRemoteAcceleration()
{
    return &m_v3RevRemoteAccel;
}

nlVector3* DetInput::GetFreestyleAcceleration()
{
    return &m_v3RevFreeStyleAccel;
}

bool DetInput::IsPressed(int button, bool remap)
{
    if (remap)
    {
        int* pArray;
        switch (m_nConnected)
        {
        case 0:
        case 3:
            pArray = gGameCubePadButtonMap;
            break;
        case 1:
            pArray = gWiiRemoteButtonRemap;
            break;
        case 2:
            pArray = gWiiFreestyleButtonRemap;
            break;
        default:
            pArray = gGameCubePadButtonMap;
            break;
        }
        button = pArray[button];
    }
    return (m_ButtonBitfield & button) != 0;
}

bool DetInput::JustPressed(int button, bool remap)
{
    if (remap)
    {
        int* pArray;
        switch (m_nConnected)
        {
        case 0:
        case 3:
            pArray = gGameCubePadButtonMap;
            break;
        case 1:
            pArray = gWiiRemoteButtonRemap;
            break;
        case 2:
            pArray = gWiiFreestyleButtonRemap;
            break;
        default:
            pArray = gGameCubePadButtonMap;
            break;
        }
        button = pArray[button];
    }
    bool result = (m_ButtonBitfield & button) != 0;
    if (result)
    {
        result = (m_pPrevInput->m_ButtonBitfield & button) == 0;
    }
    return result;
}

bool DetInput::JustReleased(int button, bool remap)
{
    if (remap)
    {
        int* pArray;
        switch (m_nConnected)
        {
        case 0:
        case 3:
            pArray = gGameCubePadButtonMap;
            break;
        case 1:
            pArray = gWiiRemoteButtonRemap;
            break;
        case 2:
            pArray = gWiiFreestyleButtonRemap;
            break;
        default:
            pArray = gGameCubePadButtonMap;
            break;
        }
        button = pArray[button];
    }
    bool result = (m_ButtonBitfield & button) == 0;
    if (result)
    {
        result = (m_pPrevInput->m_ButtonBitfield & button) != 0;
    }
    return result;
}

void DetInput::UpdatePolarAnalog()
{
    nlCartesianToPolar(m_PolarAnalogLeft, m_AnalogLeftX, m_AnalogLeftY);
    nlCartesianToPolar(m_PolarAnalogRight, m_AnalogRightX, m_AnalogRightY);
}

void DetInput::UpdateButtonStateTicks()
{
    for (int i = 0; i < 13; ++i)
    {
        int button;
        switch (m_nConnected)
        {
        case 0:
        case 3:
            button = GetPadButtonMask(i);
            break;
        case 1:
        case 2:
            button = GetWiiButtonMask(i);
            break;
        }

        bool result = JustReleased(button, false);
        if (!result)
        {
            result = JustPressed(button, false);
        }

        if (result)
        {
            m_buttonStateTicks[i] = 0;
        }
        else
        {
            ++m_buttonStateTicks[i];
        }
    }
}

int DetInput::GetButtonStateTicks(int button, bool remap)
{
    if (remap)
    {
        int* pArray;
        switch (m_nConnected)
        {
        case 0:
        case 3:
            pArray = gGameCubePadButtonMap;
            break;
        case 1:
            pArray = gWiiRemoteButtonRemap;
            break;
        case 2:
            pArray = gWiiFreestyleButtonRemap;
            break;
        default:
            pArray = gGameCubePadButtonMap;
            break;
        }
        button = pArray[button];
    }

    switch (m_nConnected)
    {
    case 0:
    case 3:
        return m_buttonStateTicks[GetPadButtonIndex(button)];
    case 1:
    case 2:
        return m_buttonStateTicks[GetWiiButtonIndex(button)];
    default:
        return 0;
    }
}

void DetInput::ResetButtonStateTicks(int button, bool remap)
{
    if (remap)
    {
        int* pArray;
        switch (m_nConnected)
        {
        case 0:
        case 3:
            pArray = gGameCubePadButtonMap;
            break;
        case 1:
            pArray = gWiiRemoteButtonRemap;
            break;
        case 2:
            pArray = gWiiFreestyleButtonRemap;
            break;
        default:
            pArray = gGameCubePadButtonMap;
            break;
        }
        button = pArray[button];
    }

    switch (m_nConnected)
    {
    case 0:
    case 3:
    {
        int buttonIndex = GetPadButtonIndex(button);
        m_buttonStateTicks[buttonIndex] = 0;
        break;
    }
    case 1:
    case 2:
    {
        int buttonIndex = GetWiiButtonIndex(button);
        m_buttonStateTicks[buttonIndex] = 0;
        break;
    }
    }
}

DetInput::DetInput()
{
    m_AnalogLeftX = 0.0f;
    m_AnalogLeftY = 0.0f;
    m_AnalogRightX = 0.0f;
    m_AnalogRightY = 0.0f;
    m_nConnected = 2;
    m_ButtonBitfield = 0;
    m_LeftTrigger = 0;
    m_RightTrigger = 0;
    m_v3RevRemoteAccel.x = 0.0f;
    m_v3RevRemoteAccel.y = 0.0f;
    m_v3RevRemoteAccel.z = 0.0f;
    m_v3RevFreeStyleAccel.x = 0.0f;
    m_v3RevFreeStyleAccel.y = 0.0f;
    m_v3RevFreeStyleAccel.z = 0.0f;
    m_nRevDPDNumTargets = 0;
    m_v2RevDPDCoord.x = 0.0f;
    m_v2RevDPDCoord.y = 0.0f;
    m_buttonStateTicks[0] = 0;
    m_buttonStateTicks[1] = 0;
    m_buttonStateTicks[2] = 0;
    m_buttonStateTicks[3] = 0;
    m_buttonStateTicks[4] = 0;
    m_buttonStateTicks[5] = 0;
    m_buttonStateTicks[6] = 0;
    m_buttonStateTicks[7] = 0;
    m_buttonStateTicks[8] = 0;
    m_buttonStateTicks[9] = 0;
    m_buttonStateTicks[10] = 0;
    m_buttonStateTicks[11] = 0;
    m_buttonStateTicks[12] = 0;
    m_pMyUser = 0;
    m_pPrevInput = 0;
    m_PolarAnalogLeft.a = 0;
    m_PolarAnalogLeft.r = 0.0f;
    m_PolarAnalogRight.a = 0;
    m_PolarAnalogRight.r = 0.0f;
    m_aRemapAngle = 0;
}

void DetInput::CopyState(const DetInput& input)
{
    m_AnalogLeftX = input.m_AnalogLeftX;
    m_AnalogLeftY = input.m_AnalogLeftY;
    m_AnalogRightX = input.m_AnalogRightX;
    m_AnalogRightY = input.m_AnalogRightY;
    m_nConnected = input.m_nConnected;
    m_ButtonBitfield = input.m_ButtonBitfield;
    m_LeftTrigger = input.m_LeftTrigger;
    m_RightTrigger = input.m_RightTrigger;
    m_v3RevRemoteAccel = input.m_v3RevRemoteAccel;
    m_v3RevFreeStyleAccel = input.m_v3RevFreeStyleAccel;
    m_nRevDPDNumTargets = input.m_nRevDPDNumTargets;
    m_v2RevDPDCoord = input.m_v2RevDPDCoord;
    m_PolarAnalogLeft = input.m_PolarAnalogLeft;
    m_PolarAnalogRight = input.m_PolarAnalogRight;
    m_buttonStateTicks[0] = input.m_buttonStateTicks[0];
    m_buttonStateTicks[1] = input.m_buttonStateTicks[1];
    m_buttonStateTicks[2] = input.m_buttonStateTicks[2];
    m_buttonStateTicks[3] = input.m_buttonStateTicks[3];
    m_buttonStateTicks[4] = input.m_buttonStateTicks[4];
    m_buttonStateTicks[5] = input.m_buttonStateTicks[5];
    m_buttonStateTicks[6] = input.m_buttonStateTicks[6];
    m_buttonStateTicks[7] = input.m_buttonStateTicks[7];
    m_buttonStateTicks[8] = input.m_buttonStateTicks[8];
    m_buttonStateTicks[9] = input.m_buttonStateTicks[9];
    m_buttonStateTicks[10] = input.m_buttonStateTicks[10];
    m_buttonStateTicks[11] = input.m_buttonStateTicks[11];
    m_buttonStateTicks[12] = input.m_buttonStateTicks[12];
    m_aRemapAngle = input.m_aRemapAngle;
}

void DetInput::ReadFromPad(cGlobalPad* pad)
{
    m_AnalogLeftX = pad->AnalogLeftX();
    m_AnalogLeftY = pad->AnalogLeftY();
    m_AnalogRightX = pad->AnalogRightX();
    m_AnalogRightY = pad->AnalogRightY();

    PadBackend* backend = pad->mBackend;
    if (backend == 0 || !backend->IsConnected())
    {
        m_nConnected = 0;
        m_LeftTrigger = 0;
        m_RightTrigger = 0;
        m_ButtonBitfield = 0;
        m_v3RevRemoteAccel.x = 0.0f;
        m_v3RevRemoteAccel.y = 0.0f;
        m_v3RevRemoteAccel.z = 0.0f;
        m_v3RevFreeStyleAccel.x = 0.0f;
        m_v3RevFreeStyleAccel.y = 0.0f;
        m_v3RevFreeStyleAccel.z = 0.0f;
        m_nRevDPDNumTargets = 0;
        m_v2RevDPDCoord.x = 0.0f;
        m_v2RevDPDCoord.y = 0.0f;
    }
    else if (backend->GetClassID() == gWiiRemotePadClassID)
    {
        m_nConnected = 1;
        WiiRemotePad* remote = static_cast<WiiRemotePad*>(pad->mBackend);
        WPADStatus* status = &remote->mCurrentStatus->wpad;
        m_ButtonBitfield = status->button;
        m_LeftTrigger = 0;
        m_RightTrigger = 0;
        const float scale = 0.0048780488f;
        m_v3RevRemoteAccel.x = scale * status->accX;
        m_v3RevRemoteAccel.y = scale * status->accY;
        m_v3RevRemoteAccel.z = scale * status->accZ;
        m_v3RevFreeStyleAccel.x = 0.0f;
        m_v3RevFreeStyleAccel.y = 0.0f;
        m_v3RevFreeStyleAccel.z = 0.0f;
        m_nRevDPDNumTargets = remote->mCurrentStatus->kpad.dpd_valid_fg;
        m_v2RevDPDCoord.x = remote->mCurrentStatus->kpad.pos.x;
        m_v2RevDPDCoord.y = remote->mCurrentStatus->kpad.pos.y;
    }
    else if (backend->GetClassID() == gWiiFreestylePadClassID)
    {
        m_nConnected = 2;
        WiiFreestylePad* nunchuk = static_cast<WiiFreestylePad*>(pad->mBackend);
        WPADFSStatus* status = &nunchuk->mCurrentStatus->wpad;
        m_ButtonBitfield = status->button;
        m_LeftTrigger = 0;
        m_RightTrigger = 0;
        const float scale = 0.0048780488f;
        m_v3RevRemoteAccel.x = scale * status->accX;
        m_v3RevRemoteAccel.y = scale * status->accY;
        m_v3RevRemoteAccel.z = scale * status->accZ;
        m_v3RevFreeStyleAccel.x = scale * status->fsAccX;
        m_v3RevFreeStyleAccel.y = scale * status->fsAccY;
        m_v3RevFreeStyleAccel.z = scale * status->fsAccZ;
        m_nRevDPDNumTargets = nunchuk->mCurrentStatus->kpad.dpd_valid_fg;
        m_v2RevDPDCoord.x = nunchuk->mCurrentStatus->kpad.pos.x;
        m_v2RevDPDCoord.y = nunchuk->mCurrentStatus->kpad.pos.y;
    }
    else if (backend->GetClassID() == gGameCubePadClassID)
    {
        m_nConnected = 3;
        PadBackend* gameCube = pad->mBackend;
        PADStatus* status = static_cast<GameCubePad*>(gameCube)->mCurrentStatus;
        m_ButtonBitfield = status->button;
        m_LeftTrigger = (u8)(255.0f * gameCube->GetPressure(0x40, false));
        m_RightTrigger = (u8)(255.0f * gameCube->GetPressure(0x20, false));
        m_v3RevRemoteAccel.x = 0.0f;
        m_v3RevRemoteAccel.y = 0.0f;
        m_v3RevRemoteAccel.z = 0.0f;
        m_v3RevFreeStyleAccel.x = 0.0f;
        m_v3RevFreeStyleAccel.y = 0.0f;
        m_v3RevFreeStyleAccel.z = 0.0f;
        m_nRevDPDNumTargets = 0;
        m_v2RevDPDCoord.x = 0.0f;
        m_v2RevDPDCoord.y = 0.0f;
    }
    else if (backend->GetClassID() == PadMonkey::sClassID)
    {
        PadMonkey* monkey = static_cast<PadMonkey*>(pad->mBackend);
        m_nConnected = 3;
        m_ButtonBitfield = 0;
        for (int button = 1; button < (1 << monkey->GetButtonCount()); button <<= 1)
        {
            if (monkey->IsPressed(button, false))
            {
                m_ButtonBitfield |= button;
            }
        }
        m_LeftTrigger = (u8)(255.0f * monkey->GetPressure(0x40, false));
        m_RightTrigger = (u8)(255.0f * monkey->GetPressure(0x20, false));
        m_v3RevRemoteAccel.x = 0.0f;
        m_v3RevRemoteAccel.y = 0.0f;
        m_v3RevRemoteAccel.z = 0.0f;
        m_v3RevFreeStyleAccel.x = 0.0f;
        m_v3RevFreeStyleAccel.y = 0.0f;
        m_v3RevFreeStyleAccel.z = 0.0f;
        m_nRevDPDNumTargets = 0;
        m_v2RevDPDCoord.x = 0.0f;
        m_v2RevDPDCoord.y = 0.0f;
    }

    UpdatePolarAnalog();
    UpdateButtonStateTicks();
}

int DetInput::GetPadID()
{
    return ((NetworkPeerChannel*)m_pMyUser)->GetNetworkPeerChannelId();
}
