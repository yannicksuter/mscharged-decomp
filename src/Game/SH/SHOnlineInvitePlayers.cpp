#include "NL/nlSingleton.inl"
#include "Game/SH/SHOnlineInvitePlayers.h"
#include "Game/FE/feOnlineError.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/FE/FEAudio.h"
#include "Game/Sys/debug.h"

#include "Game/GameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkLobby.h"
#include "Game/Render/Presentation.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
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
                SetPlayerRow(1, i);
            else
                SetPlayerRow(0, i);
        }
        else if (mSlots[i].mGuest)
            SetPlayerRow(6, i);
        else
            SetPlayerRow(5, i);
    }
    if (mIsHost && i < 4)
    {
        if (mDeclinedDisplayTime > 0.0f)
            SetPlayerRow(4, i);
        else if (g_pFriendManager->mOwnStatus.mStatus == EFriendStatus_HostInvitingPlayer)
            SetPlayerRow(3, i);
        else
            SetPlayerRow(2, i);
        ++i;
    }
    for (; i < 4; ++i)
        SetPlayerRow(7, i);
}

void SHOnlineInvitePlayers::InitializeButtons()
{
    FEPointerListener::Callback over(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnInvitePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnInvitePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback select(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnInvitePointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback cancelOver(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnCancelPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback cancelOff(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnCancelPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback cancelSelect(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnCancelPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback startOver(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnStartPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback startOff(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnStartPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback startSelect(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnStartPointerPress), this, Placeholder<0>(), Placeholder<1>()));
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
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
}

void SHOnlineInvitePlayers::OnInvitationErrorDismissed()
{
    mPopupActive = false;
    if (g_pNetworkSession->RequiresDisconnectAfterError())
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
        Presentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
        return;
    }
    RefreshRows();
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

    TLComponentInstance* scrollbar = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        mPresentation->m_currentSlide, nlStringLowerHash("Layer"), nlStringLowerHash("scrollbar"), 0, 0, 0, 0);
    if (scrollbar == 0)
        scrollbar = &gDefaultTLComponentInstance;
    mScrollBar.SetComponent(scrollbar);
    mScrollBar.SetRange(0);
    mScrollBar.SetValue(0);
    mBackEnabled = mIsHost && !(mDeclinedDisplayTime > 0.0f) && g_pFriendManager->mOwnStatus.mStatus != EFriendStatus_HostInvitingPlayer;
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
        mRowInstances[i] = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
            presentation->m_currentSlide, nlStringLowerHash("Layer"), nlStringLowerHash(name), 0, 0, 0, 0);
        TLInstance* over = FEFinder<TLInstance, 5>::Find(mRowInstances[i],
            nlStringLowerHash("over"), nlStringLowerHash("FRIEND_0"), 0, 0, 0, 0);
        TLInstance* off = FEFinder<TLInstance, 5>::Find(mRowInstances[i],
            nlStringLowerHash("off"), nlStringLowerHash("FRIEND_0"), 0, 0, 0, 0);
        mCancelInstances[i] = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
            over, nlStringLowerHash("cancel"), 0, 0, 0, 0, 0);
        TLInstance* image = FEFinder<TLComponentInstance, 4>::_Find(
            over, nlStringLowerHash("Mii_btn"), nlStringLowerHash("logo_32x32"), 0, 0, 0, 0);
        image->m_bVisible = false;
        image->SetAssetVisible(false);
        image = FEFinder<TLComponentInstance, 4>::_Find(
            off, nlStringLowerHash("Mii_btn"), nlStringLowerHash("logo_32x32"), 0, 0, 0, 0);
        image->m_bVisible = false;
        image->SetAssetVisible(false);
    }
    TLComponentInstance* timer = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        presentation->m_currentSlide, nlStringLowerHash("Layer"), nlStringLowerHash("timer"), 0, 0, 0, 0);
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
        UnidentifiedDraftEntry* entry = lobby->GetMachineInfo(i);
        mSlots[mSlotCount].mEntry = entry;
        ++mSlotCount;
        if (entry != 0 && entry->mUnidentified7F)
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
        if (!(slide->m_time >= slide->m_duration + slide->m_start))
            return;
        InitializeButtons();
        mInitialized = true;
        RefreshRows();
    }
    if (RefreshLobbySlots())
        RefreshRows();
    if (mBackEnabled)
    {
        if (!(mIsHost && !(mDeclinedDisplayTime > 0.0f) && g_pFriendManager->mOwnStatus.mStatus != EFriendStatus_HostInvitingPlayer))
        {
            mBackButtonInstance->m_bVisible = false;
            mBackButton.Disable();
            mBackEnabled = false;
        }
    }
    else if (mIsHost && !(mDeclinedDisplayTime > 0.0f) && g_pFriendManager->mOwnStatus.mStatus != EFriendStatus_HostInvitingPlayer)
    {
        mBackButtonInstance->m_bVisible = true;
        mBackButton.Enable();
        mBackEnabled = true;
    }
    for (unsigned int i = 0; i < 4; ++i)
    {
        TLComponentInstance* controller = gFEPointerInstances[i];
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
    if (g_pFriendManager->mOwnStatus.mStatus == EFriendStatus_HostInvitingPlayer && g_pFriendManager->HasFriendDeclined())
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
        && g_pFriendManager->mOwnStatus.mStatus == EFriendStatus_HostInvitingPlayer
        && g_pFriendManager->GetFriendInvitationResponse() == 0)
    {
        g_pFriendManager->SetOwnStatusInitial(0);
        mDeclinedFriendIndex = -1;
        mDeclinedDisplayTime = 0.0f;
        int popup = GetOnlineErrorPopup(0x13A2E, false, 0x79);
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
        {
            FEPopupMenu* menu = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
            menu->Create((ePopupMenu)popup, Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnInvitationErrorDismissed), this)));
            mPopupActive = true;
        }
        return;
    }
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    if (lobby->mMatchFailed)
    {
        lobby->CloseConnectionsAndReset();
        g_pFriendManager->SetOwnStatusInitial(0);
        int popup = GetOnlineErrorPopup(g_pNetworkSession->mDWCErrorCode, g_pNetworkSession->RequiresDisconnectAfterError(), 0x5B);
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
        {
            FEPopupMenu* menu = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
            menu->Create((ePopupMenu)popup, Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineInvitePlayers::OnLobbyErrorDismissed), this)));
            mPopupActive = true;
        }
    }
    else if (lobby->mUnidentified052)
    {
        lobby->CloseConnectionsAndReset();
        lobby->StartFriendServer();
        lobby->mUnidentified052 = false;
    }
    else if (lobby->mCancelRequested)
    {
        if (g_pFriendManager->mOwnStatus.mStatus == EFriendStatus_HostInvitingPlayer)
        {
            mDeclinedFriendIndex = g_pFriendManager->mFriendStatusIndex;
            g_pFriendManager->SetOwnStatusInitial(0);
            mDeclinedDisplayTime = 3.0f;
            RefreshRows();
        }
        lobby->mCancelRequested = false;
    }
}

void SHOnlineInvitePlayers::SetPlayerRow(int value, int index)
{
    if (value == 0)
    {
        FEOnlinePlayerRow& row = mRows[index];
        row.Reset();
        nlStrNCpy(row.mName, gNetworkMiiNameWide, 14);
        memcpy(row.mMiiData, &gNetworkMiiData, sizeof(row.mMiiData));
        row.mSearchState = 4;
        row.mStatus = 1;
        memset(&row.mStats, 0, sizeof(row.mStats));
        if (NetworkStatsManager_8012F378::Instance()->UsesEuropeanRankings())
        {
            if (NetworkStatsManager_8012F378::Instance()->GetLocalStats(2) != 0)
                row.mStats = *NetworkStatsManager_8012F378::Instance()->GetLocalStats(2);
        }
        else
        {
            NetworkLeaderboardCategory* category = NetworkStatsManager_8012F378::Instance()->GetCategory(4);
            if (category != 0)
            {
                int player = category->FindPlayer(GameInfoManager::GetInstance()->GetSaveSlot(gNetworkSaveSlotIndex)->unknown_0x01C);
                if (player != -1)
                    row.mStats = category->mMetadata[player];
            }
        }
        row.mVisible = true;
        UpdateOnlinePlayerRow(&row, mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Disable();
        mCancelButtons[index].Disable();
        return;
    }
    else if (value == 1 || value == 6)
    {
        FEOnlinePlayerRow& row = mRows[index];
        row.Reset();
        BasicString<unsigned short, Detail::TempStringAllocator> string(
            g_pLocalization->GetString("ONLINE_CONTROLLERS_GUEST"));
        nlStrNCpy(row.mName, string.c_str(), 14);
        if (value == 6)
            memcpy(row.mMiiData, mSlots[index].mEntry->mUnidentified32, sizeof(row.mMiiData));
        else
            memcpy(row.mMiiData, &gNetworkMiiData, sizeof(row.mMiiData));
        row.mSearchState = 4;
        row.mStatus = 1;
        row.mGuest = true;
        row.mVisible = true;
        UpdateOnlinePlayerRow(&row, mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Disable();
        mCancelButtons[index].Disable();
        return;
    }
    else if (value == 5)
    {
        FEOnlinePlayerRow& row = mRows[index];
        row.Reset();
        UnidentifiedDraftEntry* entry = mSlots[index].mEntry;
        nlStrNCpy(row.mName, entry->mName, 14);
        memcpy(row.mMiiData, entry->mUnidentified32, sizeof(row.mMiiData));
        row.mSearchState = 4;
        row.mStatus = 1;
        row.mStats = entry->mHead;
        NetworkLeaderboardCategory* category = NetworkStatsManager_8012F378::Instance()->GetCategory(4);
        if (category != 0)
        {
            int player = category->FindPlayer(entry->mUnidentified18);
            if (player != -1)
                row.mStats.mDisplayRank = category->mMetadata[player].mDisplayRank;
        }
        row.mVisible = true;
        UpdateOnlinePlayerRow(&row, mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Disable();
        mCancelButtons[index].Disable();
        return;
    }
    else if (value == 2)
    {
        FEOnlinePlayerRow& row = mRows[index];
        row.Reset();
        row.mSearchState = 2;
        row.mVisible = true;
        UpdateOnlinePlayerRow(&row, mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Enable();
        mCancelButtons[index].Disable();
        HidePlayerPortrait(index);
        return;
    }
    else if (value == 3)
    {
        FEOnlinePlayerRow& row = mRows[index];
        row.Reset();
        int friendIndex = g_pFriendManager->GetFriendStatusIndex();
        nlStrNCpy(row.mName,
            GameInfoManager::GetInstance()->GetSavedFriendName(gNetworkSaveSlotIndex, friendIndex), 14);
        row.mSearchState = 4;
        row.mStatus = 10;
        row.mStats.Reset();
        row.mSide = 0;
        row.mVisible = true;
        row.mGuest = false;
        row.mShowCancel = true;
        UpdateOnlinePlayerRow(&row, mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Disable();
        mCancelButtons[index].Enable();
        HidePlayerPortrait(index);
        return;
    }
    else if (value == 4)
    {
        FEOnlinePlayerRow& row = mRows[index];
        row.Reset();
        nlStrNCpy(row.mName,
            GameInfoManager::GetInstance()->GetSavedFriendName(gNetworkSaveSlotIndex, mDeclinedFriendIndex), 14);
        row.mSearchState = 4;
        row.mStatus = 9;
        row.mStats.Reset();
        row.mSide = 0;
        row.mVisible = true;
        row.mGuest = false;
        row.mShowCancel = true;
        UpdateOnlinePlayerRow(&row, mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Disable();
        mCancelButtons[index].Enable();
        HidePlayerPortrait(index);
        return;
    }
    else if (value == 7)
    {
        FEOnlinePlayerRow& row = mRows[index];
        row.Reset();
        row.mVisible = false;
        UpdateOnlinePlayerRow(&row, mRowInstances[index], mRankText[index], 0x20,
            mRecordText[index], 0x30, index, mInitialized);
        mInviteButtons[index].Disable();
        mCancelButtons[index].Disable();
        HidePlayerPortrait(index);
        return;
    }
}

void SHOnlineInvitePlayers::HidePlayerPortrait(int index)
{
    TLInstance* over = FEFinder<TLInstance, 5>::Find(mRowInstances[index],
        nlStringLowerHash("over"), nlStringLowerHash("FRIEND_0"), 0, 0, 0, 0);
    TLInstance* off = FEFinder<TLInstance, 5>::Find(mRowInstances[index],
        nlStringLowerHash("off"), nlStringLowerHash("FRIEND_0"), 0, 0, 0, 0);
    TLInstance* image = FEFinder<TLComponentInstance, 4>::_Find(
        over, nlStringLowerHash("Mii_btn"), nlStringLowerHash("Mii"), 0, 0, 0, 0);
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::_Find(
        off, nlStringLowerHash("Mii_btn"), nlStringLowerHash("Mii"), 0, 0, 0, 0);
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::_Find(
        over, nlStringLowerHash("Mii_btn"), nlStringLowerHash("logo_32x32"), 0, 0, 0, 0);
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::_Find(
        off, nlStringLowerHash("Mii_btn"), nlStringLowerHash("logo_32x32"), 0, 0, 0, 0);
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::_Find(
        over, nlStringLowerHash("Mii_btn"), nlStringLowerHash("shoulders"), 0, 0, 0, 0);
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::_Find(
        off, nlStringLowerHash("Mii_btn"), nlStringLowerHash("shoulders"), 0, 0, 0, 0);
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::_Find(
        over, nlStringLowerHash("Mii_btn"), nlStringLowerHash("Online_Mii_select_background"), 0, 0, 0, 0);
    image->m_bVisible = false;
    image->SetAssetVisible(false);
    image = FEFinder<TLComponentInstance, 4>::_Find(
        off, nlStringLowerHash("Mii_btn"), nlStringLowerHash("Online_Mii_select_background"), 0, 0, 0, 0);
    image->m_bVisible = false;
    image->SetAssetVisible(false);
}

void SHOnlineInvitePlayers::OnLobbyErrorDismissed()
{
    mPopupActive = false;
    if (g_pNetworkSession->RequiresDisconnectAfterError())
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
        Presentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
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
