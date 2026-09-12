#include "Game/AI/AIPad.h"

#include "Game/NetworkSession.h"
#include "Game/TweakValueFloat.h"
#include "NL/nlConfig.h"
#include "NL/nlFormat.h"
#include "Game/NetworkInput.h"
#include "NL/nlPrint.h"

#include <stdlib.h>

static float g_fMovementDeadZone = 0.3f;
static float g_fCStickDeadZone = 0.5f;
float lbl_806DB3C8 = 0.5f;

static TweakValueFloat sTweak_80568410(
    "DPD_Sensitivity", "Controller Config/DPD", 1.8f);
static TweakValueFloat sTweak_80568430(
    "gfLeftShakeThreshold", "Controller Config", 2.5f);
static TweakValueFloat sTweak_80568450(
    "gfRightShakeThreshold", "Controller Config", 1.33f);

cAIPad AIPadManager::mAIPads[16];

cAIPad::cAIPad()
{
    mUnidentified2D4 = 0;
    mUnidentified2D8 = -1;
    m_pGlobalPad = 0;

    for (int i = 0; i < 30; ++i)
    {
        mUnidentified004[i].x = 1000.0f;
        mUnidentified004[i].y = 0.0f;
        mUnidentified004[i].z = 0.0f;
        mUnidentified16C[i].x = 1000.0f;
        mUnidentified16C[i].y = 0.0f;
        mUnidentified16C[i].z = 0.0f;
    }
}

float cAIPad::GetMovementStickMagnitude()
{
    float mag = m_pGlobalPad->m_PolarAnalogLeft.r;
    float dz = g_fMovementDeadZone;
    return (mag - dz) / (1.0f - dz);
}

u16 cAIPad::GetMovementStickDirection()
{
    return m_pGlobalPad->m_aRemapAngle + m_pGlobalPad->m_PolarAnalogLeft.a;
}

float cAIPad::GetCStickMovementStickMagnitude()
{
    float mag = m_pGlobalPad->m_PolarAnalogRight.r;
    float dz = g_fCStickDeadZone;
    return (mag - dz) / (1.0f - dz);
}

u16 cAIPad::GetCStickMovementStickDirection()
{
    return m_pGlobalPad->m_aRemapAngle + m_pGlobalPad->m_PolarAnalogRight.a;
}

void AIPadManager::Startup()
{
    for (int i = 0; i < 16; ++i)
    {
        mAIPads[i].m_pGlobalPad = 0;
    }

    int numGroups = g_pNetworkSessionBase->GetNumMachines();
    for (s8 groupIndex = 0; groupIndex < numGroups; ++groupIndex)
    {
        NetworkPeer* group = g_pNetworkSessionBase->GetPeer(groupIndex);
        for (s8 controllerIndex = 0;
            controllerIndex < (int)group->mPlayerCount;
            ++controllerIndex)
        {
            NetworkPeerChannel* controller
                = group->GetNetworkPeerChannel(controllerIndex);
            s8 padIndex = GetNetworkPlayerId(controllerIndex, groupIndex);
            cAIPad& pad = mAIPads[padIndex];
            pad.m_pGlobalPad = controller->GetNetworkPeerChannelInput();

            if (groupIndex == g_pNetworkSessionBase->GetLocalMachineId())
            {
                pad.mUnidentified2D8 = controllerIndex;
            }
        }
    }
}

cAIPad* GetAIPad(int index)
{
    return &AIPadManager::mAIPads[index];
}

TweakValueFloat::~TweakValueFloat()
{
}

void TweakValueFloat::CopyValueFrom(
    TweakValueBase* other)
{
    switch (other->GetStorageKind())
    {
    case 1:
        value = ((TweakValueFloat*)other)->value;
        break;
    case 2:
        value = *((TweakFloatBinding*)other)->m_pValue;
        break;
    }
}

int TweakValueFloat::GetStorageKind()
{
    return 1;
}

int TweakValueFloat::GetValueType()
{
    return 5;
}

void* TweakValueFloat::GetValueAddress()
{
    return &value;
}

void TweakValueFloat::FormatValue(
    char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%.3f", value);
}

void TweakValueFloat::ParseValue(
    const char* string)
{
    value = (float)atof(string);
}

void TweakValueFloat::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = 0.0f;
    *maximum = 0.0f;
    *increment = 0.0f;
}

void TweakValueFloat::UnidentifiedVirtual18()
{
}
