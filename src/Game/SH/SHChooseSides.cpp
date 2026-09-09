#include "Game/SH/SHNavigation.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/GameSceneManager.h"
#include "Game/SH/SHChooseSides.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/FEAudio.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/DB/SaveLoad.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feManager.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/Render/Presentation.h"
#include "Game/Team.h"
#include "NL/nlBind.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/FEAudio.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/SH/SHNavigation.h"


extern BaseGameSceneManager* g_pOverlayManager;

class TU8021CBD0Scene : public BaseSceneHandler
{
public:
    virtual void fn_8021CBD0(int value);
};

static const char* lbl_806DE038[2] = { "home_group", "away_group" };
extern const char* lbl_8051CAFC[3];

/**
 * Offset/Address/Size: 0x0 | 0x8021B1EC | size: 0x2B0
 */
SHChooseSides2::SHChooseSides2(eCSContext context, ScreenMovement movement)
    : mUnidentified1C(false)
    , mUnidentified1D(false)
    , mUnidentified1E(false)
    , mUnidentified1F(false)
    , mHomeAwayComponent((void*)2)
    , mHelpComponent()
    , mMovement(movement)
    , mHomeAwayBox(0)
    , mContext(context)
    , mUnidentified408(0)
    , mState(0)
{
    if (movement == SCREEN_BACK || context == PAUSE)
    {
        for (int i = 0; i < 4; ++i)
        {
            mPlayingSides[i] = GameInfoManager::Instance()->GetPlayingSide(i);
        }
    }
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            mPlayingSides[i] = -1;
        }
    }

    if (movement != SCREEN_BACK && mContext != CUP && mContext != PAUSE && mContext != TOURNAMENT)
    {
        Presentation::GetInstance()->Call("StartHomeAwayCaptainHologramSequence");
    }

    mUnidentified2F0.SetPopScene(false);
    if (mContext == CUP)
    {
        mUnidentified2F0.SetPushBackScene(false);
    }
    else if (mContext == TOURNAMENT)
    {
        mUnidentified2F0.SetPushBackScene(false);
    }
    else if (mContext != PAUSE)
    {
        mUnidentified2F0.SetPushBackScene(false);
    }
    else
    {
        mUnidentified2F0.mDisabled = true;
        FEPointerEvent event;
        mUnidentified2F0.mPreviousEvents[0] = event;
        mUnidentified2F0.mPreviousEvents[1] = event;
        mUnidentified2F0.mPreviousEvents[2] = event;
        mUnidentified2F0.mPreviousEvents[3] = event;
    }

    mControllerComponents[0].mContext = 0;
    mControllerComponents[0].mSpeakerEnabled = false;
    mControllerComponents[1].mContext = (void*)1;
    mControllerComponents[1].mSpeakerEnabled = false;
    mHomeAwayComponent.mSpeakerEnabled = false;

    for (int i = 0; i < 4; ++i)
    {
        mControllerCounts[i] = 0;
    }
}

/**
 * Offset/Address/Size: 0x2B0 | 0x8021B49C | size: 0xDC
 */
SHChooseSides2::~SHChooseSides2()
{
    if (mContext == PAUSE)
    {
        SHNavigation* object = GetNavigationScene();
        if (object != 0)
        {
            object->ResetButtons(true);
            object->SetButtonVisibility(mUnidentified408, false);
        }
    }
}

/**
 * Offset/Address/Size: 0x38C | 0x8021B578 | size: 0x113C
 */
void SHChooseSides2::SceneCreated()
{
    TLComponentInstance* sideGroup = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find<TLSlide>(
        mPresentation->m_currentSlide, nlStringLowerHash("Layer"), nlStringLowerHash("home"), 0, 0, 0, 0);
    if (sideGroup == 0)
    {
        sideGroup = &gDefaultTLComponentInstance;
    }
    mSideGroups[0] = sideGroup;

    sideGroup = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find<TLSlide>(
        mPresentation->m_currentSlide, nlStringLowerHash("Layer"), nlStringLowerHash("away"), 0, 0, 0, 0);
    if (sideGroup == 0)
    {
        sideGroup = &gDefaultTLComponentInstance;
    }
    mSideGroups[1] = sideGroup;

    mSideGroups[0]->SetActiveSlide("controllers", true, false);
    mSideGroups[1]->SetActiveSlide("controllers", true, false);

    int team0 = GameInfoManager::Instance()->GetTeam(0);
    int team1 = GameInfoManager::Instance()->GetTeam(1);
    const CharacterInfo& info0 = GetCharacterInfo(GetCharacterIndexFromCaptain(team0));
    const CharacterInfo& info1 = GetCharacterInfo(GetCharacterIndexFromCaptain(team1));

    unsigned long packedColour = GetTeamColour(info0, info1, true);
    nlColour colour0;
    nlColourSet(colour0, packedColour >> 24, packedColour >> 16, packedColour >> 8, packedColour);
    mUnidentified3F0[0] = colour0;

    packedColour = GetTeamColour(info1, info0, true);
    nlColour colour1;
    nlColourSet(colour1, packedColour >> 24, packedColour >> 16, packedColour >> 8, packedColour);
    mUnidentified3F0[1] = colour1;

    TLComponentInstance* screen = 0;
    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        if (mContext == CUP || mContext == TOURNAMENT)
        {
            object->HideButtons();
            screen = object->GetButton(4);
            mHomeAwayBox = object->GetButton(0x10);
            mUnidentified408 = 0x10;
        }
        else if (mContext != PAUSE)
        {
            object->HideButtons();
            screen = object->GetButton(4);
            mHomeAwayBox = object->GetButton(0x20);
            mUnidentified408 = 0x20;
        }
        else
        {
            object->HideButtons();
            mHomeAwayBox = object->GetButton(0x20);
            mUnidentified408 = 0x20;
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);

        char controllerName[16];
        nlSNPrintf(controllerName, 16, "controller%d", i);

        TLComponentInstance* homeController = FEFinder<TLComponentInstance, 4>::Find(
            mSideGroups[0], nlStringLowerHash("controllers"), nlStringLowerHash(lbl_806DE038[0]), nlStringLowerHash(controllerName), 0, 0, 0);
        TLComponentInstance* homeOver = FEFinder<TLComponentInstance, 4>::Find(
            mSideGroups[0], nlStringLowerHash("over"), nlStringLowerHash(lbl_806DE038[0]), nlStringLowerHash(controllerName), 0, 0, 0);

        FEFinder<TLInstance, 2>::_Find<TLSlide>(homeController->GetActiveSlide(),
            nlStringLowerHash("Text"),
            0,
            0,
            0,
            0,
            0);
        FEFinder<TLInstance, 2>::_Find<TLSlide>(homeOver->GetActiveSlide(),
            nlStringLowerHash("Text"),
            0,
            0,
            0,
            0,
            0);

        TLComponentInstance* awayController = FEFinder<TLComponentInstance, 4>::Find(
            mSideGroups[1], nlStringLowerHash("controllers"), nlStringLowerHash(lbl_806DE038[1]), nlStringLowerHash(controllerName), 0, 0, 0);
        TLComponentInstance* awayOver = FEFinder<TLComponentInstance, 4>::Find(
            mSideGroups[1], nlStringLowerHash("over"), nlStringLowerHash(lbl_806DE038[1]), nlStringLowerHash(controllerName), 0, 0, 0);

        FEFinder<TLInstance, 2>::_Find<TLSlide>(awayController->GetActiveSlide(),
            nlStringLowerHash("Text"),
            0,
            0,
            0,
            0,
            0);
        FEFinder<TLInstance, 2>::_Find<TLSlide>(awayOver->GetActiveSlide(),
            nlStringLowerHash("Text"),
            0,
            0,
            0,
            0,
            0);

        if (mPlayingSides[i] == 0)
        {
            homeController->m_bVisible = true;
            homeOver->m_bVisible = true;
            awayController->m_bVisible = false;
            awayOver->m_bVisible = false;
        }
        else if (mPlayingSides[i] == 1)
        {
            homeController->m_bVisible = false;
            homeOver->m_bVisible = false;
            awayController->m_bVisible = true;
            awayOver->m_bVisible = true;
        }
        else
        {
            homeController->m_bVisible = false;
            homeOver->m_bVisible = false;
            awayController->m_bVisible = false;
            awayOver->m_bVisible = false;
        }
    }

    TLComponentInstance* homeCPU = FEFinder<TLComponentInstance, 4>::Find(
        mSideGroups[0], nlStringLowerHash("controllers"), nlStringLowerHash("home_group"), nlStringLowerHash("CPU"), 0, 0, 0);
    TLComponentInstance* awayCPU = FEFinder<TLComponentInstance, 4>::Find(
        mSideGroups[1], nlStringLowerHash("controllers"), nlStringLowerHash("away_group"), nlStringLowerHash("CPU"), 0, 0, 0);
    TLComponentInstance* homeCPUOver = FEFinder<TLComponentInstance, 4>::Find(
        mSideGroups[0], nlStringLowerHash("over"), nlStringLowerHash("home_group"), nlStringLowerHash("CPU"), 0, 0, 0);
    TLComponentInstance* awayCPUOver = FEFinder<TLComponentInstance, 4>::Find(
        mSideGroups[1], nlStringLowerHash("over"), nlStringLowerHash("away_group"), nlStringLowerHash("CPU"), 0, 0, 0);

    if (GameInfoManager::Instance()->IsInMode3())
    {
        if (GameInfoManager::Instance()->GetTeam(0) == g_pCupManager->GetUserSelectedCupTeam())
        {
            mControllerComponents[1].mDisabled = true;
            FEPointerEvent event;
            mControllerComponents[1].mPreviousEvents[0] = event;
            mControllerComponents[1].mPreviousEvents[1] = event;
            mControllerComponents[1].mPreviousEvents[2] = event;
            mControllerComponents[1].mPreviousEvents[3] = event;
            homeCPU->m_bVisible = false;
            homeCPUOver->m_bVisible = false;
        }
        else
        {
            mControllerComponents[0].mDisabled = true;
            FEPointerEvent event;
            mControllerComponents[0].mPreviousEvents[0] = event;
            mControllerComponents[0].mPreviousEvents[1] = event;
            mControllerComponents[0].mPreviousEvents[2] = event;
            mControllerComponents[0].mPreviousEvents[3] = event;
            awayCPU->m_bVisible = false;
            awayCPUOver->m_bVisible = false;
        }
    }
    else if (GameInfoManager::Instance()->IsInMode4())
    {
        mControllerComponents[1].mDisabled = true;
        FEPointerEvent event;
        mControllerComponents[1].mPreviousEvents[0] = event;
        mControllerComponents[1].mPreviousEvents[1] = event;
        mControllerComponents[1].mPreviousEvents[2] = event;
        mControllerComponents[1].mPreviousEvents[3] = event;
        homeCPU->m_bVisible = false;
        homeCPUOver->m_bVisible = false;
    }
    else
    {
        homeCPU->m_bVisible = false;
        awayCPU->m_bVisible = false;
        homeCPUOver->m_bVisible = false;
        awayCPUOver->m_bVisible = false;
        if (mContext != PAUSE)
        {
            FEMusic::StartStreamIfDifferent(2);
        }
    }

    if (mContext != PAUSE)
    {
        mUnidentified2F0.SetButtonInstance(screen);
    }

    for (int team = 0; team < 2; ++team)
    {
        if (mContext != PAUSE)
        {
            const char* componentName = team == 0 ? "sk_left2" : "sk_right";
            TLInstance* component = FEFinder<TLInstance, 5>::_Find<TLSlide>(
                mPresentation->m_currentSlide, nlStringLowerHash("Layer"), nlStringLowerHash(componentName), 0, 0, 0, 0);
            if (component == 0)
            {
                component = &gDefaultTLGroupInstance;
            }

            const char** sidekickName = lbl_8051CAFC;
            for (int slot = 0; slot < 3; ++slot)
            {
                TLComponentInstance* sidekick = (TLComponentInstance*)FEFinder<TLComponentInstance, 2>::_Find<TLInstance>(
                    component, nlStringLowerHash(*sidekickName), 0, 0, 0, 0, 0);
                if (sidekick == 0)
                {
                    sidekick = &gDefaultTLComponentInstance;
                }

                TLImageInstance* image = (TLImageInstance*)FEFinder<TLImageInstance, 2>::_Find<TLSlide>(
                    sidekick->GetActiveSlide(), nlStringLowerHash("00_dummy_texture"), 0, 0, 0, 0, 0);
                if (image == 0)
                {
                    image = &gDefaultTLImageInstance;
                }

                fn_8021ED64(image, GameInfoManager::Instance()->GetSidekick(team, slot), team);
                ++sidekickName;
            }
        }

        TLInstance* instance = FEFinder<TLInstance, 2>::Find(mSideGroups[team],
            nlStringLowerHash("empty"),
            nlStringLowerHash(lbl_806DE038[team]),
            nlStringLowerHash("white_8x8"),
            0,
            0,
            0);
        if (instance == 0)
        {
            instance = &gDefaultTLImageInstance;
        }
        instance->SetAssetColour(mUnidentified3F0[team]);

        instance = FEFinder<TLInstance, 2>::Find(mSideGroups[team],
            nlStringLowerHash("over"),
            nlStringLowerHash(lbl_806DE038[team]),
            nlStringLowerHash("white_8x8"),
            0,
            0,
            0);
        if (instance == 0)
        {
            instance = &gDefaultTLImageInstance;
        }
        instance->SetAssetColour(mUnidentified3F0[team]);

        instance = FEFinder<TLInstance, 2>::Find(mSideGroups[team],
            nlStringLowerHash("controllers"),
            nlStringLowerHash(lbl_806DE038[team]),
            nlStringLowerHash("white_8x8"),
            0,
            0,
            0);
        if (instance == 0)
        {
            instance = &gDefaultTLImageInstance;
        }
        instance->SetAssetColour(mUnidentified3F0[team]);
    }

    TLComponentInstance* help = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find<TLSlide>(
        mPresentation->m_currentSlide, nlStringLowerHash("Layer"), nlStringLowerHash("HELP_BUTTON"), 0, 0, 0, 0);
    if (help == 0)
    {
        help = &gDefaultTLComponentInstance;
    }

    if (IsWidescreen())
    {
        help->SetActiveSlide("16:9", true, false);
    }
    else
    {
        help->SetActiveSlide("4:3", true, false);
    }

    TLComponentInstance* helpButton = (TLComponentInstance*)FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(
        help->GetActiveSlide(), nlStringLowerHash("HELP"), 0, 0, 0, 0, 0);
    if (helpButton == 0)
    {
        helpButton = &gDefaultTLComponentInstance;
    }
    mHelpButton = helpButton;

    if (mContext == PAUSE)
    {
        FEAudio::PlayAnimAudioEvent(0x4861E03D, 0, 0, 1);
    }
    else if (mContext == CUP)
    {
        FEAudio::PlayAnimAudioEvent(0xE3C7087A, 0, 0, 1);
    }
}

/**
 * Offset/Address/Size: 0x14C8 | 0x8021C6B4 | size: 0x51C
 */
void SHChooseSides2::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    mUnidentified1F = false;

    if (mContext != PAUSE)
    {
        UpdateCharacterIdleAnimations(fDeltaT);
    }

    if (mState == 0 || mState == 2 || mState == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->m_time < slide->m_start + slide->m_duration)
        {
            for (int i = 0; i < 4; ++i)
            {
                gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (mState == 0 && !mUnidentified1C)
        {
            SHNavigation* object = GetNavigationScene();
            if (mContext == CUP || mContext == TOURNAMENT)
            {
                object->SetButtons(20, true);
            }
            else if (mContext != PAUSE)
            {
                object->SetButtons(36, true);
            }
            else
            {
                object->SetButtons(32, true);
            }

            object->SetButtonVisibility(mUnidentified408, false);
            BindChooseSideInstances();
            fn_8021EB18();
            mUnidentified1C = true;
            mState = 1;

            for (int i = 0; i < 4; ++i)
            {
                if (mPlayingSides[i] == -1)
                {
                    gFEPointerInstances[i]->SetActiveSlide("holding", true, false);
                }
                else
                {
                    gFEPointerInstances[i]->SetActiveSlide("cursor", true, false);
                }
            }

            if (mContext == PAUSE)
            {
                fn_8021EED8(false);
                return;
            }
        }

        if (mState == 2)
        {
            Proceed();
            return;
        }
        if (mState == 3)
        {
            fn_8021CBD0();
            return;
        }
    }

    if (mUnidentified1D)
    {
        TLSlide* slide = mHomeAwayBox->GetActiveSlide();
        if (slide->m_time >= slide->m_start + slide->m_duration)
        {
            if (mContext == CUP || mContext == TOURNAMENT)
            {
                SetPlayButtonBounds(&mHomeAwayComponent, mHomeAwayBox);
            }
            else
            {
                SetDoneButtonBounds(&mHomeAwayComponent, mHomeAwayBox, 0);
            }
            mHomeAwayComponent.mDisabled = false;
            mUnidentified1D = false;
        }
    }

    if (g_pFEInput->m_InputLockDepth != 0)
    {
        return;
    }

    for (int i = 0; i < 4; ++i)
    {
        bool valid = true;
        TLComponentInstance* controller = gFEPointerInstances[i];
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, (u8*)&valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);

        if (GameInfoManager::Instance()->IsInMode4())
        {
            int required = 1;
            int available = 4 - g_pStrikerChallenge->mMissingSidekicks[HOME];
            if (available > 0)
            {
                required = available;
            }

            int count = 0;
            if (mPlayingSides[0] == 0)
                ++count;
            if (mPlayingSides[1] == 0)
                ++count;
            if (mPlayingSides[2] == 0)
                ++count;
            if (mPlayingSides[3] == 0)
                ++count;

            if (count >= required && mPlayingSides[i] != 0)
            {
                controller->SetActiveSlide("waiting", true, false);
                continue;
            }
        }

        mHomeAwayComponent.HandlePointerEvent(&event);
        mControllerComponents[0].HandlePointerEvent(&event);
        mControllerComponents[1].HandlePointerEvent(&event);

        if (mUnidentified1E)
        {
            return;
        }

        if (mPlayingSides[i] != -1 && !g_pFEInput->IsConnected((eFEINPUT_PAD)i))
        {
            fn_8021E910(i);
        }

        bool leave = false;
        if (mContext != PAUSE)
        {
            if (mUnidentified2F0.UpdateBackButton(event, fDeltaT))
            {
                leave = true;
            }
        }

        if (leave)
        {
            mState = 3;
            SHNavigation* object = GetNavigationScene();
            if (object != 0)
            {
                object->HideButtons();
            }
            mPresentation->SetActiveSlide("out", true);
            return;
        }

        mHelpComponent.HandlePointerEvent(&event);
        if (!mUnidentified1F)
        {
            if (mPlayingSides[i] == -1)
            {
                controller->SetActiveSlide("holding", true, false);
            }
            else if (mUnidentified2F0.mPointerInside[i] || mControllerCounts[i] > 0)
            {
                controller->SetActiveSlide("A", true, false);
            }
            else
            {
                controller->SetActiveSlide("cursor", true, false);
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x19E4 | 0x8021CBD0 | size: 0x234
 */
void SHChooseSides2::fn_8021CBD0()
{
    nlColour white;
    nlColourSet(white, 0xFF, 0xFF, 0xFF, 0xFF);
    GameSceneManager::Instance()->Pop();

    if (mContext == CUP)
    {
        for (int i = 0; i < 4; ++i)
        {
            SetPointerColour(i, white);
        }
        FEAudio::PlayAnimAudioEvent(0xA6F93A5D, 0, 0, 1);
        Presentation::GetInstance()->Call("TransitionChooseSidesToCup");
    }
    else if (mContext == TOURNAMENT)
    {
        for (int i = 0; i < 4; ++i)
        {
            SetPointerColour(i, white);
        }
        Presentation::GetInstance()->Call("TransitionFromStrikerChallengeChooseSides");
        TU8021CBD0Scene* scene = (TU8021CBD0Scene*)GameSceneManager::Instance()->Push(
            (SceneList)77, SCREEN_BACK, false);
        if (scene != 0)
        {
            scene->fn_8021CBD0(8);
        }
    }
    else if (mContext != PAUSE)
    {
        Presentation::GetInstance()->Call("RemoveModels");
        Presentation::GetInstance()->Call("KillLightCones");
        for (int i = 0; i < 4; ++i)
        {
            gFEPointerInstances[i]->SetActiveSlide("cursor", true, false);
            SetPointerColour(i, white);
        }
        GameSceneManager::Instance()->Push((SceneList)3, SCREEN_BACK, false);
        FEAudio::PlayAnimAudioEvent(0xF8F6BB3C, 0, 0, 1);
    }
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            gFEPointerInstances[i]->SetActiveSlide("cursor", true, false);
            SetPointerColour(i, white);
        }
    }
}

/**
 * Offset/Address/Size: 0x1C18 | 0x8021CE04 | size: 0xE24
 */
void SHChooseSides2::BindChooseSideInstances()
{
    TLInstance* found = FEFinder<TLImageInstance, 2>::Find(mSideGroups[0],
        nlStringLowerHash("empty"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("home_away_box"),
        0,
        0,
        0);
    TLInstance* homeInstance;
    if (found == 0)
    {
        homeInstance = &gDefaultTLImageInstance;
    }
    else
    {
        homeInstance = found;
    }
    feVector3 position = mSideGroups[0]->GetAssetPosition();
    mControllerComponents[0].SetInstanceBounds(
        homeInstance, true, position.f.x, position.f.y, 1.0f, 1.0f);

    found = FEFinder<TLImageInstance, 2>::Find(mSideGroups[1],
        nlStringLowerHash("empty"),
        nlStringLowerHash("away_group"),
        nlStringLowerHash("home_away_box"),
        0,
        0,
        0);
    TLInstance* awayInstance;
    if (found == 0)
    {
        awayInstance = &gDefaultTLImageInstance;
    }
    else
    {
        awayInstance = found;
    }
    position = mSideGroups[1]->GetAssetPosition();
    mControllerComponents[1].SetInstanceBounds(
        awayInstance, true, position.f.x, position.f.y, 1.0f, 1.0f);

    FEPointerListener::Callback callback(Bind<void>(MemFun(&SHChooseSides2::fn_8021DC28), this, Placeholder<0>(), Placeholder<1>()));
    mControllerComponents[0].SetPointerEnterCallback(callback);
    mControllerComponents[1].SetPointerEnterCallback(callback);

    callback = FEPointerListener::Callback(Bind<void>(MemFun(&SHChooseSides2::fn_8021DCFC), this, Placeholder<0>(), Placeholder<1>()));
    mControllerComponents[0].SetPointerLeaveCallback(callback);
    mControllerComponents[1].SetPointerLeaveCallback(callback);

    FEPointerListener::Callback selectCallback(Bind<void>(MemFun(&SHChooseSides2::fn_8021DDAC), this, Placeholder<0>(), Placeholder<1>()));
    mControllerComponents[0].SetPointerPressCallback(selectCallback);
    mControllerComponents[1].SetPointerPressCallback(selectCallback);

    callback = FEPointerListener::Callback(Bind<void>(MemFun(&SHChooseSides2::fn_8021DFCC), this, Placeholder<0>(), Placeholder<1>()));
    mHomeAwayComponent.SetPointerEnterCallback(callback);
    callback = FEPointerListener::Callback(Bind<void>(MemFun(&SHChooseSides2::fn_8021E170), this, Placeholder<0>(), Placeholder<1>()));
    mHomeAwayComponent.SetPointerLeaveCallback(callback);
    callback = FEPointerListener::Callback(Bind<void>(MemFun(&SHChooseSides2::fn_8021E098), this, Placeholder<0>(), Placeholder<1>()));
    mHomeAwayComponent.SetPointerInsideCallback(callback);
    selectCallback = FEPointerListener::Callback(Bind<void>(MemFun(&SHChooseSides2::fn_8021E1E0), this, Placeholder<0>(), Placeholder<1>()));
    mHomeAwayComponent.SetPointerPressCallback(selectCallback);

    mHomeAwayComponent.Disable();

    FEPointerListener::Callback helpEnter(Bind<void>(MemFun(&SHChooseSides2::fn_8021E64C), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback helpLeave(Bind<void>(MemFun(&SHChooseSides2::fn_8021E6E8), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback helpSelect(Bind<void>(MemFun(&SHChooseSides2::fn_8021E76C), this, Placeholder<0>(), Placeholder<1>()));

    found = FEFinder<TLImageInstance, 2>::Find(mHelpButton, nlStringLowerHash("OVER"), nlStringLowerHash("list_high_250x60"), 0, 0, 0, 0);
    TLInstance* helpInstance;
    if (found == 0)
    {
        helpInstance = &gDefaultTLImageInstance;
    }
    else
    {
        helpInstance = found;
    }
    position = mHelpButton->GetAssetPosition();
    mHelpComponent.SetInstanceBounds(
        helpInstance, true, position.f.x, position.f.y, 1.0f, 1.0f);

    mHelpComponent.SetPointerEnterCallback(helpEnter);
    mHelpComponent.SetPointerLeaveCallback(helpLeave);
    mHelpComponent.SetPointerPressCallback(helpSelect);
}

/**
 * Offset/Address/Size: 0x2A3C | 0x8021DC28 | size: 0xD4
 */
void SHChooseSides2::fn_8021DC28(int index, void* context)
{
    unsigned long side = (unsigned long)context;
    if (mPlayingSides[index] != -1 && mPlayingSides[index] != side)
        return;

    if (!mControllerComponents[side].HasOtherPointerState(1, index))
    {
        mSideGroups[side]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0x19E7B6AE, 0, 0, 1);
    }

    mControllerComponents[side].SetPointerState(1, index);
    ++mControllerCounts[index];
    mControllerComponents[side].PlayHoverFeedback(index);
}

/**
 * Offset/Address/Size: 0x2B10 | 0x8021DCFC | size: 0xB0
 */
void SHChooseSides2::fn_8021DCFC(int index, void* context)
{
    unsigned long side = (unsigned long)context;
    if (mPlayingSides[index] != -1 && mPlayingSides[index] != side)
        return;

    if (!mControllerComponents[side].HasOtherPointerState(1, index))
    {
        mSideGroups[side]->SetActiveSlide("controllers", true, false);
    }

    mControllerComponents[side].SetPointerState(0, index);
    --mControllerCounts[index];
}

/**
 * Offset/Address/Size: 0x2BC0 | 0x8021DDAC | size: 0x220
 */
void SHChooseSides2::fn_8021DDAC(int index, void* context)
{
    unsigned long side = (unsigned long)context;
    if (mPlayingSides[index] != -1 && mPlayingSides[index] != side)
        return;

    TLComponentInstance* controller = gFEPointerInstances[index];
    char controllerName[16];
    nlSNPrintf(controllerName, 16, "controller%d", index);

    TLComponentInstance* selected = FEFinder<TLComponentInstance, 4>::Find(mSideGroups[side],
        nlStringLowerHash("controllers"),
        nlStringLowerHash(lbl_806DE038[side]),
        nlStringLowerHash(controllerName),
        0,
        0,
        0);
    TLComponentInstance* highlighted = FEFinder<TLComponentInstance, 4>::Find(mSideGroups[side],
        nlStringLowerHash("over"),
        nlStringLowerHash(lbl_806DE038[side]),
        nlStringLowerHash(controllerName),
        0,
        0,
        0);

    if (mPlayingSides[index] == -1)
    {
        controller->SetActiveSlide("A", true, false);
        mPlayingSides[index] = side;
        selected->m_bVisible = true;
        highlighted->m_bVisible = true;

        nlColour colour = mUnidentified3F0[side];
        SetPointerColour(index, colour);
        FEAudio::PlayAnimAudioEvent(0xB3586309, 0, 0, 1);
    }
    else if (mPlayingSides[index] == side)
    {
        controller->SetActiveSlide("OVER", true, false);
        mPlayingSides[index] = -1;
        selected->m_bVisible = false;
        highlighted->m_bVisible = false;

        nlColour white;
        nlColourSet(white, 0xFF, 0xFF, 0xFF, 0xFF);
        SetPointerColour(index, white);
        FEAudio::PlayAnimAudioEvent(0xB3586309, 0, 0, 1);
    }

    fn_8021EB18();
}

/**
 * Offset/Address/Size: 0x2DE0 | 0x8021DFCC | size: 0xCC
 */
void SHChooseSides2::fn_8021DFCC(int index, void*)
{
    ++mControllerCounts[index];
    mHomeAwayComponent.SetPointerState(1, index);
    mHomeAwayComponent.PlayHoverFeedback(index);
    if (!mHomeAwayComponent.HasOtherPointerState(1, index))
    {
        mHomeAwayBox->SetActiveSlide("over", true, false);
        if (mContext == CUP || mContext == TOURNAMENT)
        {
            FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
        }
        else
        {
            FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
        }
    }
}

/**
 * Offset/Address/Size: 0x2EAC | 0x8021E098 | size: 0xD8
 */
void SHChooseSides2::fn_8021E098(int index, void*)
{
    if (mHomeAwayComponent.GetPointerState(index) != 0)
        return;

    ++mControllerCounts[index];
    mHomeAwayComponent.SetPointerState(1, index);
    mHomeAwayComponent.PlayHoverFeedback(index);
    if (!mHomeAwayComponent.HasOtherPointerState(1, index))
    {
        mHomeAwayBox->SetActiveSlide("over", true, false);
        if (mContext == CUP || mContext == TOURNAMENT)
        {
            FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
        }
        else
        {
            FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
        }
    }
}

/**
 * Offset/Address/Size: 0x2F84 | 0x8021E170 | size: 0x70
 */
void SHChooseSides2::fn_8021E170(int index, void*)
{
    --mControllerCounts[index];
    mHomeAwayComponent.SetPointerState(0, index);
    if (!mHomeAwayComponent.HasOtherPointerState(1, index))
    {
        mHomeAwayBox->SetActiveSlide("off", true, false);
    }
}

/**
 * Offset/Address/Size: 0x2FF4 | 0x8021E1E0 | size: 0x27C
 */
void SHChooseSides2::fn_8021E1E0(int, void*)
{
    if (mContext == PAUSE && fn_8021EED8(true))
        return;

    {
        mControllerComponents[0].mDisabled = true;
        FEPointerEvent event;
        mControllerComponents[0].mPreviousEvents[0] = event;
        mControllerComponents[0].mPreviousEvents[1] = event;
        mControllerComponents[0].mPreviousEvents[2] = event;
        mControllerComponents[0].mPreviousEvents[3] = event;
    }
    {
        mControllerComponents[1].mDisabled = true;
        FEPointerEvent event;
        mControllerComponents[1].mPreviousEvents[0] = event;
        mControllerComponents[1].mPreviousEvents[1] = event;
        mControllerComponents[1].mPreviousEvents[2] = event;
        mControllerComponents[1].mPreviousEvents[3] = event;
    }
    {
        mHomeAwayComponent.mDisabled = true;
        FEPointerEvent event;
        mHomeAwayComponent.mPreviousEvents[0] = event;
        mHomeAwayComponent.mPreviousEvents[1] = event;
        mHomeAwayComponent.mPreviousEvents[2] = event;
        mHomeAwayComponent.mPreviousEvents[3] = event;
    }
    mState = 2;

    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->HideButtons();
    }

    if (mContext == CUP || mContext == TOURNAMENT)
    {
        FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, 1);
    }
    else
    {
        FEAudio::PlayAnimAudioEvent(0x9F9BF00F, 0, 0, 1);
    }

    mPresentation->SetActiveSlide("out", true);
    mUnidentified1E = true;
}

/**
 * Offset/Address/Size: 0x3270 | 0x8021E45C | size: 0x1F0
 */
void SHChooseSides2::Proceed()
{
    for (int i = 0; i < 4; ++i)
    {
        GameInfoManager::Instance()->SetPlayingSide(i, (short)mPlayingSides[i]);
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }

    SHNavigation* object = GetNavigationScene();
    if (mContext == CUP)
    {
        FEAudio::PlayAnimAudioEvent(0xF8350154, 0, 0, 1);
        g_pCupManager->mUnidentified869C = 1;
        CupManager* info = g_pCupManager;
        info->mPreviousGameTeams[0] = GameInfoManager::Instance()->GetTeam(0);
        info->mPreviousGameTeams[1] = GameInfoManager::Instance()->GetTeam(1);
        GameSceneManager::Instance()->PushLoadingScene(true);
        SaveLoad::StartSave(false);
        object->SetButtons(0, true);
    }
    else if (mContext == TOURNAMENT)
    {
        GameInfoManager::Instance()->unknown_0x71C8 = 1;
        Presentation::GetInstance()->Call("StartChallengeSequence");
        GameSceneManager::Instance()->PushLoadingScene(true);
        object->SetButtons(0, true);
    }
    else if (mContext != PAUSE)
    {
        FEAudio::PlayAnimAudioEvent(0x64B85E8D, 0, 0, 1);
        GameSceneManager::Instance()->Push((SceneList)5, SCREEN_FORWARD, true);
    }
    else
    {
        g_pTeams[0]->UpdateControllers();
        g_pTeams[1]->UpdateControllers();
        GameInfoManager::Instance()->ApplyDifficultySettings();
        g_pGame->SetDifficulty(GameInfoManager::Instance()->mCurrentDifficulty[0],
            GameInfoManager::Instance()->mCurrentDifficulty[1],
            4,
            false);
        g_pOverlayManager->Push((SceneList)80, SCREEN_BACK, true);
    }

    FrontEnd::SetControllerState();
}

const char* lbl_8051CAFC[3] = { "sk_2", "sk_1", "sk_0" };

/**
 * Offset/Address/Size: 0x3460 | 0x8021E64C | size: 0x9C
 */
void SHChooseSides2::fn_8021E64C(int index, void*)
{
    ++mControllerCounts[index];
    if (!mHelpComponent.HasOtherPointerState(1, index))
    {
        mHelpButton->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xACCDCA48, 0, 0, 1);
    }
    mHelpComponent.SetPointerState(1, index);
}

/**
 * Offset/Address/Size: 0x34FC | 0x8021E6E8 | size: 0x84
 */
void SHChooseSides2::fn_8021E6E8(int index, void*)
{
    --mControllerCounts[index];
    if (!mHelpComponent.HasOtherPointerState(1, index))
    {
        mHelpButton->SetActiveSlide("off", true, false);
    }
    mHelpComponent.SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x3580 | 0x8021E76C | size: 0x1A4
 */
void SHChooseSides2::fn_8021E76C(int, void*)
{
    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }

    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);

    if (mContext == PAUSE)
    {
        FEPopupMenu* popup = (FEPopupMenu*)g_pOverlayManager->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x3B, Function<FnVoidVoid>(FEPopupMenu::Nothing));
        popup->mUnidentified9A1 = true;
    }
    else
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x3B, Function<FnVoidVoid>(FEPopupMenu::Nothing));
        popup->mUnidentified9A1 = true;
    }

    mUnidentified1F = true;
}

/**
 * Offset/Address/Size: 0x3724 | 0x8021E910 | size: 0x208
 */
void SHChooseSides2::fn_8021E910(int index)
{
    gFEPointerInstances[index]->SetActiveSlide("holding", true, false);
    mPlayingSides[index] = -1;
    mControllerCounts[index] = 0;

    char controllerName[16];
    nlSNPrintf(controllerName, 16, "controller%d", index);

    TLInstance* instance = (TLInstance*)FEFindInstance(mSideGroups[0],
        nlStringLowerHash("controllers"),
        nlStringLowerHash(lbl_806DE038[0]),
        nlStringLowerHash(controllerName),
        0,
        0,
        0);
    instance->m_bVisible = false;

    instance = (TLInstance*)FEFindInstance(mSideGroups[1],
        nlStringLowerHash("controllers"),
        nlStringLowerHash(lbl_806DE038[1]),
        nlStringLowerHash(controllerName),
        0,
        0,
        0);
    instance->m_bVisible = false;

    instance = (TLInstance*)FEFindInstance(mSideGroups[0], nlStringLowerHash("over"), nlStringLowerHash(lbl_806DE038[0]), nlStringLowerHash(controllerName), 0, 0, 0);
    instance->m_bVisible = false;

    instance = (TLInstance*)FEFindInstance(mSideGroups[1], nlStringLowerHash("over"), nlStringLowerHash(lbl_806DE038[1]), nlStringLowerHash(controllerName), 0, 0, 0);
    instance->m_bVisible = false;

    nlColour white;
    nlColourSet(white, 0xFF, 0xFF, 0xFF, 0xFF);
    SetPointerColour(index, white);
    fn_8021EB18();
}

/**
 * Offset/Address/Size: 0x392C | 0x8021EB18 | size: 0x24C
 */
void SHChooseSides2::fn_8021EB18()
{
    SHNavigation* object = GetNavigationScene();
    bool hasPlayingSide = false;
    for (int i = 0; i < 4; ++i)
    {
        if (mPlayingSides[i] != -1)
        {
            hasPlayingSide = true;
            break;
        }
    }

    if (mHomeAwayBox->m_bVisible == true)
    {
        if (!hasPlayingSide)
        {
            mUnidentified1D = false;
            object->SetButtonVisibility(mUnidentified408, false);
            mHomeAwayComponent.mDisabled = true;
            FEPointerEvent event;
            mHomeAwayComponent.mPreviousEvents[0] = event;
            mHomeAwayComponent.mPreviousEvents[1] = event;
            mHomeAwayComponent.mPreviousEvents[2] = event;
            mHomeAwayComponent.mPreviousEvents[3] = event;

            for (unsigned int i = 0; i < 4; ++i)
            {
                if (mHomeAwayComponent.GetPointerState(i) == 1)
                {
                    --mControllerCounts[i];
                    mHomeAwayComponent.SetPointerState(0, i);
                }
            }
        }
    }
    else if (hasPlayingSide == true)
    {
        FEAudio::PlayAnimAudioEvent(0x2AB04562, 0, 0, 1);
        object->SetButtonVisibility(mUnidentified408, true);

        if (mContext == CUP || mContext == TOURNAMENT)
        {
            mHomeAwayBox->SetActiveSlide("off", true, false);
        }
        else
        {
            mHomeAwayBox->SetActiveSlide("in", true, false);
        }
        mUnidentified1D = true;
    }
}

/**
 * Offset/Address/Size: 0x3B78 | 0x8021ED64 | size: 0x174
 */
void SHChooseSides2::fn_8021ED64(TLImageInstance* image, int sidekick, int team)
{
    if (image != 0 && sidekick != -1)
    {
        const CharacterInfo& sidekickInfo = GetCharacterInfo(GetCharacterIndexFromSidekick(sidekick));
        char textureName[64];

        switch (sidekick)
        {
        case 0:
        case 1:
        case 4:
        case 5:
        {
            int captain = GameInfoManager::Instance()->GetTeam((short)team);
            const CharacterInfo& captainInfo = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
            if (captain == 0 || captain == 5 || captain == 3 || captain == 1)
            {
                nlSNPrintf(textureName, 64, "sidekick_%s_%s_s", sidekickInfo.mName, captainInfo.mName);
            }
            else
            {
                nlSNPrintf(textureName, 64, "sidekick_%s_mario_s", sidekickInfo.mName);
            }
            break;
        }
        default:
            nlSNPrintf(textureName, 64, "sidekick_%s_s", sidekickInfo.mName);
            break;
        }

        TLInstance* found = (TLInstance*)FEFindInstance(mPresentation, nlStringLowerHash("art"), nlStringLowerHash("Layer"), nlStringLowerHash(textureName), 0, 0, 0);
        TLImageInstance* texture = found == 0 ? 0 : (TLImageInstance*)found;
        if (texture != 0 && texture->m_pTextureResource != 0)
        {
            image->m_pTextureResource = texture->m_pTextureResource;
        }
    }
}

/**
 * Offset/Address/Size: 0x3CEC | 0x8021EED8 | size: 0x134
 */
bool SHChooseSides2::fn_8021EED8(bool playSound)
{
    bool removedController = false;
    for (int i = 0; i < 4; ++i)
    {
        if (mPlayingSides[i] != -1 && g_pFEInput->IsConnected((eFEINPUT_PAD)i) && !IsFreeStylePad(i))
        {
            fn_8021E910(i);
            removedController = true;
        }
    }

    if (removedController)
    {
        if (playSound)
        {
            FEAudio::PlayAnimAudioEvent(0x9F9BF00F, 0, 0, 1);
        }

        FEPopupMenu* popup = (FEPopupMenu*)g_pOverlayManager->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x3C, Function<FnVoidVoid>(FEPopupMenu::Nothing));
        popup->mUnidentified9A1 = true;
    }
    return removedController;
}
