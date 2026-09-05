#pragma once

#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define DWC_MATCHING_VERSION 3U
#define DWC_MAX_CONNECTIONS  32
#define DWC_SB_COMMAND_STRING "SBCM"

#define DWC_QR2_PID_KEY        NUM_RESERVED_KEYS
#define DWC_QR2_MATCH_TYPE_KEY (NUM_RESERVED_KEYS + 1)
#define DWC_QR2_MATCH_RESV_KEY (NUM_RESERVED_KEYS + 2)
#define DWC_QR2_MATCH_VER_KEY  (NUM_RESERVED_KEYS + 3)
#define DWC_QR2_MATCH_EVAL_KEY (NUM_RESERVED_KEYS + 4)

#define DWC_QR2_RESERVED_KEYS      (100 - NUM_RESERVED_KEYS)
#define DWC_QR2_GAME_RESERVED_KEYS                                             \
    (MAX_REGISTERED_KEYS - NUM_RESERVED_KEYS - DWC_QR2_RESERVED_KEYS)
#define DWC_QR2_GAME_KEY_START 100

    typedef struct DWCstSBMessageHeader
    {
        char identifier[4];
        u32 version;
        u8 command;
        u8 size;
        u16 qr2Port;
        u32 qr2IP;
        int profileID;
    } DWCSBMessageHeader;

    typedef struct DWCstSBMessage
    {
        DWCSBMessageHeader header;
        u32 data[DWC_MAX_CONNECTIONS];
    } DWCSBMessage;

    typedef struct DWCstGameMatchKeyData
    {
        u8 keyID;
        u8 isStr;
        u16 pad;
        char* keyStr;
        const void* value;
    } DWCGameMatchKeyData;

    typedef enum DWCMatchOptionType
    {
        DWC_MATCH_OPTION_MIN_COMPLETE,
        DWC_MATCH_OPTION_SC_CONNECT_BLOCK
    } DWCMatchOptionType;

    typedef struct DWCMatchOptMinComplete
    {
        u8 valid;
        u8 minEntry;
        u8 padding[2];
        u32 timeout;
    } DWCMatchOptMinComplete;

    typedef struct DWCMatchOptSCConnectBlock
    {
        u32 valid;
    } DWCMatchOptSCConnectBlock;

    BOOL DWC_RegisterMatchingStatus(void);
    BOOL DWC_CancelMatch(void);
    BOOL DWC_IsValidMatchCancel(void);
    u8 DWC_AddMatchKeyInt(u8 keyID, const char* keyString, const int* value);
    u8 DWC_AddMatchKeyString(u8 keyID, const char* keyString,
        const char* const* value);
    int DWC_SetMatchingOption(int option, const void* optval);
    void DWCi_CloseMatching(void);

#ifdef __cplusplus
}
#endif
