#pragma once

#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

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
