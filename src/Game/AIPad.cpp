#include "Game/AI/AIPad.h"

#include "Game/NetworkSession.h"
#include "Game/TweakValueFloat.h"
#include "NL/nlConfig.h"
#include "NL/nlFormat.h"
#include "Game/NetworkInput.h"
#include "Game/UnidentifiedStaticStorage.h"
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

bool cAIPad::IsWiiController() const
{
    if (m_pGlobalPad != 0)
    {
        if (m_pGlobalPad->GetControllerType() == 1
            || m_pGlobalPad->GetControllerType() == 2)
        {
            return true;
        }
    }
    return false;
}

bool cAIPad::DetectLeftShake(u16* direction)
{
    float thresholdSq
        = sTweak_80568430.value * sTweak_80568430.value;
    nlVector3 acceleration;
    if (GetMaxFreestyleAccelDelta(5, &acceleration) > 0)
    {
        float z = acceleration.z;
        float y = acceleration.y;
        if (nlAbs(y) < nlAbs(z))
            acceleration.y = z;
        else
            acceleration.y = -y;

        if (nlGetLengthSquared2D(acceleration.x,
                acceleration.y) > thresholdSq)
        {
            u16 remapAngle = m_pGlobalPad->m_aRemapAngle;
            float angle = nlATan2f(acceleration.y, -acceleration.x);
            *direction = (u16)(int)(angle * 10430.378f)
                + remapAngle;
            return true;
        }
    }
    *direction = 0;
    return false;
}

bool cAIPad::DetectRightShake(u16* direction)
{
    float thresholdSq
        = sTweak_80568450.value * sTweak_80568450.value;
    nlVector3 acceleration;
    if (GetMaxRemoteAccelDelta(5, &acceleration) > 0)
    {
        float z = acceleration.z;
        float y = acceleration.y;
        if (nlAbs(y) < nlAbs(z))
            acceleration.y = z;
        else
            acceleration.y = -y;

        if (nlGetLengthSquared2D(acceleration.x,
                acceleration.y) > thresholdSq)
        {
            u16 remapAngle = m_pGlobalPad->m_aRemapAngle;
            float angle = nlATan2f(acceleration.y, -acceleration.x);
            *direction = (u16)(int)(angle * 10430.378f)
                + remapAngle;
            return true;
        }
    }
    *direction = 0;
    return false;
}

static const nlVector3 sZeroAccelDelta = {0.0f, 0.0f, 0.0f};

int cAIPad::GetMaxRemoteAccelDelta(
    unsigned int count, nlVector3* deltaOut)
{
    unsigned int currentIndex = (mUnidentified2D4 + 30) % 30;
    const nlVector3& current = mUnidentified004[currentIndex];
    *deltaOut = sZeroAccelDelta;
    float maximum = 0.0f;
    int bestOffset = 0;
    if (current.x < 999.0f)
    {
        unsigned int sampleCount = count > 30 ? 30 : count;

        nlVector3 bestPrevious;
        for (unsigned int offset = 1; offset < sampleCount; ++offset)
        {
            unsigned int cappedOffset = offset;
            if (cappedOffset >= 30)
                cappedOffset = 29;
            const nlVector3& previous = mUnidentified004[
                (mUnidentified2D4 + 30 - cappedOffset) % 30];
            if (previous.x > 999.0f)
                break;

            nlVector3 candidate;
            nlVec3Sub(candidate, current, previous);
            float magnitudeSq = nlVec3LengthSquared(candidate);
            if (magnitudeSq > maximum)
            {
                maximum = magnitudeSq;
                bestOffset = offset;
                bestPrevious = previous;
            }
        }

        if (bestOffset != 0)
            nlVec3Sub(*deltaOut, current, bestPrevious);
    }
    return bestOffset;
}

int cAIPad::GetMaxFreestyleAccelDelta(
    unsigned int count, nlVector3* deltaOut)
{
    unsigned int currentIndex = (mUnidentified2D4 + 30) % 30;
    const nlVector3& current = mUnidentified16C[currentIndex];
    *deltaOut = sZeroAccelDelta;
    float maximum = 0.0f;
    int bestOffset = 0;
    if (current.x < 999.0f)
    {
        unsigned int sampleCount = count > 30 ? 30 : count;

        nlVector3 bestPrevious;
        for (unsigned int offset = 1; offset < sampleCount; ++offset)
        {
            unsigned int cappedOffset = offset;
            if (cappedOffset >= 30)
                cappedOffset = 29;
            const nlVector3& previous = mUnidentified16C[
                (mUnidentified2D4 + 30 - cappedOffset) % 30];
            if (previous.x > 999.0f)
                break;

            nlVector3 candidate;
            nlVec3Sub(candidate, current, previous);
            float magnitudeSq = nlVec3LengthSquared(candidate);
            if (magnitudeSq > maximum)
            {
                maximum = magnitudeSq;
                bestOffset = offset;
                bestPrevious = previous;
            }
        }

        if (bestOffset != 0)
            nlVec3Sub(*deltaOut, current, bestPrevious);
    }
    return bestOffset;
}

void cAIPad::ResetAccelerationHistory()
{
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
            DetInput* input = controller->GetNetworkPeerChannelInput();
            mAIPads[padIndex].m_pGlobalPad = input;

            if (groupIndex == g_pNetworkSessionBase->GetLocalMachineId())
            {
                mAIPads[padIndex].mUnidentified2D8 = controllerIndex;
            }
        }
    }
}

extern "C" void StartupAIPads()
{
    AIPadManager::Startup();
}

cAIPad* GetAIPad(int index)
{
    return &AIPadManager::mAIPads[index];
}

void AIPadManager::UpdateAccelerationHistory()
{
    for (int i = 0; i < 16; ++i)
    {
        cAIPad& pad = mAIPads[i];
        if (pad.m_pGlobalPad == 0)
            continue;

        nlVector3 freestyle;
        nlVector3 remote;
        remote = *pad.m_pGlobalPad->GetRemoteAcceleration();
        freestyle = *pad.m_pGlobalPad->GetFreestyleAcceleration();
        unsigned int previousIndex = pad.mUnidentified2D4;
        pad.mUnidentified2D4 = previousIndex + 1;
        if (pad.mUnidentified2D4 >= 30)
            pad.mUnidentified2D4 = 0;

        nlVector3& previousRemote = pad.mUnidentified004[previousIndex];
        nlVector3& previousFreestyle = pad.mUnidentified16C[previousIndex];
        if (previousRemote.x < 999.0f)
        {
            float alpha = lbl_806DB3C8;
            nlVecLerp(pad.mUnidentified004[pad.mUnidentified2D4],
                previousRemote, remote, alpha);
            nlVecLerp(pad.mUnidentified16C[pad.mUnidentified2D4],
                previousFreestyle, freestyle, alpha);
        }
        else
        {
            pad.mUnidentified004[pad.mUnidentified2D4] = remote;
            pad.mUnidentified16C[pad.mUnidentified2D4] = freestyle;
        }
    }
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
