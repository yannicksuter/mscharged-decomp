#include <private/wud.h>
#include <private/wud/WUDHidHost.h>

#include <private/bte/bt_trace.h>
#include <private/bte/bta_api.h>
#include <private/bte/btm_api.h>
#include <private/bte/btm_int.h>
#include <private/bte/l2c_api.h>
#include <private/bte/misc.h>
#include <private/bte/bta_sys.h>
#include <revolution/os.h>
#include <revolution/nand.h>

#include <string.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define DEBUGPrint WUD_DEBUGPrint

#define BT_VSC_NINTENDO_SUPER_PEEK_POKE 0xFC0A
#define BT_VSC_NINTENDO_INSTALL_PATCH   0xFC4F
#define BT_VSC_NINTENDO_WRITE_PATCH     0xFC4C

void WUDSecurityCallback(tBTA_DM_SEC_EVT event, tBTA_DM_SEC* pData);
void WUDSearchCallback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH* pData);
void WUDVendorSpecificCallback(UINT8 len, UINT8* pData);
void WUDDeviceStatusCallback(tBTM_DEV_STATUS status);
void WUDStoredLinkKeyCallback(void* p1);
void WUDPowerManagerCallback(BD_ADDR addr, tBTM_PM_STATUS status, UINT16 value,
    UINT8 hciStatus);

static void InitCore(void);

void WUDiSetDevAddrForHandle(UINT8 handle, BD_ADDR addr);
BD_ADDR_PTR WUDiGetDevAddrForHandle(UINT8 handle);
void WUDiSetQueueSizeForHandle(UINT8 handle, u16 size);
u16 WUDiGetQueueSizeForHandle(UINT8 handle);
void WUDiSetNotAckNumForHandle(UINT8 handle, u16 count);
u16 WUDiGetNotAckNumForHandle(UINT8 handle);

static void WUDiRemovePatch(void);
static u32 WUDiUpdateWiiFitCrc(void* buffer, u32 bytes, u32 crc);
void WUDiMoveTopOfUnusedStdDevice(WUDDevInfo* pInfo);
static void WUDiWritePatch(void);
static void WUDiInstallPatch(void);

static u8 WUDiVirginSimpleSync(void);
static u8 WUDiChangeSimpleToStandard(void);
static u8 WUDiRegisterSyncDevice(void);

WUDCB _wcb;
WUDDevInfo _work;
static WUDDiscResp _discResp;
SCBtDeviceInfoArray _scArray;
static SCBtCmpDevInfoArray _spArray;

typedef struct WUDWiiFitPairingInfo
{
    BD_ADDR devAddr;
    LINK_KEY linkKey;
    SCDevInfo info;
    u8 reserved[42];
    u32 crc;
} ALIGN(32) WUDWiiFitPairingInfo;

static WUDWiiFitPairingInfo _nandWbcInfo ALIGN(32);
static NANDFileInfo _nandFileInfo;
static NANDCommandBlock _nandBlock;

typedef enum
{
    WUD_NAND_OPEN = 1,
    WUD_NAND_SEEK = 2,
    WUD_NAND_READ = 3,
    WUD_NAND_GET_DEV_INFO = 4,
    WUD_NAND_CLOSE = 5,
    WUD_NAND_DONE = 6,
} WUDNandState;

#define WUD_WBC_SC_INDEX           14
#define WUD_DEV_NAME_IS_WBC(NAME)  WUD_DEV_NAME_IS(NAME, "Nintendo RVL-WBC")
#define WUD_WII_FIT_PAIRING_OFFSET 0x4AF18
#define WUD_WII_FIT_SAVE_SIZE      0x4B000
#define WUD_WII_FIT_PAIRING_SIZE   128
#define WUD_WII_FIT_PATH           "/title/00010004/52464e4a/data/RPHealth.dat"

u8 __WUDHandlerStack[0x1000] ALIGN(32);

BD_ADDR_PTR _dev_handle_to_bda[WUD_MAX_DEV_ENTRY];
u16 _dev_handle_queue_size[WUD_MAX_DEV_ENTRY];
u16 _dev_handle_notack_num[WUD_MAX_DEV_ENTRY];

static OSAlarm _arm;

static u8 descriptor[] = {
    0x05,
    0x01, // Usage Page (Generic Desktop Ctrls)
    0x09,
    0x05, // Usage (Game Pad)

    0xA1,
    0x01, // Collection (Application)

    0x85,
    0x10, // Report ID (16)
    0x15,
    0x00, // Logical Minimum (0)
    0x26,
    0xFF,
    0x00, // Logical Maximum (255)
    0x75,
    0x08, // Report Size (8)
    0x95,
    0x01, // Report Count (1)
    0x06,
    0x00,
    0xFF, // Usage Page (Vendor Defined 0xFF00)
    0x09,
    0x01, // Usage (0x01)
    0x91,
    0x00, // Output (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    0x85,
    0x11, // Report ID (17)
    0x95,
    0x01, // Report Count (1)
    0x09,
    0x01, // Usage (0x01)
    0x91,
    0x00, // Output (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    0x85,
    0x12, // Report ID (18)
    0x95,
    0x02, // Report Count (2)
    0x09,
    0x01, // Usage (0x01)
    0x91,
    0x00, // Output (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    0x85,
    0x13, // Report ID (19)
    0x95,
    0x01, // Report Count (1)
    0x09,
    0x01, // Usage (0x01)
    0x91,
    0x00, // Output (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    0x85,
    0x14, // Report ID (20)
    0x95,
    0x01, // Report Count (1)
    0x09,
    0x01, // Usage (0x01)
    0x91,
    0x00, // Output (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    0x85,
    0x15, // Report ID (21)
    0x95,
    0x01, // Report Count (1)
    0x09,
    0x01, // Usage (0x01)
    0x91,
    0x00, // Output (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    0x85,
    0x16, // Report ID (22)
    0x95,
    0x15, // Report Count (21)
    0x09,
    0x01, // Usage (0x01)
    0x91,
    0x00, // Output (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    0x85,
    0x17, // Report ID (23)
    0x95,
    0x06, // Report Count (6)
    0x09,
    0x01, // Usage (0x01)
    0x91,
    0x00, // Output (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    0x85,
    0x18, // Report ID (24)
    0x95,
    0x15, // Report Count (21)
    0x09,
    0x01, // Usage (0x01)
    0x91,
    0x00, // Output (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    0x85,
    0x19, // Report ID (25)
    0x95,
    0x01, // Report Count (1)
    0x09,
    0x01, // Usage (0x01)
    0x91,
    0x00, // Output (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    0x85,
    0x1A, // Report ID (26)
    0x95,
    0x01, // Report Count (1)
    0x09,
    0x01, // Usage (0x01)
    0x91,
    0x00, // Output (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    0x85,
    0x20, // Report ID (32)
    0x95,
    0x06, // Report Count (6)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x21, // Report ID (33)
    0x95,
    0x15, // Report Count (21)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x22, // Report ID (34)
    0x95,
    0x04, // Report Count (4)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x30, // Report ID (48)
    0x95,
    0x02, // Report Count (2)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x31, // Report ID (49)
    0x95,
    0x05, // Report Count (5)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x32, // Report ID (50)
    0x95,
    0x0A, // Report Count (10)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x33, // Report ID (51)
    0x95,
    0x11, // Report Count (17)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x34, // Report ID (52)
    0x95,
    0x15, // Report Count (21)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x35, // Report ID (53)
    0x95,
    0x15, // Report Count (21)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x36, // Report ID (54)
    0x95,
    0x15, // Report Count (21)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x37, // Report ID (55)
    0x95,
    0x15, // Report Count (21)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x3D, // Report ID (61)
    0x95,
    0x15, // Report Count (21)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x3E, // Report ID (62)
    0x95,
    0x15, // Report Count (21)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0x85,
    0x3F, // Report ID (63)
    0x95,
    0x15, // Report Count (21)
    0x09,
    0x01, // Usage (0x01)
    0x81,
    0x00, // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    0xC0, // End Collection
};

static u8 super_peek_poke[] = {
    0x05, 0x00, 0x9A, 0x0E, 0x00, 0x01, 0x00, 0x00, 0x00
};

static u8 patch_binary[] = {
    0x70, 0x99, 0x08, 0x00, // address (little-endian)
    0xB4,
    0x00,
    0x00,
    0x00, // size (little-endian)

    // patch data
    0x88,
    0x43,
    0xD1,
    0x07,
    0x09,
    0x0C,
    0x08,
    0x43,
    0xA0,
    0x62,
    0x19,
    0x23,
    0xDB,
    0x01,
    0x33,
    0x80,
    0x7C,
    0xF7,
    0x88,
    0xF8,
    0x28,
    0x76,
    0x80,
    0xF7,
    0x17,
    0xFF,
    0x43,
    0x78,
    0xEB,
    0x70,
    0x19,
    0x23,
    0xDB,
    0x01,
    0x33,
    0x87,
    0x7C,
    0xF7,
    0xBC,
    0xFB,
    0x0B,
    0x60,
    0xA3,
    0x7B,
    0x01,
    0x49,
    0x0B,
    0x60,
    0x90,
    0xF7,
    0x96,
    0xFB,
    0xD8,
    0x1D,
    0x08,
    0x00,
    0x00,
    0xF0,
    0x04,
    0xF8,
    0x00,
    0x23,
    0x79,
    0xF7,
    0xE3,
    0xFA,
    0x00,
    0x00,
    0x00,
    0xB5,
    0x00,
    0x23,
    0x11,
    0x49,
    0x0B,
    0x60,
    0x1D,
    0x21,
    0xC9,
    0x03,
    0x0B,
    0x60,
    0x7D,
    0x20,
    0x80,
    0x01,
    0x01,
    0x38,
    0xFD,
    0xD1,
    0x0E,
    0x4B,
    0x0E,
    0x4A,
    0x13,
    0x60,
    0x47,
    0x20,
    0x00,
    0x21,
    0x96,
    0xF7,
    0x96,
    0xFF,
    0x46,
    0x20,
    0x00,
    0x21,
    0x96,
    0xF7,
    0x92,
    0xFF,
    0x0A,
    0x4A,
    0x13,
    0x68,
    0x0A,
    0x48,
    0x03,
    0x40,
    0x13,
    0x60,
    0x0A,
    0x4A,
    0x13,
    0x68,
    0x0A,
    0x48,
    0x03,
    0x40,
    0x13,
    0x60,
    0x09,
    0x4A,
    0x13,
    0x68,
    0x09,
    0x48,
    0x03,
    0x40,
    0x13,
    0x60,
    0x00,
    0xBD,
    0x24,
    0x80,
    0x0E,
    0x00,
    0x81,
    0x03,
    0x0F,
    0xFE,
    0x5C,
    0x00,
    0x0F,
    0x00,
    0x60,
    0xFC,
    0x0E,
    0x00,
    0xFE,
    0xFF,
    0x00,
    0x00,
    0xFC,
    0xFC,
    0x0E,
    0x00,
    0xFF,
    0x9F,
    0x00,
    0x00,
    0x30,
    0xFC,
    0x0E,
    0x00,
    0x7F,
    0xFF,
    0x00,
    0x00
};

static WUDPatchList patch_install = {
    7,
    { { 0x20, 0xBC, 0x65, 0x01, 0x00, 0x84, 0x42, 0x09, 0xD2, 0x84, 0x42, 0x09, 0xD1 },
        { 0x21, 0x84, 0x5A, 0x00, 0x00, 0x83, 0xF0, 0x74, 0xFF, 0x09, 0x0C, 0x08, 0x43 },
        { 0x22, 0x00, 0x61, 0x00, 0x00, 0x83, 0xF0, 0x40, 0xFC, 0x00, 0x00, 0x00, 0x00 },
        { 0x23, 0xCC, 0x9F, 0x01, 0x00, 0x6F, 0xF0, 0xE4, 0xFC, 0x03, 0x28, 0x7D, 0xD1 },
        { 0x24, 0x3C, 0x62, 0x01, 0x00, 0x28, 0x20, 0x00, 0xE0, 0x60, 0x8D, 0x23, 0x68 },
        { 0x25, 0x04, 0x12, 0x01, 0x00, 0x20, 0x1C, 0x20, 0x1C, 0x24, 0xE0, 0xB0, 0x21 },
        { 0x26, 0x74, 0x2F, 0x00, 0x00, 0x86, 0xF0, 0x18, 0xFD, 0x21, 0x4F, 0x3B, 0x60 } }
};

static u8 reset_auth_count[] = {
    0x30, 0x36, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static u8 _discNumResps;
static s8 _discRssi;
static u8 _syncTarget;
static u8 _scFlush;
static u32 size;
static s32 offset;
static u32 addr;
static u8 patch_num;
static u8 install_num;
static u8 _nandLocked;
static int _nandPhase;
static u32 _nandWbcCrc;
DECL_WEAK BOOL _linkedWBC;
DECL_WEAK void (*_initWBC)(void);
static u8* _nandBufPtr;
static u8 __bte_trace_level;
static u8 remove_patch[1];
static BOOL _abortSync;
static BOOL _readNand;
static BOOL _initialized;

BOOL WUDIsLinkedWBC(void)
{
    return _linkedWBC;
}

void* App_MEMalloc(u32 size)
{
    DEBUGPrint("App_MEMalloc\n");
    return _wcb.allocFunc(size);
}

BOOL App_MEMfree(void* pBlock)
{
    DEBUGPrint("App_MEMfree\n");
    return _wcb.freeFunc(pBlock);
}

static void ReverseAddr(BD_ADDR_PTR pDst, BD_ADDR src)
{
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < BD_ADDR_LEN; i++)
    {
        pDst[i] = src[BD_ADDR_LEN - 1 - i];
    }

    OSRestoreInterrupts(enabled);
}

static void SyncFlushCallback(SCStatus status)
{
    WUDCB* p = &_wcb;

    DEBUGPrint("__wudSyncFlushCallback() : %d, Sync: %d\n", status, _wcb.syncState);

    if (p->syncState == WUD_STATE_START)
    {
        return;
    }

    if (status == SC_STATUS_OK)
    {
        p->syncState = WUD_STATE_SYNC_COMPLETE;
    }
    else
    {
        p->syncState = WUD_STATE_ERROR;
    }
}

static void ClearDiscoverResult(void)
{
    _discNumResps = 0;
    memset(&_discResp, 0, sizeof(WUDDiscResp));
}

static void _resumeSmpSync(OSAlarm* pAlarm, OSContext* pContext)
{
#pragma unused(pAlarm)
#pragma unused(pContext)

    _wcb.syncState = WUD_STATE_SYNC_PREPARE_SEARCH;
}

static void WUDiWaitForIncomingConnection(void)
{
    u32 waitSec = WUDiGetLinkNum() == WUD_MAX_CHANNELS - 1 ? 4 : 2;

    if (WUDiGetLinkNum() == WUD_MAX_CHANNELS && WUDiGetConnNumber() == WUD_MAX_CHANNELS)
    {

        _wcb.syncState = WUD_STATE_SYNC_DONE;
    }

    WUDSetVisibility(FALSE, TRUE);

    OSCreateAlarm(&_arm);
    OSSetAlarm(&_arm, OS_SEC_TO_TICKS(waitSec), _resumeSmpSync);
}

static u8 WUDiWaitForStartSearch(void)
{
    WUDCB* p = &_wcb;
    if (p->syncLoopNum == 0)
    {
        return WUD_STATE_SYNC_DONE;
    }
    if (WUDiGetLinkNum() == WUD_MAX_CHANNELS && WUDiGetConnNumber() == WUD_MAX_CHANNELS)
    {
        return WUD_STATE_SYNC_DONE;
    }
    if (--p->waitStartSearchFrames < 0)
    {
        return WUD_STATE_SYNC_START_SEARCH;
    }
    return WUD_STATE_SYNC_WAIT_FOR_START_SEARCH;
}

static u8 WUDiWaitForIncoming(void)
{
    WUDSyncState nextState = WUD_STATE_SYNC_WAIT_FOR_INCOMING;
    if (WUDiGetLinkNum() == WUD_MAX_CHANNELS && WUDiGetConnNumber() == WUD_MAX_CHANNELS)
    {
        nextState = WUD_STATE_SYNC_DONE;
    }
    if (--_wcb.waitIncomingFrames < 0)
    {
        nextState = WUD_STATE_SYNC_PREPARE_SEARCH;
    }
    return nextState;
}

static WUDDevInfo* WUDiGetWbcDevice(void)
{
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo = NULL;
    WUDDevInfoList* pIt;
    BOOL enabled = OSDisableInterrupts();
    for (pIt = p->stdListHead; pIt != NULL; pIt = pIt->next)
    {
        if (WUD_DEV_NAME_IS_WBC(pIt->devInfo->conf.devName))
        {
            pInfo = pIt->devInfo;
        }
    }
    OSRestoreInterrupts(enabled);
    return pInfo;
}

static void WUDiPrepareWbcDevInfo(BD_ADDR addr, LINK_KEY key, const SCDevInfo* info)
{
    u32 crc;
    if (addr)
    {
        memcpy(_nandWbcInfo.devAddr, addr, BD_ADDR_LEN);
    }
    if (key)
    {
        memcpy(_nandWbcInfo.linkKey, key, LINK_KEY_LEN);
    }
    if (info)
    {
        memcpy(&_nandWbcInfo.info, info, sizeof(SCDevInfo));
    }
    crc = WUDiUpdateWiiFitCrc(&_nandWbcInfo, WUD_WII_FIT_PAIRING_SIZE, _nandWbcCrc);
    memcpy(&_nandWbcInfo.crc, &crc, sizeof(crc));
}

static u8 WUDiIsSyncDisabled(void)
{
    WUDCB* p = &_wcb;
    int i;
    WUDDevInfo* pInfo;

    WUDSetVisibility(FALSE, FALSE);

    if (p->syncLoopNum == 0)
    {
        return WUD_STATE_SYNC_DONE;
    }

    if (WUDiGetLinkNum() == WUD_MAX_CHANNELS && WUDiGetConnNumber() == WUD_MAX_CHANNELS)
    {

        return WUD_STATE_SYNC_DONE;
    }

    if (p->syncSkipChecks)
    {
        for (i = 0; i < WUD_MAX_DEV_ENTRY; i++)
        {
            pInfo = WUDiGetDevInfoIndex(i);

            if (pInfo->status == 9)
            {
                WUDSetSniffMode(pInfo->devAddr, 0);
                return WUD_STATE_SYNC_PREPARE_SEARCH;
            }
        }
    }

    if (p->syncLoopNum > 0)
    {
        p->syncLoopNum--;
    }

    p->waitStartSearchFrames = 50;
    return WUD_STATE_SYNC_WAIT_FOR_START_SEARCH;
}

static u8 WUDiStartSearch(void)
{
    WUDCB* p = &_wcb;
    tBTA_DM_INQ dm_inq;

    dm_inq.mode = BTM_LIMITED_INQUIRY;
    dm_inq.max_resps = 1;
    dm_inq.report_dup = FALSE;

    if (p->syncSkipChecks)
    {
        dm_inq.duration = 5 - 2;
    }
    else if (p->syncType == WUD_SYNC_TYPE_STANDARD)
    {
        dm_inq.duration = WUDiGetLinkNum() == WUD_MAX_CHANNELS - 1 ? 10 : 5;
        dm_inq.duration -= p->syncLoopNum;
    }
    else
    {
        dm_inq.duration = WUDiGetLinkNum() == WUD_MAX_CHANNELS - 1 ? 10 - 2 : 5 - 2;
    }

    ClearDiscoverResult();
    BTA_DmSearch(&dm_inq, 0, &WUDSearchCallback);

    return WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT;
}

static u8 WUDiWaitForSearchResult(void)
{
    if (WUDiGetLinkNum() == WUD_MAX_CHANNELS && WUDiGetConnNumber() == WUD_MAX_CHANNELS)
    {

        DEBUGPrint("Cancel searching because 4 connections exist.\n");
        BTA_DmSearchCancel();

        return WUD_STATE_SYNC_CANCEL_SEARCH;
    }

    return WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT;
}

static u8 WUDiNextStepBySearchResult(void)
{
    WUDSyncState nextState = WUD_STATE_SYNC_PREPARE_SEARCH;
    if (_discNumResps > 0)
    {
        if (WUD_DEV_NAME_IS_CNT(_discResp.devName))
        {
            nextState = WUD_STATE_SYNC_IS_EXISTED_DEVICE;
        }
        if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS_WBC(_discResp.devName))
        {
            nextState = WUD_STATE_SYNC_IS_EXISTED_DEVICE;
        }
        if (_discRssi < _wcb.syncRssi)
        {
            nextState = WUD_STATE_SYNC_PREPARE_SEARCH;
        }
    }
    if (_wcb.syncType == WUD_SYNC_TYPE_STANDARD && nextState == WUD_STATE_SYNC_PREPARE_SEARCH)
    {
        nextState = WUD_STATE_SYNC_WAIT_FOR_INCOMING;
        _wcb.waitIncomingFrames = WUDiGetLinkNum() == WUD_MAX_CHANNELS - 1 ? 200 : 100;
        WUDSetVisibility(FALSE, TRUE);
    }
    return nextState;
}

static u8 WUDiCheckDeviceDataBase(void)
{
    WUDDevInfo* pInfo;
    WUDSyncState nextState = WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE;

    pInfo = WUDiGetDevInfo(_discResp.devAddr);

    if (pInfo != NULL)
    {
        memcpy(&_work, pInfo, sizeof(WUDDevInfo));
        nextState = WUD_STATE_SYNC_PREPARE_FOR_EXISTED_DEVICE;
    }

    return nextState;
}

static u8 WUDiExistedDevice(void)
{
    WUDDevInfo* pWork = &_work;

    pWork->status = 1;

    switch (pWork->UNK_0x5B)
    {
    case 0:
    case 2:
    case 4:
    case 5:
    {
        if (_wcb.syncType == WUD_SYNC_TYPE_STANDARD)
        {
            WUDiMoveBottomStdDevInfoPtr(pWork);
            WUDiRemoveDevice(pWork->devAddr);
            return WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE;
        }
        else
        {
            pWork->UNK_0x5B = 4;
        }

        break;
    }

    case 1:
    case 3:
    {
        WUDiMoveBottomSmpDevInfoPtr(pWork);
        WUDiRemoveDevice(_wcb.smpListTail->devInfo->devAddr);
        return WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE;
    }
    }

    return WUD_STATE_SYNC_TRY_CONNECT;
}

static u8 WUDiUnknownDevice(void)
{
    WUDDevInfo* pWork = &_work;

    pWork->status = 1;
    pWork->UNK_0x5B = _wcb.syncType == WUD_SYNC_TYPE_STANDARD ? 0 : 1;

    WUD_BDCPY(pWork->devAddr, _discResp.devAddr);
    memcpy(pWork->conf.devName, _discResp.devName, sizeof(_discResp.devName));
    memset(pWork->linkKey, 0, LINK_KEY_LEN);

    return WUD_STATE_SYNC_TRY_CONNECT;
}

static u8 WUDiTryConnecting(void)
{
    WUDSyncState nextState = WUD_STATE_ERROR;

    if (WUD_DEV_NAME_IS_CNT(_discResp.devName))
    {
        _work.status = 2;
        BTA_HhOpen(_work.devAddr, BTA_HH_PROTO_RPT_MODE, BTA_SEC_AUTHENTICATE);
        nextState = WUD_STATE_SYNC_6;
    }

    if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS_WBC(_discResp.devName))
    {
        WUDDevInfo* pInfo = WUDiGetWbcDevice();
        if (pInfo != NULL)
        {
            DEBUGPrint("Found the registered WBC in database\n");
            if (pInfo->status > 1)
            {
                return nextState;
            }
            if (WUD_BDCMP(_discResp.devAddr, pInfo->devAddr) != 0)
            {
                DEBUGPrint("Removed WBC data\n");
                WUDiMoveBottomStdDevInfoPtr(pInfo);
                WUDiRemoveDevice(pInfo->devAddr);
            }
        }
        _work.status = 2;
        BTA_HhOpen(_work.devAddr, BTA_HH_PROTO_RPT_MODE, BTA_SEC_AUTHENTICATE);
        nextState = WUD_STATE_SYNC_6;
    }
    return nextState;
}

static int WUDiSetPinCode(BD_ADDR addr)
{
    BD_ADDR pin;
    BD_ADDR_PTR pAddr;

    pAddr = _wcb.syncType == WUD_SYNC_TYPE_STANDARD ? _wcb.hostAddr : addr;

    if (_work.UNK_0x5B == 4)
    {
        WUDiRemoveDevice(_work.devAddr);
        btm_remove_acl(_work.devAddr);
    }

    _work.UNK_0x5A = 1;

    ReverseAddr(pin, pAddr);
    BTA_DmPinReply(addr, TRUE, BD_ADDR_LEN, pin);

    return 0;
}

static BOOL WUDiIsBusyLinkKeyCmd(void)
{
    return _wcb.linkKeyState != WUD_STATE_START ? TRUE : FALSE;
}

static BOOL WUDiIsBusyScCmd(void)
{
    return SCCheckStatus() == SC_STATUS_BUSY ? TRUE : FALSE;
}

static u8 WUDiVirginStandardSync(void)
{
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;

    if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS_CNT(_work.conf.devName))
    {
        pInfo = WUDiGetWbcDevice();
        if (pInfo != NULL && WUDiGetDevNumber() == WUD_MAX_DEV_ENTRY_FOR_STD)
        {
            pInfo = p->stdListTail->prev->devInfo;
            WUDiMoveBottomStdDevInfoPtr(pInfo);
            WUDiRemoveDevice(pInfo->devAddr);
        }
    }
    if (WUDiGetDevNumber() == WUD_MAX_DEV_ENTRY_FOR_STD)
    {
        pInfo = WUDiGetRemoveStdDevice();
        WUDiRemoveDevice(pInfo->devAddr);
    }

    pInfo = WUDiGetNewDevInfo();
    if (pInfo == NULL)
    {
        return WUD_STATE_ERROR;
    }
    if (pInfo->status != 0)
    {
        return WUD_STATE_ERROR;
    }
    memcpy(pInfo, &_work, sizeof(WUDDevInfo));

    WUDiRegisterDevice(pInfo->devAddr);
    WUDiMoveTopStdDevInfoPtr(pInfo);

    if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS_WBC(pInfo->conf.devName))
    {
        WUDiMoveTopOfUnusedStdDevice(pInfo);
        memset(&_scArray.devices[WUD_WBC_SC_INDEX], 0, sizeof(SCBtDeviceInfo));
        WUD_BDCPY(_scArray.devices[WUD_WBC_SC_INDEX].addr, pInfo->devAddr);
        memcpy(&_scArray.devices[WUD_WBC_SC_INDEX].info, &pInfo->conf, sizeof(SCDevInfo));
        memcpy(_scArray.devices[WUD_WBC_SC_INDEX].info.at_0x14, pInfo->linkKey, LINK_KEY_LEN);
        WUDiPrepareWbcDevInfo(pInfo->devAddr, pInfo->linkKey, &pInfo->conf);
    }
    WUDiSetDevAddrForHandle(pInfo->devHandle, pInfo->devAddr);

    return WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
}

static u8 WUDiSaveDeviceToNand(void)
{
    WUDSyncState nextState = WUD_STATE_SYNC_COMPLETE;
    u8 i;
    WUDDevInfoList* pIt;

    if (WUDiIsBusyScCmd())
    {
        return WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND;
    }

    memset(&_scArray.regist, 0, sizeof(SCBtDeviceInfo) * WUD_MAX_DEV_ENTRY_FOR_STD);

    _scArray.numRegist = WUDiGetDevNumber();

    for (pIt = _wcb.stdListHead, i = 0; pIt != NULL; i++, pIt = pIt->next)
    {
        WUD_BDCPY(_scArray.regist[i].addr, pIt->devInfo->devAddr);

        memcpy(&_scArray.regist[i].info, &pIt->devInfo->conf, sizeof(SCDevInfo));
    }

    DEBUGPrint("%d devices is stored into SC.\n", i);

    if (!SCSetBtDeviceInfoArray(&_scArray))
    {
        return WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND;
    }
    nextState = WUD_STATE_SYNC_SC_FLUSH;
    if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS_WBC(_work.conf.devName) && SCGetProductGameRegion() == 0)
    {
        nextState = WUD_STATE_WII_FIT_OPEN;
    }
    return nextState;
}

static void OpenWiiFitCallback(s32 result, NANDCommandBlock* block)
{
    if (_wcb.syncState != WUD_STATE_START)
    {
        _wcb.syncState = result == NAND_RESULT_OK ? WUD_STATE_WII_FIT_SEEK : WUD_STATE_SYNC_SC_FLUSH;
    }
    if (_wcb.deleteState != WUD_STATE_START)
    {
        _wcb.deleteState = result == NAND_RESULT_OK ? WUD_STATE_WII_FIT_SEEK : WUD_STATE_DELETE_CLEANUP_SETTING;
    }
    _nandLocked = FALSE;
    DEBUGPrint("NANDOpen. [%d]\n", result);
}

static void OpenWiiFitFile(void)
{
    const char* path = WUD_WII_FIT_PATH;
    if (_nandLocked)
    {
        return;
    }
    DEBUGPrint("WbcSaveData: %s\n", path);
    _nandLocked = TRUE;
    NANDOpenAsync(path, &_nandFileInfo, NAND_ACCESS_WRITE, OpenWiiFitCallback, &_nandBlock);
}

static void SeekWiiFitCallback(s32 result, NANDCommandBlock* block)
{
    if (_wcb.syncState != WUD_STATE_START)
    {
        _wcb.syncState = result == WUD_WII_FIT_PAIRING_OFFSET ? WUD_STATE_WII_FIT_UPDATE : WUD_STATE_SYNC_SC_FLUSH;
    }
    if (_wcb.deleteState != WUD_STATE_START)
    {
        _wcb.deleteState = result == WUD_WII_FIT_PAIRING_OFFSET ? WUD_STATE_WII_FIT_UPDATE : WUD_STATE_DELETE_CLEANUP_SETTING;
    }
    _nandLocked = FALSE;
    DEBUGPrint("NANDSeek. [%d]\n", result);
}

static void SeekWiiFitFile(void)
{
    if (_nandLocked)
    {
        return;
    }
    _nandLocked = TRUE;
    NANDSeekAsync(&_nandFileInfo, WUD_WII_FIT_PAIRING_OFFSET, NAND_SEEK_BEG, SeekWiiFitCallback, &_nandBlock);
}

static void UpdateWiiFitCallback(s32 result, NANDCommandBlock* block)
{
    if (_wcb.syncState != WUD_STATE_START)
    {
        _wcb.syncState = result == 132 ? WUD_STATE_WII_FIT_CLOSE : WUD_STATE_SYNC_SC_FLUSH;
    }
    if (_wcb.deleteState != WUD_STATE_START)
    {
        _wcb.deleteState = result == 132 ? WUD_STATE_WII_FIT_CLOSE : WUD_STATE_DELETE_CLEANUP_SETTING;
    }
    _nandLocked = FALSE;
    DEBUGPrint("NANDWrite. [%d]\n", result);
}

static void UpdateWiiFitFile(void)
{
    u8* crc;
    if (_nandLocked)
    {
        return;
    }
    DEBUGPrint("WBC to WiiFit\n");
    DEBUGPrint(" %02x %02x %02x %02x %02x %02x\n",
        _nandWbcInfo.devAddr[0],
        _nandWbcInfo.devAddr[1],
        _nandWbcInfo.devAddr[2],
        _nandWbcInfo.devAddr[3],
        _nandWbcInfo.devAddr[4],
        _nandWbcInfo.devAddr[5]);
    DEBUGPrint(" %02x %02x %02x %02x %02x %02x %02x %02x\n",
        _nandWbcInfo.linkKey[0],
        _nandWbcInfo.linkKey[1],
        _nandWbcInfo.linkKey[2],
        _nandWbcInfo.linkKey[3],
        _nandWbcInfo.linkKey[4],
        _nandWbcInfo.linkKey[5],
        _nandWbcInfo.linkKey[6],
        _nandWbcInfo.linkKey[7]);
    DEBUGPrint(" %02x %02x %02x %02x %02x %02x %02x %02x\n",
        _nandWbcInfo.linkKey[8],
        _nandWbcInfo.linkKey[9],
        _nandWbcInfo.linkKey[10],
        _nandWbcInfo.linkKey[11],
        _nandWbcInfo.linkKey[12],
        _nandWbcInfo.linkKey[13],
        _nandWbcInfo.linkKey[14],
        _nandWbcInfo.linkKey[15]);
    DEBUGPrint(" %s\n", _nandWbcInfo.info.devName);
    crc = (u8*)&_nandWbcInfo.crc;
    DEBUGPrint(" %02x%02x%02x%02x\n", crc[0], crc[1], crc[2], crc[3]);
    _nandLocked = TRUE;
    NANDWriteAsync(&_nandFileInfo, &_nandWbcInfo, 132, UpdateWiiFitCallback, &_nandBlock);
}

static void CloseWiiFitCallback(s32 result, NANDCommandBlock* block)
{
    if (_wcb.syncState != WUD_STATE_START)
    {
        _wcb.syncState = WUD_STATE_SYNC_SC_FLUSH;
    }
    if (_wcb.deleteState != WUD_STATE_START)
    {
        _wcb.deleteState = WUD_STATE_DELETE_CLEANUP_SETTING;
    }
    _nandLocked = FALSE;
    DEBUGPrint("NANDClose. [%d]\n", result);
}

static void CloseWiiFitFile(void)
{
    if (_nandLocked)
    {
        return;
    }
    _nandLocked = TRUE;
    NANDCloseAsync(&_nandFileInfo, CloseWiiFitCallback, &_nandBlock);
}

static void WUDiSaveFlush(void)
{
    WUDCB* p = &_wcb;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    p->syncState = WUD_STATE_SYNC_13;
    OSRestoreInterrupts(enabled);

    SCFlushAsync(SyncFlushCallback);
}

static u8 WUDiVirginSimpleSync(void)
{
    WUDDevInfo* pInfo;

    if (WUDiGetDevSmpNumber() == WUD_MAX_DEV_ENTRY_FOR_SMP)
    {
        pInfo = WUDiGetRemoveSmpDevice();
        WUDiRemoveDevice(pInfo->devAddr);
    }

    pInfo = WUDiGetNewDevInfo();
    if (pInfo == NULL)
    {
        return WUD_STATE_ERROR;
    }
    if (pInfo->status != 0)
    {
        return WUD_STATE_ERROR;
    }
    memcpy(pInfo, &_work, sizeof(WUDDevInfo));

    WUDiRegisterDevice(pInfo->devAddr);
    WUDiMoveTopSmpDevInfoPtr(pInfo);

    WUDiSetDevAddrForHandle(pInfo->devHandle, pInfo->devAddr);

    return WUD_STATE_SYNC_COMPLETE;
}

static u8 WUDiChangeSimpleToStandard(void)
{
    WUDiMoveBottomSmpDevInfoPtr(&_work);
    WUDiRemoveDevice(_wcb.smpListTail->devInfo->devAddr);
    return WUDiVirginStandardSync();
}

static u8 WUDiRegisterSyncDevice(void)
{
    WUDSyncState nextState;

    switch (_work.UNK_0x5B)
    {
    case 1:
    {
        nextState = WUD_STATE_SYNC_VIRGIN_SIMPLE;
        break;
    }

    case 0:
    {
        nextState = WUD_STATE_SYNC_VIRGIN_STANDARD;
        break;
    }

    case 5:
    {
        nextState = WUD_STATE_SYNC_CHANGE_SIMPLE_TO_STANDARD;
        break;
    }

    case 3:
    {
        nextState = WUD_STATE_SYNC_COMPLETE;
        break;
    }

    case 4:
    {
        nextState = WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
        break;
    }

    case 2:
    {
        nextState = WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
        break;
    }

    default:
    {
        // UB
        break;
    }
    }

    return nextState;
}

static u8 WUDiStoredLinkKey(void)
{
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;

    if (WUDiIsBusyLinkKeyCmd())
    {
        return WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
    }

    if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS_WBC(_work.conf.devName))
    {
        return WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND;
    }
    pInfo = WUDiGetDevInfo(_work.devAddr);

    DEBUGPrint("write stored link key\n");

    DEBUGPrint("addr : %02x:%02x:%02x:%02x:%02x:%02x\n",
        pInfo->devAddr[0],
        pInfo->devAddr[1],
        pInfo->devAddr[2],
        pInfo->devAddr[3],
        pInfo->devAddr[4],
        pInfo->devAddr[5]);

    DEBUGPrint("key  : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
        pInfo->linkKey[0],
        pInfo->linkKey[1],
        pInfo->linkKey[2],
        pInfo->linkKey[3],
        pInfo->linkKey[4],
        pInfo->linkKey[5],
        pInfo->linkKey[6],
        pInfo->linkKey[7],
        pInfo->linkKey[8],
        pInfo->linkKey[9],
        pInfo->linkKey[10],
        pInfo->linkKey[11],
        pInfo->linkKey[12],
        pInfo->linkKey[13],
        pInfo->linkKey[14],
        pInfo->linkKey[15]);

    p->linkKeyState = WUD_STATE_LINK_KEY_WRITING;

    BTM_WriteStoredLinkKey(1, &pInfo->devAddr, &pInfo->linkKey, WUDStoredLinkKeyCallback);

    return WUD_STATE_SYNC_WAIT_FOR_STORING;
}

static u8 WUDiWaitFinishStoring(void)
{
    return !WUDiIsBusyLinkKeyCmd() ? WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND
                                   : WUD_STATE_SYNC_WAIT_FOR_STORING;
}

static u8 WUDiWaitFinishReading(void)
{
    WUDSyncState nextState = WUD_STATE_SYNC_WAIT_FOR_READING;

    if (!WUDiIsBusyLinkKeyCmd())
    {
        nextState = WUD_STATE_SYNC_VIRGIN_STANDARD;
        WUDiRemoveDevice(_wcb.pairAddr);
    }

    return nextState;
}

static u8 WUDiSyncComplete(void)
{
    _wcb.syncedNum++;

    memset(&_work, 0, sizeof(WUDDevInfo));

    return _wcb.syncType == WUD_SYNC_TYPE_STANDARD
             ? WUD_STATE_SYNC_DONE
             : WUD_STATE_SYNC_PREPARE_SEARCH;
}

static u8 WUDiSyncError(void)
{
    memset(&_work, 0, sizeof(WUDDevInfo));

    return _wcb.syncType == WUD_SYNC_TYPE_STANDARD
             ? WUD_STATE_SYNC_DONE
             : WUD_STATE_SYNC_PREPARE_SEARCH;
}

static u8 WUDiSyncDone(void)
{
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    int i;
    WUDSyncDeviceCallback pSyncCallback;

    if (p->syncSkipChecks)
    {
        for (i = 0; i < WUD_MAX_DEV_ENTRY; i++)
        {
            pInfo = WUDiGetDevInfoIndex(i);

            if (pInfo->status == 8)
            {
                WUDSetSniffMode(pInfo->devAddr, 8);
                return WUD_STATE_SYNC_DONE;
            }
        }
    }

    OSCancelAlarm(&p->alarm);

    if (!_abortSync)
    {
        WUDSetVisibility(FALSE, TRUE);
    }

    pSyncCallback = p->syncType == WUD_SYNC_TYPE_STANDARD ? p->syncStdCB : p->syncSmpCB;

    if (pSyncCallback != NULL)
    {
        pSyncCallback(WUD_RESULT_SYNC_DONE, p->syncedNum);
    }

    DEBUGPrint("Pairing Done\n");
    return WUD_STATE_START;
}

static void SyncHandler(void)
{
    WUDCB* p = &_wcb;

    switch (p->syncState)
    {
    case WUD_STATE_SYNC_PREPARE_SEARCH:
    {
        p->syncState = WUDiIsSyncDisabled();
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_START_SEARCH:
        p->syncState = WUDiWaitForStartSearch();
        break;

    case WUD_STATE_SYNC_START_SEARCH:
    {
        p->syncState = WUDiStartSearch();
        break;
    }

    case WUD_STATE_SYNC_CHECK_SEARCH_RESULT:
    {
        p->syncState = WUDiNextStepBySearchResult();
        break;
    }

    case WUD_STATE_SYNC_IS_EXISTED_DEVICE:
    {
        p->syncState = WUDiCheckDeviceDataBase();
        break;
    }

    case WUD_STATE_SYNC_PREPARE_FOR_EXISTED_DEVICE:
    {
        p->syncState = WUDiExistedDevice();
        break;
    }

    case WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE:
    {
        p->syncState = WUDiUnknownDevice();
        break;
    }

    case WUD_STATE_SYNC_TRY_CONNECT:
    {
        p->syncState = WUDiTryConnecting();
        break;
    }

    case WUD_STATE_SYNC_REGISTER_DEVICE:
    {
        p->syncState = WUDiRegisterSyncDevice();
        break;
    }

    case WUD_STATE_SYNC_VIRGIN_SIMPLE:
    {
        p->syncState = WUDiVirginSimpleSync();
        break;
    }

    case WUD_STATE_SYNC_VIRGIN_STANDARD:
    {
        p->syncState = WUDiVirginStandardSync();
        break;
    }

    case WUD_STATE_SYNC_CHANGE_SIMPLE_TO_STANDARD:
    {
        p->syncState = WUDiChangeSimpleToStandard();
        break;
    }

    case WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM:
    {
        p->syncState = WUDiStoredLinkKey();
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_STORING:
    {
        p->syncState = WUDiWaitFinishStoring();
        break;
    }

    case WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND:
    {
        p->syncState = WUDiSaveDeviceToNand();
        break;
    }

    case WUD_STATE_SYNC_SC_FLUSH:
    {
        WUDiSaveFlush();
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_READING:
    {
        p->syncState = WUDiWaitFinishReading();
        break;
    }

    case WUD_STATE_SYNC_COMPLETE:
    {
        p->syncState = WUDiSyncComplete();
        break;
    }

    case WUD_STATE_ERROR:
    {
        p->syncState = WUDiSyncError();
        break;
    }

    case WUD_STATE_SYNC_DONE:
    {
        p->syncState = WUDiSyncDone();
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT:
    {
        p->syncState = WUDiWaitForSearchResult();
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_INCOMING:
        p->syncState = WUDiWaitForIncoming();
        break;
    case WUD_STATE_WII_FIT_OPEN:
        OpenWiiFitFile();
        break;
    case WUD_STATE_WII_FIT_SEEK:
        SeekWiiFitFile();
        break;
    case WUD_STATE_WII_FIT_UPDATE:
        UpdateWiiFitFile();
        break;
    case WUD_STATE_WII_FIT_CLOSE:
        CloseWiiFitFile();
        break;
    case WUD_STATE_SYNC_6:
    case WUD_STATE_SYNC_13:
    case WUD_STATE_SYNC_CANCEL_SEARCH:
    {
        break;
    }

    case WUD_STATE_SYNC_11:
    case WUD_STATE_SYNC_12:
    default:
    {
        DEBUGPrint("WARNING: Illigal status\n");
        break;
    }
    }
}

static void SyncHandler0(OSAlarm* pAlarm, OSContext* pContext)
{
    OSSwitchFiberEx((u32)pAlarm, (u32)pContext, 0, 0, SyncHandler, __WUDHandlerStack + sizeof(__WUDHandlerStack));
}

static void DeleteFlushCallback(SCStatus status)
{
    WUDCB* p = &_wcb;

    DEBUGPrint("__wudDeleteFlushCallback() : %d, Delete: %d\n", status, _wcb.deleteState);

    if (p->deleteState == WUD_STATE_START)
    {
        return;
    }

    if (status == SC_STATUS_OK)
    {
        p->deleteState = WUD_STATE_DELETE_DONE;
    }
    else
    {
        p->deleteState = WUD_STATE_DELETE_DONE;
    }
}

static WUDDeleteState WUDiDisallowIncoming(void)
{
    BTA_DmSetVisibility(FALSE, FALSE);
    return WUD_STATE_DELETE_DISCONNECT_ALL;
}

static WUDDeleteState WUDiTerminateDevice(void)
{
    int i;
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++)
    {
        pInfo = &p->stdDevs[i];

        if (pInfo->status > 1)
        {
            btm_remove_acl(pInfo->devAddr);
        }
    }

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++)
    {
        pInfo = &p->smpDevs[i];

        if (pInfo->status > 1)
        {
            btm_remove_acl(pInfo->devAddr);
        }
    }

    return WUD_STATE_DELETE_CLEANUP_DATABASE;
}

static WUDDeleteState WUDiDeleteDevice(void)
{
    int i;
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;

    if (WUDiGetLinkNum() > 0)
    {
        return WUD_STATE_DELETE_CLEANUP_DATABASE;
    }

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++)
    {
        pInfo = &p->stdDevs[i];

        if (pInfo->status == 1)
        {
            WUDiRemoveDevice(pInfo->devAddr);
        }
    }

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++)
    {
        pInfo = &p->smpDevs[i];

        if (pInfo->status == 1)
        {
            WUDiRemoveDevice(pInfo->devAddr);
        }
    }

    if (WUDIsLinkedWBC())
    {
        if (SCGetProductGameRegion() == 0)
        {
            LINK_KEY key = { 0 };
            WUDiPrepareWbcDevInfo(NULL, key, NULL);
            return WUD_STATE_WII_FIT_OPEN;
        }
        else
        {
            return WUD_STATE_DELETE_CLEANUP_SETTING;
        }
    }
    else
    {
        return WUD_STATE_DELETE_CLEANUP_SETTING;
    }
}

static void WUDiCleanUp(void)
{
    WUDCB* p = &_wcb;
    BOOL success = FALSE;

    if (SCCheckStatus() == SC_STATUS_BUSY)
    {
        return;
    }

    memset(&_scArray, 0, sizeof(SCBtDeviceInfoArray));
    memset(&_spArray, 0, sizeof(_spArray));
    success |= SCSetBtDeviceInfoArray(&_scArray);
    success |= SCSetBtCmpDevInfoArray(&_spArray);

    if (success)
    {
        p->deleteState = WUD_STATE_DELETE_6;
        SCFlushAsync(DeleteFlushCallback);
    }
    else
    {
        p->deleteState = WUD_STATE_DELETE_DONE;
    }
}

static WUDDeleteState WUDiDeleteAllComplete(void)
{
    WUDCB* p = &_wcb;

    WUDSetVisibility(FALSE, WUDGetConnectable());

    OSCancelAlarm(&p->alarm);
    p->deleteState = WUD_STATE_START;

    if (p->clearDevCB != NULL)
    {
        p->clearDevCB(WUD_RESULT_DELETE_COMPLETE);
    }

    return WUD_STATE_START;
}

static void DeleteAllHandler(void)
{
    WUDCB* p = &_wcb;

    p->syncState = WUD_STATE_START;

    switch (p->deleteState)
    {
    case WUD_STATE_DELETE_DISALLOW_INCOMING:
    {
        p->deleteState = WUDiDisallowIncoming();
        break;
    }

    case WUD_STATE_DELETE_DISCONNECT_ALL:
    {
        p->deleteState = WUDiTerminateDevice();
        break;
    }

    case WUD_STATE_DELETE_CLEANUP_DATABASE:
    {
        p->deleteState = WUDiDeleteDevice();
        break;
    }

    case WUD_STATE_DELETE_CLEANUP_SETTING:
    {
        WUDiCleanUp();
        break;
    }

    case WUD_STATE_DELETE_DONE:
    {
        p->deleteState = WUDiDeleteAllComplete();
        break;
    }

    case WUD_STATE_WII_FIT_OPEN:
        OpenWiiFitFile();
        break;
    case WUD_STATE_WII_FIT_SEEK:
        SeekWiiFitFile();
        break;
    case WUD_STATE_WII_FIT_UPDATE:
        UpdateWiiFitFile();
        break;
    case WUD_STATE_WII_FIT_CLOSE:
        CloseWiiFitFile();
        break;
    case WUD_STATE_DELETE_7:
    {
        break;
    }
    }
}

static void DeleteAllHandler0(OSAlarm* pAlarm, OSContext* pContext)
{
    OSSwitchFiberEx((u32)pAlarm, (u32)pContext, 0, 0, DeleteAllHandler, __WUDHandlerStack + sizeof(__WUDHandlerStack));
}

static WUDStackState WUDiCheckRegisteredDeviceInfo(void)
{
    WUDCB* p = &_wcb;

    // Set by BTA_HH_ENABLE_EVT
    if (p->hhFlags == 1)
    {
        p->linkKeyState = WUD_STATE_LINK_KEY_READING;
        BTM_ReadStoredLinkKey(NULL, WUDStoredLinkKeyCallback);
    }

    return WUD_STATE_STACK_CHECK_DEVICE_INFO;
}

static WUDStackState WUDiClearUnregisteredDevice(void)
{
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    int i;

    if (p->linkKeyState == WUD_STATE_START)
    {
        for (i = 0; i < WUD_MAX_DEV_ENTRY; i++)
        {
            pInfo = WUDiGetDevInfoIndex(i);
            if (pInfo->status == 0)
            {
                continue;
            }

            if (pInfo->UNK_0x5C == 1)
            {
                p->linkKeyState = WUD_STATE_LINK_KEY_DELETING;
                BTM_DeleteStoredLinkKey(pInfo->devAddr,
                    WUDStoredLinkKeyCallback);

                pInfo->UNK_0x5C = 0;
                return WUD_STATE_STACK_CHECK_DEVICE_INFO;
            }

            if (pInfo->UNK_0x5C != 3)
            {
                WUDiRemoveDevInfo(pInfo->devAddr);
            }
        }

        return WUD_STATE_STACK_DONE;
    }

    return WUD_STATE_STACK_CHECK_DEVICE_INFO;
}

static WUDStackState WUDiStackSetupComplete(void)
{
    WUDCB* p = &_wcb;

    if (p->linkKeyState == WUD_STATE_START)
    {
        OSCancelAlarm(&p->alarm);
        WUDiGetFirmwareVersion();
        return WUD_STATE_STACK_INITIALIZED;
    }

    return WUD_STATE_STACK_CHECK_DEVICE_INFO;
}

static void EnableStackHandler(void)
{
    WUDCB* p = &_wcb;

    switch (p->stackState)
    {
    case WUD_STATE_STACK_GET_STORED_LINK_KEY:
    {
        p->stackState = WUDiCheckRegisteredDeviceInfo();
        break;
    }

    case WUD_STATE_STACK_CHECK_DEVICE_INFO:
    {
        p->stackState = WUDiClearUnregisteredDevice();
        break;
    }

    case WUD_STATE_STACK_DONE:
    {
        p->stackState = WUDiStackSetupComplete();
        break;
    }
    }
}

static void EnableStackHandler0(OSAlarm* pAlarm, OSContext* pContext)
{
    OSSwitchFiberEx((u32)pAlarm, (u32)pContext, 0, 0, EnableStackHandler, __WUDHandlerStack + sizeof(__WUDHandlerStack));
}

static WUDInitState WUDiWaitSCSetup(void)
{
    s32 diff = 0;
    WUDInitState nextState = WUD_STATE_INIT_WAIT_FOR_INITIALIZATION;
    s64 time = __OSGetSystemTime();

    diff = 500 - OS_TICKS_TO_MSEC(OS_TICKS_DELTA(time, __OSStartTime));
    if (diff < 0 && SCCheckStatus() != SC_STATUS_BUSY)
    {
        InitCore();
        nextState = WUD_STATE_INIT_GET_DEV_INFO;
        if (WUDIsLinkedWBC() && SCGetProductGameRegion() == 0)
        {
            nextState = WUD_STATE_INIT_GET_DEV_INFO_FROM_WII_FIT;
            _nandLocked = FALSE;
            _nandPhase = WUD_STATE_START;
        }
    }
    return nextState;
}

static void InitFlushCallback(SCStatus status)
{
    WUDCB* p = &_wcb;
    DEBUGPrint("__wudInitFlushCallback() : %d, Init: %d\n", status, _wcb.initState);
    p->initState = WUD_STATE_INIT_DONE;
}

static BOOL WUDiIsRegisteredDevice(BD_ADDR addr)
{
    int i;
    for (i = 0; i < _scArray.numRegist; i++)
    {
        if (WUD_BDCMP(_scArray.devices[i].addr, addr) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

static WUDInitState WUDiGetRegisteredDevice(void)
{
    BD_ADDR dev_addr;
    int i, j;
    u32 arrayNum;
    u8 a;
    WUDDevInfo* p_info;

    memset(&_scArray, 0, sizeof _scArray);
    memset(&_spArray, 0, sizeof _spArray);
    memset(dev_addr, 0, BD_ADDR_LEN);

    SCGetBtDeviceInfoArray(&_scArray);
    SCGetBtCmpDevInfoArray(&_spArray);

    _wcb.syncType = WUD_SYNC_TYPE_STANDARD;
    for (i = 0, arrayNum = _scArray.numRegist, a = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++)
    {
        if (!arrayNum)
            break;

        if (!WUD_DEV_NAME_IS_CNT(_scArray.devices[i].info.devName))
            memset(&_scArray.devices[i], 0, sizeof _scArray.devices[i]);

        if (WUD_BDCMP(_scArray.devices[i].addr, dev_addr) == 0)
        {
            if (i < WUD_MAX_DEV_ENTRY_FOR_STD - 1)
            {
                for (j = i + 1; j < WUD_MAX_DEV_ENTRY_FOR_STD; j++)
                {
                    if (WUD_DEV_NAME_IS_CNT(_scArray.devices[j].info.devName))
                    {
                        memcpy(
                            &_scArray.devices[i],
                            &_scArray.devices[j],
                            sizeof _scArray.devices[i]);
                        memset(&_scArray.devices[j], 0, sizeof _scArray.devices[i]);

                        goto there;
                    }
                }
            }
        }
        else
        {
        there:
            p_info = WUDiGetNewDevInfo();
            if (p_info)
            {
                WUD_BDCPY(p_info->devAddr, _scArray.devices[i].addr);
                memcpy(&p_info->conf, &_scArray.devices[i].info, sizeof p_info->conf);

                p_info->status = 1;
                p_info->UNK_0x5B = 0;
                p_info->UNK_0x5C = 2;
                p_info->subclass = 2; // Reserved HID subclass.
                p_info->hhAttrMask = 0x8074;
                p_info->appID = 3;

                DEBUGPrint("addr : %02x:%02x:%02x:%02x:%02x:%02x\n",
                    p_info->devAddr[0],
                    p_info->devAddr[1],
                    p_info->devAddr[2],
                    p_info->devAddr[3],
                    p_info->devAddr[4],
                    p_info->devAddr[5]);
                DEBUGPrint("name : %s\n", p_info->conf.devName);

                a++;
                arrayNum--;
            }
        }
    }

    if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS_WBC(_scArray.devices[WUD_WBC_SC_INDEX].info.devName))
    {
        p_info = WUDiGetNewDevInfo();
        if (!p_info)
        {
            p_info = WUDiGetDevInfoIndex(9);
            a--;
        }

        WUD_BDCPY(p_info->devAddr, _scArray.devices[WUD_WBC_SC_INDEX].addr);
        memcpy(p_info->conf.devName, _scArray.devices[WUD_WBC_SC_INDEX].info.devName, 19);
        memcpy(p_info->linkKey, _scArray.devices[WUD_WBC_SC_INDEX].info.at_0x14, LINK_KEY_LEN);

        p_info->status = 1;
        p_info->UNK_0x5B = 0;
        p_info->subclass = 2;
        p_info->hhAttrMask = 0x8074;
        p_info->appID = 3;
        p_info->UNK_0x5C = 3;

        DEBUGPrint("addr : %02x:%02x:%02x:%02x:%02x:%02x\n",
            p_info->devAddr[0],
            p_info->devAddr[1],
            p_info->devAddr[2],
            p_info->devAddr[3],
            p_info->devAddr[4],
            p_info->devAddr[5]);
        DEBUGPrint("name : %s\n", p_info->conf.devName);

        memset(&_scArray.devices[a], 0, sizeof _scArray.devices[a]);
        WUD_BDCPY(_scArray.devices[a].addr, _scArray.devices[WUD_WBC_SC_INDEX].addr);
        memcpy(_scArray.devices[a].info.devName, _scArray.devices[WUD_WBC_SC_INDEX].info.devName, 19);

        a++;
    }

    _scArray.numRegist = a;
    _wcb.syncType = WUD_SYNC_TYPE_SIMPLE;

    for (i = 5, arrayNum = _spArray.numRegist; i >= 0; i--)
    {
        if (!arrayNum)
            break;

        if (WUD_BDCMP(_spArray.devices[i].addr, dev_addr) == 0)
            continue;

        if (WUDiIsRegisteredDevice(_spArray.devices[i].addr))
            continue;

        p_info = WUDiGetNewDevInfo();
        if (p_info)
        {
            WUD_BDCPY(p_info->devAddr, _spArray.devices[i].addr);
            memcpy(&p_info->conf, _spArray.devices[i].name, sizeof p_info->conf);
            memcpy(p_info->linkKey, _spArray.devices[i].linkKey, LINK_KEY_LEN);

            p_info->status = 1;
            p_info->UNK_0x5B = 1;
            p_info->UNK_0x5C = 3;

            if (WUD_DEV_NAME_IS_CNT_01(p_info->conf.devName))
            {
                p_info->subclass = 2;
                p_info->hhAttrMask = 0x8074;
                p_info->appID = 3;
            }

            DEBUGPrint("addr : %02x:%02x:%02x:%02x:%02x:%02x\n",
                p_info->devAddr[0],
                p_info->devAddr[1],
                p_info->devAddr[2],
                p_info->devAddr[3],
                p_info->devAddr[4],
                p_info->devAddr[5]);
            DEBUGPrint("name : %s\n", p_info->conf.devName);

            WUDiMoveTopSmpDevInfoPtr(p_info);

            arrayNum--;
        }
    }

    _wcb.syncType = WUD_SYNC_TYPE_STANDARD;
    _wcb.initState = WUD_STATE_INIT_UNK5;

    memset(&_spArray, 0, sizeof _spArray);
    SCSetBtDeviceInfoArray(&_scArray);
    SCSetBtCmpDevInfoArray(&_spArray);
    SCFlushAsync(&InitFlushCallback);

    return WUD_STATE_INIT_DONE;
}

static WUDInitState WUDiInitComplete(void)
{
    WUDCB* p = &_wcb;

    OSCancelAlarm(&p->alarm);
    p->libStatus = WUD_LIB_STATUS_1;

    BTA_EnableBluetooth(WUDSecurityCallback);
    return WUD_STATE_INIT_INITIALIZED;
}

static void NandResultCallback(s32 result, NANDCommandBlock* block)
{
    _nandLocked = FALSE;
    switch (_nandPhase)
    {
    case WUD_NAND_OPEN:
        _nandPhase = result == NAND_RESULT_OK ? _nandPhase + 1 : WUD_STATE_ERROR;
        break;
    case WUD_NAND_SEEK:
        _nandPhase = result == 0 ? _nandPhase + 1 : WUD_NAND_CLOSE;
        break;
    case WUD_NAND_READ:
        _nandPhase = result == WUD_WII_FIT_SAVE_SIZE ? _nandPhase + 1 : WUD_NAND_CLOSE;
        break;
    default:
        _nandPhase = WUD_NAND_DONE;
        break;
    }
}

static void NandFlushCallback(SCStatus status)
{
    _nandLocked = FALSE;
    _nandPhase++;
}

static u32 WUDiCalcWiiFitCrc(void* buffer, u32 bytes)
{
    u16* data = buffer;
    u32 hi = 0;
    u16 lo = 0;
    u32 i;
    u32 length = bytes / sizeof(*data);
    for (i = 0; i < length; i++, data++)
    {
        hi += *data;
        lo += ~*data;
    }
    return (u16)hi << 16 | lo;
}

static u32 WUDiUpdateWiiFitCrc(void* buffer, u32 bytes, u32 crc)
{
    u16* data = buffer;
    u32 hi = (u16)(crc >> 16);
    u16 lo = (u16)crc;
    u32 i;
    u32 length = bytes / sizeof(*data);
    for (i = 0; i < length; i++, data++)
    {
        hi += *data;
        lo += ~*data;
    }
    return (u16)hi << 16 | lo;
}

static WUDInitState WUDiGetWiiFitDevice(void)
{
    const char* path = WUD_WII_FIT_PATH;
    const char* title = "RFNJ";
    u8 zero[16] = { 0 };
    WUDInitState nextState = WUD_STATE_INIT_GET_DEV_INFO_FROM_WII_FIT;
    s32 result;
    u32 savedCrc;
    u32 actualCrc;

    if (_nandLocked)
    {
        return nextState;
    }
    if (!_nandBufPtr || ((u32)_nandBufPtr & 31) != 0)
    {
        OSPanic(__FILE__, 2704, "Error: the workarea for synchronizing WBC pairing information is not allocated.\n");
    }
    switch (_nandPhase)
    {
    case WUD_STATE_START:
        result = NANDInit();
        _nandPhase = result == NAND_RESULT_OK ? WUD_NAND_OPEN : WUD_STATE_ERROR;
        DEBUGPrint("NANDInit. [%d]\n", result);
        break;
    case WUD_NAND_OPEN:
        if (SCGetProductGameRegion() == 0)
        {
            DEBUGPrint("WbcSaveData: %s\n", path);
            _nandLocked = TRUE;
            _nandWbcCrc = 0;
            result = NANDOpenAsync(path, &_nandFileInfo, NAND_ACCESS_READ, NandResultCallback, &_nandBlock);
        }
        else
        {
            _nandPhase = WUD_NAND_DONE;
        }
        break;
    case WUD_NAND_SEEK:
        _nandLocked = TRUE;
        result = NANDSeekAsync(&_nandFileInfo, 0, NAND_SEEK_BEG, NandResultCallback, &_nandBlock);
        break;
    case WUD_NAND_READ:
        _nandLocked = TRUE;
        result = NANDReadAsync(&_nandFileInfo, _nandBufPtr, WUD_WII_FIT_SAVE_SIZE, NandResultCallback, &_nandBlock);
        break;
    case WUD_NAND_GET_DEV_INFO:
        memcpy(&_nandWbcInfo, _nandBufPtr + WUD_WII_FIT_PAIRING_OFFSET, WUD_WII_FIT_PAIRING_SIZE);
        memcpy(&savedCrc, _nandBufPtr + WUD_WII_FIT_PAIRING_OFFSET + WUD_WII_FIT_PAIRING_SIZE, sizeof(savedCrc));
        DEBUGPrint("CRC  = %d:%08x\n", savedCrc, savedCrc);
        _nandWbcCrc = WUDiCalcWiiFitCrc(_nandBufPtr, WUD_WII_FIT_PAIRING_OFFSET);
        actualCrc = WUDiUpdateWiiFitCrc(_nandBufPtr + WUD_WII_FIT_PAIRING_OFFSET,
            WUD_WII_FIT_PAIRING_SIZE,
            _nandWbcCrc);
        DEBUGPrint("CRC' = %d:%08x\n", actualCrc, actualCrc);
        if (savedCrc != actualCrc)
        {
            _nandPhase = WUD_NAND_CLOSE;
            DEBUGPrint("CRC Error on WiiFit SaveData.\n");
            break;
        }
        if (WUD_BDCMP(_nandWbcInfo.devAddr, zero) == 0)
        {
            _nandPhase = WUD_NAND_CLOSE;
            DEBUGPrint("WiiFit does not register WBC yet.\n");
            break;
        }
        if (memcmp(OSGetAppGamename(), title, 4) != 0 && memcmp(_nandWbcInfo.linkKey, zero, LINK_KEY_LEN) == 0)
        {
            _nandPhase = WUD_NAND_CLOSE;
            DEBUGPrint("LinkKey saved in WiiFit SaveData is invalid.\n");
            break;
        }
        memset(_nandBufPtr, 0, WUD_WII_FIT_SAVE_SIZE);
        memset(&_scArray, 0, sizeof(_scArray));
        SCGetBtDeviceInfoArray(&_scArray);
        WUD_BDCPY(_scArray.devices[WUD_WBC_SC_INDEX].addr, _nandWbcInfo.devAddr);
        memcpy(&_scArray.devices[WUD_WBC_SC_INDEX].info, &_nandWbcInfo.info, sizeof(SCDevInfo));
        memcpy(_scArray.devices[WUD_WBC_SC_INDEX].info.at_0x14, _nandWbcInfo.linkKey, LINK_KEY_LEN);
        DEBUGPrint("WBC from WiiFit\n");
        DEBUGPrint(" %02x %02x %02x %02x %02x %02x\n",
            _nandWbcInfo.devAddr[0],
            _nandWbcInfo.devAddr[1],
            _nandWbcInfo.devAddr[2],
            _nandWbcInfo.devAddr[3],
            _nandWbcInfo.devAddr[4],
            _nandWbcInfo.devAddr[5]);
        DEBUGPrint(" %02x %02x %02x %02x %02x %02x %02x %02x\n",
            _nandWbcInfo.linkKey[0],
            _nandWbcInfo.linkKey[1],
            _nandWbcInfo.linkKey[2],
            _nandWbcInfo.linkKey[3],
            _nandWbcInfo.linkKey[4],
            _nandWbcInfo.linkKey[5],
            _nandWbcInfo.linkKey[6],
            _nandWbcInfo.linkKey[7]);
        DEBUGPrint(" %02x %02x %02x %02x %02x %02x %02x %02x\n",
            _nandWbcInfo.linkKey[8],
            _nandWbcInfo.linkKey[9],
            _nandWbcInfo.linkKey[10],
            _nandWbcInfo.linkKey[11],
            _nandWbcInfo.linkKey[12],
            _nandWbcInfo.linkKey[13],
            _nandWbcInfo.linkKey[14],
            _nandWbcInfo.linkKey[15]);
        DEBUGPrint(" %s\n", _nandWbcInfo.info.devName);
        SCSetBtDeviceInfoArray(&_scArray);
        _nandLocked = TRUE;
        SCFlushAsync(NandFlushCallback);
        break;
    case WUD_NAND_CLOSE:
        _nandLocked = TRUE;
        result = NANDCloseAsync(&_nandFileInfo, NandResultCallback, &_nandBlock);
        break;
    default:
        nextState = WUD_STATE_INIT_GET_DEV_INFO;
        break;
    }
    return nextState;
}

static void InitHandler(void)
{
    WUDCB* p = &_wcb;
    switch (p->initState)
    {
    case WUD_STATE_INIT_WAIT_FOR_INITIALIZATION:
        p->initState = WUDiWaitSCSetup();
        break;
    case WUD_STATE_INIT_GET_DEV_INFO:
        WUDiGetRegisteredDevice();
        break;
    case WUD_STATE_INIT_DONE:
        p->initState = WUDiInitComplete();
        break;
    case WUD_STATE_INIT_GET_DEV_INFO_FROM_WII_FIT:
        p->initState = WUDiGetWiiFitDevice();
        break;
    }
}

static void InitHandler0(OSAlarm* pAlarm, OSContext* pContext)
{
    OSSwitchFiberEx((u32)pAlarm, (u32)pContext, 0, 0, InitHandler, __WUDHandlerStack + sizeof(__WUDHandlerStack));
}

static void ShutFlushCallback(SCStatus status)
{
    WUDCB* p = &_wcb;

    DEBUGPrint("__wudShutdownFlushCallback() : %d, Shutdown: %d\n", status, _wcb.shutdownState);

    p->shutdownState = WUD_STATE_SHUTDOWN_DONE;
}

static void WUDiContMapTableFlush(void)
{
    BOOL success = _readNand;

    if (SCCheckStatus() == SC_STATUS_BUSY)
    {
        return;
    }

    success &= SCSetBtDeviceInfoArray(&_scArray);
    success &= SCSetBtCmpDevInfoArray(&_spArray);

    if (success)
    {
        _wcb.shutdownState = WUD_STATE_SHUTDOWN_FLUSH_SETTINGS;
        SCFlushAsync(ShutFlushCallback);
    }
    else
    {
        _wcb.shutdownState = WUD_STATE_SHUTDOWN_DONE;
    }
}

static void WUDiShutdownComplete(void)
{
    OSCancelAlarm(&_wcb.alarm);
    BTA_DisableBluetooth();
}

static void ShutdownHandler(void)
{
    WUDCB* p = &_wcb;

    switch (p->shutdownState)
    {
    case WUD_STATE_SHUTDOWN_STORE_SETTINGS:
        WUDiContMapTableFlush();
        break;
    case WUD_STATE_SHUTDOWN_DONE:
        WUDiShutdownComplete();
        break;
    }
}

static void ShutdownHandler0(OSAlarm* pAlarm, OSContext* pContext)
{
    OSSwitchFiberEx((u32)pAlarm, (u32)pContext, 0, 0, ShutdownHandler, __WUDHandlerStack + sizeof(__WUDHandlerStack));
}

static void InitCore(void)
{
    WUDCB* p = &_wcb;
    int i;

    DEBUGPrint("__wudClearControlBlock\n");

    for (i = 0; i < WUD_MAX_DEV_ENTRY; i++)
    {
        WUDiSetDevAddrForHandle(i, NULL);
        WUDiSetQueueSizeForHandle(i, 0);
        WUDiSetNotAckNumForHandle(i, 0);
    }

    p->smpListTail = &p->smpList[WUD_MAX_DEV_ENTRY_FOR_SMP - 1];
    p->smpListHead = &p->smpList[0];

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++)
    {
        p->smpList[i].devInfo = &p->smpDevs[WUD_MAX_DEV_ENTRY_FOR_SMP - 1 - i];
        p->smpList[i].prev = i == 0 ? NULL : &p->smpList[i - 1];
        p->smpList[i].next = i == WUD_MAX_DEV_ENTRY_FOR_SMP - 1 ? NULL : &p->smpList[i + 1];
    }

    p->stdListTail = &p->stdList[WUD_MAX_DEV_ENTRY_FOR_STD - 1];
    p->stdListHead = &p->stdList[0];

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++)
    {
        p->stdList[i].devInfo = &p->stdDevs[i];
        p->stdList[i].prev = i == 0 ? NULL : &p->stdList[i - 1];
        p->stdList[i].next = i == WUD_MAX_DEV_ENTRY_FOR_STD - 1 ? NULL : &p->stdList[i + 1];
    }

    p->syncState = WUD_STATE_START;
    p->linkKeyState = WUD_STATE_START;
    p->deleteState = WUD_STATE_START;
    p->stackState = WUD_STATE_START;
    p->initState = WUD_STATE_START;
    p->shutdownState = WUD_STATE_START;

    p->syncSkipChecks = FALSE;
    p->syncType = WUD_SYNC_TYPE_STANDARD;
    p->syncLoopNum = 1;

    p->connectable = FALSE;
    p->discoverable = FALSE;

    p->libStatus = WUD_LIB_STATUS_0;
    p->pmID = 0;
    p->syncRssi = -65;

    memset(p->hostAddr, 0, BD_ADDR_LEN);
    memset(p->pairAddr, 0, BD_ADDR_LEN);

    p->bufferStatus0 = 0;
    p->bufferStatus1 = 10;

    WUDiClearDevice();
}

BOOL WUDInit(void)
{
    WUDCB* p = &_wcb;

    if (_initialized)
    {
        return FALSE;
    }

    DEBUGPrint("BTA_Init() is started\n");

    BTA_Init();
    bta_sys_set_trace_level(__bte_trace_level);
    L2CA_SetTraceLevel(__bte_trace_level);
    SDP_SetTraceLevel(__bte_trace_level);

    DEBUGPrint("BTA_Init() is done\n");

    p->syncStdCB = NULL;
    p->syncSmpCB = NULL;
    p->clearDevCB = NULL;

    p->hidConnCB = NULL;
    p->hidRecvCB = NULL;

    p->initState = WUD_STATE_INIT_WAIT_FOR_INITIALIZATION;

    SCInit();

    if (_linkedWBC && _initWBC)
    {
        _initWBC();
    }

    OSCreateAlarm(&p->alarm);
    OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OS_MSEC_TO_TICKS(10), InitHandler0);

    _initialized = TRUE;
    _scFlush = FALSE;
    return TRUE;
}

void WUDRegisterAllocator(WUDAllocFunc pAllocFunc, WUDFreeFunc pFreeFunc)
{
    WUDCB* p = &_wcb;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    p->allocFunc = pAllocFunc;
    p->freeFunc = pFreeFunc;

    OSRestoreInterrupts(enabled);
}

u32 WUDGetAllocatedMemSize(void)
{
    return __ntd_get_allocated_mem_size();
}

void WUDShutdown(BOOL saveSimpleDevs)
{
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;
    WUDDevInfoList* pIt;

    DEBUGPrint("WUDShutdown()\n");

    enabled = OSDisableInterrupts();

    if (WUDIsBusy())
    {
        OSCancelAlarm(&p->alarm);
    }

    memset(_scArray.devices, 0, sizeof(SCBtDeviceInfo) * WUD_MAX_DEV_ENTRY_FOR_STD);

    for (i = 0, pIt = _wcb.stdListHead; pIt != NULL; pIt = pIt->next, i++)
    {
        WUD_BDCPY(_scArray.devices[i].addr, pIt->devInfo->devAddr);
        memcpy(&_scArray.devices[i].info, &pIt->devInfo->conf, sizeof(SCDevInfo));
    }
    _scArray.numRegist = WUDiGetDevNumber();

    memset(_spArray.devices, 0, sizeof(_spArray.devices));
    if (saveSimpleDevs)
    {
        for (i = 0, pIt = _wcb.smpListHead; pIt != NULL; pIt = pIt->next, i++)
        {
            WUD_BDCPY(_spArray.devices[i].addr, pIt->devInfo->devAddr);
            memcpy(_spArray.devices[i].name, &pIt->devInfo->conf, sizeof(_spArray.devices[i].name));
            memcpy(_spArray.devices[i].linkKey, pIt->devInfo->linkKey, LINK_KEY_LEN);
        }
        _spArray.numRegist = WUDiGetDevSmpNumber();
    }
    else
    {
        _spArray.numRegist = 0;
    }

    p->shutdownState = WUD_STATE_SHUTDOWN_STORE_SETTINGS;
    OSCreateAlarm(&p->alarm);
    OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OS_MSEC_TO_TICKS(10), ShutdownHandler0);
    p->libStatus = WUD_LIB_STATUS_4;
    OSRestoreInterrupts(enabled);
}

WUDLibStatus WUDGetStatus(void)
{
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    s8 libStatus = p->libStatus;

    OSRestoreInterrupts(enabled);
    return libStatus;
}

u8 WUDGetBufferStatus(void)
{
    WUDCB* p = &_wcb;
    u8 status;
    BOOL enabled = OSDisableInterrupts();

    status = p->bufferStatus1 - p->bufferStatus0;

    OSRestoreInterrupts(enabled);
    return status;
}

void WUDSetSniffMode(BD_ADDR addr, s32 interval)
{
    WUDCB* p = &_wcb;
    tBTM_PM_PWR_MD block;

    if (interval > 0)
    {
        block.mode = BTM_PM_MD_SNIFF;
    }
    else
    {
        block.mode = BTM_PM_MD_ACTIVE;
    }

    block.max = interval;
    block.min = interval;
    block.attempt = 1;
    block.timeout = 0;

    BTM_SetPowerMode(p->pmID, addr, &block);
}

WUDSyncDeviceCallback
WUDSetSyncSimpleCallback(WUDSyncDeviceCallback pCallback)
{

    WUDCB* p = &_wcb;
    BOOL enabled;
    WUDSyncDeviceCallback pOldCallback;

    DEBUGPrint("WUDSetSyncDeviceCallback\n");

    enabled = OSDisableInterrupts();

    pOldCallback = p->syncSmpCB;
    p->syncSmpCB = pCallback;

    OSRestoreInterrupts(enabled);
    return pOldCallback;
}

WUDClearDeviceCallback
WUDSetClearDeviceCallback(WUDClearDeviceCallback pCallback)
{

    WUDCB* p = &_wcb;
    BOOL enabled;
    WUDClearDeviceCallback pOldCallback;

    DEBUGPrint("WUDSetClearDeviceCallback\n");

    enabled = OSDisableInterrupts();

    pOldCallback = p->clearDevCB;
    p->clearDevCB = pCallback;

    OSRestoreInterrupts(enabled);
    return pOldCallback;
}

static BOOL StartSyncDevice(u8 syncType, s8 syncLoopNum, u8 target, BOOL syncSkipChecks)
{
    WUDCB* p = &_wcb;
    BOOL result = FALSE;
    BOOL enabled = OSDisableInterrupts();
    u32 status = p->libStatus;
    OSRestoreInterrupts(enabled);

    if (status == WUD_LIB_STATUS_3 && !WUDIsBusy())
    {
        enabled = OSDisableInterrupts();
        _syncTarget = target;
        p->syncState = WUD_STATE_SYNC_PREPARE_SEARCH;
        p->syncLoopNum = syncLoopNum;
        p->syncType = syncType;
        p->syncSkipChecks = syncSkipChecks ? 1 : 0;
        p->syncedNum = 0;
        p->waitStartSearchFrames = 50;
        p->waitIncomingFrames = 200;
        OSCreateAlarm(&p->alarm);
        OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OS_MSEC_TO_TICKS(20), SyncHandler0);
        OSRestoreInterrupts(enabled);
        result = TRUE;
    }
    return result;
}

static BOOL StartSyncStandard(BOOL syncSkipChecks)
{
    return StartSyncDevice(WUD_SYNC_TYPE_STANDARD, 3, 0, syncSkipChecks);
}

BOOL WUDStartSyncDevice(void)
{
    return StartSyncStandard(FALSE);
}

static BOOL StartSyncSimple(BOOL syncSkipChecks)
{
    return StartSyncDevice(WUD_SYNC_TYPE_SIMPLE, -1, 0, syncSkipChecks);
}

BOOL WUDStartFastSyncSimple(void)
{
    return StartSyncSimple(TRUE);
}

BOOL WUDStartSyncSimple(void)
{
    return StartSyncSimple(FALSE);
}

BOOL WUDStartSyncSpDevice(u8 target)
{
    WUDCB* p = &_wcb;
    BOOL success;
    BOOL enabled;
    WUDSyncDeviceCallback callback;

    DEBUGPrint("WUDStartSyncSpDevice()\n");
    success = StartSyncDevice(WUD_SYNC_TYPE_SIMPLE, -1, target, FALSE);
    enabled = OSDisableInterrupts();
    callback = p->syncSmpCB;
    OSRestoreInterrupts(enabled);
    if (!success && callback != NULL)
    {
        callback(WUD_RESULT_SYNC_BUSY, 0);
    }
    return success;
}

static BOOL StopSync(void)
{
    WUDCB* p = &_wcb;
    BOOL result = FALSE;
    BOOL enabled = OSDisableInterrupts();
    if (p->libStatus == WUD_LIB_STATUS_3)
    {
        if (p->syncLoopNum != 0)
        {
            if (p->syncState == WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT)
            {
                BTA_DmSearchCancel();
            }
            p->syncLoopNum = 0;
        }
        result = TRUE;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

BOOL WUDCancelSyncDevice(void)
{
    _abortSync = TRUE;
    return StopSync();
}

BOOL WUDStopSyncSimple(void)
{
    return StopSync();
}

BOOL WUDStartClearDevice(void)
{
    WUDCB* p = &_wcb;
    BOOL success;
    BOOL enabled;
    u32 libStatus;

    success = FALSE;
    DEBUGPrint("WUDStartClearDevice()\n");

    enabled = OSDisableInterrupts();
    libStatus = p->libStatus;

    OSRestoreInterrupts(enabled);

    if (libStatus == WUD_LIB_STATUS_3 && !WUDIsBusy())
    {
        enabled = OSDisableInterrupts();

        p->deleteState = WUD_STATE_DELETE_DISALLOW_INCOMING;

        OSCreateAlarm(&p->alarm);
        OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OS_MSEC_TO_TICKS(20), DeleteAllHandler0);

        OSRestoreInterrupts(enabled);

        success = TRUE;
    }

    return success;
}

BOOL WUDSetDisableChannel(s8 afhChannel)
{
    int first = 0;
    int last = 0;
    BOOL success = FALSE;
    WUDCB* p = &_wcb;
    BOOL enabled;
    u32 libStatus;
    tBTM_STATUS status;

    DEBUGPrint("WUDSetDisableChannel()\n");

    if ((u8)afhChannel > 13)
    {
        return FALSE;
    }

    enabled = OSDisableInterrupts();
    libStatus = p->libStatus;
    OSRestoreInterrupts(enabled);

    if (libStatus == WUD_LIB_STATUS_3)
    {
        if (!afhChannel)
        {
            first = 0xFF;
            last = 0xFF;
        }
        else
        {
            first = (afhChannel + 1) * 5 - 14;
            last = (afhChannel + 1) * 5 + 14;

            if (first < 0)
            {
                first = 0;
            }

            if (last > 78)
            {
                last = 78;
            }
        }

        status = BTM_SetAfhChannels(first, last);
        DEBUGPrint("BTM_SetAfhChannels() : %d\n", status);

        success = TRUE;
    }

    return success;
}

WUDHidRecvCallback WUDSetHidRecvCallback(WUDHidRecvCallback pCallback)
{
    WUDCB* p = &_wcb;
    BOOL enabled;
    WUDHidRecvCallback pOldCallback;

    DEBUGPrint("WUDSetHidRecvCallback()\n");

    enabled = OSDisableInterrupts();

    pOldCallback = p->hidRecvCB;
    p->hidRecvCB = pCallback;

    OSRestoreInterrupts(enabled);
    return pOldCallback;
}

WUDHidConnCallback WUDSetHidConnCallback(WUDHidConnCallback pCallback)
{
    WUDCB* p = &_wcb;
    BOOL enabled;
    WUDHidConnCallback pOldCallback;

    DEBUGPrint("WUDSetHidConnCallback()\n");

    enabled = OSDisableInterrupts();

    pOldCallback = p->hidConnCB;
    p->hidConnCB = pCallback;

    OSRestoreInterrupts(enabled);
    return pOldCallback;
}

void WUDSetVisibility(u8 disc, u8 conn)
{
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    p->discoverable = disc;
    p->connectable = conn;

    OSRestoreInterrupts(enabled);
    BTA_DmSetVisibility(disc, conn);
}

u8 WUDGetConnectable(void)
{
    WUDCB* p = &_wcb;
    u8 connectable;
    BOOL enabled = OSDisableInterrupts();

    connectable = p->connectable;

    OSRestoreInterrupts(enabled);
    return connectable;
}

void reset_again_cb(void* p1)
{
#pragma unused(p1)
    DEBUGPrint("Initialize the BCM2045 firmware complete.\n");
    WUDiInitSub();
}

void reset_again(void)
{
    DEBUGPrint("Reset again!\n");
    BTM_DeviceReset(reset_again_cb);
}

static void install_patch_cb(tBTM_VSC_CMPL* p1)
{
    if ((patch_num == install_num && p1) || !p1)
    {
        reset_again();
    }
    else
    {
        WUDiInstallPatch();
    }
}

static void WUDiInstallPatch(void)
{
    u8 buf[WUD_PATCH_BUFFER_SIZE];
    u8 num;

    num = MIN(patch_num - install_num, WUD_MAX_PATCHES);
    buf[offsetof(WUDPatchList, num)] = num;

    memcpy(&buf[offsetof(WUDPatchList, cmds)],
        patch_install.cmds[install_num].data,
        num * sizeof(WUDPatchCmd));

    install_num += num;
    DEBUGPrint("Install the patches\n");
    BTM_VendorSpecificCommand(BT_VSC_NINTENDO_INSTALL_PATCH,
        sizeof(num) + num * sizeof(WUDPatchCmd),
        buf,
        install_patch_cb);
}

static void write_patch_cb(tBTM_VSC_CMPL* p1)
{
    if (p1 != NULL)
    {
        if (size == (u32)offset)
        {
            install_num = 0;
            patch_num = patch_install.num;
            WUDiInstallPatch();
        }
        else
        {
            WUDiWritePatch();
        }
    }
    else
    {
        reset_again();
    }
}

static void WUDiWritePatch(void)
{
    u8 buf[WUD_PATCH_BUFFER_SIZE];
    u8 length;
    int i;

    length = MIN(size - offset, WUD_PATCH_BUFFER_SIZE - sizeof(u32));

    for (i = 0; i < (s32)sizeof(u32); i++)
    {
        buf[i] = addr + offset >> i * 8;
    }

    for (i = 0; i < (s32)length; i++)
    {
        buf[(s32)sizeof(u32) + i] = patch_binary[sizeof(size) + sizeof(offset) + offset + i];
    }

    offset += length;
    DEBUGPrint("Download the patch codes [%d]\n", offset);
    BTM_VendorSpecificCommand(BT_VSC_NINTENDO_WRITE_PATCH,
        (s32)sizeof(u32) + length,
        buf,
        write_patch_cb);
}

static void remove_patch_cb(tBTM_VSC_CMPL* p1)
{
    DEBUGPrint("RemovePatch Callback\n");
    if (p1 != NULL)
    {
        offset = 0;
        WUDiWritePatch();
    }
    else
    {
        reset_again();
    }
}

static void WUDiRemovePatch(void)
{
    DEBUGPrint("RemovePatch\n");
    BTM_VendorSpecificCommand(BT_VSC_NINTENDO_INSTALL_PATCH,
        sizeof(remove_patch),
        remove_patch,
        remove_patch_cb);
}

static void SuperPeekPokeCallback(tBTM_VSC_CMPL* p1)
{
#pragma unused(p1)
    DEBUGPrint("SuperPeekPoke Callback\n");
    WUDiRemovePatch();
}

static void SuperPeekPoke(void)
{
    DEBUGPrint("SuperPeekPoke\n");
    BTM_VendorSpecificCommand(BT_VSC_NINTENDO_SUPER_PEEK_POKE,
        sizeof(super_peek_poke),
        super_peek_poke,
        SuperPeekPokeCallback);
}

static void WUDiAppendRuntimePatch(void)
{
    int i;

    DEBUGPrint("__wudAppendRuntimePatch()\n");

    size = 0;
    addr = 0;

    for (i = 0; i < (int)sizeof(u32); i++)
    {
        addr <<= 8;
        addr += patch_binary[3 - i];

        size <<= 8;
        size += patch_binary[7 - i];
    }

    if (__OSInIPL)
    {
        SuperPeekPoke();
    }
    else
    {
        WUDiRemovePatch();
    }
}

void WUDiGetFirmwareVersion(void)
{
    tBTM_VERSION_INFO version;
    BTM_ReadLocalVersion(&version);

    DEBUGPrint("hci_version   : %02x\n", version.hci_version);
    DEBUGPrint("hci_revision  : %04x\n", version.hci_revision);

    DEBUGPrint("lmp_version   : %02x\n", version.lmp_version);
    DEBUGPrint("lmp_subversion: %04x\n", version.lmp_subversion);

    DEBUGPrint("manufacturer  : %04x\n", version.manufacturer);

    DEBUGPrint(" ==> 2045 firmware ver.002.003.014.%d\n",
        version.hci_revision & 0xFFF);

    if ((version.hci_revision & 0xFFF) == 167)
    {
        WUDiAppendRuntimePatch();
    }
    else
    {
        WUDiInitSub();
    }
}

void WUDiInitSub(void)
{
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    char devName[] = "Wii";
    DEV_CLASS devClass = {
        0x00, // No designated Major Service Classes
        0x04, // Major Device Class 4 (Audio/Video)
        0x48  // Minor Device Class 18 (Audio/Video -> Gaming/Toy)
    };

    DEBUGPrint("start __wudInitSub()\n");

    BTA_DmSetDeviceName(devName);
    BTM_SetDeviceClass(devClass);

    BTM_RegisterForVSEvents(&WUDVendorSpecificCallback);
    BTM_RegisterForDeviceStatusNotif(&WUDDeviceStatusCallback);
    BTM_PmRegister(BTM_PM_REG_SET | BTM_PM_REG_NOTIF, &p->pmID, &WUDPowerManagerCallback);

    BTM_WritePageTimeout(32768);
    BTM_SetDefaultLinkPolicy(HCI_ENABLE_MASTER_SLAVE_SWITCH | HCI_ENABLE_SNIFF_MODE);
    BTM_SetDefaultLinkSuperTout(3200);

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++)
    {
        if (p->stdDevs[i].status == 1)
        {
            WUDiRegisterDevice(p->stdDevs[i].devAddr);
        }
    }

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++)
    {
        if (p->smpDevs[i].status == 1)
        {
            WUDiRegisterDevice(p->smpDevs[i].devAddr);
        }
    }

    enabled = OSDisableInterrupts();
    p->libStatus = WUD_LIB_STATUS_3;
    _readNand = TRUE;
    OSRestoreInterrupts(enabled);

    WUDSetVisibility(FALSE, TRUE);
}

void WUDiEnableStack(void)
{
    WUDCB* p = &_wcb;

    BTA_HhEnable(BTA_SEC_AUTHENTICATE, WUDHidHostCallback);

    p->stackState = WUD_STATE_STACK_GET_STORED_LINK_KEY;

    OSCreateAlarm(&p->alarm);
    OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OS_MSEC_TO_TICKS(10), EnableStackHandler0);
}

void WUDiRegisterDevice(BD_ADDR addr)
{
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    tBTA_STATUS status;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    pInfo = WUDiGetDevInfo(addr);

    status = BTA_DmAddDevice(pInfo->devAddr, pInfo->linkKey, 0, FALSE);
    DEBUGPrint("BTA_DmAddDevice(): %d\n", status);

    if (WUD_DEV_NAME_IS_CNT(pInfo->conf.devName) || (WUD_DEV_NAME_IS_WBC(pInfo->conf.devName) && WUDIsLinkedWBC()))
    {
        tBTA_HH_DEV_DESCR desc;
        desc.dl_len = sizeof(descriptor);
        desc.dsc_list = descriptor;

        DEBUGPrint("BTA_HhAddDev()\n");
        BTA_HhAddDev(pInfo->devAddr, pInfo->hhAttrMask, pInfo->subclass, pInfo->appID, desc);
    }

    if (pInfo->UNK_0x5B == 0 || pInfo->UNK_0x5B == 4 || pInfo->UNK_0x5B == 2 || pInfo->UNK_0x5B == 5)
    {

        p->devNums++;
    }
    else
    {
        p->devSmpNums++;
    }

    OSRestoreInterrupts(enabled);
}

void WUDiRemoveDevice(BD_ADDR addr)
{
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    tBTM_STATUS status;
    BOOL enabled = OSDisableInterrupts();

    DEBUGPrint("WUDiRemoveDevice : \n");

    pInfo = WUDiGetDevInfo(addr);

    if (pInfo != NULL)
    {
        DEBUGPrint(" handle : %d,  addr : %02x:%02x:%02x:%02x:%02x:%02x\n",
            pInfo->devHandle,
            pInfo->devAddr[0],
            pInfo->devAddr[1],
            pInfo->devAddr[2],
            pInfo->devAddr[3],
            pInfo->devAddr[4],
            pInfo->devAddr[5]);

        DEBUGPrint("remove device info from database.\n");

        if (WUD_DEV_NAME_IS_CNT(pInfo->conf.devName) || (WUD_DEV_NAME_IS_WBC(pInfo->conf.devName) && WUDIsLinkedWBC()))
        {
            DEBUGPrint("BTA_HhRemoveDev()\n");
            DEBUGPrint(" handle : %d\n", pInfo->devHandle);
            BTA_HhRemoveDev(pInfo->devHandle);
        }

        status = BTA_DmRemoveDevice(pInfo->devAddr);
        DEBUGPrint("BTA_DmRemoveDevice(): %d\n", status);

        if (pInfo->UNK_0x5B == 0 || pInfo->UNK_0x5B == 2 || pInfo->UNK_0x5B == 4 || pInfo->UNK_0x5B == 5)
        {
            p->devNums--;
        }
        else
        {
            p->devSmpNums--;
        }

        memset(pInfo, 0, sizeof *pInfo);
    }

    OSRestoreInterrupts(enabled);
}

WUDDevInfo* WUDiGetDevInfo(BD_ADDR addr)
{
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    BOOL enabled;
    int i;

    pInfo = NULL;
    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++)
    {
        if (WUD_BDCMP(p->stdDevs[i].devAddr, addr) == 0)
        {
            pInfo = &p->stdDevs[i];
            break;
        }
    }

    if (pInfo == NULL)
    {
        for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++)
        {
            if (WUD_BDCMP(p->smpDevs[i].devAddr, addr) == 0)
            {
                pInfo = &p->smpDevs[i];
                break;
            }
        }
    }

    OSRestoreInterrupts(enabled);
    return pInfo;
}

WUDDevInfo* WUDiGetDevInfoIndex(int idx)
{
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if ((u32)idx <= WUD_MAX_DEV_ENTRY_FOR_STD - 1)
    {
        pInfo = &p->stdDevs[idx];
    }
    else
    {
        pInfo = &p->smpDevs[idx - WUD_MAX_DEV_ENTRY_FOR_STD];
    }

    OSRestoreInterrupts(enabled);
    return pInfo;
}

static WUDDevInfo* WUDiGetNewStdDevInfo(void)
{
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo = NULL;
    int i;

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++)
    {
        if (p->stdDevs[i].status == 0)
        {
            DEBUGPrint("new entry index is %d\n", i);
            pInfo = &p->stdDevs[i];
            break;
        }
    }
    return pInfo;
}

static WUDDevInfo* WUDiGetNewSmpDevInfo(void)
{
    return _wcb.smpListTail->devInfo;
}

WUDDevInfo* WUDiGetNewDevInfo(void)
{
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo = NULL;
    BOOL enabled = OSDisableInterrupts();

    if (p->syncType == WUD_SYNC_TYPE_STANDARD)
    {
        pInfo = WUDiGetNewStdDevInfo();
    }
    else
    {
        pInfo = WUDiGetNewSmpDevInfo();
    }
    OSRestoreInterrupts(enabled);
    return pInfo;
}

void WUDiRemoveDevInfo(BD_ADDR addr)
{
    WUDDevInfo* pInfo;
    BOOL enabled;

    pInfo = WUDiGetDevInfo(addr);
    if (pInfo == NULL)
    {
        return;
    }

    enabled = OSDisableInterrupts();
    memset(pInfo, 0, sizeof(WUDDevInfo));
    OSRestoreInterrupts(enabled);
}

void WUDiClearDevice(void)
{
    WUDCB* p = &_wcb;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    memset(p->stdDevs, 0, sizeof(WUDDevInfo) * WUD_MAX_DEV_ENTRY_FOR_STD);
    memset(p->smpDevs, 0, sizeof(WUDDevInfo) * WUD_MAX_DEV_ENTRY_FOR_SMP);

    p->devNums = 0;
    p->devSmpNums = 0;

    p->connectedNum = 0;
    p->linkedNum = 0;
    p->syncedNum = 0;

    OSRestoreInterrupts(enabled);
}

u8 WUDiGetDevNumber(void)
{
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    u8 num = p->devNums;

    OSRestoreInterrupts(enabled);
    return num;
}

u8 WUDiGetDevSmpNumber(void)
{
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    u8 num = p->devSmpNums;

    OSRestoreInterrupts(enabled);
    return num;
}

u8 WUDiGetConnNumber(void)
{
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    u8 num = p->connectedNum;

    OSRestoreInterrupts(enabled);
    return num;
}

u8 WUDiGetLinkNum(void)
{
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    u8 num = p->linkedNum;

    OSRestoreInterrupts(enabled);
    return num;
}

void WUDiMoveTopSmpDevInfoPtr(WUDDevInfo* pInfo)
{
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++)
    {
        if (WUD_BDCMP(p->smpList[i].devInfo->devAddr, pInfo->devAddr) != 0)
        {
            continue;
        }

        if (WUD_BDCMP(p->smpListHead->devInfo->devAddr,
                p->smpList[i].devInfo->devAddr)
            == 0)
        {
            break;
        }

        p->smpList[i].prev->next = p->smpList[i].next;

        if (WUD_BDCMP(p->smpListTail->devInfo->devAddr,
                p->smpList[i].devInfo->devAddr)
            == 0)
        {
            p->smpListTail = p->smpList[i].prev;
        }
        else
        {
            p->smpList[i].next->prev = p->smpList[i].prev;
        }

        p->smpList[i].next = p->smpListHead;
        p->smpListHead->prev = &p->smpList[i];
        p->smpListHead = &p->smpList[i];

        p->smpList[i].prev = NULL;

        break;
    }

    OSRestoreInterrupts(enabled);
}

void WUDiMoveBottomSmpDevInfoPtr(WUDDevInfo* pInfo)
{
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++)
    {
        if (WUD_BDCMP(p->smpList[i].devInfo->devAddr, pInfo->devAddr) != 0)
        {
            continue;
        }

        if (WUD_BDCMP(p->smpListTail->devInfo->devAddr,
                p->smpList[i].devInfo->devAddr)
            == 0)
        {
            break;
        }

        p->smpList[i].next->prev = p->smpList[i].prev;

        if (WUD_BDCMP(p->smpListHead->devInfo->devAddr,
                p->smpList[i].devInfo->devAddr)
            == 0)
        {
            p->smpListHead = p->smpList[i].next;
        }
        else
        {
            p->smpList[i].prev->next = p->smpList[i].next;
        }

        p->smpList[i].prev = p->smpListTail;
        p->smpListTail->next = &p->smpList[i];
        p->smpListTail = &p->smpList[i];

        p->smpList[i].next = NULL;

        break;
    }

    OSRestoreInterrupts(enabled);
}

WUDDevInfo* WUDiGetRemoveSmpDevice(void)
{
    return _wcb.smpListTail->devInfo;
}

void WUDiMoveTopOfDisconnectedSmpDevice(WUDDevInfo* pInfo)
{
    WUDCB* p = &_wcb;
    int i;
    BOOL enabled;
    WUDDevInfoList* pIt;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++)
    {
        if (WUD_BDCMP(p->smpList[i].devInfo->devAddr, pInfo->devAddr) != 0)
        {
            continue;
        }

        for (pIt = p->smpListHead; pIt; pIt = pIt->next)
        {
            if (WUD_BDCMP(pIt->devInfo->devAddr, pInfo->devAddr) == 0)
            {
                continue;
            }

            if (pIt->devInfo->status != 1 && pIt->devInfo->status != 0)
            {
                continue;
            }

            if (WUD_BDCMP(p->smpListHead->devInfo->devAddr,
                    p->smpList[i].devInfo->devAddr)
                == 0)
            {
                if (pIt == p->smpListHead->next)
                {
                    break;
                }

                p->smpListHead = p->smpList[i].next;
            }
            else
            {
                p->smpList[i].prev->next = p->smpList[i].next;
            }

            p->smpList[i].next->prev = p->smpList[i].prev;

            if (pIt != p->smpListHead)
            {
                p->smpList[i].prev = pIt->prev;
                p->smpList[i].next = pIt;

                pIt->prev->next = &p->smpList[i];
                pIt->prev = &p->smpList[i];
            }
            else
            {
                p->smpList[i].prev = pIt;
                p->smpList[i].next = pIt->next;

                pIt->next->prev = &p->smpList[i];
                pIt->next = &p->smpList[i];
            }

            break;
        }
    }

    OSRestoreInterrupts(enabled);
}

void WUDiMoveTopStdDevInfoPtr(WUDDevInfo* pInfo)
{
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++)
    {
        if (WUD_BDCMP(p->stdList[i].devInfo->devAddr, pInfo->devAddr) != 0)
        {
            continue;
        }

        if (WUD_BDCMP(p->stdListHead->devInfo->devAddr,
                p->stdList[i].devInfo->devAddr)
            == 0)
        {
            break;
        }

        p->stdList[i].prev->next = p->stdList[i].next;

        if (WUD_BDCMP(p->stdListTail->devInfo->devAddr,
                p->stdList[i].devInfo->devAddr)
            == 0)
        {
            p->stdListTail = p->stdList[i].prev;
        }
        else
        {
            p->stdList[i].next->prev = p->stdList[i].prev;
        }

        p->stdList[i].next = p->stdListHead;
        p->stdListHead->prev = &p->stdList[i];
        p->stdListHead = &p->stdList[i];

        p->stdList[i].prev = NULL;

        break;
    }

    OSRestoreInterrupts(enabled);
}

void WUDiMoveBottomStdDevInfoPtr(WUDDevInfo* pInfo)
{
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++)
    {
        if (WUD_BDCMP(p->stdList[i].devInfo->devAddr, pInfo->devAddr) != 0)
        {
            continue;
        }

        if (WUD_BDCMP(p->stdListTail->devInfo->devAddr,
                p->stdList[i].devInfo->devAddr)
            == 0)
        {
            break;
        }

        p->stdList[i].next->prev = p->stdList[i].prev;

        if (WUD_BDCMP(p->stdListHead->devInfo->devAddr,
                p->stdList[i].devInfo->devAddr)
            == 0)
        {
            p->stdListHead = p->stdList[i].next;
        }
        else
        {
            p->stdList[i].prev->next = p->stdList[i].next;
        }

        p->stdList[i].prev = p->stdListTail;
        p->stdListTail->next = &p->stdList[i];
        p->stdListTail = &p->stdList[i];

        p->stdList[i].next = NULL;

        break;
    }

    OSRestoreInterrupts(enabled);
}

WUDDevInfo* WUDiGetRemoveStdDevice(void)
{
    return _wcb.stdListTail->devInfo;
}

void WUDiMoveTopOfDisconnectedStdDevice(WUDDevInfo* pInfo)
{
    WUDCB* p = &_wcb;
    int i;
    BOOL enabled;
    WUDDevInfoList* pIt;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++)
    {
        if (WUD_BDCMP(p->stdList[i].devInfo->devAddr, pInfo->devAddr) != 0)
        {
            continue;
        }

        for (pIt = p->stdListHead; pIt; pIt = pIt->next)
        {
            if (WUD_BDCMP(pIt->devInfo->devAddr, pInfo->devAddr) == 0)
            {
                continue;
            }

            if (pIt->devInfo->status != 1 && pIt->devInfo->status != 0 && (WUD_DEV_NAME_IS_CNT(pIt->devInfo->conf.devName) || !WUDIsLinkedWBC()))
            {
                continue;
            }

            if (WUD_BDCMP(p->stdListHead->devInfo->devAddr,
                    p->stdList[i].devInfo->devAddr)
                == 0)
            {
                if (pIt == p->stdListHead->next)
                    break;

                p->stdListHead = p->stdList[i].next;
            }
            else
            {
                p->stdList[i].prev->next = p->stdList[i].next;
            }

            p->stdList[i].next->prev = p->stdList[i].prev;

            if (pIt != p->stdListHead)
            {
                p->stdList[i].prev = pIt->prev;
                p->stdList[i].next = pIt;

                pIt->prev->next = &p->stdList[i];
                pIt->prev = &p->stdList[i];
            }
            else
            {
                p->stdList[i].prev = pIt;
                p->stdList[i].next = pIt->next;

                pIt->next->prev = &p->stdList[i];
                pIt->next = &p->stdList[i];
            }

            break;
        }
    }

    OSRestoreInterrupts(enabled);
}

void WUDiMoveTopOfUnusedStdDevice(WUDDevInfo* pInfo)
{
    WUDCB* p = &_wcb;
    int i;
    BOOL intrStatus = OSDisableInterrupts();
    WUDDevInfoList* pIt;
    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++)
    {
        if (WUD_BDCMP(p->stdList[i].devInfo->devAddr, pInfo->devAddr) != 0)
        {
            continue;
        }

        for (pIt = p->stdListHead; pIt; pIt = pIt->next)
        {
            if (WUD_BDCMP(pIt->devInfo->devAddr, pInfo->devAddr) == 0)
                continue;

            if (pIt->devInfo->status != 0 && pIt != p->stdListTail)
                continue;

            if (WUD_BDCMP(
                    p->stdListHead->devInfo->devAddr,
                    p->stdList[i].devInfo->devAddr)
                == 0)
            {
                if (pIt == p->stdListHead->next)
                    break;

                p->stdListHead = p->stdList[i].next;
            }
            else
            {
                p->stdList[i].prev->next = p->stdList[i].next;
            }

            p->stdList[i].next->prev = p->stdList[i].prev;

            if (pIt == p->stdListTail && pIt->devInfo->status != 0)
            {
                p->stdList[i].prev = pIt;
                p->stdList[i].next = NULL;

                pIt->next = &p->stdList[i];

                p->stdListTail = &p->stdList[i];
            }
            else if (pIt != p->stdListHead)
            {
                p->stdList[i].prev = pIt->prev;
                p->stdList[i].next = pIt;

                pIt->prev->next = &p->stdList[i];
                pIt->prev = &p->stdList[i];
            }
            else
            {
                p->stdList[i].prev = pIt;
                p->stdList[i].next = pIt->next;

                pIt->next->prev = &p->stdList[i];
                pIt->next = &p->stdList[i];
            }

            break;
        }
    }

    OSRestoreInterrupts(intrStatus);
}

BOOL WUDIsBusy(void)
{
    WUDCB* p = &_wcb;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if (p->syncState == WUD_STATE_START && p->deleteState == WUD_STATE_START && p->stackState == WUD_STATE_STACK_INITIALIZED && p->initState == WUD_STATE_INIT_INITIALIZED)
    {

        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

static void CleanupCallback(tBTA_STATUS status)
{
    WUDCB* p = &_wcb;

    if (status == BTA_SUCCESS)
    {
        _initialized = FALSE;
        p->libStatus = WUD_LIB_STATUS_0;
        return;
    }

    DEBUGPrint("WARNING: USB_CLOSE_FAILURE!\n");
}

void WUDSecurityCallback(tBTA_DM_SEC_EVT event, tBTA_DM_SEC* pData)
{
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    u8 i;

    tBTA_DM_ENABLE* pEnable;
    tBTA_DM_LINK_UP* pLinkUp;
    tBTA_DM_LINK_DOWN* pLinkDown;
    tBTA_DM_PIN_REQ* pPinReq;
    tBTA_DM_AUTH_CMPL* pAuthCmpl;

    DEBUGPrint("__wudCleanupStackCallback: ");

    switch (event)
    {
    case BTA_DM_ENABLE_EVT:
    {
        pEnable = &pData->enable;
        WUD_BDCPY(p->hostAddr, pEnable->bd_addr);

        DEBUGPrint("BTA_ENABLE_EVT\n");
        DEBUGPrint("host : %02x:%02x:%02x:%02x:%02x:%02x\n", p->hostAddr[0], p->hostAddr[1], p->hostAddr[2], p->hostAddr[3], p->hostAddr[4], p->hostAddr[5]);

        WUDiEnableStack();
        p->libStatus = WUD_LIB_STATUS_2;
        break;
    }

    case BTA_DM_DISABLE_EVT:
    {
        DEBUGPrint("BTA_DISABLE_EVT\n");

        BTA_CleanUp(CleanupCallback);
        break;
    }

    case BTA_DM_PIN_REQ_EVT:
    {
        pPinReq = &pData->pin_req;

        DEBUGPrint("BTA_DM_PIN_REQ_EVT\n");

        WUDiSetPinCode(pPinReq->bd_addr);
        break;
    }

    case BTA_DM_AUTH_CMPL_EVT:
    {
        pAuthCmpl = &pData->auth_cmpl;

        DEBUGPrint("BTA_DM_AUTH_CMPL_EVT\n");

        DEBUGPrint("  addr : %02x:%02x:%02x:%02x:%02x:%02x\n",
            pAuthCmpl->bd_addr[0],
            pAuthCmpl->bd_addr[1],
            pAuthCmpl->bd_addr[2],
            pAuthCmpl->bd_addr[3],
            pAuthCmpl->bd_addr[4],
            pAuthCmpl->bd_addr[5]);

        DEBUGPrint("  key  : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
            pAuthCmpl->key[0],
            pAuthCmpl->key[1],
            pAuthCmpl->key[2],
            pAuthCmpl->key[3],
            pAuthCmpl->key[4],
            pAuthCmpl->key[5],
            pAuthCmpl->key[6],
            pAuthCmpl->key[7],
            pAuthCmpl->key[8],
            pAuthCmpl->key[9],
            pAuthCmpl->key[10],
            pAuthCmpl->key[11],
            pAuthCmpl->key[12],
            pAuthCmpl->key[13],
            pAuthCmpl->key[14],
            pAuthCmpl->key[15]);

        DEBUGPrint("  result = %d\n", pAuthCmpl->success);

        if (!pAuthCmpl->success)
        {
            break;
        }

        pInfo = WUDiGetDevInfo(pAuthCmpl->bd_addr);

        if (WUD_BDCMP(_work.devAddr, pAuthCmpl->bd_addr) == 0)
        {
            _work.status = 12;

            if (pInfo == NULL)
            {
                pInfo = &_work;
            }
        }

        memcpy(pInfo->linkKey, pAuthCmpl->key, LINK_KEY_LEN);
        break;
    }

    case BTA_DM_AUTHORIZE_EVT:
    {
        DEBUGPrint("BTA_DM_AUTHORIZE_EVT\n");
        break;
    }

    case BTA_DM_LINK_UP_EVT:
    {
        pLinkUp = &pData->link_up;

        DEBUGPrint("BTA_DM_LINK_UP_EVT\n");

        DEBUGPrint("   addr : %02x:%02x:%02x:%02x:%02x:%02x\n",
            pLinkUp->bd_addr[0],
            pLinkUp->bd_addr[1],
            pLinkUp->bd_addr[2],
            pLinkUp->bd_addr[3],
            pLinkUp->bd_addr[4],
            pLinkUp->bd_addr[5]);

        pInfo = WUDiGetDevInfo(pLinkUp->bd_addr);
        if (pInfo == NULL && WUD_BDCMP(pLinkUp->bd_addr, _work.devAddr) == 0)
        {
            pInfo = &_work;
        }

        if (pInfo == NULL || WUDiGetLinkNum() == WUD_MAX_CHANNELS)
        {
            btm_remove_acl(pLinkUp->bd_addr);

            DEBUGPrint("%s --> %02x:%02x:%02x:%02x:%02x:%02x\n",
                pInfo == NULL ? "not paired" : "4 links exist",
                pLinkUp->bd_addr[0],
                pLinkUp->bd_addr[1],
                pLinkUp->bd_addr[2],
                pLinkUp->bd_addr[3],
                pLinkUp->bd_addr[4],
                pLinkUp->bd_addr[5]);
            break;
        }

        pInfo->status = pInfo->status == 2 ? 12 : 3;
        p->linkedNum++;
        break;
    }

    case BTA_DM_LINK_DOWN_EVT:
    {
        pLinkDown = &pData->link_down;

        DEBUGPrint("BTA_DM_LINK_DOWN_EVT\n");

        DEBUGPrint("   addr : %02x:%02x:%02x:%02x:%02x:%02x\n",
            pLinkDown->bd_addr[0],
            pLinkDown->bd_addr[1],
            pLinkDown->bd_addr[2],
            pLinkDown->bd_addr[3],
            pLinkDown->bd_addr[4],
            pLinkDown->bd_addr[5]);

        DEBUGPrint("result: %d\n", pLinkDown->status);

        pInfo = WUDiGetDevInfo(pLinkDown->bd_addr);
        if (pInfo != NULL)
        {
            pInfo->status = 1;
            p->linkedNum--;

            if (WUD_BDCMP(_work.devAddr, pLinkDown->bd_addr) == 0)
            {
                p->syncState = WUD_STATE_ERROR;
            }

            if (pLinkDown->status == HCI_ERR_PEER_POWER_OFF)
            {
                for (i = 0; i < WUD_MAX_CHANNELS; i++)
                {
                    if (WUD_BDCMP(
                            _scArray.devices[WUD_MAX_DEV_ENTRY_FOR_STD + i]
                                .addr,
                            pLinkDown->bd_addr)
                        == 0)
                    {

                        memset(&_scArray.devices[WUD_MAX_DEV_ENTRY_FOR_STD + i],
                            0,
                            sizeof(SCBtDeviceInfo));

                        _scFlush = TRUE;
                    }
                }
            }
        }
        else if (WUD_BDCMP(_work.devAddr, pLinkDown->bd_addr) == 0)
        {
            p->syncState = WUD_STATE_ERROR;
            p->linkedNum--;
        }
        else
        {
            DEBUGPrint("this device in not paired\n");
        }

        if (p->linkedNum <= 255 && p->linkedNum >= 250)
        {
            OSReport("WARNING: link num count is reset.\n");
            p->linkedNum = 0;
        }
        break;
    }

    case BTA_DM_SIG_STRENGTH_EVT:
    {
        DEBUGPrint("BTA_DM_SIG_STRENGTH_EVT\n");
        break;
    }

    case BTA_DM_BUSY_LEVEL_EVT:
    {
        DEBUGPrint("BTA_DM_BUSY_LEVEL_EVT\n");
        break;
    }
    }
}

static void WUDiResetAuthFailCount(void)
{
    BTM_VendorSpecificCommand(BT_VSC_NINTENDO_WRITE_PATCH,
        sizeof(reset_auth_count),
        reset_auth_count,
        NULL);
}

void WUDSearchCallback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH* pData)
{
    s32 timeout;

    switch (event)
    {
    case BTA_DM_INQ_RES_EVT:
    {
        tBTA_DM_INQ_RES* pResp = &pData->inq_res;

        DEBUGPrint("INQUIRY RESULT: %02x:%02x:%02x:%02x:%02x:%02x   %02x%02x%02x   %d\n",
            pResp->bd_addr[0],
            pResp->bd_addr[1],
            pResp->bd_addr[2],
            pResp->bd_addr[3],
            pResp->bd_addr[4],
            pResp->bd_addr[5],
            pResp->dev_class[0],
            pResp->dev_class[1],
            pResp->dev_class[2],
            pResp->rssi);

        _discRssi = pResp->rssi;

        if (_wcb.syncSkipChecks == TRUE || (_wcb.syncSkipChecks == FALSE && WUDiGetLinkNum() < WUD_MAX_CHANNELS - 1))
        {

            timeout = 6400;
        }
        else
        {
            timeout = 32768;
        }

        BTM_WritePageTimeout(timeout);
        break;
    }

    case BTA_DM_INQ_CMPL_EVT:
    {
        DEBUGPrint("INQUIRY_COMPLETED\n");
        break;
    }

    case BTA_DM_DISC_RES_EVT:
    {
        tBTA_DM_DISC_RES* dmDiscRes = &pData->disc_res;

        WUD_BDCPY(_discResp.devAddr, dmDiscRes->bd_addr);
        memcpy(_discResp.devName, dmDiscRes->bd_name, sizeof(_discResp.devName));

        _discResp.services = dmDiscRes->services;
        _discNumResps++;

        DEBUGPrint("DISCOVER RESULT:  %02x:%02x:%02x:%02x:%02x:%02x   %s (%04x)\n",
            _discResp.devAddr[0],
            _discResp.devAddr[1],
            _discResp.devAddr[2],
            _discResp.devAddr[3],
            _discResp.devAddr[4],
            _discResp.devAddr[5],
            _discResp.devName,
            _discResp.services);

        break;
    }

    case BTA_DM_DISC_CMPL_EVT:
    {
        DEBUGPrint("DISCOVER COMPLETED\n");

        _wcb.syncState = WUD_STATE_SYNC_CHECK_SEARCH_RESULT;
        break;
    }

    case BTA_DM_SEARCH_CANCEL_CMPL_EVT:
    {
        DEBUGPrint("SEARCH CANCEL\n\n");

        WUDiResetAuthFailCount();
        _discNumResps = 0;
        memset(&_discResp, 0, sizeof(WUDDiscResp));

        _wcb.syncState = WUD_STATE_SYNC_CHECK_SEARCH_RESULT;
        break;
    }

    default:
    {
        DEBUGPrint("Warning: Search Callback returns invalid event\n");
        break;
    }
    }
}

void WUDiAutoSync(void)
{
    WUDCB* p = &_wcb;
    WUDSyncDeviceCallback pSyncCallback;
    BOOL enabled;

    DEBUGPrint("__wudProcSyncEvent()\n");
    if (WUDIsBusy())
    {
        return;
    }

    enabled = OSDisableInterrupts();
    pSyncCallback = p->syncStdCB;
    OSRestoreInterrupts(enabled);

    if (pSyncCallback != NULL)
    {
        pSyncCallback(0, 0);
    }
    else
    {
        WUDStartSyncDevice();
    }
}

void WUDiDeleteAllLinkKeys(void)
{
    WUDCB* p = &_wcb;
    WUDClearDeviceCallback pClearCallback;
    s32 result;
    BOOL enabled;

    DEBUGPrint("__wudProcDeleteEvent()\n");
    enabled = OSDisableInterrupts();

    pClearCallback = p->clearDevCB;
    result = WUDIsBusy() ? -1 /* WUD_RESULT_DELETE_BUSY */
                         : WUD_RESULT_DELETE_WAITING;

    OSRestoreInterrupts(enabled);

    if (pClearCallback != NULL)
    {
        pClearCallback(result);
    }
    else
    {
        WUDStartClearDevice();
    }
}

void WUDVendorSpecificCallback(UINT8 len, UINT8* pData)
{
#pragma unused(len)
    u8 event = pData[0];
    WUDCB* p = &_wcb;

    switch (event)
    {
    case WUD_VSE_INITIATE_PAIRING:
        WUDiAutoSync();
        break;
    case WUD_VSE_DELETE_ALL_KEYS:
        WUDiDeleteAllLinkKeys();
        break;
    case WUD_VSE_SI_PORT_STATUS:
        p->serialPortStatus = pData[1];
        break;
    case WUD_VSE_WATCH_DOG_RESET_HW:
        OSPanic(__FILE__, 5342, "MODULE FATAL ERROR\n");
        break;
    }
}

void WUDDeviceStatusCallback(tBTM_DEV_STATUS status)
{
    DEBUGPrint("__wudDeviceStatusEventStackCallback\n");

    if (status == BTM_DEV_STATUS_CMD_TOUT)
    {
        OSReport("---- WARNING: USB FATAL ERROR! ----\n");
    }
}

void WUDStoredLinkKeyCallback(void* p1)
{
    tBTM_BD_ADDR_LINK_KEY_PAIR* pPair;
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    int i;

    tBTM_STORED_LINK_KEYS_EVT* pData = p1;
    tBTM_READ_STORED_LINK_KEY_COMPLETE* pRead;
    tBTM_WRITE_STORED_LINK_KEY_COMPLETE* pWrite;
    tBTM_DELETE_STORED_LINK_KEY_COMPLETE* pDelete;
    tBTM_RETURN_LINK_KEYS_EVT* pReturn;

    switch (pData->event)
    {
    case BTM_CB_EVT_RETURN_LINK_KEYS:
    {
        pReturn = p1;

        DEBUGPrint("BTM_CB_EVT_RETURN_LINK_KEYS\n");

        pPair = (tBTM_BD_ADDR_LINK_KEY_PAIR*)(pReturn + 1);
        i = 0;

        while (i < pReturn->num_keys)
        {
            pInfo = WUDiGetDevInfo(pPair->bd_addr);

            DEBUGPrint("BD_ADDR:  %02x:%02x:%02x:%02x:%02x:%02x\n",
                pPair->bd_addr[0],
                pPair->bd_addr[1],
                pPair->bd_addr[2],
                pPair->bd_addr[3],
                pPair->bd_addr[4],
                pPair->bd_addr[5]);

            DEBUGPrint("LINKKEY: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
                pPair->link_key[0],
                pPair->link_key[1],
                pPair->link_key[2],
                pPair->link_key[3],
                pPair->link_key[4],
                pPair->link_key[5],
                pPair->link_key[6],
                pPair->link_key[7],
                pPair->link_key[8],
                pPair->link_key[9],
                pPair->link_key[10],
                pPair->link_key[11],
                pPair->link_key[12],
                pPair->link_key[13],
                pPair->link_key[14],
                pPair->link_key[15]);

            if (pInfo == NULL)
            {
                DEBUGPrint("this device is not stored in NAND\n");

                pInfo = WUDiGetNewDevInfo();

                if (pInfo == NULL)
                {
                    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++)
                    {
                        if (p->smpDevs[i].status == 0)
                        {
                            pInfo = &p->smpDevs[i];
                            break;
                        }
                    }
                }

                if (pInfo)
                {
                    pInfo->UNK_0x5C = 1;
                    pInfo->status = 1;

                    WUD_BDCPY(pInfo->devAddr, pPair->bd_addr);
                    memcpy(pInfo->linkKey, pPair->link_key, LINK_KEY_LEN);
                }
            }
            else
            {
                WUD_BDCPY(pInfo->devAddr, pPair->bd_addr);
                memcpy(pInfo->linkKey, pPair->link_key, LINK_KEY_LEN);

                if (pInfo->UNK_0x5C == 2)
                {
                    pInfo->UNK_0x5C = 3;
                }

                WUD_BDCPY(p->pairAddr, pPair->bd_addr);
                DEBUGPrint("   LAST: %02x:%02x:%02x:%02x:%02x:%02x\n",
                    p->pairAddr[0],
                    p->pairAddr[1],
                    p->pairAddr[2],
                    p->pairAddr[3],
                    p->pairAddr[4],
                    p->pairAddr[5]);
            }

            pPair++;
            i++;
        }

        break;
    }

    case BTM_CB_EVT_READ_STORED_LINK_KEYS:
    {
        pRead = p1;

        DEBUGPrint("BTM_CB_EVT_READ_STORED_LINK_KEYS\n");
        DEBUGPrint("  status: %d  max_keys: %d  num_keys: %d\n", pRead->status, pRead->max_keys, pRead->read_keys);

        p->linkKeyState = WUD_STATE_START;
        break;
    }

    case BTM_CB_EVT_WRITE_STORED_LINK_KEYS:
    {
        pWrite = p1;

        DEBUGPrint("BTM_CB_EVT_WRITE_STORED_LINK_KEYS\n");
        DEBUGPrint("  status: %d  num_keys: %d\n", pWrite->status, pWrite->num_keys);

        p->linkKeyState = WUD_STATE_START;
        break;
    }

    case BTM_CB_EVT_DELETE_STORED_LINK_KEYS:
    {
        pDelete = p1;

        DEBUGPrint("BTM_CB_EVT_DELETE_STORED_LINK_KEYS\n");
        DEBUGPrint("  status: %d  num_keys: %d\n", pDelete->status, pDelete->num_keys);

        if (pDelete->num_keys == 0)
        {
            DEBUGPrint("WARNING: no entry is deleted\n");
        }

        p->linkKeyState = WUD_STATE_START;
        break;
    }

    default:
    {

        OSPanic(__FILE__, 5498, "Unknown event\n");
    }
    }
}

void WUDPowerManagerCallback(BD_ADDR addr, tBTM_PM_STATUS status, UINT16 value,
    UINT8 hciStatus)
{
#pragma unused(value)

    WUDDevInfo* pInfo;

    DEBUGPrint("__wudPowerMangeEventStackCallback\n");
    DEBUGPrint("hci_status = %d\n", hciStatus);

    pInfo = WUDiGetDevInfo(addr);
    if (pInfo == NULL)
    {
        if (WUD_BDCMP(_work.devAddr, addr) == 0)
        {
            pInfo = &_work;
        }
        else
        {
            DEBUGPrint("Unknown device is connected and changes the connection type!!!!\n");
            DEBUGPrint(" addr = %02x:%02x:%02x:%02x:%02x:%02x,  status = %d\n",
                addr[0],
                addr[1],
                addr[2],
                addr[3],
                addr[4],
                addr[5],
                status);
            return;
        }
    }

    switch (status)
    {
    case BTM_PM_STS_ACTIVE:
    {
        pInfo->status = 8;
        break;
    }

    case BTM_PM_STS_SNIFF:
    {
        pInfo->status = 9;
        break;
    }
    }

    DEBUGPrint(" addr = %02x:%02x:%02x:%02x:%02x:%02x,  status = %d\n",
        pInfo->devAddr[0],
        pInfo->devAddr[1],
        pInfo->devAddr[2],
        pInfo->devAddr[3],
        pInfo->devAddr[4],
        pInfo->devAddr[5],
        pInfo->status);
}

BD_ADDR_PTR _WUDGetDevAddr(UINT8 handle)
{
    BD_ADDR_PTR pAddr;
    BOOL enabled = OSDisableInterrupts();

    if (handle < WUD_MAX_DEV_ENTRY)
    {
        pAddr = _dev_handle_to_bda[handle];
    }
    else
    {
        pAddr = NULL;
    }

    OSRestoreInterrupts(enabled);
    return pAddr;
}

u16 _WUDGetQueuedSize(s8 handle)
{
    u16 queuedSize;
    BOOL enabled = OSDisableInterrupts();

    if ((u8)handle <= WUD_MAX_DEV_ENTRY - 1)
    {
        queuedSize = _dev_handle_queue_size[(u8)handle];
    }
    else
    {
        queuedSize = 0;
    }

    OSRestoreInterrupts(enabled);
    return queuedSize;
}

u16 _WUDGetNotAckedSize(s8 handle)
{
    u16 notAckedSize;
    BOOL enabled = OSDisableInterrupts();

    if ((u8)handle <= WUD_MAX_DEV_ENTRY - 1)
    {
        notAckedSize = _dev_handle_notack_num[(u8)handle];
    }
    else
    {
        notAckedSize = 0;
    }

    OSRestoreInterrupts(enabled);
    return notAckedSize;
}

u8 _WUDGetLinkNumber(void)
{
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    u8 num = p->linkedNum;

    OSRestoreInterrupts(enabled);
    return num;
}

BOOL _WUDEnableTestMode(void)
{
    DEBUGPrint("_WUDEnableTestMode\n");
    return BTM_EnableTestMode() == BTM_SUCCESS;
}

void _WUDStartSyncDevice(BD_ADDR devAddr, const char* devName)
{
    WUDCB* p = &_wcb;
    BOOL enabled;

    DEBUGPrint("_WUDStartSyncDevice()\n");
    if (WUDGetStatus() == WUD_LIB_STATUS_3 && !WUDIsBusy())
    {
        DEBUGPrint("start\n");
        enabled = OSDisableInterrupts();
        p->syncState = WUD_STATE_SYNC_IS_EXISTED_DEVICE;
        p->syncedNum = 0;
        memcpy(_discResp.devName, devName, 16);
        WUD_BDCPY(_discResp.devAddr, devAddr);
        OSCreateAlarm(&p->alarm);
        OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OS_MSEC_TO_TICKS(20), SyncHandler0);
        OSRestoreInterrupts(enabled);
    }
}

void _WUDDeleteStoredDevice(void)
{
    WUDCB* p = &_wcb;
    DEBUGPrint("_WUDDeleteStoreDevice()\n");
    WUDiDeleteAllLinkKeys();
    while (p->deleteState != WUD_STATE_START)
    {
    }
    DEBUGPrint("dev number = %d\n", WUDiGetDevNumber());
}

BOOL WUDRegisterDevice(BD_ADDR devAddr, LINK_KEY linkKey, const SCDevInfo* info)
{
    BD_ADDR empty = { 0 };
    WUDDevInfo* pInfo;
    BOOL enabled;
    int index;

    if (WUD_BDCMP(empty, devAddr) == 0)
    {
        return FALSE;
    }
    enabled = OSDisableInterrupts();
    pInfo = WUDiGetDevInfo(devAddr);
    if (pInfo == NULL && WUD_DEV_NAME_IS_WBC(info->devName))
    {
        pInfo = WUDiGetWbcDevice();
        DEBUGPrint("%s WBC\n", pInfo ? "Found" : "Not Found");
    }
    if (pInfo != NULL)
    {
        DEBUGPrint("Found the registered WBC\n");
        if (pInfo->status > 1)
        {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
        DEBUGPrint("Removed\n");
        WUDiMoveBottomStdDevInfoPtr(pInfo);
        WUDiRemoveDevice(pInfo->devAddr);
    }
    else
    {
        DEBUGPrint("Not Found WBC\n");
        if (WUDiGetDevNumber() == WUD_MAX_DEV_ENTRY_FOR_STD)
        {
            WUDiRemoveDevice(WUDiGetRemoveStdDevice()->devAddr);
        }
    }

    pInfo = WUDiGetNewStdDevInfo();
    if (pInfo == NULL)
    {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    if (pInfo->status != 0)
    {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    WUD_BDCPY(pInfo->devAddr, devAddr);
    memcpy(&pInfo->conf, info, sizeof(pInfo->conf));
    memcpy(pInfo->linkKey, linkKey, LINK_KEY_LEN);
    pInfo->subclass = 2;
    pInfo->hhAttrMask = 0x8074;
    pInfo->appID = 3;
    pInfo->status = 1;
    pInfo->UNK_0x5B = 0;
    pInfo->UNK_0x5C = 3;
    pInfo->UNK_0x5A = 0;
    WUDiRegisterDevice(pInfo->devAddr);
    WUDiMoveTopStdDevInfoPtr(pInfo);
    WUDiMoveTopOfUnusedStdDevice(pInfo);

    index = WUDiGetDevNumber() - 1;
    WUD_BDCPY(_scArray.devices[index].addr, pInfo->devAddr);
    memcpy(&_scArray.devices[index].info, &pInfo->conf, sizeof(pInfo->conf));
    OSRestoreInterrupts(enabled);
    return TRUE;
}

WUDDevInfo* WUDiGetDiscoverDevice(void)
{
    return &_work;
}

void WUDSetDeviceHistory(WUDChannel channel, const BD_ADDR addr)
{
    if (addr == NULL)
    {
        memset(&_scArray.devices[SC_MAX_DEV_ENTRY_FOR_STD + channel], 0, sizeof(SCBtDeviceInfo));
    }
    else
    {
        WUD_BDCPY(_scArray.devices[SC_MAX_DEV_ENTRY_FOR_STD + channel].addr, addr);
    }
    _scFlush = TRUE;
}

BOOL WUDIsLatestDevice(WUDChannel channel, const BD_ADDR addr)
{
    if (addr == NULL)
    {
        return FALSE;
    }
    return WUD_BDCMP(_scArray.devices[SC_MAX_DEV_ENTRY_FOR_STD + channel].addr, addr) == 0 ? TRUE : FALSE;
}

void WUDUpdateSCSetting(void)
{
    if (_scFlush && SCSetBtDeviceInfoArray(&_scArray))
    {
        SCFlushAsync(NULL);
        _scFlush = FALSE;
    }
}

void WUDiSetDevAddrForHandle(UINT8 handle, BD_ADDR addr)
{
    _dev_handle_to_bda[handle] = addr;
}

BD_ADDR_PTR WUDiGetDevAddrForHandle(UINT8 handle)
{
    return _dev_handle_to_bda[handle];
}

void WUDiSetQueueSizeForHandle(UINT8 handle, u16 size)
{
    _dev_handle_queue_size[handle] = size;
}

u16 WUDiGetQueueSizeForHandle(UINT8 handle)
{
    return _dev_handle_queue_size[handle];
}

void WUDiSetNotAckNumForHandle(UINT8 handle, u16 count)
{
    _dev_handle_notack_num[handle] = count;
}

u16 WUDiGetNotAckNumForHandle(UINT8 handle)
{
    return _dev_handle_notack_num[handle];
}
