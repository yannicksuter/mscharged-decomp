#include "NL/nlFile.h"

#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

struct AsyncFileLoadData
{
    nlFile* file;
    void* alloc_data;
    unsigned long datasize;
    LoadAsyncCallback callback;
    void* user_data;

    AsyncFileLoadData(nlFile* const f, void* const alloc, const unsigned long size, LoadAsyncCallback const cb, void* const user)
        : file(f)
        , alloc_data(alloc)
        , datasize(size)
        , callback(cb)
        , user_data(user)
    {
    }
};

nlFile::nlFile()
{
}

nlFile::~nlFile()
{
}

unsigned int nlFileSize(nlFile* file, unsigned int* size)
{
    return file->FileSize(size);
}

void nlRead(nlFile* file, void* buffer, unsigned int size, unsigned long bufferSize)
{
    file->Read(buffer, size, bufferSize);
}

void nlClose(nlFile* file)
{
    delete file;
}

bool nlFileExists(const char* filename)
{
    nlFile* file = nlOpen(filename);
    bool exists = file != 0;
    if (file != 0)
    {
        delete file;
    }
    return exists;
}

void* nlLoadEntireFile(const char* filename, unsigned long* outSize, unsigned int alignment, eAllocType type, void* buffer, unsigned long bufferSize, MemoryAllocator* allocator)
{
    unsigned int filesize;
    unsigned long datasize;
    nlFile* file;
    void* alloc_data = 0;

    file = nlOpen(filename);
    if (file != 0)
    {
        datasize = file->FileSize(&filesize);
        if (datasize != 0)
        {
            if (buffer != 0)
            {
                alloc_data = buffer;
            }
            else
            {
                bufferSize = filesize;
                if (allocator == 0)
                {
                    allocator = CurrentAllocator;
                }
                CurrentAllocator = allocator;
                AllocatorStack[AllocatorStackDepth++] = allocator;

                if (type == AllocateEnd)
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
            }
            file->Read(alloc_data, datasize, bufferSize);
        }

        delete file;

        if (outSize != 0)
        {
            *outSize = datasize;
        }
    }
    return alloc_data;
}

static void nlLoadEntireFileAsyncCallback(nlFile*, void*, unsigned int, unsigned long userParam)
{
    AsyncFileLoadData* data = (AsyncFileLoadData*)userParam;
    data->callback(data->alloc_data, data->datasize, data->user_data);
    delete data->file;
    delete data;
}

unsigned int nlLoadEntireFileAsync(const char* filename, LoadAsyncCallback callback, void* user_data, unsigned int alignment, eAllocType type, void* buffer, unsigned long bufferSize, MemoryAllocator* allocator)
{
    unsigned int filesize;
    unsigned long datasize;
    nlFile* file;
    AsyncFileLoadData* asyncData;
    void* alloc_data;
    bool result;

    file = nlOpen(filename);
    if (file == 0)
    {
        return false;
    }

    datasize = file->FileSize(&filesize);
    result = false;
    if (datasize != 0)
    {
        if (allocator == 0)
        {
            allocator = CurrentAllocator;
        }
        CurrentAllocator = allocator;
        AllocatorStack[AllocatorStackDepth++] = allocator;

        if (buffer != 0)
        {
            filesize = bufferSize;
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

        asyncData = new (nlMalloc(sizeof(AsyncFileLoadData), 8, true)) AsyncFileLoadData(file, alloc_data, datasize, callback, user_data);
        result = nlReadAsync(file, alloc_data, datasize, nlLoadEntireFileAsyncCallback, (unsigned long)asyncData, filesize);
    }
    else
    {
        delete file;
        callback(0, datasize, user_data);
    }
    return result;
}
