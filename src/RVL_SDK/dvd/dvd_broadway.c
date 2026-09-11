#include <revolution/dvd.h>
#include <string.h>
#include <revolution/esp.h>
#include <revolution/ipc.h>
#include <revolution/os.h>

static volatile u8 requestInProgress = false;
static u8 breakRequested;
static u8 callbackInProgress = false;

s32 DiFD = -1;

static int freeCommandBuf = 0;

static int freeDvdContext = 0;
static u8 dvdContextsInited = false;
static u8 DVDLowInitCalled = false;
static diCommand_t* diCommand;
static char* pathBuf;

static u32 readLength;
static u32 spinUpValue;

typedef enum callbackType { BOGUS_TYPE = 0, TRANSACTION_CB, COVER_CB, COVER_REG_CB } callbackType_t;

typedef struct dvdContext {
    DVDLowCallback callback;
    callbackType_t callbackType;
    u8 inUse;
    u32 contextMagic;
    u32 contextNum;
    u32 pad[3];
} dvdContext_t;

static dvdContext_t dvdContexts[4] __attribute__((aligned(32)));
static diRegVals_t diRegValCache __attribute__((aligned(32)));
static u32 registerBuf[8] __attribute__((aligned(32)));

static u32 coverRegister[8] __attribute__((aligned(32)));
static u32 coverStatus[8] __attribute__((aligned(32)));
static s32 lastTicketError[8] __attribute__((aligned(32)));

static IPCIOVector ioVec[5] __attribute__((aligned(32)));

static void* ddrAllocAligned32(const int size) {
    void *low, *high;

    if ((size & 0x1F) != 0) {
        return 0;
    }

    low = IPCGetBufferLo();
    high = IPCGetBufferHi();

    if (((u32)low & 0x1F) != 0) {
        low = (void*)(((u32)low + 31) & 0x1F);
    }

    if ((u32)low + size > (u32)high) {
        OSReport("(ddrAllocAligned32) Not enough space to allocate %d bytes\n", size);
    }

    IPCSetBufferLo((void*)((u32)low + size));
    return low;
}

static BOOL allocateStructures(void) {
    if ((diCommand = ddrAllocAligned32(sizeof(diCommand_t) * 4)) == 0) {
        OSReport("Allocation of diCommand blocks failed\n");
        return false;
    }

    if ((pathBuf = ddrAllocAligned32(32)) == 0) {
        OSReport("Allocation of pathBuf failed\n");
        return false;
    }

    return true;
}

void initDvdContexts(void) {
    u32 i;

    for (i = 0; i < 4; i++) {
        dvdContexts[i].callback = 0;
        dvdContexts[i].callbackType = 0;
        dvdContexts[i].inUse = false;
        dvdContexts[i].contextMagic = 0xFEEBDAED;
        dvdContexts[i].contextNum = i;
    }

    freeDvdContext = 0;
}

static inline dvdContext_t* newContext(const DVDLowCallback callback, const callbackType_t type) {
    int returnIndex;
    u8 use = dvdContexts[freeDvdContext].inUse != 0;

    if (use == 1) {
        OSReport("(newContext) ERROR: freeDvdContext.inUse (#%d) is true\n", freeDvdContext);
        OSReport("(newContext) Now spinning in infinite loop\n");

        while (1) {
        }
    }

    if (dvdContexts[freeDvdContext].contextMagic != 0xFEEBDAED) {
        OSReport("(newContext) Something overwrote the context magic - spinning \n");

        while (1) {
        }
    }

    dvdContexts[freeDvdContext].callback = callback;
    dvdContexts[freeDvdContext].callbackType = type;
    dvdContexts[freeDvdContext].inUse = true;
    returnIndex = freeDvdContext;
    freeDvdContext++;

    if (freeDvdContext >= 4) {
        freeDvdContext = 0;
    }

    return dvdContexts + returnIndex;
}

static inline void nextCommandBuf(int* bufNum) {
    (*bufNum)++;

    if (*bufNum >= 4) {
        *bufNum = 0;
    }
}

s32 doTransactionCallback(s32 ret, void* context) {
    dvdContext_t* dvdContext = context;

    if (dvdContext->contextMagic != 0xFEEBDAED) {
        OSReport("(doTransactionCallback) Error - context mangled!\n");
        dvdContext->contextMagic = 0xFEEBDAED;
        goto out;
    }

    requestInProgress = false;

    if (dvdContext->callback != NULL) {
        int callbackArg;
        callbackInProgress = TRUE;
        callbackArg = ret;

        if (breakRequested == TRUE) {
            breakRequested = false;
            callbackArg |= 8;
        }

        if (callbackArg & 1) {
            readLength = 0;
        }

        dvdContext->callback((u32)callbackArg);
        callbackInProgress = false;
    }

out:
    dvdContext->inUse = false;
    return 0;
}

static s32 doCoverCallback(s32 ret, void* context) {
    dvdContext_t* dvdContext;

    requestInProgress = false;
    dvdContext = (dvdContext_t*)context;
    if (dvdContext->contextMagic != 0xfeebdaed) {
        OSReport("(doCoverCallback) Error - context mangled!\n");
        dvdContext->contextMagic = 0xfeebdaed;
        goto out;
    }
    if (dvdContext->callback != NULL) {
        s32 callbackArg;
        callbackInProgress = true;
        callbackArg = ret;
        if (breakRequested == true) {
            breakRequested = false;
            callbackArg |= 0x00000008;
        }
        dvdContext->callback((u32)callbackArg);
        callbackInProgress = false;
    }
out:
    dvdContext->inUse = false;

    return 0;
}

s32 doPrepareCoverRegisterCallback(s32 ret, void* context) {
    dvdContext_t* dvdContext;
    requestInProgress = false;

    diRegValCache.CoverRegVal = registerBuf[0];
    dvdContext = (dvdContext_t*)context;

    if (dvdContext->contextMagic != 0xFEEBDAED) {
        OSReport("(doTransactionCallback) Error - context mangled!\n");
        dvdContext->contextMagic = 0xFEEBDAED;
    } else {
        if (dvdContext->callback != 0) {
            callbackInProgress = true;

            if (breakRequested == true) {
                breakRequested = false;
                ret |= 8;
            }

            dvdContext->callback(ret);
            callbackInProgress = false;
        }
    }

    dvdContext->inUse = false;
    return 0;
}

BOOL DVDLowInit(void) {
    s32 retVal;

    if (DVDLowInitCalled == false) {
        DVDLowInitCalled = true;
        retVal = IPCCltInit();

        if (retVal != IPC_RESULT_OK) {
            OSReport("IPCCltInit returned error: %d\n", retVal);
            return false;
        }

        if (allocateStructures() == false) {
            return false;
        }

        if (dvdContextsInited == false) {
            initDvdContexts();
            dvdContextsInited = true;
        }
    }

    strncpy(pathBuf, "/dev/di", 32);
    DiFD = IOS_Open(pathBuf, 0);

    if (DiFD >= 0) {
        return true;
    } else {
        switch (DiFD) {
        case IPC_RESULT_NOEXISTS_INTERNAL:
            OSReport("(DVDLowInit) Error: IOS_Open failed - pathname '/dev/di' does not exist\n");
            return false;
            break;
        case IPC_RESULT_ACCESS_INTERNAL:
            OSReport("(DVDLowInit) Error: IOS_Open failed - calling thread lacks permission\n");
            return false;
            break;
        case IPC_RESULT_CONN_MAX_INTERNAL:
            OSReport("(DVDLowInit) Error: IOS_Open failed - connection limit has been reached\n");
            return false;
            break;
        default:
            OSReport("(DVDLowInit) IOS_Open failed, errorcode = %d\n", DiFD);
            return false;
            break;
        }
    }
}

BOOL DVDLowReadDiskID(DVDDiskID* diskID, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    if (diskID == 0) {
        OSReport("@@@@@@ WARNING - Calling DVDLowReadDiskId with NULL ptr\n");
    }

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x70;

    rv = IOS_IoctlAsync(DiFD, 0x70, &diCommand[freeCommandBuf], sizeof(diCommand_t), diskID, sizeof(DVDDiskID), doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReadDiskID) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowOpenPartition(const u32 partitionWordOffset, const ESPTicket* const eTicket, const u32 numCertBytes, const void* const certificates,
                         ESPTmd* tmd, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    if (eTicket != 0 && ((u32)eTicket & 0x1F)) {
        OSReport("(DVDLowOpenPartition) eTicket memory is unaligned\n");
        return false;
    }

    if (certificates != 0 && ((u32)certificates & 0x1F)) {
        OSReport("(DVDLowOpenPartition) certificates memory is unaligned\n");
        return false;
    }

    if (tmd != 0 && ((u32)tmd & 0x1F)) {
        OSReport("(DVDLowOpenPartition) certificates memory is unaligned\n");
        return false;
    }

    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x8B;
    diCommand[freeCommandBuf].arg[0] = partitionWordOffset;
    ioVec[0].base = (u8*)&diCommand[freeCommandBuf];
    ioVec[0].length = sizeof(diCommand_t);

    ioVec[1].base = (u8*)eTicket;
    if (eTicket == 0) {
        ioVec[1].length = 0;
    } else {
        ioVec[1].length = sizeof(ESPTicket);
    }

    ioVec[2].base = (u8*)certificates;
    if (certificates == 0) {
        ioVec[2].length = 0;
    } else {
        ioVec[2].length = numCertBytes;
    }

    ioVec[3].base = (u8*)tmd;
    ioVec[3].length = sizeof(ESPTmd);

    ioVec[4].base = (u8*)&lastTicketError[0];
    ioVec[4].length = sizeof(lastTicketError);

    rv = IOS_IoctlvAsync(DiFD, 0x8B, 3, 2, ioVec, doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowOpenPartition) IOS_IoctlvAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowClosePartition(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x8C;

    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    rv = IOS_IoctlAsync(DiFD, 0x8C, &diCommand[freeCommandBuf], sizeof(diCommand_t), 0, 0, doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowClosePartition) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowUnencryptedRead(void* destAddr, u32 length, u32 wordOffset, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, 1);
    readLength = length;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x8D;
    diCommand[freeCommandBuf].arg[0] = length;
    diCommand[freeCommandBuf].arg[1] = wordOffset;

    rv = IOS_IoctlAsync(DiFD, 0x8D, &diCommand[freeCommandBuf], sizeof(diCommand_t), destAddr, length, doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowUnencryptedRead) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowStopMotor(BOOL eject, BOOL saving, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xE3;
    diCommand[freeCommandBuf].arg[0] = eject;
    diCommand[freeCommandBuf].arg[1] = saving;

    rv = IOS_IoctlAsync(DiFD, 0xE3, &diCommand[freeCommandBuf], sizeof(diCommand_t), &diRegValCache, sizeof(diRegVals_t), doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowStopMotor) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowWaitForCoverClose(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, COVER_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x79;

    rv = IOS_IoctlAsync(DiFD, 0x79, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), NULL, 0, doCoverCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowWaitForCoverClose) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowInquiry(DVDDriveInfo* info, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x12;

    rv = IOS_IoctlAsync(DiFD, 0x12, &diCommand[freeCommandBuf], sizeof(diCommand_t), info, sizeof(DVDDriveInfo), doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowInquiry) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowRequestError(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xE0;

    rv = IOS_IoctlAsync(DiFD, 0xE0, &diCommand[freeCommandBuf], sizeof(diCommand_t), &diRegValCache, sizeof(diRegVals_t), doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowRequestError) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowSetSpinupFlag(BOOL spinUp) {
    spinUpValue = spinUp;
    return true;
}

BOOL DVDLowNotifyReset(void) {
    s32 rv;

    if (callbackInProgress == true) {
        OSReport("(DVDLowSetSpinupFlag): Synch functions can't be called in callbacks\n");
        return false;
    }

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x7E;

    rv = IOS_Ioctl(DiFD, 0x7E, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), NULL, 0);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowNotifyReset) IOS_IoctlAsync returned error: %d\n", rv);
        return false;
    }

    return true;
}

BOOL DVDLowReset(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x8A;
    diCommand[freeCommandBuf].arg[0] = spinUpValue;

    rv = IOS_IoctlAsync(DiFD, 0x8A, &diCommand[freeCommandBuf], sizeof(diCommand_t), 0, 0, doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReset) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowAudioBufferConfig(BOOL enable, u32 size, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xE4;
    diCommand[freeCommandBuf].arg[0] = enable;
    diCommand[freeCommandBuf].arg[1] = size;

    rv = IOS_IoctlAsync(DiFD, 0xE4, &diCommand[freeCommandBuf], sizeof(diCommand_t), &diRegValCache, sizeof(diRegVals_t), doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowAudioBufferConfig) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

u32 DVDLowGetCoverStatus(void) {
    s32 rv;

    if (callbackInProgress == true) {
        OSReport("(DVDLowGetCoverStatus): Synch functions can't be called in callbacks\n");
        return false;
    }

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x88;

    rv = IOS_Ioctl(DiFD, 0x88, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), coverStatus, sizeof(u32) * 8);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowGetCoverStatus) IOS_Ioctl returned error: %d\n", rv);
        return 0xdeaddead;
    }

    return coverStatus[0];
}

BOOL DVDLowReadDvd(u32 strm, u32 retry, void* destAddr, u32 lengthInSectors, u32 lsn, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xD0;
    if (strm == 0) {
        diCommand[freeCommandBuf].arg[0] = 0x0;
    } else {
        diCommand[freeCommandBuf].arg[0] = 0x1;
    }
    if (retry == 0) {
        diCommand[freeCommandBuf].arg[1] = 0x0;
    } else {
        diCommand[freeCommandBuf].arg[1] = 0x1;
    }
    diCommand[freeCommandBuf].arg[2] = lengthInSectors;
    diCommand[freeCommandBuf].arg[3] = lsn;
    readLength = lengthInSectors * 2048;

    rv = IOS_IoctlAsync(DiFD, 0xD0, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), destAddr, lengthInSectors * 2048, doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReadDVD) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowReadDvdConfig(u8 set, u32 type, u32 config, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xD1;
    diCommand[freeCommandBuf].arg[0] = set;
    diCommand[freeCommandBuf].arg[1] = type;
    diCommand[freeCommandBuf].arg[2] = config;

    rv = IOS_IoctlAsync(DiFD, 0xD1, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache, sizeof(diRegVals_t), doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReadDVDConfig) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowReadDvdCopyright(u32 layer, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x81;
    diCommand[freeCommandBuf].arg[0] = layer;

    rv = IOS_IoctlAsync(DiFD, 0x81, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache, sizeof(diRegVals_t), doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReadDvdCopyright) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowReadDvdPhysical(DVDVideoPhysical* physical, u32 layer, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x80;
    diCommand[freeCommandBuf].arg[0] = layer;
    if ((sizeof(DVDVideoPhysical) & 0x1F) != 0) {
        OSReport("Size of DVDVideoPhysical is not a multiple of 32!\n");
        return false;
    }

    rv = IOS_IoctlAsync(DiFD, 0x80, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), physical, sizeof(DVDVideoPhysical), doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReadDvdPhysical) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowReadDvdDiscKey(DVDVideoDiscKey* diskKey, u32 layer, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x82;
    diCommand[freeCommandBuf].arg[0] = layer;

    rv = IOS_IoctlAsync(DiFD, 0x82, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), diskKey, sizeof(DVDVideoDiscKey), doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReadDvdDiscKey) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowReportKey(DVDVideoReportKey* reportKey, u32 format, u32 lsn, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xA4;
    diCommand[freeCommandBuf].arg[0] = format >> 16;
    diCommand[freeCommandBuf].arg[1] = lsn;

    rv = IOS_IoctlAsync(DiFD, 0xA4, &diCommand[freeCommandBuf], sizeof(diCommand_t), reportKey, sizeof(DVDVideoReportKey), doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReportKey) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowOffset(u32 subcmd, u32 offset_4_byte, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xD9;
    if (subcmd == 0) {
        diCommand[freeCommandBuf].arg[0] = 0x0;
    } else {
        diCommand[freeCommandBuf].arg[0] = 0x1;
    }
    diCommand[freeCommandBuf].arg[1] = offset_4_byte;

    rv = IOS_IoctlAsync(DiFD, 0xD9, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache, sizeof(diRegVals_t), doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowOffset) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowStopLaser(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xD2;

    rv = IOS_IoctlAsync(DiFD, 0xD2, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache, sizeof(diRegVals_t), doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowStopLaser) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowReadDiskBca(DVDDiskBca* diskBca, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xDA;
    rv =
        IOS_IoctlAsync(DiFD, 0xDA, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), diskBca, sizeof(DVDDiskBca), doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReadDiskBca) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowSerMeasControl(DVDLowDriveSer* ser, u8 clear, u8 enable, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xDF;
    diCommand[freeCommandBuf].arg[0] = clear;
    diCommand[freeCommandBuf].arg[1] = enable;
    rv =
        IOS_IoctlAsync(DiFD, 0xDF, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), ser, sizeof(DVDLowDriveSer), doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowSerMeasControl) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowRequestDiscStatus(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xDB;

    rv = IOS_IoctlAsync(DiFD, 0xDB, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache, sizeof(diRegVals_t), doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowRequestDiscStatus) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowRequestRetryNumber(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xDC;

    rv = IOS_IoctlAsync(DiFD, 0xDC, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache, sizeof(diRegVals_t), doTransactionCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowRequestRetryNumber) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowSetMaximumRotation(u32 subcmd, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xDD;
    diCommand[freeCommandBuf].arg[0] = (subcmd >> 16) & 3;

    rv = IOS_IoctlAsync(DiFD, 0xDD, &diCommand[freeCommandBuf], sizeof(diCommand_t), 0, 0, doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowSetMaxRotation) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowRead(void* destAddr, u32 length, u32 wordOffset, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    if (((u32)destAddr & 0x1F) != 0) {
        OSReport("(DVDLowRead): ERROR - destAddr buffer is not 32 byte aligned\n");
        return false;
    }

    requestInProgress = true;
    dvdContext = newContext(callback, 1);
    readLength = length;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x71;
    diCommand[freeCommandBuf].arg[0] = length;
    diCommand[freeCommandBuf].arg[1] = wordOffset;

    rv = IOS_IoctlAsync(DiFD, 0x71, &diCommand[freeCommandBuf], sizeof(diCommand_t), destAddr, length, doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowRead) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL DVDLowSeek(u32 wordOffset, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0xAB;
    diCommand[freeCommandBuf].arg[0] = wordOffset;

    rv = IOS_IoctlAsync(DiFD, 0xAB, &diCommand[freeCommandBuf], sizeof(diCommand_t), 0, 0, doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowSeek) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

u32 DVDLowGetCoverReg(void) {
    s32 rv;

    if (callbackInProgress == true) {
        OSReport("(DVDLowGetCoverReg): Synch functions can't be called in callbacks\n");
        return false;
    }

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x7A;

    rv = IOS_Ioctl(DiFD, 0x7A, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), coverRegister, sizeof(u32) * 8);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowGetCoverReg) IOS_Ioctl returned error: %d\n", rv);
        return false;
    }

    return coverRegister[0];
}

u32 DVDLowGetCoverRegister(void) {
    return diRegValCache.CoverRegVal;
}

BOOL DVDLowPrepareCoverRegister(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x7A;
    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    rv = IOS_IoctlAsync(DiFD, 0x7A, &diCommand[freeCommandBuf], sizeof(diCommand_t), registerBuf, sizeof(registerBuf), doPrepareCoverRegisterCallback,
                        dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowPrepareCoverRegsiter) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

u32 DVDLowGetImmBufferReg(void) {
    return diRegValCache.ImmRegVal;
}

BOOL DVDLowUnmaskStatusInterrupts(void) {
    return true;
}

BOOL DVDLowMaskCoverInterrupt(void) {
    return true;
}

BOOL DVDLowClearCoverInterrupt(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x86;
    requestInProgress = true;
    dvdContext = newContext(callback, 1);

    rv = IOS_IoctlAsync(DiFD, 0x86, &diCommand[freeCommandBuf], sizeof(diCommand_t), 0, 0, doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowClearCoverInterrupt) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}

BOOL __DVDLowTestAlarm(const OSAlarm*) {
    return FALSE;
}

BOOL DVDLowEnableDvdVideo(const u8 enable, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    s32 rv;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = 0x8E;
    diCommand[freeCommandBuf].arg[0] = enable;
    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    rv = IOS_IoctlAsync(DiFD, 0x8E, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), NULL, 0, doTransactionCallback, dvdContext);

    if (rv != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowEnableDvdVideo) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }

    return true;
}
