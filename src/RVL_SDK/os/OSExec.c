#include <decomp.h>
#include <revolution/dvd/dvd.h>
#include <revolution/esp/esp.h>
#include <revolution/ipc.h>
#include <revolution/os.h>
#include <stdio.h>
#include <string.h>

#define MENU_TITLE_ID    0x0000000100000002ULL
#define TICKET_VIEW_SIZE 0xD8
#define ES_ERR_INVALID   -1017
#define DVD_INTTYPE_TC   1

typedef void (*DVDLowCallback)(u32 intType);

BOOL DVDLowInit(void);
BOOL DVDLowReadDiskID(DVDDiskID* out, DVDLowCallback callback);
BOOL DVDLowOpenPartition(u32 offset, const void* ticket, u32 certsSize,
    const void* certs, ESPTmd* tmd,
    DVDLowCallback callback);
BOOL DVDLowClosePartition(DVDLowCallback callback);
BOOL DVDLowUnencryptedRead(void* dst, u32 size, u32 offset,
    DVDLowCallback callback);

typedef struct DVDGameTOC
{
    u32 numGamePartitions;
    u32 partitionInfos;
    u8 padding[0x18];
} DVDGameTOC;

typedef struct DVDPartitionInfo
{
    u32 gamePartition;
    u32 type;
} DVDPartitionInfo;

typedef struct ESTicketView
{
    u8 data[TICKET_VIEW_SIZE];
} ESTicketView;

typedef struct ImgHeader
{
    char date[16];
    u32 entry;
    u32 size;
    u32 rebootSize;
    u32 reserved2;
} ImgHeader;

typedef void (*AppInitFunc)(void (*report)(const char*, ...));
typedef BOOL (*AppGetNextFunc)(void**, u32*, u32*);
typedef void* (*AppGetEntryFunc)(void);
typedef void (*AppInterfaceFunc)(AppInitFunc*, AppGetNextFunc*, AppGetEntryFunc*);

OSExecParams* __OSExecParamsAddr AT_ADDRESS(0x800030F0);
s32 __OSCurrentTGCOffset AT_ADDRESS(0x800030F4);
vu8 __OSLockedFlag AT_ADDRESS(0x80003187);
vu32 __OSLaunchPartitionType AT_ADDRESS(0x80003194);
vu32 __OSLaunchPartitionOffset AT_ADDRESS(0x80003198);
vu8 __OSBootDolFlag AT_ADDRESS(0x800030E2);
void* BOOT_REGION_END AT_ADDRESS(0x812FDFEC);
void* BOOT_REGION_START AT_ADDRESS(0x812FDFF0);

static u8 views[0xBD00] ALIGN(32);
static DVDGameTOC gameTocBuffer;
static DVDPartitionInfo partInfoBuffer[4];
static u8 tmdBuffer[0x4A00] ALIGN(64);
static DVDDiskID id;

static volatile BOOL Prepared;
static DVDGameTOC* gameToc;
static DVDPartitionInfo* partInfo;
static DVDPartitionInfo* bootGameInfo;
static u8* tmd;
BOOL __OSInReboot;
static volatile u32 DVDLowIntType;

static s32 _ES_GetTicketViews(s32* fd, u64 tid, void* pViews, u32* count);
static s32 _ES_LaunchTitle(s32* fd, u64 tid, void* pViews);

static BOOL PackArgs(void* addr, s32 argc, char* argv[])
{
    s32 numArgs;
    char* bootInfo2;
    char* ptr;
    char** list;
    u32 i;

    bootInfo2 = (char*)addr;
    memset(bootInfo2, 0, 0x2000);

    if (argc == 0)
    {
        *(u32*)&bootInfo2[8] = 0;
    }
    else
    {
        numArgs = argc;
        ptr = bootInfo2 + 0x2000;
        while (--argc >= 0)
        {
            ptr -= strlen(argv[argc]) + 1;
            strcpy(ptr, argv[argc]);
            argv[argc] = (char*)(ptr - bootInfo2);
        }

        ptr = bootInfo2 + ((ptr - bootInfo2) & ~3);
        ptr -= 4 * (numArgs + 1);
        list = (char**)ptr;

        for (i = 0; i < numArgs + 1; i++)
        {
            list[i] = argv[i];
        }

        ptr -= 4;
        *(s32*)ptr = numArgs;
        *(u32*)&bootInfo2[8] = (u32)(ptr - bootInfo2);
    }

    return TRUE;
}

// clang-format off
static asm void Run(register void* entryPoint) {
    fralloc
    bl ICFlashInvalidate
    sync
    isync
    mtctr entryPoint
    bctr
    frfree
    blr
}
// clang-format on

static void Callback(s32 result, DVDCommandBlock* block)
{
    Prepared = TRUE;
}

void __OSGetExecParams(OSExecParams* params)
{
    if (0x80000000 <= (u32)__OSExecParamsAddr)
    {
        memcpy(params, __OSExecParamsAddr, sizeof(OSExecParams));
    }
    else
    {
        params->valid = FALSE;
    }
}

static void callback(u32 intType)
{
    DVDLowIntType = intType;
}

static s32 _ES_InitLib(s32* fd)
{
    s32 result;

    result = IPC_RESULT_OK;
    *fd = IOS_Open("/dev/es", IPC_OPEN_NONE);
    if (*fd < 0)
    {
        result = *fd;
    }

    return result;
}

static s32 _ES_GetTitleId(s32* fd, u64* tidOut)
{
    s32 result;
    u8 tidWork[256] ALIGN(32);
    u8 vectorWork[32] ALIGN(32);
    IPCIOVector* pVectors = (IPCIOVector*)vectorWork;
    u64* pTid = (u64*)tidWork;

    if (*fd < 0 || tidOut == ((void*)NULL))
    {
        return ES_ERR_INVALID;
    }

    pVectors[0].base = pTid;
    pVectors[0].length = sizeof(u64);
    result = IOS_Ioctlv(*fd, ES_IOCTLV_GET_TITLE_ID, 0, 1, pVectors);
    if (result == IPC_RESULT_OK)
    {
        *tidOut = *pTid;
    }

    return result;
}

static void CheckDVDLowIntType(void)
{
    if (DVDLowIntType != DVD_INTTYPE_TC)
    {
        OSReport("\nsecurity error(%d) has occurred", DVDLowIntType);
        __OSReturnToMenuForError();
    }
}

void __OSLaunchNextFirmware(void)
{
    u8 i;
    s32 rc;
    u32 ticketCnt = 1;
    ESTicketView* t = (ESTicketView*)views;
    u64 version;
    s32 fd = -1;
    struct
    {
        u8 tmp[4];
    } unused = { 0xFF, 0xFF, 0xFF, 0 };

    DVDLowIntType = 0;
    DVDLowClosePartition(callback);
    while (!DVDLowIntType)
    {
    }
    CheckDVDLowIntType();

    gameToc = &gameTocBuffer;
    DVDLowIntType = 0;
    DVDLowUnencryptedRead(gameToc, sizeof(DVDGameTOC), 0x10000, callback);
    while (!DVDLowIntType)
    {
    }
    CheckDVDLowIntType();

    partInfo = partInfoBuffer;
    DVDLowIntType = 0;
    DVDLowUnencryptedRead(partInfo, sizeof(partInfoBuffer), gameToc->partitionInfos, callback);
    while (!DVDLowIntType)
    {
    }
    CheckDVDLowIntType();

    bootGameInfo = NULL;
    for (i = 0; i < gameToc->numGamePartitions; i++)
    {
        if (partInfo->type == 0)
        {
            bootGameInfo = partInfo;
        }
        partInfo++;
    }

    DVDLowIntType = 0;
    DVDLowOpenPartition(bootGameInfo->gamePartition, NULL, 0, NULL, (ESPTmd*)tmdBuffer, callback);
    while (!DVDLowIntType)
    {
    }
    CheckDVDLowIntType();

    DVDLowIntType = 0;
    DVDLowClosePartition(callback);
    while (!DVDLowIntType)
    {
    }
    CheckDVDLowIntType();

    tmd = tmdBuffer;
    version = *(u64*)(tmd + 0x184);

    rc = _ES_InitLib(&fd);
    if (rc != IPC_RESULT_OK)
    {
        OSReport("\nsecurity error(%d) has occurred", rc);
        __OSHotReset();
    }

    rc = _ES_GetTicketViews(&fd, version, NULL, &ticketCnt);
    if (ticketCnt != 1 || rc != IPC_RESULT_OK)
    {
        OSReport("\nsecurity error(%d) has occurred", rc);
        __OSHotReset();
    }

    rc = _ES_GetTicketViews(&fd, version, t, &ticketCnt);
    if (rc != IPC_RESULT_OK)
    {
        OSReport("\nsecurity error(%d) has occurred", rc);
        __OSHotReset();
    }

    rc = _ES_LaunchTitle(&fd, version, t);
    if (rc != IPC_RESULT_OK)
    {
        OSReport("\nsecurity error(%d) has occurred", rc);
        __OSHotReset();
    }

    if (fd >= 0 && IOS_Close(fd) == IPC_RESULT_OK)
    {
        fd = -1;
    }

    DVDLowInit();
    DVDLowIntType = 0;
    DVDLowReadDiskID(&id, callback);
    while (!DVDLowIntType)
    {
    }
    CheckDVDLowIntType();

    gameToc = &gameTocBuffer;
    DVDLowIntType = 0;
    DVDLowUnencryptedRead(gameToc, sizeof(DVDGameTOC), 0x10000, callback);
    while (!DVDLowIntType)
    {
    }
    CheckDVDLowIntType();

    partInfo = partInfoBuffer;
    DVDLowIntType = 0;
    DVDLowUnencryptedRead(partInfo, sizeof(partInfoBuffer), gameToc->partitionInfos, callback);
    while (!DVDLowIntType)
    {
    }
    CheckDVDLowIntType();

    bootGameInfo = NULL;
    for (i = 0; i < gameToc->numGamePartitions; i++)
    {
        if (partInfo->type == 0)
        {
            bootGameInfo = partInfo;
        }
        partInfo++;
    }

    DVDLowIntType = 0;
    DVDLowOpenPartition(bootGameInfo->gamePartition, NULL, 0, NULL, (ESPTmd*)tmdBuffer, callback);
    while (!DVDLowIntType)
    {
    }
    CheckDVDLowIntType();
}

void __OSLaunchMenu(void)
{
    s32 result;
    void* pviews = views;
    u32 count = 1;
    s32 fd = -1;
    struct
    {
        u8 tmp[4];
    } unused = { 0xFF, 0xFF, 0xFF, 0 };

    if (_ES_InitLib(&fd) != IPC_RESULT_OK)
    {
        return;
    }

    result = _ES_GetTicketViews(&fd, MENU_TITLE_ID, NULL, &count);
    if (count != 1 || result != IPC_RESULT_OK)
    {
        return;
    }

    if (_ES_GetTicketViews(&fd, MENU_TITLE_ID, pviews, &count) != IPC_RESULT_OK)
    {
        return;
    }

    if (_ES_LaunchTitle(&fd, MENU_TITLE_ID, pviews) != IPC_RESULT_OK)
    {
        return;
    }

    while (TRUE)
    {
    }
}

void __OSRelaunchTitle(void)
{
    s32 result;
    u64 titleId ALIGN(32);
    ESTicketView* pviews;
    u32 count = 1;
    s32 fd = -1;

    if (_ES_InitLib(&fd) != IPC_RESULT_OK)
    {
        return;
    }

    result = _ES_GetTitleId(&fd, &titleId);
    if (result != IPC_RESULT_OK)
    {
        return;
    }

    result = _ES_GetTicketViews(&fd, titleId, NULL, &count);
    if (result != IPC_RESULT_OK)
    {
        return;
    }

    pviews = OSAllocFromMEM1ArenaLo(count * sizeof(ESTicketView), 32);
    result = _ES_GetTicketViews(&fd, titleId, pviews, &count);
    if (result != IPC_RESULT_OK)
    {
        return;
    }

    if (_ES_LaunchTitle(&fd, titleId, pviews) != IPC_RESULT_OK)
    {
        return;
    }

    while (TRUE)
    {
    }
}

static inline void ReadDisc(void* addr, s32 length, u32 offset)
{
    DVDCommandBlock block;

    DVDReadAbsAsyncPrio(&block, addr, length, offset, NULL, 0);
    while (DVDGetCommandBlockStatus(&block) != DVD_STATE_IDLE)
    {
        if (DVDGetCommandBlockStatus(&block) > DVD_STATE_WAITING || DVDGetCommandBlockStatus(&block) < DVD_STATE_IDLE)
        {
            __OSReturnToMenuForError();
        }
    }
}

static u32 GetApploaderPosition(void)
{
    static u32 apploaderPosition = 0;

    if (apploaderPosition)
    {
        return apploaderPosition;
    }

    if (__OSCurrentTGCOffset)
    {
        u32* tgcHeader;
        s32 apploaderOffsetInTGC;

        tgcHeader = OSAllocFromMEM1ArenaLo(64, 32);
        ReadDisc(tgcHeader, 64, __OSCurrentTGCOffset >> 2);
        apploaderOffsetInTGC = *(s32*)((u8*)tgcHeader + 56);
        apploaderPosition = (__OSCurrentTGCOffset + apploaderOffsetInTGC) >> 2;
    }
    else
    {
        apploaderPosition = 0x910;
    }

    return apploaderPosition;
}

static ImgHeader* LoadApploader(void)
{
    ImgHeader* header;

    header = OSAllocFromMEM1ArenaLo(sizeof(ImgHeader), 32);
    ReadDisc(header, sizeof(ImgHeader), GetApploaderPosition());
    ReadDisc((void*)0x81200000, OSRoundUp32B(header->size), GetApploaderPosition() + (sizeof(ImgHeader) >> 2));
    ICInvalidateRange((void*)0x81200000, OSRoundUp32B(header->size));

    return header;
}

static BOOL IsNewApploader(ImgHeader* header)
{
    if (strncmp(header->date, "2004/02/01", 10) > 0)
    {
        return TRUE;
    }
    return FALSE;
}

static void __OSSetExecParams(const OSExecParams* params, OSExecParams* addr)
{
    memcpy(addr, params, sizeof(OSExecParams));
    __OSExecParamsAddr = addr;
}

static void* LoadDol(const OSExecParams* params, AppInterfaceFunc getInterface)
{
    AppInitFunc appInit;
    AppGetNextFunc appGetNext;
    AppGetEntryFunc appGetEntry;
    void* addr;
    u32 length;
    u32 offset;
    OSExecParams* paramsWork;

    getInterface(&appInit, &appGetNext, &appGetEntry);
    paramsWork = OSAllocFromMEM1ArenaLo(sizeof(OSExecParams), 1);
    __OSSetExecParams(params, paramsWork);
    appInit(OSReport);
    OSSetArenaLo(paramsWork);

    while (appGetNext(&addr, &length, &offset))
    {
        ReadDisc(addr, length, offset >> __DVDLayoutFormat);
    }

    return appGetEntry();
}

static void StartDol(const OSExecParams* params, void* entry)
{
    OSExecParams* paramsWork;

    paramsWork = OSAllocFromMEM1ArenaLo(sizeof(OSExecParams), 1);
    __OSSetExecParams(params, paramsWork);
    PI_HW_REGS[9] = 7;
    OSDisableInterrupts();
    Run(entry);
}

void __OSBootDolSimple(u32 doloffset, u32 restartCode, void* regionStart,
    void* regionEnd, BOOL argsUseDefault, s32 argc,
    char* argv[])
{
    OSExecParams* params;
    void* dolEntry;
    ImgHeader* header;

    OSDisableInterrupts();

    params = OSAllocFromMEM1ArenaLo(sizeof(OSExecParams), 1);
    params->valid = TRUE;
    params->restartCode = restartCode;
    params->regionStart = regionStart;
    params->regionEnd = regionEnd;
    params->argsUseDefault = argsUseDefault;

    if (!argsUseDefault)
    {
        params->argsAddr = OSAllocFromMEM1ArenaLo(0x2000, 1);
        PackArgs(params->argsAddr, argc, argv);
    }

    DVDInit();
    DVDSetAutoInvalidation(TRUE);
    DVDResume();

    Prepared = FALSE;
    __DVDPrepareResetAsync(Callback);
    __OSMaskInterrupts(0xFFFFFFF0);
    __OSUnmaskInterrupts(0x10);
    OSEnableInterrupts();

    while (Prepared != TRUE)
    {
    }

    __OSLaunchNextFirmware();

    header = LoadApploader();
    if (IsNewApploader(header))
    {
        if (doloffset == 0xFFFFFFFF)
        {
            doloffset = GetApploaderPosition() + ((sizeof(ImgHeader) + header->size) >> 2);
        }

        params->bootDol = doloffset;
        dolEntry = LoadDol(params, (AppInterfaceFunc)header->entry);
        StartDol(params, dolEntry);
    }
    else
    {
        BOOT_REGION_START = regionStart;
        BOOT_REGION_END = regionEnd;
        __OSBootDolFlag = 1;

        ReadDisc((void*)0x81330000, OSRoundUp32B(header->rebootSize), GetApploaderPosition() + ((sizeof(ImgHeader) + header->size) >> 2));
        ICInvalidateRange((void*)0x81330000,
            OSRoundUp32B(header->rebootSize));
        OSDisableInterrupts();
        ICFlashInvalidate();
        Run((void*)0x81330000);
    }
}

void __OSBootDol(u32 doloffset, u32 restartCode, char* const argv[])
{
    char doloffInString[20];
    s32 argvlen;
    char** argvToPass;
    s32 i;
    void* saveStart;
    void* saveEnd;

    OSGetSaveRegion(&saveStart, &saveEnd);
    sprintf(doloffInString, "%d", doloffset);

    argvlen = 0;
    if (argv)
    {
        while (argv[argvlen])
        {
            argvlen++;
        }
    }

    argvlen++;
    argvToPass = OSAllocFromMEM1ArenaLo((argvlen + 1) * sizeof(char*), 1);
    argvToPass[0] = doloffInString;

    for (i = 1; i < argvlen; i++)
    {
        argvToPass[i] = argv[i - 1];
    }

    __OSBootDolSimple(0xFFFFFFFF, restartCode, saveStart, saveEnd, FALSE, argvlen, argvToPass);
}

static s32 _ES_GetTicketViews(s32* fd, u64 tid, void* pViews, u32* count)
{
    s32 result;
    u8 work[0x120] ALIGN(32);
    IPCIOVector* pVectors = (IPCIOVector*)(work + 0x0);
    u64* pTid = (u64*)(work + 0x20);
    u32* pCount = (u32*)(work + 0x40);

    if (*fd < 0 || count == ((void*)NULL))
    {
        return ES_ERR_INVALID;
    }

    if ((u32)pViews % 32 != 0)
    {
        return ES_ERR_INVALID;
    }

    *pTid = tid;

    if (pViews == (void*)NULL)
    {
        pVectors[0].base = pTid;
        pVectors[0].length = sizeof(u64);
        pVectors[1].base = pCount;
        pVectors[1].length = sizeof(u32);

        result = IOS_Ioctlv(*fd, ES_IOCTLV_GET_NUM_TICKET_VIEWS, 1, 1, pVectors);
        if (result == IPC_RESULT_OK)
        {
            *count = *pCount;
        }

        return result;
    }

    if (*count == 0)
    {
        return ES_ERR_INVALID;
    }

    *pCount = *count;
    pVectors[0].base = pTid;
    pVectors[0].length = sizeof(u64);
    pVectors[1].base = pCount;
    pVectors[1].length = sizeof(u32);
    pVectors[2].base = pViews;
    pVectors[2].length = *count * TICKET_VIEW_SIZE;

    return IOS_Ioctlv(*fd, ES_IOCTLV_GET_TICKET_VIEWS, 2, 1, pVectors);
}

static s32 _ES_LaunchTitle(s32* fd, u64 tid, void* pViews)
{
    u8 tidWork[256] ALIGN(32);
    u8 vectorWork[32] ALIGN(32);
    IPCIOVector* pVectors = (IPCIOVector*)vectorWork;
    u64* pTid = (u64*)tidWork;

    if (*fd < 0)
    {
        return ES_ERR_INVALID;
    }

    if ((u32)pViews % 32 != 0)
    {
        return ES_ERR_INVALID;
    }

    *pTid = tid;
    pVectors[0].base = pTid;
    pVectors[0].length = sizeof(u64);
    pVectors[1].base = pViews;
    pVectors[1].length = TICKET_VIEW_SIZE;

    return IOS_IoctlvReboot(*fd, ES_IOCTLV_LAUNCH_TITLE, 2, 0, pVectors);
}
