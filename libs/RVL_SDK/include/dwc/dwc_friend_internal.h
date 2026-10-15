#pragma once

#include <dwc/dwc_friend.h>
#include <dwc/dwc_datastorage.h>
#include <gamespy/GP/gp.h>
#include <revolution/os/OSTime.h>

typedef struct DWCFriendControl
{
    DWCFriendState state;
    GPConnection* pGpObj;
    u32 gpProcessCount;
    OSTime lastGpProcess;
    int friendListLen;
    DWCFriendData* friendList;
    u8 buddyUpdateIdx;
    u8 friendListChanged;
    u8 buddyUpdateState;
    u8 svUpdateComplete;
    u32 persCallbackLevel;
    int profileID;
    const u16* playerName;
    DWCUpdateServersCallback updateCallback;
    void* updateParam;
    DWCFriendStatusCallback statusCallback;
    void* statusParam;
    DWCDeleteFriendListCallback deleteCallback;
    void* deleteParam;
    DWCBuddyFriendCallback buddyCallback;
    void* buddyParam;
    DWCStorageLoginCallback persLoginCallback;
    void* persLoginParam;
    DWCSaveToServerCallback saveCallback;
    DWCLoadFromServerCallback loadCallback;
#if defined(VERSION_R4QK01)
    GPProfile* reverseBuddyTargets;
#endif
} DWCFriendControl;

#ifdef __cplusplus
extern "C"
{
#endif

    void DWCi_FriendInit(DWCFriendControl* friendcnt, GPConnection* pGpObj,
        const u16* playerName, DWCFriendData* friendList, int friendListLen);
    void DWCi_ShutdownFriend(void);

#ifdef __cplusplus
}
#endif
