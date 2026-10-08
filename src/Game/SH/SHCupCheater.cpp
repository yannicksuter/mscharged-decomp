#include "Game/SH/SHCupCheater.h"

#include "Game/DB/SaveLoad.h"
#include "Game/DB/StatsTracker.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/GameSceneManager.h"
#include "Game/Render/FrontEndPresentation.h"
#include "NL/nlBasicString.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlLexicalCast.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"

static inline void TrackHomeWinResult()
{
    GameInfoManager* gameInfoManager = GameInfoManager::s_pInstance;
    StatsTracker::s_pInstance->SetBasicGameInfoPointer(
        gameInfoManager->mGameInfo[gameInfoManager->mCurrentMode], true);
    CupManager::s_pInstance->PrepareCurrentGame();
    StatsTracker::s_pInstance->TrackStat(
        STATS_GOALS_FOR, 0, nlRandom(4, &nlDefaultSeed), -1, 0, 1, 0);
    StatsTracker::s_pInstance->TrackStat(STATS_WIN, 0, 0, 1, 0, 0, 0);
    CupManager::s_pInstance->SetRoundResult(false, 0);
}

static inline void TrackAwayWinResult()
{
    GameInfoManager* gameInfoManager = GameInfoManager::s_pInstance;
    StatsTracker::s_pInstance->SetBasicGameInfoPointer(
        gameInfoManager->mGameInfo[gameInfoManager->mCurrentMode], true);
    CupManager::s_pInstance->PrepareCurrentGame();
    StatsTracker::s_pInstance->TrackStat(
        STATS_GOALS_FOR, 1, nlRandom(4, &nlDefaultSeed), -1, 0, 1, 0);
    StatsTracker::s_pInstance->TrackStat(STATS_WIN, 1, 0, 0, 1, 0, 0);
    CupManager::s_pInstance->SetRoundResult(false, 1);
}

static inline void TrackHomeOTWinResult()
{
    GameInfoManager* gameInfoManager = GameInfoManager::s_pInstance;
    StatsTracker::s_pInstance->SetBasicGameInfoPointer(
        gameInfoManager->mGameInfo[gameInfoManager->mCurrentMode], true);
    CupManager::s_pInstance->PrepareCurrentGame();
    StatsTracker::s_pInstance->TrackStat(
        STATS_GOALS_FOR, 0, nlRandom(4, &nlDefaultSeed), -1, 0, 1, 0);
    StatsTracker::s_pInstance->TrackStat(STATS_OT_WIN, 0, 0, 1, 0, 0, 0);
    CupManager::s_pInstance->SetRoundResult(true, 0);
}

static inline void TrackAwayOTWinResult()
{
    GameInfoManager* gameInfoManager = GameInfoManager::s_pInstance;
    StatsTracker::s_pInstance->SetBasicGameInfoPointer(
        gameInfoManager->mGameInfo[gameInfoManager->mCurrentMode], true);
    CupManager::s_pInstance->PrepareCurrentGame();
    StatsTracker::s_pInstance->TrackStat(
        STATS_GOALS_FOR, 1, nlRandom(4, &nlDefaultSeed), -1, 0, 1, 0);
    StatsTracker::s_pInstance->TrackStat(STATS_OT_WIN, 1, 0, 0, 1, 0, 0);
    CupManager::s_pInstance->SetRoundResult(true, 1);
}

CupCheaterScene::CupCheaterScene()
    : BaseSceneHandler()
{
    mHomeScore = 0;
    mAwayScore = 0;

    GameInfoManager* gameInfoManager = GameInfoManager::s_pInstance;
    int homeTeam = gameInfoManager->mGameInfo[gameInfoManager->mCurrentMode]->mTeamIndex[0];
    mUserSide = homeTeam != CupManager::s_pInstance->GetUserSelectedCupTeam();
}

CupCheaterScene::~CupCheaterScene()
{
    if (m_SlideMenu != 0)
    {
        delete m_SlideMenu;
    }
}

void CupCheaterScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (g_pFEInput->JustPressed(FE_ALL_PADS, 0x1E, true, 0))
    {
        m_SlideMenu->ApplyFunction();
    }
    else if (g_pFEInput->JustPressed(FE_ALL_PADS, 0xD, true, 0))
    {
        m_SlideMenu->PrevItem();
    }
    else if (g_pFEInput->JustPressed(FE_ALL_PADS, 0xE, true, 0))
    {
        m_SlideMenu->NextItem();
    }
    else if (g_pFEInput->JustPressed(FE_ALL_PADS, 0xB, true, 0))
    {
        switch (m_SlideMenu->m_currentSlide)
        {
        case 5:
            mHomeScore--;
            mHomeScore &= ~(mHomeScore >> 31);
            UpdateSlides();
            break;
        case 6:
            mAwayScore--;
            mAwayScore &= ~(mAwayScore >> 31);
            UpdateSlides();
            break;
        }
    }
    else if (g_pFEInput->JustPressed(FE_ALL_PADS, 0xC, true, 0))
    {
        switch (m_SlideMenu->m_currentSlide)
        {
        case 5:
            mHomeScore++;
            UpdateSlides();
            break;
        case 6:
            mAwayScore++;
            UpdateSlides();
            break;
        }
    }
}

void CupCheaterScene::OnSelectHomeWin()
{
    TrackHomeWinResult();
    ProcessPostGame();
}

void CupCheaterScene::OnSelectAwayWin()
{
    TrackAwayWinResult();
    ProcessPostGame();
}

void CupCheaterScene::OnSelectHomeOTWin()
{
    TrackHomeOTWinResult();
    ProcessPostGame();
}

void CupCheaterScene::OnSelectAwayOTWin()
{
    TrackAwayOTWinResult();
    ProcessPostGame();
}

void CupCheaterScene::OnSelectScore()
{
    GameInfoManager* gameInfoManager = GameInfoManager::s_pInstance;
    StatsTracker::s_pInstance->SetBasicGameInfoPointer(
        gameInfoManager->mGameInfo[gameInfoManager->mCurrentMode], true);
    CupManager::s_pInstance->PrepareCurrentGame();

    if (mHomeScore == mAwayScore)
    {
        if (!mUserSide)
        {
            mHomeScore++;
        }
        else
        {
            mAwayScore++;
        }
    }

    int winningSide = mAwayScore >= mHomeScore;
    if (mHomeScore > 0)
    {
        StatsTracker::s_pInstance->TrackStat(
            STATS_GOALS_FOR, 0, nlRandom(4, &nlDefaultSeed), -1, 0, mHomeScore, 0);
    }
    if (mAwayScore > 0)
    {
        StatsTracker::s_pInstance->TrackStat(
            STATS_GOALS_FOR, 1, nlRandom(4, &nlDefaultSeed), -1, 0, mAwayScore, 0);
    }
    StatsTracker::s_pInstance->TrackStat(
        STATS_WIN, winningSide, 0, mHomeScore, mAwayScore, 0, 0);
    CupManager::s_pInstance->SetRoundResult(false, winningSide);
    ProcessPostGame();
}

void CupCheaterScene::ProcessPostGame()
{
    FEMusic::StopStream();
    GameInfoManager* gameInfoManager = GameInfoManager::Instance();
    StatsTracker::Instance()->CompileEndOfGameStats();

    CupManager* cup = CupManager::Instance();
    cup->mPreviousGameTeams[0] = GameInfoManager::Instance()->GetTeam(0);
    cup->mPreviousGameTeams[1] = GameInfoManager::Instance()->GetTeam(1);

    if (gameInfoManager->IsInMode3())
    {
        CupManager::Instance()->AdvanceToNextUserGame();
        GameSceneManager::Instance()->Pop();

        CupManager* cup = CupManager::Instance();
        if (cup->GetCurrentRoundNumber() == -5 && cup->mState == 4)
        {
            if (cup->GetCurrentMode() == 0)
            {
                SetUnlockFlag(1);
                cup->SaveCupRecord();
            }
            else if (cup->GetCurrentMode() == 1)
            {
                SetUnlockFlag(2);
                cup->SaveCupRecord();
            }
            else
            {
                SetUnlockFlag(4);
            }
        }

        CupManager::Instance()->ShowRoundNews();
        CupManager::Instance()->AwardGoalTrophies();
        SaveLoad::StartSave(false);
    }
}

void CupCheaterScene::SceneCreated()
{
    typedef Detail::MemFunImpl<void, void (CupCheaterScene::*)()> MemFunImpl_CupCheaterScene_v;
    typedef BindExp1<void, MemFunImpl_CupCheaterScene_v, CupCheaterScene*> BindExp1_vfmfcp;

    unsigned long menuHash;
    unsigned long layerHash;
    void* presentation;
    presentation = mFEScene->m_pFEPackage->GetPresentation();
    menuHash = nlStringLowerHash("Menu");
    layerHash = nlStringLowerHash("Layer");
    TLComponentInstance* comp = FEFinder<TLComponentInstance, 4>::Find(
        (FEPresentation*)presentation, nlStringLowerHash("Slide1"), layerHash, menuHash, 0, 0, 0);

    m_SlideMenu = new ((FESlideMenu*)nlMalloc(sizeof(FESlideMenu), 8, false)) FESlideMenu(comp);

    {
        Function<FnVoidVoid> callback(Bind<void, MemFunImpl_CupCheaterScene_v, CupCheaterScene*>(
            MemFun<CupCheaterScene, void>(&CupCheaterScene::OnSelectGameplay), this));
        m_SlideMenu->AddMenuItem("Slide1", callback);
    }
    {
        Function<FnVoidVoid> callback(Bind<void, MemFunImpl_CupCheaterScene_v, CupCheaterScene*>(
            MemFun<CupCheaterScene, void>(&CupCheaterScene::OnSelectHomeWin), this));
        m_SlideMenu->AddMenuItem("Slide2", callback);
    }
    {
        Function<FnVoidVoid> callback(Bind<void, MemFunImpl_CupCheaterScene_v, CupCheaterScene*>(
            MemFun<CupCheaterScene, void>(&CupCheaterScene::OnSelectAwayWin), this));
        m_SlideMenu->AddMenuItem("Slide3", callback);
    }
    {
        Function<FnVoidVoid> callback(Bind<void, MemFunImpl_CupCheaterScene_v, CupCheaterScene*>(
            MemFun<CupCheaterScene, void>(&CupCheaterScene::OnSelectHomeOTWin), this));
        m_SlideMenu->AddMenuItem("Slide4", callback);
    }
    {
        Function<FnVoidVoid> callback(Bind<void, MemFunImpl_CupCheaterScene_v, CupCheaterScene*>(
            MemFun<CupCheaterScene, void>(&CupCheaterScene::OnSelectAwayOTWin), this));
        m_SlideMenu->AddMenuItem("Slide5", callback);
    }
    {
        Function<FnVoidVoid> callback(Bind<void, MemFunImpl_CupCheaterScene_v, CupCheaterScene*>(
            MemFun<CupCheaterScene, void>(&CupCheaterScene::OnSelectScore), this));
        m_SlideMenu->AddMenuItem("Slide6", callback);
    }
    {
        Function<FnVoidVoid> callback(Bind<void, MemFunImpl_CupCheaterScene_v, CupCheaterScene*>(
            MemFun<CupCheaterScene, void>(&CupCheaterScene::OnSelectScore), this));
        m_SlideMenu->AddMenuItem("Slide7", callback);
    }

    m_SlideMenu->AddMenuItem("Slide8");
    m_SlideMenu->AddMenuItem("Slide9");
    m_SlideMenu->m_doWrapAround = true;
    m_SlideMenu->UpdatePresentation();
    UpdateSlides();
}

void CupCheaterScene::OnSelectGameplay()
{
    FrontEndPresentation::GetInstance()->Call("TransitionCupToChooseSides");
    GameSceneManager::Instance()->Pop();
}

void CupCheaterScene::UpdateSlides()
{
    TLComponentInstance* comp;
    int currentSlide = m_SlideMenu->m_currentSlide;
    TLTextInstance* pText;
    TLSlide* pSlide;
    comp = m_SlideMenu->m_pMenuComp;

    for (int i = 0; i < 9; i++)
    {
        m_SlideMenu->SetSlideByIndex((unsigned char)i);
        pSlide = comp->GetActiveSlide();

        pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(
            pSlide, "number1");

        BasicString<char, Detail::TempStringAllocator> HomeScore(
            LexicalCast<BasicString<char, Detail::TempStringAllocator>, int>(mHomeScore));
        nlStrToWcs(HomeScore.c_str(), mHomeScoreBuffer, 10);
        pText->SetString(mHomeScoreBuffer);

        pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(
            pSlide, "number2");

        BasicString<char, Detail::TempStringAllocator> AwayScore(
            LexicalCast<BasicString<char, Detail::TempStringAllocator>, int>(mAwayScore));
        nlStrToWcs(AwayScore.c_str(), mAwayScoreBuffer, 10);
        pText->SetString(mAwayScoreBuffer);

        pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(
            pSlide, "number5");
        if (!mUserSide)
        {
            pText->SetStringId("HOME");
        }
        else
        {
            pText->SetStringId("AWAY");
        }
    }

    m_SlideMenu->SetSlideByIndex((unsigned char)currentSlide);
}
