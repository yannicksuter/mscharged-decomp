#include "Game/Goalie.h"

#include "Game/MathHelpers.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnSingleAxisBlender.h"

void Goalie::MoveDirectionCB(
    unsigned int nParam, cPN_SingleAxisBlender* blender)
{
    Goalie* pGoalie = (Goalie*)nParam;
    float result = 0.0f;
    if (pGoalie->mv3LocalNavTarget.y < 0.0f)
    {
        result = 1.0f;
    }
    blender->m_fDesiredWeight = result;
}

void Goalie::MoveWeightCB(
    unsigned int nParam, cPN_SingleAxisBlender* blender)
{
    Goalie* pGoalie = (Goalie*)nParam;
    blender->m_fDesiredWeight
        = (s32)(u16)abs_s16(pGoalie->maLocalAngle) / 32768.0f;
}

void Goalie::StrafeSynchronizedSpeedCallback(
    unsigned int nParam, cPN_SAnimController* controller)
{
    Goalie* pGoalie = (Goalie*)nParam;
    controller->m_fPlaybackSpeedScale = pGoalie->mfSpeedScale;
}

void Goalie::ActionSaveSetup(float deltaTime)
{
    float deflectResult = CheckForDelflectAwayFromNet();

    if (deflectResult < 0.0f)
    {
        return;
    }

    if (deflectResult > 0.0f)
    {
        InitActionSaveSetup(false);
        return;
    }

    if (mnOffplayPending != 0)
    {
        InitActionMove(true);
        return;
    }

    mfWaitTime -= deltaTime;
    if (mfWaitTime < 0.02f)
    {
        InitActionSave();
    }
}

void Goalie::RunWeightCB(
    unsigned int nParam, cPN_SingleAxisBlender* blender)
{
    const Goalie* pGoalie = (Goalie*)nParam;

    s16 diff = (s16)(pGoalie->m_aDesiredFacingDirection
                     - pGoalie->m_aActualFacingDirection);

    s16 minClampedDiff;
    if (diff < -0x31C4)
    {
        minClampedDiff = -0x31C4;
    }
    else
    {
        minClampedDiff = diff;
    }

    s16 clampedDiff;
    if (minClampedDiff > 0x31C4)
    {
        clampedDiff = 0x31C4;
    }
    else
    {
        clampedDiff = minClampedDiff;
    }

    blender->m_fDesiredWeight
        = (float)(clampedDiff + 0x31C4) / 25480.0f;
}
