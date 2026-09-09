#include "NL/PointerEntryTable.h"

#include "NL/nlMemory.h"

static PointerEntryTable* sPointerEntryTables[32];
static int sNextPointerEntryTableIndex;

PointerEntryTable::PointerEntryTable(int capacity, TweakNameAllocator* allocator)
    : mAllocator(allocator)
    , mCapacity(capacity)
    , mTableIndex(-1)
{
    mEntries = new Entry[capacity];
    mTableIndex = sNextPointerEntryTableIndex++;
    sPointerEntryTables[mTableIndex] = this;
}

PointerEntryTable::~PointerEntryTable()
{
    delete[] mEntries;
    sPointerEntryTables[mTableIndex] = 0;
}

bool PointerEntryTable::Remove(void* entry)
{
    for (int i = 0; i < mCapacity; ++i)
    {
        if (mEntries[i].mPointer == entry)
        {
            Entry& value = mEntries[i];
            value.mPointer = 0;
            value.mUnidentified004 = 0;
            return true;
        }
    }
    return false;
}
