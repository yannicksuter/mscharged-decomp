#include <revolution/dvd.h>
#include <revolution/esp.h>
#include <revolution/ipc.h>
#include <revolution/os.h>

#include <string.h>

#define DVD_LOW_CTX_MAX 4
#define DVD_LOW_CMD_MAX 4
#define DVD_LOW_CTX_MAGIC 0xFEEBDAED

// Your guess is as good as mine
#define DVD_LOW_CTX_IN_USE(ctx) ((u8)((ctx).inUse != FALSE))

typedef enum {
    DVD_IOCTL_INQUIRY = 0x12,
    DVD_IOCTL_READ_DISK_ID = 0x70,
    DVD_IOCTL_READ = 0x71,
    DVD_IOCTL_WAIT_FOR_COVER_CLOSE = 0x79,
    DVD_IOCTL_PREPARE_COVER_REGISTER = 0x7A,
    DVD_IOCTL_NOTIFY_RESET = 0x7E,
    DVD_IOCTL_READ_DVD_PHYSICAL = 0x80,
    DVD_IOCTL_READ_DVD_COPYRIGHT = 0x81,
    DVD_IOCTL_READ_DVD_DISC_KEY = 0x82,
    DVD_IOCTL_CLEAR_COVER_INTERRUPT = 0x86,
    DVD_IOCTL_GET_COVER_STATUS = 0x88,
    DVD_IOCTL_RESET = 0x8A,
    DVD_IOCTL_CLOSE_PARTITION = 0x8C,
    DVD_IOCTL_UNENCRYPTED_READ = 0x8D,
    DVD_IOCTL_ENABLE_DVD_VIDEO = 0x8E,
    DVD_IOCTL_REPORT_KEY = 0xA4,
    DVD_IOCTL_SEEK = 0xAB,
    DVD_IOCTL_READ_DVD = 0xD0,
    DVD_IOCTL_READ_DVD_CONFIG = 0xD1,
    DVD_IOCTL_STOP_LASER = 0xD2,
    DVD_IOCTL_OFFSET = 0xD9,
    DVD_IOCTL_READ_DISK_BCA = 0xDA,
    DVD_IOCTL_REQUEST_DISC_STATUS = 0xDB,
    DVD_IOCTL_REQUEST_RETRY_NUMBER = 0xDC,
    DVD_IOCTL_SET_MAX_ROTATION = 0xDD,
    DVD_IOCTL_SER_MEAS_CONTROL = 0xDF,
    DVD_IOCTL_REQUEST_ERROR = 0xE0,
    DVD_IOCTL_STOP_MOTOR = 0xE3,
    DVD_IOCTL_AUDIO_BUFFER_CONFIG = 0xE4,
} DVDLowIoctl;

typedef enum {
    DVD_IOCTLV_OPEN_PARTITION = 0x8B,
} DVDLowIoctlv;

typedef struct DVDLowContext {
    DVDLowCallback callback; // at 0x0
    UNKWORD WORD_0x4;
    u8 inUse;  // at 0x8
    u32 magic; // at 0xC
    u32 id;    // at 0x10
    char padding[32 - 0x14];
} DVDLowContext;

typedef struct DVDLowDICommand {
    u8 command; // at 0x0
    u32 arg1;   // at 0x4
    u32 arg2;   // at 0x8
    u32 arg3;   // at 0xC
    u32 arg4;   // at 0x10
    u32 arg5;   // at 0x14
    u32 arg6;   // at 0x18
    u32 arg7;   // at 0x1C
} DVDLowDICommand;

typedef struct DVDLowDiRegs {
    u32 diimmbuf; // at 0x0
    u32 dicvr;    // at 0x4
    char padding[32 - 0x8];
} DVDLowDiRegs;

typedef struct DVDLowRegBuffer {
    u32 reg; // at 0x0
    char padding[32 - 0x4];
} DVDLowRegBuffer;

typedef struct DVDVideoReportKey {
    u8 data[32];
} DVDVideoReportKey;

static s32 DiFD = -1;

static u8 breakRequested;
static DVDLowDICommand* diCommand;
static char* pathBuf;
static u32 readLength;
static BOOL spinUpValue;
static u8 DVDLowInitCalled;
static u8 dvdContextsInited;
static s32 freeDvdContext;
static s32 freeCommandBuf;
static u8 callbackInProgress;
static u8 requestInProgress;

static DVDLowContext dvdContexts[DVD_LOW_CTX_MAX] ALIGN(32);
static DVDLowDiRegs diRegValCache ALIGN(32);
static DVDLowRegBuffer registerBuf ALIGN(32);
static IPCIOVector ioVec[5] ALIGN(32);
static u8 lastTicketError[32] ALIGN(32);

static void nextCommandBuf(void);
static DVDLowContext* newContext(DVDLowCallback callback, UNKWORD arg2);

static void* ddrAllocAligned32(size_t size) {
    u8* lo = IPCGetBufferLo();
    u8* hi = IPCGetBufferHi();

    // @bug Incorrect rounding (& 31) results in pointer mod 32
    if ((u32)lo % 32 != 0) {
        lo = (u8*)(((u32)lo + 31) & 31);
    }

    if (lo + size > hi) {
        OSReport("(ddrAllocAligned32) Not enough space to allocate %d bytes\n",
                 size);
    }

    IPCSetBufferLo(lo + size);
    return lo;
}

static BOOL allocateStructures(void) {
    diCommand = ddrAllocAligned32(sizeof(DVDLowDICommand) * DVD_LOW_CMD_MAX);
    if (diCommand == NULL) {
        OSReport("Allocation of diCommand blocks failed\n");
        return FALSE;
    }

    pathBuf = ddrAllocAligned32(32);
    if (pathBuf == NULL) {
        OSReport("Allocation of pathBuf failed\n");
        return FALSE;
    }

    return TRUE;
}

/**
 * Emitted standalone and then dead-stripped: retail allocates dvdContexts at
 * this unit's first .bss offset, before doPrepareCoverRegisterCallback's
 * diRegValCache and registerBuf, which requires a surviving pre-callback
 * reference from this routine's own emission. A fully inlined static is
 * dropped before allocation and orders the section wrongly, so this helper
 * had external linkage; the call below is still inlined, and the link strips
 * this copy.
 */
void initDvdContexts(void) {
    int i;
    for (i = 0; i < DVD_LOW_CTX_MAX; i++) {
        dvdContexts[i].callback = NULL;
        dvdContexts[i].WORD_0x4 = 0;
        dvdContexts[i].inUse = FALSE;
        dvdContexts[i].magic = DVD_LOW_CTX_MAGIC;
        dvdContexts[i].id = i;
    }

    freeDvdContext = 0;
}

static s32 doTransactionCallback(s32 intType, void* arg) {
    DVDLowContext* ctx = (DVDLowContext*)arg;

    if (ctx->magic != DVD_LOW_CTX_MAGIC) {
        OSReport("(doTransactionCallback) Error - context mangled!\n");
        ctx->magic = DVD_LOW_CTX_MAGIC;
    } else {
        requestInProgress = FALSE;

        if (ctx->callback != NULL) {
            callbackInProgress = TRUE;

            if (breakRequested == TRUE) {
                breakRequested = FALSE;
                intType |= DVD_INTTYPE_BR;
            }

            if (intType & DVD_INTTYPE_TC) {
                readLength = 0;
            }

            ctx->callback(intType);

            callbackInProgress = FALSE;
        }
    }

    ctx->inUse = FALSE;
    return IPC_RESULT_OK;
}

/**
 * Cover-close completion callback.
 *
 * R4QE01 never calls DVDLowWaitForCoverClose, so the link drops both that
 * routine and this callback. Their messages still occupy the pooled string
 * block, which is what places every later string at the offset the retained
 * DVDLow* routines reference.
 */
static s32 doCoverCallback(s32 intType, void* arg) {
    DVDLowContext* ctx = (DVDLowContext*)arg;

    requestInProgress = FALSE;

    if (ctx->magic != DVD_LOW_CTX_MAGIC) {
        OSReport("(doCoverCallback) Error - context mangled!\n");
        ctx->magic = DVD_LOW_CTX_MAGIC;
    } else {
        if (ctx->callback != NULL) {
            callbackInProgress = TRUE;

            if (breakRequested == TRUE) {
                breakRequested = FALSE;
                intType |= DVD_INTTYPE_BR;
            }

            ctx->callback(intType);

            callbackInProgress = FALSE;
        }
    }

    ctx->inUse = FALSE;
    return IPC_RESULT_OK;
}

static s32 doPrepareCoverRegisterCallback(s32 intType, void* arg) {
    DVDLowContext* ctx = (DVDLowContext*)arg;

    requestInProgress = FALSE;
    diRegValCache.dicvr = registerBuf.reg;

    if (ctx->magic != DVD_LOW_CTX_MAGIC) {
        OSReport("(doTransactionCallback) Error - context mangled!\n");
        ctx->magic = DVD_LOW_CTX_MAGIC;
    } else {
        if (ctx->callback != NULL) {
            callbackInProgress = TRUE;

            if (breakRequested == TRUE) {
                breakRequested = FALSE;
                intType |= DVD_INTTYPE_BR;
            }

            ctx->callback(intType);

            callbackInProgress = FALSE;
        }
    }

    ctx->inUse = FALSE;
    return IPC_RESULT_OK;
}

BOOL DVDLowInit(void) {
    IPCResult result;

    if (!DVDLowInitCalled) {
        DVDLowInitCalled = TRUE;

        result = IPCCltInit();
        if (result != IPC_RESULT_OK) {
            OSReport("IPCCltInit returned error: %d\n", result);
            return FALSE;
        }

        if (!allocateStructures()) {
            return FALSE;
        }

        if (!dvdContextsInited) {
            initDvdContexts();
            dvdContextsInited = TRUE;
        }
    }

    strncpy(pathBuf, "/dev/di", 32);
    DiFD = IOS_Open(pathBuf, IPC_OPEN_NONE);

    if (DiFD >= 0) {
        return TRUE;
    }

    switch (DiFD) {
    case IPC_RESULT_NOEXISTS_INTERNAL:
        OSReport("(DVDLowInit) Error: IOS_Open failed - pathname '/dev/di' "
                 "does not exist\n");
        return FALSE;
    case IPC_RESULT_ACCESS_INTERNAL:
        OSReport("(DVDLowInit) Error: IOS_Open failed - calling thread lacks "
                 "permission\n");
        return FALSE;
    case IPC_RESULT_CONN_MAX_INTERNAL:
        OSReport("(DVDLowInit) Error: IOS_Open failed - connection limit has "
                 "been reached\n");
        return FALSE;
    default:
        OSReport("(DVDLowInit) IOS_Open failed, errorcode = %d\n", DiFD);
        return FALSE;
    }
}

BOOL DVDLowReadDiskID(DVDDiskID* out, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    if (out == NULL) {
        OSReport("@@@@@@ WARNING - Calling DVDLowReadDiskId with NULL ptr\n");
    }

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_READ_DISK_ID;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_READ_DISK_ID,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            out, sizeof(DVDDiskID), doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReadDiskID) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

static void nextCommandBuf(void) {
    if (++freeCommandBuf >= DVD_LOW_CMD_MAX) {
        freeCommandBuf = 0;
    }
}

static DECOMP_INLINE DVDLowContext* newContext(DVDLowCallback callback,
                                               UNKWORD arg2) {
    s32 id;

    // The last operation somehow did not complete
    if (DVD_LOW_CTX_IN_USE(dvdContexts[freeDvdContext]) == TRUE) {
        OSReport("(newContext) ERROR: freeDvdContext.inUse (#%d) is true\n",
                 freeDvdContext);
        OSReport("(newContext) Now spinning in infinite loop\n");

        while (TRUE) {
            ;
        }
    }

    // Memory corruption
    if (dvdContexts[freeDvdContext].magic != DVD_LOW_CTX_MAGIC) {
        OSReport(
            "(newContext) Something overwrote the context magic - spinning \n");

        while (TRUE) {
            ;
        }
    }

    dvdContexts[freeDvdContext].callback = callback;
    dvdContexts[freeDvdContext].WORD_0x4 = arg2;
    dvdContexts[freeDvdContext].inUse = TRUE;

    id = freeDvdContext;

    if (++freeDvdContext >= DVD_LOW_CTX_MAX) {
        freeDvdContext = 0;
    }

    return &dvdContexts[id];
}

BOOL DVDLowOpenPartition(u32 offset, const ESPTicket* ticket, u32 certsSize,
                         const void* certs, ESPTmd* tmd,
                         DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    if (ticket != NULL && (u32)ticket % 32 != 0) {
        OSReport("(DVDLowOpenPartition) eTicket memory is unaligned\n");
        return FALSE;
    }

    if (certs != NULL && (u32)certs % 32 != 0) {
        OSReport("(DVDLowOpenPartition) certificates memory is unaligned\n");
        return FALSE;
    }

    if (tmd != NULL && (u32)tmd % 32 != 0) {
        OSReport("(DVDLowOpenPartition) certificates memory is unaligned\n");
        return FALSE;
    }

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTLV_OPEN_PARTITION;
    diCommand[freeCommandBuf].arg1 = offset;

    // Input vector 1: DI command
    ioVec[0].base = &diCommand[freeCommandBuf];
    ioVec[0].length = sizeof(DVDLowDICommand);

    // Input vector 2: eTicket
    ioVec[1].base = (void*)ticket;
    if (ticket == NULL) {
        ioVec[1].length = 0;
    } else {
        ioVec[1].length = sizeof(ESPTicket);
    }

    // Input vector 3: Shared certs
    ioVec[2].base = (void*)certs;
    if (certs == NULL) {
        ioVec[2].length = 0;
    } else {
        ioVec[2].length = certsSize;
    }

    // Output vector 1: TMD
    ioVec[3].base = tmd;
    ioVec[3].length = sizeof(ESPTmd);

    // Output vector 2: Ticket error
    ioVec[4].base = &lastTicketError;
    ioVec[4].length = sizeof(lastTicketError);

    result = IOS_IoctlvAsync(DiFD, DVD_IOCTLV_OPEN_PARTITION, 3, 2, ioVec,
                             doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowOpenPartition) IOS_IoctlvAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowClosePartition(DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_CLOSE_PARTITION;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_CLOSE_PARTITION,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            NULL, 0, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowClosePartition) IOS_IoctlAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowUnencryptedRead(void* dst, u32 size, u32 offset,
                           DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    readLength = size;

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_UNENCRYPTED_READ;
    diCommand[freeCommandBuf].arg1 = size;
    diCommand[freeCommandBuf].arg2 = offset;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_UNENCRYPTED_READ,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            dst, size, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowUnencryptedRead) IOS_IoctlAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowStopMotor(BOOL eject, BOOL kill, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_STOP_MOTOR;
    diCommand[freeCommandBuf].arg1 = eject;
    diCommand[freeCommandBuf].arg2 = kill;

    result =
        IOS_IoctlAsync(DiFD, DVD_IOCTL_STOP_MOTOR, &diCommand[freeCommandBuf],
                       sizeof(DVDLowDICommand), &diRegValCache,
                       sizeof(DVDLowDiRegs), doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowStopMotor) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowWaitForCoverClose(DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_WAIT_FOR_COVER_CLOSE;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_WAIT_FOR_COVER_CLOSE,
                            &diCommand[freeCommandBuf],
                            sizeof(DVDLowDICommand), NULL, 0, doCoverCallback,
                            ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowWaitForCoverClose) IOS_IoctlAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowInquiry(DVDDriveInfo* out, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_INQUIRY;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_INQUIRY, &diCommand[freeCommandBuf],
                            sizeof(DVDLowDICommand), out, sizeof(DVDDriveInfo),
                            doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowInquiry) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowRequestError(DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_REQUEST_ERROR;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_REQUEST_ERROR,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            &diRegValCache, sizeof(DVDLowDiRegs),
                            doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowRequestError) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowSetSpinupFlag(BOOL enable) {
    spinUpValue = enable;
    return TRUE;
}

/*
 * The DI routines below are dead code in R4QE01: nothing references them, so
 * the link strips their text, exactly as it strips DVDLowWaitForCoverClose
 * and doCoverCallback above. Their OSReport messages still enter this unit's
 * pooled string block in source order, which is what places every later
 * string at the offset the retained routines reference. The DOL evidences
 * each routine's name (each message names its owner), its pool position, and
 * the stripping itself; the stripped bodies cannot be byte-verified, so they
 * are reconstructed minimally from this file's own wrapper template.
 */

/**
 * Owner of the pooled spin-up-flag guard message.
 *
 * The retained 12-byte DVDLowSetSpinupFlag above never references this
 * string, and every known link (including the IPL-era SDK, whose pool shows
 * the same orphaned message beside the same 12-byte setter) strips the owner,
 * so the routine's original name is unrecoverable; this identifier is a
 * reconstruction placeholder. The synchronous-guard shape follows this file's
 * other synchronous routines.
 */
BOOL DVDLowSetSpinupFlagSync(BOOL enable) {
    if (callbackInProgress == TRUE) {
        OSReport("(DVDLowSetSpinupFlag): Synch functions can't be called in "
                 "callbacks\n");
        return FALSE;
    }

    spinUpValue = enable;
    return TRUE;
}

BOOL DVDLowNotifyReset(DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_NOTIFY_RESET;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_NOTIFY_RESET,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            NULL, 0, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowNotifyReset) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowReset(DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_RESET;
    diCommand[freeCommandBuf].arg1 = spinUpValue;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_RESET, &diCommand[freeCommandBuf],
                            sizeof(DVDLowDICommand), NULL, 0,
                            doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReset) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowAudioBufferConfig(BOOL enable, u32 size, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_AUDIO_BUFFER_CONFIG;
    diCommand[freeCommandBuf].arg1 = enable;
    diCommand[freeCommandBuf].arg2 = size;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_AUDIO_BUFFER_CONFIG,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            &diRegValCache, sizeof(DVDLowDiRegs),
                            doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowAudioBufferConfig) IOS_IoctlAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

/**
 * Synchronous cover-status query. Dead-stripped by the link; see the note
 * above DVDLowSetSpinupFlagSync. The synchronous shape follows the rhf
 * reference reconstruction of this file.
 */
u32 DVDLowGetCoverStatus(void) {
    u32 status;
    IPCResult result;

    if (callbackInProgress == TRUE) {
        OSReport("(DVDLowGetCoverStatus): Synch functions can't be called in "
                 "callbacks\n");
        return 0;
    }

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_GET_COVER_STATUS;

    result = IOS_Ioctl(DiFD, DVD_IOCTL_GET_COVER_STATUS,
                       &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                       &registerBuf, sizeof(DVDLowRegBuffer));
    status = registerBuf.reg;

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowGetCoverStatus) IOS_Ioctl returned error: %d\n",
                 result);
        status = 0xDEADDEAD;
    }

    return status;
}

BOOL DVDLowReadDVD(void* dst, u32 size, u32 offset, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_READ_DVD;
    diCommand[freeCommandBuf].arg1 = size;
    diCommand[freeCommandBuf].arg2 = offset;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_READ_DVD,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            dst, size, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReadDVD) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowReadDVDConfig(void* out, u32 size, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_READ_DVD_CONFIG;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_READ_DVD_CONFIG,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            out, size, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowReadDVDConfig) IOS_IoctlAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowReadDvdCopyright(void* out, u32 size, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_READ_DVD_COPYRIGHT;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_READ_DVD_COPYRIGHT,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            out, size, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowReadDvdCopyright) IOS_IoctlAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowReadDvdPhysical(void* out, u32 size, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_READ_DVD_PHYSICAL;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_READ_DVD_PHYSICAL,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            out, size, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowReadDvdPhysical) IOS_IoctlAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowReadDvdDiscKey(void* out, u32 size, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_READ_DVD_DISC_KEY;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_READ_DVD_DISC_KEY,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            out, size, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowReadDvdDiscKey) IOS_IoctlAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

/**
 * Dead-stripped by the link. The signature follows the rhf and xenoblade
 * reference reconstructions of this file.
 */
BOOL DVDLowReportKey(DVDVideoReportKey* reportKey, u32 format, u32 lsn,
                     DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_REPORT_KEY;
    diCommand[freeCommandBuf].arg1 = format >> 16;
    diCommand[freeCommandBuf].arg2 = lsn;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_REPORT_KEY,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            reportKey, sizeof(DVDVideoReportKey),
                            doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReportKey) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowOffset(u32 offset, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_OFFSET;
    diCommand[freeCommandBuf].arg1 = offset;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_OFFSET, &diCommand[freeCommandBuf],
                            sizeof(DVDLowDICommand), NULL, 0,
                            doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowOffset) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowStopLaser(DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_STOP_LASER;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_STOP_LASER,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            NULL, 0, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowStopLaser) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowReadDiskBca(void* out, u32 size, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_READ_DISK_BCA;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_READ_DISK_BCA,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            out, size, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowReadDiskBca) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowSerMeasControl(DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_SER_MEAS_CONTROL;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_SER_MEAS_CONTROL,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            NULL, 0, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowSerMeasControl) IOS_IoctlAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowRequestDiscStatus(DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_REQUEST_DISC_STATUS;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_REQUEST_DISC_STATUS,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            NULL, 0, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowRequestDiscStatus) IOS_IoctlAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowRequestRetryNumber(DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_REQUEST_RETRY_NUMBER;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_REQUEST_RETRY_NUMBER,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            NULL, 0, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowRequestRetryNumber) IOS_IoctlAsync returned "
                 "error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowSetMaximumRotation(u32 speed, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_SET_MAX_ROTATION;
    diCommand[freeCommandBuf].arg1 = speed >> 16 & 3;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_SET_MAX_ROTATION,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            NULL, 0, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowSetMaxRotation) IOS_IoctlAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowRead(void* dst, u32 size, u32 offset, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    if ((u32)dst % 32 != 0) {
        OSReport(
            "(DVDLowRead): ERROR - destAddr buffer is not 32 byte aligned\n");
        return FALSE;
    }

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    readLength = size;

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_READ;
    diCommand[freeCommandBuf].arg1 = size;
    diCommand[freeCommandBuf].arg2 = offset;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_READ, &diCommand[freeCommandBuf],
                            sizeof(DVDLowDICommand), dst, size,
                            doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowRead) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL DVDLowSeek(u32 offset, DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_SEEK;
    diCommand[freeCommandBuf].arg1 = offset;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_SEEK, &diCommand[freeCommandBuf],
                            sizeof(DVDLowDICommand), NULL, 0,
                            doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowSeek) IOS_IoctlAsync returned error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

/**
 * Synchronous cover-register query. Dead-stripped by the link; see the note
 * above DVDLowSetSpinupFlagSync. The synchronous shape follows the rhf
 * reference reconstruction of this file.
 */
u32 DVDLowGetCoverReg(void) {
    u32 val;
    IPCResult result;

    if (callbackInProgress == TRUE) {
        OSReport("(DVDLowGetCoverReg): Synch functions can't be called in "
                 "callbacks\n");
        return 0;
    }

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_PREPARE_COVER_REGISTER;

    result = IOS_Ioctl(DiFD, DVD_IOCTL_PREPARE_COVER_REGISTER,
                       &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                       &registerBuf, sizeof(DVDLowRegBuffer));
    val = registerBuf.reg;

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowGetCoverReg) IOS_Ioctl returned error: %d\n",
                 result);
        val = 0;
    }

    return val;
}

u32 DVDLowGetCoverRegister(void) {
    return diRegValCache.dicvr;
}

BOOL DVDLowPrepareCoverRegister(DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_PREPARE_COVER_REGISTER;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_PREPARE_COVER_REGISTER,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            &registerBuf, sizeof(DVDLowRegBuffer),
                            doPrepareCoverRegisterCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowPrepareCoverRegsiter) IOS_IoctlAsync returned "
                 "error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

u32 DVDLowGetImmBufferReg(void) {
    return diRegValCache.diimmbuf;
}

BOOL DVDLowUnmaskStatusInterrupts(void) {
    return TRUE;
}

BOOL DVDLowMaskCoverInterrupt(void) {
    return TRUE;
}

BOOL DVDLowClearCoverInterrupt(DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_CLEAR_COVER_INTERRUPT;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_CLEAR_COVER_INTERRUPT,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            NULL, 0, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport("@@@ (DVDLowClearCoverInterrupt) IOS_IoctlAsync returned "
                 "error: %d\n",
                 result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}

BOOL __DVDLowTestAlarm(const OSAlarm* alarm) {
#pragma unused(alarm)

    return FALSE;
}

/**
 * Dead-stripped by the link; see the note above DVDLowSetSpinupFlagSync.
 */
BOOL DVDLowEnableDvdVideo(DVDLowCallback callback) {
    DVDLowContext* ctx;
    IPCResult result;

    requestInProgress = TRUE;
    ctx = newContext(callback, 1);

    nextCommandBuf();
    diCommand[freeCommandBuf].command = DVD_IOCTL_ENABLE_DVD_VIDEO;

    result = IOS_IoctlAsync(DiFD, DVD_IOCTL_ENABLE_DVD_VIDEO,
                            &diCommand[freeCommandBuf], sizeof(DVDLowDICommand),
                            NULL, 0, doTransactionCallback, ctx);

    if (result != IPC_RESULT_OK) {
        OSReport(
            "@@@ (DVDLowEnableDvdVideo) IOS_IoctlAsync returned error: %d\n",
            result);
        ctx->inUse = FALSE;

        return FALSE;
    }

    return TRUE;
}
