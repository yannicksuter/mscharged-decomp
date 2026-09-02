#include "Game/AI/ShotMeter.h"

#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Ball.h"
#include "Game/CharacterTweaks.h"
#include "Game/GameInfo.h"
#include "NL/nlMath.h"

extern "C" void fn_800154FC(cBall*, float);
extern "C" float fn_800155A0(cBall*, int);
extern "C" float fn_800156A8(cBall*);
extern "C" float fn_8002BE18(PlayerTweaks*);
extern "C" float fn_8002C7A8(PlayerTweaks*);
extern "C" float fn_8002C7E8(PlayerTweaks*);
extern "C" float fn_8002C7F4(PlayerTweaks*);
extern "C" bool fn_80039148(cFielder*);
extern "C" void fn_80039CA0(cFielder*);
extern "C" bool fn_8003C180(cFielder*);
extern "C" PlayerTweaks* fn_8003E6E4(cFielder*);
extern "C" void fn_80060A00(void*, cFielder*);
extern "C" float fn_800A0508(cFielder*, bool, bool);

float lbl_806DBE58 = 1.55f;
float lbl_806DBE5C = 1.55f;
float lbl_806DBE60 = 0.05f;
float lbl_806DBE64 = 0.15f;
float lbl_806DBE68 = 0.8f;
float lbl_806DBE6C = 0.8f;
float lbl_806DBE70 = 0.1f;
float lbl_806DBE74 = 0.3f;

bool lbl_806E0DD0;
float lbl_806E0DD4;
float lbl_806E0DD8;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

void ShotMeter::ShotReleased(cFielder* pFielder)
{
    if (m_eShotMeterState != SHOT_METER_STS_RELEASED)
    {
        if ((lbl_806E0DD0
                || GameInfoManager::Instance()->IsRule0x8Equal3())
            && fn_80039148(pFielder))
        {
            m_eShotMeterState = SHOT_METER_STS_RELEASED;
        }
        else
        {
            m_eShotMeterState = SHOT_METER_RELEASED;
        }
    }

    if (mUnidentified008 < 0.01f)
    {
        mUnidentified008 = 0.01f;
    }
    m_fSpeedValue = InterpolateClamped(0.1f, 1.0f, m_fTime / mUnidentified008);
    if (m_fSpeedValue > 1.0f)
    {
        m_fSpeedValue = 1.0f;
    }
    if (fn_80039148(pFielder))
    {
        m_fSTSValue = fn_800156A8(g_pBall);
    }
    m_fScoreValue = fn_800A0508(pFielder,
        pFielder->mActionShotVars.bIsChipShot,
        false);
    CalcShotAim(pFielder);
}

void ShotMeter::Reset(cFielder* pFielder)
{
    m_eShotMeterState = SHOT_METER_ACTIVE;
    m_fTime = 0.0f;
    m_fScoreValue = 0.0f;
    m_fSpeedValue = 0.0f;
    m_fSTSValue = 0.0f;
    mUnidentified008 = fn_8002C7E8(fn_8003E6E4(pFielder));
    mUnidentified00C = fn_8002C7F4(fn_8003E6E4(pFielder));
}

extern "C" float fn_800A0508(cFielder* pFielder, bool bIsChipShot,
    bool bWasPerfectPass)
{
    float fNetOpeness;
    float fPlayerDistance;
    float fChargedValue;
    float fRatingsValue;

    fRatingsValue = LikelyToScore(pFielder);
    fPlayerDistance = PlayerShotDistance(pFielder);
    PlayerTweaks* pTweaks = fn_8003E6E4(pFielder);
    float fShooting = pTweaks->fShooting;
    fChargedValue = fn_800155A0(g_pBall, 0);
    fChargedValue *= 0.25f;
    float fGoalieOut = GoalieOutOfPosition(pFielder);

    fNetOpeness = fRatingsValue;

    float fScoreValue;
    if (!bIsChipShot)
    {
        float fPlayerWeighting = lbl_806DBE60;
        fShooting *= lbl_806E0DD4;
        float fNetWeighting = lbl_806DBE64;
        fNetOpeness *= fNetWeighting;
        fPlayerDistance *= fPlayerWeighting;
        fChargedValue *= lbl_806DBE68;
        float fScore = fNetOpeness + fPlayerDistance;
        fScoreValue = fShooting + (fChargedValue + fScore);
    }
    else
    {
        float fChipWeight = lbl_806DBE6C;
        float fGoalieVal;
        float fRemainder;
        fGoalieVal = fGoalieOut;
        fGoalieVal *= fChipWeight;
        fShooting *= lbl_806E0DD8;
        float fChipOpenWeight = lbl_806DBE70;
        fNetOpeness *= fChipOpenWeight;
        fRemainder = lbl_806DBE74;
        fChargedValue *= fRemainder;
        fScoreValue = fShooting
                    + (fChargedValue + (fGoalieVal + fNetOpeness));
    }

    if (fScoreValue > 1.0f)
    {
        fScoreValue = 1.0f;
    }
    return fScoreValue;
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
        if (pFielder->m_v3Position.y < 0.0f)
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
    mfSShotAimValue = fAimValue;
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
    nlVec3Sub(v3FielderToNet, v3OffNetLocation, pFielder->m_v3Position);
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
    nlVector3 v3BallToNet;
    nlVec3Sub(v3BallToNet, g_pBall->m_v3Position, v3OffNetLocation2);
    float fDistanceValue = InterpolateRangeClamped(0.0f, 1.0f, 15.0f, 5.0f, nlSqrt(v3BallToNet.GetLengthSq3D(), true));
    float fDot = (v3FielderToNet.x * v3BallDirection.x)
               + (v3FielderToNet.y * v3BallDirection.y)
               + (v3FielderToNet.z * v3BallDirection.z);
    float fDirectionValue
        = InterpolateRangeClamped(0.0f, 1.0f, 1.0f, 0.0f, fDot);
    float fCombinedValue = (fDirectionValue + fDistanceValue) * 0.5f;

    m_fSpeedValue = InterpolateRangeClamped(0.2f,
        fn_8002C7A8(fn_8003E6E4(pFielder)),
        0.0f,
        1.0f,
        fCombinedValue);
    m_fScoreValue = fn_800A0508(pFielder,
        pFielder->mActionShotVars.bIsChipShot,
        bWasPerfectPass);
    CalcShotAim(pFielder);
}

void ShotMeter::Abort()
{
    m_eShotMeterState = SHOT_METER_INACTIVE;
    m_fTime = 0.0f;
    m_fScoreValue = 0.0f;
    m_fSpeedValue = 0.0f;
    m_fSTSValue = 0.0f;
}

void ShotMeter::Update(float fDeltaT)
{
    m_fTime = m_fTime + fDeltaT;

    switch (m_eShotMeterState)
    {
    case SHOT_METER_ACTIVE:
    {
        if (g_pBall->GetOwnerFielder() != 0)
        {
            float fCurrent = m_fTime / mUnidentified008;
            if (fCurrent > 1.0f)
            {
                fCurrent = 1.0f;
            }

            float fPrevious = (m_fTime - fDeltaT) / mUnidentified008;
            if (fPrevious < 0.0f)
            {
                fPrevious = 0.0f;
            }

            float fDelta = fCurrent - fPrevious;
            float fRange = InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE18(fn_8003E6E4(g_pBall->GetOwnerFielder())));
            float fValue = Interpolate(fDelta * lbl_806DBE58,
                fDelta * lbl_806DBE5C,
                fRange);
            fValue += fn_800155A0(g_pBall, 0);
            fn_800154FC(g_pBall, fValue);
        }

        if (m_fTime >= mUnidentified008
            && g_pBall->GetOwnerFielder() != 0)
        {
            if (!fn_8003C180(g_pBall->GetOwnerFielder()))
            {
                fn_80039CA0(g_pBall->GetOwnerFielder());
            }
            m_eShotMeterState = SHOT_METER_STS_ACTIVE;
        }
        break;
    }
    case SHOT_METER_STS_ACTIVE:
        if (m_fTime >= mUnidentified00C)
        {
            m_eShotMeterState = SHOT_METER_RELEASED;
            if (g_pBall->GetOwnerFielder() != 0)
            {
                if (g_pBall->GetOwnerFielder()
                        ->CanDoCaptainShootToScore())
                {
                    m_eShotMeterState = SHOT_METER_STS_TRANSISTION;
                }
                else if (fn_80039148(g_pBall->GetOwnerFielder()))
                {
                    m_eShotMeterState = SHOT_METER_STS_RELEASED;
                    fn_80060A00(lbl_806E0C94,
                        g_pBall->GetOwnerFielder());
                }
            }
        }
        break;
    case SHOT_METER_INACTIVE:
    case SHOT_METER_RELEASED:
    case SHOT_METER_STS_TRANSISTION:
    case SHOT_METER_STS_RELEASED:
    default:
        break;
    }
}
