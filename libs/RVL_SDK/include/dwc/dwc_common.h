#pragma once

#include <dwc/dwc_account.h>
#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    int DWC_SetCommonKeyValueString(const char* key, const char* value,
        char* string, char separator);
    int DWC_AddCommonKeyValueString(const char* key, const char* value,
        char* string, char separator);
    int DWC_GetCommonValueString(const char* key, char* value, const char* string,
        char separator);

    u8 DWC_GetFriendStatus(const DWCFriendData* friendData, char* statusString);
    u8 DWC_GetFriendStatusSC(const DWCFriendData* friendData, u8* maxEntry,
        u8* numEntry, char* statusString);

#ifdef __cplusplus
}
#endif
