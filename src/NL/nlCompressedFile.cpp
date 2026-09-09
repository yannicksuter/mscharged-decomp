#include "NL/nlCompressedFile.h"

#include "NL/MemAlloc.h"
#include "NL/InflateStream.h"
#include "NL/nlMemory.h"

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
            readBuffers[0] = nlMalloc(chunkSize * 2, 32, true);
            readBuffers[1] = (unsigned char*)readBuffers[0] + chunkSize;
            ownsReadBuffers = true;
        }
        inflateState.Initialize();
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
    unsigned int finalChunkSize;
    InflateStream inflateState;
};

void OnCompressedFileDataRead(nlFile*, void*, unsigned int, unsigned long);

unsigned int gCompressedFileReadBufferIndex;

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

    if (state->nextRead < state->fullChunkCount)
    {
        nlReadAsync(file, state->readBuffers[gCompressedFileReadBufferIndex], state->chunkSize,
            OnCompressedFileDataRead, (unsigned long)state, 0);
        gCompressedFileReadBufferIndex = 1 - gCompressedFileReadBufferIndex;
        ++state->nextRead;
    }
    else if (state->nextRead < state->readCount)
    {
        nlReadAsync(file, state->readBuffers[gCompressedFileReadBufferIndex], state->finalChunkSize,
            OnCompressedFileDataRead, (unsigned long)state, 0);
        gCompressedFileReadBufferIndex = 1 - gCompressedFileReadBufferIndex;
        ++state->nextRead;
    }

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

    for (int i = 0; i < 2; ++i)
    {
        if (state->nextRead < state->fullChunkCount)
        {
            nlReadAsync(file, state->readBuffers[gCompressedFileReadBufferIndex], state->chunkSize,
                OnCompressedFileDataRead, (unsigned long)state, 0);
            gCompressedFileReadBufferIndex = 1 - gCompressedFileReadBufferIndex;
            ++state->nextRead;
        }
        else if (state->nextRead < state->readCount)
        {
            nlReadAsync(file, state->readBuffers[gCompressedFileReadBufferIndex], state->finalChunkSize,
                OnCompressedFileDataRead, (unsigned long)state, 0);
            gCompressedFileReadBufferIndex = 1 - gCompressedFileReadBufferIndex;
            ++state->nextRead;
        }
    }

    return true;
}
