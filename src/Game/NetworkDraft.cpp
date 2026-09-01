#include "Game/NetworkDraft.h"

#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/TweakValue.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <stdlib.h>
#include <string.h>

extern "C" int fn_8004F594(int channel, const char* format, ...);
extern "C" u32 fn_8032C830(void* codec, void* message, void* buffer, int size);
extern "C" void fn_8032CA1C(
    void* codec, int type, UnidentifiedNetworkMessageReceiver* receiver);
extern "C" void fn_8032CA2C(void* codec, int type);
extern "C" int fn_8025BD88();

extern int lbl_80519920[12];
extern void* lbl_806E2100;

static NetworkDraft* sNetworkDraft;

static float s_fDefaultTimeToWaitBeforeDrafting = 5.0f;
static float s_fDefaultTimeToChangeDrafters = 20.0f;
static float s_fDefaultTimeToChooseSidekicks = 20.0f;
static float s_fDefaultTimeFinalCountdown = 5.0f;

static TweakValueImpl_804F4DC8 sDefaultTimeToWaitBeforeDraftingTweak(
    "s_fDefaultTimeToWaitBeforeDrafting", "Network/Draft",
    &s_fDefaultTimeToWaitBeforeDrafting);
static TweakValueImpl_804F4DC8 sDefaultTimeToChangeDraftersTweak(
    "s_fDefaultTimeToChangeDrafters", "Network/Draft",
    &s_fDefaultTimeToChangeDrafters);
static TweakValueImpl_804F4DC8 sDefaultTimeToChooseSidekicksTweak(
    "s_fDefaultTimeToChooseSidekicks", "Network/Draft",
    &s_fDefaultTimeToChooseSidekicks);
static TweakValueImpl_804F4DC8 sDefaultTimeFinalCountdownTweak(
    "s_fDefaultTimeFinalCountdown", "Network/Draft",
    &s_fDefaultTimeFinalCountdown);

void NetMessageDraft::Serialize(UnidentifiedMessageSerializer* serializer)
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
        serializer->Transfer(&mEntries[i].mUnidentified7F,
            sizeof(mEntries[i].mUnidentified7F));
    }
}

void NetworkMessageType22_8050B7B4::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mEntry.mHead, sizeof(mEntry.mHead));
    serializer->Transfer(&mEntry.mUnidentified18, sizeof(mEntry.mUnidentified18));
    serializer->Transfer(mEntry.mName, sizeof(mEntry.mName));
    serializer->Transfer(mEntry.mUnidentified32, sizeof(mEntry.mUnidentified32));
    serializer->Transfer(&mEntry.mIndex, sizeof(mEntry.mIndex));
    serializer->Transfer(
        &mEntry.mUnidentified7F, sizeof(mEntry.mUnidentified7F));
}

void NetMessageDraftPickedCaptain::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mTeamIndex, sizeof(mTeamIndex));
    serializer->Transfer(&mCaptain, sizeof(mCaptain));
}

void NetMessageDraftPickedSidekicks::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mTeamIndex, sizeof(mTeamIndex));
    serializer->Transfer(&mSidekick0, sizeof(mSidekick0));
    serializer->Transfer(&mSidekick1, sizeof(mSidekick1));
    serializer->Transfer(&mSidekick2, sizeof(mSidekick2));
}

void NetworkMessageType25_8050B778::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
    serializer->Transfer(&mUnidentified09, sizeof(mUnidentified09));
    serializer->Transfer(&mUnidentified0A, sizeof(mUnidentified0A));
    serializer->Transfer(&mUnidentified0B, sizeof(mUnidentified0B));
}

void NetMessageCheckConnection::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    for (int i = 0; i < 2; ++i)
    {
        serializer->Transfer(&mUnidentified08[i], sizeof(mUnidentified08[i]));
    }
}

void NetworkMessageType27_8050B750::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
    serializer->Transfer(&mUnidentified09, sizeof(mUnidentified09));
}

NetworkMessageType22_8050B7B4::~NetworkMessageType22_8050B7B4() { }
NetMessageDraftPickedCaptain::~NetMessageDraftPickedCaptain() { }
NetMessageDraftPickedSidekicks::~NetMessageDraftPickedSidekicks() { }
NetworkMessageType25_8050B778::~NetworkMessageType25_8050B778() { }
NetworkMessageType27_8050B750::~NetworkMessageType27_8050B750() { }

int NetworkMessageType27_8050B750::GetType() { return 27; }
int NetMessageCheckConnection::GetType() { return 26; }
int NetworkMessageType25_8050B778::GetType() { return 25; }
int NetMessageDraftPickedSidekicks::GetType() { return 24; }
int NetMessageDraftPickedCaptain::GetType() { return 23; }
int NetworkMessageType22_8050B7B4::GetType() { return 22; }
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
    mCurrentDraftingTeam = -1;
    mCurrentDraftingPeer = -1;
    mCurrentDrafterIsLocal = false;
    mSideToTeam[0] = -1;
    mSideToTeam[1] = -1;
    mSideDrafted[0] = false;
    mSideDrafted[1] = false;
    mTeamCount = 0;
    mNextDraftingTeam = -1;
    mTimeBeforeDrafting = s_fDefaultTimeToWaitBeforeDrafting;
    mTimeToChangeDrafters = s_fDefaultTimeToChooseSidekicks;
    mFinalCountdown = s_fDefaultTimeFinalCountdown;
}

void NetworkDraft::BeginSortedDraft(NetMessageDraft* message)
{
    fn_8032CA1C(lbl_806E2100, 23, this);
    fn_8032CA1C(lbl_806E2100, 24, this);
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

    fn_8004F594(16, "Starting Draft num Teams %d\n", mTeamCount);
    for (int teamIndex = 0; teamIndex < mTeamCount; ++teamIndex)
    {
        NetworkDraftTeam& team = mTeams[teamIndex];
        const UnidentifiedDraftEntry& entry = message->mEntries[teamIndex];
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
    fn_8004F594(16, "Sorted Draft MyTeamIndex %d MyMachineIndex %d\n",
        mMyTeamIndex, mLocalMachineIndex);
    for (int teamIndex = 0; teamIndex < mTeamCount; ++teamIndex)
    {
        char name[12];
        nlWcsToStr(mTeams[teamIndex].mPlayers[0].mName, name, 11);
        fn_8004F594(16, "%s Rank %d. %d-%d MyPeerIndex %d\n", name,
            mTeams[teamIndex].mPlayers[0].mHead.mUnidentified04[0],
            mTeams[teamIndex].mPlayers[0].mHead.mUnidentified04[1],
            mTeams[teamIndex].mPlayers[0].mHead.mUnidentified04[2],
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
    fn_8032CA1C(lbl_806E2100, 23, this);
    fn_8032CA1C(lbl_806E2100, 24, this);
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
        const UnidentifiedDraftEntry& entry = message->mEntries[entryIndex];
        int playerCount = entry.mUnidentified7F ? 2 : 1;
        for (int playerIndex = 0; playerIndex < playerCount; ++playerIndex)
        {
            int teamIndex = message->mUnidentified0B.mData[entryIndex * 2 + playerIndex];
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
            player.mUnidentified81 = playerIndex == 1;
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
    int leftRank = leftTeam->mPlayers[0].mHead.mUnidentified04[0];
    int rightRank = rightTeam->mPlayers[0].mHead.mUnidentified04[0];
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
        fn_8004F594(16, "Found initial captain choice %d\n", captain);
        mTeams[mCurrentDraftingTeam].mCaptain = captain;
    }
}

void NetworkDraft::UnregisterMessageReceivers()
{
    fn_8032CA2C(lbl_806E2100, 23);
    fn_8032CA2C(lbl_806E2100, 24);
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
    int captain = lbl_80519920[index];
    while (IsCaptainTaken(captain) && attempts < 12)
    {
        ++attempts;
        ++index;
        if (index >= 12)
        {
            index = 0;
        }
        captain = lbl_80519920[index];
    }
    return attempts < 12 ? captain : -1;
}

void NetworkDraft::SendCaptainChoice()
{
    NetMessageDraftPickedCaptain message;
    message.mTeamIndex = mCurrentDraftingTeam;
    message.mCaptain = mTeams[mCurrentDraftingTeam].mCaptain;
    u8 buffer[0x20];
    int size = fn_8032C830(lbl_806E2100, &message, buffer, sizeof(buffer));
    fn_8004F594(16,
        "Sending NetworkDraftPickedCaptain team %d captain %d\n",
        (s8)message.mTeamIndex, message.mCaptain);
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
    int size = fn_8032C830(lbl_806E2100, &message, buffer, sizeof(buffer));
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

int NetworkDraft::ReceiverVirtual00(UnidentifiedNetworkMessage* message)
{
    int type = message->GetType();
    if (type == 23)
    {
        NetMessageDraftPickedCaptain* pickedCaptain =
            (NetMessageDraftPickedCaptain*)message;
        if (mState != NET_DRAFT_CAPTAINS)
        {
            fn_8004F594(16,
                "Ignoring ReceivedDraftPickedCaptain because in draft state %d\n",
                mState);
        }
        else if ((s8)pickedCaptain->mTeamIndex != mNextDraftingTeam)
        {
            fn_8004F594(16,
                "Ignoring ReceivedDraftPickedCaptain because expected update from team %d but got from %d\n",
                mNextDraftingTeam, (s8)pickedCaptain->mTeamIndex);
        }
        else if (mNextDraftingTeam < 0 || mNextDraftingTeam >= mTeamCount)
        {
            fn_8004F594(16,
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
            fn_8004F594(16,
                "Ignoring ReceivedDraftPickedSidekicks because in draft state %d\n",
                mState);
        }
        else if ((s8)pickedSidekicks->mTeamIndex < 0
            || (s8)pickedSidekicks->mTeamIndex >= mTeamCount)
        {
            fn_8004F594(16,
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
    if (lbl_806E20D8 == 0)
    {
        fn_8004F594(16,
            "No lobby found, cannot send message of size %d to all machines in draft\n",
            size);
        return;
    }

    UnidentifiedMachineRoster* roster = lbl_806E20D8->GetMachineRoster();
    NetworkSocket_801246E4* socket = lbl_806E20D8->GetDirectSocket();
    for (int team = 0; team < mTeamCount; ++team)
    {
        for (int player = 0; player < mTeams[team].mPlayerCount; ++player)
        {
            NetworkDraftPlayer& draftPlayer = mTeams[team].mPlayers[player];
            if (draftPlayer.mUnidentified81)
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
                fn_8004F594(16,
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
