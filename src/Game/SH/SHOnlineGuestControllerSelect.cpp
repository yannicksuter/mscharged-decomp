#include "Game/SH/SHOnlineGuestControllerSelect.h"
#include "Game/SH/SHOnlineInviteStatus.h"
#include "Game/SH/SHOnlineInviteResponse.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/FE/FEAudio.h"
#include "Game/NetworkSession.h"
#include "Game/Sys/debug.h"

#include "Game/GameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FriendManager.h"
#include "NL/nlPrint.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlBasicString.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.h"
#include "NL/nlLocalization.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlString.h"
#include <string.h>
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/FEAudio.h"
#include "NL/nlPrint.h"

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

SHOnlineGuestControllerSelect::SHOnlineGuestControllerSelect()
    : mRespondingToInvitation(false)
    , mState(0)
    , mNavigation()
    , mDoneButton()
    , mControllerButton()
    , mButtonsInitialized(false)
    , mDoneButtonEntering(false)
    , mSelectionConfirmed(false)
{
    SetOnlineTwoLocalPlayers(true);

    mHoverCounts[0] = 0;
    mHoverCounts[1] = 0;
    mHoverCounts[2] = 0;
    mHoverCounts[3] = 0;
    mPrimaryController = gFEControllerIndex;
    mGuestController = gOnlineLocalControllerIndices[1];

    WideBasicString string(LookupLocString("ONLINE_CONTROLLERS_GUEST"));
    memcpy(mGuestName, string.c_str(), sizeof(mGuestName));
    mNavigation.SetPopScene(false);
}

SHOnlineGuestControllerSelect::~SHOnlineGuestControllerSelect()
{
}

void SHOnlineGuestControllerSelect::UpdateDoneButtonVisibility()
{
    if (mDoneButtonInstance->m_bVisible == true)
    {
        if (mGuestController == -1)
        {
            mDoneButtonEntering = false;
            mDoneButtonInstance->m_bVisible = false;
            mDoneButton.mDisabled = true;
            FEPointerEvent event;
            mDoneButton.mPreviousEvents[0] = event;
            mDoneButton.mPreviousEvents[1] = event;
            mDoneButton.mPreviousEvents[2] = event;
            mDoneButton.mPreviousEvents[3] = event;

            for (unsigned int i = 0; i < 4; ++i)
            {
                if (mDoneButton.GetPointerState(i) == 1)
                {
                    --mHoverCounts[i];
                    mDoneButton.SetPointerState(0, i);
                }
            }
        }
    }
    else if (mGuestController != -1)
    {
        FEAudio::PlayAnimAudioEvent(0x2AB04562, 0, 0, 1);
        mDoneButtonInstance->m_bVisible = true;
        mDoneButtonInstance->SetActiveSlide("in", true, false);
        mDoneButtonEntering = true;
    }
}

void SHOnlineGuestControllerSelect::OnControllerPointerEnter(int index, void*)
{
    unsigned int which = index;
    if (mGuestController != -1)
    {
        bool valid = mPrimaryController == which || mGuestController == which;
        if (!valid)
            return;
    }

    if (mPrimaryController == which)
        return;

    if (!mControllerButton.HasOtherPointerState(1, which))
    {
        mHomeInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }

    mControllerButton.SetPointerState(1, which);
    ++mHoverCounts[which];
    mControllerButton.PlayHoverFeedback(index);
}

void SHOnlineGuestControllerSelect::OnControllerPointerLeave(int index, void*)
{
    unsigned int which = index;
    if (mGuestController != -1)
    {
        bool valid = mPrimaryController == which || mGuestController == which;
        if (!valid)
            return;
    }

    if (mPrimaryController == which)
        return;

    if (!mControllerButton.HasOtherPointerState(1, which))
        mHomeInstance->SetActiveSlide("controllers", true, false);

    mControllerButton.SetPointerState(0, which);
    --mHoverCounts[which];
}

void SHOnlineGuestControllerSelect::OnControllerPointerPress(int index, void*)
{
    TLComponentInstance* controller = gFEPointerInstances[index];
    unsigned int which = index;
    if (mGuestController != -1)
    {
        bool valid = mPrimaryController == which || mGuestController == which;
        if (!valid)
            return;
    }

    if (mPrimaryController == which)
        return;

    TLComponentInstance* selected = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance,
        nlStringLowerHash("controllers"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("controller1"),
        0,
        0,
        0);
    TLTextInstance* text = FEFinder<TLTextInstance, 1>::Find(
        selected, nlStringLowerHash("Text"), 0, 0, 0, 0, 0);
    text->SetString(mGuestName);

    TLComponentInstance* highlighted = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance,
        nlStringLowerHash("over"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("controller1"),
        0,
        0,
        0);
    text = FEFinder<TLTextInstance, 1>::Find(
        highlighted, nlStringLowerHash("Text"), 0, 0, 0, 0, 0);
    text->SetString(mGuestName);

    bool already = mPrimaryController == which || mGuestController == which;
    if (!already)
    {
        mGuestController = index;
        controller->SetActiveSlide("A", true, false);
        selected->m_bVisible = true;
        highlighted->m_bVisible = true;
        FEAudio::PlayAnimAudioEvent(0xB3586309, 0, 0, 1);
    }
    else
    {
        if (which == mGuestController)
            mGuestController = -1;
        controller->SetActiveSlide("holding", true, false);
        selected->m_bVisible = false;
        highlighted->m_bVisible = false;
    }

    UpdateDoneButtonVisibility();
}

void SHOnlineGuestControllerSelect::OnDonePointerEnter(int index, void*)
{
    ++mHoverCounts[index];
    mDoneButton.SetPointerState(1, index);
    mDoneButton.PlayHoverFeedback(index);
    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        mDoneButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }
}

void SHOnlineGuestControllerSelect::OnDonePointerInside(int index, void*)
{
    if (mDoneButton.GetPointerState(index) != 0)
        return;

    ++mHoverCounts[index];
    mDoneButton.SetPointerState(1, index);
    mDoneButton.PlayHoverFeedback(index);
    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        mDoneButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }
}

void SHOnlineGuestControllerSelect::OnDonePointerPress(int, void*)
{
    mControllerButton.Disable();
    mDoneButton.Disable();

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    mSelectionConfirmed = true;
    mState = 2;

    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->HideButtons();
    }

    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
}

void SHOnlineGuestControllerSelect::SceneCreated()
{
    TLComponentInstance* screen = 0;
    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->HideButtons();
        screen = object->GetButton(4);
        mDoneButtonInstance = object->GetButton(0x20);
    }

    mNavigation.SetButtonInstance(screen);
    mDoneButtonInstance->m_bVisible = false;

    TLComponentInstance* sideGroup = FEFinder<TLComponentInstance, 4>::Find(mPresentation->m_currentSlide,
        nlStringLowerHash("Layer"),
        nlStringLowerHash("home"),
        0,
        0,
        0,
        0);
    if (sideGroup == 0)
    {
        sideGroup = &UnidentifiedTLComponentDefault::sInstance;
    }
    mHomeInstance = sideGroup;
    mHomeInstance->SetActiveSlide("controllers", true, false);

    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }

    TLComponentInstance* homeController = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance,
        nlStringLowerHash("controllers"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("controller0"),
        0,
        0,
        0);
    TLTextInstance* text = FEFinder<TLTextInstance, 1>::Find(
        homeController, nlStringLowerHash("Text"), 0, 0, 0, 0, 0);
    homeController->m_bVisible = true;
    text->SetString(gNetworkMiiNameWide);

    TLComponentInstance* homeOver = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance,
        nlStringLowerHash("over"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("controller0"),
        0,
        0,
        0);
    text = FEFinder<TLTextInstance, 1>::Find(
        homeOver, nlStringLowerHash("Text"), 0, 0, 0, 0, 0);
    homeOver->m_bVisible = true;
    text->SetString(gNetworkMiiNameWide);

    homeController = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance,
        nlStringLowerHash("controllers"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("controller1"),
        0,
        0,
        0);
    text = FEFinder<TLTextInstance, 1>::Find(
        homeController, nlStringLowerHash("Text"), 0, 0, 0, 0, 0);
    homeController->m_bVisible = mGuestController != -1;
    text->SetString(mGuestName);

    homeOver = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance,
        nlStringLowerHash("over"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("controller1"),
        0,
        0,
        0);
    text = FEFinder<TLTextInstance, 1>::Find(
        homeOver, nlStringLowerHash("Text"), 0, 0, 0, 0, 0);
    homeOver->m_bVisible = mGuestController != -1;
    text->SetString(mGuestName);

    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void SHOnlineGuestControllerSelect::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (mDoneButtonEntering)
    {
        TLSlide* slide = mDoneButtonInstance->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->m_duration + slide->m_start)
        {
            SetDoneButtonBounds(&mDoneButton, mDoneButtonInstance, 0);
            mDoneButton.mDisabled = false;
            mDoneButtonEntering = false;
        }
    }

    if (!GameSceneManager::Instance()->IsOnStack((SceneList)0xA)
        && !mRespondingToInvitation
        && g_pFriendManager->FindHostInvitation())
    {
        SetOnlineTwoLocalPlayers(false);
        int response = IsOnlineRankedMatch();
        FriendManager* friendManager = g_pFriendManager;
        friendManager->mReturnScene = SCENE_ONLINE_GUEST_CONTROLLER_SELECT;
        friendManager->mPreviousRankedMode = response;
        GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_RESPONSE, SCREEN_FORWARD, true);
        return;
    }

    int state = mState;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->m_duration + slide->m_start)
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                gFEPointerInstances[pad]->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (state == 0)
        {
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
            {
                scene->SetButtons(0x24, true);
            }
            InitializeButtons();
            UpdateDoneButtonVisibility();
            mButtonsInitialized = true;
            mState = 1;

            if (mGuestController != -1)
            {
                FEAudio::PlayAnimAudioEvent(0x2AB04562, 0, 0, 1);
                mDoneButtonInstance->m_bVisible = true;
                mDoneButtonInstance->SetActiveSlide("in", true, false);
                mDoneButtonEntering = true;
            }
        }
        else if (state == 2)
        {
            gOnlineLocalControllerIndices[0] = mPrimaryController;
            gOnlineLocalControllerIndices[1] = mGuestController;

            if (mRespondingToInvitation)
            {
                tDebugPrintManager::Print(DC_NETWORK, "Respond invitation With Guest\n");
                JoinOnlineFriendInvitation();
            }
            else if (IsOnlineRankedMatch())
            {
                GameSceneManager::Instance()->Push((SceneList)0x31, SCREEN_FORWARD, true);
            }
            else
            {
                FEAudio::PlayAnimAudioEvent(0x94A22E0E, 0, 0, 1);
                GameSceneManager::Instance()->Push((SceneList)0x1B, SCREEN_FORWARD, true);
            }
            FrontEnd::SetControllerState();
            return;
        }
        else if (state == 3)
        {
            if (mRespondingToInvitation)
            {
                GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_RESPONSE, SCREEN_NOTHING, true);
            }
            else
            {
                SceneList nextScene = IsOnlineRankedMatch() ? (SceneList)0x29 : (SceneList)0x2A;
                GameSceneManager::Instance()->Push(nextScene, SCREEN_NOTHING, true);
            }
            SetOnlineTwoLocalPlayers(false);
            return;
        }
    }

    if (mRespondingToInvitation && !g_pFriendManager->ValidateHostInvitation())
    {
        SetOnlineTwoLocalPlayers(false);
        GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_STATUS, SCREEN_FORWARD, true);
        SHOnlineInviteStatus* scene
            = (SHOnlineInviteStatus*)GameSceneManager::Instance()->GetScene(SCENE_ONLINE_INVITE_STATUS);
        scene->mStatus = 2;
        scene->mReturnDelay = 2.0f;
        return;
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        unsigned char valid = 1;
        TLComponentInstance* controller = gFEPointerInstances[pad];
        FEPointerEvent event;
        event.mIndex = pad;
        event.mPosition = GetPointerPosition(pad, &valid);
        event.mPressed
            = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);

        mDoneButton.HandlePointerEvent(&event);
        mControllerButton.HandlePointerEvent(&event);

        if (mNavigation.UpdateBackButton(event, fDeltaT))
        {
            mState = 3;
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
            {
                scene->HideButtons();
            }
            mPresentation->SetActiveSlide("out", true);
            mPresentation->Update(0.0f);
            return;
        }

        if (mSelectionConfirmed)
        {
            return;
        }

        bool playing
            = mPrimaryController == (unsigned int)pad
           || mGuestController == (unsigned int)pad;
        if (playing
            && !g_pFEInput->IsConnected((eFEINPUT_PAD)pad)
            && (unsigned int)pad != mPrimaryController)
        {
            controller->SetActiveSlide("holding", true, false);
            if ((unsigned int)pad == mGuestController)
            {
                mGuestController = -1;
            }
            mHoverCounts[pad] = 0;

            TLComponentInstance* homeController = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance,
                nlStringLowerHash("controllers"),
                nlStringLowerHash("home_group"),
                nlStringLowerHash("controller1"),
                0,
                0,
                0);
            homeController->m_bVisible = false;

            TLComponentInstance* homeOver = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance,
                nlStringLowerHash("over"),
                nlStringLowerHash("home_group"),
                nlStringLowerHash("controller1"),
                0,
                0,
                0);
            homeOver->m_bVisible = false;
            UpdateDoneButtonVisibility();
        }

        playing
            = mPrimaryController == (unsigned int)pad
           || mGuestController == (unsigned int)pad;
        if (!playing)
        {
            controller->SetActiveSlide("holding", true, false);
        }
        else if (mNavigation.mPointerInside[pad] || mHoverCounts[pad] > 0)
        {
            controller->SetActiveSlide("A", true, false);
        }
        else
        {
            controller->SetActiveSlide("cursor", true, false);
        }
    }
}

void SHOnlineGuestControllerSelect::InitializeButtons()
{
    TLInstance* homeAwayBox = FEFinder<TLInstance, 2>::Find(mHomeInstance->GetActiveSlide(),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("home_away_box"),
        0,
        0,
        0,
        0);
    mControllerButton.SetInstanceBounds(homeAwayBox, true, 0.0f, 0.0f, 1.0f, 1.0f);

    FEPointerListener::Callback callback(Bind<void>(
        MemFun(&SHOnlineGuestControllerSelect::OnControllerPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    mControllerButton.SetPointerEnterCallback(callback);
    callback = FEPointerListener::Callback(Bind<void>(
        MemFun(&SHOnlineGuestControllerSelect::OnControllerPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    mControllerButton.SetPointerLeaveCallback(callback);

    FEPointerListener::Callback selectCallback(Bind<void>(
        MemFun(&SHOnlineGuestControllerSelect::OnControllerPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    mControllerButton.SetPointerPressCallback(selectCallback);
    mControllerButton.mSpeakerEnabled = false;

    callback = FEPointerListener::Callback(Bind<void>(
        MemFun(&SHOnlineGuestControllerSelect::OnDonePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerEnterCallback(callback);
    callback = FEPointerListener::Callback(Bind<void>(
        MemFun(&SHOnlineGuestControllerSelect::OnDonePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerLeaveCallback(callback);
    callback = FEPointerListener::Callback(Bind<void>(
        MemFun(&SHOnlineGuestControllerSelect::OnDonePointerInside), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerInsideCallback(callback);
    selectCallback = FEPointerListener::Callback(Bind<void>(
        MemFun(&SHOnlineGuestControllerSelect::OnDonePointerPress), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerPressCallback(selectCallback);

    {
        mDoneButton.mDisabled = true;
        FEPointerEvent event;
        mDoneButton.mPreviousEvents[0] = event;
        mDoneButton.mPreviousEvents[1] = event;
        mDoneButton.mPreviousEvents[2] = event;
        mDoneButton.mPreviousEvents[3] = event;
    }
}

void SHOnlineGuestControllerSelect::OnDonePointerLeave(int index, void*)
{
    --mHoverCounts[index];
    mDoneButton.SetPointerState(0, index);
    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        mDoneButtonInstance->SetActiveSlide("off", true, false);
    }
}

void SetOnlineRankedMatch(bool value)
{
    GameInfoManager::Instance()->mOnlineRankedMatch = value;
}

bool IsOnlineRankedMatch()
{
    return GameInfoManager::Instance()->mOnlineRankedMatch;
}

void SetOnlineTwoLocalPlayers(bool value)
{
    GameInfoManager::Instance()->mOnlineTwoLocalPlayers = value;
}

bool HasOnlineTwoLocalPlayers()
{
    return GameInfoManager::Instance()->mOnlineTwoLocalPlayers;
}

void SetOnlineFriendSelectionMode(bool value)
{
    GameInfoManager::Instance()->mOnlineFriendSelectionMode = value;
}

bool IsOnlineFriendSelectionMode()
{
    return GameInfoManager::Instance()->mOnlineFriendSelectionMode;
}

void SetOnlineFriendSelectionContext(void* context)
{
    gOnlineFriendSelectionContext = context;
}

void FormatFriendKey(unsigned long long friendKey, u16* output)
{
    if (friendKey != 0)
    {
        nlSNPrintf(output,
            14,
            (const unsigned short*)L"%.6llu %.6llu",
            friendKey / 1000000,
            friendKey % 1000000);
    }
    else
    {
        nlSNPrintf(output, 14, (const unsigned short*)L"UNKNOWN");
    }
}
