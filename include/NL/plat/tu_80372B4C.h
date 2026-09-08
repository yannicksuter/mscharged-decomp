#ifndef NL_PLAT_TU_80372B4C_H
#define NL_PLAT_TU_80372B4C_H

#include "NL/CircularQueue.h"
#include "NL/nlAVLTree.h"
#include "NL/nlFile.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"

typedef void (*FileCacheCallback_80372F14)(s32, const char*, void*, u32, void*);

struct CachedFile_80373588
{
    CachedFile_80373588()
        : m_00(0)
        , m_04(0)
    {
        m_08[0] = 0;
        m_88[0] = 0;
    }

    u32 m_00;
    u32 m_04;
    char m_08[128];
    char m_88[16];
};

struct CacheRequest_803734A0
{
    enum State
    {
        State0,
        State1,
        State2,
        State3,
        State4,
        State5,
        State6
    };

    CacheRequest_803734A0()
        : m_00(State0)
        , m_04(0)
        , m_08(0)
        , m_0C(0)
        , m_10(0)
        , m_14(0)
        , m_18(0)
    {
    }

    State m_00;
    void* m_04;
    FileCacheCallback_80372F14 m_08;
    FileCacheCallback_80372F14 m_0C;
    u32 m_10;
    void* m_14;
    CachedFile_80373588* m_18;
};

class FileCache_80535C20 : public nlTask
{
public:
    FileCache_80535C20()
        : m_20(16, 16)
        , m_44(0)
        , m_48(0)
        , m_5D4(false)
    {
        m_20.m_Allocator.Initialize(200, 0);
    }

    bool LoadEntireCachedFileAsync(const char*, void*, u32, u32,
        FileCacheCallback_80372F14, void*);
    bool WriteEntireFileToCacheAsync(const char*, void*, u32, u32,
        FileCacheCallback_80372F14, void*);
    virtual void Run(float);
    virtual const char* GetName() { return "FileCacheTask"; }

    u32 fn_80372BB0(const char* filename, unsigned int* size)
    {
        u32 hash = nlStringLowerHash(filename);
        CachedFile_80373588* value = 0;
        if (m_20.FindGet(hash, &value))
        {
            *size = value->m_00;
            return value->m_04;
        }
        return 0;
    }

    nlAVLTreeSlotPool<u32, CachedFile_80373588, DefaultKeyCompare<u32> > m_20;
    u32 m_44;
    u32 m_48;
    StaticCircularQueue<CacheRequest_803734A0, 50> m_4C;
    bool m_5D4;
};

extern "C"
{
    bool fn_80372B4C(const char*, LoadAsyncCallback, void*, unsigned int,
        eAllocType, void*, unsigned long, MemoryAllocator*);
    void fn_803730D8();
    FileCache_80535C20* fn_803733D4();
}

#endif // NL_PLAT_TU_80372B4C_H
