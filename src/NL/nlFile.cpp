#include "NL/nlFile.h"

#include "NL/InflateStream.h"
#include "NL/MemAlloc.h"
#include "NL/nlCompressedFile.h"
#include "NL/nlFileGC.h"
#include "NL/nlMemory.h"


void nlCancelEntireFileLoadCallback(nlFile*, void*, unsigned int, unsigned long, ReadAsyncCallback);
void (*sCancelFileLoadCallback)(void*, unsigned long, void*, LoadAsyncCallback);

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
    unsigned int result;

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
        result = (unsigned int)nlReadAsync(file, alloc_data, datasize, nlLoadEntireFileAsyncCallback, (unsigned long)asyncData, filesize);
    }
    else
    {
        delete file;
        callback(0, datasize, user_data);
    }
    return result;
}

bool nlCancelEntireFileLoad(unsigned int handle, void (*callback)(void*, unsigned long, void*, LoadAsyncCallback))
{
    if (nlAsyncReadBusy((AsyncEntry*)handle))
        return false;

    sCancelFileLoadCallback = callback;
    nlCancelAsyncRead((AsyncEntry*)handle, nlCancelEntireFileLoadCallback);
    sCancelFileLoadCallback = 0;
    return true;
}

void nlCancelEntireFileLoadCallback(nlFile*, void* pBuffer, unsigned int, unsigned long uParam, ReadAsyncCallback)
{
    AsyncFileLoadData* p = (AsyncFileLoadData*)uParam;
    if (sCancelFileLoadCallback != 0)
        sCancelFileLoadCallback(p->alloc_data, p->datasize, p->user_data, p->callback);

    nlFree(pBuffer);
    delete p->file;
    delete p;
}

void OnCompressedFileDataRead(nlFile*, void*, unsigned int, unsigned long);

unsigned int gCompressedFileReadBufferIndex;

struct CompressedFileLoad
{
    CompressedFileLoad(nlFile* file, unsigned long param,
        unsigned int compressedSize, LoadAsyncCallback callback, void* userData,
        MemoryAllocator* allocator, eAllocType allocType, unsigned int chunkSize,
        void* readBuffer0, void* readBuffer1)
        : file(file)
        , output(0)
        , param(param)
        , compressedSize(compressedSize)
        , callback(callback)
        , userData(userData)
        , allocator(allocator)
        , allocType(allocType)
        , chunkSize(chunkSize)
        , nextRead(0)
        , completedReads(0)
        , readCount(0)
        , fullChunkCount(0)
        , finalChunkSize(0)
        , inflateState(compressedSize, 0, 0)
    {
        if (readBuffer0 != 0)
        {
            readBuffers[0] = readBuffer0;
            readBuffers[1] = readBuffer1;
            ownsReadBuffers = false;
        }
        else
        {
            readBuffers[0] = nlMalloc(this->chunkSize * 2, 32, true);
            readBuffers[1] = (unsigned char*)readBuffers[0] + this->chunkSize;
            ownsReadBuffers = true;
        }
        inflateState.Initialize();
    }

    void QueueNextRead(nlFile* file)
    {
        if (nextRead < fullChunkCount)
        {
            nlReadAsync(file, readBuffers[gCompressedFileReadBufferIndex], chunkSize,
                OnCompressedFileDataRead, (unsigned long)this, 0);
            gCompressedFileReadBufferIndex = 1 - gCompressedFileReadBufferIndex;
            ++nextRead;
        }
        else if (nextRead < readCount)
        {
            nlReadAsync(file, readBuffers[gCompressedFileReadBufferIndex], finalChunkSize,
                OnCompressedFileDataRead, (unsigned long)this, 0);
            gCompressedFileReadBufferIndex = 1 - gCompressedFileReadBufferIndex;
            ++nextRead;
        }
    }

    unsigned int uncompressedSize;
    nlFile* file;
    void* output;
    unsigned long param;
    unsigned int compressedSize;
    LoadAsyncCallback callback;
    void* userData;
    MemoryAllocator* allocator;
    eAllocType allocType;
    unsigned int chunkSize;
    void* readBuffers[2];
    bool ownsReadBuffers;
    unsigned char padding[3];
    int nextRead;
    int completedReads;
    int readCount;
    int fullChunkCount;
    int finalChunkSize;
    InflateStream inflateState;
};

void* AllocateInflateMemory(void*, unsigned int count, unsigned int size)
{
    return nlMalloc(count * size, 8, false);
}

void FreeInflateMemory(void*, void* memory)
{
    nlFree(memory);
}

void OnCompressedFileHeaderRead(nlFile*, void*, unsigned int, unsigned long userParam)
{
    CompressedFileLoad* state = (CompressedFileLoad*)userParam;
    MemoryAllocator* allocator = state->allocator;

    AllocatorStack[AllocatorStackDepth++] = allocator;
    CurrentAllocator = allocator;

    if (state->output == 0)
    {
        if (state->allocType == AllocateStart)
        {
            state->output = nlMalloc(state->uncompressedSize, 32, false);
        }
        else if (state->allocType == AllocateEnd)
        {
            state->output = nlMalloc(state->uncompressedSize, 32, true);
        }
        else
        {
            state->output = nlMalloc(state->uncompressedSize, 32, false);
        }
    }

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];

    unsigned int uncompressedSize = state->uncompressedSize;
    state->inflateState.output = state->output;
    state->inflateState.outputSize = uncompressedSize;
}

void OnCompressedFileDataRead(nlFile* file, void* buffer, unsigned int size, unsigned long userParam)
{
    CompressedFileLoad* state = (CompressedFileLoad*)userParam;

    ++state->completedReads;
    state->inflateState.Process(buffer, size);
    state->QueueNextRead(file);

    if (state->completedReads == state->readCount)
    {
        state->inflateState.Finish();
        state->callback(state->output, state->uncompressedSize, state->userData);
        delete state->file;

        if (state != 0)
        {
            if (state->ownsReadBuffers)
            {
                nlFree(state->readBuffers[0]);
            }
            delete state;
        }
    }
}

bool nlLoadCompressedFileAsync(const char* path, LoadAsyncCallback callback,
    void* userData, unsigned int, eAllocType allocType, unsigned int chunkSize,
    void* readBuffer0, void* readBuffer1, void*, unsigned long param,
    MemoryAllocator* allocator)
{
    nlFile* file = nlOpen(path);
    if (file == 0)
    {
        return false;
    }

    unsigned int ignoredSize;
    unsigned int compressedSize = file->FileSize(&ignoredSize);
    if (compressedSize <= 4)
    {
        delete file;
        return false;
    }

    if (allocator == 0)
    {
        allocator = CurrentAllocator;
    }

    InflateStream::SetMemoryCallbacks(AllocateInflateMemory, FreeInflateMemory, 0);

    CompressedFileLoad* state = new (
        nlMalloc(sizeof(CompressedFileLoad), 32, true))
        CompressedFileLoad(file, param, compressedSize, callback, userData,
            allocator, allocType, chunkSize, readBuffer0, readBuffer1);

    nlReadAsync(file, state, 4, OnCompressedFileHeaderRead, (unsigned long)state, 0);

    unsigned int payloadSize = compressedSize - 4;
    state->fullChunkCount = payloadSize / chunkSize;
    state->finalChunkSize = payloadSize - state->fullChunkCount * chunkSize;
    state->readCount = state->fullChunkCount;
    if (state->finalChunkSize > 0)
    {
        ++state->readCount;
    }

    state->QueueNextRead(file);
    state->QueueNextRead(file);

    return true;
}
