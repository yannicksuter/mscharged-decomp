#include "Game/AI/ShotMeter.h"

#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Ball.h"
#include "Game/CharacterTweaks.h"
#include "Game/GameInfo.h"
#include "Game/Game.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlMath.h"

float gShotMeterChargeGainMin = 1.55f;
float gShotMeterChargeGainMax = 1.55f;
float gShotMeterPlayerDistanceWeight = 0.05f;
float gShotMeterNetOpenWeight = 0.15f;
float gShotMeterChargeWeight = 0.8f;
float gChipShotMeterGoaliePositionWeight = 0.8f;
float gChipShotMeterNetOpenWeight = 0.1f;
float gChipShotMeterChargeWeight = 0.3f;

bool gShotMeterSidekickShootToScoreEnabled;
float gShotMeterRatingsWeight;
float gChipShotMeterRatingsWeight;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

static inline float DistanceToNet(const nlVector3& ballPosition, const nlVector3& netPosition)
{
    nlVector3 difference;
    nlVec3Sub(difference, ballPosition, netPosition);
    return nlVec3Length(difference);
}

void ShotMeter::ResetValues()
{
    m_fTime = 0.0f;
    m_fScoreValue = 0.0f;
    m_fSpeedValue = 0.0f;
    m_fSTSValue = 0.0f;
}

void ShotMeter::Update(float fDeltaT)
{
    m_fTime += fDeltaT;

    switch (m_eShotMeterState)
    {
    case SHOT_METER_ACTIVE:
    {
        if (g_pBall->GetOwnerFielder() != 0)
        {
            float fCurrent = m_fTime / m_fShotDuration;
            if (fCurrent > 1.0f)
            {
                fCurrent = 1.0f;
            }

            float fPrevious = (m_fTime - fDeltaT) / m_fShotDuration;
            if (fPrevious < 0.0f)
            {
                fPrevious = 0.0f;
            }

            float fDelta = fCurrent - fPrevious;
            float fRange = InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, GetOffensiveRating((g_pBall->GetOwnerFielder())->GetTweaks()));
            float fValue = Interpolate(fDelta * gShotMeterChargeGainMin,
                fDelta * gShotMeterChargeGainMax,
                fRange);
            fValue += GetBallChargeValue(g_pBall, 0);
            fn_800154FC(g_pBall, fValue);
        }

        if (m_fTime >= m_fShotDuration
            && g_pBall->GetOwnerFielder() != 0)
        {
            if (!g_pBall->GetOwnerFielder()->ShouldIClearBall())
            {
                g_pBall->GetOwnerFielder()->EmitMegaStrikeWindup();
            }
            m_eShotMeterState = SHOT_METER_STS_ACTIVE;
        }
        break;
    }
    case SHOT_METER_STS_ACTIVE:
        if (m_fTime >= GetTotalDuration())
        {
            m_eShotMeterState = SHOT_METER_RELEASED;
            if (g_pBall->GetOwnerFielder() != 0)
            {
                if (g_pBall->GetOwnerFielder()
                        ->CanDoCaptainShootToScore())
                {
                    m_eShotMeterState = SHOT_METER_STS_TRANSITION;
                }
                else if (g_pBall->GetOwnerFielder()->CanDoSidekickShootToScore())
                {
                    m_eShotMeterState = SHOT_METER_STS_RELEASED;
                    fn_80060A00(g_pGame,
                        g_pBall->GetOwnerFielder());
                }
            }
        }
        break;
    case SHOT_METER_INACTIVE:
    case SHOT_METER_RELEASED:
    case SHOT_METER_STS_TRANSITION:
    case SHOT_METER_STS_RELEASED:
    default:
        break;
    }
}

void ShotMeter::Abort()
{
    m_eShotMeterState = SHOT_METER_INACTIVE;
    ResetValues();
}

void ShotMeter::CalcOneTimerValue(cFielder* pFielder, bool bWasPerfectPass)
{
    m_eShotMeterState = SHOT_METER_INACTIVE;

    nlVector3 v3BallDirection;
    nlVec3Sub(v3BallDirection, g_pBall->m_v3Position, g_pBall->m_v3PrevPosition);
    if (nlSqrt(v3BallDirection.GetLengthSq3D(), true) > 0.0001f)
    {
        float fBallDirectionInvLength
            = nlRecipSqrt(v3BallDirection.GetLengthSq3D(), true);
        nlVec3Scale(v3BallDirection, fBallDirectionInvLength);
    }
    else
    {
        v3BallDirection = v3Zero;
    }

    nlVector3 v3FielderToNet;
    const nlVector3& v3OffNetLocation
        = pFielder->GetAIOffNetLocation(0);
    nlVec3Sub(v3FielderToNet, v3OffNetLocation, pFielder->mUnidentified024.m_v3Position);
    if (nlSqrt(v3FielderToNet.GetLengthSq3D(), true) > 0.0001f)
    {
        float fFielderToNetInvLength
            = nlRecipSqrt(v3FielderToNet.GetLengthSq3D(), true);
        nlVec3Scale(v3FielderToNet, fFielderToNetInvLength);
    }
    else
    {
        v3FielderToNet = v3Zero;
    }

    const nlVector3& v3OffNetLocation2
        = pFielder->GetAIOffNetLocation(0);
    float fDistanceValue = InterpolateRangeClamped(0.0f, 1.0f, 15.0f, 5.0f, DistanceToNet(g_pBall->m_v3Position, v3OffNetLocation2));
    float fDot = (v3FielderToNet.x * v3BallDirection.x)
               + (v3FielderToNet.y * v3BallDirection.y)
               + (v3FielderToNet.z * v3BallDirection.z);
    float fDirectionValue
        = InterpolateRangeClamped(0.0f, 1.0f, 1.0f, 0.0f, fDot);
    float fCombinedValue = (fDirectionValue + fDistanceValue) / 2.0f;

    m_fSpeedValue = InterpolateRangeClamped(0.2f,
        GetOneTimerMaxSpeed(pFielder->GetTweaks()),
        0.0f,
        1.0f,
        fCombinedValue);
    m_fScoreValue = CalcShotScoreValue(pFielder,
        pFielder->bIsModified,
        bWasPerfectPass);
    CalcShotAim(pFielder);
}

void ShotMeter::CalcSpeedValue()
{
    if (m_fShotDuration < 0.01f)
    {
        m_fShotDuration = 0.01f;
    }
    m_fSpeedValue = InterpolateClamped(0.1f, 1.0f, m_fTime / m_fShotDuration);
    if (m_fSpeedValue > 1.0f)
    {
        m_fSpeedValue = 1.0f;
    }
}

void ShotMeter::CalcShotAim(cFielder* pFielder)
{
    float fAimValue = 0.0f;
    cAIPad* pPad = pFielder->m_pController;
    if (pPad != 0)
    {
        if (pPad->GetMovementStickMagnitude() > 0.0001f)
        {
            s16 dir = pPad->GetMovementStickDirection();
            if ((s16)(dir + 0x8000) >= 0)
            {
                fAimValue = -1.0f;
            }
            else
            {
                fAimValue = 1.0f;
            }
        }
    }
    else
    {
        float fRandom = nlRandomf(1.0f);
        if (pFielder->mUnidentified024.m_v3Position.y < 0.0f)
        {
            if (fRandom < 0.5f)
            {
                fAimValue = 1.0f;
            }
            else if (fRandom < 0.8f)
            {
                fAimValue = -1.0f;
            }
        }
        else
        {
            if (fRandom < 0.5f)
            {
                fAimValue = -1.0f;
            }
            else if (fRandom < 0.8f)
            {
                fAimValue = 1.0f;
            }
        }
    }
    m_fShotAimValue = fAimValue;
}

float CalcShotScoreValue(cFielder* pFielder, bool bIsChipShot,
    bool bWasPerfectPass)
{
    float fRatingsWeight;
    float fNetOpenness;
    float fPlayerDistance;
    float fChargedValue;
    float fRatingsValue;

    fRatingsValue = LikelyToScore(pFielder);
    fPlayerDistance = PlayerShotDistance(pFielder);
    PlayerTweaks* pTweaks = pFielder->GetTweaks();
    float fShooting = pTweaks->fShooting;
    fChargedValue = GetBallChargeValue(g_pBall, 0);
    fChargedValue *= 0.25f;
    float fGoalieOut = GoalieOutOfPosition(pFielder);

    fNetOpenness = fRatingsValue;

    float fScoreValue;
    if (!bIsChipShot)
    {
        float fChargeWeight;
        float fPlayerWeighting = gShotMeterPlayerDistanceWeight;
        fRatingsWeight = gShotMeterRatingsWeight;
        fShooting *= fRatingsWeight;
        float fNetWeighting = gShotMeterNetOpenWeight;
        fNetOpenness *= fNetWeighting;
        fPlayerDistance = fPlayerDistance * fPlayerWeighting;
        fChargeWeight = gShotMeterChargeWeight;
        fChargedValue *= fChargeWeight;
        float fScore = fNetOpenness + fPlayerDistance;
        fScoreValue = fShooting + (fChargedValue + fScore);
    }
    else
    {
        float fChipOpenWeight;
        float fChipWeight = gChipShotMeterGoaliePositionWeight;
        float fGoalieVal;
        float fChargeWeight;
        fGoalieVal = fGoalieOut;
        fGoalieVal *= fChipWeight;
        fRatingsWeight = gChipShotMeterRatingsWeight;
        fShooting *= fRatingsWeight;
        fChipOpenWeight = gChipShotMeterNetOpenWeight;
        fNetOpenness *= fChipOpenWeight;
        fChargeWeight = gChipShotMeterChargeWeight;
        fChargedValue *= fChargeWeight;
        fScoreValue = fShooting
                    + (fChargedValue + (fGoalieVal + fNetOpenness));
    }

    if (fScoreValue > 1.0f)
    {
        fScoreValue = 1.0f;
    }
    return fScoreValue;
}

void ShotMeter::Reset(cFielder* pFielder)
{
    m_eShotMeterState = SHOT_METER_ACTIVE;
    ResetValues();
    m_fShotDuration = GetShootingWindupTime(pFielder->GetTweaks());
    m_fTotalDuration = GetShootingWindupTotalTime(pFielder->GetTweaks());
}

void ShotMeter::ShotReleased(cFielder* pFielder)
{
    if (m_eShotMeterState != SHOT_METER_STS_RELEASED)
    {
        if ((gShotMeterSidekickShootToScoreEnabled
                || GameInfoManager::Instance()->IsRule0x8Equal3())
            && pFielder->CanDoSidekickShootToScore())
        {
            m_eShotMeterState = SHOT_METER_STS_RELEASED;
        }
        else
        {
            m_eShotMeterState = SHOT_METER_RELEASED;
        }
    }

    CalcSpeedValue();

    if (pFielder->CanDoSidekickShootToScore())
    {
        m_fSTSValue = fn_800156A8(g_pBall);
    }
    m_fScoreValue = CalcShotScoreValue(pFielder,
        pFielder->bIsModified,
        false);
    CalcShotAim(pFielder);
}
