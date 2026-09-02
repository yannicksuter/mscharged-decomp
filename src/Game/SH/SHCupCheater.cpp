#include "Game/SH/SHCupCheater.h"

#include "Game/DB/SaveLoad.h"
#include "Game/DB/StatsTracker.h"
#include "Game/DB/tu_8010A40C.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/GameSceneManager.h"
#include "Game/Render/Presentation.h"
#include "NL/nlBasicString.h"
#include "NL/nlBind.h"
#include "NL/nlLexicalCast.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"

extern "C" void fn_8010BCB8(void* object, bool overtime, int winningSide);
extern "C" void fn_8010C5E0(UnidentifiedCupManager* manager);
extern "C" Presentation* fn_801FEEAC();

extern BaseGameSceneManager* lbl_806E1838;

void CupCheaterScene::UpdateSlides()
{
    CupCheaterScene* const self = this;
    struct Locals
    {
        TLTextInstance* text;
        TLComponentInstance* comp;
    };
    const unsigned char currentSlide = self->m_SlideMenu->m_currentSlide;
    Locals locals;
    locals.comp = self->m_SlideMenu->m_pMenuComp;
    TLSlide* pSlide;

    for (int i = 0; i < 9; i++)
    {
        self->m_SlideMenu->SetSlideByIndex((unsigned char)i);
        pSlide = locals.comp->GetActiveSlide();

        TLTextInstance* foundText;
        foundText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(
            pSlide, InlineHasher(nlStringLowerHash("number1")));
        locals.text = foundText;

        BasicString<char, Detail::TempStringAllocator> Sniper(
            LexicalCast<BasicString<char, Detail::TempStringAllocator>, int>(self->mSniper));
        nlStrToWcs(Sniper.c_str(), self->mSniperBuffer, 10);
        locals.text->SetString(self->mSniperBuffer);

        locals.text = FEFinder<TLTextInstance, 3>::Find<TLSlide>(
            pSlide, InlineHasher(nlStringLowerHash("number2")));

        BasicString<char, Detail::TempStringAllocator> Striker(
            LexicalCast<BasicString<char, Detail::TempStringAllocator>, int>(self->mStriker));
        nlStrToWcs(Striker.c_str(), self->mStrikerBuffer, 10);
        locals.text->SetString(self->mStrikerBuffer);

        locals.text = FEFinder<TLTextInstance, 3>::Find<TLSlide>(
            pSlide, InlineHasher(nlStringLowerHash("number5")));
        locals.text->SetStringId(self->mUnidentified50 ? "AWAY" : "HOME");
    }

    self->m_SlideMenu->SetSlideByIndex(currentSlide);
}

void CupCheaterScene::OnSelectGameplay()
{
    fn_801FEEAC()->Call("TransitionCupToChooseSides");
    lbl_806E1838->Pop();
}

void CupCheaterScene::SceneCreated()
{
    typedef Detail::MemFunImpl<void, void (CupCheaterScene::*)()> MemFunImpl_CupCheaterScene_v;
    typedef BindExp1<void, MemFunImpl_CupCheaterScene_v, CupCheaterScene*> BindExp1_vfmfcp;

    void* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLComponentInstance* comp = FEFinder<TLComponentInstance, 4>::Find(
        (FEPresentation*)presentation,
        nlStringLowerHash("Slide1"),
        nlStringLowerHash("Layer"),
        nlStringLowerHash("Menu"),
        0,
        0,
        0);

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
            MemFun<CupCheaterScene, void>(&CupCheaterScene::fn_801E793C), this));
        m_SlideMenu->AddMenuItem("Slide6", callback);
    }
    {
        Function<FnVoidVoid> callback(Bind<void, MemFunImpl_CupCheaterScene_v, CupCheaterScene*>(
            MemFun<CupCheaterScene, void>(&CupCheaterScene::fn_801E793C), this));
        m_SlideMenu->AddMenuItem("Slide7", callback);
    }

    m_SlideMenu->AddMenuItem("Slide8");
    m_SlideMenu->AddMenuItem("Slide9");
    m_SlideMenu->m_doWrapAround = true;
    m_SlideMenu->UpdatePresentation();
    UpdateSlides();
}

void CupCheaterScene::fn_801E793C()
{
    GameInfoManager* gameInfoManager = GameInfoManager::s_pInstance;
    StatsTracker::s_pInstance->SetBasicGameInfoPointer(
        gameInfoManager->mGameInfo[gameInfoManager->mCurrentMode], true);
    fn_8010C5E0(lbl_806E0F90);

    if (mSniper == mStriker)
    {
        if (!mUnidentified50)
        {
            mSniper++;
        }
        else
        {
            mStriker++;
        }
    }

    int winningSide = mStriker >= mSniper;
    if (mSniper > 0)
    {
        StatsTracker::s_pInstance->TrackStat(
            STATS_GOALS_FOR, 0, nlRandom(4, &nlDefaultSeed), -1, 0, mSniper, 0);
    }
    if (mStriker > 0)
    {
        StatsTracker::s_pInstance->TrackStat(
            STATS_GOALS_FOR, 1, nlRandom(4, &nlDefaultSeed), -1, 0, mStriker, 0);
    }
    StatsTracker::s_pInstance->TrackStat(
        STATS_WIN, winningSide, 0, mSniper, mStriker, 0, 0);
    fn_8010BCB8(lbl_806E0F90, false, winningSide);
    fn_801E7A94();
}

static inline void TrackHomeWinResult()
{
    GameInfoManager* gameInfoManager = GameInfoManager::s_pInstance;
    StatsTracker::s_pInstance->SetBasicGameInfoPointer(
        gameInfoManager->mGameInfo[gameInfoManager->mCurrentMode], true);
    fn_8010C5E0(lbl_806E0F90);
    StatsTracker::s_pInstance->TrackStat(
        STATS_GOALS_FOR, 0, nlRandom(4, &nlDefaultSeed), -1, 0, 1, 0);
    StatsTracker::s_pInstance->TrackStat(STATS_WIN, 0, 0, 1, 0, 0, 0);
    fn_8010BCB8(lbl_806E0F90, false, 0);
}

static inline void TrackAwayWinResult()
{
    GameInfoManager* gameInfoManager = GameInfoManager::s_pInstance;
    StatsTracker::s_pInstance->SetBasicGameInfoPointer(
        gameInfoManager->mGameInfo[gameInfoManager->mCurrentMode], true);
    fn_8010C5E0(lbl_806E0F90);
    StatsTracker::s_pInstance->TrackStat(
        STATS_GOALS_FOR, 1, nlRandom(4, &nlDefaultSeed), -1, 0, 1, 0);
    StatsTracker::s_pInstance->TrackStat(STATS_WIN, 1, 0, 0, 1, 0, 0);
    fn_8010BCB8(lbl_806E0F90, false, 1);
}

static inline void TrackHomeOTWinResult()
{
    GameInfoManager* gameInfoManager = GameInfoManager::s_pInstance;
    StatsTracker::s_pInstance->SetBasicGameInfoPointer(
        gameInfoManager->mGameInfo[gameInfoManager->mCurrentMode], true);
    fn_8010C5E0(lbl_806E0F90);
    StatsTracker::s_pInstance->TrackStat(
        STATS_GOALS_FOR, 0, nlRandom(4, &nlDefaultSeed), -1, 0, 1, 0);
    StatsTracker::s_pInstance->TrackStat(STATS_LOSS, 0, 0, 1, 0, 0, 0);
    fn_8010BCB8(lbl_806E0F90, true, 0);
}

static inline void TrackAwayOTWinResult()
{
    GameInfoManager* gameInfoManager = GameInfoManager::s_pInstance;
    StatsTracker::s_pInstance->SetBasicGameInfoPointer(
        gameInfoManager->mGameInfo[gameInfoManager->mCurrentMode], true);
    fn_8010C5E0(lbl_806E0F90);
    StatsTracker::s_pInstance->TrackStat(
        STATS_GOALS_FOR, 1, nlRandom(4, &nlDefaultSeed), -1, 0, 1, 0);
    StatsTracker::s_pInstance->TrackStat(STATS_LOSS, 1, 0, 0, 1, 0, 0);
    fn_8010BCB8(lbl_806E0F90, true, 1);
}

void CupCheaterScene::OnSelectAwayOTWin()
{
    TrackAwayOTWinResult();
    fn_801E7A94();
}

void CupCheaterScene::OnSelectHomeOTWin()
{
    TrackHomeOTWinResult();
    fn_801E7A94();
}

void CupCheaterScene::OnSelectAwayWin()
{
    TrackAwayWinResult();
    fn_801E7A94();
}

void CupCheaterScene::OnSelectHomeWin()
{
    TrackHomeWinResult();
    fn_801E7A94();
}

void CupCheaterScene::Update(float dt)
{
    BaseSceneHandler::Update(dt);

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
            mSniper--;
            mSniper &= ~(mSniper >> 31);
            UpdateSlides();
            break;
        case 6:
            mStriker--;
            mStriker &= ~(mStriker >> 31);
            UpdateSlides();
            break;
        }
    }
    else if (g_pFEInput->JustPressed(FE_ALL_PADS, 0xC, true, 0))
    {
        switch (m_SlideMenu->m_currentSlide)
        {
        case 5:
            mSniper++;
            UpdateSlides();
            break;
        case 6:
            mStriker++;
            UpdateSlides();
            break;
        }
    }
}

CupCheaterScene::~CupCheaterScene()
{
    if (m_SlideMenu != 0)
    {
        delete m_SlideMenu;
    }
}

CupCheaterScene::CupCheaterScene()
    : BaseSceneHandler()
{
    mSniper = 0;
    mStriker = 0;

    GameInfoManager* gameInfoManager = GameInfoManager::s_pInstance;
    int team = gameInfoManager->mGameInfo[gameInfoManager->mCurrentMode]->mTeamIndex[0];
    mUnidentified50 = team != lbl_806E0F90->GetUserSelectedCupTeam();
}
