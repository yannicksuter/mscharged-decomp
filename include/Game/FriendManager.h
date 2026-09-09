#ifndef GAME_FRIENDMANAGER_H
#define GAME_FRIENDMANAGER_H

#include "Game/DB/UserOptions.h"
#include "types.h"

enum EFriendStatus
{
    EFriendStatus_Initial_NotAvailable = 0,
    EFriendStatus_Initial_Available = 1,
    EFriendStatus_HostInvitingPlayer = 2,
    EFriendStatus_ClientDecliningHost = 3,
    EFriendStatus_ClientReceivedInvitation = 4,
};

struct FriendStatusPayload
{
    FriendStatusPayload()
    {
        mMagic[0] = 'S';
        mMagic[1] = 'C';
        mMagic[2] = '2';
        mStatus = EFriendStatus_Initial_NotAvailable;
    }

    /* 0x00 */ char mMagic[3];
    /* 0x03 */ u8 mStatus;
    /* 0x04 */ int mProfileId;
    /* 0x08 */ GameplaySettings mGameplaySettings;
    /* 0x24 */ CheatSettings mPowerupSettings;
    /* 0x30 */ u32 mNetworkVersion;
    /* 0x34 */ u8 mStadium;
    /* 0x35 */ u8 mPadding35[3];
}; // size: 0x38

class FriendManager
{
public:
    FriendManager();

    void Reset(bool setInstance);
    void GetOwnFriendKeyString(u16* output);
    bool AddFriendKey(unsigned long long friendKey, int* error);
    void HandleFriendStatus(
        int index, u8 status, const char* statusString, void* param);
    FriendStatusPayload* GetFriendStatusPayload(int index);
    bool FindHostInvitation();
    bool ValidateHostInvitation();
    bool HasFriendDeclined();
    int GetFriendInvitationResponse();
    void Update(float dt);
    void SynchronizeFriends();
    void SetOwnStatusInitial(int available);
    void SetOwnStatusDecline(int index);
    void SetOwnStatusReceivedInvitation(int index);
    void SetOwnStatusHostInvitingPlayer(int index,
        const GameplaySettings* gameplaySettings,
        const CheatSettings* cheatSettings, u8 value);
    void DeleteFriend(int index);
    int CountFriends();
    int CountBuddies();
    void SetOwnStatusAvailable();

    int GetHostInvitationIndex() const { return mHostInvitationIndex; }
    FriendStatusPayload* GetOwnStatus() { return &mOwnStatus; }
    int GetFriendStatusIndex() const { return mFriendStatusIndex; }

    /* 0x000 */ float mUpdateTime;
    /* 0x004 */ bool mFriendListChanged;
    /* 0x005 */ u8 mPadding005[3];
    /* 0x008 */ int mHostInvitationIndex;
    /* 0x00C */ int mReturnScene;
    /* 0x010 */ int mPreviousRankedMode;
    /* 0x014 */ int mFriendStatusIndex;
    /* 0x018 */ u16 mFriendCodeInput[16];
    /* 0x038 */ FriendStatusPayload mOwnStatus;
    /* 0x070 */ FriendStatusPayload mFriendStatus[64];
    /* 0xE70 */ bool mFriendStatusChanged[64];
}; // size: 0xEB0

extern FriendManager* g_pFriendManagerInstance;
extern FriendManager* g_pFriendManager;

FriendManager* GetFriendManager();
void FormatFriendKey(unsigned long long friendKey, u16* output);

#endif // GAME_FRIENDMANAGER_H
