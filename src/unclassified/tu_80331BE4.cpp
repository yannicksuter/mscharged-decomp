#include "unclassified/tu_80331BE4.h"

#include "Game/PadMonkey.h"
#include "NL/globalpad.h"
#include "NL/platpad.h"
#include "unclassified/tu_80336B2C.h"

extern int* lbl_806E2278;
extern int lbl_806E227C;
extern int* lbl_806E2288;
extern int lbl_806E228C;
extern int* lbl_806E22A8;
extern int lbl_806E22AC;

extern "C" int fn_802C03FC(int button);
extern "C" int fn_802C06C8(int buttonIndex);
extern "C" int fn_80364298(int button);
extern "C" int fn_803643A8(int buttonIndex);

u8 DetInput::fn_80331BE4()
{
    return m_nConnected;
}

u8 DetInput::fn_80331BEC()
{
    return m_nConnected;
}

nlVector3* DetInput::fn_80331BF4()
{
    return &m_v3RevRemoteAccel;
}

nlVector3* DetInput::fn_80331BFC()
{
    return &m_v3RevFreeStyleAccel;
}

extern "C" bool fn_80331C04(DetInput* input, int button, bool remap)
{
    if (remap)
    {
        int* pArray;
        switch (input->m_nConnected)
        {
        case 0:
        case 3:
            pArray = lbl_806E22A8;
            break;
        case 1:
            pArray = lbl_806E2278;
            break;
        case 2:
            pArray = lbl_806E2288;
            break;
        default:
            pArray = lbl_806E22A8;
            break;
        }
        button = pArray[button];
    }
    return (input->m_ButtonBitfield & button) != 0;
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
            pArray = lbl_806E22A8;
            break;
        case 1:
            pArray = lbl_806E2278;
            break;
        case 2:
            pArray = lbl_806E2288;
            break;
        default:
            pArray = lbl_806E22A8;
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
            pArray = lbl_806E22A8;
            break;
        case 1:
            pArray = lbl_806E2278;
            break;
        case 2:
            pArray = lbl_806E2288;
            break;
        default:
            pArray = lbl_806E22A8;
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

void DetInput::fn_80331D80()
{
    nlCartesianToPolar(m_PolarAnalogLeft, m_AnalogLeftX, m_AnalogLeftY);
    nlCartesianToPolar(m_PolarAnalogRight, m_AnalogRightX, m_AnalogRightY);
}

void DetInput::fn_80331DC8()
{
    for (int i = 0; i < 13; ++i)
    {
        int button;
        switch (m_nConnected)
        {
        case 0:
        case 3:
            button = fn_802C06C8(i);
            break;
        case 1:
        case 2:
            button = fn_803643A8(i);
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

int DetInput::fn_80331ECC(int button, bool remap)
{
    if (remap)
    {
        int* pArray;
        switch (m_nConnected)
        {
        case 0:
        case 3:
            pArray = lbl_806E22A8;
            break;
        case 1:
            pArray = lbl_806E2278;
            break;
        case 2:
            pArray = lbl_806E2288;
            break;
        default:
            pArray = lbl_806E22A8;
            break;
        }
        button = pArray[button];
    }

    switch (m_nConnected)
    {
    case 0:
    case 3:
        return m_buttonStateTicks[fn_802C03FC(button)];
    case 1:
    case 2:
        return m_buttonStateTicks[fn_80364298(button)];
    default:
        return 0;
    }
}

void DetInput::fn_80331F9C(int button, bool remap)
{
    if (remap)
    {
        int* pArray;
        switch (m_nConnected)
        {
        case 0:
        case 3:
            pArray = lbl_806E22A8;
            break;
        case 1:
            pArray = lbl_806E2278;
            break;
        case 2:
            pArray = lbl_806E2288;
            break;
        default:
            pArray = lbl_806E22A8;
            break;
        }
        button = pArray[button];
    }

    switch (m_nConnected)
    {
    case 0:
    case 3:
    {
        int buttonIndex = fn_802C03FC(button);
        m_buttonStateTicks[buttonIndex] = 0;
        break;
    }
    case 1:
    case 2:
    {
        int buttonIndex = fn_80364298(button);
        m_buttonStateTicks[buttonIndex] = 0;
        break;
    }
    }
}

DetInput::DetInput()
{
    m_ButtonBitfield = 0;
    m_AnalogLeftX = 0.0f;
    m_AnalogLeftY = 0.0f;
    m_AnalogRightX = 0.0f;
    m_AnalogRightY = 0.0f;
    m_nConnected = 2;
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

void DetInput::fn_80332110(const DetInput& input)
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

struct UnidentifiedPadStatus
{
    u16 buttons;
    s16 remoteX;
    s16 remoteY;
    s16 remoteZ;
    u8 padding08[0x22];
    s16 freestyleX;
    s16 freestyleY;
    s16 freestyleZ;
    u8 padding30[0x1C];
    float coordinateX;
    float coordinateY;
    u8 padding54[0x36];
    u8 remoteTargets;
    u8 padding8B[7];
    u8 freestyleTargets;
};

void DetInput::fn_8033222C(cGlobalPad* pad)
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
    else
    {
        int classID = backend->UnidentifiedClassID();
        UnidentifiedPadStatus* status
            = *(UnidentifiedPadStatus**)((u8*)backend + 0x1C);
        if (classID == lbl_806E227C || classID == lbl_806E228C)
        {
            m_nConnected = classID == lbl_806E227C ? 1 : 2;
            m_ButtonBitfield = status->buttons;
            m_LeftTrigger = 0;
            m_RightTrigger = 0;
            const float scale = 0.0048780488f;
            m_v3RevRemoteAccel.x = scale * status->remoteX;
            m_v3RevRemoteAccel.y = scale * status->remoteY;
            m_v3RevRemoteAccel.z = scale * status->remoteZ;
            if (classID == lbl_806E228C)
            {
                m_v3RevFreeStyleAccel.x = scale * status->freestyleX;
                m_v3RevFreeStyleAccel.y = scale * status->freestyleY;
                m_v3RevFreeStyleAccel.z = scale * status->freestyleZ;
                m_nRevDPDNumTargets = status->freestyleTargets;
                m_v2RevDPDCoord.x = status->coordinateX;
                m_v2RevDPDCoord.y = status->coordinateY;
            }
            else
            {
                m_v3RevFreeStyleAccel.x = 0.0f;
                m_v3RevFreeStyleAccel.y = 0.0f;
                m_v3RevFreeStyleAccel.z = 0.0f;
                m_nRevDPDNumTargets = status->remoteTargets;
                m_v2RevDPDCoord.x = status->coordinateX;
                m_v2RevDPDCoord.y = status->coordinateY;
            }
        }
        else if (classID == lbl_806E22AC)
        {
            m_nConnected = 3;
            m_ButtonBitfield = status->buttons;
            m_LeftTrigger = (u8)(255.0f * backend->GetPressure(0x40, false));
            m_RightTrigger = (u8)(255.0f * backend->GetPressure(0x20, false));
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
        else if (classID == PadMonkey::sClassID)
        {
            PadMonkey* monkey = (PadMonkey*)backend;
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
    }

    fn_80331D80();
    fn_80331DC8();
}

int DetInput::fn_80332748()
{
    return fn_80336D70((UnidentifiedNetworkPeerChannel*)m_pMyUser);
}
