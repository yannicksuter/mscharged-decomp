#include "NL/plat/tu_80372B4C.h"
#include "Game/Sys/debug.h"

#include "Game/TweakValue.h"
#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/plat/nlFlash.h"
#include "NL/nlstring_tmpl.h"


static bool g_bDisableAllFileCaching = true;
static FileCache_80535C20* lbl_806E2460;

struct AsyncFileLoadData_80372D84
{
    AsyncFileLoadData_80372D84(nlFile* const f, const char* filename,
        void* const alloc, const unsigned long size, unsigned int bufferSize,
        LoadAsyncCallback const cb, void* const user, int value)
        : file(f)
        , alloc_data(alloc)
        , datasize(size)
        , m_8C(bufferSize)
        , callback(cb)
        , user_data(user)
        , m_98(value)
    {
        nlStrNCpy(m_04, filename, sizeof(m_04));
    }

    nlFile* file;
    char m_04[128];
    void* alloc_data;
    unsigned long datasize;
    unsigned int m_8C;
    LoadAsyncCallback callback;
    void* user_data;
    int m_98;
};

static void fn_80372F14(s32, const char*, void*, u32, void*);
static void fn_80372F88(nlFile*, void*, unsigned int, unsigned long);
static void fn_80373024(nlFile*, void*, unsigned int, unsigned long);
static void AfterWriteToCacheCallback(s32, const char*, void*, u32, void*);
static void FlashWriteCallback(s32);
static void FlashReadCallback(s32);

extern "C" bool fn_80372B4C(const char* filename, LoadAsyncCallback callback,
    void* user_data, unsigned int alignment, eAllocType type, void* buffer,
    unsigned long bufferSize, MemoryAllocator* allocator)
{
    if (g_bDisableAllFileCaching)
        return nlLoadEntireFileAsync(filename, callback, user_data, alignment, type, buffer, bufferSize, allocator);

    if (alignment < 32)
        alignment = 32;
    nlFile* file = 0;
    unsigned int filesize = 0;
    unsigned long datasize;
    bool cached = false;
    datasize = lbl_806E2460->fn_80372BB0(filename, &filesize);
    if (datasize != 0)
    {
        cached = true;
    }
    else
    {
        file = nlOpen(filename);
        if (file == 0)
            return false;
        datasize = nlFileSize(file, &filesize);
        if (datasize == 0)
        {
            nlClose(file);
            callback(0, datasize, user_data);
            return false;
        }
        filesize = (datasize & 31) == 0 ? datasize : (filesize + 32) & ~31;
    }

    if (allocator == 0)
        allocator = CurrentAllocator;
    CurrentAllocator = allocator;
    AllocatorStack[AllocatorStackDepth++] = allocator;
    void* alloc_data;
    if (buffer != 0)
        alloc_data = buffer;
    else if (type == AllocateStart)
        alloc_data = operator new(filesize, alignment, false);
    else if (type == AllocateEnd)
        alloc_data = operator new(filesize, alignment, true);
    else
        alloc_data = operator new(filesize, alignment, false);
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];

    if (cached)
    {
        AsyncFileLoadData_80372D84* asyncData = new (8, true)
            AsyncFileLoadData_80372D84(0, filename, alloc_data, datasize, filesize, callback, user_data, 0);
        if (!lbl_806E2460->LoadEntireCachedFileAsync(filename, alloc_data, datasize, filesize, fn_80372F14, asyncData))
        {
            tDebugPrintManager::Print(DC_LOADER, "Error calling LoadEntireCachedFileAsync\n");
            delete asyncData;
            return false;
        }
        return true;
    }
    else
    {
        AsyncFileLoadData_80372D84* asyncData = new (8, true)
            AsyncFileLoadData_80372D84(file, filename, alloc_data, datasize, filesize, callback, user_data, 1);
        nlReadAsync(file, alloc_data, datasize, lbl_806E2460->m_5D4 ? fn_80372F88 : fn_80373024, (unsigned long)asyncData, 0);
        return true;
    }
}

static void fn_80372F14(s32 result, const char*, void*, u32 size, void* user)
{
    AsyncFileLoadData_80372D84* data = (AsyncFileLoadData_80372D84*)user;
    if (result == size)
    {
        data->callback(data->alloc_data, data->datasize, data->user_data);
        delete data;
    }
    else
    {
        tDebugPrintManager::Print(DC_LOADER, "Error %d reading NAND cached file\n", result);
        delete data;
    }
}

static void fn_80372F88(nlFile*, void* pBuffer, unsigned int, unsigned long uParam)
{
    AsyncFileLoadData_80372D84* data = (AsyncFileLoadData_80372D84*)uParam;
    nlClose(data->file);
    if (!lbl_806E2460->WriteEntireFileToCacheAsync(data->m_04, pBuffer, data->datasize, data->m_8C, AfterWriteToCacheCallback, data))
    {
        tDebugPrintManager::Print(DC_LOADER, "Initial WriteEntireFileToCacheAsync failure\n");
        data->callback(data->alloc_data, data->datasize, data->user_data);
        delete data;
    }
}

static void fn_80373024(nlFile*, void*, unsigned int, unsigned long uParam)
{
    AsyncFileLoadData_80372D84* data = (AsyncFileLoadData_80372D84*)uParam;
    data->callback(data->alloc_data, data->datasize, data->user_data);
    nlClose(data->file);
    delete data;
}

static void AfterWriteToCacheCallback(s32 result, const char*, void*, u32 size, void* user)
{
    AsyncFileLoadData_80372D84* data = (AsyncFileLoadData_80372D84*)user;
    if (result != size)
        tDebugPrintManager::Print(DC_LOADER, "Failed in AfterWriteToCacheCallback\n");
    data->callback(data->alloc_data, data->datasize, data->user_data);
    delete data;
}

extern "C" void fn_803730D8()
{
    lbl_806E2460 = new (8, false) FileCache_80535C20;
}

extern "C" FileCache_80535C20* fn_803733D4()
{
    return lbl_806E2460;
}

bool FileCache_80535C20::LoadEntireCachedFileAsync(const char* filename,
    void* buffer, u32, u32, FileCacheCallback_80372F14 callback, void* user)
{
    u32 hash = nlStringLowerHash(filename);
    CachedFile_80373588* value = 0;
    m_20.FindGet(hash, &value);
    if (nlFlashChangeDirectory(2, 0) != 0)
    {
        tDebugPrintManager::Print(DC_LOADER, "nlFileCache cannot ensure in temp directory for load\n");
        return false;
    }
    if (m_4C.IsFull())
    {
        tDebugPrintManager::Print(DC_LOADER, "Cannot load from cache, work request Q is full\n");
        return false;
    }
    CacheRequest_803734A0 request;
    request.m_00 = CacheRequest_803734A0::State3;
    request.m_04 = buffer;
    request.m_0C = callback;
    request.m_10 = value->m_00;
    request.m_14 = user;
    request.m_18 = value;
    bool start = m_4C.GetCount() == 0;
    m_4C.Push(request);
    if (start)
        Run(0.01f);
    return true;
}

bool FileCache_80535C20::WriteEntireFileToCacheAsync(const char* filename,
    void* buffer, u32 size, u32 bufferSize,
    FileCacheCallback_80372F14 callback, void* user)
{
    if (nlFlashChangeDirectory(2, 0) != 0)
    {
        tDebugPrintManager::Print(DC_LOADER, "nlFileCache cannot ensure in temp directory for write\n");
        return false;
    }
    if (m_4C.IsFull())
    {
        tDebugPrintManager::Print(DC_LOADER, "Cannot write to cache, work request Q is full\n");
        return false;
    }
    u32 hash = nlStringLowerHash(filename);
    CachedFile_80373588 value;
    value.m_04 = size;
    value.m_00 = bufferSize;
    nlStrNCpy(value.m_08, filename, sizeof(value.m_08));
    nlSNPrintf(value.m_88, sizeof(value.m_88), "C%x", hash);
    m_20.Add(hash, value);
    CachedFile_80373588* found = 0;
    m_20.FindGet(hash, &found);
    CacheRequest_803734A0 request;
    request.m_00 = CacheRequest_803734A0::State1;
    request.m_04 = buffer;
    request.m_08 = callback;
    request.m_14 = user;
    request.m_18 = found;
    bool start = m_4C.GetCount() == 0;
    m_4C.Push(request);
    if (start)
        Run(0.01f);
    return true;
}

static void FlashWriteCallback(s32 result)
{
    FileCache_80535C20* cache = lbl_806E2460;
    CacheRequest_803734A0 request = cache->m_4C.Pop();
    CachedFile_80373588* value = request.m_18;
    s32 closeResult = nlFlashClose(0);
    if (closeResult != 0)
        tDebugPrintManager::Print(DC_LOADER, "nlFileCache: FlashMemClose returned error %d after write callback\n", closeResult);
    if (result != value->m_00)
    {
        tDebugPrintManager::Print(DC_LOADER, "nlFileCache: FlashWriteCallback returned error %d\n", result);
        request.m_08(result, value->m_08, 0, 0, request.m_14);
        cache->m_20.Remove(nlStringLowerHash(value->m_08));
    }
    else
    {
        request.m_08(result, value->m_08, request.m_04, value->m_00, request.m_14);
    }
}

static void FlashReadCallback(s32 result)
{
    CacheRequest_803734A0 request = lbl_806E2460->m_4C.Pop();
    CachedFile_80373588* value = request.m_18;
    s32 closeResult = nlFlashClose(0);
    if (closeResult != 0)
        tDebugPrintManager::Print(DC_LOADER, "nlFileCache: FlashMemClose returned error %d after read callback\n", closeResult);
    if (result != value->m_00)
    {
        tDebugPrintManager::Print(DC_LOADER, "nlFileCache: FlashReadCallback returned error %d\n", result);
        request.m_0C(result, value->m_08, 0, 0, request.m_14);
    }
    else
    {
        request.m_0C(result, value->m_08, request.m_04, value->m_00, request.m_14);
    }
}

void FileCache_80535C20::Run(float)
{
    if (m_4C.GetCount() == 0)
        return;
    CacheRequest_803734A0& request = m_4C.Peek();
    switch (request.m_00)
    {
    case CacheRequest_803734A0::State0:
    case CacheRequest_803734A0::State2:
        break;
    case CacheRequest_803734A0::State1:
    {
        CachedFile_80373588* value = request.m_18;
        bool started = false;
        s32 result = nlFlashCreate(value->m_88, 0x30, 0);
        if (result == 0)
        {
            result = nlFlashOpen(value->m_88, 2, 0);
            if (result == 0)
            {
                result = nlFlashWrite(request.m_04, value->m_00, FlashWriteCallback);
                if (result == 0)
                    started = true;
                else
                {
                    tDebugPrintManager::Print(DC_LOADER, "nlFileCache: Error %d calling FlashMemWrite\n", result);
                    s32 closeResult = nlFlashClose(0);
                    if (closeResult != 0)
                        tDebugPrintManager::Print(DC_LOADER, "nlFileCache: Error %d calling FlashMemClose after write error\n", closeResult);
                }
            }
            else
                tDebugPrintManager::Print(DC_LOADER, "nlFileCache: Error %d calling FlashMemOpen\n", result);
        }
        else
            tDebugPrintManager::Print(DC_LOADER, "nlFileCache: Error %d calling FlashMemCreate\n", result);
        if (!started)
        {
            request.m_08(result, value->m_08, 0, 0, request.m_14);
            m_20.Remove(nlStringLowerHash(value->m_08));
            m_4C.Pop();
        }
        else
            request.m_00 = CacheRequest_803734A0::State2;
        break;
    }
    case CacheRequest_803734A0::State3:
    {
        CachedFile_80373588* value = request.m_18;
        bool started = false;
        s32 result = nlFlashOpen(value->m_88, 1, 0);
        if (result == 0)
        {
            result = nlFlashRead(&request.m_04, &request.m_10, FlashReadCallback, true);
            if (result == 0)
                started = true;
            else
            {
                tDebugPrintManager::Print(DC_LOADER, "nlFileCache: Error %d calling FlashMemRead\n", result);
                s32 closeResult = nlFlashClose(0);
                if (closeResult != 0)
                    tDebugPrintManager::Print(DC_LOADER, "nlFileCache: Error %d calling FlashMemClose after read error\n", closeResult);
            }
        }
        else
            tDebugPrintManager::Print(DC_LOADER, "nlFileCache: Error %d calling FlashMemOpen\n", result);
        if (!started)
        {
            request.m_0C(result, value->m_08, 0, 0, request.m_14);
            m_4C.Pop();
        }
        else
            request.m_00 = CacheRequest_803734A0::State4;
        break;
    }
    case CacheRequest_803734A0::State4:
    case CacheRequest_803734A0::State5:
    case CacheRequest_803734A0::State6:
        break;
    }
}

static TweakBoolBinding lbl_8059C428(
    "g_bDisableAllFileCaching", "FileCache", &g_bDisableAllFileCaching, true);
