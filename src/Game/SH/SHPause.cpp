#include "Game/HBMManager.h"

#include "Game/SH/SHPause.h"
#include "NL/nlPrint.h"
#include "Game/FE/feHelpFuncs_decl.h"

#include "Game/GameSceneManager.h"
#include "Game/DB/StatsTracker.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feManager.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feScene.h"
#include "Game/FE/feSceneManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/SH/SHPausePostGame.h"
#include "Game/Task/GameRenderTask.h"
#include "Game/main.h"
#include "NL/glx/glxSwap.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "Game/FE/feDPD.h"
#include "Game/FE/UnidentifiedTLDefault.h"


static const char* MENU_NAMES[7] = {
    "btn_0", "btn_1", "btn_2", "btn_3", "btn_4", "btn_5", "btn_6"
};

eFEINPUT_PAD PauseMenuScene::mControllingInput = FE_ALL_PADS;
float PauseMenuScene::mDelayBeforeUnpause = 0.1f;
s32 PauseMenuScene::mLastSelectedIndex;

/**
 * Offset/Address/Size: 0x0 | 0x80239454 | size: 0x114
 */
PauseMenuScene::PauseMenuScene()
    : mGameIsOver(false)
    , mQuitDelay(0.0f)
    , mQuittingController(FE_ALL_PADS)
    , mUnidentified530(false)
    , mTransitionTo(TT_IN)
    , mIsInTransition(false)
    , mStartAnimAtEnd(false)
    , mUnidentified54A(false)
    , mUnidentified54B(false)
{
    mDelayBeforeUnpause = 0.1f;
    for (int i = 0; i < 7; ++i)
    {
        mUnidentified044[i].mContext = (void*)i;
        // Retail also clears three words beyond the four controller counts.
        mUnidentified534[i] = 0;
    }
}

/**
 * Offset/Address/Size: 0x114 | 0x80239568 | size: 0x88
 */
PauseMenuScene::~PauseMenuScene()
{
    g_bRenderWorld = true;
}

void PauseMenuScene::OnSelectRESUME(TLComponentInstance* instance)
{
    TransitionOut(TT_OUT);
    g_pFEInput->Reset();
    mUnidentified54A = true;
    mLastSelectedIndex = 0;
    FEAudio::PlayAnimAudioEvent(0xDF52130F, 0, 0, 1);
}

/**
 * Offset/Address/Size: 0x19C | 0x802395F0 | size: 0x6E0
 */
void PauseMenuScene::OnSelectQUIT()
{
    mUnidentified54A = true;
    if (FrontEnd::m_bGameOver)
    {
        g_pOverlayManager->Pop();
        g_pOverlayManager->Pop();
    }
    else
    {
        FEPopupMenu* popup = (FEPopupMenu*)g_pOverlayManager->Push((SceneList)10, SCREEN_NOTHING, false);
        popup->mUnidentifiedC0C = mQuittingController;
        popup->mUnidentified9A1 = true;
        WorldDarkening::Instance().Fade(100.0f, 1.0f);
        if (GameInfoManager::Instance()->mIsInStrikers101Mode)
        {
            popup->Create((ePopupMenu)11,
                Bind<void>(MemFun(&PauseMenuScene::OnSelectPopupYESFORFEIT), this),
                Bind<void>(MemFun(&PauseMenuScene::OnSelectPopupNOFORFEIT), this));
        }
        else if (GameInfoManager::Instance()->mCurrentMode == 0
            || GameInfoManager::Instance()->IsInMode4() || g_pGame->m_eGameState == 3)
        {
            popup->Create((ePopupMenu)10,
                Bind<void>(MemFun(&PauseMenuScene::OnSelectPopupYESFORFEIT), this),
                Bind<void>(MemFun(&PauseMenuScene::OnSelectPopupNOFORFEIT), this));
        }
        else if (GameInfoManager::Instance()->IsInMode3()
            || (GameInfoManager::Instance()->IsInMode1()
                && GameInfoManager::Instance()->GetPlayingSide((unsigned short)mQuittingController) != -1))
        {
            popup->Create((ePopupMenu)9,
                Bind<void>(MemFun(&PauseMenuScene::OnSelectPopupYESFORFEIT), this),
                Bind<void>(MemFun(&PauseMenuScene::OnSelectPopupNOFORFEIT), this));
        }
        else
        {
            popup->Create((ePopupMenu)22,
                Bind<void>(MemFun(&PauseMenuScene::OnSelectPopupNOFORFEIT), this));
        }
    }
}

/**
 * Offset/Address/Size: 0x87C | 0x80239CD0 | size: 0x40
 */
void PauseMenuScene::OnSelectPopupNOFORFEIT()
{
    WorldDarkening::Instance().Fade(100.0f, 0.0f);
    mUnidentified54A = false;
}

/**
 * Offset/Address/Size: 0x8BC | 0x80239D10 | size: 0x204
 */
void PauseMenuScene::OnSelectPopupYESFORFEIT()
{
    FEFinder<TLInstance, 2>::Find<>(mPresentation->m_currentSlide, InlineHasher("Layer"))->m_bVisible = false;
    mUnidentified54A = true;
    GameInfoManager* gameInfoManager = GameInfoManager::Instance();
    CupManager* cupManager = g_pCupManager;
    if (gameInfoManager->mIsInStrikers101Mode)
    {
        gpHBMManager->mBlocked = true;
        mQuitDelay = 1.0f;
        return;
    }
    if (g_pGame->m_eGameState != 3)
    {
        s32 quittingSide = -1;
        if (gameInfoManager->IsInMode3())
        {
            int userTeam = cupManager->GetUserSelectedCupTeam();
            if (userTeam == gameInfoManager->GetTeam(0))
                quittingSide = 0;
            else if (userTeam == gameInfoManager->GetTeam(1))
                quittingSide = 1;
        }
        else if (gameInfoManager->IsInMode1())
        {
            quittingSide = gameInfoManager->GetPlayingSide((unsigned short)mQuittingController);
        }
        if (gameInfoManager->IsInOddCupMode())
        {
            if (quittingSide == 0)
                StatsTracker::Instance()->TrackWinner(0);
            else if (quittingSide == 1)
                StatsTracker::Instance()->TrackWinner(1);
        }
    }
    gpHBMManager->mBlocked = true;
    mQuitDelay = 1.0f;
}

/**
 * Offset/Address/Size: 0xAC0 | 0x80239F14 | size: 0x394
 */
void PauseMenuScene::SceneCreated()
{
    FEAudio::EnableSounds(false);
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    for (int i = 0; i < 7; ++i)
    {
        mUnidentified028[i] = FEFinder<TLComponentInstance, 4>::FindOrDefault(
            presentation->m_currentSlide, "Layer", MENU_NAMES[i]);
    }
    FEAudio::EnableSounds(true);
    if (GameInfoManager::Instance()->IsInMode4())
    {
        if (GetRegion() == 1)
        {
            FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified028[4], "off", "option")->SetStringId("CHALLENGES_OBJECTIVES_BUTTON");
            FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified028[4], "over", "option")->SetStringId("CHALLENGES_OBJECTIVES_BUTTON");
            FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified028[4], "down", "option")->SetStringId("CHALLENGES_OBJECTIVES_BUTTON");
        }
        else
        {
            const char* string = g_pStrikerChallenge->mCurrentChallenge < 10
                ? "101_OBJECTIVES_BUTTON" : "CHALLENGES_OBJECTIVES_BUTTON";
            FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified028[4], "off", "option")->SetStringId(string);
            FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified028[4], "over", "option")->SetStringId(string);
            FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified028[4], "down", "option")->SetStringId(string);
        }
    }
}

/**
 * Offset/Address/Size: 0xE54 | 0x8023A2A8 | size: 0x5B4
 */
void PauseMenuScene::Update(float fDeltaT)
{
    if (mQuitDelay > 0.0f)
    {
        mQuitDelay -= fDeltaT;
        if (!g_pOverlayManager->IsOnStack((SceneList)10))
            glxSwapSetBlack(true);
        if (mQuitDelay <= 0.0f)
        {
            mQuitDelay = 0.0f;
            FrontEnd::ReturnToFE();
        }
        return;
    }
    if (mStartAnimAtEnd && mPresentation->m_currentSlide != 0)
    {
        mPresentation->m_fadeDuration = 999.9f;
        mStartAnimAtEnd = false;
    }
    if (!mUnidentified54B)
    {
        mUnidentified54B = true;
        FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
    }
    BaseSceneHandler::Update(fDeltaT);
    if (!mUnidentified530)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
            return;
        fn_8023A85C();
        mUnidentified530 = true;
        for (int i = 0; i < 4; ++i)
            GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    if (mIsInTransition)
    {
        for (int i = 0; i < 4; ++i)
            GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
        TLSlide* slide = mPresentation->m_currentSlide;
        float currentTime = slide->GetCurrentTime();
        float endTime = slide->GetStartTime() + slide->GetDuration();
        if (!(currentTime >= endTime))
            return;
        switch (mTransitionTo)
        {
        case TT_OUT:
            FrontEnd::ExitMenuState();
            break;
        case 2:
            mUnidentified54A = true;
            g_pOverlayManager->Push((SceneList)81, SCREEN_FORWARD, true);
            break;
        case 3:
            mUnidentified54A = true;
            g_pOverlayManager->Push((SceneList)82, SCREEN_FORWARD, true);
            break;
        case 4:
            mUnidentified54A = true;
            g_pOverlayManager->Push((SceneList)83, SCREEN_FORWARD, true);
            break;
        case 5:
            mUnidentified54A = true;
            g_pOverlayManager->Push((SceneList)103, SCREEN_NOTHING, true);
            break;
        case 6:
        {
            mUnidentified54A = true;
            PausePostGameScene* scene = static_cast<PausePostGameScene*>(g_pOverlayManager->Push((SceneList)92, SCREEN_FORWARD, true));
            scene->mUnidentified5D8 = mControllingInput;
            scene->SetDisplayMode(12);
            break;
        }
        case 7:
            mUnidentified54A = true;
            g_pOverlayManager->Push((SceneList)104, SCREEN_NOTHING, true);
            break;
        }
        mIsInTransition = false;
        mTransitionTo = TT_IN;
        return;
    }
    u8 goToChooseSides = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (mUnidentified54A)
            return;
        bool curConnected = g_pFEInput->IsConnected((eFEINPUT_PAD)i) && IsFreeStylePad(i);
        if (!curConnected && GameInfoManager::Instance()->GetPlayingSide((unsigned short)i) != -1)
        {
            if (!goToChooseSides)
            {
                while (g_pOverlayManager->GetCurrentScene() != this)
                {
                    g_pOverlayManager->Pop();
                    FESceneManager::Instance()->ForceImmediateStackProcessing();
                }
                mUnidentified54A = true;
                g_pOverlayManager->Push((SceneList)81, SCREEN_FORWARD, true);
            }
            goToChooseSides = 1;
        }
        FrontEnd::m_ctrlConnectedState[i] = curConnected;
        if (g_pFEInput->JustPressed((eFEINPUT_PAD)i, 47, true, 0))
        {
            OnSelectRESUME(0);
            return;
        }
        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
        TLComponentInstance* cursor = GetPointerInstance(i);
        for (int j = 0; j < 7; ++j)
            mUnidentified044[j].HandlePointerEvent(&event);
        if (mUnidentified534[i] > 0)
            cursor->SetActiveSlide("A", true, false);
        else
            cursor->SetActiveSlide("cursor", true, false);
    }
    if (goToChooseSides)
        return;
    mDelayBeforeUnpause = mDelayBeforeUnpause - fDeltaT;
    if (mDelayBeforeUnpause > 0.0f)
        return;
    mDelayBeforeUnpause = 0.0f;
}

void PauseMenuScene::TransitionOut(TransitionType newtype)
{
    mIsInTransition = true;
    mTransitionTo = newtype;
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    presentation->SetActiveSlide("menu out", true);
    presentation->Update(0.0f);
}

/**
 * Offset/Address/Size: 0x1408 | 0x8023A85C | size: 0x338
 */
void PauseMenuScene::fn_8023A85C()
{
    typedef Detail::MemFunImpl<void, void (PauseMenuScene::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, PauseMenuScene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback callback0(PointerBinding(MemFun(&PauseMenuScene::fn_8023AB94), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback callback1(PointerBinding(MemFun(&PauseMenuScene::fn_8023AC58), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback callback2(PointerBinding(MemFun(&PauseMenuScene::fn_8023AD04), this, Placeholder<0>(), Placeholder<1>()));
    for (int i = 0; i < 7; ++i)
    {
        mUnidentified044[i].SetInstanceBounds(mUnidentified028[i], true, 0.0f, 0.0f, 1.0f, 0.5f);
        mUnidentified044[i].SetPointerEnterCallback(callback0);
        mUnidentified044[i].SetPointerLeaveCallback(callback1);
        mUnidentified044[i].SetPointerPressCallback(callback2);
    }
}

/**
 * Offset/Address/Size: 0x1740 | 0x8023AB94 | size: 0xC4
 */
void PauseMenuScene::fn_8023AB94(unsigned int index, void* context)
{
    unsigned int which = (unsigned int)context;
    ++mUnidentified534[index];
    if (!mUnidentified044[which].HasOtherPointerState(1, index))
    {
        FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
        mUnidentified028[which]->SetActiveSlide("over", true, false);
    }
    mUnidentified044[which].SetPointerState(1, index);
}

/**
 * Offset/Address/Size: 0x1804 | 0x8023AC58 | size: 0xAC
 */
void PauseMenuScene::fn_8023AC58(unsigned int index, void* context)
{
    unsigned int which = (unsigned int)context;
    --mUnidentified534[index];
    if (!mUnidentified044[which].HasOtherPointerState(1, index))
        mUnidentified028[which]->SetActiveSlide("off", true, false);
    mUnidentified044[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x18B0 | 0x8023AD04 | size: 0x280
 */
void PauseMenuScene::fn_8023AD04(unsigned int index, void* context)
{
    if (mUnidentified54A)
        return;
    mUnidentified54A = true;
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    switch ((unsigned int)context)
    {
    case 0:
        OnSelectRESUME(0);
        break;
    case 1:
        TransitionOut((TransitionType)2);
        break;
    case 2:
        TransitionOut((TransitionType)3);
        break;
    case 3:
        TransitionOut((TransitionType)4);
        break;
    case 4:
        if (GameInfoManager::Instance()->IsInMode4())
            TransitionOut((TransitionType)5);
        else
            TransitionOut((TransitionType)6);
        break;
    case 5:
        mQuittingController = (eFEINPUT_PAD)index;
        OnSelectQUIT();
        break;
    case 6:
        TransitionOut((TransitionType)7);
        break;
    }
}
