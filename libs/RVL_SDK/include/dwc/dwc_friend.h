#pragma once

#include <dwc/dwc_common.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*DWCBuddyFriendCallback)(int index, void* param);
    typedef void (*DWCFriendStatusCallback)(int index, u8 status,
        const char* statusString, void* param);
    typedef void (*DWCUpdateServersCallback)(int error, BOOL isChanged,
        void* param);
    typedef void (*DWCDeleteFriendListCallback)(int deletedIndex,
        int sourceIndex, void* param);

    typedef enum DWCFriendState
    {
        DWC_FRIEND_STATE_INIT,
        DWC_FRIEND_STATE_PERS_LOGIN,
        DWC_FRIEND_STATE_LOGON,
        DWC_FRIEND_STATE_NUM
    } DWCFriendState;

    enum
    {
        DWC_BUDDY_UPDATE_STATE_WAIT,
        DWC_BUDDY_UPDATE_STATE_CHECK,
        DWC_BUDDY_UPDATE_STATE_PSEARCH,
        DWC_BUDDY_UPDATE_STATE_COMPLETE,
        DWC_BUDDY_UPDATE_STATE_NUM
    };

    BOOL DWC_SetOwnStatusData(const char* statusData, u32 size);
    void DWC_DeleteBuddyFriendData(DWCFriendData* friendData);
    BOOL DWC_SetBuddyFriendCallback(DWCBuddyFriendCallback callback, void* param);
    BOOL DWC_SetFriendStatusCallback(DWCFriendStatusCallback callback, void* param);
    BOOL DWC_UpdateServersAsync(const char* reserved,
        DWCUpdateServersCallback updateCallback, void* updateParam,
        DWCFriendStatusCallback statusCallback, void* statusParam,
        DWCDeleteFriendListCallback deleteCallback, void* deleteParam);
    void DWCi_StopFriendProcess(int error, int errorCode);
    void DWCi_UpdateServersAsync(const char* authToken,
        const char* partnerChallenge, DWCUpdateServersCallback updateCallback,
        void* updateParam, DWCFriendStatusCallback statusCallback,
        void* statusParam, DWCDeleteFriendListCallback deleteCallback,
        void* deleteParam);

#ifdef __cplusplus
}
#endif
