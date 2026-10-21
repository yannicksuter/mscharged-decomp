#include <revolution/esp.h>
#include <revolution/nand.h>
#include <revolution/nand/NANDLogging.h>
#include <revolution/os.h>

#include <stdio.h>
#include <string.h>

typedef enum {
    NAND_LIB_UNINITIALIZED,
    NAND_LIB_INITIALIZING,
    NAND_LIB_INITIALIZED
} NANDLibState;

static void nandShutdownCallback(s32 result, void* arg);
static void nandChangeDirCallback(s32 result, void* arg);
static void nandGetTypeCallback(s32 result, void* arg);
static BOOL nandOnShutdown(BOOL final, u32 event);

const char* __NANDVersion =
    "<< RVL_SDK - NAND \trelease build: Feb 27 2009 10:05:16 (0x4302_145) >>";

static NANDLibState s_libState = NAND_LIB_UNINITIALIZED;
static char s_currentDir[64] ALIGN(32) = "/";
static OSShutdownFunctionInfo s_shutdownFuncInfo = {nandOnShutdown, 255};

static char s_homeDir[64] ALIGN(32);

void nandRemoveTailToken(char* newp, const char* oldp) {
    int i;

    if (oldp[0] == '/' && oldp[1] == '\0') {
        newp[0] = '/';
        newp[1] = '\0';
    } else {
        for (i = strlen(oldp) - 1; i >= 0; i--) {
            if (oldp[i] == '/') {
                if (i != 0) {
                    strncpy(newp, oldp, i);
                    newp[i] = '\0';
                    break;
                } else {
                    newp[0] = '/';
                    newp[1] = '\0';
                    break;
                }
            }
        }
    }
}

void nandGetHeadToken(char* head, char* rest, const char* path) {
    int i;

    for (i = 0; i <= strlen(path); i++) {
        if (path[i] == '/') {
            strncpy(head, path, i);
            head[i] = '\0';

            if (path[i + 1] == '\0') {
                *rest = '\0';
            } else {
                strcpy(rest, path + i + 1);
            }
            break;
        } else if (path[i] == '\0') {
            strncpy(head, path, i);
            head[i] = '\0';
            *rest = '\0';
            break;
        }
    }
}

void nandGetRelativeName(char* name, const char* path) {
    int i;

    if (strcmp("/", path) == 0) {
        strcpy(name, "");
    } else {
        for (i = strlen(path) - 1; i >= 0; i--) {
            if (path[i] == '/') {
                break;
            }
        }

        strcpy(name, path + i + 1);
    }
}

void nandConvertPath(char* abs, const char* dir, const char* rel) {
    char head[128];
    char rest[128];
    char notail[128];
    char newdir[128];

    if (strlen(rel) == 0) {
        strcpy(abs, dir);
        return;
    }

    nandGetHeadToken(head, rest, rel);

    if (strcmp(head, ".") == 0) {
        nandConvertPath(abs, dir, rest);
    } else if (strcmp(head, "..") == 0) {
        nandRemoveTailToken(notail, dir);
        nandConvertPath(abs, notail, rest);
    } else if (head[0] != '\0') {
        if (strcmp(dir, "/") == 0) {
            sprintf(newdir, "/%s", head);
        } else {
            sprintf(newdir, "%s/%s", dir, head);
        }
        nandConvertPath(abs, newdir, rest);
    } else {
        strcpy(abs, dir);
    }
}

BOOL nandIsRelativePath(const char* path) {
    return *path == '/' ? FALSE : TRUE;
}

BOOL nandIsPrivatePath(const char* path) {
    size_t len = sizeof("/shared2") - 1;
    if (strncmp(path, "/shared2", len) == 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

BOOL nandIsUnderPrivatePath(const char* path) {
    size_t len = sizeof("/shared2/") - 1;

    if (strncmp(path, "/shared2/", len) == 0 && path[len] != '\0') {
        return TRUE;
    } else {
        return FALSE;
    }
}

BOOL nandIsInitialized(void) {
    return s_libState == NAND_LIB_INITIALIZED ? TRUE : FALSE;
}

// Error descriptions are disabled in release builds.
void nandReportErrorCode(s32 result) {
    switch (result) {
    case -23:
    case IPC_RESULT_ALLOC_FAILED:
    case -21:
    case -20:
    case -19:
    case -18:
    case -17:
    case -16:
    case -15:
    case -14:
    case -13:
    case IPC_RESULT_ECC_CRIT_INTERNAL:
    case -11:
    case -10:
    case -9:
    case IPC_RESULT_BUSY_INTERNAL:
    case -7:
    case IPC_RESULT_NOEXISTS_INTERNAL:
    case IPC_RESULT_CONN_MAX_INTERNAL:
    case IPC_RESULT_INVALID_INTERNAL:
    case -3:
    case IPC_RESULT_EXISTS_INTERNAL:
    case IPC_RESULT_ACCESS_INTERNAL:
#ifndef NDEBUG
        OSReport("IPC error code: %d\n", result);
#endif
        break;
    case IPC_RESULT_FATAL_ERROR:
    case IPC_RESULT_BUSY:
    case -117:
    case -116:
    case IPC_RESULT_NOTEMPTY:
    case IPC_RESULT_ECC_CRIT:
    case -113:
    case -112:
    case IPC_RESULT_OPENFD:
    case -110:
    case IPC_RESULT_MAXFD:
    case IPC_RESULT_MAXBLOCKS:
    case IPC_RESULT_MAXFILES:
    case IPC_RESULT_NOEXISTS:
    case IPC_RESULT_EXISTS:
    case -104:
    case IPC_RESULT_CORRUPT:
    case IPC_RESULT_ACCESS:
    case IPC_RESULT_INVALID:
#ifndef NDEBUG
        OSReport("ISFS error code: %d\n", result);
#endif
        break;
    default:
#ifndef NDEBUG
        OSReport("Unknown error code: %d\n", result);
#endif
        break;
    }
}

s32 nandConvertErrorCode(s32 result) {
    int i;

    const s32 errorMap[] = {
        IPC_RESULT_OK, NAND_RESULT_OK,
        IPC_RESULT_ACCESS, NAND_RESULT_ACCESS,
        IPC_RESULT_CORRUPT, NAND_RESULT_CORRUPT,
        IPC_RESULT_ECC_CRIT, NAND_RESULT_ECC_CRIT,
        IPC_RESULT_EXISTS, NAND_RESULT_EXISTS,
        -116, NAND_RESULT_AUTHENTICATION,
        IPC_RESULT_INVALID, NAND_RESULT_INVALID,
        IPC_RESULT_MAXBLOCKS, NAND_RESULT_MAXBLOCKS,
        IPC_RESULT_MAXFD, NAND_RESULT_MAXFD,
        IPC_RESULT_MAXFILES, NAND_RESULT_MAXFILES,
        -110, NAND_RESULT_MAXDEPTH,
        IPC_RESULT_NOEXISTS, NAND_RESULT_NOEXISTS,
        IPC_RESULT_NOTEMPTY, NAND_RESULT_NOTEMPTY,
        -104, NAND_RESULT_UNKNOWN,
        IPC_RESULT_OPENFD, NAND_RESULT_OPENFD,
        -117, NAND_RESULT_UNKNOWN,
        IPC_RESULT_BUSY, NAND_RESULT_BUSY,
        IPC_RESULT_FATAL_ERROR, NAND_RESULT_FATAL_ERROR,
        IPC_RESULT_ACCESS_INTERNAL, NAND_RESULT_ACCESS,
        IPC_RESULT_EXISTS_INTERNAL, NAND_RESULT_EXISTS,
        -3, NAND_RESULT_UNKNOWN,
        IPC_RESULT_INVALID_INTERNAL, NAND_RESULT_INVALID,
        -5, NAND_RESULT_UNKNOWN,
        IPC_RESULT_NOEXISTS_INTERNAL, NAND_RESULT_NOEXISTS,
        -7, NAND_RESULT_UNKNOWN,
        IPC_RESULT_BUSY_INTERNAL, NAND_RESULT_BUSY,
        -9, NAND_RESULT_UNKNOWN,
        -10, NAND_RESULT_UNKNOWN,
        -11, NAND_RESULT_UNKNOWN,
        IPC_RESULT_ECC_CRIT_INTERNAL, NAND_RESULT_ECC_CRIT,
        -13, NAND_RESULT_UNKNOWN,
        -14, NAND_RESULT_UNKNOWN,
        -15, NAND_RESULT_UNKNOWN,
        -16, NAND_RESULT_UNKNOWN,
        -17, NAND_RESULT_UNKNOWN,
        -18, NAND_RESULT_UNKNOWN,
        -19, NAND_RESULT_UNKNOWN,
        -20, NAND_RESULT_UNKNOWN,
        -21, NAND_RESULT_UNKNOWN,
        IPC_RESULT_ALLOC_FAILED, NAND_RESULT_ALLOC_FAILED,
        -23, NAND_RESULT_UNKNOWN,
    };

    i = 0;

    if (result >= 0) {
        return result;
    }

    for (; i < ARRAY_SIZE(errorMap); i += 2) {
        if (result == errorMap[i]) {
            if (result == IPC_RESULT_ECC_CRIT || result == -116 || result == -117 ||
                result == -9 || result == IPC_RESULT_ECC_CRIT_INTERNAL) {
                char buf[128] ALIGN(64);
                sprintf(buf, "ISFS error code: %d", result);
                NANDLoggingAddMessageAsync(NULL, buf);
            }
            nandReportErrorCode(result);
            return errorMap[i + 1];
        }
    }

    OSReport("CAUTION!  Unexpected error code [%d] was found.\n", result);
    {
        char buf[128] ALIGN(64);
        sprintf(buf, "ISFS unexpected error code: %d", result);
        NANDLoggingAddMessageAsync(NULL, buf);
    }
    nandReportErrorCode(result);
    return NAND_RESULT_UNKNOWN;
}

void nandGenerateAbsPath(char* abs, const char* rel) {
    s32 len;

    if (strlen(rel) == 0) {
        strcpy(abs, "");
    } else if (nandIsRelativePath(rel)) {
        nandConvertPath(abs, s_currentDir, rel);
    } else {
        strcpy(abs, rel);
        len = strlen(abs);
        if (len != 0 && abs[len - 1] == '/' && len - 1 != 0) {
            abs[len - 1] = '\0';
        }
    }
}

void nandGetParentDirectory(char* dir, const char* path) {
    int i;

    for (i = strlen(path); i >= 0; i--) {
        if (path[i] == '/') {
            break;
        }
    }

    if (i == 0) {
        strcpy(dir, "/");
    } else {
        strncpy(dir, path, i);
        dir[i] = '\0';
    }
}

s32 NANDInit(void) {
    s32 result;
    u64 tid;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if (s_libState == NAND_LIB_INITIALIZING) {
        OSRestoreInterrupts(enabled);
        return NAND_RESULT_BUSY;
    } else if (s_libState == NAND_LIB_INITIALIZED) {
        OSRestoreInterrupts(enabled);
        return NAND_RESULT_OK;
    }

    s_libState = NAND_LIB_INITIALIZING;
    OSRestoreInterrupts(enabled);

    result = ISFS_OpenLib();
    if (result == IPC_RESULT_OK) {

        result = ESP_InitLib();

        if (result == IPC_RESULT_OK) {
            result = ESP_GetTitleId(&tid);
        }

        if (result == IPC_RESULT_OK) {
            result = ESP_GetDataDir(tid, s_homeDir);
        }

        if (result == IPC_RESULT_OK) {
            strcpy(s_currentDir, s_homeDir);
        }

        ESP_CloseLib();

        if (result != IPC_RESULT_OK) {
            OSReport("Failed to set home directory.\n");
        }

        OSRegisterShutdownFunction(&s_shutdownFuncInfo);
        enabled = OSDisableInterrupts();
        s_libState = NAND_LIB_INITIALIZED;
        OSRestoreInterrupts(enabled);
        OSRegisterVersion(__NANDVersion);
        return NAND_RESULT_OK;
    } else {
        enabled = OSDisableInterrupts();
        s_libState = NAND_LIB_UNINITIALIZED;
        OSRestoreInterrupts(enabled);
        return nandConvertErrorCode(result);
    }
}

static BOOL nandOnShutdown(BOOL final, u32 event) {
    if (!final) {
        if (event == OS_SD_EVENT_SHUTDOWN) {
            volatile BOOL shutdown = FALSE;
            s64 start = OSGetTime();
            ISFS_ShutdownAsync(nandShutdownCallback, (void*)&shutdown);

            while (OS_TICKS_TO_MSEC(OSGetTime() - start) < 500) {
                if (shutdown) {
                    break;
                }
            }
        }

        return TRUE;
    }

    return TRUE;
}

static void nandShutdownCallback(s32 result, void* arg) {
    *(BOOL*)arg = TRUE;
}

static s32 nandChangeDir(const char* path, NANDCommandBlock* block, BOOL isAsync,
                         BOOL hasPrivateAccess) {
    if (isAsync) {
        nandGenerateAbsPath(block->path, path);
        if (!hasPrivateAccess && nandIsPrivatePath(block->path)) {
            return IPC_RESULT_ACCESS;
        } else {
            return ISFS_ReadDirAsync(block->path, NULL, &block->dirFileCount,
                                     nandChangeDirCallback, block);
        }
    } else {
        u32 num = 0;
        char absPath[NAND_MAX_PATH] = "";

        nandGenerateAbsPath(absPath, path);
        if (!hasPrivateAccess && nandIsPrivatePath(absPath)) {
            return IPC_RESULT_ACCESS;
        } else {
            s32 err = ISFS_ReadDir(absPath, NULL, &num);
            if (err == IPC_RESULT_OK) {
                BOOL enabled = OSDisableInterrupts();
                strcpy(s_currentDir, absPath);
                OSRestoreInterrupts(enabled);
            }
            return err;
        }
    }
}

s32 NANDChangeDir(const char* path) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandChangeDir(path, NULL, FALSE, FALSE));
}

s32 NANDChangeDirAsync(const char* path, NANDAsyncCallback callback,
                       NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    return nandConvertErrorCode(nandChangeDir(path, block, TRUE, FALSE));
}

static void nandChangeDirCallback(s32 result, void* arg) {
    NANDCommandBlock* block = (NANDCommandBlock*)arg;
    if (result == IPC_RESULT_OK) {
        BOOL enabled = OSDisableInterrupts();
        strcpy(s_currentDir, block->path);
        OSRestoreInterrupts(enabled);
    }

    block->callback(nandConvertErrorCode(result), block);
}

s32 NANDGetCurrentDir(char* out) {
    BOOL enabled;

    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    enabled = OSDisableInterrupts();
    strcpy(out, s_currentDir);
    OSRestoreInterrupts(enabled);
    return NAND_RESULT_OK;
}

s32 NANDGetHomeDir(char* out) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    strcpy(out, s_homeDir);
    return NAND_RESULT_OK;
}

void nandCallback(s32 result, void* arg) {
    NANDCommandBlock* block = (NANDCommandBlock*)arg;
    block->callback(nandConvertErrorCode(result), block);
}

static s32 nandGetType(const char* path, u8* type, NANDCommandBlock* block,
                       BOOL isAsync, BOOL hasPrivateAccess) {
    if (strlen(path) == 0) {
        return IPC_RESULT_INVALID;
    }

    if (isAsync) {
        nandGenerateAbsPath(block->path, path);

        if (!hasPrivateAccess && nandIsUnderPrivatePath(block->path)) {
            return IPC_RESULT_ACCESS;
        }

        block->type = type;
        return ISFS_ReadDirAsync(block->path, NULL, &block->dirFileCount,
                                 nandGetTypeCallback, block);
    } else {
        char absPath[NAND_MAX_PATH] = "";

        nandGenerateAbsPath(absPath, path);

        if (!hasPrivateAccess && nandIsUnderPrivatePath(absPath)) {
            return IPC_RESULT_ACCESS;
        } else {
            u32 dummy = 0;
            s32 err = ISFS_ReadDir(absPath, NULL, &dummy);

            if (err == IPC_RESULT_OK || err == IPC_RESULT_ACCESS) {
                *type = NAND_FILE_TYPE_DIR;
                err = IPC_RESULT_OK;
            } else if (err == IPC_RESULT_INVALID) {
                *type = NAND_FILE_TYPE_FILE;
                err = IPC_RESULT_OK;
            }

            return err;
        }
    }
}

s32 NANDGetType(const char* path, u8* type) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandGetType(path, type, NULL, FALSE, FALSE));
}

s32 NANDPrivateGetTypeAsync(const char* path, u8* type,
                            NANDAsyncCallback callback,
                            NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    return nandConvertErrorCode(nandGetType(path, type, block, TRUE, TRUE));
}

static void nandGetTypeCallback(s32 result, void* arg) {
    NANDCommandBlock* block = (NANDCommandBlock*)arg;

    if (result == IPC_RESULT_OK || result == IPC_RESULT_ACCESS) {
        *block->type = NAND_FILE_TYPE_DIR;
        result = IPC_RESULT_OK;
    } else if (result == IPC_RESULT_INVALID) {
        *block->type = NAND_FILE_TYPE_FILE;
        result = IPC_RESULT_OK;
    }

    block->callback(nandConvertErrorCode(result), block);
}

const char* nandGetHomeDir(void) {
    return s_homeDir;
}
