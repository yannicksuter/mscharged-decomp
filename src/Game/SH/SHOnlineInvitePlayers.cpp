#include "NL/nlSingleton.inl"
#include "Game/SH/SHOnlineInvitePlayers.h"
#include "Game/FE/feOnlineError.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/FEAudio.h"
#include "Game/Sys/debug.h"

#include "Game/GameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePointer.inl"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkSession.h"
#include "Game/Task/NetworkUpdateTask.h"
#include "Game/NetworkLobby.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlstring_tmpl.h"

SHOnlineInvitePlayers::SHOnlineInvitePlayers()
    : mIsHost(true)
    , mStartFriendServer(false)
    , mPopupActive(false)
    , mInitialized(false)
    , mDeclinedDisplayTime(0.0f)
    , mDeclinedFriendIndex(-1)
    , mInvitationTimeout(0.0f)
    , mStartingMatch(false)
    , mStartHovered(false)
    , mBackEnabled(false)
    , mSlotCount(0)
    , mMachineCount(0)
{
    mHoverCounts[0] = 0;
    mHoverCounts[1] = 0;
    mHoverCounts[2] = 0;
    mHoverCounts[3] = 0;
    for (int i = 0; i < 4; ++i)
    {
        mInviteButtons[i].mContext = (void*)i;
        mCancelButtons[i].mContext = (void*)i;
    }
}

SHOnlineInvitePlayers::~SHOnlineInvitePlayers()
{
}

void SHOnlineInvitePlayers::RefreshRows()
{
    int i;
    for (i = 0; i < mSlotCount; ++i)
    {
        if (mSlots[i].mLocal)
        {
            if (mSlots[i].mGuest)
                SetPlayerRow(RowLocalGuest, i);
            else
                SetPlayerRow(RowLocalPlayer, i);
        }
        else if (mSlots[i].mGuest)
            SetPlayerRow(RowRemoteGuest, i);
        else
            SetPlayerRow(RowRemotePlayer, i);
    }
    if (mIsHost && i < 4)
    {
        if (mDeclinedDisplayTime > 0.0f)
            SetPlayerRow(RowInvitationDeclined, i);
        else if (g_pFriendManager->mOwnStatus.mHeader.mStatus == EFriendStatus_HostInvitingPlayer)
            SetPlayerRow(RowInvitationPending, i);
        else
            SetPlayerRow(RowInviteAvailable, i);
        ++i;
    }
    for (; i < 4; ++i)
        SetPlayerRow(RowHidden, i);
}

void SHOnlineInvitePlayers::SceneCreated()
{
    mSlotCount = 0;
    if (mIsHost)
    {
        if (mStartFriendServer)
            g_pNetworkSession->GetOnlineLobby()->StartFriendServer();
        else
            mInvitationTimeout = 30.0f;
        mSlotCount = 1;
        mSlots[0].mLocal = true;
        mSlots[0].mGuest = false;
        mSlots[0].mEntry = 0;
        if (HasOnlineTwoLocalPlayers())
        {
            mSlots[mSlotCount].mLocal = true;
            mSlots[mSlotCount].mGuest = true;
            mSlots[mSlotCount].mEntry = 0;
            ++mSlotCount;
        }
        mBackButton.SetBackScene(5);
    }

    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::FindOrDefault(
        mPresentation->m_currentSlide, "Layer", "scrollbar");
    mScrollBar.SetComponent(scrollbar);
    mScrollBar.SetRange(0);
    mScrollBar.SetValue(0);
    bool backAvailable;
    if (!mIsHost)
        backAvailable = false;
    else if (mDeclinedDisplayTime > 0.0f)
        backAvailable = false;
    else
        backAvailable = g_pFriendManager->mOwnStatus.mHeader.mStatus != EFriendStatus_HostInvitingPlayer;
    if (backAvailable)
        mBackEnabled = true;
    else
        mBackEnabled = false;
    SHNavigation* screen = GetNavigationScene();
    screen->SetButtons(0x24, true);
    mBackButtonInstance = screen->GetButton(4);
    mStartButtonInstance = screen->GetButton(0x20);
    mBackButton.SetButtonInstance(mBackButtonInstance);
    if (mBackEnabled)
    {
        mBackButtonInstance->m_bVisible = true;
        mBackButton.Enable();
    }
    else
    {
        screen->SetButtonVisibility(4, false);
        mBackButton.Disable();
    }
    SetDoneButtonBounds(&mStartButton, mStartButtonInstance, 0);
    screen->SetButtonVisibility(0x20, false);
    mStartButton.Disable();

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    for (int i = 0; i < 4; ++i)
    {
        char name[0x10];
        nlSNPrintf(name, sizeof(name), "FRIEND_%d", i);
        mRowInstances[i] = FEFinder<TLComponentInstance, 4>::FindChecked(presentation->m_currentSlide, "Layer", name);
        TLInstance* over = FEFinder<TLInstance, 5>::Find(mRowInstances[i], "over", "FRIEND_0");
        TLInstance* off = FEFinder<TLInstance, 5>::Find(mRowInstances[i], "off", "FRIEND_0");
        mCancelInstances[i] = FEFinder<TLComponentInstance, 4>::Find(over, "cancel");
        TLInstance* image = FEFinder<TLComponentInstance, 4>::Find(over, "Mii_btn", "logo_32x32");
        image->m_bVisible = false;
        image->SetAssetVisible(false);
        image = FEFinder<TLComponentInstance, 4>::Find(off, "Mii_btn", "logo_32x32");
        image->m_bVisible = false;
        image->SetAssetVisible(false);
    }
    TLComponentInstance* timer = FEFinder<TLComponentInstance, 4>::FindChecked(presentation->m_currentSlide, "Layer", "timer");
    timer->m_bVisible = false;
    RefreshLobbySlots();
    RefreshRows();
    FEAudio::PlayAnimAudioEvent(0xD5048410, 0, 0, 1);
    FEMusic::StartStreamIfDifferent(8);
}

bool SHOnlineInvitePlayers::RefreshLobbySlots()
{
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    if (lobby->GetMachineCount() == 0)
        return false;
    if (!lobby->AllMachineInfoReceived())
        return false;
    if (mMachineCount == lobby->GetMachineCount())
        return false;

    tDebugPrintManager::Print(DC_NETWORK,
        "SHOnlineInvitePlayers lobby numPeers changed from %d to %d\n",
        mMachineCount, lobby->GetMachineCount());
    mMachineCount = lobby->GetMachineCount();
    mSlotCount = 0;
    for (int i = 0; i < lobby->GetMachineCount(); ++i)
    {
        mSlots[mSlotCount].mLocal = i == lobby->GetLocalMachineIndex();
        mSlots[mSlotCount].mGuest = false;
        NetworkDraftMachineInfo* entry = lobby->GetMachineInfo(i);
        mSlots[mSlotCount].mEntry = entry;
        ++mSlotCount;
        if (entry != 0 && entry->mGuestEnabled)
        {
            mSlots[mSlotCount].mLocal = i == lobby->GetLocalMachineIndex();
            mSlots[mSlotCount].mGuest = true;
            mSlots[mSlotCount].mEntry = entry;
            ++mSlotCount;
        }
    }
    if (lobby->GetMachineCount() >= 2 && mIsHost && gOnlineFourMachineFriendLobby)
    {
        mStartButton.Enable();
        mStartButtonInstance->m_bVisible = true;
        mBackButton.Disable();
        mBackButtonInstance->m_bVisible = false;
    }
    return true;
}

void SHOnlineInvitePlayers::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mPopupActive && !g_pFEInput->HasInputLock(this))
        return;
    if (!mInitialized)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
        {
            InitializeButtons();
            mInitialized = true;
            RefreshRows();
        }
        else
        {
            return;
        }
    }
    if (RefreshLobbySlots())
        RefreshRows();
    if (mBackEnabled)
    {
        bool backAvailable;
        if (!mIsHost)
            backAvailable = false;
        else if (mDeclinedDisplayTime > 0.0f)
            backAvailable = false;
        else
            backAvailable = g_pFriendManager->GetOwnStatus()->mHeader.mStatus != EFriendStatus_HostInvitingPlayer;
        if (!backAvailable)
        {
            mBackButtonInstance->m_bVisible = false;
            mBackButton.Disable();
            mBackEnabled = false;
        }
    }
    else
    {
        bool backAvailable;
        if (!mIsHost)
            backAvailable = false;
        else if (mDeclinedDisplayTime > 0.0f)
            backAvailable = false;
        else
            backAvailable = g_pFriendManager->GetOwnStatus()->mHeader.mStatus != EFriendStatus_HostInvitingPlayer;
        if (backAvailable)
        {
            mBackButtonInstance->m_bVisible = true;
            mBackButton.Enable();
            mBackEnabled = true;
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* controller = GetPointerInstance(i);
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (i != gFEControllerIndex)
            {
                controller->SetActiveSlide("waiting", true, false);
                continue;
            }
            if (mHoverCounts[i] > 0 || mBackButton.mPointerInside[i] || mStartHovered)
                controller->SetActiveSlide("A", true, false);
            else
                controller->SetActiveSlide("cursor", true, false);
        }
        unsigned char valid = 1;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);
        mStartButton.HandlePointerEvent(&event);
        if (!mStartingMatch)
        {
            if (mBackButton.UpdateBackButton(event, fDeltaT))
            {
                FEAudio::PlayAnimAudioEvent(0x64B85E8D, 0, 0, 1);
                NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
                if (lobby != 0)
                    lobby->StopFriendServer();
                g_pFriendManager->SetOwnStatusInitial(1);
                return;
            }
            if (mIsHost)
            {
                for (int j = 0; j < 4; ++j)
                {
                    mInviteButtons[j].HandlePointerEvent(&event);
                    mCancelButtons[j].HandlePointerEvent(&event);
                }
            }
        }
    }
    if (g_pFriendManager->GetOwnStatus()->mHeader.mStatus == EFriendStatus_HostInvitingPlayer && g_pFriendManager->HasFriendDeclined())
    {
        mDeclinedFriendIndex = g_pFriendManager->mFriendStatusIndex;
        g_pFriendManager->SetOwnStatusInitial(0);
        mDeclinedDisplayTime = 3.0f;
        RefreshRows();
    }
    if (mDeclinedDisplayTime > 0.0f)
    {
        mDeclinedDisplayTime -= fDeltaT;
        if (mDeclinedDisplayTime <= 0.0f)
        {
            mDeclinedDisplayTime = 0.0f;
            mDeclinedFriendIndex = -1;
            RefreshRows();
        }
    }
    else if (mInvitationTimeout > 0.0f && (mInvitationTimeout -= fDeltaT) <= 0.0f
        && g_pFriendManager->GetOwnStatus()->mHeader.mStatus == EFriendStatus_HostInvitingPlayer
        && g_pFriendManager->GetFriendInvitationResponse() == 0)
    {
        g_pFriendManager->SetOwnStatusInitial(0);
        mDeclinedFriendIndex = -1;
        mDeclinedDisplayTime = 0.0f;
        ShowInvitationError(GetOnlineErrorPopup(0x13A2E, false, 0x79));
        return;
    }
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    if (lobby->mMatchFailed)
    {
        lobby->CloseConnectionsAndReset();
        g_pFriendManager->SetOwnStatusInitial(0);
        int errorCode = g_pNetworkSession->GetDWCErrorCode();
        ShowLobbyError(GetOnlineErrorPopup(errorCode, g_pNetworkSession->RequiresDisconnectAfterError(), 0x5B));
    }
    else if (lobby->mLateCancelRequested)
    {
        lobby->CloseConnectionsAndReset();
        lobby->StartFriendServer();
        lobby->mLateCancelRequested = false;
    }
    else if (lobby->mCancelRequested)
    {
        if (g_pFriendManager->GetOwnStatus()->mHeader.mStatus == EFriendStatus_HostInvitingPlayer)
        {
            mDeclinedFriendIndex = g_pFriendManager->mFriendStatusIndex;
            g_pFriendManager->SetOwnStatusInitial(0);
            mDeclinedDisplayTime = 3.0f;
            RefreshRows();
        }
        lobby->mCancelRequested = false;
    }
}

void SHOnlineInvitePlayers::SetPlayerRow(int state, int index)
{
    if (state == RowLocalPlayer)
    {
        mRows[index].Reset();
        nlStrNCpy(mRows[index].mName, gNetworkMiiNameWide, 14);
        memcpy(mRows[index].mMiiData, &gNetworkMiiData, sizeof(mRows[index].mMiiData));
        mRows[index].mSearchState = 4;
        mRows[index].mStatus = 1;
        memset(&mRows[index].mStats, 0, sizeof(mRows[index].mStats));
        if (NetworkStatsManager::Instance()->UsesEuropeanRankings())
        {
            if (NetworkStatsManager::Instance()->GetLocalStats(2) != 0)
                mRows[index].mStats = *NetworkStatsManager::Instance()->GetLocalStats(2);
        }
        else
        {
            NetworkLeaderboardCategory* category = NetworkStatsManager::Instance()->GetCategory(4);
            if (category != 0)
            {
                int profileId = GameInfoManager::Instance()->GetSaveSlot(gNetworkSaveSlotIndex)->unknown_0x01C;
                int player = category->FindPlayer(profileId);
                if (player != -1)
                    mRows[index].mStats = category->mMetadata[player];
            }
        }
        mRows[index].mVisible = true;
        UpdateOnlinePlayerRow(&mRows[index], mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Disable();
        mCancelButtons[index].Disable();
        return;
    }
    else if (state == RowLocalGuest || state == RowRemoteGuest)
    {
        mRows[index].Reset();
        BasicString<unsigned short, Detail::TempStringAllocator> string(
            g_pLocalization->GetString("ONLINE_CONTROLLERS_GUEST"));
        nlStrNCpy(mRows[index].mName, string.c_str(), 14);
        if (state == RowRemoteGuest)
            memcpy(mRows[index].mMiiData, mSlots[index].mEntry->mMiiData, sizeof(mRows[index].mMiiData));
        else
            memcpy(mRows[index].mMiiData, &gNetworkMiiData, sizeof(mRows[index].mMiiData));
        mRows[index].mSearchState = 4;
        mRows[index].mStatus = 1;
        mRows[index].mGuest = true;
        mRows[index].mVisible = true;
        UpdateOnlinePlayerRow(&mRows[index], mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Disable();
        mCancelButtons[index].Disable();
        return;
    }
    else if (state == RowRemotePlayer)
    {
        mRows[index].Reset();
        NetworkDraftMachineInfo* entry = mSlots[index].mEntry;
        nlStrNCpy(mRows[index].mName, entry->mName, 14);
        memcpy(mRows[index].mMiiData, entry->mMiiData, sizeof(mRows[index].mMiiData));
        mRows[index].mSearchState = 4;
        mRows[index].mStatus = 1;
        mRows[index].mStats = entry->mStats;
        NetworkLeaderboardCategory* category = NetworkStatsManager::Instance()->GetCategory(4);
        if (category != 0)
        {
            int player = category->FindPlayer(entry->mProfileId);
            if (player != -1)
                mRows[index].mStats.mDisplayRank = category->mMetadata[player].mDisplayRank;
        }
        mRows[index].mVisible = true;
        UpdateOnlinePlayerRow(&mRows[index], mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Disable();
        mCancelButtons[index].Disable();
        return;
    }
    else if (state == RowInviteAvailable)
    {
        mRows[index].Reset();
        mRows[index].mSearchState = 2;
        mRows[index].mVisible = true;
        UpdateOnlinePlayerRow(&mRows[index], mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Enable();
        mCancelButtons[index].Disable();
        HidePlayerPortrait(index);
        return;
    }
    else if (state == RowInvitationPending)
    {
        mRows[index].Reset();
        GetFriendManager()->GetOwnStatus();
        int friendIndex = GetFriendManager()->GetFriendStatusIndex();
        nlStrNCpy(mRows[index].mName,
            GameInfoManager::Instance()->GetSavedFriendName(gNetworkSaveSlotIndex, friendIndex), 14);
        mRows[index].mSearchState = 4;
        mRows[index].mStatus = 10;
        mRows[index].mStats.Reset();
        mRows[index].mSide = 0;
        mRows[index].mVisible = true;
        mRows[index].mGuest = false;
        mRows[index].mShowCancel = true;
        UpdateOnlinePlayerRow(&mRows[index], mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Disable();
        mCancelButtons[index].Enable();
        HidePlayerPortrait(index);
        return;
    }
    else if (state == RowInvitationDeclined)
    {
        mRows[index].Reset();
        nlStrNCpy(mRows[index].mName,
            GameInfoManager::Instance()->GetSavedFriendName(gNetworkSaveSlotIndex, mDeclinedFriendIndex), 14);
        mRows[index].mSearchState = 4;
        mRows[index].mStatus = 9;
        mRows[index].mStats.Reset();
        mRows[index].mSide = 0;
        mRows[index].mVisible = true;
        mRows[index].mGuest = false;
        mRows[index].mShowCancel = true;
        UpdateOnlinePlayerRow(&mRows[index], mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Disable();
        mCancelButtons[index].Enable();
        HidePlayerPortrait(index);
        return;
    }
    else if (state == RowHidden)
    {
        mRows[index].Reset();
        mRows[index].mVisible = false;
        UpdateOnlinePlayerRow(&mRows[index], mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Disable();
        mCancelButtons[index].Disable();
        HidePlayerPortrait(index);
        return;
    }
}

void SHOnlineInvitePlayers::HidePlayerPortrait(int index)
{
    TLInstance* over = FEFinder<TLInstance, 5>::Find(mRowInstances[index], "over", "FRIEND_0");
    TLInstance* off = FEFinder<TLInstance, 5>::Find(mRowInstances[index], "off", "FRIEND_0");
    TLInstance* image = FEFinder<TLComponentInstance, 4>::Find(over, "Mii_btn", "Mii");
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::Find(off, "Mii_btn", "Mii");
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::Find(over, "Mii_btn", "logo_32x32");
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::Find(off, "Mii_btn", "logo_32x32");
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::Find(over, "Mii_btn", "shoulders");
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::Find(off, "Mii_btn", "shoulders");
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::Find(over, "Mii_btn", "Online_Mii_select_background");
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::Find(off, "Mii_btn", "Online_Mii_select_background");
    image->m_bVisible = false;
    image->SetAssetVisible(false);
}

void SHOnlineInvitePlayers::InitializeButtons()
{
    typedef Detail::MemFunImpl<void, void (SHOnlineInvitePlayers::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHOnlineInvitePlayers*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback over(PointerBinding(MemFun(&SHOnlineInvitePlayers::OnInvitePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(PointerBinding(MemFun(&SHOnlineInvitePlayers::OnInvitePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback select(PointerBinding(MemFun(&SHOnlineInvitePlayers::OnInvitePointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback cancelOver(PointerBinding(MemFun(&SHOnlineInvitePlayers::OnCancelPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback cancelOff(PointerBinding(MemFun(&SHOnlineInvitePlayers::OnCancelPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback cancelSelect(PointerBinding(MemFun(&SHOnlineInvitePlayers::OnCancelPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback startOver(PointerBinding(MemFun(&SHOnlineInvitePlayers::OnStartPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback startOff(PointerBinding(MemFun(&SHOnlineInvitePlayers::OnStartPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback startSelect(PointerBinding(MemFun(&SHOnlineInvitePlayers::OnStartPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    mStartButton.SetPointerEnterCallback(startOver);
    mStartButton.SetPointerLeaveCallback(startOff);
    mStartButton.SetPointerPressCallback(startSelect);
    for (int i = 0; i < 4; ++i)
    {
        feVector3 position = mRowInstances[i]->GetAssetPosition();
        mInviteButtons[i].SetInstanceBounds(mRowInstances[i], true, -24.0f, 10.0f, 0.7f, 0.55f);
        mInviteButtons[i].SetPointerEnterCallback(over);
        mInviteButtons[i].SetPointerLeaveCallback(off);
        mInviteButtons[i].SetPointerPressCallback(select);
        mCancelButtons[i].SetInstanceBounds(mCancelInstances[i], true, position.f.x, position.f.y, 1.0f, 1.4f);
        mCancelButtons[i].SetPointerEnterCallback(cancelOver);
        mCancelButtons[i].SetPointerLeaveCallback(cancelOff);
        mCancelButtons[i].SetPointerPressCallback(cancelSelect);
    }
}

inline void SHOnlineInvitePlayers::ShowInvitationError(int popup)
{
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
    {
        FEPopupMenu* menu = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
        menu->Create((ePopupMenu)popup, Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnInvitationErrorDismissed), this)));
        mPopupActive = true;
    }
}

inline void SHOnlineInvitePlayers::ShowLobbyError(int popup)
{
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
    {
        FEPopupMenu* menu = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
        menu->Create((ePopupMenu)popup, Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnLobbyErrorDismissed), this)));
        mPopupActive = true;
    }
}

void SHOnlineInvitePlayers::OnInvitePointerEnter(int index, void* context)
{
    FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
    ++mHoverCounts[(int)context];
    mRowInstances[(int)context]->SetActiveSlide("over", true, false);
    mInviteButtons[(int)context].SetPointerState(1, index);
}

void SHOnlineInvitePlayers::OnInvitePointerLeave(int index, void* context)
{
    --mHoverCounts[(int)context];
    mRowInstances[(int)context]->SetActiveSlide("off", true, false);
    mInviteButtons[(int)context].SetPointerState(0, index);
}

void SHOnlineInvitePlayers::OnInvitePointerPress(int index, void* context)
{
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    SetOnlineFriendSelectionMode(1);
    SetOnlineFriendSelectionContext(context);
    GameSceneManager::Instance()->Push((SceneList)0x2F, SCREEN_FORWARD, true);
}

void SHOnlineInvitePlayers::OnCancelPointerEnter(int index, void* context)
{
    FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
    ++mHoverCounts[(int)context];
    mCancelInstances[(int)context]->SetActiveSlide("over", true, false);
    mCancelButtons[(int)context].SetPointerState(1, index);
}

void SHOnlineInvitePlayers::OnCancelPointerLeave(int index, void* context)
{
    --mHoverCounts[(int)context];
    mCancelInstances[(int)context]->SetActiveSlide("off", true, false);
    mCancelButtons[(int)context].SetPointerState(0, index);
}

void SHOnlineInvitePlayers::OnCancelPointerPress(int, void*)
{
    FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, 1);
    g_pFriendManager->SetOwnStatusInitial(0);
    mDeclinedFriendIndex = -1;
    mDeclinedDisplayTime = 0.0f;
    RefreshRows();
}

void SHOnlineInvitePlayers::OnStartPointerEnter(int index, void*)
{
    mStartButtonInstance->SetActiveSlide("over", true, false);
    mStartButton.SetPointerState(1, index);
    FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
    mStartHovered = true;
}

void SHOnlineInvitePlayers::OnStartPointerLeave(int index, void*)
{
    mStartButtonInstance->SetActiveSlide("off", true, false);
    mStartButton.SetPointerState(0, index);
    mStartHovered = false;
}

void SHOnlineInvitePlayers::OnStartPointerPress(int, void*)
{
    mBackButton.Disable();
    mStartButton.Disable();
    mStartingMatch = true;
    g_pNetworkSession->GetOnlineLobby()->SetFriendHostInviting();
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
}

void SHOnlineInvitePlayers::OnLobbyErrorDismissed()
{
    mPopupActive = false;
    if (g_pNetworkSession->RequiresDisconnectAfterError())
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
        FrontEndPresentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
        return;
    }
    if (mIsHost)
    {
        GameSceneManager::Instance()->Push((SceneList)5, SCREEN_BACK, true);
        return;
    }
    g_pFriendManager->SetOwnStatusAvailable();
    GameSceneManager::Instance()->Push((SceneList)g_pFriendManager->mReturnScene, SCREEN_BACK, true);
}

void SHOnlineInvitePlayers::OnInvitationErrorDismissed()
{
    mPopupActive = false;
    if (g_pNetworkSession->RequiresDisconnectAfterError())
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
        FrontEndPresentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
        return;
    }
    RefreshRows();
}
