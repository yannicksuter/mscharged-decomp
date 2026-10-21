#include <revolution/esp.h>

static s32 __esFd = -1;

s32 ESP_InitLib(void) {
    s32 ret = 0;

    if (__esFd < 0) {
        __esFd = IOS_Open("/dev/es", IPC_OPEN_NONE);

        if (__esFd < 0) {
            ret = __esFd;
        }
    }

    return ret;
}

s32 ESP_CloseLib(void) {
    s32 ret = 0;

    if (__esFd >= 0) {
        ret = IOS_Close(__esFd);
        if (ret == 0) {
            __esFd = -1;
        }
    }

    return ret;
}

s32 ESP_LaunchTitle(u64 titleId, ESPTicketView* ticketView) {
    s32 ret = 0;
    u8 buf[256] ALIGN(32);
    IPCIOVector* vec = (IPCIOVector*)(buf + sizeof(buf) - 6 * sizeof(IPCIOVector));
    u64* id = (u64*)buf;

    if (__esFd < 0) {
        return -0x3F9;
    }
    if ((u32)ticketView & 31) {
        return -0x3F9;
    }

    *id = titleId;
    vec[0].base = (u8*)id;
    vec[0].length = 8;
    vec[1].base = (u8*)ticketView;
    vec[1].length = 0xD8;

    ret = IOS_IoctlvReboot(__esFd, 8, 2, 0, vec);
    __esFd = -1;
    return ret;
}

s32 ESP_GetTicketViews(u64 titleId, ESPTicketView* ticketViewList, u32* ticketViewCnt) {
    s32 rv = 0;
    u8 __esBuf[256] ALIGN(32);
    IPCIOVector* v = (IPCIOVector*)(__esBuf + 256 - 6 * sizeof(IPCIOVector));
    u64* p1 = (u64*)__esBuf;
    u32* p2 = (u32*)(__esBuf + 32);

    if (__esFd < 0 || ticketViewCnt == (u32*)NULL) {
        rv = -1017;
        goto out;
    }

    if (((u32)ticketViewList & 31)) {
        rv = -1017;
        goto out;
    }

    *p1 = titleId;
    if (ticketViewList == (ESPTicketView*)NULL) {
        v[0].base = (u8*)p1;
        v[0].length = sizeof(u64);
        v[1].base = (u8*)p2;
        v[1].length = 4;
        rv = IOS_Ioctlv(__esFd, 18, 1, 1, v);

        if (rv == 0) {
            *ticketViewCnt = *p2;
        }
    } else {
        if (*ticketViewCnt == 0) {
            rv = -1017;
            goto out;
        }

        *p2 = *ticketViewCnt;
        v[0].base = (u8*)p1;
        v[0].length = sizeof(u64);
        v[1].base = (u8*)p2;
        v[1].length = 4;
        v[2].base = (u8*)ticketViewList;
        v[2].length = sizeof(ESPTicketView) * (*ticketViewCnt);
        rv = IOS_Ioctlv(__esFd, 19, 2, 1, v);
    }

out:
    return rv;
}

s32 ESP_DiGetTicketView(const ESPTicket* ticket, ESPTicketView* ticketView) {
    s32 rv = 0;
    u8 __esBuf[256] ALIGN(32);
    IPCIOVector* v = (IPCIOVector*)(__esBuf + 256 - 6 * sizeof(IPCIOVector));

    if (__esFd < 0 || ticketView == (ESPTicketView*)NULL) {
        rv = -1017;
        goto out;
    }

    if (((u32)ticket & 31) || ((u32)ticketView & 31)) {
        rv = -1017;
        goto out;
    }

    v[0].base = (u8*)ticket;

    if (ticket == (const ESPTicket*)NULL) {
        v[0].length = 0;
    } else {
        v[0].length = sizeof(ESPTicket);
    }

    v[1].base = (u8*)ticketView;
    v[1].length = sizeof(ESPTicketView);
    rv = IOS_Ioctlv(__esFd, 27, 1, 1, v);

out:
    return rv;
}

s32 ESP_DiGetTmd(ESPTmd* tmd, u32* tmdSize) {
    s32 rv = 0;
    u8 __esBuf[256] ALIGN(32);
    IPCIOVector* v = (IPCIOVector*)(__esBuf + 256 - 6 * sizeof(IPCIOVector));
    u32* p1 = (u32*)__esBuf;

    if (__esFd < 0 || tmdSize == (u32*)NULL) {
        rv = -1017;
        goto out;
    }

    if (((u32)tmd & 31)) {
        rv = -1017;
        goto out;
    }

    if (tmd == (ESPTmd*)NULL) {
        v[0].base = (u8*)p1;
        v[0].length = 4;
        rv = IOS_Ioctlv(__esFd, 57, 0, 1, v);

        if (rv == 0) {
            *tmdSize = *p1;
        }
    } else {
        if (*tmdSize == 0) {
            rv = -1017;
            goto out;
        }

        *p1 = *tmdSize;
        v[0].base = (u8*)p1;
        v[0].length = 4;
        v[1].base = (u8*)tmd;
        v[1].length = *tmdSize;
        rv = IOS_Ioctlv(__esFd, 58, 1, 1, v);
    }

out:
    return rv;
}

s32 ESP_GetDataDir(u64 titleId, char* dataDir) {
    s32 rv = 0;
    u8 __esBuf[256] ALIGN(32);
    IPCIOVector* v = (IPCIOVector*)(__esBuf + 256 - 6 * sizeof(IPCIOVector));
    u64* p1 = (u64*)__esBuf;

    if (__esFd < 0 || dataDir == (char*)NULL) {
        rv = -1017;
        goto out;
    }

    if (((u32)dataDir & 31)) {
        rv = -1017;
        goto out;
    }

    *p1 = titleId;
    v[0].base = (u8*)p1;
    v[0].length = sizeof(u64);
    v[1].base = (u8*)dataDir;
    v[1].length = 0x1E;
    rv = IOS_Ioctlv(__esFd, 29, 1, 1, v);

out:
    return rv;
}

s32 ESP_GetTitleId(u64* titleId) {
    s32 rv = 0;
    u8 __esBuf[256] ALIGN(32);
    IPCIOVector* v = (IPCIOVector*)(__esBuf + 256 - 6 * sizeof(IPCIOVector));

    if (__esFd < 0 || titleId == (u64*)NULL) {
        rv = -1017;
        goto out;
    }

    v[0].base = __esBuf;
    v[0].length = sizeof(u64);

    rv = IOS_Ioctlv(__esFd, 32, 0, 1, v);

    if (rv == 0) {
        *titleId = *(u64*)__esBuf;
    }

out:
    return rv;
}

s32 ESP_GetConsumption(u64 ticketId, ESLpEntry* entries, u32* nEntries) {
    s32 rv = 0;
    u8 __esBuf[256] ALIGN(32);
    IPCIOVector* v = (IPCIOVector*)(__esBuf + 256 - 6 * sizeof(IPCIOVector));
    u64* p1 = (u64*)__esBuf;
    u32* p2 = (u32*)(__esBuf + 32);

    if (__esFd < 0) {
        rv = -1017;
        goto out;
    }

    if (((u32)entries & 31)) {
        rv = -1017;
        goto out;
    }

    *p1 = ticketId;
    v[0].base = (u8*)p1;
    v[0].length = sizeof(u64);

    if (entries == (ESLpEntry*)NULL) {
        v[1].base = NULL;
        v[1].length = 0;
    } else {
        v[1].base = (u8*)entries;
        v[1].length = sizeof(ESLpEntry) * (*nEntries);
        *p2 = *nEntries;
    }

    v[2].base = (u8*)p2;
    v[2].length = 4;
    rv = IOS_Ioctlv(__esFd, 22, 1, 2, v);
    *nEntries = *p2;
out:
    return rv;
}
