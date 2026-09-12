#ifndef NL_SMALL_BLOCK_ALLOCATOR_H
#define NL_SMALL_BLOCK_ALLOCATOR_H

#include "NL/nlPrint.h"

#include "NL/nlSlotPool.h"
#include "NL/nlPrint.h"

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

    nlSlotPoolFixed(int initial, int delta)
        : SlotPoolBase()
    {
        m_Depth = 0;
        m_Initial = initial;
        SlotPoolBase::BaseAddNewBlock(this, kBlockSize);
        m_Delta = delta;
    }

    ~nlSlotPoolFixed()
    {
        if (this != 0)
        {
            while (m_Depth > 0)
            {
                FreeBlocks();
                PopState();
            }
            FreeBlocks();
        }
    }

    void FreeBlocks()
    {
        fn_802B467C(this);
        SlotPoolBase::BaseFreeBlocks(this, kBlockSize);
    }

    void PushState()
    {
        SavedState& state = m_States[m_Depth];
        state.block = m_BlockList;
        state.free = m_FreeList;
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

    template <typename T>
    void Delete(T* entry)
    {
        entry->~T();
        Free(entry);
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

namespace Detail
{
struct SavedSlotPool
{
    SavedSlotPool()
        : blockList(0)
        , freeList(0)
    {
    }

    SlotPoolBlock* blockList;
    SlotPoolEntry* freeList;
};

class TempStringSlotPool : public SlotPoolBase
{
public:
    TempStringSlotPool()
        : SlotPoolBase()
    {
    }

    ~TempStringSlotPool()
    {
        fn_802B467C(this);
        BaseFreeBlocks(this, 0x40);
    }
};

class TempStringAllocatorPool : public TempStringSlotPool
{
public:
    TempStringAllocatorPool()
        : TempStringSlotPool()
        , mDepth(0)
    {
        m_Delta = 0x10;
        m_Initial = 0x10;
        if (m_Initial == 0)
        {
            BaseAddNewBlock(this, 0x40);
        }

        mMaxAllocationSize = 0x40;
        if (mMaxAllocationSize < 0x8000)
        {
            mMaxAllocationSize = 0x8000;
        }
        if (mMaxAllocationSize < 1)
        {
            mMaxAllocationSize = 1;
        }
        if (mMaxAllocationSize < 1)
        {
            mMaxAllocationSize = 1;
        }
    }

    ~TempStringAllocatorPool()
    {
        while (mDepth > 0)
        {
            fn_802B467C(this);
            BaseFreeBlocks(this, 0x40);
            fn_802B467C(this);

            SavedSlotPool& saved = mSavedPools[mDepth - 1];
            m_BlockList = saved.blockList;
            m_FreeList = saved.freeList;
            saved.blockList = 0;
            saved.freeList = 0;
            --mDepth;
        }
    }

    void PushState()
    {
        SavedSlotPool& saved = mSavedPools[mDepth];
        saved.blockList = m_BlockList;
        saved.freeList = m_FreeList;
        ++mDepth;
        m_BlockList = 0;
        m_FreeList = 0;
    }

    void* Allocate(unsigned long size)
    {
        void* result = 0;
        if (size <= 0x40)
        {
            if (m_FreeList == 0)
            {
                BaseAddNewBlock(this, 0x40);
            }
            if (m_FreeList != 0)
            {
                result = m_FreeList;
                m_FreeList = m_FreeList->next;
            }
        }
        else if (size <= 0x8000)
        {
            result = nlMalloc(0x8000, 8, false);
        }
        else if (size <= 1)
        {
            result = 0;
        }
        else if (size <= 1)
        {
            result = 0;
        }
        else
        {
            nlPrintf("SBA %s: Trying to alloc %d bytes, blocks are %d %d %d %d\n",
                __FILE__,
                size,
                0x40,
                0x8000,
                1,
                1);
            result = 0;
        }
        if (result == 0)
        {
            nlPrintf("SBA %s: Allocator returned NULL for size %d, blocks are %d %d %d %d\n",
                __FILE__,
                size,
                0x40,
                0x8000,
                1,
                1);
        }
        return result;
    }

    void Free(void* ptr, unsigned long size)
    {
        if (size <= 0x40)
        {
            SlotPoolEntry* slot = (SlotPoolEntry*)ptr;
            slot->next = m_FreeList;
            m_FreeList = slot;
        }
        else if (size <= 0x8000)
        {
            nlFree(ptr);
        }
        else if (size <= 1)
        {
        }
        else if (size <= 1)
        {
        }
        else
        {
            nlPrintf("SBA %s: Trying to free %d bytes, blocks are %d %d %d %d\n",
                __FILE__,
                size,
                0x40,
                0x8000,
                1,
                1);
        }
    }

private:
    SavedSlotPool mSavedPools[5];
    int mDepth;
    u32 mUnused;
    int mMaxAllocationSize;
};
} // namespace Detail

#endif // NL_SMALL_BLOCK_ALLOCATOR_H
