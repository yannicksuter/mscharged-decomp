#include <revolution/ipc.h>
#include <revolution/ncd.h>
#include <revolution/os.h>

#include <string.h>

/*
 * Reconstructed from R4QE01. The translation unit name comes from the retail
 * assert literal at 0x80554DF0; no Wii decompilation project publishes an NCD
 * implementation, so the bodies here are recovered from the target rather than
 * adapted from a donor.
 */

#define NCD_IPC_HEAP_SIZE 0x1B60
#define NCD_MAX_PROFILE 3

static u32 ncdInitialized;
static NCDConfig* ncdConfig;

static OSMutex ncdMutex;
/* IOS ioctl buffers: the IPC path requires 32-byte alignment. */
static u8 ncdWork0[0x20] ALIGN(32);
static u8 ncdWork1[0x20] ALIGN(32);

extern const char* __NCDVersion;

/*
 * Retail global at 0x804108F4. The DOL preserves no name for it, so the
 * address placeholder stands until one is established.
 */
void fn_804108F4(void);
s32 fn_80410760(const char* name, u32 arg1, u32 arg2);

void fn_804108F4(void) {
    BOOL enabled;
    void* lo;

    enabled = OSDisableInterrupts();

    if (!(ncdInitialized & 1)) {
        OSRegisterVersion(__NCDVersion);
        OSInitMutex(&ncdMutex);

        lo = (void*)OSRoundUp32B(IPCGetBufferLo());

        if ((u32)IPCGetBufferHi() - (u32)lo < NCD_IPC_HEAP_SIZE) {
            OSPanic("ncdsystem.c", 1448,
                    "Could not reserve heap for NCD library from IPC arena");
        }

        IPCSetBufferLo((void*)((u32)lo + NCD_IPC_HEAP_SIZE));
        ncdConfig = (NCDConfig*)lo;

        memset(ncdConfig, 0, NCD_IPC_HEAP_SIZE);
        memset(ncdWork0, 0, sizeof(ncdWork0));
        memset(ncdWork1, 0, sizeof(ncdWork1));

        ncdInitialized |= 1;
    }

    OSRestoreInterrupts(enabled);
    OSLockMutex(&ncdMutex);
}

NCDErr NCDGetCurrentIfConfig(NCDIfConfig* ifConfig) {
    NCDErr err = 0;
    NCDConfig* config;
    NCDProfile* profile;
    s32 idx;

    if (ifConfig == (void*)NULL) {
        return -3;
    }

    fn_804108F4();

    err = fn_80410760("NCDGetCurrentIfConfig", 0, 3);
    if (err == 0) {
        config = ncdConfig;
        idx = *(s32*)&ncdWork0[4];

        if (idx < 0 || idx >= NCD_MAX_PROFILE) {
            err = -7;
        } else {
            profile = &config->profiles[idx];

            if (profile->flags & 1) {
                ifConfig->selectedMedia = 2;
                memcpy(&ifConfig->netif, &profile->netif,
                       sizeof(NCDWiredProfile));
            } else {
                ifConfig->selectedMedia = 1;
                memcpy(&ifConfig->netif, &profile->netif,
                       sizeof(NCDWirelessProfile));
            }

            ifConfig->linkTimeout = config->linkTimeout;
        }
    }

    OSUnlockMutex(&ncdMutex);
    return err;
}

NCDErr NCDGetCurrentIpConfig(NCDIpConfig* ipConfig) {
    NCDErr err = 0;
    NCDConfig* config;
    s32 idx;

    if (ipConfig == (void*)NULL) {
        return -3;
    }

    fn_804108F4();

    err = fn_80410760("NCDGetCurrentIpConfig", 0, 3);
    if (err == 0) {
        config = ncdConfig;
        idx = *(s32*)&ncdWork0[4];

        if (idx < 0 || idx >= NCD_MAX_PROFILE) {
            err = -7;
        } else {
            memcpy(&ipConfig->adjust, &config->profiles[idx].adjust,
                   sizeof(NCDIpAdjustProfile));

            if (config->profiles[idx].flags & 0x6) {
                ipConfig->useDhcp = TRUE;
                memcpy(&ipConfig->ip, &config->profiles[idx].ip,
                       sizeof(NCDIpProfile));
            } else {
                ipConfig->useDhcp = FALSE;
                memcpy(&ipConfig->ip, &config->profiles[idx].ip,
                       sizeof(NCDIpProfile));
            }

            if (config->profiles[idx].flags & 0x10) {
                ipConfig->useProxy = TRUE;
                memcpy(&ipConfig->proxy, &config->profiles[idx].proxy,
                       sizeof(NCDProxyProfile));
            } else {
                ipConfig->useProxy = FALSE;
                memset(&ipConfig->proxy, 0, sizeof(NCDProxyProfile));
            }
        }
    }

    OSUnlockMutex(&ncdMutex);
    return err;
}
