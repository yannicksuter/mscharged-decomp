#ifndef GAME_NETWORK_DRAFT_H
#define GAME_NETWORK_DRAFT_H

#include "Game/NetworkMessages.h"
#include "types.h"

#include <string.h>

struct NetworkDraftPlayer
{
    NetworkDraftPlayer()
        : mPeerIndex(-1)
        , mDisconnected(false)
        , mUnidentified81(false)
    {
        mName[0] = 0;
        memset(mData, 0, sizeof(mData));
    }

    /* 0x00 */ UnidentifiedDraftEntryHead mHead;
    /* 0x18 */ u16 mName[11];
    /* 0x2E */ u8 mData[0x4C];
    /* 0x7A */ u8 mPadding7A[2];
    /* 0x7C */ int mPeerIndex;
    /* 0x80 */ bool mDisconnected;
    /* 0x81 */ bool mUnidentified81;
    /* 0x82 */ u8 mPadding82[2];
}; // size: 0x84

struct NetworkDraftTeam
{
    NetworkDraftTeam()
        : mPlayerCount(0)
        , mCaptain(-1)
        , mSidekick0(-1)
        , mSidekick1(-1)
        , mSidekick2(-1)
    {
    }

    /* 0x000 */ int mPlayerCount;
    /* 0x004 */ int mCaptain;
    /* 0x008 */ int mSidekick0;
    /* 0x00C */ int mSidekick1;
    /* 0x010 */ int mSidekick2;
    /* 0x014 */ NetworkDraftPlayer mPlayers[3];
}; // size: 0x1A0

enum NetworkDraftState
{
    NET_DRAFT_IDLE = 0,
    NET_DRAFT_CAPTAINS = 1,
    NET_DRAFT_SIDEKICKS = 2,
    NET_DRAFT_FINAL_COUNTDOWN = 3,
    NET_DRAFT_STARTED = 4,
    NET_DRAFT_DISCONNECTED = 5,
};

class NetworkDraft : public UnidentifiedNetworkMessageReceiver
{
public:
    NetworkDraft() { Reset(true); }

    static void CreateInstance();
    static NetworkDraft* Instance();

    void Reset(bool initialize);
    void BeginSortedDraft(NetMessageDraft* message);
    void BeginTeamDraft(NetMessageDraft* message);
    void AssignDraftSides();
    static int CompareDraftTeams(const void* left, const void* right);
    bool HasDisconnectedPlayer(int team) const;
    void Update(float dt);
    void AdvanceDraftTeam();
    void UnregisterMessageReceivers();
    int GetCurrentDraftingTeam() const;
    int GetRandomAvailableCaptain() const;
    void SendCaptainChoice();
    void SendSidekickChoice();
    bool IsCaptainTaken(int captain) const;
    NetworkDraftTeam* GetDraftTeam(int team);
    NetworkDraftTeam* FindDraftTeamByPeerIndex(int peerIndex);
    virtual int ReceiverVirtual00(UnidentifiedNetworkMessage* message);
    void SendToAllDraftPlayers(void* data, int size);

    /* 0x004 */ NetworkDraftState mState;
    /* 0x008 */ int mLocalMachineIndex;
    /* 0x00C */ int mMyTeamIndex;
    /* 0x010 */ int mCurrentDraftingTeam;
    /* 0x014 */ int mCurrentDraftingPeer;
    /* 0x018 */ bool mCurrentDrafterIsLocal;
    /* 0x019 */ u8 mPadding019[3];
    /* 0x01C */ int mSideToTeam[2];
    /* 0x024 */ bool mSideDrafted[2];
    /* 0x026 */ u8 mPadding026[2];
    /* 0x028 */ int mTeamCount;
    /* 0x02C */ NetworkDraftTeam mTeams[8];
    /* 0xD2C */ int mNextDraftingTeam;
    /* 0xD30 */ float mTimeBeforeDrafting;
    /* 0xD34 */ float mTimeToChangeDrafters;
    /* 0xD38 */ float mFinalCountdown;
    /* 0xD3C */ NetMessageDraft mDraftMessage;
}; // size: 0x1150

#endif // GAME_NETWORK_DRAFT_H
