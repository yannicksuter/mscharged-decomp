#include <revolution/nand.h>

#include "NL/plat/nlFlash.h"

#include "NL/nlMemory.h"
#include "NL/nlString.h"

static char sTempDirectoryPath[] = "/tmp";
static char* sTempDirectory = sTempDirectoryPath;
static s32 sCallbackDelay = -1;
static char sNoCopyDirectorySuffix[] = "/nocopy";

static bool sInitialized;
static NANDResultCallback sResultCallback;
static s32 sAsyncResult;

static NANDCommandBlock sCommandBlock;
static NANDFileInfo sFileInfo;

FlashMemoryTask::FlashMemoryTask()
{
}

extern "C" void nlFlashAsyncCallback(s32 result, NANDCommandBlock*)
{
    sAsyncResult = result;
    sCallbackDelay = 1;
}

void FlashMemoryTask::Run(float)
{
    if (sCallbackDelay > 0)
    {
        --sCallbackDelay;
    }

    if (sCallbackDelay == 0)
    {
        sCallbackDelay = -1;
        NANDResultCallback callback = sResultCallback;
        if (callback != 0)
        {
            sResultCallback = 0;
            callback(sAsyncResult);
        }
    }
}

extern "C" void nlFlashInitialize()
{
    if (NANDInit() == NAND_RESULT_OK)
    {
        sInitialized = true;
        nlFlashChangeDirectory(0, 0);
    }
}

extern "C" s32 nlFlashChangeDirectory(
    s32 directory, NANDResultCallback callback)
{
    char current[64];
    char path[64] = { 0 };
    s32 result = NANDGetCurrentDir(current);
    if (result != NAND_RESULT_OK)
    {
        return result;
    }

    if (directory == 0)
    {
        result = NANDGetHomeDir(path);
        if (result != NAND_RESULT_OK)
        {
            return result;
        }
    }
    else if (directory == 1)
    {
        result = NANDGetHomeDir(path);
        nlStrNCat(path, path, sNoCopyDirectorySuffix, sizeof(path));
        if (result != NAND_RESULT_OK)
        {
            return result;
        }
    }
    else if (directory == 2)
    {
        nlStrNCpy(path, sTempDirectory, sizeof(path));
    }

    if (nlStrNCmp(current, path, sizeof(path)) == 0)
    {
        if (callback != 0)
        {
            callback(NAND_RESULT_OK);
        }
        return NAND_RESULT_OK;
    }

    if (callback != 0)
    {
        result = NANDChangeDirAsync(
            path, nlFlashAsyncCallback, &sCommandBlock);
        if (result == NAND_RESULT_OK)
        {
            sResultCallback = callback;
        }
        return result;
    }

    return NANDChangeDir(path);
}

extern "C" s32 nlFlashCreateDirectory(
    const char* name, u8 permissions, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDCreateDirAsync(name, permissions, 0,
            nlFlashAsyncCallback, &sCommandBlock);
        if (result == NAND_RESULT_OK)
        {
            sResultCallback = callback;
        }
    }
    else
    {
        result = NANDCreateDir(name, permissions, 0);
    }
    return result;
}

extern "C" s32 nlFlashCreate(
    const char* name, u8 permissions, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDCreateAsync(name, permissions, 0,
            nlFlashAsyncCallback, &sCommandBlock);
        if (result == NAND_RESULT_OK)
        {
            sResultCallback = callback;
        }
    }
    else
    {
        result = NANDCreate(name, permissions, 0);
    }
    return result;
}

extern "C" s32 nlFlashCheck(
    u32 blocks, u32 files, u32* answer, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDCheckAsync(blocks, files, answer,
            nlFlashAsyncCallback, &sCommandBlock);
        if (result == NAND_RESULT_OK)
        {
            sResultCallback = callback;
        }
    }
    else
    {
        result = NANDCheck(blocks, files, answer);
    }
    return result;
}

extern "C" s32 nlFlashDelete(
    const char* name, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDDeleteAsync(
            name, nlFlashAsyncCallback, &sCommandBlock);
        if (result == NAND_RESULT_OK)
        {
            sResultCallback = callback;
        }
    }
    else
    {
        result = NANDDelete(name);
    }
    return result;
}

extern "C" s32 nlFlashWrite(
    const void* data, u32 size, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDWriteAsync(&sFileInfo, data, size,
            nlFlashAsyncCallback, &sCommandBlock);
        if (result == NAND_RESULT_OK)
        {
            sResultCallback = callback;
        }
    }
    else
    {
        result = NANDWrite(&sFileInfo, data, size);
    }
    return result;
}

extern "C" s32 nlFlashGetLength(
    u32* size, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDGetLengthAsync(&sFileInfo, size,
            nlFlashAsyncCallback, &sCommandBlock);
        if (result == NAND_RESULT_OK)
        {
            sResultCallback = callback;
        }
    }
    else
    {
        result = NANDGetLength(&sFileInfo, size);
    }
    return result;
}

extern "C" s32 nlFlashRead(void** data, u32* size,
    NANDResultCallback callback, bool bufferProvided)
{
    s32 result;
    if (!bufferProvided)
    {
        result = NANDGetLength(&sFileInfo, size);
        if (result != NAND_RESULT_OK)
        {
            return result;
        }
        *size = (*size + 31) & ~31u;
        *data = nlMalloc(*size, 32, true);
    }

    if (callback != 0)
    {
        result = NANDReadAsync(&sFileInfo, *data, *size,
            nlFlashAsyncCallback, &sCommandBlock);
        if (result == NAND_RESULT_OK)
        {
            sResultCallback = callback;
        }
    }
    else
    {
        result = NANDRead(&sFileInfo, *data, *size);
    }
    return result;
}

extern "C" s32 nlFlashOpen(
    const char* name, u8 mode, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDOpenAsync(name, &sFileInfo, mode,
            nlFlashAsyncCallback, &sCommandBlock);
        if (result == NAND_RESULT_OK)
        {
            sResultCallback = callback;
        }
    }
    else
    {
        result = NANDOpen(name, &sFileInfo, mode);
    }
    return result;
}

extern "C" s32 nlFlashClose(NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDCloseAsync(
            &sFileInfo, nlFlashAsyncCallback, &sCommandBlock);
        if (result == NAND_RESULT_OK)
        {
            sResultCallback = callback;
        }
    }
    else
    {
        result = NANDClose(&sFileInfo);
    }
    return result;
}

extern "C" bool nlFlashCallbackPending()
{
    return sCallbackDelay != -1;
}
