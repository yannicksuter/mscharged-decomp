#ifndef NL_UNKNOWN_HASH_TABLE_80307620_H
#define NL_UNKNOWN_HASH_TABLE_80307620_H

class TweakNameAllocator_8052BE68;

class UnknownHashTable_80307620
{
public:
    struct UnidentifiedEntry
    {
        UnidentifiedEntry()
            : mUnidentified000(0)
            , mUnidentified004(0)
            , mUnidentified006(0)
        {
        }

        /* 0x00 */ void* mUnidentified000;
        /* 0x04 */ unsigned short mUnidentified004;
        /* 0x06 */ unsigned short mUnidentified006;
    };

    UnknownHashTable_80307620(int capacity, TweakNameAllocator_8052BE68* allocator);
    virtual ~UnknownHashTable_80307620();
    bool fn_80307748(void* entry);

    /* 0x04 */ TweakNameAllocator_8052BE68* mUnidentified004;
    /* 0x08 */ UnidentifiedEntry* mUnidentified008;
    /* 0x0C */ int mUnidentified00C;
    /* 0x10 */ int mUnidentified010;
}; // size: 0x14

#endif // NL_UNKNOWN_HASH_TABLE_80307620_H
