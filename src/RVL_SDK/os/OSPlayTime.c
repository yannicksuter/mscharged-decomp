#include <revolution/ai.h>
#include <revolution/esp.h>
#include <revolution/nand.h>
#include <revolution/os.h>
#include <revolution/os/OSPlayTime.h>
#include <revolution/os/OSStateTM.h>
#include <revolution/vi.h>
#include <string.h>

typedef void (*OSPlayTimeCallbackFunc)(void);

OSAlarm __OSExpireAlarm;
OSTime __OSExpireTime;
OSPlayTimeCallbackFunc __OSExpireCallback;
BOOL __OSExpireSetExpiredFlag;

typedef struct OSExpireAIFade {
    s16 aiBuffer[2][288];
    s32 aiIndex;
    s16* aiSrcIndex;
    u32 frames;
    s16 auxL;
    s16 auxR;
    AIDMACallback gameAIDCallback;
} OSExpireAIFade;

static OSExpireAIFade* __OSExpireAIFade = NULL;

BOOL OSPlayTimeIsLimited(void) {
    return __OSExpireTime != 0;
}

static void __OSPlayTimeFadeLastAIDCallback(void) {
    if (__OSExpireAIFade->gameAIDCallback) {
        __OSExpireAIFade->gameAIDCallback();
    }

    if (__OSExpireAIFade->frames == 0) {
        __OSExpireAIFade->aiSrcIndex =
            (s16*)OSPhysicalToCached(AIGetDMAStartAddr());
    }

    if (__OSExpireAIFade->frames == 1) {
        DCInvalidateRange(__OSExpireAIFade->aiSrcIndex, 4);
        __OSExpireAIFade->auxL = *__OSExpireAIFade->aiSrcIndex++;
        __OSExpireAIFade->auxR = *__OSExpireAIFade->aiSrcIndex;
    }

    if (__OSExpireAIFade->frames >= 1) {
        s16* buffer = __OSExpireAIFade->aiBuffer[__OSExpireAIFade->aiIndex];
        s16* dest = buffer;
        u32 bytes = AIGetDMALength();
        u32 i = bytes;
        f32 delta = 0.995f;

        while (i) {
            *dest++ = __OSExpireAIFade->auxL;
            *dest++ = __OSExpireAIFade->auxR;
            __OSExpireAIFade->auxL *= delta;
            __OSExpireAIFade->auxR *= delta;
            i -= 4;
        }

        DCFlushRange(buffer, bytes);
        AIInitDMA(buffer, bytes);
        __OSExpireAIFade->aiIndex++;
        __OSExpireAIFade->aiIndex &= 1;
    }

    __OSExpireAIFade->frames++;
}

BOOL __OSWriteExpiredFlag(void) {
    s32 result = NAND_RESULT_OK;
    NANDFileInfo fileInfo;
    BOOL isOpen = FALSE;
    u8 titleId[32] ALIGN32;

    result = NANDPrivateCreate("/shared2/expired", NAND_PERM_RWALL, 0);
    if (result != NAND_RESULT_OK && result != NAND_RESULT_EXISTS) {
        goto out;
    }

    result = NANDPrivateOpen("/shared2/expired", &fileInfo, NAND_ACCESS_WRITE);
    if (result != NAND_RESULT_OK) {
        goto out;
    }
    isOpen = TRUE;

    result = ESP_InitLib();
    if (result != 0) {
        goto out;
    }

    memset(titleId, 0, sizeof(titleId));
    result = ESP_GetTitleId((u64*)titleId);
    if (result != 0) {
        goto out;
    }

    result = NANDWrite(&fileInfo, titleId, sizeof(titleId));
    if (result < NAND_RESULT_OK) {
        goto out;
    } else if (result != (s32)sizeof(titleId)) {
        result = NAND_RESULT_INVALID;
        goto out;
    } else {
        result = NAND_RESULT_OK;
    }

out:
    if (isOpen) {
        NANDClose(&fileInfo);
    }
    return result == NAND_RESULT_OK ? TRUE : FALSE;
}

BOOL __OSWriteExpiredFlagIfSet(void) {
    if (__OSExpireSetExpiredFlag) {
        return __OSWriteExpiredFlag();
    }
    return FALSE;
}

static void* __OSPlayTimeRebootThread(void* args) {
    BOOL enabled;
    u32 frames, fadeShift = 1;
    OSExpireAIFade aiFade ALIGN32;

    __OSExpireAIFade = &aiFade;
    memset(__OSExpireAIFade, 0, sizeof(OSExpireAIFade));
    __OSExpireAIFade->gameAIDCallback =
        AIRegisterDMACallback(__OSPlayTimeFadeLastAIDCallback);

    for (frames = 0; frames < 20; frames++) {
        fadeShift = frames / 5 + 1;
        if (fadeShift > 7) {
            fadeShift = 7;
        }
        VIWaitForRetrace();
        __OSSetVIForceDimming(TRUE, fadeShift, fadeShift);
    }

    AIRegisterDMACallback(NULL);
    VISetBlack(TRUE);
    VIFlush();
    enabled = OSDisableInterrupts();
    __OSWriteExpiredFlagIfSet();
    OSRestoreInterrupts(enabled);
    OSReturnToMenu();
    return NULL;
}

static void __OSPlayTimeAlarmExpired(OSAlarm* alarm, OSContext* context) {
    void* arenaHi;
    u32 memSize;
    OSThread* thread;
    void* stack;

    if (__OSExpireCallback != NULL) {
        __OSExpireCallback();
    } else {
        for (thread = OS_THREAD_QUEUE.head; thread; thread = thread->nextActive) {
            OSSuspendThread(thread);
        }

        memSize = OSRoundUp32B(sizeof(OSThread)) + 0x1000;
        arenaHi = *(void**)OSPhysicalToCached(0x3128);
        arenaHi = (void*)((u32)arenaHi - memSize);
        thread = (OSThread*)arenaHi;
        arenaHi = (void*)((u32)arenaHi + OSRoundUp32B(sizeof(OSThread)));
        stack = arenaHi;

        if (!OSCreateThread(thread, __OSPlayTimeRebootThread, NULL,
                            (void*)((u32)stack + 0x1000), 0x1000, 0, 0)) {
            __OSHotResetForError();
        }
        OSResumeThread(thread);
    }
}

static s32 __OSPlayTimeGetConsumption(ESPTicketView* ticket,
                                    ESLpEntry* entries, u32* numEntries) {
    s32 result = ESP_GetConsumption(ticket->ticketId, NULL, numEntries);

    if (result > 0) {
        goto out;
    } else if (result != 0) {
        return result;
    }

    if (*numEntries != 0) {
        result = ESP_GetConsumption(ticket->ticketId, entries, numEntries);
        if (result != 0) {
            return result;
        }
    }

out:
    return result;
}

s32 __OSGetPlayTime(ESPTicketView* ticket, OSPlayTimeType* type, u32* playTime) {
    s32 result;
    u32 i;
    ESLpEntry entries[8] ALIGN32;
    u32 numEntries = 0, seenOther = 0;
    ESPTicketView ticketAligned ALIGN32;

    if ((u32)ticket & 31) {
        memcpy(&ticketAligned, ticket, sizeof(ESPTicketView));
        ticket = &ticketAligned;
    }

    result = __OSPlayTimeGetConsumption(ticket, entries, &numEntries);
    if (result != 0) {
        goto out;
    }

    for (i = 0; i < 8; i++) {
        if (ticket->limits[i].code == OS_PLAYTIME_TIME_LIMIT) {
            *type = OS_PLAYTIME_TIME_LIMIT;
            if (numEntries == 0) {
                *playTime = ticket->limits[i].limit;
            } else {
                if (entries[i].limit >= ticket->limits[i].limit) {
                    *playTime = 0;
                } else {
                    *playTime = ticket->limits[i].limit - entries[i].limit;
                }
            }
            goto out;
        } else if (ticket->limits[i].code != OS_PLAYTIME_PERMANENT) {
            seenOther = i + 1;
        }
    }

    if (!seenOther) {
        *type = OS_PLAYTIME_PERMANENT;
        *playTime = 0xFFFFFFFF;
    } else {
        seenOther--;
        if (ticket->limits[seenOther].code == OS_PLAYTIME_LAUNCH_LIMIT) {
            *type = OS_PLAYTIME_LAUNCH_LIMIT;
            *playTime = ticket->limits[seenOther].limit;
            if (numEntries > 0) {
                *playTime -= entries[seenOther].limit;
            }
        } else {
            *type = OS_PLAYTIME_UNKNOWN;
        }
    }

out:
    return result;
}

static s32 __OSGetPlayTimeCurrent(OSPlayTimeType* type, u32* playTime) {
    s32 result;
    ESPTicketView ticket ALIGN32;

    result = ESP_DiGetTicketView(NULL, &ticket);
    if (result == -1017) {
        goto out;
    }
    if (result != 0) {
        return result;
    }
    result = __OSGetPlayTime(&ticket, type, playTime);

out:
    return result;
}

static inline s32 __OSStartPlayTime(void) {
    s32 result;
    OSPlayTimeType type;
    u32 limit;

    __OSExpireTime = 0;
    __OSExpireCallback = NULL;
    __OSExpireSetExpiredFlag = TRUE;

    result = ESP_InitLib();
    if (result != 0) {
        return result;
    }
    result = __OSGetPlayTimeCurrent(&type, &limit);
    if (result != 0) {
        if (result != -1017) {
            return result;
        }
        goto out;
    }

    if (type == OS_PLAYTIME_PERMANENT) {
        goto out;
    } else if (type == OS_PLAYTIME_TIME_LIMIT) {
        if (limit == 0) {
            OSPanic("OSPlayTime.c", 0x2E1, "Expired");
        }
        OSCreateAlarm(&__OSExpireAlarm);
        OSSetAlarm(&__OSExpireAlarm, OSSecondsToTicks((OSTime)limit + 20),
                   __OSPlayTimeAlarmExpired);
        __OSExpireTime = __OSExpireAlarm.end;
        OSReport("PlayTime: %d seconds left\n", limit);
    }

out:
    return result;
}

void __OSInitPlayTime(void) {
    __OSStartPlayTime();
    ESP_CloseLib();
}
