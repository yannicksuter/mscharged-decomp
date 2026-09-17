#include "Game/NetworkMessageRegistry.h"
#include "Game/NetworkDraft.h"
#include "Game/Sys/debug.h"

#include "Game/GameInfo.h"
#include "Game/GameSceneManager.h"
#include "Game/NetworkSession.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/TweakValue.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <stdlib.h>
#include <string.h>


extern int gCaptainSelectionOrder[12];

static NetworkDraft* sNetworkDraft;

static float s_fDefaultTimeToWaitBeforeDrafting = 5.0f;
static float s_fDefaultTimeToChangeDrafters = 20.0f;
static float s_fDefaultTimeToChooseSidekicks = 20.0f;
static float s_fDefaultTimeFinalCountdown = 5.0f;

static TweakFloatBinding sDefaultTimeToWaitBeforeDraftingTweak(
    "s_fDefaultTimeToWaitBeforeDrafting", "Network/Draft",
    &s_fDefaultTimeToWaitBeforeDrafting);
static TweakFloatBinding sDefaultTimeToChangeDraftersTweak(
    "s_fDefaultTimeToChangeDrafters", "Network/Draft",
    &s_fDefaultTimeToChangeDrafters);
static TweakFloatBinding sDefaultTimeToChooseSidekicksTweak(
    "s_fDefaultTimeToChooseSidekicks", "Network/Draft",
    &s_fDefaultTimeToChooseSidekicks);
static TweakFloatBinding sDefaultTimeFinalCountdownTweak(
    "s_fDefaultTimeFinalCountdown", "Network/Draft",
    &s_fDefaultTimeFinalCountdown);

void NetMessageDraft::Serialize(NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(&mMachineCount, sizeof(mMachineCount));
    serializer->Transfer(&mUnidentified0A, sizeof(mUnidentified0A));
    serializer->Transfer(&mPlayerSides, sizeof(mPlayerSides));
    for (int i = 0; i < 8; ++i)
    {
        serializer->Transfer(&mEntries[i].mStats, sizeof(mEntries[i].mStats));
        serializer->Transfer(
            &mEntries[i].mProfileId, sizeof(mEntries[i].mProfileId));
        serializer->Transfer(mEntries[i].mName, sizeof(mEntries[i].mName));
        serializer->Transfer(
            mEntries[i].mMiiData, sizeof(mEntries[i].mMiiData));
        serializer->Transfer(&mEntries[i].mMachineIndex, sizeof(mEntries[i].mMachineIndex));
        serializer->Transfer(&mEntries[i].mGuestEnabled,
            sizeof(mEntries[i].mGuestEnabled));
    }
}

void NetMessageDraftMachineInfo::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mEntry.mStats, sizeof(mEntry.mStats));
    serializer->Transfer(&mEntry.mProfileId, sizeof(mEntry.mProfileId));
    serializer->Transfer(mEntry.mName, sizeof(mEntry.mName));
    serializer->Transfer(mEntry.mMiiData, sizeof(mEntry.mMiiData));
    serializer->Transfer(&mEntry.mMachineIndex, sizeof(mEntry.mMachineIndex));
    serializer->Transfer(
        &mEntry.mGuestEnabled, sizeof(mEntry.mGuestEnabled));
}

void NetMessageDraftPickedCaptain::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mTeamIndex, sizeof(mTeamIndex));
    serializer->Transfer(&mCaptain, sizeof(mCaptain));
}

void NetMessageDraftPickedSidekicks::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mTeamIndex, sizeof(mTeamIndex));
    serializer->Transfer(&mSidekick0, sizeof(mSidekick0));
    serializer->Transfer(&mSidekick1, sizeof(mSidekick1));
    serializer->Transfer(&mSidekick2, sizeof(mSidekick2));
}

void NetMessageSidesChanged::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(&mSide, sizeof(mSide));
    serializer->Transfer(&mGuest, sizeof(mGuest));
    serializer->Transfer(&mAccepted, sizeof(mAccepted));
}

void NetMessageCheckConnection::Serialize(
    NetworkMessageSerializer* serializer)
{
    for (int i = 0; i < 2; ++i)
    {
        serializer->Transfer(&mProfileIds[i], sizeof(mProfileIds[i]));
    }
}

void NetMessageConnectionDecision::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mAccepted, sizeof(mAccepted));
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
}

NetMessageDraftPickedCaptain::~NetMessageDraftPickedCaptain() { }
NetMessageDraftPickedSidekicks::~NetMessageDraftPickedSidekicks() { }
NetMessageConnectionDecision::~NetMessageConnectionDecision() { }

int NetMessageConnectionDecision::GetType() { return 27; }
int NetMessageCheckConnection::GetType() { return 26; }
int NetMessageSidesChanged::GetType() { return 25; }
int NetMessageDraftPickedSidekicks::GetType() { return 24; }
int NetMessageDraftPickedCaptain::GetType() { return 23; }
int NetMessageDraftMachineInfo::GetType() { return 22; }
int NetMessageDraft::GetType() { return 21; }

void NetworkDraft::CreateInstance()
{
    void* storage = nlMalloc(sizeof(NetworkDraft), 8, false);
    NetworkDraft* draft = 0;
    if (storage != 0)
    {
        draft = new (storage) NetworkDraft;
    }
    sNetworkDraft = draft;
}

NetworkDraft* NetworkDraft::Instance()
{
    return sNetworkDraft;
}

void NetworkDraft::Reset(bool)
{
    mState = NET_DRAFT_IDLE;
    mLocalMachineIndex = -1;
    mMyTeamIndex = -1;
    mTeamCount = 0;
    mCurrentDraftingTeam = -1;
    mCurrentDraftingPeer = -1;
    mCurrentDrafterIsGuest = false;
    mSideToTeam[0] = -1;
    mSideDrafted[0] = false;
    mSideToTeam[1] = -1;
    mSideDrafted[1] = false;
    mNextDraftingTeam = -1;
    mTimeBeforeDrafting = s_fDefaultTimeToWaitBeforeDrafting;
    mTimeToChangeDrafters = s_fDefaultTimeToChooseSidekicks;
    mFinalCountdown = s_fDefaultTimeFinalCountdown;
}

void NetworkDraft::BeginSortedDraft(NetMessageDraft* message)
{
    gNetworkMessageRegistry->RegisterReceiver(23, this);
    gNetworkMessageRegistry->RegisterReceiver(24, this);
    mDraftMessage = *message;
    mTeamCount = message->mMachineCount;
    mLocalMachineIndex = message->mMachineIndex;
    mMyTeamIndex = -1;
    mCurrentDraftingTeam = -1;
    mCurrentDraftingPeer = -1;
    mCurrentDrafterIsGuest = false;
    mSideToTeam[0] = -1;
    mSideDrafted[0] = false;
    mSideToTeam[1] = -1;
    mSideDrafted[1] = false;

    tDebugPrintManager::Print(DC_NETWORK, "Starting Draft num Teams %d\n", mTeamCount);
    for (int teamIndex = 0; teamIndex < mTeamCount; ++teamIndex)
    {
        mTeams[teamIndex].Reset();
        mTeams[teamIndex].mPlayerCount = 1;
        NetworkDraftPlayer& player = mTeams[teamIndex].mPlayers[0];
        const NetworkDraftMachineInfo& entry = message->mEntries[teamIndex];
        player.mHead = entry.mStats;
        nlStrNCpy(player.mName, entry.mName, 11);
        memcpy(player.mData, entry.mMiiData, sizeof(player.mData));
        player.mPeerIndex = (s8)entry.mMachineIndex;
    }

    qsort(mTeams, mTeamCount, sizeof(NetworkDraftTeam), CompareDraftTeams);
    for (int teamIndex = 0; teamIndex < mTeamCount; ++teamIndex)
    {
        if (mTeams[teamIndex].mPlayers[0].mPeerIndex == GetLocalMachineIndex())
        {
            mMyTeamIndex = teamIndex;
        }
    }
    tDebugPrintManager::Print(DC_NETWORK, "Sorted Draft MyTeamIndex %d MyMachineIndex %d\n",
        mMyTeamIndex, GetLocalMachineIndex());
    for (int teamIndex = 0; teamIndex < mTeamCount; ++teamIndex)
    {
        char name[12];
        NetworkDraftPlayer& player = mTeams[teamIndex].mPlayers[0];
        nlWcsToStr(player.mName, name, 11);
        tDebugPrintManager::Print(DC_NETWORK,
            "%s Rank %d. %d-%d MyPeerIndex %d\n", name,
            player.mHead.mDisplayRank, player.mHead.mWins, player.mHead.mLosses,
            player.mPeerIndex);
    }
    mNextDraftingTeam = -1;
    mTimeBeforeDrafting = s_fDefaultTimeToWaitBeforeDrafting;
    mTimeToChangeDrafters = s_fDefaultTimeToChooseSidekicks;
    mFinalCountdown = s_fDefaultTimeFinalCountdown;
    mState = NET_DRAFT_CAPTAINS;
    gOnlineStartMatchmaking = 0;
    GameSceneManager::Instance()->Push((SceneList)0x31, SCREEN_NOTHING, true);
}

void NetworkDraft::BeginTeamDraft(NetMessageDraft* message)
{
    gNetworkMessageRegistry->RegisterReceiver(23, this);
    gNetworkMessageRegistry->RegisterReceiver(24, this);
    mDraftMessage = *message;
    mTeamCount = 2;
    mLocalMachineIndex = message->mMachineIndex;
    mMyTeamIndex = -1;
    mCurrentDraftingTeam = -1;
    mCurrentDraftingPeer = -1;
    mCurrentDrafterIsGuest = false;
    mTeams[0].Reset();
    mTeams[1].Reset();

    for (int entryIndex = 0; entryIndex < message->mMachineCount; ++entryIndex)
    {
        const NetworkDraftMachineInfo& entry = message->mEntries[entryIndex];
        int playerCount = entry.mGuestEnabled ? 2 : 1;
        for (int playerIndex = 0; playerIndex < playerCount; ++playerIndex)
        {
            int teamIndex = message->mPlayerSides.mData[entryIndex][playerIndex];
            NetworkDraftTeam& team = mTeams[teamIndex];
            NetworkDraftPlayer& player = team.mPlayers[team.mPlayerCount];
            player.mHead = entry.mStats;
            nlStrNCpy(player.mName, entry.mName, 11);
            memcpy(player.mData, entry.mMiiData, sizeof(player.mData));
            player.mPeerIndex = (s8)entry.mMachineIndex;
            player.mGuest = playerIndex == 1;
            ++team.mPlayerCount;
        }
    }
    AssignDraftSides();
    mNextDraftingTeam = -1;
    mTimeBeforeDrafting = s_fDefaultTimeToWaitBeforeDrafting;
    mTimeToChangeDrafters = s_fDefaultTimeToChooseSidekicks;
    mFinalCountdown = s_fDefaultTimeFinalCountdown;
    mState = NET_DRAFT_CAPTAINS;
    GameSceneManager::Instance()->Push((SceneList)0x32, SCREEN_FORWARD, true);
}

void NetworkDraft::AssignDraftSides()
{
    mSideToTeam[0] = 0;
    mSideToTeam[1] = mTeamCount > 1 ? 1 : -1;
    mSideDrafted[0] = mSideToTeam[0] >= 0;
    mSideDrafted[1] = mSideToTeam[1] >= 0;
}

int NetworkDraft::CompareDraftTeams(const void* left, const void* right)
{
    const NetworkDraftTeam* leftTeam = (const NetworkDraftTeam*)left;
    const NetworkDraftTeam* rightTeam = (const NetworkDraftTeam*)right;
    int leftRank = leftTeam->mPlayers[0].mHead.mScore;
    int rightRank = rightTeam->mPlayers[0].mHead.mScore;
    if (leftRank > rightRank)
    {
        return 1;
    }
    if (leftRank < rightRank)
    {
        return -1;
    }
    return 0;
}

bool NetworkDraft::HasDisconnectedPlayer(int team) const
{
    const NetworkDraftTeam& draftTeam = mTeams[team];
    for (int player = 0; player < draftTeam.mPlayerCount; ++player)
    {
        if (draftTeam.mPlayers[player].mDisconnected)
        {
            return true;
        }
    }
    return false;
}

void NetworkDraft::Update(float dt)
{
    if (mState != NET_DRAFT_IDLE)
    {
        NetworkMachineRoster* roster =
            g_pNetworkSessionBase->GetMachineRoster();
        for (int team = 0; team < mTeamCount; ++team)
        {
            for (int player = 0; player < mTeams[team].mPlayerCount; ++player)
            {
                int peer = mTeams[team].mPlayers[player].mPeerIndex;
                if (peer != mLocalMachineIndex
                    && roster->GetMachineAid(peer) == 0
                    && !mTeams[team].mPlayers[player].mDisconnected)
                {
                    mTeams[team].mPlayers[player].mDisconnected = true;
                }
            }
        }
    }

    switch (mState)
    {
    case NET_DRAFT_CAPTAINS:
        if (mTimeBeforeDrafting > 0.0f)
        {
            mTimeBeforeDrafting -= dt;
            if (mTimeBeforeDrafting <= 0.0f && mNextDraftingTeam == -1)
            {
                AdvanceDraftTeam();
            }
        }
        if (mNextDraftingTeam >= 0 && mNextDraftingTeam < mTeamCount
            && HasDisconnectedPlayer(mNextDraftingTeam))
        {
            AdvanceDraftTeam();
        }
        break;
    case NET_DRAFT_SIDEKICKS:
    {
        if (mTimeToChangeDrafters > 0.0f)
        {
            mTimeToChangeDrafters -= dt;
        }
        bool allTeamsFinished = true;
        for (int team = 0; team < mTeamCount; ++team)
        {
            if (!HasDisconnectedPlayer(team)
                && mTeams[team].mSidekick0 == -1)
            {
                allTeamsFinished = false;
            }
        }
        if (allTeamsFinished)
        {
            mState = NET_DRAFT_FINAL_COUNTDOWN;
            mFinalCountdown = s_fDefaultTimeFinalCountdown;
        }
        break;
    }
    case NET_DRAFT_FINAL_COUNTDOWN:
        if (mFinalCountdown > 0.0f)
        {
            mFinalCountdown -= dt;
            if (mFinalCountdown <= 0.0f)
            {
                gNetworkMessageRegistry->UnregisterReceiver(23);
                gNetworkMessageRegistry->UnregisterReceiver(24);

                bool disconnected = false;
                if (!g_pNetworkSession->mCupMode)
                {
                    for (int team = 0; team < mTeamCount; ++team)
                    {
                        if (HasDisconnectedPlayer(team))
                        {
                            disconnected = true;
                            break;
                        }
                    }
                }

                if (disconnected)
                {
                    mState = NET_DRAFT_DISCONNECTED;
                }
                else
                {
                    mState = NET_DRAFT_STARTED;
                    if (g_pNetworkSessionBase->GetMachineRoster()
                            ->GetLocalMachineIndex()
                        == 0)
                    {
                        if (g_pNetworkSession->mCupMode)
                        {
                            g_pNetworkSession
                                ->SendTournamentStartToEveryone();
                        }
                        else
                        {
                            g_pNetworkSession->SendGameStartToEveryone();
                        }
                    }
                }
            }
        }
        break;
    case NET_DRAFT_STARTED:
    {
        bool disconnected = false;
        if (!g_pNetworkSession->mCupMode)
        {
            for (int team = 0; team < mTeamCount; ++team)
            {
                if (HasDisconnectedPlayer(team))
                {
                    disconnected = true;
                    break;
                }
            }
        }
        if (disconnected)
        {
            mState = NET_DRAFT_DISCONNECTED;
        }
        break;
    }
    case NET_DRAFT_DISCONNECTED:
        break;
    default:
        break;
    }
}

void NetworkDraft::AdvanceDraftTeam()
{
    ++mNextDraftingTeam;
    if (mNextDraftingTeam >= mTeamCount)
    {
        mTimeToChangeDrafters = s_fDefaultTimeToChooseSidekicks;
        mState = NET_DRAFT_SIDEKICKS;
        return;
    }

    mTimeBeforeDrafting = s_fDefaultTimeToChangeDrafters;
    mCurrentDraftingTeam = mNextDraftingTeam;
    if (mCurrentDraftingTeam == mMyTeamIndex)
    {
        int captain = GetRandomAvailableCaptain();
        tDebugPrintManager::Print(DC_NETWORK, "Found initial captain choice %d\n", captain);
        mTeams[mCurrentDraftingTeam].mCaptain = captain;
    }
}

void NetworkDraft::UnregisterMessageReceivers()
{
    gNetworkMessageRegistry->UnregisterReceiver(23);
    gNetworkMessageRegistry->UnregisterReceiver(24);
}

int NetworkDraft::GetCurrentDraftingTeam() const
{
    return mNextDraftingTeam;
}

int NetworkDraft::GetRandomAvailableCaptain() const
{
    int attempts = 0;
    int index = (int)nlRandomf(0.0f, 12.0f, &nlDefaultSeed);
    if (index >= 12)
    {
        index = 0;
    }
    int captain = gCaptainSelectionOrder[index];
    while (IsCaptainTaken(captain) && attempts < 12)
    {
        ++attempts;
        ++index;
        if (index >= 12)
        {
            index = 0;
        }
        captain = gCaptainSelectionOrder[index];
    }
    return attempts < 12 ? captain : -1;
}

void NetworkDraft::SendCaptainChoice()
{
    NetMessageDraftPickedCaptain message;
    message.mTeamIndex = mCurrentDraftingTeam;
    message.mCaptain = GameInfoManager::Instance()->GetTeam(0);
    u8 buffer[0x20];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    tDebugPrintManager::Print(DC_NETWORK,
        "Sending NetworkDraftPickedCaptain team %d captain %d\n",
        message.mTeamIndex, message.mCaptain);
    SendToAllDraftPlayers(buffer, size);
}

void NetworkDraft::SendSidekickChoice()
{
    int teamIndex = mCurrentDraftingTeam;
    NetMessageDraftPickedSidekicks message;
    message.mTeamIndex = teamIndex;
    message.mSidekick0 = GameInfoManager::Instance()->GetSidekick(0, 0);
    message.mSidekick1 = GameInfoManager::Instance()->GetSidekick(0, 1);
    message.mSidekick2 = GameInfoManager::Instance()->GetSidekick(0, 2);
    u8 buffer[0x20];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    SendToAllDraftPlayers(buffer, size);
    gOnlineStartMatchmaking = 0;
    gOnlineSidekickChoiceSent = 1;
    if (IsOnlineRankedMatch())
    {
        GameSceneManager::Instance()->Push(
            (SceneList)0x31, SCREEN_NOTHING, true);
    }
    else
    {
        GameSceneManager::Instance()->Push(
            (SceneList)0x32, SCREEN_NOTHING, true);
    }
}

bool NetworkDraft::IsCaptainTaken(int captain) const
{
    if (captain == -1)
    {
        return true;
    }
    for (int team = 0; team < mTeamCount; ++team)
    {
        if (mTeams[team].mCaptain == captain)
        {
            return true;
        }
    }
    return false;
}

NetworkDraftTeam* NetworkDraft::GetDraftTeam(int team)
{
    return &mTeams[team];
}

NetworkDraftTeam* NetworkDraft::FindDraftTeamByPeerIndex(int peerIndex)
{
    for (int team = 0; team < mTeamCount; ++team)
    {
        for (int player = 0; player < mTeams[team].mPlayerCount; ++player)
        {
            if (mTeams[team].mPlayers[player].mPeerIndex == peerIndex)
            {
                return &mTeams[team];
            }
        }
    }
    return 0;
}

int NetworkDraft::ProcessMessage(NetworkMessage* message)
{
    int type = message->GetType();
    if (type == 23)
    {
        NetMessageDraftPickedCaptain* pickedCaptain =
            (NetMessageDraftPickedCaptain*)message;
        if (mState != NET_DRAFT_CAPTAINS)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Ignoring ReceivedDraftPickedCaptain because in draft state %d\n",
                mState);
        }
        else if ((s8)pickedCaptain->mTeamIndex != mNextDraftingTeam)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Ignoring ReceivedDraftPickedCaptain because expected update from team %d but got from %d\n",
                mNextDraftingTeam, (s8)pickedCaptain->mTeamIndex);
        }
        else if (mNextDraftingTeam < 0 || mNextDraftingTeam >= mTeamCount)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Ignoring ReceivedDraftPickedCaptain because m_nCurrentDraftingTeam is bad value %d\n",
                mNextDraftingTeam);
        }
        else
        {
            mTeams[mNextDraftingTeam].mCaptain = pickedCaptain->mCaptain;
            AdvanceDraftTeam();
        }
    }
    else if (type == 24)
    {
        NetMessageDraftPickedSidekicks* pickedSidekicks =
            (NetMessageDraftPickedSidekicks*)message;
        if (mState != NET_DRAFT_CAPTAINS && mState != NET_DRAFT_SIDEKICKS)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Ignoring ReceivedDraftPickedSidekicks because in draft state %d\n",
                mState);
        }
        else if ((s8)pickedSidekicks->mTeamIndex < 0
            || (s8)pickedSidekicks->mTeamIndex >= mTeamCount)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Ignoring ReceivedDraftPickedSidekicks because pDraftPickedSidekicks m_nMyTeamIndex is bad value %d\n",
                (s8)pickedSidekicks->mTeamIndex);
        }
        else
        {
            NetworkDraftTeam& team = mTeams[(s8)pickedSidekicks->mTeamIndex];
            team.mSidekick0 = pickedSidekicks->mSidekick0;
            team.mSidekick1 = pickedSidekicks->mSidekick1;
            team.mSidekick2 = pickedSidekicks->mSidekick2;
        }
    }
    return 1;
}

void NetworkDraft::SendToAllDraftPlayers(void* data, int size)
{
    if (g_pNetworkSessionBase == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "No lobby found, cannot send message of size %d to all machines in draft\n",
            size);
        return;
    }

    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    NetworkSocket* socket = g_pNetworkSessionBase->GetDirectSocket();
    for (int team = 0; team < mTeamCount; ++team)
    {
        for (int player = 0; player < mTeams[team].mPlayerCount; ++player)
        {
            NetworkDraftPlayer& draftPlayer = mTeams[team].mPlayers[player];
            if (draftPlayer.mGuest)
            {
                continue;
            }
            u32 aid = roster->GetMachineAid(draftPlayer.mPeerIndex);
            if (aid == 0xFFFFFFFF)
            {
                socket->Receive(data, size);
            }
            else if (aid == 0)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Warning: Cannot send message to draft team %d player %d of size %d - no connection\n",
                    team, player, size);
            }
            else
            {
                socket->Send(aid, data, size, true);
            }
        }
    }
}
