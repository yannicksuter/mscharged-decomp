#include "Game/NetworkMessageRegistry.h"
#include "Game/NetworkDraft.h"
#include "Game/Sys/debug.h"

#include "Game/GameInfo.h"
#include "Game/GameSceneManager.h"
#include "Game/FE/CaptainSelectionOrder.h"
#include "Game/FE/FEAudio.h"
#include "Game/NetworkSession.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static NetworkDraft* sNetworkDraft;

static float s_fDefaultTimeToWaitBeforeDrafting = 5.0f;
static float s_fDefaultTimeToChangeDrafters = 20.0f;
static float s_fDefaultTimeToChooseSidekicks = 20.0f;
static float s_fDefaultTimeFinalCountdown = 5.0f;

static TweakFloatBinding sDefaultTimeToWaitBeforeDraftingTweak(
    "s_fDefaultTimeToWaitBeforeDrafting", "Network/Draft",
    &s_fDefaultTimeToWaitBeforeDrafting, true);
static TweakFloatBinding sDefaultTimeToChangeDraftersTweak(
    "s_fDefaultTimeToChangeDrafters", "Network/Draft",
    &s_fDefaultTimeToChangeDrafters, true);
static TweakFloatBinding sDefaultTimeToChooseSidekicksTweak(
    "s_fDefaultTimeToChooseSidekicks", "Network/Draft",
    &s_fDefaultTimeToChooseSidekicks, true);
static TweakFloatBinding sDefaultTimeFinalCountdownTweak(
    "s_fDefaultTimeFinalCountdown", "Network/Draft",
    &s_fDefaultTimeFinalCountdown, true);

void NetworkDraft::CreateInstance()
{
    sNetworkDraft = new (nlMalloc(sizeof(NetworkDraft), 8, false)) NetworkDraft;
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
    mDraftingMachines[0] = -1;
    mDraftingGuests[0] = false;
    mDraftingMachines[1] = -1;
    mDraftingGuests[1] = false;
    mNextDraftingTeam = -1;
    mTimeBeforeDrafting = s_fDefaultTimeToWaitBeforeDrafting;
    mTimeToChooseSidekicks = s_fDefaultTimeToChooseSidekicks;
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
    mDraftingMachines[0] = -1;
    mDraftingGuests[0] = false;
    mDraftingMachines[1] = -1;
    mDraftingGuests[1] = false;

    tDebugPrintManager::Print(DC_NETWORK, "Starting Draft num Teams %d\n", mTeamCount);
    for (int teamIndex = 0; teamIndex < mTeamCount; ++teamIndex)
    {
        mTeams[teamIndex].Reset();
        mTeams[teamIndex].mPlayerCount = 1;
        NetworkDraftPlayer& player = mTeams[teamIndex].mPlayers[0];
        const NetworkDraftMachineInfo& entry = message->mEntries[teamIndex];
        player.mStats = entry.mStats;
        nlStrNCpy(player.mName, entry.mName, 11);
        memcpy(player.mMiiData, entry.mMiiData, sizeof(player.mMiiData));
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
            player.mStats.mDisplayRank, player.mStats.mWins, player.mStats.mLosses,
            player.mPeerIndex);
    }
    mNextDraftingTeam = -1;
    mTimeBeforeDrafting = s_fDefaultTimeToWaitBeforeDrafting;
    mTimeToChooseSidekicks = s_fDefaultTimeToChooseSidekicks;
    mFinalCountdown = s_fDefaultTimeFinalCountdown;
    mState = NET_DRAFT_CAPTAINS;
    gOnlineStartMatchmaking = 0;
    GameSceneManager::Instance()->Push((SceneList)0x31, SCREEN_NOTHING, true);
}

struct DraftMachineCursor
{
    const NetworkDraftMachineInfo* mEntry;
};

static inline void CopyDraftPlayerName(NetworkDraftPlayer& player, const u16* name)
{
    nlStrNCpy(player.mName, name, sizeof(player.mName) / sizeof(player.mName[0]));
}

static inline void CopyDraftMachineInfo(NetworkDraftPlayer& player, const NetworkDraftMachineInfo& entry)
{
    player.mStats = entry.mStats;
    CopyDraftPlayerName(player, entry.mName);
    memcpy(player.mMiiData, entry.mMiiData, sizeof(player.mMiiData));
    player.mPeerIndex = (s8)entry.mMachineIndex;
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

    DraftMachineCursor cursor = { message->mEntries };
    for (int entryIndex = 0; entryIndex < message->mMachineCount; ++entryIndex)
    {
        int playerCount = cursor.mEntry->mGuestEnabled ? 2 : 1;
        for (int playerIndex = 0; playerIndex < playerCount; ++playerIndex)
        {
            int teamIndex = message->mPlayerSides.mData[entryIndex][playerIndex];
            NetworkDraftPlayer& player =
                mTeams[teamIndex].mPlayers[mTeams[teamIndex].mPlayerCount];
            CopyDraftMachineInfo(player, *cursor.mEntry);
            player.mGuest = playerIndex == 1;
            ++mTeams[teamIndex].mPlayerCount;
        }
        ++cursor.mEntry;
    }
    AssignDraftSides();
    mNextDraftingTeam = -1;
    mTimeBeforeDrafting = s_fDefaultTimeToWaitBeforeDrafting;
    mTimeToChooseSidekicks = s_fDefaultTimeToChooseSidekicks;
    mFinalCountdown = s_fDefaultTimeFinalCountdown;
    mState = NET_DRAFT_CAPTAINS;
    GameSceneManager::Instance()->Push((SceneList)0x32, SCREEN_FORWARD, true);
}

struct DraftSidePlayer
{
    void Reset() { machine = -1; guest = false; }

    s8 machine;
    bool guest;
};

struct DraftSideCursor
{
    int GetMachineIndex() const { return mMachineIndex; }
    void Advance() { ++mMachineIndex; }
    int mMachineIndex;
};

struct DraftSideRow
{
    int GetPrimary() const
    {
        return (s8)mDraftBytes[offsetof(NetworkDraft, mDraftMessage)
            + offsetof(NetMessageDraft, mPlayerSides)];
    }

    int GetGuest() const
    {
        return (s8)mDraftBytes[offsetof(NetworkDraft, mDraftMessage)
            + offsetof(NetMessageDraft, mPlayerSides) + 1];
    }

    DraftSideRow Next() const
    {
        DraftSideRow next = { mDraftBytes + sizeof(((NetworkDraftSides*)0)->mData[0]) };
        return next;
    }

    const char* mDraftBytes;
};

static inline void CollectDraftSidePlayers(const NetworkDraft& draft,
    int sideCounts[2], DraftSidePlayer sidePlayers[2][3])
{
    DraftSideRow current = { reinterpret_cast<const char*>(&draft) };
    DraftSideCursor cursor = DraftSideCursor();
    for (; cursor.GetMachineIndex() < draft.mDraftMessage.mMachineCount; cursor.Advance())
    {
        int side = current.GetPrimary();
        if (side != -1)
        {
            int count = sideCounts[side]++;
            DraftSidePlayer& player = sidePlayers[side][count];
            player.machine = cursor.GetMachineIndex();
            player.guest = false;
        }

        side = current.GetGuest();
        if (side != -1)
        {
            int count = sideCounts[side]++;
            DraftSidePlayer& player = sidePlayers[side][count];
            player.machine = cursor.GetMachineIndex();
            player.guest = true;
        }
        current = current.Next();
    }
}

void NetworkDraft::AssignDraftSides()
{
    bool usedMachines[4] = { false };
    int sideCounts[2] = { 0, 0 };
    DraftSidePlayer sidePlayers[2][3];
    for (int side = 0; side < 2; ++side)
        for (int player = 0; player < 3; ++player)
            sidePlayers[side][player].Reset();

    CollectDraftSidePlayers(*this, sideCounts, sidePlayers);

    int side = 0;
    if (sideCounts[1] == 1 && sideCounts[0] > 1)
    {
        side = 1;
    }

    mDraftingMachines[side] = sidePlayers[side][0].machine;
    mDraftingGuests[side] = sidePlayers[side][0].guest;
    usedMachines[mDraftingMachines[side]] = true;

    if (side == 0)
    {
        side = 1;
    }
    else if (side == 1)
    {
        side = 0;
    }

    for (int i = 0; i < sideCounts[side]; ++i)
    {
        if (!usedMachines[sidePlayers[side][i].machine])
        {
            mDraftingMachines[side] = sidePlayers[side][i].machine;
            mDraftingGuests[side] = sidePlayers[side][i].guest;
            usedMachines[mDraftingMachines[side]] = true;
            break;
        }
    }
}

int NetworkDraft::CompareDraftTeams(const void* left, const void* right)
{
    const NetworkDraftTeam* leftTeam = (const NetworkDraftTeam*)left;
    const NetworkDraftTeam* rightTeam = (const NetworkDraftTeam*)right;
    int leftRank = leftTeam->mPlayers[0].mStats.mScore;
    int rightRank = rightTeam->mPlayers[0].mStats.mScore;
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
        if (mTimeToChooseSidekicks > 0.0f)
        {
            mTimeToChooseSidekicks -= dt;
        }
        bool allTeamsFinished = true;
        for (int team = 0; team < mTeamCount; ++team)
        {
            if (!HasDisconnectedPlayer(team)
                && mTeams[team].mSidekicks[0] == -1)
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

static void StartCaptainChoice(NetworkDraft* draft)
{
    int captain = draft->GetRandomAvailableCaptain();
    tDebugPrintManager::Print(DC_NETWORK, "Found initial captain choice %d\n", captain);
    GameInfoManager::Instance()->SetTeam(0, captain);
    GameInfoManager::Instance()->SetTeam(1, captain);
    GameInfoManager::Instance()->ResetPlayingSides();
    FEAudio::PlayAnimAudioEvent(0x74572C29, 0, 0, true);
    GameSceneManager::Instance()->Push(SCENE_CHOOSE_CAPTAINS_DOMINATION, SCREEN_NOTHING, true);
}

void NetworkDraft::AdvanceDraftTeam()
{
    ++mNextDraftingTeam;
    if (mNextDraftingTeam >= mTeamCount)
    {
        mTimeToChooseSidekicks = s_fDefaultTimeToChooseSidekicks;
        mState = NET_DRAFT_SIDEKICKS;
        return;
    }

    mTimeBeforeDrafting = s_fDefaultTimeToChangeDrafters;
    if (IsOnlineRankedMatch())
    {
        if (mNextDraftingTeam != mMyTeamIndex)
            return;

        mCurrentDraftingTeam = mMyTeamIndex;
        StartCaptainChoice(this);
    }
    else
    {
        if (mNextDraftingTeam < 0)
            return;
        if (mNextDraftingTeam >= 2)
            return;
        if (mLocalMachineIndex != mDraftingMachines[mNextDraftingTeam])
            return;

        mCurrentDraftingTeam = mNextDraftingTeam;
        mCurrentDraftingPeer = mDraftingMachines[mNextDraftingTeam];
        mCurrentDrafterIsGuest = mDraftingGuests[mNextDraftingTeam];
        StartCaptainChoice(this);
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
    int index = (int)nlRandomf(0.0f, 12.0f, &nlDefaultSeed);
    for (int attempts = 0; attempts < 12; ++attempts, ++index)
    {
        if (index >= 12)
        {
            index = 0;
        }
        int captain = gCaptainSelectionOrder[index];
        if (!IsCaptainTaken(captain))
        {
            return captain;
        }
    }
    return -1;
}

void NetworkDraft::SendCaptainChoice()
{
    int teamIndex = mCurrentDraftingTeam;
    NetMessageDraftPickedCaptain message;
    message.mTeamIndex = teamIndex;
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
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    s8 machine = (s8)roster->MachineIdxFromConnection(message->mSource);
    if (machine < 0 || machine >= roster->GetMachineCount())
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Discarded message type %d because from unknown connection %x\n",
            (u8)message->GetType(), message->mSource);
        return 1;
    }

    switch ((u8)message->GetType())
    {
    case 23:
    {
        NetMessageDraftPickedCaptain* pickedCaptain =
            (NetMessageDraftPickedCaptain*)message;
        if (mState != NET_DRAFT_CAPTAINS)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Ignoring ReceivedDraftPickedCaptain because in draft state %d\n",
                mState);
        }
        else if (mNextDraftingTeam != (s8)pickedCaptain->mTeamIndex)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Ignoring ReceivedDraftPickedCaptain because expected update from team %d but got from %d\n",
                mNextDraftingTeam, (s8)pickedCaptain->mTeamIndex);
        }
        else if (mNextDraftingTeam >= 0 && mNextDraftingTeam < mTeamCount)
        {
            mTeams[mNextDraftingTeam].mCaptain = pickedCaptain->mCaptain;
            AdvanceDraftTeam();
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Ignoring ReceivedDraftPickedCaptain because m_nCurrentDraftingTeam is bad value %d\n",
                mNextDraftingTeam);
        }
        break;
    }
    case 24:
    {
        NetMessageDraftPickedSidekicks* pickedSidekicks =
            (NetMessageDraftPickedSidekicks*)message;
        if (mState != NET_DRAFT_CAPTAINS && mState != NET_DRAFT_SIDEKICKS)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Ignoring ReceivedDraftPickedSidekicks because in draft state %d\n",
                mState);
        }
        else if ((s8)pickedSidekicks->mTeamIndex >= 0
            && (s8)pickedSidekicks->mTeamIndex < mTeamCount)
        {
            mTeams[(s8)pickedSidekicks->mTeamIndex].mSidekicks[0] = pickedSidekicks->mSidekick0;
            mTeams[(s8)pickedSidekicks->mTeamIndex].mSidekicks[1] = pickedSidekicks->mSidekick1;
            mTeams[(s8)pickedSidekicks->mTeamIndex].mSidekicks[2] = pickedSidekicks->mSidekick2;
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Ignoring ReceivedDraftPickedSidekicks because pDraftPickedSidekicks m_nMyTeamIndex is bad value %d\n",
                (s8)pickedSidekicks->mTeamIndex);
        }
        break;
    }
    }
    return 1;
}

void NetworkDraft::SendToAllDraftPlayers(void* data, int size)
{
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    if (roster == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "No lobby found, cannot send message of size %d to all machines in draft\n",
            size);
        return;
    }

    for (int team = 0; team < mTeamCount; ++team)
    {
        NetworkDraftTeam& draftTeam = mTeams[team];
        for (int player = 0; player < draftTeam.mPlayerCount; ++player)
        {
            int peer = draftTeam.mPlayers[player].mPeerIndex;
            if (draftTeam.mPlayers[player].mGuest)
            {
                continue;
            }
            u32 aid = roster->GetMachineAid(peer);
            if (aid == 0xFFFFFFFF)
            {
                g_pNetworkSessionBase->GetDirectSocket()->Receive(data, size);
            }
            else if (aid == 0)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "Warning: Cannot send message to draft team %d player %d of size %d - no connection\n",
                    team, player, size);
            }
            else
            {
                g_pNetworkSessionBase->GetDirectSocket()->Send(aid, data, size, true);
            }
        }
    }
}
