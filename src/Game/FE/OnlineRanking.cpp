#include "Game/FE/OnlineRanking.h"

#include "Game/FE/feDPD.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feManager.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feScene.inl"
#include "Game/FE/tlInstance.inl"
#include "Game/GameInfo.h"
#include "Game/NetworkStatsManager.h"
#include "Game/OverlayManager.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHPausePostGame.h"
#include "NL/nlBind.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"
#include "NL/nlstring_tmpl.h"
#include "NL/nlFunction.h"

int lbl_806DD830 = 10;

UnidentifiedOnlineRankingScene::UnidentifiedOnlineRankingScene()
    : BaseOverlayHandler(0xFFFFFFFF, POSITION_ALL)
    , mUnidentified188(1.0f,
          Function<FETimer*>(Bind<void>(
              MemFun(&UnidentifiedOnlineRankingScene::fn_801F048C), this, Placeholder<0>())))
{
    mUnidentified1A4 = false;
    mUnidentified1A5 = false;
    mUnidentified1A6 = false;
    mUnidentified1A8 = lbl_806DD830;
    mUnidentified188.SetEnabled(true);
}

UnidentifiedOnlineRankingScene::~UnidentifiedOnlineRankingScene()
{
}

void UnidentifiedOnlineRankingScene::SceneCreated()
{
    FEPresentation* presentation = mFEScene->GetPackage()->GetPresentation();
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    NetworkStatsManager* stats = NetworkStatsManager::Instance();
    mTotalPoints = stats->mCurrentJob;
    mUnidentified1B0 = stats->mUnidentifiedC430;
    mUnidentified1B1 = stats->mUnidentifiedC431;
    mScorePoints = stats->mUnidentifiedC434;
    mGoalPoints = stats->mUnidentifiedC438;
    mUnidentified1BC = stats->mUnidentifiedC43C;
    FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(presentation, "out", "Layer", "screen", "TimerText")->SetVisible(false);
}

void UnidentifiedOnlineRankingScene::fn_801F048C(FETimer* timer)
{
    mUnidentified1A4 = true;
    --mUnidentified1A8;
}

void UnidentifiedOnlineRankingScene::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    if (mUnidentified1A5)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
        {
            fn_801EDC90(true);
            g_pOverlayManager->Pop();
        }
        return;
    }
    mUnidentified188.Update(dt);
    if (!mUnidentified1A6 || mUnidentified1A4)
    {
        fn_801F05D4();
        mUnidentified1A6 = true;
        if (mUnidentified1A4)
            mUnidentified1A4 = false;
    }
    if (mUnidentified1A8 <= 0)
    {
        GetNavigationScene()->GetTimer()->SetVisible(false);
        SetPointerEnabled(false);
        if (GameInfoManager::Instance()->mCurrentMode == 0)
        {
            mPresentation->SetActiveSlide("out", true);
            mUnidentified1A5 = true;
        }
        else
        {
            FEMusic::StopStream();
            FrontEnd::ReturnToFE();
        }
    }
}

void UnidentifiedOnlineRankingScene::fn_801F05D4()
{
    FEPresentation* presentation = mFEScene->GetPackage()->GetPresentation();
    TLInstance* screen = FEFinder<TLInstance, TLAT_UNKNOWN>::Find(presentation->GetActiveSlide(), "Layer", "screen");
    FEFindTextInstance(screen, "TimerText")->SetVisible(false);
    TLTextInstance* timer = static_cast<TLTextInstance*>(GetNavigationScene()->GetTimer());
    GetNavigationScene()->fn_801CA9E0(true);
    char countdown[12];
    nlSNPrintf(countdown, 12, "%d", mUnidentified1A8);
    nlStrToWcs(countdown, mTimerText, 12);
    timer->SetString(mTimerText);

    BasicGameInfo* game = GameInfoManager::Instance()->GetCurrentGameInfo();
    int winner = game->GetWinningSide();
    TLComponentInstance* result = static_cast<TLComponentInstance*>(FEFinder<TLInstance, TLAT_COMPONENT>::Find(screen, "RESULT"));
    int localSide = GetLocalPlayingSide_801323F4();
    if (winner == localSide)
        result->SetActiveSlide("VICTORY", true, false);
    else
        result->SetActiveSlide("DEFEAT", true, false);

    TLTextInstance* text = FEFinder<TLTextInstance, TLAT_TEXT>::Find(screen, "score");
    WideBasicString unformatted(g_pLocalization->GetString("ONLINE_RANKING_SCORE"));
    int score0 = game->GetFinalScore(0);
    int score1 = game->GetFinalScore(1);
    WideBasicString formatted = Format(unformatted, score0, score1);
    nlStrNCpy(mScoreText, formatted.c_str(), 16);
    text->SetString(mScoreText);

    TLTextInstance* scorePoints = FEFinder<TLTextInstance, TLAT_TEXT>::Find(screen, "score_points");
    if (mScorePoints == 1)
        formatted = WideBasicString(g_pLocalization->GetString("ONLINE_RANKING_POINT"));
    else
    {
        unformatted = WideBasicString(g_pLocalization->GetString("ONLINE_RANKING_SCORE_POINTS"));
        formatted = Format(unformatted, mScorePoints);
    }
    nlStrNCpy(mScorePointsText, formatted.c_str(), 32);
    scorePoints->SetString(mScorePointsText);

    TLTextInstance* goalsText = FEFinder<TLTextInstance, TLAT_TEXT>::Find(screen, "goals");
    unformatted = WideBasicString(g_pLocalization->GetString("ONLINE_RANKING_GOALS"));
    int goals = game->GetFinalScore(localSide);
    formatted = Format(unformatted, goals);
    nlStrNCpy(mGoalsText, formatted.c_str(), 16);
    goalsText->SetString(mGoalsText);

    TLTextInstance* goalPoints = FEFinder<TLTextInstance, TLAT_TEXT>::Find(screen, "goal_points");
    if (mGoalPoints == 1)
        formatted = WideBasicString(g_pLocalization->GetString("ONLINE_RANKING_POINT"));
    else
    {
        unformatted = WideBasicString(g_pLocalization->GetString("ONLINE_RANKING_GOAL_POINTS"));
        formatted = Format(unformatted, mGoalPoints);
    }
    nlStrNCpy(mGoalPointsText, formatted.c_str(), 32);
    goalPoints->SetString(mGoalPointsText);

    TLTextInstance* totalPoints = FEFinder<TLTextInstance, TLAT_TEXT>::Find(screen, "total_points");
    if (mTotalPoints == 1)
        formatted = WideBasicString(g_pLocalization->GetString("ONLINE_RANKING_TOTAL_POINT"));
    else
    {
        unformatted = WideBasicString(g_pLocalization->GetString("ONLINE_RANKING_TOTAL_POINTS"));
        formatted = Format(unformatted, mTotalPoints);
    }
    nlStrNCpy(mTotalPointsText, formatted.c_str(), 32);
    totalPoints->SetString(mTotalPointsText);
}
