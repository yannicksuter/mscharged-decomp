#include "Game/SH/SHOnlineFriendsChooseSides.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/FEAudio.h"
#include "Game/Sys/debug.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/GameInfo.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkLobby.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlstring_tmpl.h"

#include "Game/FE/FEAudio.h"
#include "Game/FE/UnidentifiedTLDefault.h"

const char* gOnlineSideGroupNames[2] = { "home_group", "away_group" };
int gOnlineSideSelectionSeconds = 30;

SHOnlineFriendsChooseSides::SHOnlineFriendsChooseSides()
    : mDisconnectPopupActive(false)
    , mButtonsInitialized(false)
    , mDoneButtonAnimating(false)
    , mDraftStarted(false)
    , mDoneButton((void*)2)
    , mDoneButtonInstance(0)
    , mPlayerCount(0)
    , mCountdownTimer(1.0f,
          Function<FETimer*>(
              Bind<void>(MemFun(&SHOnlineFriendsChooseSides::OnCountdownTick), this, Placeholder<0>())))
    , mTimerTextDirty(false)
    , mSecondsRemaining(gOnlineSideSelectionSeconds)
{
    for (int i = 0; i < 4; ++i)
    {
        mPlayerSides[i] = -1;
    }

    mSideButtons[0].mContext = (void*)0;
    mSideButtons[0].mSpeakerEnabled = false;
    mSideButtons[1].mContext = (void*)1;
    mSideButtons[1].mSpeakerEnabled = false;
    mDoneButton.mSpeakerEnabled = false;

    for (int i = 0; i < 4; ++i)
    {
        mPointerInsideCounts[i] = 0;
        for (int j = 0; j < 4; ++j)
        {
            mPlayerColours[i].c[j] = 0xFF;
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        mSidePlayerIndices[0][i] = -1;
        mSidePlayerIndices[1][i] = -1;
    }
    mCountdownTimer.SetEnabled(true);
}

SHOnlineFriendsChooseSides::~SHOnlineFriendsChooseSides()
{
    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->mTimer->m_bVisible = false;
    }
}

void SHOnlineFriendsChooseSides::SceneCreated()
{
    NetworkDraft::Instance();
    mSideInstances[0] = FEFinder<TLComponentInstance, 4>::Find<>(
        mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("home"));
    if (mSideInstances[0] == 0)
    {
        mSideInstances[0] = &UnidentifiedTLComponentDefault::sInstance;
    }
    mSideInstances[1] = FEFinder<TLComponentInstance, 4>::Find<>(
        mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("away"));
    if (mSideInstances[1] == 0)
    {
        mSideInstances[1] = &UnidentifiedTLComponentDefault::sInstance;
    }
    mSideInstances[0]->SetActiveSlide("controllers", true, false);
    mSideInstances[1]->SetActiveSlide("controllers", true, false);

    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->SetButtons(0x20, true);
        mDoneButtonInstance = object->GetButton(0x20);
    }
    mDoneButtonInstance->m_bVisible = false;

    int machine = 0;
    bool guest = false;
    int groups = 0;
    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
        char controller[16];
        char friendName[16];
        nlSNPrintf(controller, sizeof(controller), "controller%d", i);
        nlSNPrintf(friendName, sizeof(friendName), "friend_%d", i);

        TLComponentInstance* home = FEFinder<TLComponentInstance, 4>::Find<>(
            mSideInstances[0], nlStringLowerHash("controllers"),
            nlStringLowerHash(gOnlineSideGroupNames[0]), nlStringLowerHash(controller), 0, 0, 0);
        TLComponentInstance* homeOver = FEFinder<TLComponentInstance, 4>::Find<>(
            mSideInstances[0], nlStringLowerHash("over"),
            nlStringLowerHash(gOnlineSideGroupNames[0]), nlStringLowerHash(controller), 0, 0, 0);
        TLComponentInstance* away = FEFinder<TLComponentInstance, 4>::Find<>(
            mSideInstances[1], nlStringLowerHash("controllers"),
            nlStringLowerHash(gOnlineSideGroupNames[1]), nlStringLowerHash(controller), 0, 0, 0);
        TLComponentInstance* awayOver = FEFinder<TLComponentInstance, 4>::Find<>(
            mSideInstances[1], nlStringLowerHash("over"),
            nlStringLowerHash(gOnlineSideGroupNames[1]), nlStringLowerHash(controller), 0, 0, 0);
        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find<>(
            mPresentation->m_currentSlide, InlineHasher("Layer"), InlineHasher(friendName));

        FEFinder<TLTextInstance, 3>::Find<>(home->GetActiveSlide(), InlineHasher("Text"));
        FEFinder<TLTextInstance, 3>::Find<>(homeOver->GetActiveSlide(), InlineHasher("Text"));
        FEFinder<TLTextInstance, 3>::Find<>(away->GetActiveSlide(), InlineHasher("Text"));
        FEFinder<TLTextInstance, 3>::Find<>(awayOver->GetActiveSlide(), InlineHasher("Text"));

        if (machine < mDraftMessage.mMachineCount)
        {
            if (!guest)
            {
                NetworkDraftMachineInfo entry = mDraftMessage.mEntries[machine];
                nlStrNCpy(mPlayerNames[i], entry.mName, 14);
                mOnlinePlayers[i].mMachineIndex = machine;
                mOnlinePlayers[i].mIsGuest = false;
                if (entry.mGuestEnabled != 0)
                {
                    ++groups;
                    guest = true;
                    if (groups == 1)
                    {
                        nlColourSet(mPlayerColours[i], 237, 0, 12, 255);
                    }
                    else
                    {
                        nlColourSet(mPlayerColours[i], 34, 98, 171, 255);
                    }
                    text->SetAssetColour(mPlayerColours[i]);
                }
                else
                {
                    guest = false;
                    ++machine;
                }
            }
            else
            {
                nlStrNCpy(mPlayerNames[i], LookupLocString("ONLINE_CONTROLLERS_GUEST"), 14);
                guest = false;
                mOnlinePlayers[i].mMachineIndex = machine;
                ++machine;
                mOnlinePlayers[i].mIsGuest = true;
                if (groups == 1)
                {
                    nlColourSet(mPlayerColours[i], 237, 0, 12, 255);
                }
                else
                {
                    nlColourSet(mPlayerColours[i], 34, 98, 171, 255);
                }
                text->SetAssetColour(mPlayerColours[i]);
            }
            text->SetString(mPlayerNames[i]);
            ++mPlayerCount;
        }
        else
        {
            text->m_bVisible = false;
        }
        home->m_bVisible = false;
        homeOver->m_bVisible = false;
        away->m_bVisible = false;
        awayOver->m_bVisible = false;
    }

    mSelectSideText = FEFinder<TLTextInstance, 3>::Find<>(
        mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("Text"));
    if (mSelectSideText == 0)
    {
        mSelectSideText = &UnidentifiedTLTextDefault::sInstance;
    }
    TLTextInstance* timer = static_cast<TLTextInstance*>(GetNavigationScene()->mTimer);
    GetNavigationScene()->mTimer->m_bVisible = true;
    nlSNPrintf(mTimerText, 8, (const unsigned short*)L"%d", mSecondsRemaining);
    timer->SetString(mTimerText);
    memset(g_pFriendManager->mFriendCodeInput, 0, sizeof(g_pFriendManager->mFriendCodeInput));
    FEAudio::PlayAnimAudioEvent(0x7EF9225A, 0, 0, 1);
}

void SHOnlineFriendsChooseSides::OnCountdownTick(FETimer* timer)
{
    mTimerTextDirty = true;
    if (mSecondsRemaining > 0)
    {
        --mSecondsRemaining;
        if (mSecondsRemaining <= 5 && mSecondsRemaining > 0)
        {
            if (mSecondsRemaining == 1)
            {
                FEAudio::PlayAnimAudioEvent(0x09AA8790, 0, 0, 1);
            }
            else
            {
                FEAudio::PlayAnimAudioEvent(0xFF48403F, 0, 0, 1);
            }
        }
    }
}

void SHOnlineFriendsChooseSides::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mDisconnectPopupActive && !g_pFEInput->HasInputLock(this))
    {
        return;
    }
    mCountdownTimer.Update(fDeltaT);
    if (mTimerTextDirty)
    {
        TLTextInstance* timer = static_cast<TLTextInstance*>(GetNavigationScene()->mTimer);
        GetNavigationScene()->mTimer->m_bVisible = true;
        nlSNPrintf(mTimerText, 8, (const unsigned short*)L"%d", mSecondsRemaining);
        timer->SetString(mTimerText);
        mTimerTextDirty = false;
    }

    if (mDoneButtonAnimating)
    {
        TLSlide* slide = mDoneButtonInstance->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->m_start + slide->m_duration)
        {
            SetDoneButtonBounds(&mDoneButton, mDoneButtonInstance, 0);
            mDoneButton.mDisabled = false;
            mDoneButtonAnimating = false;
        }
    }

    if (!mButtonsInitialized)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->m_start + slide->m_duration)
        {
            return;
        }
        InitializeButtons();
        UpdateDoneButton();
        mButtonsInitialized = true;
        for (int pad = 0; pad < 4; ++pad)
        {
            TLComponentInstance* controller = gFEPointerInstances[pad];
            int index = GetOnlinePlayerIndex(pad);
            if (index == -1)
            {
                controller->SetActiveSlide("waiting", true, false);
            }
            else if (mPlayerSides[index] == -1)
            {
                controller->SetActiveSlide("holding", true, false);
            }
            else
            {
                controller->SetActiveSlide("cursor", true, false);
            }
        }
    }

    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    bool disconnected = false;
    for (int i = 0; i < roster->GetMachineCount(); ++i)
    {
        if (roster->GetMachineAid(i) == 0)
        {
            disconnected = true;
            break;
        }
    }
    if (disconnected)
    {
        g_pNetworkSession->GetOnlineLobby()->CloseConnectionsAndReset();
        SHNavigation* object = GetNavigationScene();
        if (object != 0)
        {
            object->mTimer->m_bVisible = false;
        }
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = static_cast<FEPopupMenu*>(
                GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false));
            popup->Create((ePopupMenu)0x60,
                Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineFriendsChooseSides::OnDisconnectPopupClosed), this)));
            mDisconnectPopupActive = true;
        }
        return;
    }

    if (mSecondsRemaining == 0 && mDraftMessage.mMachineIndex == 0 && !mDraftStarted)
    {
        for (int i = 0; i < mPlayerCount; ++i)
        {
            if (mPlayerSides[i] == -1)
            {
                int home = 0;
                for (int j = 0; j < 4; ++j)
                {
                    if (mPlayerSides[j] == 0)
                    {
                        ++home;
                    }
                }
                int away = 0;
                for (int j = 0; j < 4; ++j)
                {
                    if (mPlayerSides[j] == 1)
                    {
                        ++away;
                    }
                }
                mPlayerSides[i] = home > away ? 1 : 0;
            }
        }
        OnDonePointerPress(0, (void*)2);
        return;
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = gFEPointerInstances[pad];
        int index = GetOnlinePlayerIndex(pad);
        if (index == -1)
        {
            controller->SetActiveSlide("waiting", true, false);
            continue;
        }
        u8 valid = 1;
        FEPointerEvent event;
        event.mIndex = pad;
        event.mPosition = GetPointerPosition(pad, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
        mDoneButton.HandlePointerEvent(&event);
        mSideButtons[0].HandlePointerEvent(&event);
        mSideButtons[1].HandlePointerEvent(&event);
        if (mDraftStarted)
        {
            return;
        }
        if (mPlayerSides[index] != -1 && !g_pFEInput->IsConnected((eFEINPUT_PAD)pad))
        {
            GetOnlinePlayerIndex(pad);
            mPointerInsideCounts[pad] = 0;
            NetMessageSidesChanged message;
            message.mMachineIndex = mDraftMessage.mMachineIndex;
            message.mSide = -1;
            message.mAccepted = 0;
            if (HasOnlineTwoLocalPlayers())
            {
                message.mGuest = pad == gOnlineLocalControllerIndices[1];
            }
            else
            {
                message.mGuest = 0;
            }
            g_pNetworkSession->SendSidesChangedToHost(&message);
        }
        if (mPlayerSides[index] == -1)
        {
            controller->SetActiveSlide("holding", true, false);
        }
        else
        {
            controller->SetActiveSlide("cursor", true, false);
        }
    }
}

void SHOnlineFriendsChooseSides::InitializeButtons()
{
    typedef Detail::MemFunImpl<void, void (SHOnlineFriendsChooseSides::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHOnlineFriendsChooseSides*, Placeholder<0>, Placeholder<1> > PointerBinding;

    mSideButtons[0].SetInstanceBounds(mSideInstances[0], true, 0.0f, 0.0f, 1.0f, 1.0f);
    mSideButtons[1].SetInstanceBounds(mSideInstances[1], true, 0.0f, 0.0f, 1.0f, 1.0f);

    FEPointerListener::Callback callback(
        PointerBinding(MemFun(&SHOnlineFriendsChooseSides::OnSidePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    mSideButtons[0].SetPointerEnterCallback(callback);
    mSideButtons[1].SetPointerEnterCallback(callback);
    callback = FEPointerListener::Callback(
        PointerBinding(MemFun(&SHOnlineFriendsChooseSides::OnSidePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    mSideButtons[0].SetPointerLeaveCallback(callback);
    mSideButtons[1].SetPointerLeaveCallback(callback);
    callback = FEPointerListener::Callback(
        PointerBinding(MemFun(&SHOnlineFriendsChooseSides::OnSidePointerInside), this, Placeholder<0>(), Placeholder<1>()));
    mSideButtons[0].SetPointerInsideCallback(callback);
    mSideButtons[1].SetPointerInsideCallback(callback);

    FEPointerListener::Callback callback2(
        PointerBinding(MemFun(&SHOnlineFriendsChooseSides::OnSidePointerPress), this, Placeholder<0>(), Placeholder<1>()));
    mSideButtons[0].SetPointerPressCallback(callback2);
    mSideButtons[1].SetPointerPressCallback(callback2);

    callback = FEPointerListener::Callback(
        PointerBinding(MemFun(&SHOnlineFriendsChooseSides::OnDonePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerEnterCallback(callback);
    callback = FEPointerListener::Callback(
        PointerBinding(MemFun(&SHOnlineFriendsChooseSides::OnDonePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerLeaveCallback(callback);
    callback = FEPointerListener::Callback(
        PointerBinding(MemFun(&SHOnlineFriendsChooseSides::OnDonePointerInside), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerInsideCallback(callback);
    callback2 = FEPointerListener::Callback(
        PointerBinding(MemFun(&SHOnlineFriendsChooseSides::OnDonePointerPress), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerPressCallback(callback2);
    mDoneButton.Disable();
}

void SHOnlineFriendsChooseSides::OnSidePointerEnter(unsigned int index, void* context)
{
    int value = mPlayerSides[GetOnlinePlayerIndex(index)];
    if (value != -1 && context != (void*)value)
    {
        return;
    }
    if (value == -1)
    {
        int count = 0;
        GetOnlinePlayerIndex(index);
        for (int i = 0; i < mPlayerCount; ++i)
        {
            if ((int)context == mPlayerSides[i])
            {
                ++count;
            }
        }
        if (count >= mPlayerCount - 1)
        {
            return;
        }
    }
    if (!mSideButtons[(int)context].HasOtherPointerState(1, index))
    {
        mSideInstances[(int)context]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }
    mSideButtons[(int)context].SetPointerState(1, index);
    ++mPointerInsideCounts[index];
    mSideButtons[(int)context].PlayHoverFeedback(index);
    FEAudio::PlayAnimAudioEvent(0x19E7B6AE, 0, 0, 1);
}

void SHOnlineFriendsChooseSides::OnSidePointerLeave(unsigned int index, void* context)
{
    int value = mPlayerSides[GetOnlinePlayerIndex(index)];
    if (value != -1 && context != (void*)value)
    {
        return;
    }
    if (!mSideButtons[(int)context].HasOtherPointerState(1, index))
    {
        mSideInstances[(int)context]->SetActiveSlide("controllers", true, false);
    }
    mSideButtons[(int)context].SetPointerState(0, index);
    --mPointerInsideCounts[index];
}

void SHOnlineFriendsChooseSides::OnSidePointerInside(unsigned int index, void* context)
{
    int slot = GetOnlinePlayerIndex(index);
    if (mSideButtons[(int)context].GetPointerState(index) == 0)
    {
        if (mPlayerSides[slot] != -1)
        {
            return;
        }
        int count = 0;
        GetOnlinePlayerIndex(index);
        for (int i = 0; i < mPlayerCount; ++i)
        {
            if ((int)context == mPlayerSides[i])
            {
                ++count;
            }
        }
        if (count < mPlayerCount - 1)
        {
            OnSidePointerEnter(index, context);
        }
    }
    else if (mSideButtons[(int)context].GetPointerState(index) == 1 && mPlayerSides[slot] == -1)
    {
        int count = 0;
        GetOnlinePlayerIndex(index);
        for (int i = 0; i < mPlayerCount; ++i)
        {
            if ((int)context == mPlayerSides[i])
            {
                ++count;
            }
        }
        if (count >= mPlayerCount - 1)
        {
            if (!mSideButtons[(int)context].HasOtherPointerState(1, index))
            {
                mSideInstances[(int)context]->SetActiveSlide("controllers", true, false);
            }
            mSideButtons[(int)context].SetPointerState(0, index);
            --mPointerInsideCounts[index];
        }
    }
}

void SHOnlineFriendsChooseSides::OnSidePointerPress(unsigned int index, void* context)
{
    int slot = GetOnlinePlayerIndex(index);
    if (mPlayerSides[slot] != -1 && context != (void*)mPlayerSides[slot])
    {
        return;
    }
    if (mPlayerSides[slot] == -1)
    {
        int count = 0;
        GetOnlinePlayerIndex(index);
        for (int i = 0; i < mPlayerCount; ++i)
        {
            if ((int)context == mPlayerSides[i])
            {
                ++count;
            }
        }
        if (count >= mPlayerCount - 1)
        {
            return;
        }
    }
    int side = -1;
    if (mPlayerSides[slot] == -1)
    {
        side = (int)context;
    }
    FEAudio::PlayAnimAudioEvent(0xB3586309, 0, 0, 1);
    NetMessageSidesChanged message;
    message.mMachineIndex = mDraftMessage.mMachineIndex;
    message.mSide = side;
    message.mAccepted = 0;
    if (HasOnlineTwoLocalPlayers())
    {
        message.mGuest = index == gOnlineLocalControllerIndices[1];
    }
    else
    {
        message.mGuest = 0;
    }
    g_pNetworkSession->SendSidesChangedToHost(&message);
}

void SHOnlineFriendsChooseSides::OnDonePointerEnter(unsigned int index, void* context)
{
    if (mDraftMessage.mMachineIndex != 0)
    {
        return;
    }
    ++mPointerInsideCounts[index];
    mDoneButton.SetPointerState(1, index);
    mDoneButton.PlayHoverFeedback(index);
    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        mDoneButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }
}

void SHOnlineFriendsChooseSides::OnDonePointerInside(unsigned int index, void* context)
{
    if (mDoneButton.GetPointerState(index) == 0)
    {
        OnDonePointerEnter(index, context);
    }
}

void SHOnlineFriendsChooseSides::OnDonePointerLeave(unsigned int index, void* context)
{
    if (mDraftMessage.mMachineIndex != 0)
    {
        return;
    }
    --mPointerInsideCounts[index];
    mDoneButton.SetPointerState(0, index);
    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        mDoneButtonInstance->SetActiveSlide("off", true, false);
    }
}

void SHOnlineFriendsChooseSides::OnDonePointerPress(unsigned int index, void* context)
{
    if (mDraftMessage.mMachineIndex != 0)
    {
        return;
    }
    mSideButtons[0].Disable();
    mSideButtons[1].Disable();
    mDoneButton.Disable();
    for (int i = 0; i < 4; ++i)
    {
        GameInfoManager::Instance()->SetPlayingSide(i, mPlayerSides[i]);
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }
    mDraftMessage.mUnidentified0A = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (mPlayerSides[i] != -1)
        {
            int machine = mOnlinePlayers[i].mMachineIndex;
            if (!mOnlinePlayers[i].mIsGuest)
            {
                mDraftMessage.mPlayerSides.mData[machine][0] = mPlayerSides[i];
            }
            else
            {
                mDraftMessage.mPlayerSides.mData[machine][1] = mPlayerSides[i];
            }
        }
    }
    g_pNetworkSession->SendDraftToEveryone(&mDraftMessage);
    mDraftStarted = true;
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
}

void SHOnlineFriendsChooseSides::UpdateDoneButton()
{
    bool assigned = true;
    bool home = false;
    bool away = false;
    for (int i = 0; i < mPlayerCount; ++i)
    {
        if (mPlayerSides[i] == -1)
        {
            assigned = false;
            break;
        }
        if (mPlayerSides[i] == 0)
        {
            home = true;
        }
        else if (mPlayerSides[i] == 1)
        {
            away = true;
        }
    }
    mSelectSideText->m_bVisible = !assigned;
    if (mDraftMessage.mMachineIndex != 0)
    {
        mDoneButton.Disable();
        return;
    }
    bool ready = assigned && home && away;
    if (mDoneButtonInstance->m_bVisible == true)
    {
        if (!ready)
        {
            mDoneButtonAnimating = false;
            mDoneButtonInstance->m_bVisible = false;
            mDoneButton.Disable();
            for (int i = 0; i < 4; ++i)
            {
                if (mDoneButton.GetPointerState(i) == 1)
                {
                    --mPointerInsideCounts[i];
                    mDoneButton.SetPointerState(0, i);
                }
            }
        }
    }
    else if (ready == true)
    {
        FEAudio::PlayAnimAudioEvent(0x2AB04562, 0, 0, 1);
        mDoneButtonInstance->m_bVisible = true;
        mDoneButtonInstance->SetActiveSlide("in", true, false);
        mDoneButtonAnimating = true;
    }
}

#pragma dont_inline on
int SHOnlineFriendsChooseSides::GetOnlinePlayerIndex(int pad)
{
    int guest = -1;
    if (HasOnlineTwoLocalPlayers())
    {
        guest = gOnlineLocalControllerIndices[1];
    }
    for (int i = 0; i < 4; ++i)
    {
        if (mDraftMessage.mMachineIndex == mOnlinePlayers[i].mMachineIndex)
        {
            if (pad == gFEControllerIndex && !mOnlinePlayers[i].mIsGuest)
            {
                return i;
            }
            if (pad == guest && mOnlinePlayers[i].mIsGuest)
            {
                return i;
            }
        }
    }
    return -1;
}
#pragma dont_inline reset

void SHOnlineFriendsChooseSides::OnSidesChanged(NetMessageSidesChanged* message)
{
    if (message->mAccepted == 0)
    {
        if (mDraftStarted)
        {
            return;
        }
        int index = GetOnlinePlayerIndex((s8)message->mMachineIndex, message->mGuest);
        int side = (s8)message->mSide;
        if (side != -1)
        {
            int count = 0;
            GetOnlinePlayerIndex(index);
            for (int i = 0; i < mPlayerCount; ++i)
            {
                if (side == mPlayerSides[i])
                {
                    ++count;
                }
            }
            if (count >= mPlayerCount - 1)
            {
                return;
            }
        }
        NetMessageSidesChanged response(*message);
        response.mAccepted = 1;
        g_pNetworkSession->SendSidesChangedToEveryone(&response);
    }
    else
    {
        int index = GetOnlinePlayerIndex((s8)message->mMachineIndex, message->mGuest);
        int oldSide = mPlayerSides[index];
        int newSide = (s8)message->mSide;
        mPlayerSides[index] = newSide;
        DoChangeSides(newSide, oldSide, index);
        UpdateDoneButton();
        if (mDraftMessage.mMachineIndex == (s8)message->mMachineIndex)
        {
            TLComponentInstance* controller = GetPointerInstance(index);
            if (mPlayerSides[index] == -1)
            {
                controller->SetActiveSlide("holding", true, false);
            }
            else
            {
                controller->SetActiveSlide("cursor", true, false);
            }
        }
        char friendName[16];
        nlSNPrintf(friendName, sizeof(friendName), "friend_%d", index);
        FEFinder<TLTextInstance, 3>::Find(mPresentation->m_currentSlide,
            InlineHasher("Layer"), InlineHasher(friendName))->m_bVisible = mPlayerSides[index] == -1;
    }
}

void SHOnlineFriendsChooseSides::DoChangeSides(int newSide, int oldSide, int index)
{
    int slot = -1;
    char controller[16];
    tDebugPrintManager::Print(DC_NETWORK, "DoChangeSides NewSide %d OldSide %d OnlineIndex %d\n",
        newSide, oldSide, index);
    if (oldSide == newSide)
    {
        return;
    }
    if (oldSide == -1)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (mSidePlayerIndices[newSide][i] == -1)
            {
                slot = i;
                mSidePlayerIndices[newSide][i] = index;
                break;
            }
        }
        nlSNPrintf(controller, sizeof(controller), "controller%d", slot);
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find<>(
            mSideInstances[newSide], nlStringLowerHash("controllers"),
            nlStringLowerHash(gOnlineSideGroupNames[newSide]), nlStringLowerHash(controller), 0, 0, 0);
        TLComponentInstance* over = FEFinder<TLComponentInstance, 4>::Find<>(
            mSideInstances[newSide], nlStringLowerHash("over"),
            nlStringLowerHash(gOnlineSideGroupNames[newSide]), nlStringLowerHash(controller), 0, 0, 0);
        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find<>(
            component->GetActiveSlide(), InlineHasher("Text"));
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        TLTextInstance* overText = FEFinder<TLTextInstance, 3>::Find<>(
            over->GetActiveSlide(), InlineHasher("Text"));
        if (overText == 0)
        {
            overText = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mPlayerNames[index]);
        overText->SetString(mPlayerNames[index]);
        text->SetAssetColour(mPlayerColours[index]);
        overText->SetAssetColour(mPlayerColours[index]);
        component->m_bVisible = true;
        over->m_bVisible = true;
    }
    else
    {
        for (int i = 0; i < 3; ++i)
        {
            if (mSidePlayerIndices[oldSide][i] == index)
            {
                slot = i;
                mSidePlayerIndices[oldSide][i] = -1;
                break;
            }
        }
        nlSNPrintf(controller, sizeof(controller), "controller%d", slot);
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find<>(
            mSideInstances[oldSide], nlStringLowerHash("controllers"),
            nlStringLowerHash(gOnlineSideGroupNames[oldSide]), nlStringLowerHash(controller), 0, 0, 0);
        TLComponentInstance* over = FEFinder<TLComponentInstance, 4>::Find<>(
            mSideInstances[oldSide], nlStringLowerHash("over"),
            nlStringLowerHash(gOnlineSideGroupNames[oldSide]), nlStringLowerHash(controller), 0, 0, 0);
        FEFinder<TLTextInstance, 3>::Find<>(component->GetActiveSlide(), InlineHasher("Text"));
        FEFinder<TLTextInstance, 3>::Find<>(over->GetActiveSlide(), InlineHasher("Text"));
        component->m_bVisible = false;
        over->m_bVisible = false;
    }
}

void SHOnlineFriendsChooseSides::OnDisconnectPopupClosed()
{
    mDisconnectPopupActive = false;
    FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, 1);
    GameSceneManager::Instance()->Push((SceneList)40, SCREEN_BACK, true);
}
