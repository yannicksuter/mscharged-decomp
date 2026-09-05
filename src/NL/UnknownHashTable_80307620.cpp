#include "NL/UnknownHashTable_80307620.h"

#include "NL/nlMemory.h"

static UnknownHashTable_80307620* lbl_80584048[32];
static int lbl_806E2098;

UnknownHashTable_80307620::UnknownHashTable_80307620(int capacity, TweakNameAllocator_8052BE68* allocator)
    : mUnidentified004(allocator)
    , mUnidentified00C(capacity)
    , mUnidentified010(-1)
{
    mUnidentified008 = new UnidentifiedEntry[capacity];
    mUnidentified010 = lbl_806E2098++;
    lbl_80584048[mUnidentified010] = this;
}

UnknownHashTable_80307620::~UnknownHashTable_80307620()
{
    delete[] mUnidentified008;
    lbl_80584048[mUnidentified010] = 0;
}

bool UnknownHashTable_80307620::fn_80307748(void* entry)
{
    for (int i = 0; i < mUnidentified00C; ++i)
    {
        if (mUnidentified008[i].mUnidentified000 == entry)
        {
            UnidentifiedEntry& value = mUnidentified008[i];
            value.mUnidentified000 = 0;
            value.mUnidentified004 = 0;
            return true;
        }
    }
    return false;
}
