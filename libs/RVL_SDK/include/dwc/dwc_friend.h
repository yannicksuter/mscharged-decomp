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

    BOOL DWC_SetOwnStatusData(const char* statusData, u32 size);
    void DWC_DeleteBuddyFriendData(DWCFriendData* friendData);
    BOOL DWC_SetBuddyFriendCallback(DWCBuddyFriendCallback callback, void* param);
    BOOL DWC_SetFriendStatusCallback(DWCFriendStatusCallback callback, void* param);

#ifdef __cplusplus
}
#endif
