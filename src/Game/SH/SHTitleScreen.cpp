#include "NL/plat/PlatPadManager.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlFunction.inl"
#include "Game/GameSceneManager.h"
#include "Game/SH/SHTitleScreen.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/FEAudio.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/SaveLoad.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/GameInfo.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/TweakRegistry.h"
#include "NL/globalpad.h"
#include "NL/nlBind.h"
#include "NL/nlConfig.h"
#include "NL/nlMath.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/FEAudio.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHLoading.h"
#include "Game/SH/SHMoviePlayer.h"
#include "Game/UnidentifiedStaticStorage.h"

class SHNavigation;
extern "C" int VISetTimeToDimming(int time);

extern bool g_e3_Build;
static bool setDimmingTime;

extern const int lbl_804E8368[10] = {
    13, 14, 13, 14, 11, 12, 0, 1, 2, 0,
};

void StartMovieCB()
{
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    FrontEndPresentation::GetInstance()->Call("TransitionTitleScreenToMainMenu");
    FEAudio::PlayAnimAudioEvent(0x80060B2D, 0, 0, 1);
}

TitleScene::TitleScene(ScreenMovement movement)
    : m_fTimeElapsed(0.0f)
    , mControllerComponent()
    , mStartedDemo(false)
    , mStartedMovie(false)
    , mUnidentifiedDE(false)
    , mUnidentifiedDF(false)
    , mMovement(movement)
{
    for (int i = 0; i < 9; ++i)
    {
        mControllerDefaults[i] = lbl_804E8368[i];
        mControllerReady[i] = false;
    }

    if (!setDimmingTime)
    {
        VISetTimeToDimming(2);
        setDimmingTime = true;
    }

    LoadMemoryCardIconData();
}

TitleScene::~TitleScene()
{
}

void TitleScene::SceneCreated()
{
    if (IsWidescreen())
    {
        mPresentation->SetActiveSlide("widescreen", true);
    }
    else
    {
        mPresentation->SetActiveSlide("regular", true);
    }

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    mTextPressStart = FEFinder<TLComponentInstance, 2>::Find<TLSlide>(
        mPresentation->m_currentSlide,
        nlStringLowerHash("Layer2"),
        nlStringLowerHash("Component2"),
        0,
        0,
        0,
        0);

    FEMusic::StartStreamIfDifferent(0);
    SetPointerEnabled(0);
    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->SetButtons(0, true);
    }

    if (mMovement != SCREEN_BACK)
    {
        FEAudio::PlayAnimAudioEvent(0x26894C84, 0, 0, 1);
    }
}

inline void TitleScene::UnidentifiedInitializeControls()
{
    typedef Detail::MemFunImpl<void, void (TitleScene::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, TitleScene*, Placeholder<0>, Placeholder<1> >
        PointerBinding;
    FEPointerListener::Callback enter(
        PointerBinding(MemFun(&TitleScene::fn_801D2478), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leave(
        PointerBinding(MemFun(&TitleScene::fn_801D24EC), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback select(
        PointerBinding(MemFun(&TitleScene::fn_801D22C8), this, Placeholder<0>(), Placeholder<1>()));

    mControllerComponent.SetInstanceBounds(mTextPressStart, true, 0.0f, 0.0f, 1.0f, 1.0f);
    mControllerComponent.SetPointerEnterCallback(enter);
    mControllerComponent.SetPointerLeaveCallback(leave);
    mControllerComponent.SetPointerPressCallback(select);
}

void TitleScene::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    m_fTimeElapsed += dt;
    if (m_fTimeElapsed < 1.5f)
        return;

    if (!mUnidentifiedDE)
    {
        UnidentifiedInitializeControls();

        for (int i = 0; i < 4; ++i)
        {
            GetPointerInstance(i)->SetActiveSlide("cursor", true, false);
        }
        mUnidentifiedDE = true;
    }

    if (mStartedDemo)
        return;

    float demoTimeout = GetConfigFloat(Config::Global(), "fe_demo_mode_time_out", 60.0f);
    if (GetTweakBool("/user/dosoak", false))
    {
        if (GameInfoManager::Instance()->unknown_0x121 && GetTweakBool("/user/dosoak", false))
        {
            fn_801D1F6C();
        }
        m_fTimeElapsed = 0.0f;
        mStartedDemo = true;
    }
    else if (GetTweakBool("/user/Smoke Test", false)
        && GetTweakBool("/user/Smoke Test FE", false)
        && m_fTimeElapsed >= demoTimeout)
    {
        GameInfoManager::Instance()->SetMode(GameInfoManager::GM_FRIENDLY, false);
        SuperLoadingScene* scene = static_cast<SuperLoadingScene*>(
            GameSceneManager::Instance()->Push(SCENE_SUPER_LOADING, SCREEN_NOTHING, true));
        scene->mType = SuperLoadingScene::TT_IN;
        m_fTimeElapsed = 0.0f;
        mStartedDemo = true;
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* pointer = GetPointerInstance(pad);
        if ((unsigned int)pad != gFEControllerIndex)
        {
            pointer->SetActiveSlide("waiting", true, false);
            continue;
        }

        if (m_fTimeElapsed > 160.0f)
        {
            FEMusic::StopStream();
            SetPointerEnabled(false);
            IntroMovieScene* scene = static_cast<IntroMovieScene*>(
                GameSceneManager::Instance()->Push(SCENE_INTRO_MOVIE, SCREEN_NOTHING, true));
            if (scene != 0)
            {
                scene->fn_801D9B84();
            }
            mStartedDemo = true;
            m_fTimeElapsed = 0.0f;
            return;
        }

        pointer->SetActiveSlide("A", true, false);
        if (g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0))
        {
            fn_801D22C8(pad, 0);
        }

        bool acceptedInput = false;
        for (int input = 0; input < 6; ++input)
        {
            if (!mControllerReady[input])
            {
                if (g_pFEInput->JustPressed(
                        (eFEINPUT_PAD)pad, mControllerDefaults[input], true, 0))
                {
                    mControllerReady[input] = true;
                    acceptedInput = true;
                    break;
                }
                else if (g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1F, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x29, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x28, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x2D, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x2C, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x30, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x31, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x0C, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x0B, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x0D, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x0E, true, 0))
                {
                    for (int reset = 0; reset < 6; ++reset)
                    {
                        mControllerReady[reset] = false;
                    }
                }
            }
        }

        if (!acceptedInput
            && (g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0)
                || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1F, true, 0)
                || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x29, true, 0)
                || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x28, true, 0)
                || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x2D, true, 0)
                || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x2C, true, 0)
                || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x30, true, 0)
                || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x31, true, 0)
                || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x0C, true, 0)
                || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x0B, true, 0)
                || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x0D, true, 0)
                || g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x0E, true, 0)))
        {
            for (int reset = 0; reset < 6; ++reset)
            {
                mControllerReady[reset] = false;
            }
        }

        bool sequenceReady = true;
        for (int input = 0; input < 6; ++input)
        {
            sequenceReady = sequenceReady && mControllerReady[input];
        }

        if (sequenceReady && g_pPlatPadManager->type[pad] == 2)
        {
            WiiFreestylePadStatus status = *g_pPlatPadManager->GetFreestyleStatus(pad);
            float remoteAcceleration = status.kpad.acc_speed;
            float freestyleAcceleration = status.kpad.ex_status.fs.acc_speed;

            for (int input = 6; input < 9; ++input)
            {
                if (!mControllerReady[input])
                {
                    switch (mControllerDefaults[input])
                    {
                    case 1:
                        if (remoteAcceleration > 2.0f)
                            mControllerReady[input] = true;
                        break;
                    case 0:
                        if (freestyleAcceleration > 2.0f)
                            mControllerReady[input] = true;
                        break;
                    case 2:
                        if (freestyleAcceleration > 2.0f && remoteAcceleration > 2.0f)
                            mControllerReady[input] = true;
                        break;
                    }
                }
            }

            for (int input = 0; input < 9; ++input)
            {
                sequenceReady = sequenceReady && mControllerReady[input];
            }

            if (sequenceReady && !fn_8010FD74())
            {
                fn_8010FD7C(true);
                FEAudio::PlayAnimAudioEvent(0xCF37DAC7, 0, 0, true);
                for (int input = 0; input < 9; ++input)
                {
                    mControllerReady[input] = false;
                }
            }
        }
    }
}

void TitleScene::fn_801D1F6C()
{
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    gameInfo->SetMode(GameInfoManager::GM_MODE_2, false);

    bool filterCharacters = g_e3_Build;
    int homeCaptain;
    do
    {
        homeCaptain = nlRandom(12, &nlDefaultSeed);
        int availability = GetCharacterInfo(GetCharacterIndexFromCaptain(homeCaptain)).unknown_0x24;
        if ((!filterCharacters && availability == 1) || (filterCharacters && availability != 0))
            break;
    } while (true);

    int awayCaptain;
    do
    {
        do
        {
            awayCaptain = nlRandom(12, &nlDefaultSeed);
            int availability = GetCharacterInfo(GetCharacterIndexFromCaptain(awayCaptain)).unknown_0x24;
            if ((!filterCharacters && availability == 1) || (filterCharacters && availability != 0))
                break;
        } while (true);
    } while (awayCaptain == homeCaptain);

    int sidekicks[6];
    for (int i = 0; i < 6; ++i)
    {
        do
        {
            sidekicks[i] = nlRandom(8, &nlDefaultSeed);
            if (sidekicks[i] == 3)
                continue;
            int availability = GetCharacterInfo(GetCharacterIndexFromSidekick(sidekicks[i])).unknown_0x24;
            if ((filterCharacters && availability == 1) || (!filterCharacters && availability != 0))
                break;
        } while (true);
    }

    int stadium;
    do
    {
        stadium = nlRandom(17, &nlDefaultSeed);
    } while (!IsStadiumEnabled(stadium));

    gameInfo->SetStadium(stadium);
    gameInfo->SetTeam(0, homeCaptain);
    gameInfo->SetTeam(1, awayCaptain);
    for (int side = 0; side < 2; ++side)
    {
        for (int slot = 0; slot < 3; ++slot)
        {
            gameInfo->SetSidekick(side, sidekicks[side * 3 + slot], slot);
        }
    }
    gameInfo->ResetPlayingSides();
    GameSceneManager::Instance()->PushLoadingScene(true);
}

void TitleScene::fn_801D22C8(int index, void*)
{
    mTextPressStart->SetActiveSlide("down", true, false);
    mControllerComponent.SetPointerState(2, index);
    FEAudio::PlayAnimAudioEvent(0x55C84A9D, 0, 0, 1);
    SetPointerEnabled(1);
    GameSceneManager::Instance()->Pop();
    GameInfoManager::Instance()->mUserInfo.mGameplayOptions.OnSettingsUpdated();
    GameInfoManager::Instance()->mUserInfo.mCheatOptions.OnSettingsUpdated();
    VISetTimeToDimming(0);
    setDimmingTime = false;

    WPADInfo info;
    if (WPADGetInfo(index, &info) == WPAD_ERR_OK && info.battery <= 1)
    {
        FEPopupMenu* popup = static_cast<FEPopupMenu*>(
            GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false));
        popup->Create(POPUP_LOW_BATTERY, Function<FnVoidVoid>(StartMovieCB));
    }
    else
    {
        StartMovieCB();
    }
}

void TitleScene::fn_801D2478(int index, void*)
{
    mTextPressStart->SetActiveSlide("over", true, false);
    mControllerComponent.SetPointerState(1, index);
    FEAudio::PlayAnimAudioEvent(0xAA73EF32, 0, 0, 1);
    mUnidentifiedDF = true;
}

void TitleScene::fn_801D24EC(int index, void*)
{
    mTextPressStart->SetActiveSlide("off", true, false);
    mControllerComponent.SetPointerState(0, index);
    mUnidentifiedDF = false;
}

HealthWarningSceneV2::HealthWarningSceneV2()
    : mState(0)
{
}

HealthWarningSceneV2::~HealthWarningSceneV2()
{
}

void HealthWarningSceneV2::SceneCreated()
{
}

void HealthWarningSceneV2::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    switch (mState)
    {
    case 0:
        if (IsWidescreen())
            mPresentation->SetActiveSlide("fadein_widescreen", true);
        else
            mPresentation->SetActiveSlide("fadein_regular", true);
        mPresentation->Update(0.0f);
        mState = 1;
        break;
    case 1:
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() >= slide->GetDuration())
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                cGlobalPad* controller = g_pPadManager->GetPad(pad);
                if (controller != 0 && controller->IsConnected()
                    && controller->IsPressed(0x1E, true)
                    && controller->IsPressed(0x1F, true))
                {
                    if (IsWidescreen())
                    {
                        mPresentation->SetActiveSlide("fadeout_widescreen", true);
                        mPresentation->Update(0.0f);
                    }
                    else
                    {
                        mPresentation->SetActiveSlide("fadeout_regular", true);
                        mPresentation->Update(0.0f);
                    }
                    mState = 2;
                }
            }
        }
        break;
    }
    case 2:
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
        {
            mState = 3;
            GameSceneManager::Instance()->Push(SCENE_MAIN_MENU, SCREEN_FORWARD, true);
            FrontEndPresentation::GetInstance()->Call("TransitionTitleScreenToMainMenu");
        }
        break;
    }
    }
}

#include "Game/FE/feFinder_impl.h"
