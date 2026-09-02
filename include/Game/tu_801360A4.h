#ifndef GAME_TU_801360A4_H
#define GAME_TU_801360A4_H

#include "Game/DB/UserOptions.h"
#include "types.h"

struct UnidentifiedFriendStatusPayload
{
    UnidentifiedFriendStatusPayload()
    {
        mMagic[0] = 'S';
        mMagic[1] = 'C';
        mMagic[2] = '2';
        mStatus = 0;
    }

    /* 0x00 */ char mMagic[3];
    /* 0x03 */ u8 mStatus;
    /* 0x04 */ int mProfileId;
    /* 0x08 */ GameplaySettings mGameplaySettings;
    /* 0x24 */ PowerupSettings mPowerupSettings;
    /* 0x30 */ u32 mUnidentified30;
    /* 0x34 */ u8 mUnidentified34;
    /* 0x35 */ u8 mPadding35[3];
}; // size: 0x38

class UnidentifiedFriendManager_801360A4
{
public:
    UnidentifiedFriendManager_801360A4();

    void Reset(bool setCallbackInstance);
    void GetOwnFriendKeyString_80136290(u16* output);
    bool AddFriendKey_801362FC(unsigned long long friendKey, int* error);
    void HandleFriendStatus_801365C4(
        int index, u8 status, const char* statusString, void* param);
    UnidentifiedFriendStatusPayload* GetFriendStatusPayload_80136A88(int index);
    bool FindHostInvitation_80136AB0();
    bool ValidateHostInvitation_80136BC0();
    bool HasFriendDeclined_80136C68();
    int GetFriendInvitationResponse_80136D30();
    void Update(float dt);
    void SynchronizeFriends_80136F30();
    void SetOwnStatusInitial_80136FA4(int available);
    void SetOwnStatusDecline_80137068(int index);
    void SetOwnStatusReceivedInvitation_80137118(int index);
    void SetOwnStatusHostInviting_801371C8(int index,
        const GameplaySettings* gameplaySettings,
        const PowerupSettings* powerupSettings, u8 value);
    void DeleteFriend_8013730C(int index);
    int CountFriends_801373A8();
    int CountBuddies_8013740C();
    void SetOwnStatusAvailable_801374A4();

    /* 0x000 */ float mUpdateTime;
    /* 0x004 */ bool mFriendListChanged;
    /* 0x005 */ u8 mPadding005[3];
    /* 0x008 */ int mHostInvitationIndex;
    /* 0x00C */ int mUnidentified00C;
    /* 0x010 */ int mUnidentified010;
    /* 0x014 */ int mFriendStatusIndex;
    /* 0x018 */ char mUnidentified018[0x20];
    /* 0x038 */ UnidentifiedFriendStatusPayload mOwnStatus;
    /* 0x070 */ UnidentifiedFriendStatusPayload mFriendStatus[64];
    /* 0xE70 */ bool mFriendStatusChanged[64];
}; // size: 0xEB0

extern UnidentifiedFriendManager_801360A4* lbl_806E1190;
extern UnidentifiedFriendManager_801360A4* lbl_806E1194;

#endif // GAME_TU_801360A4_H
