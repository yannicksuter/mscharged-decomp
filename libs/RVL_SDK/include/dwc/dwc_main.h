#pragma once

#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum DWCState
    {
        DWC_STATE_INIT,
        DWC_STATE_AVAILABLE_CHECK,
        DWC_STATE_LOGIN,
        DWC_STATE_ONLINE,
        DWC_STATE_UPDATE_SERVERS,
        DWC_STATE_MATCHING,
        DWC_STATE_CONNECTED
    } DWCState;

    typedef void (*DWCLoginCallback)(int error, int profileID, void* param);
    typedef void (*DWCConnectionClosedCallback)(int error, BOOL isLocal,
        BOOL isServer, u8 aid, int index, void* param);

    DWCState DWC_GetState(void);
    BOOL DWC_SetConnectionClosedCallback(DWCConnectionClosedCallback callback,
        void* param);
    int DWC_GetNumConnectionHost(void);
    u8 DWC_GetMyAID(void);
    int DWC_GetAIDList(u8** aidList);
    BOOL DWC_IsValidAID(u8 aid);

#ifdef __cplusplus
}
#endif
