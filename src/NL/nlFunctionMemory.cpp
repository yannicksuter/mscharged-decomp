#include "NL/nlFunctionMemory.h"

#include "NL/nlSmallBlockAllocator.h"

template <int BlockSize>
class UnidentifiedFunctionMemoryPool : public UnidentifiedSlotPoolFixedState<BlockSize>
{
public:
    UnidentifiedFunctionMemoryPool(const int initial, const int delta)
    {
        this->Initialize(initial, delta);
    }
};

UnidentifiedFunctionMemoryPool<0x10> gFunctionMemoryPool16(0x40, 0);
UnidentifiedFunctionMemoryPool<0x20> gFunctionMemoryPool32(0x200, 0);
UnidentifiedFunctionMemoryPool<0x40> gFunctionMemoryPool64(0x40, 0);

void PushFunctionMemoryState()
{
    gFunctionMemoryPool16.PushState();
    gFunctionMemoryPool32.PushState();
    gFunctionMemoryPool64.PushState();
}

void InitializeFunctionMemory()
{
}

void FreeFunctionMemoryPools()
{
    gFunctionMemoryPool16.FreeBlocks();
    gFunctionMemoryPool32.FreeBlocks();
    gFunctionMemoryPool64.FreeBlocks();
}

void PopFunctionMemoryState()
{
    gFunctionMemoryPool16.PopState();
    gFunctionMemoryPool32.PopState();
    gFunctionMemoryPool64.PopState();
}

void* AllocateFunctionMemory(unsigned long size)
{
    void* result;

    if (size <= 0x10)
    {
        result = gFunctionMemoryPool16.Allocate();
    }
    else if (size <= 0x20)
    {
        result = gFunctionMemoryPool32.Allocate();
    }
    else
    {
        result = gFunctionMemoryPool64.Allocate();
    }

    return result;
}

void FreeFunctionMemory(void* entry, unsigned long size)
{
    if (size <= 0x10)
    {
        gFunctionMemoryPool16.Free(entry);
    }
    else if (size <= 0x20)
    {
        gFunctionMemoryPool32.Free(entry);
    }
    else
    {
        gFunctionMemoryPool64.Free(entry);
    }
}
