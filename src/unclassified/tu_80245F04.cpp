#include "unclassified/tu_80245F04.h"

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

TU80245F04Scene::TU80245F04Scene()
    : mIntroFinished(false)
    , mCountdown(-1)
    , mErrorPopupOpen(false)
{
    NetMessageDraft& draft = NetworkDraft::Instance()->GetDraftMessage();
    mPlayerCount = 0;
    for (int i = 0; i < draft.mMachineCount; ++i)
    {
        mPlayerMappings[mPlayerCount].mMachine = &draft.mEntries[i];
        mPlayerMappings[mPlayerCount].mGuest = false;
        mPlayerMappings[mPlayerCount].mTeam = draft.mUnidentified0B.mData[i][0];
        ++mPlayerCount;
        if (draft.mEntries[i].mGuestEnabled)
        {
            mPlayerMappings[mPlayerCount].mMachine = &draft.mEntries[i];
            mPlayerMappings[mPlayerCount].mGuest = true;
            mPlayerMappings[mPlayerCount].mTeam = draft.mUnidentified0B.mData[i][1];
            ++mPlayerCount;
        }
    }
    fn_8024671C();
    fn_802460D8();
}

void TU80245F04Scene::fn_802460D8()
{
    NetworkDraft::Instance();
    int draftingTeam = NetworkDraft::Instance()->GetCurrentDraftingTeam();
    for (int i = 0; i < mPlayerCount; ++i)
    {
        FEOnlinePlayerRow& row = mPlayers[i];
        int teamIndex = mPlayerMappings[i].mTeam;
        NetworkDraftTeam* team = NetworkDraft::Instance()->GetDraftTeam(teamIndex);
        NetworkDraftPlayer* player = team->FindPlayer(mPlayerMappings[i].mMachine->mIndex, mPlayerMappings[i].mGuest);
        if (player->mDisconnected)
            row.mStatus = 8;
        else if (teamIndex > draftingTeam)
            row.mStatus = 1;
        else if (teamIndex == draftingTeam)
        {
            NetworkDraft* draft = NetworkDraft::Instance();
            int draftingPeer = draft->mCurrentDraftingPeer;
            bool draftingGuest = draft->mCurrentDrafterIsLocal;
            if (draftingPeer == mPlayerMappings[i].mMachine->mIndex
                && draftingGuest == mPlayerMappings[i].mGuest)
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

TU80245F04Scene::~TU80245F04Scene()
{
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
        scene->mTimer->m_bVisible = false;
}

void TU80245F04Scene::fn_8024671C()
{
    int i;
    for (i = 0; i < mPlayerCount; ++i)
    {
        FEOnlinePlayerRow& row = mPlayers[i];
        row.Reset();
        if (!mPlayerMappings[i].mGuest)
            nlStrNCpy(row.mName, mPlayerMappings[i].mMachine->mName, 14);
        else
        {
            BasicString<unsigned short, Detail::TempStringAllocator> string(LookupLocString("ONLINE_CONTROLLERS_GUEST"));
            nlStrNCpy(row.mName, string.c_str(), 14);
        }
        memcpy(row.mMiiData, mPlayerMappings[i].mMachine->mUnidentified32, sizeof(row.mMiiData));
        row.mSearchState = 4;
        row.mStatus = 1;
        row.mStats = mPlayerMappings[i].mMachine->mHead;
        NetworkLeaderboardCategory* category = NetworkStatsManager::Instance()->GetCategory(4);
        if (category != 0)
        {
            int player = category->FindPlayer(mPlayerMappings[i].mMachine->mUnidentified18);
            if (player != -1)
                row.mStats.mDisplayRank = category->mMetadata[player].mDisplayRank;
        }
        int side = 2;
        if (mPlayerMappings[i].mTeam == 0)
            side = 1;
        row.mSide = side;
        row.mVisible = true;
        row.mGuest = mPlayerMappings[i].mGuest;
    }
    for (; i < 4; ++i)
    {
        mPlayers[i].Reset();
        mPlayers[i].mVisible = false;
    }
}

void TU80245F04Scene::SceneCreated()
{
    for (int i = 0; i < 4; ++i)
    {
        char buffer[32];
        nlSNPrintf(buffer, sizeof(buffer), "FRIEND_%d", i);
        TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::Find(
            mPresentation->GetActiveSlide(), "Layer", buffer);
        if (instance == 0)
            instance = &UnidentifiedTLComponentDefault::sInstance;
        mPlayerInstances[i] = instance;
    }
    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::Find(
        mPresentation->GetActiveSlide(), "Layer", "scrollbar");
    mScrollBar.SetComponent(scrollbar == 0 ? &UnidentifiedTLComponentDefault::sInstance : scrollbar);
    mScrollBar.SetRange(0);
    mScrollBar.SetValue(0);
    GetNavigationScene()->SetButtons(0, true);
    UpdatePlayerRows();
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);

    TLComponentInstance* title = FEFinder<TLComponentInstance, 4>::Find(
        mPresentation->GetActiveSlide(), "Layer", "Title2");
    TLTextInstance* title1 = FEFinder<TLTextInstance, TLAT_TEXT>::Find(title->GetActiveSlide(), "Title");
    TLTextInstance* title2 = FEFinder<TLTextInstance, TLAT_TEXT>::Find(title->GetActiveSlide(), "Title2");
    TLTextInstance* title3 = FEFinder<TLTextInstance, TLAT_TEXT>::Find(title->GetActiveSlide(), "Title3");
    title1->SetStringId("TITLE_LW_DOMINATION_DRAFT");
    title2->SetStringId("TITLE_LW_DOMINATION_DRAFT");
    title3->SetStringId("TITLE_LW_DOMINATION_DRAFT");

    int countdown = NetworkDraft::Instance()->GetCountdown();
    UpdateTimer(countdown);
}

void TU80245F04Scene::UpdatePlayerRows()
{
    for (int i = 0; i < 4; ++i)
        UpdateOnlinePlayerRow(&mPlayers[i], mPlayerInstances[i], mPlayerNameBuffers[i], 0x20,
            mPlayerDescriptionBuffers[i], 0x30, i, mIntroFinished);
}

void TU80245F04Scene::UpdateTimer(int countdown)
{
    mCountdown = countdown;
    TLSlide* timerSlide = mPresentation->GetActiveSlide();
    FEFinder<TLTextInstance, TLAT_TEXT>::Find(timerSlide, "Layer", "Timer")->m_bVisible = false;
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

inline void TU80245F04Scene::ShowDisconnectedError()
{
    g_pNetworkSession->GetOnlineLobby()->CloseConnectionsAndReset();
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) == (SceneList)10)
        return;
    FEPopupMenu* menu = static_cast<FEPopupMenu*>(
        GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false));
    menu->Create((ePopupMenu)0x60,
        Function<FnVoidVoid>(Bind<void>(MemFun(&TU80245F04Scene::fn_802466C0), this)));
    mErrorPopupOpen = true;
}

void TU80245F04Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mErrorPopupOpen && !g_pFEInput->HasInputLock(this))
        return;
    if (!mIntroFinished)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
            mIntroFinished = true;
        else
            return;
    }
    fn_802460D8();
    if (NetworkDraft::Instance()->mState == NET_DRAFT_DISCONNECTED)
    {
        ShowDisconnectedError();
        return;
    }
    int countdown = NetworkDraft::Instance()->GetCountdown();
    if (mCountdown != countdown)
        UpdateTimer(countdown);
    UpdatePlayerRows();
}

void TU80245F04Scene::fn_802466C0()
{
    mErrorPopupOpen = false;
    FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, true);
    GameSceneManager::Instance()->Push((SceneList)40, SCREEN_BACK, true);
}
