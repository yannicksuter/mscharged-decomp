#include "Game/OverlayHandlerGoal.h"
#include "Game/Render/RLViewLayers.h"

#include "Game/DB/GameProgress.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlSlide.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/Render/RLViewLayers.h"


GoalOverlay::~GoalOverlay()
{
}

void GoalOverlay::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (!mIsInOvertime && g_pGame->m_eGameState == 6)
    {
        mIsInOvertime = true;
    }
}

void GoalOverlay::fn_801F17D0(int homeAway)
{
    if (g_pGame->mbCaptainShotToScoreOn)
    {
        UpdateGoalInfo(homeAway, 0, true, g_pGame->mUnidentified030);
    }
}

void GoalOverlay::fn_801F17F4(MegaStrikeEndData*)
{
}

void GoalOverlay::Restart()
{
    if (mIsCreated)
    {
        const char* slideName = IsWidescreen() ? "widescreen" : "normal";
        mPresentation->SetActiveSlide(slideName, true);
        mPresentation->m_fadeDuration = mPresentation->m_currentSlide->m_start;
    }
}

void GoalOverlay::Reset()
{
    mCaptainGoals[0] = 0;
    mCaptainGoals[1] = 0;
    mSidekickGoals[0] = 0;
    mSidekickGoals[1] = 0;
    mIsInOvertime = false;

    if (GameInfoManager::Instance()->IsInMode4())
    {
        mCaptainGoals[0] = g_pStrikerChallenge->mHomeScore;
        mCaptainGoals[1] = g_pStrikerChallenge->mAwayScore;
    }
}

void GoalOverlay::SceneCreated()
{
    mIsCreated = true;
    const char* slideName = IsWidescreen() ? "widescreen" : "normal";
    mPresentation->SetActiveSlide(slideName, true);
}
