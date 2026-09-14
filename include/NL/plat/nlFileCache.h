#ifndef NL_PLAT_NL_FILE_CACHE_H
#define NL_PLAT_NL_FILE_CACHE_H

#include "NL/CircularQueue.h"
#include "NL/nlAVLTree.h"
#include "NL/nlFile.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"

typedef void (*FileCacheCallback)(s32, const char*, void*, u32, void*);

struct CachedFile
{
    CachedFile()
        : mBufferSize(0)
        , mFileSize(0)
    {
        mFilename[0] = 0;
        mCacheFilename[0] = 0;
    }

    u32 mBufferSize;
    u32 mFileSize;
    char mFilename[128];
    char mCacheFilename[16];
};

struct CacheRequest
{
    enum State
    {
        Idle,
        WritePending,
        Writing,
        ReadPending,
        Reading,
        State5,
        State6
    };

    CacheRequest()
        : mState(Idle)
        , mBuffer(0)
        , mWriteCallback(0)
        , mReadCallback(0)
        , mBufferSize(0)
        , mUserData(0)
        , mFile(0)
    {
    }

    State mState;
    void* mBuffer;
    FileCacheCallback mWriteCallback;
    FileCacheCallback mReadCallback;
    u32 mBufferSize;
    void* mUserData;
    CachedFile* mFile;
};

class nlFileCache : public nlTask
{
public:
    nlFileCache()
        : mFiles(16, 16)
        , m_44(0)
        , m_48(0)
        , mCacheWritesEnabled(false)
    {
        mFiles.GetAllocator()->Initialize(200, 0);
    }

    bool LoadEntireCachedFileAsync(const char*, void*, u32, u32,
        FileCacheCallback, void*);
    bool WriteEntireFileToCacheAsync(const char*, void*, u32, u32,
        FileCacheCallback, void*);
    virtual void Run(float);
    virtual const char* GetName() { return "FileCacheTask"; }

    u32 GetCachedFileSize(const char* filename, unsigned int* size)
    {
        u32 hash = nlStringLowerHash(filename);
        CachedFile* value = 0;
        if (mFiles.FindGet(hash, &value))
        {
            *size = value->mBufferSize;
            return value->mFileSize;
        }
        return 0;
    }

    nlAVLTreeSlotPool<u32, CachedFile, DefaultKeyCompare<u32> > mFiles;
    u32 m_44;
    u32 m_48;
    StaticCircularQueue<CacheRequest, 50> mRequests;
    bool mCacheWritesEnabled;
};

bool nlLoadEntireCachedFileAsync(const char*, LoadAsyncCallback, void*, unsigned int,
    eAllocType, void*, unsigned long, MemoryAllocator*);
void nlInitFileCache();
nlFileCache* nlGetFileCache();

#endif // NL_PLAT_NL_FILE_CACHE_H
