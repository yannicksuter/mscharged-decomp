#ifndef NL_SMALL_BLOCK_ALLOCATOR_H
#define NL_SMALL_BLOCK_ALLOCATOR_H

#include "NL/nlSlotPool.h"

int nlPrintf(const char*, ...);

template <int kBlockSize>
class nlSlotPoolFixed : public SlotPoolBase
{
public:
    struct SavedState
    {
        SlotPoolBlock* block;
        SlotPoolEntry* free;

        SavedState()
            : block(0)
            , free(0)
        {
        }
    };

    nlSlotPoolFixed(int count = 16)
        : SlotPoolBase()
    {
        m_Depth = 0;
        m_Delta = count;
        m_Initial = count;
        if (m_Initial == 0)
        {
            SlotPoolBase::BaseAddNewBlock(this, kBlockSize);
        }
    }

    ~nlSlotPoolFixed()
    {
        while (m_Depth > 0)
        {
            FreeBlocks();
            PopState();
        }
        FreeBlocks();
    }

    void FreeBlocks()
    {
        fn_802B467C(this);
        SlotPoolBase::BaseFreeBlocks(this, kBlockSize);
    }

    void PushState()
    {
        m_States[m_Depth].block = m_BlockList;
        m_States[m_Depth].free = m_FreeList;
        m_Depth++;
        m_BlockList = 0;
        m_FreeList = 0;
    }

    void PopState()
    {
        fn_802B467C(this);
        int index = m_Depth - 1;
        m_BlockList = m_States[index].block;
        m_FreeList = m_States[index].free;
        m_States[index].block = 0;
        m_States[index].free = 0;
        m_Depth = m_Depth - 1;
    }

    void* Allocate()
    {
        void* result = 0;
        if (m_FreeList == 0)
        {
            SlotPoolBase::BaseAddNewBlock(this, kBlockSize);
        }
        if (m_FreeList != 0)
        {
            result = m_FreeList;
            m_FreeList = m_FreeList->next;
        }
        return result;
    }

    template <typename T>
    void Allocate(T*& out)
    {
        out = 0;
        if (m_FreeList == 0)
        {
            SlotPoolBase::BaseAddNewBlock(this, kBlockSize);
        }
        if (m_FreeList != 0)
        {
            out = (T*)m_FreeList;
            m_FreeList = m_FreeList->next;
        }
    }

    void Free(void* entry)
    {
        SlotPoolEntry* slot = (SlotPoolEntry*)entry;
        slot->next = m_FreeList;
        m_FreeList = slot;
    }

    SavedState m_States[5];
    int m_Depth;
}; // size: 0x44

template <int kSize1, int kSize2, int kSize3, int kSize4>
class nlSmallBlockAllocator;

template <int kSize1, int kSize2, int kSize3>
class nlSmallBlockAllocator<kSize1, kSize2, kSize3, 1>
{
public:
    nlSmallBlockAllocator()
    {
        m_MaxBlockSize = kSize1;
        if (m_MaxBlockSize < kSize2)
        {
            m_MaxBlockSize = kSize2;
        }
        if (m_MaxBlockSize < kSize3)
        {
            m_MaxBlockSize = kSize3;
        }
        if (m_MaxBlockSize < 1)
        {
            m_MaxBlockSize = 1;
        }
    }

    void* Allocate(unsigned long size)
    {
        void* result = 0;
        if (size <= kSize1)
        {
            result = m_Pool1.Allocate();
        }
        else if (size <= kSize2)
        {
            result = m_Pool2.Allocate();
        }
        else if (size <= kSize3)
        {
            result = m_Pool3.Allocate();
        }
        if (result == 0)
        {
            nlPrintf("SBA %s: Allocator returned NULL for size %d, blocks are %d %d %d %d\n",
                __FILE__,
                size,
                kSize1,
                kSize2,
                kSize3,
                1);
        }
        return result;
    }

    nlSlotPoolFixed<kSize1> m_Pool1;
    nlSlotPoolFixed<kSize2> m_Pool2;
    nlSlotPoolFixed<kSize3> m_Pool3;
    int m_Unidentified;
    int m_MaxBlockSize;
};

template <int kSize1, int kSize2>
class nlSmallBlockAllocator<kSize1, kSize2, 1, 1>
{
public:
    nlSmallBlockAllocator()
    {
        m_MaxBlockSize = kSize1;
        if (m_MaxBlockSize < kSize2)
        {
            m_MaxBlockSize = kSize2;
        }
        if (m_MaxBlockSize < 1)
        {
            m_MaxBlockSize = 1;
        }
        if (m_MaxBlockSize < 1)
        {
            m_MaxBlockSize = 1;
        }
    }

    void* Allocate(unsigned long size)
    {
        void* result = 0;
        if (size <= kSize1)
        {
            result = m_Pool1.Allocate();
        }
        else if (size <= kSize2)
        {
            result = m_Pool2.Allocate();
        }
        if (result == 0)
        {
            nlPrintf("SBA %s: Allocator returned NULL for size %d, blocks are %d %d %d %d\n",
                __FILE__,
                size,
                kSize1,
                kSize2,
                1,
                1);
        }
        return result;
    }

    nlSlotPoolFixed<kSize1> m_Pool1;
    nlSlotPoolFixed<kSize2> m_Pool2;
    int m_Unidentified;
    int m_MaxBlockSize;
};

#endif // NL_SMALL_BLOCK_ALLOCATOR_H
