#include <revolution/esp.h>
#include <revolution/os.h>
#include <revolution/os/OSLaunch.h>
#include <revolution/os/OSNandbootInfo.h>
#include <revolution/os/OSPlayTime.h>
#include <revolution/os/OSStateFlags.h>
#include <string.h>

void __OSRelaunchTitle(u32 resetCode) {
    s32 result = 0;
    u32 ticketCount = 1;
    ESPTicketView* ticket = NULL;
    u64 titleId ALIGN32;
    u8* bootInfo = NULL;
    OSNandbootInfo* info = NULL;
    OSStateFlags state;

    OSSetArenaLo((void*)0x81280000);
    OSSetArenaHi((void*)0x812F0000);

    result = ESP_InitLib();
    if (result != 0) {
        __OSReturnToMenuForError();
    }

    result = ESP_GetTitleId(&titleId);
    if (result != 0) {
        __OSReturnToMenuForError();
    }

    ticket = (ESPTicketView*)OSAllocFromMEM1ArenaLo(
        OSRoundUp32B(sizeof(ESPTicketView)), 32);
    if (ticket == (void*)NULL) {
        __OSReturnToMenuForError();
    }

    memset(ticket, 0, OSRoundUp32B(sizeof(ESPTicketView)));
    result = ESP_DiGetTicketView(NULL, ticket);
    if (result == -1017) {
        ESPTicketView* views = NULL;
        result = ESP_GetTicketViews(titleId, NULL, &ticketCount);
        if (result != 0) {
            __OSReturnToMenuForError();
        }

        views = (ESPTicketView*)OSAllocFromMEM1ArenaLo(
            OSRoundUp32B(ticketCount * sizeof(ESPTicketView)), 32);
        if (views == (void*)NULL) {
            __OSReturnToMenuForError();
        }

        result = ESP_GetTicketViews(titleId, views, &ticketCount);
        if (result != 0) {
            __OSReturnToMenuForError();
        }

        memcpy(ticket, views, sizeof(ESPTicketView));
    } else if (result != 0) {
        __OSReturnToMenuForError();
    } else {
        if (OSPlayTimeIsLimited()) {
            OSPlayTimeType type = OS_PLAYTIME_PERMANENT;
            u32 remaining = 0xFFFFFFFF;
            __OSGetPlayTime(ticket, &type, &remaining);
            if (remaining == 0) {
                __OSWriteExpiredFlag();
                __OSReturnToMenuForError();
            }
        }
    }

    bootInfo = (u8*)OSAllocFromMEM1ArenaLo(0x2000, 64);
    info = (OSNandbootInfo*)((u32)bootInfo + (0x2000 - sizeof(OSNandbootInfo)));
    memset(bootInfo, 0, 0x2000);
    info->prevTitleId = titleId;
    info->prevAppType = OSGetAppType();
    info->returnValue = 1;
    info->argValue = resetCode | 0x80000000;

    __OSCreateNandbootInfo();
    __OSWriteNandbootInfo(info);
    __OSReadStateFlags(&state);
    state.shutdownType = 3;
    __OSWriteStateFlags(&state);
    result = ESP_LaunchTitle(titleId, ticket);

    if (result != 0) {
        __OSReturnToMenuForError();
    }

    while (TRUE) {
    }
}
