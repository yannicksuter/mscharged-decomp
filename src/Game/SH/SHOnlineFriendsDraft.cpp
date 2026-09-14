#include "Game/SH/SHOnlineFriendsDraft.h"
#include "NL/nlBasicString.inl"

#include "Game/GameSceneManager.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/UnidentifiedTLDefault.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkLobby.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkStatsManager.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlBasicString.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

SHOnlineFriendsDraft::SHOnlineFriendsDraft()
    : mIntroComplete(false)
    , mCountdownSeconds(-1)
    , mErrorPopupOpen(false)
{
    NetMessageDraft& draft = NetworkDraft::Instance()->GetDraftMessage();
    mPlayerCount = 0;
    for (int i = 0; i < draft.mMachineCount; ++i)
    {
        mDraftPlayers[mPlayerCount].mMachineInfo = &draft.mEntries[i];
        mDraftPlayers[mPlayerCount].mIsGuest = false;
        mDraftPlayers[mPlayerCount].mTeamIndex = draft.mPlayerSides.mData[i][0];
        ++mPlayerCount;
        if (draft.mEntries[i].mGuestEnabled)
        {
            mDraftPlayers[mPlayerCount].mMachineInfo = &draft.mEntries[i];
            mDraftPlayers[mPlayerCount].mIsGuest = true;
            mDraftPlayers[mPlayerCount].mTeamIndex = draft.mPlayerSides.mData[i][1];
            ++mPlayerCount;
        }
    }
    InitializePlayerRows();
    UpdateDraftStatuses();
}

void SHOnlineFriendsDraft::UpdateDraftStatuses()
{
    NetworkDraft::Instance();
    int draftingTeam = NetworkDraft::Instance()->GetCurrentDraftingTeam();
    for (int i = 0; i < mPlayerCount; ++i)
    {
        FEOnlinePlayerRow& row = mPlayerRows[i];
        int teamIndex = mDraftPlayers[i].mTeamIndex;
        NetworkDraftTeam* team = NetworkDraft::Instance()->GetDraftTeam(teamIndex);
        NetworkDraftPlayer* player = team->FindPlayer(mDraftPlayers[i].mMachineInfo->mMachineIndex, mDraftPlayers[i].mIsGuest);
        if (player->mDisconnected)
            row.mStatus = 8;
        else if (teamIndex > draftingTeam)
            row.mStatus = 1;
        else if (teamIndex == draftingTeam)
        {
            NetworkDraft* draft = NetworkDraft::Instance();
            int draftingPeer = draft->mCurrentDraftingPeer;
            bool draftingGuest = draft->mCurrentDrafterIsGuest;
            if (draftingPeer == mDraftPlayers[i].mMachineInfo->mMachineIndex
                && draftingGuest == mDraftPlayers[i].mIsGuest)
                row.mStatus = 0;
            else
                row.mStatus = 4;
        }
        else if (team->mCaptain != -1)
        {
            row.mStatus = 6;
            row.mCaptain = team->mCaptain;
        }
        else
            row.mStatus = 11;
    }
}

SHOnlineFriendsDraft::~SHOnlineFriendsDraft()
{
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
        scene->mTimer->m_bVisible = false;
}

void SHOnlineFriendsDraft::InitializePlayerRows()
{
    int i;
    for (i = 0; i < mPlayerCount; ++i)
    {
        FEOnlinePlayerRow& row = mPlayerRows[i];
        row.Reset();
        if (!mDraftPlayers[i].mIsGuest)
            nlStrNCpy(row.mName, mDraftPlayers[i].mMachineInfo->mName, 14);
        else
        {
            BasicString<unsigned short, Detail::TempStringAllocator> string(LookupLocString("ONLINE_CONTROLLERS_GUEST"));
            nlStrNCpy(row.mName, string.c_str(), 14);
        }
        memcpy(row.mMiiData, mDraftPlayers[i].mMachineInfo->mMiiData, sizeof(row.mMiiData));
        row.mSearchState = 4;
        row.mStatus = 1;
        row.mStats = mDraftPlayers[i].mMachineInfo->mStats;
        NetworkLeaderboardCategory* category = NetworkStatsManager::Instance()->GetCategory(4);
        if (category != 0)
        {
            int player = category->FindPlayer(mDraftPlayers[i].mMachineInfo->mProfileId);
            if (player != -1)
                row.mStats.mDisplayRank = category->mMetadata[player].mDisplayRank;
        }
        int side = 2;
        if (mDraftPlayers[i].mTeamIndex == 0)
            side = 1;
        row.mSide = side;
        row.mVisible = true;
        row.mGuest = mDraftPlayers[i].mIsGuest;
    }
    for (; i < 4; ++i)
    {
        mPlayerRows[i].Reset();
        mPlayerRows[i].mVisible = false;
    }
}

void SHOnlineFriendsDraft::SceneCreated()
{
    for (int i = 0; i < 4; ++i)
    {
        char buffer[32];
        nlSNPrintf(buffer, sizeof(buffer), "FRIEND_%d", i);
        TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::Find<>(
            mPresentation->GetActiveSlide(), "Layer", buffer);
        if (instance == 0)
            instance = &UnidentifiedTLComponentDefault::sInstance;
        mPlayerRowInstances[i] = instance;
    }
    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::Find<>(
        mPresentation->GetActiveSlide(), "Layer", "scrollbar");
    mScrollBar.SetComponent(scrollbar == 0 ? &UnidentifiedTLComponentDefault::sInstance : scrollbar);
    mScrollBar.SetRange(0);
    mScrollBar.SetValue(0);
    GetNavigationScene()->SetButtons(0, true);
    RefreshPlayerRows();
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);

    TLComponentInstance* title = FEFinder<TLComponentInstance, 4>::Find<>(
        mPresentation->GetActiveSlide(), "Layer", "Title2");
    TLTextInstance* title1 = FEFinder<TLTextInstance, TLAT_TEXT>::Find<>(title->GetActiveSlide(), "Title");
    TLTextInstance* title2 = FEFinder<TLTextInstance, TLAT_TEXT>::Find<>(title->GetActiveSlide(), "Title2");
    TLTextInstance* title3 = FEFinder<TLTextInstance, TLAT_TEXT>::Find<>(title->GetActiveSlide(), "Title3");
    title1->SetStringId("TITLE_LW_DOMINATION_DRAFT");
    title2->SetStringId("TITLE_LW_DOMINATION_DRAFT");
    title3->SetStringId("TITLE_LW_DOMINATION_DRAFT");

    int countdown = NetworkDraft::Instance()->GetCountdown();
    UpdateCountdown(countdown);
}

void SHOnlineFriendsDraft::RefreshPlayerRows()
{
    for (int i = 0; i < 4; ++i)
        UpdateOnlinePlayerRow(&mPlayerRows[i], mPlayerRowInstances[i], mRankText[i], 0x20,
            mRecordText[i], 0x30, i, mIntroComplete);
}

void SHOnlineFriendsDraft::UpdateCountdown(int countdown)
{
    mCountdownSeconds = countdown;
    TLSlide* timerSlide = mPresentation->GetActiveSlide();
    FEFinder<TLTextInstance, TLAT_TEXT>::Find<TLSlide>(timerSlide, "Layer", "Timer")->m_bVisible = false;
    TLTextInstance* text = static_cast<TLTextInstance*>(GetNavigationScene()->mTimer);
    if (countdown == -1)
        text->m_bVisible = false;
    else
    {
        text->m_bVisible = true;
        char buffer[8];
        nlSNPrintf(buffer, sizeof(buffer), "%d", countdown);
        nlStrToWcs(buffer, mCountdownText, 8);
        text->SetString(mCountdownText);
    }
}

inline void SHOnlineFriendsDraft::ShowDisconnectedError()
{
    g_pNetworkSession->GetOnlineLobby()->CloseConnectionsAndReset();
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) == (SceneList)10)
        return;
    FEPopupMenu* menu = static_cast<FEPopupMenu*>(
        GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false));
    menu->Create((ePopupMenu)0x60,
        Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineFriendsDraft::OnErrorDismissed), this)));
    mErrorPopupOpen = true;
}

void SHOnlineFriendsDraft::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mErrorPopupOpen && !g_pFEInput->HasInputLock(this))
        return;
    if (!mIntroComplete)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
            mIntroComplete = true;
        else
            return;
    }
    UpdateDraftStatuses();
    if (NetworkDraft::Instance()->mState == NET_DRAFT_DISCONNECTED)
    {
        ShowDisconnectedError();
        return;
    }
    int countdown = NetworkDraft::Instance()->GetCountdown();
    if (mCountdownSeconds != countdown)
        UpdateCountdown(countdown);
    RefreshPlayerRows();
}

void SHOnlineFriendsDraft::OnErrorDismissed()
{
    mErrorPopupOpen = false;
    FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, true);
    GameSceneManager::Instance()->Push((SceneList)40, SCREEN_BACK, true);
}
