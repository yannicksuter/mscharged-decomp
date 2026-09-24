#include "Game/HBMManager.h"

#include "Game/SH/SHStrikerTimesChallenge.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/FEAudio.h"

#include "Game/GameSceneManager.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/SaveLoad.h"
#include "Game/DB/StatsTracker.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/GameProgress.inl"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/feManager.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "NL/glx/glxSwap.h"
#include "NL/nlConfig.h"
#include "NL/nlPrint.h"
#include "Game/TweakFileLoader.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHChallengeSelect.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/UnidentifiedTLDefault.h"

/**
 * Offset/Address/Size: 0x0 | 0x802337F4 | size: 0x84
 */
SHStrikerTimesChallenge::SHStrikerTimesChallenge()
    : mBackButtonInstance(0)
    , mLoadingChallengeSettings(false)
    , mNewUnlock(false)
{
    mBackButton.SetPushBackScene(false);
    mBackButton.SetPopScene(false);
}

/**
 * Offset/Address/Size: 0x84 | 0x80233878 | size: 0x68
 */
SHStrikerTimesChallenge::~SHStrikerTimesChallenge()
{
}

/**
 * Offset/Address/Size: 0xEC | 0x802338E0 | size: 0x4
 */
void SHStrikerTimesChallenge::SetDisplayMode(unsigned int transition)
{
    SHStrikerTimesBase::SetDisplayMode(transition);
}

/**
 * Offset/Address/Size: 0xF0 | 0x802338E4 | size: 0x64
 */
void SHStrikerTimesChallenge::OnBackTransitionComplete()
{
    GameSceneManager* manager = GameSceneManager::Instance();
    SceneList sceneID = (SceneList)75;
    if (g_pStrikerChallenge->mCurrentChallenge < 10)
        sceneID = (SceneList)76;
    ChallengeSelectScene* scene = static_cast<ChallengeSelectScene*>(manager->Push(sceneID, SCREEN_BACK, true));
    if (scene != 0)
        scene->mChallengeOffset = g_pStrikerChallenge->mUnidentified6C;
}

/**
 * Offset/Address/Size: 0x154 | 0x80233948 | size: 0x38
 */
void SHStrikerTimesChallenge::OnSelectNewChallenge()
{
    FEMusic::StopStream();
    SetPointerEnabled(false);
    GameInfoManager::Instance()->unknown_0x71C8 = 1;
    FrontEnd::ReturnToFE();
}

/**
 * Offset/Address/Size: 0x18C | 0x80233980 | size: 0x38
 */
void SHStrikerTimesChallenge::OnSelectMainMenu()
{
    FEMusic::StopStream();
    SetPointerEnabled(false);
    GameInfoManager::Instance()->unknown_0x71C8 = 0;
    FrontEnd::ReturnToFE();
}

/**
 * Offset/Address/Size: 0x1C4 | 0x802339B8 | size: 0x3AC
 */
void SHStrikerTimesChallenge::SceneCreated()
{
    if (mDisplayMode == 9 && g_pStrikerChallenge->IsCurrentChallengeWon() == true)
    {
        mNewUnlock = g_pStrikerChallenge->UnlockCurrentChallenge();
        if (mNewUnlock)
            SaveLoad::StartSave(false);
    }
    SetPointerEnabled(true);
    StrikerChallenge* challenge = g_pStrikerChallenge;
    int captain = challenge->fn_801CAA18();
    const CharacterInfo& character = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    int mood = -1;
    if (mDisplayMode == 8)
    {
        if (challenge->mCurrentChallenge < 10)
        {
            nlSNPrintf(mStoryStringID, 0x40, "ST_TUTORIAL_START_%s", challenge->GetTitle());
            nlSNPrintf(mHeadlineStringID, 0x40, "STH_TUTORIAL_START_%s", challenge->GetTitle());
        }
        else
        {
            nlSNPrintf(mStoryStringID, 0x40, "ST_%s_CHALLENGE_START", challenge->GetTitle());
            nlSNPrintf(mHeadlineStringID, 0x40, "STH_%s_CHALLENGE_START", challenge->GetTitle());
        }
        mood = 1;
    }
    else if (mDisplayMode == 9)
    {
        FEMusic::StartStreamIfDifferent(13);
        if (challenge->IsCurrentChallengeWon() == true)
        {
            if (challenge->mCurrentChallenge < 10)
            {
                nlSNPrintf(mStoryStringID, 0x40, "ST_TUTORIAL_SUCCEED_%s", challenge->GetTitle());
                nlSNPrintf(mHeadlineStringID, 0x40, "STH_TUTORIAL_SUCCEED_%s", challenge->GetTitle());
            }
            else
            {
                nlSNPrintf(mStoryStringID, 0x40, "ST_CHALLENGE_SUCCEED_%s", challenge->GetTitle());
                nlSNPrintf(mHeadlineStringID, 0x40, "STH_CHALLENGE_SUCCEED_%s", challenge->GetTitle());
            }
            mood = 0;
        }
        else
        {
            if (challenge->mCurrentChallenge < 10)
            {
                nlSNPrintf(mStoryStringID, 0x40, "ST_TUTORIAL_FAILED_%s", challenge->GetTitle());
                nlSNPrintf(mHeadlineStringID, 0x40, "STH_TUTORIAL_FAILED_%s", challenge->GetTitle());
            }
            else
            {
                nlSNPrintf(mStoryStringID, 0x40, "ST_CHALLENGE_FAILED_%s", challenge->GetTitle());
                nlSNPrintf(mHeadlineStringID, 0x40, "STH_CHALLENGE_FAILED_%s", challenge->GetTitle());
            }
            mood = 2;
        }
    }
    if (challenge->mCurrentChallenge == 2)
        SetArticleImageName(captain, mood, 8);
    else if (challenge->mCurrentChallenge == 4)
        SetArticleImageName(captain, mood, 2);
    else if (challenge->mCurrentChallenge == 5)
        SetArticleImageName(captain, 1, 4);
    else
        SetArticleImageName(captain, mood, -1);
    if (mDisplayMode == 8)
    {
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
        {
            scene->SetButtons(4, true);
            mBackButtonInstance = scene->GetButton(4);
            mBackButtonInstance->m_bVisible = false;
        }
        mBackButton.SetButtonInstance(mBackButtonInstance);
    }
    SHStrikerTimesBase::SceneCreated();
}

/**
 * Offset/Address/Size: 0x570 | 0x80233D64 | size: 0x3F0
 */
void SHStrikerTimesChallenge::Update(float dt)
{
    if (mLoadingChallengeSettings)
    {
        if (gTweakFileLoader.ProcessLoadedFiles())
        {
            gTweakFileLoader.mCount = 0;
            g_pStrikerChallenge->LoadSettings();
            FrontEndPresentation::GetInstance()->Call("TransitionToStrikerChallengeChooseSides");
            GameSceneManager::Instance()->Push((SceneList)78, SCREEN_FORWARD, true);
            return;
        }
        return;
    }
    SHStrikerTimesBase::Update(dt);
    if (mDisplayMode == 9)
    {
        TLInstance* instance = FEFinder<TLInstance, 2>::Find<>(mPresentation->m_currentSlide,
            "Layer", "blackbox2");
        nlColour colour = instance->GetAssetColour();
        if (mState == 2)
            nlColourSet(colour, colour[0], colour[1], colour[2], 255);
        else
            nlColourSet(colour, colour[0], colour[1], colour[2], 178);
        instance->SetAssetColour(colour);
    }
    if (mState != 1 || mPage == 0)
        return;
    for (int pad = 0; pad < 4; ++pad)
    {
        if ((unsigned int)pad == gFEControllerIndex && mDisplayMode == 8)
        {
            mBackButtonInstance->m_bVisible = true;
            u8 valid = true;
            FEPointerEvent event;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 30, true, 0);
            if (mBackButton.UpdateBackButton(event, dt))
            {
                if (mDisplayMode != 8)
                    continue;
                mState = 3;
                SHNavigation* scene = GetNavigationScene();
                if (scene != 0)
                    scene->HideButtons();
                mPresentation->SetActiveSlide("out", true);
                mPresentation->Update(0.0f);
                TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::FindOrDefault<>(
                    mPresentation->m_currentSlide, "Layer", "done");
                instance->m_bVisible = false;
                break;
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x960 | 0x80234154 | size: 0x59C
 */
void SHStrikerTimesChallenge::OnDoneTransitionComplete()
{
    SHStrikerTimesBase::OnDoneTransitionComplete();
    if (mDisplayMode == 8)
    {
        mLoadingChallengeSettings = true;
        gTweakFileLoader.LoadFileAsync(g_pStrikerChallenge->GetName(), "/challenge");
    }
    else
    {
        WorldDarkening::Instance().Fade(100.0f, 1.0f);
        if (g_pStrikerChallenge->IsCurrentChallengeWon() == true)
        {
            int challenge = g_pStrikerChallenge->mCurrentChallenge;
            if (mNewUnlock && challenge >= 10)
            {
                FEPopupMenu* popup = (FEPopupMenu*)g_pOverlayManager->Push((SceneList)10, SCREEN_NOTHING, true);
                popup->Create((ePopupMenu)47, OnSelectNewChallenge, OnRestartChallenge, OnSelectMainMenu);
            }
            else
            {
                FEPopupMenu* popup = (FEPopupMenu*)g_pOverlayManager->Push((SceneList)10, SCREEN_NOTHING, true);
                if (challenge >= 10)
                    popup->Create((ePopupMenu)48, OnSelectNewChallenge, OnRestartChallenge, OnSelectMainMenu);
                else
                    popup->Create((ePopupMenu)51, OnSelectNewChallenge, OnRestartChallenge, OnSelectMainMenu);
            }
        }
        else
        {
            int challenge = g_pStrikerChallenge->mCurrentChallenge;
            FEPopupMenu* popup = (FEPopupMenu*)g_pOverlayManager->Push((SceneList)10, SCREEN_NOTHING, true);
            if (challenge >= 10)
                popup->Create((ePopupMenu)49, OnSelectNewChallenge, OnRestartChallenge, OnSelectMainMenu);
            else
                popup->Create((ePopupMenu)52, OnSelectNewChallenge, OnRestartChallenge, OnSelectMainMenu);
        }
    }
}

/**
 * Offset/Address/Size: 0xEFC | 0x802346F0 | size: 0x14C
 */
void SHStrikerTimesChallenge::OnRestartChallenge()
{
    WorldDarkening::Instance().fn_801AF550();
    gpHBMManager->mBlocked = true;
    glxSwapSetBlack(true);
    FEMusic::StopStream();
    SetPointerEnabled(false);
    if (Config::Global().Get<bool>("save_stats", false))
        StatsTracker::Instance()->WriteStats(g_pGame->GetGameTime(), -1.0f, 0);
    StatsTracker::Instance()->ResetCurrentStats();
    FrontEnd::ExitWinnerScreen();
    RestartSinglePlayerGame();
    FrontEnd::m_bGameOver = false;
    FrontEnd::EnterStartScreen(false);
}
