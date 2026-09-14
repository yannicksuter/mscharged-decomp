#include "NL/plat/nlFileCache.h"
#include "Game/Sys/debug.h"

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/plat/nlFlash.h"
#include "NL/nlstring_tmpl.h"

static bool g_bDisableAllFileCaching = true;
static nlFileCache* sFileCache;

struct CachedFileLoadData
{
    CachedFileLoadData(nlFile* const f, const char* filename,
        void* const alloc, const unsigned long size, unsigned int bufferSize,
        LoadAsyncCallback const cb, void* const user, int fromDisc)
        : file(f)
        , alloc_data(alloc)
        , datasize(size)
        , bufferSize(bufferSize)
        , callback(cb)
        , user_data(user)
        , fromDisc(fromDisc)
    {
        nlStrNCpy(this->filename, filename, sizeof(this->filename));
    }

    nlFile* file;
    char filename[128];
    void* alloc_data;
    unsigned long datasize;
    unsigned int bufferSize;
    LoadAsyncCallback callback;
    void* user_data;
    int fromDisc;
};

static void AfterReadFromCacheCallback(s32, const char*, void*, u32, void*);
static void AfterReadFromDiscToCacheCallback(nlFile*, void*, unsigned int, unsigned long);
static void AfterReadFromDiscCallback(nlFile*, void*, unsigned int, unsigned long);
static void AfterWriteToCacheCallback(s32, const char*, void*, u32, void*);
static void FlashWriteCallback(s32);
static void FlashReadCallback(s32);

bool nlLoadEntireCachedFileAsync(const char* filename, LoadAsyncCallback callback,
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
    datasize = sFileCache->GetCachedFileSize(filename, &filesize);
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
        CachedFileLoadData* asyncData = new (8, true)
            CachedFileLoadData(0, filename, alloc_data, datasize, filesize, callback, user_data, 0);
        if (!sFileCache->LoadEntireCachedFileAsync(filename, alloc_data, datasize, filesize, AfterReadFromCacheCallback, asyncData))
        {
            tDebugPrintManager::Print(DC_LOADER, "Error calling LoadEntireCachedFileAsync\n");
            delete asyncData;
            return false;
        }
        return true;
    }
    else
    {
        CachedFileLoadData* asyncData = new (8, true)
            CachedFileLoadData(file, filename, alloc_data, datasize, filesize, callback, user_data, 1);
        nlReadAsync(file, alloc_data, datasize, sFileCache->mCacheWritesEnabled ? AfterReadFromDiscToCacheCallback : AfterReadFromDiscCallback, (unsigned long)asyncData, 0);
        return true;
    }
}

static void AfterReadFromCacheCallback(s32 result, const char*, void*, u32 size, void* user)
{
    CachedFileLoadData* data = (CachedFileLoadData*)user;
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

static void AfterReadFromDiscToCacheCallback(nlFile*, void* pBuffer, unsigned int, unsigned long uParam)
{
    CachedFileLoadData* data = (CachedFileLoadData*)uParam;
    nlClose(data->file);
    if (!sFileCache->WriteEntireFileToCacheAsync(data->filename, pBuffer, data->datasize, data->bufferSize, AfterWriteToCacheCallback, data))
    {
        tDebugPrintManager::Print(DC_LOADER, "Initial WriteEntireFileToCacheAsync failure\n");
        data->callback(data->alloc_data, data->datasize, data->user_data);
        delete data;
    }
}

static void AfterReadFromDiscCallback(nlFile*, void*, unsigned int, unsigned long uParam)
{
    CachedFileLoadData* data = (CachedFileLoadData*)uParam;
    data->callback(data->alloc_data, data->datasize, data->user_data);
    nlClose(data->file);
    delete data;
}

static void AfterWriteToCacheCallback(s32 result, const char*, void*, u32 size, void* user)
{
    CachedFileLoadData* data = (CachedFileLoadData*)user;
    if (result != size)
        tDebugPrintManager::Print(DC_LOADER, "Failed in AfterWriteToCacheCallback\n");
    data->callback(data->alloc_data, data->datasize, data->user_data);
    delete data;
}

void nlInitFileCache()
{
    sFileCache = new (8, false) nlFileCache;
}

nlFileCache* nlGetFileCache()
{
    return sFileCache;
}

bool nlFileCache::LoadEntireCachedFileAsync(const char* filename,
    void* buffer, u32, u32, FileCacheCallback callback, void* user)
{
    u32 hash = nlStringLowerHash(filename);
    CachedFile* value = 0;
    mFiles.FindGet(hash, &value);
    if (nlFlashChangeDirectory(2, 0) != 0)
    {
        tDebugPrintManager::Print(DC_LOADER, "nlFileCache cannot ensure in temp directory for load\n");
        return false;
    }
    if (mRequests.IsFull())
    {
        tDebugPrintManager::Print(DC_LOADER, "Cannot load from cache, work request Q is full\n");
        return false;
    }
    CacheRequest request;
    request.mState = CacheRequest::ReadPending;
    request.mBuffer = buffer;
    request.mReadCallback = callback;
    request.mBufferSize = value->mBufferSize;
    request.mUserData = user;
    request.mFile = value;
    bool start = mRequests.GetCount() == 0;
    mRequests.Push(request);
    if (start)
        Run(0.01f);
    return true;
}

bool nlFileCache::WriteEntireFileToCacheAsync(const char* filename,
    void* buffer, u32 size, u32 bufferSize,
    FileCacheCallback callback, void* user)
{
    if (nlFlashChangeDirectory(2, 0) != 0)
    {
        tDebugPrintManager::Print(DC_LOADER, "nlFileCache cannot ensure in temp directory for write\n");
        return false;
    }
    if (mRequests.IsFull())
    {
        tDebugPrintManager::Print(DC_LOADER, "Cannot write to cache, work request Q is full\n");
        return false;
    }
    u32 hash = nlStringLowerHash(filename);
    CachedFile value;
    value.mFileSize = size;
    value.mBufferSize = bufferSize;
    nlStrNCpy(value.mFilename, filename, sizeof(value.mFilename));
    nlSNPrintf(value.mCacheFilename, sizeof(value.mCacheFilename), "C%x", hash);
    mFiles.Add(hash, value);
    CachedFile* found = 0;
    mFiles.FindGet(hash, &found);
    CacheRequest request;
    request.mState = CacheRequest::WritePending;
    request.mBuffer = buffer;
    request.mWriteCallback = callback;
    request.mUserData = user;
    request.mFile = found;
    bool start = mRequests.GetCount() == 0;
    mRequests.Push(request);
    if (start)
        Run(0.01f);
    return true;
}

static void FlashWriteCallback(s32 result)
{
    nlFileCache* cache = sFileCache;
    CacheRequest request = cache->mRequests.Pop();
    CachedFile* value = request.mFile;
    s32 closeResult = nlFlashClose(0);
    if (closeResult != 0)
        tDebugPrintManager::Print(DC_LOADER, "nlFileCache: FlashMemClose returned error %d after write callback\n", closeResult);
    if (result != value->mBufferSize)
    {
        tDebugPrintManager::Print(DC_LOADER, "nlFileCache: FlashWriteCallback returned error %d\n", result);
        request.mWriteCallback(result, value->mFilename, 0, 0, request.mUserData);
        cache->mFiles.Remove(nlStringLowerHash(value->mFilename));
    }
    else
    {
        request.mWriteCallback(result, value->mFilename, request.mBuffer, value->mBufferSize, request.mUserData);
    }
}

static void FlashReadCallback(s32 result)
{
    CacheRequest request = sFileCache->mRequests.Pop();
    CachedFile* value = request.mFile;
    s32 closeResult = nlFlashClose(0);
    if (closeResult != 0)
        tDebugPrintManager::Print(DC_LOADER, "nlFileCache: FlashMemClose returned error %d after read callback\n", closeResult);
    if (result != value->mBufferSize)
    {
        tDebugPrintManager::Print(DC_LOADER, "nlFileCache: FlashReadCallback returned error %d\n", result);
        request.mReadCallback(result, value->mFilename, 0, 0, request.mUserData);
    }
    else
    {
        request.mReadCallback(result, value->mFilename, request.mBuffer, value->mBufferSize, request.mUserData);
    }
}

void nlFileCache::Run(float)
{
    if (mRequests.GetCount() == 0)
        return;
    CacheRequest& request = mRequests.Peek();
    switch (request.mState)
    {
    case CacheRequest::Idle:
    case CacheRequest::Writing:
        break;
    case CacheRequest::WritePending:
    {
        CachedFile* value = request.mFile;
        bool started = false;
        s32 result = nlFlashCreate(value->mCacheFilename, 0x30, 0);
        if (result == 0)
        {
            result = nlFlashOpen(value->mCacheFilename, 2, 0);
            if (result == 0)
            {
                result = nlFlashWrite(request.mBuffer, value->mBufferSize, FlashWriteCallback);
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
            request.mWriteCallback(result, value->mFilename, 0, 0, request.mUserData);
            mFiles.Remove(nlStringLowerHash(value->mFilename));
            mRequests.Pop();
        }
        else
            request.mState = CacheRequest::Writing;
        break;
    }
    case CacheRequest::ReadPending:
    {
        CachedFile* value = request.mFile;
        bool started = false;
        s32 result = nlFlashOpen(value->mCacheFilename, 1, 0);
        if (result == 0)
        {
            result = nlFlashRead(&request.mBuffer, &request.mBufferSize, FlashReadCallback, true);
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
            request.mReadCallback(result, value->mFilename, 0, 0, request.mUserData);
            mRequests.Pop();
        }
        else
            request.mState = CacheRequest::Reading;
        break;
    }
    case CacheRequest::Reading:
    case CacheRequest::State5:
    case CacheRequest::State6:
        break;
    }
}

static TweakBoolBinding sDisableAllFileCachingTweak(
    "g_bDisableAllFileCaching", "FileCache", &g_bDisableAllFileCaching, true);
