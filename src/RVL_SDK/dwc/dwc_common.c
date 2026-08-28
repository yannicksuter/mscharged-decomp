#include <dwc/dwc_common.h>

#include <gamespy/GP/gp.h>
#include <revolution/os/OSTime.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DWC_STATUS_MATCH_SC_SV 6

u64 lbl_806C9888[3];

int NCDGetWirelessMacAddress(u8* macAddr);
BOOL fn_8048B76C(const DWCFriendData* friendData, GPBuddyStatus* buddyStatus);

int DWC_SetCommonKeyValueString(const char* key, const char* value,
    char* string, char separator)
{
    snprintf(string, 4096, "%c%s%c%s", separator, key, separator, value);
    return strlen(string);
}

int DWC_AddCommonKeyValueString(const char* key, const char* value,
    char* string, char separator)
{
    DWC_SetCommonKeyValueString(key, value, strchr(string, '\0'), separator);
    return strlen(string);
}

int DWC_GetCommonValueString(const char* key, char* value, const char* string,
    char separator)
{
    const char* cursor;
    const char* end;
    int length;

    if (value == NULL)
    {
        return -1;
    }

    cursor = strchr(string, separator);
    if (cursor == NULL)
    {
        return -1;
    }

    for (;;)
    {
        if (strncmp(cursor + 1, key, strlen(key)) == 0 && cursor[strlen(key) + 1] == separator)
        {
            break;
        }
        cursor = strchr(cursor + 1, separator);
        if (cursor == NULL)
        {
            return -1;
        }
        cursor = strchr(cursor + 1, separator);
        if (cursor == NULL)
        {
            return -1;
        }
    }

    cursor = strchr(cursor + 1, separator);
    if (cursor == NULL)
    {
        return -1;
    }

    end = strchr(cursor + 1, separator);
    if (end != NULL)
    {
        length = end - (cursor + 1);
    }
    else
    {
        length = strlen(cursor + 1);
    }

    strncpy(value, cursor + 1, length);
    value[length] = '\0';
    return length;
}

u32 fn_8048A3BC(u32 max)
{
    u64 macAddress;

    if (lbl_806C9888[0] == 0 && lbl_806C9888[1] == 0 && lbl_806C9888[2] == 0)
    {
        NCDGetWirelessMacAddress((u8*)&macAddress);
        macAddress = ((u64)OSGetTime() << 24) | ((macAddress >> 24) & 0xFFFFFF);
        lbl_806C9888[0] = macAddress;
        lbl_806C9888[1] = 0x5D588B656C078965ULL;
        lbl_806C9888[2] = 0x269EC3;
    }

    lbl_806C9888[0] = lbl_806C9888[1] * lbl_806C9888[0] + lbl_806C9888[2];
    if (max == 0)
    {
        return lbl_806C9888[0] >> 32;
    }
    return ((lbl_806C9888[0] >> 32) * max) >> 32;
}

int fn_8048A504(const u16* string)
{
    int length = 0;

    while (*string != 0)
    {
        string++;
        length++;
    }
    return length;
}

u8 DWC_GetFriendStatus(const DWCFriendData* friendData, char* statusString)
{
    return DWC_GetFriendStatusSC(friendData, NULL, NULL, statusString);
}

u8 DWC_GetFriendStatusSC(const DWCFriendData* friendData, u8* maxEntry,
    u8* numEntry, char* statusString)
{
    char value[8];
    GPBuddyStatus buddyStatus;

    if (fn_8048B76C(friendData, &buddyStatus))
    {
        if (buddyStatus.status == DWC_STATUS_MATCH_SC_SV)
        {
            if (maxEntry != NULL)
            {
                if (DWC_GetCommonValueString("SCM", value, buddyStatus.statusString, '/') > 0)
                {
                    *maxEntry = strtol(value, NULL, 10);
                }
                else
                {
                    *maxEntry = 0;
                }
            }
            if (numEntry != NULL)
            {
                if (DWC_GetCommonValueString("SCN", value, buddyStatus.statusString, '/') > 0)
                {
                    *numEntry = strtol(value, NULL, 10);
                }
                else
                {
                    *numEntry = 0;
                }
            }
        }
        else
        {
            if (maxEntry != NULL)
            {
                *maxEntry = 0;
            }
            if (numEntry != NULL)
            {
                *numEntry = 0;
            }
        }

        if (statusString != NULL)
        {
            strcpy(statusString, buddyStatus.locationString);
        }
        return buddyStatus.status;
    }

    if (maxEntry != NULL)
    {
        *maxEntry = 0;
    }
    if (numEntry != NULL)
    {
        *numEntry = 0;
    }
    return 0;
}
