#include "unclassified/tu_80376888.h"

#include "NL/nlMemory.h"
#include "NL/nlString.h"

typedef void (*NANDAsyncCallback)(s32 result, NANDCommandBlock* block);

struct NANDStatus
{
    u32 ownerId;
    u16 groupId;
    u8 attr;
    u8 perm;
};

struct NANDFileInfo
{
    s32 fd;
    s32 tempFd;
    char openPath[64];
    char tempPath[64];
    u8 access;
    u8 stage;
    u8 mark;
};

struct NANDCommandBlock
{
    void* userData;
    NANDAsyncCallback callback;
    NANDFileInfo* info;
    void* bytes;
    void* inodes;
    NANDStatus* status;
    u32 ownerId;
    u16 groupId;
    u8 nextStage;
    u32 attr;
    u32 ownerPerm;
    u32 groupPerm;
    u32 otherPerm;
    u32 dirFileCount;
    char path[64];
    u32* length;
    u32* position;
    s32 state;
    void* buffer;
    u32 bufferSize;
    u8* type;
    u32 uniqueNo;
    u32 reqBlocks;
    u32 reqInodes;
    u32* answer;
    u32 homeBlocks;
    u32 homeInodes;
    u32 userBlocks;
    u32 userInodes;
    u32 workBlocks;
    u32 workInodes;
    const char** dir;
};

extern "C"
{
s32 NANDInit();
s32 NANDGetCurrentDir(char* path);
s32 NANDGetHomeDir(char* path);
s32 NANDChangeDir(const char* path);
s32 NANDChangeDirAsync(
    const char* path, NANDAsyncCallback callback, NANDCommandBlock* block);
s32 NANDCreateDir(const char* path, u8 permissions, u8 attributes);
s32 NANDCreateDirAsync(const char* path, u8 permissions, u8 attributes,
    NANDAsyncCallback callback, NANDCommandBlock* block);
s32 NANDCreate(const char* path, u8 permissions, u8 attributes);
s32 NANDCreateAsync(const char* path, u8 permissions, u8 attributes,
    NANDAsyncCallback callback, NANDCommandBlock* block);
s32 NANDCheck(u32 blocks, u32 files, u32* answer);
s32 NANDCheckAsync(u32 blocks, u32 files, u32* answer,
    NANDAsyncCallback callback, NANDCommandBlock* block);
s32 NANDDelete(const char* path);
s32 NANDDeleteAsync(
    const char* path, NANDAsyncCallback callback, NANDCommandBlock* block);
s32 NANDWrite(NANDFileInfo* info, const void* data, u32 size);
s32 NANDWriteAsync(NANDFileInfo* info, const void* data, u32 size,
    NANDAsyncCallback callback, NANDCommandBlock* block);
s32 NANDGetLength(NANDFileInfo* info, u32* size);
s32 NANDGetLengthAsync(NANDFileInfo* info, u32* size,
    NANDAsyncCallback callback, NANDCommandBlock* block);
s32 NANDRead(NANDFileInfo* info, void* data, u32 size);
s32 NANDReadAsync(NANDFileInfo* info, void* data, u32 size,
    NANDAsyncCallback callback, NANDCommandBlock* block);
s32 NANDOpen(const char* path, NANDFileInfo* info, s32 mode);
s32 NANDOpenAsync(const char* path, NANDFileInfo* info, s32 mode,
    NANDAsyncCallback callback, NANDCommandBlock* block);
s32 NANDClose(NANDFileInfo* info);
s32 NANDCloseAsync(NANDFileInfo* info,
    NANDAsyncCallback callback, NANDCommandBlock* block);
}

enum
{
    NAND_RESULT_OK = 0
};

static char lbl_806DFB28[] = "/tmp";
static char* lbl_806DFB30 = lbl_806DFB28;
static s32 lbl_806DFB34 = -1;
static char lbl_806DFB38[] = "/nocopy";

static bool lbl_806E24A0;
static NANDResultCallback lbl_806E24A4;
static s32 lbl_806E24A8;

static NANDCommandBlock lbl_805A0928;
static NANDFileInfo lbl_805A09E0;

UnidentifiedTask_80376888::UnidentifiedTask_80376888()
{
}

extern "C" void fn_803768A0(s32 result, NANDCommandBlock*)
{
    lbl_806E24A8 = result;
    lbl_806DFB34 = 1;
}

void UnidentifiedTask_80376888::Run(float)
{
    if (lbl_806DFB34 > 0)
    {
        --lbl_806DFB34;
    }

    if (lbl_806DFB34 == 0)
    {
        lbl_806DFB34 = -1;
        NANDResultCallback callback = lbl_806E24A4;
        if (callback != 0)
        {
            lbl_806E24A4 = 0;
            callback(lbl_806E24A8);
        }
    }
}

extern "C" void fn_803768F8()
{
    if (NANDInit() == NAND_RESULT_OK)
    {
        lbl_806E24A0 = true;
        fn_80376934(0, 0);
    }
}

extern "C" s32 fn_80376934(
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
        nlStrNCat(path, path, lbl_806DFB38, sizeof(path));
        if (result != NAND_RESULT_OK)
        {
            return result;
        }
    }
    else if (directory == 2)
    {
        nlStrNCpy(path, lbl_806DFB30, sizeof(path));
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
            path, fn_803768A0, &lbl_805A0928);
        if (result == NAND_RESULT_OK)
        {
            lbl_806E24A4 = callback;
        }
        return result;
    }

    return NANDChangeDir(path);
}

extern "C" s32 fn_80376B08(
    const char* name, u8 permissions, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDCreateDirAsync(name, permissions, 0,
            fn_803768A0, &lbl_805A0928);
        if (result == NAND_RESULT_OK)
        {
            lbl_806E24A4 = callback;
        }
    }
    else
    {
        result = NANDCreateDir(name, permissions, 0);
    }
    return result;
}

extern "C" s32 fn_80376B68(
    const char* name, u8 permissions, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDCreateAsync(name, permissions, 0,
            fn_803768A0, &lbl_805A0928);
        if (result == NAND_RESULT_OK)
        {
            lbl_806E24A4 = callback;
        }
    }
    else
    {
        result = NANDCreate(name, permissions, 0);
    }
    return result;
}

extern "C" s32 fn_80376BC8(
    u32 blocks, u32 files, u32* answer, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDCheckAsync(blocks, files, answer,
            fn_803768A0, &lbl_805A0928);
        if (result == NAND_RESULT_OK)
        {
            lbl_806E24A4 = callback;
        }
    }
    else
    {
        result = NANDCheck(blocks, files, answer);
    }
    return result;
}

extern "C" s32 fn_80376C20(
    const char* name, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDDeleteAsync(
            name, fn_803768A0, &lbl_805A0928);
        if (result == NAND_RESULT_OK)
        {
            lbl_806E24A4 = callback;
        }
    }
    else
    {
        result = NANDDelete(name);
    }
    return result;
}

extern "C" s32 fn_80376C78(
    const void* data, u32 size, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDWriteAsync(&lbl_805A09E0, data, size,
            fn_803768A0, &lbl_805A0928);
        if (result == NAND_RESULT_OK)
        {
            lbl_806E24A4 = callback;
        }
    }
    else
    {
        result = NANDWrite(&lbl_805A09E0, data, size);
    }
    return result;
}

extern "C" s32 fn_80376CF8(
    u32* size, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDGetLengthAsync(&lbl_805A09E0, size,
            fn_803768A0, &lbl_805A0928);
        if (result == NAND_RESULT_OK)
        {
            lbl_806E24A4 = callback;
        }
    }
    else
    {
        result = NANDGetLength(&lbl_805A09E0, size);
    }
    return result;
}

extern "C" s32 fn_80376D6C(void** data, u32* size,
    NANDResultCallback callback, bool bufferProvided)
{
    s32 result;
    if (!bufferProvided)
    {
        result = NANDGetLength(&lbl_805A09E0, size);
        if (result != NAND_RESULT_OK)
        {
            return result;
        }
        *size = (*size + 31) & ~31u;
        *data = nlMalloc(*size, 32, true);
    }

    if (callback != 0)
    {
        result = NANDReadAsync(&lbl_805A09E0, *data, *size,
            fn_803768A0, &lbl_805A0928);
        if (result == NAND_RESULT_OK)
        {
            lbl_806E24A4 = callback;
        }
    }
    else
    {
        result = NANDRead(&lbl_805A09E0, *data, *size);
    }
    return result;
}

extern "C" s32 fn_80376E3C(
    const char* name, s32 mode, NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDOpenAsync(name, &lbl_805A09E0, mode,
            fn_803768A0, &lbl_805A0928);
        if (result == NAND_RESULT_OK)
        {
            lbl_806E24A4 = callback;
        }
    }
    else
    {
        result = NANDOpen(name, &lbl_805A09E0, mode);
    }
    return result;
}

extern "C" s32 fn_80376EB0(NANDResultCallback callback)
{
    s32 result;
    if (callback != 0)
    {
        result = NANDCloseAsync(
            &lbl_805A09E0, fn_803768A0, &lbl_805A0928);
        if (result == NAND_RESULT_OK)
        {
            lbl_806E24A4 = callback;
        }
    }
    else
    {
        result = NANDClose(&lbl_805A09E0);
    }
    return result;
}

extern "C" bool fn_80376F18()
{
    return lbl_806DFB34 != -1;
}
