#include <revolution/nand.h>
#include <revolution/os.h>
#include <stdio.h>

static void nandOpenCallback(s32, void*);
static void nandReadCloseCallback(s32, void*);
static void nandCloseCallback(s32, void*);
static void nandSafeCloseCallback(s32, void*);
static void nandSafeOpenCallback(const s32, void*);
static void nandReadOpenCallback(s32, void*);
static s32 nandSafeCloseAsync(NANDFileInfo*, NANDAsyncCallback, NANDCommandBlock*, BOOL);
static s32 nandSafeOpenAsync(const char*, NANDFileInfo*, const u8, void*, const u32, NANDAsyncCallback, NANDCommandBlock*, BOOL, BOOL);

static u32 nandGetUniqueNumber(void)
{
    static u32 s_counter = 0;
    u32 ret;
    BOOL enabled = OSDisableInterrupts();
    ret = s_counter++;
    OSRestoreInterrupts(enabled);
    return ret;
}

static s32 nandOpen(const char* path, const u8 mode, NANDCommandBlock* block, const BOOL async, const BOOL priv)
{
    s32 fd;
    char absPath[64] = "";
    u32 ipcMode = IPC_OPEN_NONE;

    nandGenerateAbsPath(absPath, path);

    if (!priv && nandIsPrivatePath(absPath))
    {
        return IPC_RESULT_ACCESS;
    }
    else
    {
        switch (mode)
        {
        case NAND_ACCESS_RW:
            ipcMode = IPC_OPEN_RW;
            break;
        case NAND_ACCESS_READ:
            ipcMode = IPC_OPEN_READ;
            break;
        case NAND_ACCESS_WRITE:
            ipcMode = IPC_OPEN_WRITE;
            break;
        default:
            break;
        }

        if (async)
        {
            fd = ISFS_OpenAsync(absPath, ipcMode, nandOpenCallback, block);
        }
        else
        {
            fd = ISFS_Open(absPath, ipcMode);
        }

        return fd;
    }
}

s32 NANDOpen(const char* path, NANDFileInfo* info, const u8 access)
{
    s32 fd;

    if (!nandIsInitialized())
    {
        return NAND_RESULT_FATAL_ERROR;
    }

    fd = nandOpen(path, access, NULL, FALSE, FALSE);

    if (fd >= 0)
    {
        info->fd = fd;
        info->mark = 1;
        return NAND_RESULT_OK;
    }
    else
    {
        return nandConvertErrorCode(fd);
    }
}

s32 NANDPrivateOpen(const char* path, NANDFileInfo* info, const u8 access)
{
    s32 fd;

    if (!nandIsInitialized())
    {
        return NAND_RESULT_FATAL_ERROR;
    }

    fd = nandOpen(path, access, NULL, FALSE, TRUE);

    if (fd >= 0)
    {
        info->fd = fd;
        info->mark = 1;
        return NAND_RESULT_OK;
    }
    else
    {
        return nandConvertErrorCode(fd);
    }
}

s32 NANDOpenAsync(const char* path, NANDFileInfo* info, const u8 access, NANDAsyncCallback callback, NANDCommandBlock* block)
{
    s32 fd;

    if (!nandIsInitialized())
    {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    block->info = info;
    fd = nandOpen(path, access, block, TRUE, FALSE);
    return nandConvertErrorCode(fd);
}

s32 NANDPrivateOpenAsync(const char* path, NANDFileInfo* info, const u8 access, NANDAsyncCallback callback, NANDCommandBlock* block)
{
    s32 fd;

    if (!nandIsInitialized())
    {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    block->info = info;
    fd = nandOpen(path, access, block, TRUE, TRUE);
    return nandConvertErrorCode(fd);
}

static void nandOpenCallback(s32 result, void* arg)
{
    NANDCommandBlock* block = (NANDCommandBlock*)arg;

    if (result >= 0)
    {
        block->info->fd = result;
        block->info->stage = 2;
        block->info->mark = 1;
        block->callback(NAND_RESULT_OK, block);
    }
    else
    {
        block->callback(nandConvertErrorCode(result), block);
    }
}

s32 NANDClose(NANDFileInfo* info)
{
    s32 err;

    if (!nandIsInitialized())
    {
        return NAND_RESULT_FATAL_ERROR;
    }

    if (info->mark != 1)
    {
        return NAND_RESULT_INVALID;
    }

    err = ISFS_Close(info->fd);

    if (err == IPC_RESULT_OK)
    {
        info->mark = 2;
    }

    return nandConvertErrorCode(err);
}

s32 NANDCloseAsync(NANDFileInfo* info, NANDAsyncCallback callback, NANDCommandBlock* block)
{
    s32 err;

    if (!nandIsInitialized())
    {
        return NAND_RESULT_FATAL_ERROR;
    }

    if (info->mark != 1)
    {
        return NAND_RESULT_INVALID;
    }

    block->callback = callback;
    block->info = info;
    err = ISFS_CloseAsync(info->fd, nandCloseCallback, block);
    return nandConvertErrorCode(err);
}

s32 NANDPrivateSafeOpenAsync(const char* path, NANDFileInfo* info, const u8 access, void* buffer, const u32 bufferSize, NANDAsyncCallback callback, NANDCommandBlock* block)
{
    return nandSafeOpenAsync(path, info, access, buffer, bufferSize, callback, block, TRUE, FALSE);
}

static s32 nandSafeOpenAsync(const char* path, NANDFileInfo* info, const u8 access, void* buffer, const u32 bufferSize, NANDAsyncCallback callback, NANDCommandBlock* block, BOOL priv, BOOL simple)
{
    if (!nandIsInitialized())
    {
        return NAND_RESULT_FATAL_ERROR;
    }

    if (simple && ((bufferSize & 0x3FFF) != 0))
    {
        return NAND_RESULT_INVALID;
    }

    info->access = access;
    info->stage = 0;
    block->simpleFlag = simple;
    nandGenerateAbsPath(info->openPath, path);

    if (!priv && nandIsPrivatePath(info->openPath))
    {
        return NAND_RESULT_ACCESS;
    }

    if (access == NAND_ACCESS_READ)
    {
        s32 fd;
        block->info = info;
        block->callback = callback;
        fd = ISFS_OpenAsync(info->openPath, IPC_OPEN_READ, nandReadOpenCallback, block);

        if (fd == IPC_RESULT_OK)
        {
            return NAND_RESULT_OK;
        }
        else
        {
            return nandConvertErrorCode(fd);
        }
    }
    else if (access == NAND_ACCESS_WRITE || access == NAND_ACCESS_RW)
    {
        s32 ret;
        block->info = info;
        block->callback = callback;
        block->state = 0;
        block->buffer = buffer;
        block->bufferSize = bufferSize;
        ret = ISFS_CreateDirAsync("/tmp/sys", 0, IPC_OPEN_RW, IPC_OPEN_RW, IPC_OPEN_RW, nandSafeOpenCallback, block);

        if (ret == IPC_RESULT_OK)
        {
            return NAND_RESULT_OK;
        }
        else
        {
            return nandConvertErrorCode(ret);
        }
    }
    else
    {
        return NAND_RESULT_INVALID;
    }
}

static void nandSafeOpenCallback(const s32 result, void* arg)
{
    NANDCommandBlock* block = (NANDCommandBlock*)arg;

    if ((result >= 0) || ((result == IPC_RESULT_EXISTS) && block->state == 0))
    {
        NANDFileInfo* info = block->info;
        s32 ret = -117;

        if (block->state == 0)
        {
            info->stage = 1;
        }

        if (block->state == 2)
        {
            info->tempFd = result;
            info->stage = 2;
        }

        if ((block->state == 2) && block->simpleFlag)
        {
            block->state += 2;
        }
        else
        {
            ++block->state;
        }

        if (block->state == 1)
        {
            ret = ISFS_GetAttrAsync(info->openPath, &block->ownerId, &block->groupId, &block->attr, &block->ownerPerm, &block->groupPerm, &block->otherPerm, nandSafeOpenCallback, arg);
        }
        else if (block->state == 2)
        {
            ret = ISFS_OpenAsync(info->openPath, IPC_OPEN_READ, nandSafeOpenCallback, arg);
        }
        else if (block->state == 3)
        {
            char tmpDir[64];
            block->uniqueNo = nandGetUniqueNumber();
            sprintf(tmpDir, "%s/%08x", "/tmp/sys", block->uniqueNo);
            ret = ISFS_CreateDirAsync(tmpDir, 0, IPC_OPEN_RW, IPC_OPEN_NONE, IPC_OPEN_NONE, nandSafeOpenCallback, arg);
        }
        else if (block->state == 4)
        {
            char filename[13];
            nandGetRelativeName(filename, info->openPath);

            if (!block->simpleFlag)
            {
                info->stage = 3;
                sprintf(info->tempPath, "%s/%08x/%s", "/tmp/sys", block->uniqueNo, filename);
            }
            else
            {
                sprintf(info->tempPath, "%s/%s", "/tmp/sys", filename);
            }

            ret = ISFS_CreateFileAsync(info->tempPath, block->attr, block->ownerPerm, block->groupPerm, block->otherPerm, nandSafeOpenCallback, arg);
        }
        else if (block->state == 5)
        {
            info->stage = 4;

            if (info->access == NAND_ACCESS_WRITE)
            {
                ret = ISFS_OpenAsync(info->tempPath, IPC_OPEN_WRITE, nandSafeOpenCallback, arg);
            }
            else if (info->access == NAND_ACCESS_RW)
            {
                ret = ISFS_OpenAsync(info->tempPath, IPC_OPEN_RW, nandSafeOpenCallback, arg);
            }
            else
            {
                ret = -117;
            }
        }
        else if (block->state == 6)
        {
            info->fd = result;
            info->stage = 5;
            block->state = 7;
            ret = ISFS_ReadAsync(info->tempFd, block->buffer, block->bufferSize, nandSafeOpenCallback, arg);
        }
        else if (block->state == 7)
        {
            ret = ISFS_ReadAsync(info->tempFd, block->buffer, block->bufferSize, nandSafeOpenCallback, arg);
        }
        else if (block->state == 8)
        {
            if (result > 0)
            {
                block->state = 6;
                ret = ISFS_WriteAsync(info->fd, block->buffer, (u32)result, nandSafeOpenCallback, arg);
            }
            else if (result == 0)
            {
                ret = ISFS_SeekAsync(info->fd, 0, IPC_SEEK_BEG, nandSafeOpenCallback, arg);
            }
        }
        else if (block->state == 9)
        {
            if (result == 0)
            {
                if (!block->simpleFlag)
                {
                    info->mark = 3;
                }
                else
                {
                    info->mark = 5;
                }

                block->callback(nandConvertErrorCode(IPC_RESULT_OK), block);
            }
            else
            {
                block->callback(nandConvertErrorCode(result), block);
            }

            return;
        }

        if (ret != IPC_RESULT_OK)
        {
            block->callback(nandConvertErrorCode(ret), block);
        }
    }
    else
    {
        block->callback(nandConvertErrorCode(result), block);
    }
}

static void nandReadOpenCallback(s32 result, void* arg)
{
    NANDCommandBlock* block = (NANDCommandBlock*)arg;

    if (result >= 0)
    {
        block->info->fd = result;
        block->info->stage = 2;

        if (!(block->simpleFlag))
        {
            block->info->mark = 3;
        }
        else
        {
            block->info->mark = 5;
        }

        block->callback(NAND_RESULT_OK, block);
    }
    else
    {
        block->callback(nandConvertErrorCode(result), block);
    }
}

s32 NANDSafeCloseAsync(NANDFileInfo* info, NANDAsyncCallback callback, NANDCommandBlock* block)
{
    return nandSafeCloseAsync(info, callback, block, FALSE);
}

static s32 nandSafeCloseAsync(NANDFileInfo* info, NANDAsyncCallback callback, NANDCommandBlock* block, BOOL simple)
{
    s32 err;

    if (!nandIsInitialized())
    {
        return NAND_RESULT_FATAL_ERROR;
    }

    if (!(((info->mark == 3) && !simple) || ((info->mark == 5) && simple)))
    {
        return NAND_RESULT_INVALID;
    }

    block->simpleFlag = simple;
    if (info->access == NAND_ACCESS_READ)
    {
        block->info = info;
        block->callback = callback;
        err = ISFS_CloseAsync(info->fd, nandReadCloseCallback, block);
    }
    else if ((info->access == NAND_ACCESS_WRITE) || (info->access == NAND_ACCESS_RW))
    {
        block->info = info;
        block->callback = callback;
        block->state = 10;
        err = ISFS_CloseAsync(info->fd, nandSafeCloseCallback, block);
    }
    else
    {
        err = IPC_RESULT_INVALID;
    }

    return nandConvertErrorCode(err);
}

static void nandSafeCloseCallback(s32 result, void* arg)
{
    NANDCommandBlock* block = (NANDCommandBlock*)arg;

    if (result == 0)
    {
        NANDFileInfo* info = block->info;
        s32 ret = -117;

        if (block->state == 12)
        {
            info->stage = 8;
        }

        if ((block->state == 12) && block->simpleFlag)
        {
            block->state += 2;
        }
        else
        {
            ++block->state;
        }

        if (block->state == 11)
        {
            info->stage = 6;
            ret = ISFS_CloseAsync(info->tempFd, nandSafeCloseCallback, arg);
        }
        else if (block->state == 12)
        {
            info->stage = 7;
            ret = ISFS_RenameAsync(info->tempPath, info->openPath, nandSafeCloseCallback, arg);
        }
        else if (block->state == 13)
        {
            char tmpdir[64] = "";
            nandGetParentDirectory(tmpdir, info->tempPath);
            ret = ISFS_DeleteAsync(tmpdir, nandSafeCloseCallback, arg);
        }
        else if (block->state == 14)
        {
            if (!(block->simpleFlag))
            {
                info->stage = 9;
            }

            info->mark = 4;
            block->callback(nandConvertErrorCode(result), block);
            return;
        }

        if (ret != 0)
        {
            block->callback(nandConvertErrorCode(ret), block);
        }
    }
    else
    {
        block->callback(nandConvertErrorCode(result), block);
    }
}

static void nandReadCloseCallback(s32 result, void* arg)
{
    NANDCommandBlock* block = (NANDCommandBlock*)arg;

    if (result == 0)
    {
        block->info->stage = 7;
        block->info->mark = 4;
    }

    block->callback(nandConvertErrorCode(result), block);
}

static void nandCloseCallback(s32 result, void* arg)
{
    NANDCommandBlock* block = (NANDCommandBlock*)arg;

    if (result == 0)
    {
        block->info->stage = 7;
        block->info->mark = 2;
    }

    block->callback(nandConvertErrorCode(result), block);
}
