#include "Game/SH/SHTitleScreen.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/DB/SaveLoad.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/GameInfo.h"
#include "Game/Render/Presentation.h"
#include "Game/TweakRegistry.h"
#include "NL/globalpad.h"
#include "NL/nlBind.h"
#include "NL/nlConfig.h"
#include "NL/nlMath.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" Presentation* fn_801FEEAC();
extern "C" void fn_80253284(int value);
extern "C" void* fn_80253E18();
extern "C" void fn_802534BC(void* object, int value, bool enabled);
extern "C" bool fn_80273B00();
extern "C" cGlobalPad* fn_802C082C(void* owner, int pad);
extern "C" bool fn_801B61B8(int stadium);
extern "C" int VISetTimeToDimming(int time);

extern TLComponentInstance* lbl_80578450[4];
extern BaseGameSceneManager* lbl_806E1838;
extern void* lbl_806E1E28;
extern bool lbl_806E1090;

extern const int lbl_804E8368[10] = {
    13, 14, 13, 14, 11, 12, 0, 1, 2, 0,
};

extern const int lbl_804E8390[18] = {
    13, 11, 15, 16, 7, 14, 3, 4, 9, 5, 0, 1, 2, 6, 10, 8, 12, 0,
};

void StartMovieCB()
{
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }
    fn_801FEEAC()->Call("TransitionTitleScreenToMainMenu");
    fn_801CBCA0(0x80060B2D, 0, 0, 1);
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

    static bool setDimmingTime = false;
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
    if (fn_80273B00())
    {
        mPresentation->SetActiveSlide("widescreen", true);
    }
    else
    {
        mPresentation->SetActiveSlide("regular", true);
    }

    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    mTextPressStart = FEFinder<TLComponentInstance, 2>::Find<TLSlide>(
        mPresentation->GetActiveSlide(), InlineHasher("Layer2"), InlineHasher("Component2"));

    FEMusic::StartStreamIfDifferent(0);
    fn_80253284(0);
    void* object = fn_80253E18();
    if (object != 0)
    {
        fn_802534BC(object, 0, true);
    }

    if (mMovement != SCREEN_BACK)
    {
        fn_801CBCA0(0x26894C84, 0, 0, 1);
    }
}

void TitleScene::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    m_fTimeElapsed += dt;
    if (m_fTimeElapsed < 1.5f)
        return;

    if (!mUnidentifiedDE)
    {
        TU80300104Base::Callback enter(
            Bind<void>(MemFun(&TitleScene::fn_801D2478), this, Placeholder<0>(), Placeholder<1>()));
        TU80300104Base::Callback leave(
            Bind<void>(MemFun(&TitleScene::fn_801D24EC), this, Placeholder<0>(), Placeholder<1>()));
        TU80300104Base::Callback select(
            Bind<void>(MemFun(&TitleScene::fn_801D22C8), this, Placeholder<0>(), Placeholder<1>()));

        mControllerComponent.fn_80300D74(mTextPressStart, true, 0.0f, 0.0f, 1.0f, 1.0f);
        mControllerComponent.fn_803007C0(enter);
        mControllerComponent.fn_80300864(leave);
        mControllerComponent.fn_803009AC(select);

        for (int i = 0; i < 4; ++i)
        {
            lbl_80578450[i]->SetActiveSlide("cursor", true, false);
        }
        mUnidentifiedDE = true;
    }

    if (!mStartedDemo)
    {
        float demoTimeout = GetConfigFloat(Config::Global(), "fe_demo_mode_time_out", 60.0f);
        if (fn_802C2C84("/user/dosoak", false))
        {
            if (GameInfoManager::Instance()->unknown_0x121 && fn_802C2C84("/user/dosoak", false))
            {
                fn_801D1F6C();
            }
            m_fTimeElapsed = 0.0f;
            mStartedDemo = true;
        }
        else if (fn_802C2C84("/user/Smoke Test", false)
            && fn_802C2C84("/user/Smoke Test FE", false)
            && m_fTimeElapsed >= demoTimeout)
        {
            GameInfoManager::Instance()->SetMode(GameInfoManager::GM_FRIENDLY, false);
            lbl_806E1838->Push(SCENE_SUPER_LOADING, SCREEN_NOTHING, true);
            m_fTimeElapsed = 0.0f;
            mStartedDemo = true;
        }
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        lbl_80578450[pad]->SetActiveSlide("A", true, false);
        if (g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0))
        {
            fn_801D22C8(pad, 0);
        }
    }
}

void TitleScene::fn_801D1F6C()
{
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    gameInfo->SetMode(GameInfoManager::GM_MODE_2, false);

    bool filterCharacters = lbl_806E1090;
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
    } while (!fn_801B61B8(stadium));

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
    lbl_806E1838->PushLoadingScene(true);
}

void TitleScene::fn_801D22C8(int index, void*)
{
    mTextPressStart->SetActiveSlide("down", true, false);
    mControllerComponent.mValues[index] = 2;
    fn_801CBCA0(0x55C84A9D, 0, 0, 1);
    fn_80253284(1);
    lbl_806E1838->Pop();
    VISetTimeToDimming(0);
    StartMovieCB();
}

void TitleScene::fn_801D2478(int index, void*)
{
    mTextPressStart->SetActiveSlide("over", true, false);
    mControllerComponent.mValues[index] = 1;
    fn_801CBCA0(0xAA73EF32, 0, 0, 1);
    mUnidentifiedDF = true;
}

void TitleScene::fn_801D24EC(int index, void*)
{
    mTextPressStart->SetActiveSlide("off", true, false);
    mControllerComponent.mValues[index] = 0;
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

void HealthWarningSceneV2::Update(float dt)
{
    BaseSceneHandler::Update(dt);

    switch (mState)
    {
    case 0:
        if (fn_80273B00())
            mPresentation->SetActiveSlide("fadein_widescreen", true);
        else
            mPresentation->SetActiveSlide("fadein_regular", true);
        mPresentation->Update(0.0f);
        mState = 1;
        break;
    case 1:
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->m_duration)
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                cGlobalPad* controller = fn_802C082C(lbl_806E1E28, pad);
                if (controller != 0 && controller->IsConnected()
                    && controller->IsPressed(0x1E, true)
                    && controller->IsPressed(0x1F, true))
                {
                    if (fn_80273B00())
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
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->m_start + slide->m_duration)
        {
            mState = 3;
            lbl_806E1838->Push(SCENE_MAIN_MENU, SCREEN_FORWARD, true);
            fn_801FEEAC()->Call("TransitionTitleScreenToMainMenu");
        }
        break;
    }
    }
}
