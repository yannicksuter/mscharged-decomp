#include "NL/nlString.h"

#include "NL/MemAlloc.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"

extern "C" unsigned long strtoul(const char* str, char** endPtr, int base);

void nlStrToWcs(const char* str, unsigned short* wstr, unsigned long maxLen)
{
    unsigned short* dest = wstr;
    unsigned long remaining = maxLen;

    while (remaining-- != 0 && (*dest = (s16)*str) != 0)
    {
        ++dest;
        ++str;
    }
    wstr[maxLen - 1] = 0;
}

void nlWcsToStr(const unsigned short* wstr, char* str, unsigned long maxLen)
{
    unsigned short value;
    unsigned long remaining = maxLen;
    char* dest = str;
    while (remaining != 0)
    {
        value = *wstr;
        *str = value;
        if (value == 0)
        {
            break;
        }
        ++str;
        ++wstr;
        --remaining;
    }
    dest[maxLen - 1] = 0;
}

unsigned long nlWcsToul(const unsigned short* str, unsigned short**, int base)
{
    char buffer[100];
    nlWcsToStr(str, buffer, sizeof(buffer));
    return strtoul(buffer, 0, base);
}

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

private:
    SavedSlotPool mSavedPools[5];
    int mDepth;
    u32 mUnused;
    int mMaxAllocationSize;
};

class TempStringAllocatorStorage
{
public:
    struct Layer
    {
        TempStringAllocatorPool pool;
    };

    Layer allocator;
};

StringBlockAllocator sStringBlockAllocator;
TempStringAllocatorStorage sTempStringAllocatorPool;
} // namespace Detail
