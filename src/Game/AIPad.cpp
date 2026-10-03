#include "Game/AI/AIPad.h"

#include "Game/NetworkSession.h"
#include "Game/TweakValueFloat.h"
#include "Game/NetworkInput.h"
#include "Game/UnidentifiedStaticStorage.h"

static float g_fMovementDeadZone = 0.3f;
static float g_fCStickDeadZone = 0.5f;
float g_fAccelerationHistoryBlend = 0.5f;

static TweakValueFloat sDPDSensitivity(
    "DPD_Sensitivity", "Controller Config/DPD", 1.8f);
static TweakValueFloat sLeftShakeThreshold(
    "gfLeftShakeThreshold", "Controller Config", 2.5f);
static TweakValueFloat sRightShakeThreshold(
    "gfRightShakeThreshold", "Controller Config", 1.33f);

cAIPad AIPadManager::mAIPads[16];

cAIPad::cAIPad()
{
    mAccelerationHistoryIndex = 0;
    mLocalControllerIndex = -1;
    m_pGlobalPad = 0;

    for (int i = 0; i < 30; ++i)
    {
        mRemoteAccelerationHistory[i].x = 1000.0f;
        mRemoteAccelerationHistory[i].y = 0.0f;
        mRemoteAccelerationHistory[i].z = 0.0f;
        mFreestyleAccelerationHistory[i].x = 1000.0f;
        mFreestyleAccelerationHistory[i].y = 0.0f;
        mFreestyleAccelerationHistory[i].z = 0.0f;
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

static float GetAccelerationHistoryLimit()
{
    return 999.0f;
}

bool cAIPad::DetectLeftShake(u16* direction)
{
    float thresholdSq
        = sLeftShakeThreshold.value * sLeftShakeThreshold.value;
    nlVector3 acceleration;
    if (GetMaxFreestyleAccelDelta(5, &acceleration) > 0)
    {
        if (nlAbs(acceleration.y) < nlAbs(acceleration.z))
            acceleration.y = acceleration.z;
        else
            acceleration.y = -acceleration.y;

        const nlVector3& projected = acceleration;
        if (projected.GetLengthSq2D() > thresholdSq)
        {
            u16 remapAngle = m_pGlobalPad->m_aRemapAngle;
            float angle = nlATan2f(projected.y, -projected.x);
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
        = sRightShakeThreshold.value * sRightShakeThreshold.value;
    nlVector3 acceleration;
    if (GetMaxRemoteAccelDelta(5, &acceleration) > 0)
    {
        if (nlAbs(acceleration.y) < nlAbs(acceleration.z))
            acceleration.y = acceleration.z;
        else
            acceleration.y = -acceleration.y;

        const nlVector3& projected = acceleration;
        if (projected.GetLengthSq2D() > thresholdSq)
        {
            u16 remapAngle = m_pGlobalPad->m_aRemapAngle;
            float angle = nlATan2f(projected.y, -projected.x);
            *direction = (u16)(int)(angle * 10430.378f)
                       + remapAngle;
            return true;
        }
    }
    *direction = 0;
    return false;
}

static const nlVector3 sZeroAccelDelta = { 0.0f, 0.0f, 0.0f };

int cAIPad::GetMaxRemoteAccelDelta(
    unsigned int requestedSamples, nlVector3* deltaOut)
{
    unsigned int currentIndex = (mAccelerationHistoryIndex + 30) % 30;
    const nlVector3& current = mRemoteAccelerationHistory[currentIndex];
    *deltaOut = sZeroAccelDelta;
    float maximum = 0.0f;
    int bestOffset = 0;
    if (current.x < GetAccelerationHistoryLimit())
    {
        unsigned int sampleCount = requestedSamples > 30 ? 30 : requestedSamples;

        nlVector3 bestPrevious;
        for (unsigned int offset = 1; offset < sampleCount; ++offset)
        {
            unsigned int cappedOffset = offset;
            if (cappedOffset >= 30)
                cappedOffset = 29;
            const nlVector3& previous = mRemoteAccelerationHistory[(mAccelerationHistoryIndex + 30 - cappedOffset) % 30];
            if (previous.x > GetAccelerationHistoryLimit())
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
    unsigned int requestedSamples, nlVector3* deltaOut)
{
    unsigned int currentIndex = (mAccelerationHistoryIndex + 30) % 30;
    const nlVector3& current = mFreestyleAccelerationHistory[currentIndex];
    *deltaOut = sZeroAccelDelta;
    float maximum = 0.0f;
    int bestOffset = 0;
    if (current.x < GetAccelerationHistoryLimit())
    {
        unsigned int sampleCount = requestedSamples > 30 ? 30 : requestedSamples;

        nlVector3 bestPrevious;
        for (unsigned int offset = 1; offset < sampleCount; ++offset)
        {
            unsigned int cappedOffset = offset;
            if (cappedOffset >= 30)
                cappedOffset = 29;
            const nlVector3& previous = mFreestyleAccelerationHistory[(mAccelerationHistoryIndex + 30 - cappedOffset) % 30];
            if (previous.x > GetAccelerationHistoryLimit())
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
        mRemoteAccelerationHistory[i].x = 1000.0f;
        mRemoteAccelerationHistory[i].y = 0.0f;
        mRemoteAccelerationHistory[i].z = 0.0f;
        mFreestyleAccelerationHistory[i].x = 1000.0f;
        mFreestyleAccelerationHistory[i].y = 0.0f;
        mFreestyleAccelerationHistory[i].z = 0.0f;
    }
}

void AIPadManager::Startup()
{
    for (int i = 0; i < 16; ++i)
    {
        mAIPads[i].m_pGlobalPad = 0;
    }

    int numMachines = g_pNetworkSessionBase->GetNumMachines();
    for (s8 machineIndex = 0; machineIndex < numMachines; ++machineIndex)
    {
        NetworkPeer* peer = g_pNetworkSessionBase->GetPeer(machineIndex);
        for (s8 controllerIndex = 0;
            controllerIndex < (int)peer->mPlayerCount;
            ++controllerIndex)
        {
            NetworkPeerChannel* channel
                = peer->GetNetworkPeerChannel(controllerIndex);
            s8 padIndex = GetNetworkPlayerId(controllerIndex, machineIndex);
            DetInput* input = channel->GetNetworkPeerChannelInput();
            mAIPads[padIndex].m_pGlobalPad = input;

            if (machineIndex == g_pNetworkSessionBase->GetLocalMachineId())
            {
                mAIPads[padIndex].mLocalControllerIndex = controllerIndex;
            }
        }
    }
}

void StartupAIPads()
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
        unsigned int previousIndex = pad.mAccelerationHistoryIndex;
        pad.mAccelerationHistoryIndex = previousIndex + 1;
        if (pad.mAccelerationHistoryIndex >= 30)
            pad.mAccelerationHistoryIndex = 0;

        nlVector3& previousRemote = pad.mRemoteAccelerationHistory[previousIndex];
        nlVector3& previousFreestyle = pad.mFreestyleAccelerationHistory[previousIndex];
        if (previousRemote.x < GetAccelerationHistoryLimit())
        {
            float alpha = g_fAccelerationHistoryBlend;
            nlVecLerp(pad.mRemoteAccelerationHistory[pad.mAccelerationHistoryIndex],
                previousRemote,
                remote,
                alpha);
            nlVecLerp(pad.mFreestyleAccelerationHistory[pad.mAccelerationHistoryIndex],
                previousFreestyle,
                freestyle,
                alpha);
        }
        else
        {
            pad.mRemoteAccelerationHistory[pad.mAccelerationHistoryIndex] = remote;
            pad.mFreestyleAccelerationHistory[pad.mAccelerationHistoryIndex] = freestyle;
        }
    }
}

void* TweakValueBase::UnidentifiedVirtual1C()
{
    return 0;
}
