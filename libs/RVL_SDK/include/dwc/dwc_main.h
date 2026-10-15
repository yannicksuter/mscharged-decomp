#pragma once

#include <dwc/dwc_main_fwd.h>
#include <dwc/dwc_account.h>
#include <dwc/dwc_friend.h>
#include <dwc/dwc_login.h>
#include <dwc/dwc_transport.h>
#include <gamespy/gt2/gt2.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define DWC_DNS_ERROR_RETRY_MAX 5
#define DWC_IGNORE_GP_ERROR_ALREADY_BUDDY
#define DWC_GP_COMMAND_STRING       "GPCM"
#define DWC_GP_COMMAND_MATCH_STRING "MAT"
#define DWC_MAX_PLAYER_NAME 26

    typedef enum DWCState
    {
        DWC_STATE_INIT,
        DWC_STATE_AVAILABLE_CHECK,
        DWC_STATE_LOGIN,
        DWC_STATE_ONLINE,
#if !defined(VERSION_R4QK01)
        DWC_STATE_UPDATE_SERVERS,
#endif
        DWC_STATE_MATCHING,
        DWC_STATE_CONNECTED
    } DWCState;


    BOOL DWC_UpdateServersAsync(const char* playerName,
        DWCUpdateServersCallback updateCallback, void* updateParam,
        DWCFriendStatusCallback statusCallback, void* statusParam,
        DWCDeleteFriendListCallback deleteCallback, void* deleteParam);
#if defined(VERSION_R4QK01)
    BOOL DWC_ConnectToFriendsAsync(int topology, const u8 friendIdxList[],
        int friendIdxListLen, u8 maxEntry,
        DWCMatchedSCCallback matchedCallback, void* matchedParam,
        DWCNewClientCallback newClientCallback, void* newClientParam,
        DWCEvalPlayerCallback evalCallback, void* evalParam,
        DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam);
    BOOL DWC_ConnectToGameServerByGroupID(int topology, u32 groupID,
        DWCMatchedSCCallback matchedCallback, void* matchedParam,
        DWCNewClientCallback newClientCallback, void* newClientParam,
        DWCReservationCallback reservationCallback, const void* reservationData, void* reservationParam);
    int DWCi_CloseConnectionHard(u8 aid);
    int DWCi_CloseConnectionHardBitmap(u32* bitmap);
#else
    BOOL DWC_ConnectToFriendsAsync(const u8 friendIdxList[],
        int friendIdxListLen, u8 numEntry, BOOL distantFriend,
        DWCMatchedCallback matchedCallback, void* matchedParam,
        DWCEvalPlayerCallback evalCallback, void* evalParam);
    int DWC_CloseConnectionHardBitmap(u32* bitmap);
#endif

    DWCState DWC_GetState(void);
    u32 DWC_GetAIDBitmap(void);
    BOOL DWC_IsValidAID(u8 aid);

    GT2Result DWCi_GT2Startup(void);
    GT2Connection DWCi_GetGT2Connection(u8 aid);
    u8 DWCi_GetConnectionAID(GT2Connection connection);
    u8 DWCi_GetConnectionIndex(GT2Connection connection);
    void* DWCi_GetConnectionUserData(GT2Connection connection);
    int DWCi_GT2GetConnectionListIdx(void);
#if !defined(VERSION_R4QK01)
    void DWCi_ClearGT2ConnectionList(void);
#endif
    GT2Connection* DWCi_GetGT2ConnectionByIdx(int index);
    GT2Connection* DWCi_GetGT2ConnectionByProfileID(
        int profileID, int numHost);
    BOOL DWCi_IsValidAID(u8 aid);

#ifdef __cplusplus
}
#endif
