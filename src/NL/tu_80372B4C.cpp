#include "NL/tu_80372B4C.h"

#include "NL/nlString.h"

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

extern "C"
{
extern bool lbl_806DFB00;
extern UnidentifiedTask_803730D8* lbl_806E2460;
void fn_8004F594(int category, const char* format, ...);
void fn_80372F14(s32 result, const char* filename, void* buffer, unsigned int size, void* user_data);
void fn_80372F88(nlFile* file, void* pBuffer, unsigned int uSize, unsigned long uParam);
void fn_80373024(nlFile* file, void* pBuffer, unsigned int uSize, unsigned long uParam);
void fn_80373074(s32 result, const char* filename, void* buffer, unsigned int size, void* user_data);
bool fn_803733DC(UnidentifiedTask_803730D8* cache, const char* filename, void* buffer,
    unsigned long datasize, unsigned int filesize, UnidentifiedCacheCallback_80372F14 callback, void* user_data);
bool fn_80373588(UnidentifiedTask_803730D8* cache, const char* filename, void* buffer,
    unsigned long datasize, unsigned int filesize, UnidentifiedCacheCallback_80372F14 callback, void* user_data);
}

struct UnidentifiedAsyncFileLoadData_80372B4C
{
    /* 0x00 */ nlFile* file;
    /* 0x04 */ char field04[128];
    /* 0x84 */ void* alloc_data;
    /* 0x88 */ unsigned long datasize;
    /* 0x8C */ unsigned int field8C;
    /* 0x90 */ LoadAsyncCallback callback;
    /* 0x94 */ void* user_data;
    /* 0x98 */ unsigned int field98;

    UnidentifiedAsyncFileLoadData_80372B4C(nlFile* const f, const char* filename,
        void* const alloc, const unsigned long size, unsigned int filesize,
        LoadAsyncCallback const cb, void* const user, unsigned int field)
        : file(f)
        , alloc_data(alloc)
        , datasize(size)
        , field8C(filesize)
        , callback(cb)
        , user_data(user)
        , field98(field)
    {
        nlStrNCpy(field04, filename, sizeof(field04));
    }
}; // size 0x9C

extern "C" bool fn_80372B4C(const char* filename, LoadAsyncCallback callback, void* user_data,
    unsigned int alignment, eAllocType type, void* buffer, unsigned long bufferSize, MemoryAllocator* allocator)
{
    if (lbl_806DFB00)
    {
        return nlLoadEntireFileAsync(filename, callback, user_data, alignment, type, buffer, bufferSize, allocator) != 0;
    }
    if (alignment < 32)
    {
        alignment = 32;
    }

    nlFile* file = 0;
    unsigned int filesize = 0;
    unsigned long datasize;
    bool cached = false;
    UnidentifiedCacheEntry_80373588* entry = 0;
    if (lbl_806E2460->field20.FindGet(nlStringLowerHash(filename), &entry))
    {
        filesize = entry->field00;
        datasize = entry->field04;
    }
    else
    {
        datasize = 0;
    }
    if (datasize != 0)
    {
        cached = true;
    }
    else
    {
        file = nlOpen(filename);
        if (file == 0)
        {
            return false;
        }
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
    {
        allocator = CurrentAllocator;
    }
    CurrentAllocator = allocator;
    AllocatorStack[AllocatorStackDepth++] = allocator;
    void* alloc_data;
    if (buffer != 0)
    {
        alloc_data = buffer;
    }
    else if (type == AllocateStart)
    {
        alloc_data = operator new(filesize, alignment, false);
    }
    else if (type == AllocateEnd)
    {
        alloc_data = operator new(filesize, alignment, true);
    }
    else
    {
        alloc_data = operator new(filesize, alignment, false);
    }
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];

    if (cached)
    {
        UnidentifiedAsyncFileLoadData_80372B4C* asyncData = new (8, true)
            UnidentifiedAsyncFileLoadData_80372B4C(0, filename, alloc_data, datasize, filesize, callback, user_data, 0);
        if (!fn_803733DC(lbl_806E2460, filename, alloc_data, datasize, filesize, fn_80372F14, asyncData))
        {
            fn_8004F594(12, "Error calling LoadEntireCachedFileAsync\n");
            delete asyncData;
            return false;
        }
        return true;
    }

    UnidentifiedAsyncFileLoadData_80372B4C* asyncData = new (8, true)
        UnidentifiedAsyncFileLoadData_80372B4C(file, filename, alloc_data, datasize, filesize, callback, user_data, 1);
    ReadAsyncCallback cb = lbl_806E2460->field5D4 ? fn_80372F88 : fn_80373024;
    nlReadAsync(file, alloc_data, datasize, cb, (unsigned long)asyncData, 0);
    return true;
}

extern "C" void fn_80372F14(s32 result, const char*, void*, unsigned int size, void* user_data)
{
    UnidentifiedAsyncFileLoadData_80372B4C* p = (UnidentifiedAsyncFileLoadData_80372B4C*)user_data;
    if ((unsigned int)result == size)
    {
        p->callback(p->alloc_data, p->datasize, p->user_data);
        delete p;
    }
    else
    {
        fn_8004F594(12, "Error %d reading NAND cached file\n", result);
        delete p;
    }
}

extern "C" void fn_80372F88(nlFile*, void* pBuffer, unsigned int, unsigned long uParam)
{
    UnidentifiedAsyncFileLoadData_80372B4C* p = (UnidentifiedAsyncFileLoadData_80372B4C*)uParam;
    nlClose(p->file);
    if (!fn_80373588(lbl_806E2460, p->field04, pBuffer, p->datasize, p->field8C, fn_80373074, p))
    {
        fn_8004F594(12, "Initial WriteEntireFileToCacheAsync failure\n");
        p->callback(p->alloc_data, p->datasize, p->user_data);
        delete p;
    }
}

extern "C" void fn_80373024(nlFile*, void*, unsigned int, unsigned long uParam)
{
    UnidentifiedAsyncFileLoadData_80372B4C* p = (UnidentifiedAsyncFileLoadData_80372B4C*)uParam;
    p->callback(p->alloc_data, p->datasize, p->user_data);
    nlClose(p->file);
    delete p;
}

extern "C" void fn_80373074(s32 result, const char*, void*, unsigned int size, void* user_data)
{
    UnidentifiedAsyncFileLoadData_80372B4C* p = (UnidentifiedAsyncFileLoadData_80372B4C*)user_data;
    if ((unsigned int)result != size)
    {
        fn_8004F594(12, "Failed in AfterWriteToCacheCallback\n");
    }
    p->callback(p->alloc_data, p->datasize, p->user_data);
    delete p;
}
