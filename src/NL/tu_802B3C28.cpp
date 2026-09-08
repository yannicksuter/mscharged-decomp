#include "NL/MemAlloc.h"
#include "NL/UnidentifiedInflateStream_802A99E8.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"


struct AsyncLoadState_802B3C28
{
    AsyncLoadState_802B3C28(nlFile* file, unsigned long param,
        unsigned int compressedSize, LoadAsyncCallback callback, void* userData,
        MemoryAllocator* allocator, int allocType, unsigned int chunkSize,
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
        inflateState.fn_802A9A04();
    }

    unsigned int uncompressedSize;
    nlFile* file;
    void* output;
    unsigned long param;
    unsigned int compressedSize;
    LoadAsyncCallback callback;
    void* userData;
    MemoryAllocator* allocator;
    int allocType;
    unsigned int chunkSize;
    void* readBuffers[2];
    bool ownsReadBuffers;
    unsigned char padding[3];
    int nextRead;
    int completedReads;
    int readCount;
    int fullChunkCount;
    unsigned int finalChunkSize;
    UnidentifiedInflateStream_802A99E8 inflateState;
};

extern "C"
{
    void fn_802B3D30(nlFile*, void*, unsigned int, unsigned long);

    unsigned int lbl_806E1D9C;
}

extern "C" void* fn_802B3C28(void*, unsigned int count, unsigned int size)
{
    return nlMalloc(count * size, 8, false);
}

extern "C" void fn_802B3C38(void*, void* memory)
{
    nlFree(memory);
}

extern "C" void fn_802B3C40(nlFile*, void*, unsigned int, unsigned long userParam)
{
    AsyncLoadState_802B3C28* state = (AsyncLoadState_802B3C28*)userParam;
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

extern "C" void fn_802B3D30(nlFile* file, void* buffer, unsigned int size, unsigned long userParam)
{
    AsyncLoadState_802B3C28* state = (AsyncLoadState_802B3C28*)userParam;

    ++state->completedReads;
    state->inflateState.fn_802A9A58(buffer, size);

    if (state->nextRead < state->fullChunkCount)
    {
        nlReadAsync(file, state->readBuffers[lbl_806E1D9C], state->chunkSize,
            fn_802B3D30, (unsigned long)state, 0);
        lbl_806E1D9C = 1 - lbl_806E1D9C;
        ++state->nextRead;
    }
    else if (state->nextRead < state->readCount)
    {
        nlReadAsync(file, state->readBuffers[lbl_806E1D9C], state->finalChunkSize,
            fn_802B3D30, (unsigned long)state, 0);
        lbl_806E1D9C = 1 - lbl_806E1D9C;
        ++state->nextRead;
    }

    if (state->completedReads == state->readCount)
    {
        state->inflateState.fn_802A9B84();
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

extern "C" bool fn_802B3E94(const char* path, LoadAsyncCallback callback,
    void* userData, unsigned int, int allocType, unsigned int chunkSize,
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

    fn_802A99D8(fn_802B3C28, fn_802B3C38, 0);

    AsyncLoadState_802B3C28* state = new (
        nlMalloc(sizeof(AsyncLoadState_802B3C28), 32, true))
        AsyncLoadState_802B3C28(file, param, compressedSize, callback, userData,
            allocator, allocType, chunkSize, readBuffer0, readBuffer1);

    nlReadAsync(file, state, 4, fn_802B3C40, (unsigned long)state, 0);

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
            nlReadAsync(file, state->readBuffers[lbl_806E1D9C], state->chunkSize,
                fn_802B3D30, (unsigned long)state, 0);
            lbl_806E1D9C = 1 - lbl_806E1D9C;
            ++state->nextRead;
        }
        else if (state->nextRead < state->readCount)
        {
            nlReadAsync(file, state->readBuffers[lbl_806E1D9C], state->finalChunkSize,
                fn_802B3D30, (unsigned long)state, 0);
            lbl_806E1D9C = 1 - lbl_806E1D9C;
            ++state->nextRead;
        }
    }

    return true;
}
