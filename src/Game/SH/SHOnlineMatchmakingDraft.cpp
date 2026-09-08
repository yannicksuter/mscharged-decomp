#include <dwc/dwc_main.h>
#include "Game/SH/SHOnlineMatchmakingDraft.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/NetworkLobby.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/NetworkDraft.h"
#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/Render/Presentation.h"
#include "NL/nlBind.h"
#include "NL/nlPrint.h"
#include "NL/nlstring_tmpl.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/feOnlineError.h"

SHOnlineMatchmakingDraft::SHOnlineMatchmakingDraft()
    : mUnidentified024(0)
    , mUnidentified03C(false)
    , mUnidentified040(-1)
    , mUnidentified044(false)
{
    if (gOnlineStartMatchmaking)
    {
        mUnidentified01C = gOnlineMaxMatchmakingEntries;
        mUnidentified02C = false;
        mUnidentified030 = 0;
        nlStrNCpy(mUnidentified574[0].mName, gNetworkMiiNameWide, 14);
        memcpy(mUnidentified574[0].mMiiData, &gNetworkMiiData, 0x4C);
        mUnidentified574[0].mSearchState = 4;
        mUnidentified574[0].mStatus = 1;
        if (NetworkStatsManager_8012F378::Instance()->GetLocalStats(0) != 0)
            mUnidentified574[0].mStats = *NetworkStatsManager_8012F378::Instance()->GetLocalStats(0);
        else
            memset(&mUnidentified574[0].mStats, 0, sizeof(NetworkRankingMeta));
        mUnidentified574[0].mSide = IsOnlineRankedMatch() && HasOnlineTwoLocalPlayers() ? 3 : 0;
        mUnidentified574[0].mVisible = true;
        int i;
        for (i = 1; i < mUnidentified01C; ++i)
        {
            mUnidentified574[i].mName[0] = 0;
            mUnidentified574[i].mSearchState = 0;
            mUnidentified574[i].mStatus = 1;
            memset(&mUnidentified574[i].mStats, 0, sizeof(NetworkRankingMeta));
            mUnidentified574[i].mVisible = true;
        }
        for (; i < 8; ++i)
        {
            mUnidentified574[i].mName[0] = 0;
            mUnidentified574[i].mSearchState = 0;
            mUnidentified574[i].mStatus = 1;
            memset(&mUnidentified574[i].mStats, 0, sizeof(NetworkRankingMeta));
            mUnidentified574[i].mVisible = false;
        }
        g_pNetworkSession->GetOnlineLobby()->StartMatchmakingThread();
        FEMusic::StopStream();
        FEAudio::PlayAnimAudioEvent(0x89B1FC93, "FE_MATCHMAKING_DRAFT", (void*)42, 1);
    }
    else
    {
        mUnidentified01C = NetworkDraft::Instance()->mTeamCount;
        mUnidentified02C = true;
        mUnidentified030 = 0;
        FEMusic::StartStreamIfDifferent(8);
        FEAudio::StopAnimAudioEvent(0x89B1FC93, (void*)42);
        UpdateDraftTeams();
        UpdateDraftStatuses();
    }
    mUnidentified1FC.SetBackScene(41);
    mUnidentified038 = 41;
    mUnidentified034 = CanCancelMatchmaking();
    if (mUnidentified01C > 4)
    {
        mUnidentified028 = mUnidentified01C - 4;
        mUnidentified020 = true;
    }
    else
    {
        mUnidentified028 = 0;
        mUnidentified020 = false;
    }
}

void SHOnlineMatchmakingDraft::UpdateDraftTeams()
{
    int count = NetworkDraft::Instance()->mTeamCount;
    mUnidentified01C = count;
    int i;
    for (i = 0; i < count; ++i)
    {
        NetworkDraftTeam* team = NetworkDraft::Instance()->GetDraftTeam(i);
        FEOnlinePlayerRow& row = mUnidentified574[i];
        nlStrNCpy(row.mName, team->mPlayers[0].mName, 14);
        memcpy(row.mMiiData, team->mPlayers[0].mData, 0x4C);
        row.mSearchState = 4;
        row.mStatus = 1;
        row.mStats = team->mPlayers[0].mHead;
        row.mSide = IsOnlineRankedMatch() && HasOnlineTwoLocalPlayers() ? 3 : 0;
        row.mVisible = true;
    }
    for (; i < 8; ++i)
    {
        mUnidentified574[i].mName[0] = 0;
        mUnidentified574[i].mSearchState = 0;
        mUnidentified574[i].mStatus = 1;
        memset(&mUnidentified574[i].mStats, 0, sizeof(NetworkRankingMeta));
        mUnidentified574[i].mVisible = false;
    }
}

void SHOnlineMatchmakingDraft::UpdateDraftStatuses()
{
    int count = NetworkDraft::Instance()->mTeamCount;
    int current = NetworkDraft::Instance()->GetCurrentDraftingTeam();
    for (int i = 0; i < count; ++i)
    {
        if (NetworkDraft::Instance()->HasDisconnectedPlayer(i))
            mUnidentified574[i].mStatus = 8;
        else
        {
            NetworkDraftTeam* team = NetworkDraft::Instance()->GetDraftTeam(i);
            if (i > current)
                mUnidentified574[i].mStatus = 1;
            else if (i == current)
                mUnidentified574[i].mStatus = 4;
            else
            {
                mUnidentified574[i].mStatus = 6;
                mUnidentified574[i].mCaptain = team->mCaptain;
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

bool SHOnlineMatchmakingDraft::CanCancelMatchmaking()
{
    if (!IsOnlineRankedMatch() || NetworkDraft::Instance()->mState != NET_DRAFT_IDLE)
        return false;
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    if (lobby == 0)
        return false;
    if (!lobby->mMatchmakingThreadRunning && lobby->mState == 0)
        return true;
    return lobby->CanCancelMatchmaking();
}

int SHOnlineMatchmakingDraft::GetRemainingDraftTime()
{
    NetworkDraft* draft = NetworkDraft::Instance();
    int time = 0;
    switch (draft->mState)
    {
    case NET_DRAFT_IDLE: return -1;
    case NET_DRAFT_CAPTAINS: time = (int)draft->mTimeBeforeDrafting; break;
    case NET_DRAFT_SIDEKICKS: time = (int)draft->mTimeToChangeDrafters; break;
    case NET_DRAFT_FINAL_COUNTDOWN: time = (int)draft->mFinalCountdown; break;
    case NET_DRAFT_STARTED:
    case NET_DRAFT_DISCONNECTED: time = 0; break;
    }
    return time < 0 ? 0 : time;
}

void SHOnlineMatchmakingDraft::UpdateTimerText(int time)
{
    mUnidentified040 = time;
    TLInstance* timer = FEFinder<TLInstance, 2>::Find(mPresentation->m_currentSlide,
        InlineHasher("Layer"), InlineHasher("Timer"));
    timer->m_bVisible = false;
    TLTextInstance* overlayTimer = (TLTextInstance*)GetNavigationScene()->mTimer;
    if (time == -1)
        overlayTimer->m_bVisible = false;
    else
    {
        overlayTimer->m_bVisible = true;
        char buffer[8];
        nlSNPrintf(buffer, sizeof(buffer), "%d", time);
        nlStrToWcs(buffer, mUnidentified564, 8);
        overlayTimer->SetString(mUnidentified564);
    }
}

void SHOnlineMatchmakingDraft::SceneCreated()
{
    for (int i = 0; i < 4; ++i)
    {
        char name[32];
        nlSNPrintf(name, sizeof(name), "FRIEND_%d", i);
        TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::Find(mPresentation->m_currentSlide,
            InlineHasher("Layer"), InlineHasher(name));
        mUnidentified2D4[i] = instance != 0 ? instance : &gDefaultTLComponentInstance;
    }
    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::Find(mPresentation->m_currentSlide,
        InlineHasher("Layer"), InlineHasher("scrollbar"));
    if (scrollbar == 0)
        scrollbar = &gDefaultTLComponentInstance;
    mUnidentified048.SetComponent(scrollbar);
    if (mUnidentified020)
    {
        mUnidentified048.SetRange(mUnidentified028);
        mUnidentified048.SetValue(mUnidentified024);
    }
    else
    {
        mUnidentified048.SetRange(0);
        mUnidentified048.SetValue(0);
    }
    SHNavigation* scene = GetNavigationScene();
    scene->SetButtons(mUnidentified034 ? 4 : 0, true);
    for (int i = 0; i < 4; ++i)
        UpdateOnlinePlayerRow(&mUnidentified574[i + mUnidentified024], mUnidentified2D4[i],
            mUnidentified2E4[i], 32, mUnidentified3E4[i], 48, i, mUnidentified03C);
    for (int i = 0; i < 4; ++i)
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    TLComponentInstance* title = FEFinder<TLComponentInstance, 4>::Find(mPresentation->m_currentSlide,
        InlineHasher("Layer"), InlineHasher("Title2"));
    TLTextInstance* text0 = FEFinder<TLTextInstance, 3>::Find(title->GetActiveSlide(), InlineHasher("Title"));
    TLTextInstance* text1 = FEFinder<TLTextInstance, 3>::Find(title->GetActiveSlide(), InlineHasher("Title2"));
    TLTextInstance* text2 = FEFinder<TLTextInstance, 3>::Find(title->GetActiveSlide(), InlineHasher("Title3"));
    if (g_pNetworkSession->mTournamentMode)
    {
        text0->SetStringId("TITLE_LW_CUP_DRAFT");
        text1->SetStringId("TITLE_LW_CUP_DRAFT");
        text2->SetStringId("TITLE_LW_CUP_DRAFT");
    }
    else
    {
        text0->SetStringId("TITLE_LW_DOMINATION_DRAFT");
        text1->SetStringId("TITLE_LW_DOMINATION_DRAFT");
        text2->SetStringId("TITLE_LW_DOMINATION_DRAFT");
    }
    UpdateTimerText(GetRemainingDraftTime());
    mUnidentified1FC.SetButtonInstance(scene->GetButton(4));
    if (mUnidentified034)
        mUnidentified1FC.Enable();
    else
        mUnidentified1FC.Disable();
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void SHOnlineMatchmakingDraft::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    if (mUnidentified044 && !g_pFEInput->HasInputLock(this))
        return;
    if (!mUnidentified03C)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (!(slide->m_time >= slide->m_start + slide->m_duration))
            return;
        if (mUnidentified020 && !mUnidentified048.mInitialized)
            mUnidentified048.Initialize();
        mUnidentified03C = true;
        for (int i = 0; i < 4; ++i)
            gFEPointerInstances[i]->SetActiveSlide("cursor", true, false);
    }
    if (!mUnidentified02C)
    {
        if (NetworkDraft::Instance()->mState == NET_DRAFT_CAPTAINS)
        {
            mUnidentified02C = true;
            UpdateDraftTeams();
            FEMusic::StartStreamIfDifferent(8);
            FEAudio::StopAnimAudioEvent(0x89B1FC93, (void*)42);
        }
        else if (g_pNetworkSession->GetOnlineLobby()->mMatchFailed)
        {
            g_pNetworkSession->GetOnlineLobby()->CloseConnectionsAndReset();
            if (g_pNetworkSession->RequiresDisconnectAfterError())
                mUnidentified038 = 1;
            int error = g_pNetworkSession->mDWCErrorCode;
            error = GetOnlineErrorPopup(error, g_pNetworkSession->RequiresDisconnectAfterError(), 90);
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
            {
                FEAudio::StopAnimAudioEvent(0x89B1FC93, (void*)42);
                FEMusic::StartStreamIfDifferent(3);
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)error, Bind<void>(MemFun(&SHOnlineMatchmakingDraft::OnErrorDismissed), this));
                mUnidentified044 = true;
            }
            return;
        }
        else
        {
            g_pNetworkSession->GetOnlineLobby();
            int connections = DWC_GetNumConnectionHost();
            if (mUnidentified030 != connections)
            {
                mUnidentified030 = connections;
                for (int i = 1; i < mUnidentified01C; ++i)
                {
                    FEOnlinePlayerRow& row = mUnidentified574[i];
                    row.mName[0] = 0;
                    row.mSearchState = i < mUnidentified030 ? 1 : 0;
                    row.mStatus = 1;
                    memset(&row.mStats, 0, sizeof(NetworkRankingMeta));
                    row.mVisible = true;
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
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
            {
                FEAudio::StopAnimAudioEvent(0x89B1FC93, (void*)42);
                FEMusic::StartStreamIfDifferent(3);
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)96, Bind<void>(MemFun(&SHOnlineMatchmakingDraft::OnErrorDismissed), this));
                mUnidentified044 = true;
            }
            return;
        }
    }
    int time = GetRemainingDraftTime();
    if (mUnidentified040 != time)
        UpdateTimerText(time);
    if (mUnidentified034)
    {
        if (!CanCancelMatchmaking())
        {
            GetNavigationScene()->SetButtons(0, true);
            mUnidentified1FC.Disable();
            mUnidentified034 = false;
        }
    }
    else if (CanCancelMatchmaking())
    {
        GetNavigationScene()->SetButtons(4, true);
        mUnidentified1FC.Enable();
        mUnidentified034 = true;
    }
    for (int i = 0; i < 4; ++i)
    {
        if ((unsigned int)i != gFEControllerIndex || !CanCancelMatchmaking())
        {
            gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
            continue;
        }
        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
        event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)i, 30, true, 0);
        if (mUnidentified020)
            mUnidentified048.Update(event, dt);
        if (mUnidentified1FC.UpdateBackButton(event, dt))
        {
            NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
            if (lobby != 0 && (lobby->mMatchmakingThreadRunning || lobby->mState != 0)
                && lobby->CanCancelMatchmaking())
                lobby->CancelMatchmaking();
            FEMusic::StartStreamIfDifferent(8);
            FEAudio::StopAnimAudioEvent(0x89B1FC93, (void*)42);
            return;
        }
    }
    if (mUnidentified020)
    {
        if (mUnidentified048.IsScrolling(1, 1))
            ++mUnidentified024;
        else if (mUnidentified048.IsScrolling(0, 1))
            --mUnidentified024;
    }
    for (int i = 0; i < 4; ++i)
        UpdateOnlinePlayerRow(&mUnidentified574[i + mUnidentified024], mUnidentified2D4[i],
            mUnidentified2E4[i], 32, mUnidentified3E4[i], 48, i, mUnidentified03C);
}

void SHOnlineMatchmakingDraft::OnErrorDismissed()
{
    mUnidentified044 = false;
    if (mUnidentified038 == 1)
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
        Presentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
    }
    else
    {
        GameSceneManager::Instance()->Push((SceneList)mUnidentified038, SCREEN_BACK, true);
        FEMusic::StartStreamIfDifferent(8);
    }
}
