#ifndef GAME_NETWORK_DRAFT_H
#define GAME_NETWORK_DRAFT_H

#include "Game/NetworkMessages.h"
#include "types.h"

#include <string.h>

struct NetworkDraftPlayer
{
    NetworkDraftPlayer()
    {
        mName[0] = 0;
        memset(mData, 0, sizeof(mData));
        mPeerIndex = -1;
        mDisconnected = false;
        mGuest = false;
    }

    void Reset()
    {
        mHead.Reset();
        mName[0] = 0;
        memset(mData, 0, sizeof(mData));
        mPeerIndex = -1;
        mDisconnected = false;
        mGuest = false;
    }

    /* 0x00 */ NetworkRankingMeta mHead;
    /* 0x18 */ u16 mName[11];
    /* 0x2E */ u8 mData[0x4C];
    /* 0x7A */ u8 mPadding7A[2];
    /* 0x7C */ int mPeerIndex;
    /* 0x80 */ bool mDisconnected;
    /* 0x81 */ bool mGuest;
    /* 0x82 */ u8 mPadding82[2];
}; // size: 0x84

struct NetworkDraftTeam
{
    NetworkDraftTeam()
    {
        Reset();
    }

    void Reset()
    {
        mCaptain = -1;
        mSidekicks[0] = -1;
        mSidekicks[1] = -1;
        mSidekicks[2] = -1;
        mPlayerCount = 0;
        mPlayers[0].Reset();
        mPlayers[1].Reset();
        mPlayers[2].Reset();
    }

    NetworkDraftPlayer* FindPlayer(int peer, bool guest)
    {
        for (int i = 0; i < mPlayerCount; ++i)
        {
            if (mPlayers[i].mPeerIndex == peer && mPlayers[i].mGuest == guest)
                return &mPlayers[i];
        }
        return 0;
    }

    /* 0x000 */ int mPlayerCount;
    /* 0x004 */ int mCaptain;
    /* 0x008 */ int mSidekicks[3];
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

class NetworkDraft : public NetworkMessageReceiver
{
public:
    NetworkDraft() { Reset(true); }

    static void CreateInstance();
    static NetworkDraft* Instance();

    NetMessageDraft& GetDraftMessage() { return mDraftMessage; }

    void Reset(bool initialize);
    void BeginSortedDraft(NetMessageDraft* message);
    void BeginTeamDraft(NetMessageDraft* message);
    void AssignDraftSides();
    static int CompareDraftTeams(const void* left, const void* right);
    bool HasDisconnectedPlayer(int team) const;
    void Update(float dt);
    bool fn_8022819C() const { return mState != NET_DRAFT_IDLE; }
    int fn_8022ED88()
    {
        if (mState != NET_DRAFT_SIDEKICKS)
            return -1;
        int countdown = (int)mTimeToChangeDrafters;
        if (countdown < 0)
            countdown = 0;
        return countdown;
    }
    int GetCountdown() const
    {
        int countdown = 0;
        switch (mState)
        {
        case NET_DRAFT_IDLE:
            return -1;
        case NET_DRAFT_CAPTAINS:
            countdown = (int)mTimeBeforeDrafting;
            break;
        case NET_DRAFT_SIDEKICKS:
            countdown = (int)mTimeToChangeDrafters;
            break;
        case NET_DRAFT_FINAL_COUNTDOWN:
            countdown = (int)mFinalCountdown;
            break;
        case NET_DRAFT_STARTED:
        case NET_DRAFT_DISCONNECTED:
            countdown = 0;
            break;
        }
        if (countdown < 0)
            countdown = 0;
        return countdown;
    }

    void AdvanceDraftTeam();
    void UnregisterMessageReceivers();
    int GetCurrentDraftingTeam() const;
    int GetLocalMachineIndex() const { return mLocalMachineIndex; }
    int GetRandomAvailableCaptain() const;
    void SendCaptainChoice();
    void SendSidekickChoice();
    bool IsCaptainTaken(int captain) const;
    NetworkDraftTeam* GetDraftTeam(int team);
    NetworkDraftTeam* FindDraftTeamByPeerIndex(int peerIndex);
    virtual int ProcessMessage(NetworkMessage* message);
    void SendToAllDraftPlayers(void* data, int size);

    /* 0x004 */ NetworkDraftState mState;
    /* 0x008 */ int mLocalMachineIndex;
    /* 0x00C */ int mMyTeamIndex;
    /* 0x010 */ int mCurrentDraftingTeam;
    /* 0x014 */ int mCurrentDraftingPeer;
    /* 0x018 */ bool mCurrentDrafterIsGuest;
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
