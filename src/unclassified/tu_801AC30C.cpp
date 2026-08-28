#include "Game/ObjectBlur.h"

#include "Game/Camera/CameraMan.h"
#include "NL/nlDLRing.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"

extern SlotPool<BlurHandler> lbl_80573BF0;
extern BlurHandler* lbl_806E1610;

bool BlurHandler::ConstructViewOrientedPoints(
    nlVector3& topPoint, nlVector3& bottomPoint, nlVector3 position,
    const nlVector3& forwardVector)
{
    nlVector3 viewVector;
    float perpX;
    float perpY;
    float perpZ;

    float sLen1 = nlVec3LengthSquared(forwardVector);
    if (sLen1 < 0.5f)
    {
        return false;
    }

    float invLen = nlRecipSqrt(sLen1, 1);
    float normX;
    float normZ;
    float normY;
    normZ = invLen * forwardVector.z;
    normY = invLen * forwardVector.y;
    normX = invLen * forwardVector.x;

    cCameraManager::GetViewVector(viewVector);
    if (viewVector.x * normX + viewVector.y * normY + viewVector.z * normZ < 0.99f)
    {
        float crossX = (normY * viewVector.z) - (normZ * viewVector.y);
        float crossY = (-normX * viewVector.z) + (normZ * viewVector.x);
        float crossZ = (normX * viewVector.y) - (normY * viewVector.x);

        float invLen2 = nlRecipSqrt(
            (crossZ * crossZ) + ((crossX * crossX) + (crossY * crossY)), 1);

        float width = m_fLineWidth;
        perpX = width * (invLen2 * crossX);
        perpY = width * (invLen2 * crossY);
        perpZ = width * (invLen2 * crossZ);
    }
    else
    {
        if (m_pLastPoint != 0)
        {
            perpX = 0.5f * (m_pLastPoint->v3Top.x - m_pLastPoint->v3Bottom.x);
            perpY = 0.5f * (m_pLastPoint->v3Top.y - m_pLastPoint->v3Bottom.y);
            perpZ = 0.5f * (m_pLastPoint->v3Top.z - m_pLastPoint->v3Bottom.z);
        }
        else
        {
            return false;
        }
    }

    nlVec3Set(topPoint, position.x + perpX, position.y + perpY, position.z + perpZ);
    nlVec3Set(bottomPoint, position.x - perpX, position.y - perpY, position.z - perpZ);

    return true;
}

void BlurHandler::AddViewOrientedPoint(
    const nlVector3& position, const nlVector3& forwardVector)
{
    nlVector3 delta;
    nlVector3 topPoint, bottomPoint;

    if (ConstructViewOrientedPoints(topPoint, bottomPoint, position, forwardVector))
    {
        m_pointFinal.v3Top = topPoint;
        m_pointFinal.v3Bottom = bottomPoint;

        if (m_pLastPoint != 0)
        {
            nlVec3Set(delta, m_pLastPoint->v3Top.x - m_pointFinal.v3Top.x, m_pLastPoint->v3Top.y - m_pointFinal.v3Top.y, m_pLastPoint->v3Top.z - m_pointFinal.v3Top.z);

            if ((delta.x * delta.x) + (delta.y * delta.y) + (delta.z * delta.z)
                < 0.0025000002f)
            {
                return;
            }
        }

        BlurPointEntry* entry = &m_pointRingBuffer[m_nInsertIndex];
        entry->v3Top = m_pointFinal.v3Top;
        entry->v3Bottom = m_pointFinal.v3Bottom;

        m_pLastPoint = &m_pointRingBuffer[m_nInsertIndex];
        m_nInsertIndex = m_nInsertIndex + 1;
        m_nInsertIndex %= m_maxPositionEntries;

        if (m_nInsertIndex == m_nTrailEndPointer)
        {
            m_nTrailEndPointer = m_nTrailEndPointer + 1;
            m_nTrailEndPointer %= m_maxPositionEntries;
        }
    }
    else
    {
        if (m_nInsertIndex != m_nTrailEndPointer)
        {
            m_nTrailEndPointer = m_nTrailEndPointer + 1;
            m_nTrailEndPointer %= m_maxPositionEntries;
        }
    }
}

void BlurHandler::Die(float timeToDie)
{
    m_bDying = true;
    m_fDyingTimer = timeToDie;
    m_fTimeToDie = timeToDie;
}

BlurHandler* BlurManager::GetNewHandler(
    const char* szTextureName, float fLineWidth, int maxPositionEntries,
    bool bAdditive)
{
    BlurHandler* handler = 0;

    lbl_80573BF0.Allocate(handler);

    if (handler != 0)
    {
        handler->m_bAdditive = bAdditive;
        handler->m_fLineWidth = fLineWidth;
        handler->m_pLastPoint = 0;
        handler->m_bDying = false;
        handler->m_maxPositionEntries = maxPositionEntries;
        handler->m_nInsertIndex = 0;
        handler->m_nTrailEndPointer = 0;
        handler->m_uTexHashID = nlStringLowerHash(szTextureName);
        handler->m_pointRingBuffer = (BlurPointEntry*)nlMalloc(
            maxPositionEntries * sizeof(BlurPointEntry), 8, false);
    }

    nlDLRingAddEnd<BlurHandler>(&lbl_806E1610, handler);

    return handler;
}
