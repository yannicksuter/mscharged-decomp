#pragma once

#include <dwc/dwc_match_fwd.h>
#include <dwc/dwc_account_fwd.h>
#include <dwc/dwc_login_fwd.h>

// Scalar-only APIs usable without importing the SDK's integer typedefs.
#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*DWCConnectionClosedCallback)(int error, int isLocal,
        int isServer, unsigned char aid, int index, void* param);

    int DWC_SetConnectionClosedCallback(DWCConnectionClosedCallback callback,
        void* param);
#if defined(VERSION_R4QK01)
    int DWC_ConnectToAnybodyAsync(int topology, unsigned char numEntry, const char* addFilter,
        DWCMatchedSCCallback matchedCallback, void* matchedParam,
        DWCNewClientCallback newClientCallback, void* newClientParam,
        DWCEvalPlayerCallback evalCallback, void* evalParam,
        DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam);
    int DWC_SetupGameServer(int topology, unsigned char maxEntry,
        DWCMatchedSCCallback matchedCallback, void* matchedParam,
        DWCNewClientCallback newClientCallback, void* newClientParam,
        DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam);
    int DWC_ConnectToGameServerAsync(int topology, int serverIndex,
        DWCMatchedSCCallback matchedCallback, void* matchedParam,
        DWCNewClientCallback newClientCallback, void* newClientParam,
        DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam);
#else
    int DWC_ConnectToAnybodyAsync(unsigned char numEntry, const char* addFilter,
        DWCMatchedCallback matchedCallback, void* matchedParam,
        DWCEvalPlayerCallback evalCallback, void* evalParam);
    int DWC_SetupGameServer(unsigned char maxEntry,
        DWCMatchedSCCallback matchedCallback, void* matchedParam,
        DWCNewClientCallback newClientCallback, void* newClientParam);
    int DWC_ConnectToGameServerAsync(int serverIndex,
        DWCMatchedSCCallback matchedCallback, void* matchedParam,
        DWCNewClientCallback newClientCallback, void* newClientParam);
#endif

    typedef void* DWCFriendsMatchControl;

    void DWC_InitFriendsMatch(DWCFriendsMatchControl* dwccnt,
        DWCUserData* userdata, int productID, const char* gameName,
        const char* secretKey, int sendBufSize, int recvBufSize,
        DWCFriendData* friendList, int friendListLen);
    int DWC_LoginAsync(const unsigned short* ingamesn, const char* reserved,
        DWCLoginCallback callback, void* param);

    void DWC_ShutdownFriendsMatch(void);
    void DWC_ProcessFriendsMatch(void);
    int DWC_CloseAllConnectionsHard(void);
#if !defined(VERSION_R4QK01)
    int DWC_CloseConnectionHard(unsigned char aid);
#endif
    int DWC_GetNumConnectionHost(void);
    unsigned char DWC_GetMyAID(void);
    int DWC_GetAIDList(unsigned char** aidList);
    int DWC_GetLastSocketError(void);
    int DWC_CloseConnectionsAsync(void);

#ifdef __cplusplus
}
#endif
