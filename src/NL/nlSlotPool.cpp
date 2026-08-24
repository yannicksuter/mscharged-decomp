#include "NL/nlSlotPool.h"

#include "NL/MemAlloc.h"
#include "NL/nlList.h"
#include "NL/nlMemory.h"

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

extern void nlBreak();

static void* DefaultSlotPoolAllocator(unsigned long size)
{
    CurrentAllocator = &VirtualAllocator;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;

    void* result = nlMalloc(size, 8, false);

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    return result;
}

static void DefaultSlotPoolFree(void* data)
{
    CurrentAllocator = &VirtualAllocator;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;

    nlFree(data);

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

SlotPoolBase::SlotPoolBase()
{
    m_BlockList = 0;
    m_FreeList = 0;
    m_AllocFn = DefaultSlotPoolAllocator;
    m_FreeFn = DefaultSlotPoolFree;
    m_Initial = 0;
    m_Delta = 0;
}

SlotPoolBase::~SlotPoolBase()
{
}

extern "C" void fn_802B467C(void*)
{
}

void SlotPoolBase::BaseFreeBlocks(SlotPoolBase* slotPool, unsigned int slotSize)
{
    SlotPoolBlock* nextBlock;
    SlotPoolBlock* currentBlock;
    int blockOffset;

    currentBlock = slotPool->m_BlockList;
    blockOffset = slotSize * slotPool->m_Delta;
    while (currentBlock != 0)
    {
        nextBlock = currentBlock->next;
        if (nextBlock == 0)
        {
            blockOffset = slotSize * slotPool->m_Initial;
        }
        slotPool->m_FreeFn((unsigned char*)currentBlock - blockOffset);
        currentBlock = nextBlock;
    }

    slotPool->m_BlockList = 0;
    slotPool->m_FreeList = 0;
}

void SlotPoolBase::BaseAddNewBlock(SlotPoolBase* slotPool, unsigned int slotSize)
{
    unsigned int totalSlotMemory;
    unsigned int primarySlotCount;
    unsigned int numSlots;
    void* memoryBlock;

    primarySlotCount = slotPool->m_Delta;
    if (primarySlotCount != 0 || slotPool->m_BlockList == 0)
    {
        numSlots = primarySlotCount;
        if (slotPool->m_BlockList == 0)
        {
            numSlots = slotPool->m_Initial;
        }

        totalSlotMemory = slotSize * numSlots;
        memoryBlock = slotPool->m_AllocFn(totalSlotMemory + sizeof(SlotPoolBlock));
        if (memoryBlock == 0)
        {
            nlBreak();
        }

        nlListAddStart<SlotPoolBlock>(
            &slotPool->m_BlockList,
            (SlotPoolBlock*)((unsigned char*)memoryBlock + totalSlotMemory), 0);

        for (unsigned int i = 0; i < numSlots; ++i)
        {
            nlListAddStart<SlotPoolEntry>(
                &slotPool->m_FreeList, (SlotPoolEntry*)memoryBlock, 0);
            memoryBlock = (unsigned char*)memoryBlock + slotSize;
        }
    }
}
