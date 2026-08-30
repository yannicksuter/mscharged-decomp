#include <stddef.h>

#include "Game/AI/SpaceSearch.h"

#include "types.h"

#include "Game/AI/AiUtil.h"
#include "Game/Ball.h"
#include "Game/Debug/ShapeRender.h"
#include "Game/Field.h"

static inline int min(int a, int b)
{
    return a < b ? a : b;
}

/**
 * Offset/Address/Size: 0x14 | 0x800A3FDC | size: 0x40
 */
SpaceSearch::~SpaceSearch()
{
}

/**
 * Offset/Address/Size: 0x54 | 0x800A401C | size: 0x5F4
 */
float SpaceSearch::FindBestPosition(
    nlVector3& v3Dest,
    const nlVector3& v3CenterPos,
    eFieldDirection eSearchDir,
    const nlVector3* pv3TargetOrDirection,
    float fMaxRadius,
    unsigned short aSearchCone)
{
    int numAngleSteps;
    int numRadiusSteps;
    unsigned short aDirection;
    nlVector3 v3TowardTarget;
    nlVector3 v3AwayFromTarget;
    float fCustomX;
    float fCustomY;
    nlVector3 v3BestOpenPosition;
    float fOriginalPositionScore;
    float fBestPositionScore;
    int aFromAngle;
    int aToAngle;
    long aDelta;
    float fRadiusDelta;
    nlVector3 v3LastPos;
    int numIterations;
    int i_radius;
    unsigned short aAngleDelta;
    int i_angle;
    nlVector3 v3TestPosition;
    float fScore;
    nlColour colour;

    aDirection = 0;

    switch (eSearchDir)
    {
    case DIR_NONE:
        aSearchCone = 0xFFFF;
        break;
    case DIR_UPFIELD:
        aDirection = (m_fNetDirection > 0.0f) ? 0x8000 : 0;
        break;
    case DIR_DOWNFIELD:
        aDirection = (m_fNetDirection > 0.0f) ? 0 : 0x8000;
        break;
    case DIR_TOWARD_TARGET:
    {
        nlVec3Sub(v3TowardTarget, *pv3TargetOrDirection, v3CenterPos);
        aDirection = (unsigned short)(s32)(10430.378f
                                           * nlATan2f(v3TowardTarget.y, v3TowardTarget.x));
        break;
    }
    case DIR_AWAYFROM_TARGET:
    {
        nlVec3Sub(v3AwayFromTarget, v3CenterPos, *pv3TargetOrDirection);
        aDirection = (unsigned short)(s32)(10430.378f
                                           * nlATan2f(v3AwayFromTarget.y, v3AwayFromTarget.x));
        break;
    }
    case DIR_CUSTOM:
    {
        fCustomX = pv3TargetOrDirection->x;
        fCustomY = pv3TargetOrDirection->y;
        aDirection
            = (unsigned short)(s32)(10430.378f * nlATan2f(fCustomY, fCustomX));
        break;
    }
    }

    if (fMaxRadius <= 0.0f)
    {
        fMaxRadius = 4.0f;
    }

    m_fMaxRadius = fMaxRadius;

    v3BestOpenPosition = v3CenterPos;
    fOriginalPositionScore
        = EvaluatePosition(v3BestOpenPosition, v3CenterPos, eSearchDir, aDirection);
    fBestPositionScore = fOriginalPositionScore;

    nlPolar pLocation = { 0, 0.0f };
    float fMinRadius = 0.0f;

    aFromAngle = (int)((float)aDirection - 0.5f * (float)aSearchCone);
    aToAngle = (int)((float)aDirection + 0.5f * (float)aSearchCone);
    aDelta = (short)(aToAngle - aFromAngle);

    numRadiusSteps = min(5, (int)(0.5f + (fMaxRadius - fMinRadius) / 1.5f));

    fRadiusDelta = (fMaxRadius - fMinRadius) / (float)numRadiusSteps;

    v3LastPos = v3CenterPos;
    numIterations = 0;

    static int maxIterations = 0;

    for (i_radius = 0; i_radius < numRadiusSteps; i_radius++)
    {
        pLocation.a = aFromAngle;
        pLocation.r += fRadiusDelta;
        if (i_radius == numRadiusSteps - 1)
        {
            pLocation.r = fMaxRadius;
        }

        aAngleDelta = (unsigned short)(int)(65536.0f
                                            * (1.5f / (6.2831855f * pLocation.r)));
        numAngleSteps = min(10,
            (int)(0.5f
                  + (float)(unsigned short)aDelta / (float)aAngleDelta));

        aAngleDelta = (unsigned short)((unsigned short)aDelta / numAngleSteps);

        for (i_angle = 0; i_angle < numAngleSteps; i_angle++)
        {
            numIterations++;
            pLocation.a += aAngleDelta;
            if (i_angle == numAngleSteps - 1)
            {
                pLocation.a = aToAngle;
            }

            v3TestPosition.z = 0.0f;
            nlPolarToCartesian(v3TestPosition, pLocation);
            nlVec3Add(v3TestPosition, v3TestPosition, v3CenterPos);
            v3TestPosition.z = 0.0f;

            cField::FixOutOfBoundsPosition(v3TestPosition, 0.2f, true);

            fScore = EvaluatePosition(
                v3TestPosition, v3CenterPos, eSearchDir, aDirection);

            if (m_bDrawSearchSpace)
            {
                colour.c[0] = 0x99;
                colour.c[1] = 0;
                colour.c[2] = 0x99;
                colour.c[3] = 0xFF;
                g_ShapeRenderer.DrawLine3D(
                    v3LastPos, v3TestPosition, colour, false);
                v3LastPos = v3TestPosition;
            }

            if (fScore > fBestPositionScore)
            {
                fBestPositionScore = fScore;
                v3BestOpenPosition = v3TestPosition;
                if (fScore > 1.0f && !m_bDrawSearchSpace)
                {
                    break;
                }
            }
        }

        if (fBestPositionScore > 1.0f && !m_bDebugOn)
        {
            break;
        }
    }

    if (numIterations > maxIterations)
    {
        maxIterations = numIterations;
    }

    v3Dest = v3BestOpenPosition;
    return fBestPositionScore;
}

/**
 * Offset/Address/Size: 0x1648 | 0x800A4610 | size: 0x100
 */
SSearchOpenLane::SSearchOpenLane(cPlayer* pPlayer1, cPlayer* pPlayer2)
    : SpaceSearch(pPlayer1 != NULL ? pPlayer1->m_pTeam->m_pNet->m_fDirection
                                   : pPlayer2->m_pTeam->m_pNet->m_fDirection)
{
    if (pPlayer2 != NULL)
    {
        m_pBallOwner = pPlayer1;
        m_pPassTarget = pPlayer2;
        m_bOtherPosIsTarget = false;

        if (pPlayer1 != NULL)
        {
            m_v3OtherPos = pPlayer1->m_v3Position;
        }
        else
        {
            m_v3OtherPos = g_pBall->m_v3Position;
        }
    }
    else
    {
        m_pBallOwner = pPlayer1;
        m_pPassTarget = NULL;
        m_v3OtherPos = pPlayer1->GetAIOffNetLocation(NULL);
        m_bOtherPosIsTarget = true;
    }
}

/**
 * Offset/Address/Size: 0x16D8 | 0x800A56A0 | size: 0x12C
 */
SSearchRunToNet::SSearchRunToNet(cPlayer* pPlayer)
    : SpaceSearch(pPlayer->m_pTeam->m_pNet->m_fDirection)
    , m_SSearchIdealShot(pPlayer)
{
}

/**
 * Offset/Address/Size: 0x1A54 | 0x800A5A1C | size: 0x38
 */
SSearchCutAndBreak::SSearchCutAndBreak(cPlayer* pPlayer)
    : SpaceSearch(pPlayer->m_pTeam->m_pNet->m_fDirection)
{
    m_pPlayer = pPlayer;
}
