#include "Game/NetworkMessageRegistry.h"
#include "Game/NetworkDraft.h"
#include "Game/Sys/debug.h"

#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
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
    serializer->Transfer(&mUnidentified0B, sizeof(mUnidentified0B));
    for (int i = 0; i < 8; ++i)
    {
        serializer->Transfer(&mEntries[i].mHead, sizeof(mEntries[i].mHead));
        serializer->Transfer(
            &mEntries[i].mUnidentified18, sizeof(mEntries[i].mUnidentified18));
        serializer->Transfer(mEntries[i].mName, sizeof(mEntries[i].mName));
        serializer->Transfer(
            mEntries[i].mUnidentified32, sizeof(mEntries[i].mUnidentified32));
        serializer->Transfer(&mEntries[i].mIndex, sizeof(mEntries[i].mIndex));
        serializer->Transfer(&mEntries[i].mGuestEnabled,
            sizeof(mEntries[i].mGuestEnabled));
    }
}

void NetMessageDraftMachineInfo::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mEntry.mHead, sizeof(mEntry.mHead));
    serializer->Transfer(&mEntry.mUnidentified18, sizeof(mEntry.mUnidentified18));
    serializer->Transfer(mEntry.mName, sizeof(mEntry.mName));
    serializer->Transfer(mEntry.mUnidentified32, sizeof(mEntry.mUnidentified32));
    serializer->Transfer(&mEntry.mIndex, sizeof(mEntry.mIndex));
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

NetMessageDraftMachineInfo::~NetMessageDraftMachineInfo() { }
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
    mCurrentDrafterIsLocal = false;
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
    mCurrentDrafterIsLocal = false;
    mSideToTeam[0] = -1;
    mSideToTeam[1] = -1;
    mSideDrafted[0] = false;
    mSideDrafted[1] = false;

    tDebugPrintManager::Print(DC_NETWORK, "Starting Draft num Teams %d\n", mTeamCount);
    for (int teamIndex = 0; teamIndex < mTeamCount; ++teamIndex)
    {
        NetworkDraftTeam& team = mTeams[teamIndex];
        const NetworkDraftMachineInfo& entry = message->mEntries[teamIndex];
        team = NetworkDraftTeam();
        team.mPlayerCount = 1;
        NetworkDraftPlayer& player = team.mPlayers[0];
        player.mHead = entry.mHead;
        int character = 0;
        for (; character < 10; ++character)
        {
            player.mName[character] = entry.mName[character];
            if (entry.mName[character] == 0)
            {
                break;
            }
        }
        player.mName[character] = 0;
        memcpy(player.mData, entry.mUnidentified32, sizeof(player.mData));
        player.mPeerIndex = (s8)entry.mIndex;
    }

    qsort(mTeams, mTeamCount, sizeof(NetworkDraftTeam), CompareDraftTeams);
    for (int teamIndex = 0; teamIndex < mTeamCount; ++teamIndex)
    {
        if (mTeams[teamIndex].mPlayers[0].mPeerIndex == mLocalMachineIndex)
        {
            mMyTeamIndex = teamIndex;
        }
    }
    tDebugPrintManager::Print(DC_NETWORK, "Sorted Draft MyTeamIndex %d MyMachineIndex %d\n",
        mMyTeamIndex, mLocalMachineIndex);
    for (int teamIndex = 0; teamIndex < mTeamCount; ++teamIndex)
    {
        char name[12];
        nlWcsToStr(mTeams[teamIndex].mPlayers[0].mName, name, 11);
        tDebugPrintManager::Print(DC_NETWORK,
            "%s Rank %d. %d-%d MyPeerIndex %d\n", name,
            mTeams[teamIndex].mPlayers[0].mHead.mScore,
            mTeams[teamIndex].mPlayers[0].mHead.mDisplayRank,
            mTeams[teamIndex].mPlayers[0].mHead.mWins,
            mTeams[teamIndex].mPlayers[0].mPeerIndex);
    }
    mNextDraftingTeam = -1;
    mTimeBeforeDrafting = s_fDefaultTimeToWaitBeforeDrafting;
    mTimeToChangeDrafters = s_fDefaultTimeToChooseSidekicks;
    mFinalCountdown = s_fDefaultTimeFinalCountdown;
    mState = NET_DRAFT_CAPTAINS;
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
    mCurrentDrafterIsLocal = false;
    mTeams[0] = NetworkDraftTeam();
    mTeams[1] = NetworkDraftTeam();

    for (int entryIndex = 0; entryIndex < message->mMachineCount; ++entryIndex)
    {
        const NetworkDraftMachineInfo& entry = message->mEntries[entryIndex];
        int playerCount = entry.mGuestEnabled ? 2 : 1;
        for (int playerIndex = 0; playerIndex < playerCount; ++playerIndex)
        {
            int teamIndex = message->mUnidentified0B.mData[entryIndex][playerIndex];
            if (teamIndex < 0 || teamIndex >= mTeamCount)
            {
                continue;
            }
            NetworkDraftTeam& team = mTeams[teamIndex];
            NetworkDraftPlayer& player = team.mPlayers[team.mPlayerCount++];
            player.mHead = entry.mHead;
            int character = 0;
            for (; character < 10; ++character)
            {
                player.mName[character] = entry.mName[character];
                if (entry.mName[character] == 0)
                {
                    break;
                }
            }
            player.mName[character] = 0;
            memcpy(player.mData, entry.mUnidentified32, sizeof(player.mData));
            player.mPeerIndex = (s8)entry.mIndex;
            player.mGuest = playerIndex == 1;
        }
    }
    AssignDraftSides();
    mNextDraftingTeam = -1;
    mTimeBeforeDrafting = s_fDefaultTimeToWaitBeforeDrafting;
    mTimeToChangeDrafters = s_fDefaultTimeToChooseSidekicks;
    mFinalCountdown = s_fDefaultTimeFinalCountdown;
    mState = NET_DRAFT_CAPTAINS;
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
    switch (mState)
    {
    case NET_DRAFT_CAPTAINS:
        if (mTimeBeforeDrafting > 0.0f)
        {
            mTimeBeforeDrafting -= dt;
        }
        if (mTimeBeforeDrafting <= 0.0f && mNextDraftingTeam == -1)
        {
            AdvanceDraftTeam();
        }
        if (mNextDraftingTeam >= 0 && mNextDraftingTeam < mTeamCount
            && HasDisconnectedPlayer(mNextDraftingTeam))
        {
            AdvanceDraftTeam();
        }
        break;
    case NET_DRAFT_SIDEKICKS:
        if (mTimeToChangeDrafters > 0.0f)
        {
            mTimeToChangeDrafters -= dt;
        }
        if (mTimeToChangeDrafters <= 0.0f)
        {
            mState = NET_DRAFT_FINAL_COUNTDOWN;
            mFinalCountdown = s_fDefaultTimeFinalCountdown;
        }
        break;
    case NET_DRAFT_FINAL_COUNTDOWN:
        if (mFinalCountdown > 0.0f)
        {
            mFinalCountdown -= dt;
        }
        if (mFinalCountdown <= 0.0f)
        {
            UnregisterMessageReceivers();
            mState = NET_DRAFT_STARTED;
        }
        break;
    case NET_DRAFT_STARTED:
        for (int team = 0; team < mTeamCount; ++team)
        {
            if (HasDisconnectedPlayer(team))
            {
                mState = NET_DRAFT_DISCONNECTED;
                break;
            }
        }
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
    NetMessageDraftPickedSidekicks message;
    message.mTeamIndex = mCurrentDraftingTeam;
    NetworkDraftTeam& team = mTeams[mCurrentDraftingTeam];
    message.mSidekick0 = team.mSidekick0;
    message.mSidekick1 = team.mSidekick1;
    message.mSidekick2 = team.mSidekick2;
    u8 buffer[0x20];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    SendToAllDraftPlayers(buffer, size);
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
