#include "Game/SH/SHOnlineMatchmakingDraft.h"
#include "Game/GameInfo.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/GameSceneManager.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/fePointer.inl"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkLobby.h"
#include "Game/NetworkStatsManager.h"
#include "Game/OnlinePlayer.h"
#include "Game/FE/feOnlineError.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/Render/FrontEndPresentation.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/tlDefault.h"

int gOnlineLocalControllerIndices[2] = { -1, -1 };
u8 gOnlineStartMatchmaking;
int gOnlineMaxMatchmakingEntries;
int gOnlineMinMatchmakingEntries;
u8 gOnlineTwoLocalPlayers;
u8 gOnlineSidekickChoiceSent;
u8 gOnlineUnrankedMatch;

StaticCircularQueue<unsigned int, 3> gRejectedOpponentProfileIds;

static inline bool CanCancelOnlineMatchmaking()
{
    if (!IsOnlineRankedMatch())
        return false;
    if (NetworkDraft::Instance()->mState != NET_DRAFT_IDLE)
        return false;
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    if (lobby != 0)
    {
        if (!lobby->IsMatchmaking())
            return true;
        if (lobby->CanCancelMatchmaking())
            return true;
    }
    return false;
}

static inline void CancelOnlineMatchmaking()
{
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    if (lobby != 0 && lobby->IsMatchmaking() && lobby->CanCancelMatchmaking())
        lobby->CancelMatchmaking();
}

static inline void InitializeLocalPlayerRow(FEOnlinePlayerRow& row)
{
    nlStrNCpy(row.mName, gNetworkMiiNameWide, 14);
    memcpy(row.mMiiData, &gNetworkMiiData, sizeof(row.mMiiData));
    row.mSearchState = 4;
    row.mStatus = 1;
    if (NetworkStatsManager::Instance()->GetLocalStats(0) != 0)
        row.mStats = *NetworkStatsManager::Instance()->GetLocalStats(0);
    else
        memset(&row.mStats, 0, sizeof(NetworkRankingMeta));
    if (IsOnlineRankedMatch() && HasOnlineTwoLocalPlayers())
        row.mSide = 3;
    else
        row.mSide = 0;
    row.mVisible = true;
}

static inline void ClearPlayerRow(FEOnlinePlayerRow& row, int searchState, bool visible)
{
    row.mName[0] = 0;
    row.mSearchState = searchState;
    row.mStatus = 1;
    memset(&row.mStats, 0, sizeof(NetworkRankingMeta));
    row.mVisible = visible;
}

SHOnlineMatchmakingDraft::SHOnlineMatchmakingDraft()
    : mScrollOffset(0)
    , mIntroFinished(false)
    , mCountdown(-1)
    , mErrorPopupOpen(false)
{
    if (gOnlineStartMatchmaking)
    {
        mPlayerCount = gOnlineMaxMatchmakingEntries;
        mDraftStarted = false;
        mConnectionCount = 0;
        InitializeLocalPlayerRow(mPlayers[0]);
        int i;
        for (i = 1; i < mPlayerCount; ++i)
            ClearPlayerRow(mPlayers[i], 0, true);
        for (; i < 8; ++i)
            ClearPlayerRow(mPlayers[i], 0, false);
        g_pNetworkSession->GetOnlineLobby()->StartMatchmakingThread();
        FEMusic::StopStream();
        FEAudio::PlayAnimAudioEvent(0x89B1FC93, "FE_MATCHMAKING_DRAFT", (void*)0x2A, true);
    }
    else
    {
        mPlayerCount = NetworkDraft::Instance()->mTeamCount;
        mDraftStarted = true;
        mConnectionCount = 0;
        FEMusic::StartStreamIfDifferent(8);
        FEAudio::StopAnimAudioEvent(0x89B1FC93, (void*)0x2A);
        UpdateDraftTeams();
        UpdateDraftStatuses();
    }
    mBackButton.SetBackScene(0x29);
    mReturnScene = 0x29;
    if (CanCancelOnlineMatchmaking())
        mCanCancel = true;
    else
        mCanCancel = false;
    if (mPlayerCount > 4)
    {
        mScrollRange = mPlayerCount - 4;
        mScrollingEnabled = true;
    }
    else
    {
        mScrollRange = 0;
        mScrollingEnabled = false;
    }
}

void SHOnlineMatchmakingDraft::UpdateDraftTeams()
{
    mPlayerCount = NetworkDraft::Instance()->mTeamCount;
    int count = mPlayerCount;
    int i;
    for (i = 0; i < count; ++i)
    {
        FEOnlinePlayerRow& row = mPlayers[i];
        NetworkDraftTeam* team = NetworkDraft::Instance()->GetDraftTeam(i);
        nlStrNCpy(row.mName, team->mPlayers[0].mName, 14);
        memcpy(row.mMiiData, team->mPlayers[0].mMiiData, sizeof(row.mMiiData));
        row.mSearchState = 4;
        row.mStatus = 1;
        row.mStats = team->mPlayers[0].mStats;
        if (IsOnlineRankedMatch() && HasOnlineTwoLocalPlayers())
            row.mSide = 3;
        else
            row.mSide = 0;
        row.mVisible = true;
    }
    for (; i < 8; ++i)
        ClearPlayerRow(mPlayers[i], 0, false);
}

void SHOnlineMatchmakingDraft::UpdateDraftStatuses()
{
    int count = NetworkDraft::Instance()->mTeamCount;
    int draftingTeam = NetworkDraft::Instance()->GetCurrentDraftingTeam();
    for (int i = 0; i < count; ++i)
    {
        FEOnlinePlayerRow& row = mPlayers[i];
        if (NetworkDraft::Instance()->HasDisconnectedPlayer(i))
        {
            row.mStatus = 8;
        }
        else
        {
            NetworkDraftTeam* team = NetworkDraft::Instance()->GetDraftTeam(i);
            if (i > draftingTeam)
                row.mStatus = 1;
            else if (i == draftingTeam)
                row.mStatus = 4;
            else
            {
                row.mStatus = 6;
                row.mCaptain = team->mCaptain;
            }
        }
    }
}

SHOnlineMatchmakingDraft::~SHOnlineMatchmakingDraft()
{
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
        scene->mTimer->m_bVisible = false;
}

inline void SHOnlineMatchmakingDraft::RefreshPlayerRows()
{
    for (int i = 0; i < 4; ++i)
        UpdateOnlinePlayerRow(&mPlayers[i + mScrollOffset], mPlayerInstances[i],
            mPlayerNameBuffers[i], 0x20, mPlayerDescriptionBuffers[i], 0x30, i, mIntroFinished);
}

void SHOnlineMatchmakingDraft::SceneCreated()
{
    for (int i = 0; i < 4; ++i)
    {
        char buffer[32];
        nlSNPrintf(buffer, sizeof(buffer), "FRIEND_%d", i);
        TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::Find<>(
            mPresentation->GetActiveSlide(), "Layer",
            buffer);
        if (instance == 0)
            instance = &TLComponentDefault::sInstance;
        mPlayerInstances[i] = instance;
    }
    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::Find<>(
        mPresentation->GetActiveSlide(), "Layer",
        "scrollbar");
    mScrollWidget.SetComponent(scrollbar == 0 ? &TLComponentDefault::sInstance : scrollbar);
    if (mScrollingEnabled)
    {
        mScrollWidget.SetRange(mScrollRange);
        mScrollWidget.SetValue(mScrollOffset);
    }
    else
    {
        mScrollWidget.SetRange(0);
        mScrollWidget.SetValue(0);
    }
    SHNavigation* scene = GetNavigationScene();
    scene->SetButtons(mCanCancel ? NAVIGATION_BUTTON_BACK : NAVIGATION_BUTTON_NONE, true);
    RefreshPlayerRows();
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);

    TLComponentInstance* title = FEFinder<TLComponentInstance, 4>::Find<>(
        mPresentation->GetActiveSlide(), "Layer",
        "Title2");
    TLTextInstance* title1 = FEFinder<TLTextInstance, 2>::Find<>(
        title->GetActiveSlide(), "Title");
    TLTextInstance* title2 = FEFinder<TLTextInstance, 2>::Find<>(
        title->GetActiveSlide(), "Title2");
    TLTextInstance* title3 = FEFinder<TLTextInstance, 2>::Find<>(
        title->GetActiveSlide(), "Title3");
    if (g_pNetworkSession->mCupMode)
    {
        title1->SetStringId("TITLE_LW_CUP_DRAFT");
        title2->SetStringId("TITLE_LW_CUP_DRAFT");
        title3->SetStringId("TITLE_LW_CUP_DRAFT");
    }
    else
    {
        title1->SetStringId("TITLE_LW_DOMINATION_DRAFT");
        title2->SetStringId("TITLE_LW_DOMINATION_DRAFT");
        title3->SetStringId("TITLE_LW_DOMINATION_DRAFT");
    }

    int countdown = NetworkDraft::Instance()->GetCountdown();
    mCountdown = countdown;
    TLSlide* timerSlide = mPresentation->GetActiveSlide();
    FEFinder<TLInstance, 2>::Find(timerSlide,
        "Layer", "Timer")->m_bVisible = false;
    UpdateCountdownText(countdown);
    mBackButton.SetButtonInstance(scene->GetButton(NAVIGATION_BUTTON_BACK));
    if (mCanCancel)
        mBackButton.Enable();
    else
        mBackButton.Disable();
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, true);
}

void SHOnlineMatchmakingDraft::UpdateCountdownText(int countdown)
{
    TLTextInstance* text = static_cast<TLTextInstance*>(GetNavigationScene()->mTimer);
    if (countdown == -1)
        text->m_bVisible = false;
    else
    {
        text->m_bVisible = true;
        char buffer[8];
        nlSNPrintf(buffer, sizeof(buffer), "%d", countdown);
        nlStrToWcs(buffer, mCountdownBuffer, 8);
        text->SetString(mCountdownBuffer);
    }
}

inline void SHOnlineMatchmakingDraft::ShowError(int error)
{
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
    {
        FEAudio::StopAnimAudioEvent(0x89B1FC93, (void*)0x2A);
        FEMusic::StartStreamIfDifferent(3);
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)error,
            Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineMatchmakingDraft::OnErrorDismissed), this)));
        mErrorPopupOpen = true;
    }
}

void SHOnlineMatchmakingDraft::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mErrorPopupOpen && !g_pFEInput->HasInputLock(this))
        return;

    if (!mIntroFinished)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
        {
            if (mScrollingEnabled && !mScrollWidget.mInitialized)
                mScrollWidget.Initialize();
            mIntroFinished = true;
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("cursor", true, false);
        }
        else
        {
            return;
        }
    }

    if (!mDraftStarted)
    {
        if (NetworkDraft::Instance()->mState == NET_DRAFT_CAPTAINS)
        {
            mDraftStarted = true;
            UpdateDraftTeams();
            FEMusic::StartStreamIfDifferent(8);
            FEAudio::StopAnimAudioEvent(0x89B1FC93, (void*)0x2A);
        }
        else if (g_pNetworkSession->GetOnlineLobby()->mMatchFailed)
        {
            g_pNetworkSession->GetOnlineLobby()->CloseConnectionsAndReset();
            if (g_pNetworkSession->RequiresDisconnectAfterError())
                mReturnScene = SCENE_MAIN_MENU;
            int error = g_pNetworkSession->GetDWCErrorCode();
            ShowError(GetOnlineErrorPopup(error, g_pNetworkSession->RequiresDisconnectAfterError(), 0x5A));
            return;
        }
        else
        {
            int count = g_pNetworkSession->GetOnlineLobby()->GetConnectionCount();
            if (mConnectionCount != count)
            {
                mConnectionCount = count;
                for (int i = 1; i < mPlayerCount; ++i)
                {
                    if (i < mConnectionCount)
                        ClearPlayerRow(mPlayers[i], 1, true);
                    else
                        ClearPlayerRow(mPlayers[i], 0, true);
                }
            }
        }
    }
    else
    {
        UpdateDraftStatuses();
        if (NetworkDraft::Instance()->mState == NET_DRAFT_DISCONNECTED)
        {
            g_pNetworkSession->GetOnlineLobby()->CloseConnectionsAndReset();
            ShowError(0x60);
            return;
        }
    }

    int countdown = NetworkDraft::Instance()->GetCountdown();
    if (mCountdown != countdown)
    {
        mCountdown = countdown;
        TLSlide* timerSlide = mPresentation->GetActiveSlide();
        FEFinder<TLInstance, 2>::Find(timerSlide,
            "Layer", "Timer")->m_bVisible = false;
        UpdateCountdownText(countdown);
    }

    if (mCanCancel)
    {
        if (!CanCancelOnlineMatchmaking())
        {
            GetNavigationScene()->SetButtons(NAVIGATION_BUTTON_NONE, true);
            mBackButton.Disable();
            mCanCancel = false;
        }
    }
    else if (CanCancelOnlineMatchmaking())
    {
        GetNavigationScene()->SetButtons(NAVIGATION_BUTTON_BACK, true);
        mBackButton.Enable();
        mCanCancel = true;
    }

    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* pointer = GetPointerInstance(i);
        if (i != gFEControllerIndex || !CanCancelOnlineMatchmaking())
            pointer->SetActiveSlide("waiting", true, false);
        else
        {
            u8 valid = true;
            FEPointerEvent event;
            event.mIndex = i;
            event.mPosition = GetPointerPosition(i, &valid);
            event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
            event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)i, 30, true, 0);
            if (mScrollingEnabled)
                mScrollWidget.Update(event, fDeltaT);
            if (mBackButton.UpdateBackButton(event, fDeltaT))
            {
                CancelOnlineMatchmaking();
                FEMusic::StartStreamIfDifferent(8);
                FEAudio::StopAnimAudioEvent(0x89B1FC93, (void*)0x2A);
                return;
            }
        }
    }
    if (mScrollingEnabled)
    {
        if (mScrollWidget.IsScrolling(1, 1))
            ++mScrollOffset;
        else if (mScrollWidget.IsScrolling(0, 1))
            --mScrollOffset;
    }
    RefreshPlayerRows();
}

void SHOnlineMatchmakingDraft::OnErrorDismissed()
{
    mErrorPopupOpen = false;
    if (mReturnScene == SCENE_MAIN_MENU)
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, true);
        FrontEndPresentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
    }
    else
    {
        GameSceneManager::Instance()->Push((SceneList)mReturnScene, SCREEN_BACK, true);
        FEMusic::StartStreamIfDifferent(8);
    }
}
