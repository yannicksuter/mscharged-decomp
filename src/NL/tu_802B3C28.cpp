#include "NL/MemAlloc.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

typedef void* (*InflateAllocCallback)(void*, unsigned int, unsigned int);
typedef void (*InflateFreeCallback)(void*, void*);

struct InflateState_802B3C28
{
    unsigned int sourceSize;
    void* output;
    unsigned int outputSize;
    unsigned int sourceRemaining;
    bool complete;
    unsigned char padding[3];
    unsigned char state[0x38];
};

struct AsyncLoadState_802B3C28
{
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
    InflateState_802B3C28 inflateState;
};

extern "C"
{
    void fn_802A99D8(InflateAllocCallback, InflateFreeCallback, void*);
    void fn_802A99E8(InflateState_802B3C28*, unsigned int, void*, void*);
    bool fn_802A9A04(InflateState_802B3C28*);
    bool fn_802A9A58(InflateState_802B3C28*, void*, unsigned int);
    void fn_802A9B84(InflateState_802B3C28*);

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
    fn_802A9A58(&state->inflateState, buffer, size);

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
        fn_802A9B84(&state->inflateState);
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

    AsyncLoadState_802B3C28* state =
        (AsyncLoadState_802B3C28*)nlMalloc(sizeof(AsyncLoadState_802B3C28), 32, true);
    if (state != 0)
    {
        state->file = file;
        state->output = 0;
        state->param = param;
        state->compressedSize = compressedSize;
        state->callback = callback;
        state->userData = userData;
        state->allocator = allocator;
        state->allocType = allocType;
        state->chunkSize = chunkSize;
        state->nextRead = 0;
        state->completedReads = 0;
        state->readCount = 0;
        state->fullChunkCount = 0;
        state->finalChunkSize = 0;

        fn_802A99E8(&state->inflateState, compressedSize, 0, 0);

        if (readBuffer0 != 0)
        {
            state->readBuffers[0] = readBuffer0;
            state->readBuffers[1] = readBuffer1;
            state->ownsReadBuffers = false;
        }
        else
        {
            state->readBuffers[0] = nlMalloc(chunkSize * 2, 32, true);
            state->readBuffers[1] = (unsigned char*)state->readBuffers[0] + chunkSize;
            state->ownsReadBuffers = true;
        }

        fn_802A9A04(&state->inflateState);
    }

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
