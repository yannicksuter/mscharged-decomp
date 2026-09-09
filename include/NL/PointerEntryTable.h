#ifndef NL_POINTER_ENTRY_TABLE_H
#define NL_POINTER_ENTRY_TABLE_H

class TweakNameAllocator;

class PointerEntryTable
{
public:
    struct Entry
    {
        Entry()
            : mPointer(0)
            , mUnidentified004(0)
            , mUnidentified006(0)
        {
        }

        /* 0x00 */ void* mPointer;
        /* 0x04 */ unsigned short mUnidentified004;
        /* 0x06 */ unsigned short mUnidentified006;
    };

    PointerEntryTable(int capacity, TweakNameAllocator* allocator);
    virtual ~PointerEntryTable();
    bool Remove(void* entry);

    /* 0x04 */ TweakNameAllocator* mAllocator;
    /* 0x08 */ Entry* mEntries;
    /* 0x0C */ int mCapacity;
    /* 0x10 */ int mTableIndex;
}; // size: 0x14

#endif // NL_POINTER_ENTRY_TABLE_H
