#include "Game/SH/SHOnlineGuestControllerSelect.h"
#include "NL/nlFunction.inl"
#include "Game/SH/SHOnlineInviteStatus.h"
#include "Game/SH/SHOnlineInviteResponse.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/FEAudio.h"
#include "Game/NetworkSession.h"
#include "Game/Sys/debug.h"

#include "Game/GameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/feManager.h"
#include "Game/FE/fePointer.inl"
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

void SHOnlineGuestControllerSelect::SceneCreated()
{
    SHNavigation* object = GetNavigationScene();
    TLComponentInstance* screen = 0;
    if (object != 0)
    {
        object->HideButtons();
        screen = object->GetButton(4);
        mDoneButtonInstance = object->GetButton(0x20);
    }

    mNavigation.SetButtonInstance(screen);
    mDoneButtonInstance->m_bVisible = false;

    TLComponentInstance* sideGroup = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "home");
    mHomeInstance = sideGroup;
    mHomeInstance->SetActiveSlide("controllers", true, false);

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    TLComponentInstance* homeController = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance, "controllers", "home_group", "controller0");
    TLTextInstance* text = FEFinder<TLTextInstance, 1>::Find(homeController, "Text");
    homeController->m_bVisible = true;
    text->SetString(gNetworkMiiNameWide);

    TLComponentInstance* homeOver = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance, "over", "home_group", "controller0");
    text = FEFinder<TLTextInstance, 1>::Find(homeOver, "Text");
    homeOver->m_bVisible = true;
    text->SetString(gNetworkMiiNameWide);

    homeController = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance, "controllers", "home_group", "controller1");
    text = FEFinder<TLTextInstance, 1>::Find(homeController, "Text");
    homeController->m_bVisible = mGuestController != -1;
    text->SetString(mGuestName);

    homeOver = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance, "over", "home_group", "controller1");
    text = FEFinder<TLTextInstance, 1>::Find(homeOver, "Text");
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
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
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
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                GetPointerInstance(pad)->SetActiveSlide("waiting", true, false);
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
        TLComponentInstance* controller = GetPointerInstance(pad);
        unsigned char valid = 1;
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

        bool playing = IsLocalController(pad);
        if (playing
            && !g_pFEInput->IsConnected((eFEINPUT_PAD)pad)
            && (unsigned int)pad != mPrimaryController)
        {
            GetPointerInstance(pad)->SetActiveSlide("holding", true, false);
            if ((unsigned int)pad == mGuestController)
            {
                mGuestController = -1;
            }
            mHoverCounts[(unsigned int)pad] = 0;

            TLComponentInstance* homeController = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance, "controllers", "home_group", "controller1");
            homeController->m_bVisible = false;

            TLComponentInstance* homeOver = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance, "over", "home_group", "controller1");
            homeOver->m_bVisible = false;
            UpdateDoneButtonVisibility();
        }

        playing = IsLocalController(pad);
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
    typedef Detail::MemFunImpl<void, void (SHOnlineGuestControllerSelect::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHOnlineGuestControllerSelect*, Placeholder<0>, Placeholder<1> > PointerBinding;

    TLInstance* homeAwayBox = FEFinder<TLInstance, 2>::Find<TLSlide>(mHomeInstance->GetActiveSlide(), "home_group", "home_away_box");
    mControllerButton.SetInstanceBounds(homeAwayBox, true, 0.0f, 0.0f, 1.0f, 1.0f);

    FEPointerListener::Callback callback(PointerBinding(MemFun(&SHOnlineGuestControllerSelect::OnControllerPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    mControllerButton.SetPointerEnterCallback(callback);
    callback = FEPointerListener::Callback(PointerBinding(MemFun(&SHOnlineGuestControllerSelect::OnControllerPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    mControllerButton.SetPointerLeaveCallback(callback);

    FEPointerListener::Callback selectCallback(PointerBinding(MemFun(&SHOnlineGuestControllerSelect::OnControllerPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    mControllerButton.SetPointerPressCallback(selectCallback);
    mControllerButton.mSpeakerEnabled = false;

    callback = FEPointerListener::Callback(PointerBinding(MemFun(&SHOnlineGuestControllerSelect::OnDonePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerEnterCallback(callback);
    callback = FEPointerListener::Callback(PointerBinding(MemFun(&SHOnlineGuestControllerSelect::OnDonePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerLeaveCallback(callback);
    callback = FEPointerListener::Callback(PointerBinding(MemFun(&SHOnlineGuestControllerSelect::OnDonePointerInside), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerInsideCallback(callback);
    selectCallback = FEPointerListener::Callback(PointerBinding(MemFun(&SHOnlineGuestControllerSelect::OnDonePointerPress), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerPressCallback(selectCallback);

    mDoneButton.Disable();
}

void SHOnlineGuestControllerSelect::UpdateDoneButtonVisibility()
{
    if (mDoneButtonInstance->m_bVisible == true)
    {
        if (mGuestController == -1)
        {
            mDoneButtonEntering = false;
            mDoneButtonInstance->m_bVisible = false;
            mDoneButton.Disable();

            for (int i = 0; i < 4; ++i)
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
        if (!IsLocalController(which))
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
        if (!IsLocalController(which))
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
    TLComponentInstance* controller = GetPointerInstance(index);
    unsigned int which = index;
    if (mGuestController != -1)
    {
        if (!IsLocalController(which))
            return;
    }

    if (mPrimaryController == which)
        return;

    TLComponentInstance* selected
        = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance, "controllers", "home_group", "controller1");
    TLTextInstance* text = FEFinder<TLTextInstance, 1>::Find(selected, "Text");
    text->SetString(mGuestName);

    TLComponentInstance* highlighted
        = FEFinder<TLComponentInstance, 4>::Find(mHomeInstance, "over", "home_group", "controller1");
    text = FEFinder<TLTextInstance, 1>::Find(highlighted, "Text");
    text->SetString(mGuestName);

    bool already = IsLocalController(which);
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

void SHOnlineGuestControllerSelect::OnDonePointerLeave(int index, void*)
{
    --mHoverCounts[index];
    mDoneButton.SetPointerState(0, index);
    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        mDoneButtonInstance->SetActiveSlide("off", true, false);
    }
}
