#ifndef NL_TU_80372B4C_H
#define NL_TU_80372B4C_H

#include "NL/nlAVLTree.h"
#include "NL/nlFile.h"
#include "NL/nlTask.h"

typedef void (*UnidentifiedCacheCallback_80372F14)(s32, const char*, void*, unsigned int, void*);

struct UnidentifiedCacheEntry_80373588
{
    /* 0x00 */ unsigned int field00;
    /* 0x04 */ unsigned int field04;
    /* 0x08 */ char field08[128];
    /* 0x88 */ char field88[16];
}; // size 0x98

struct UnidentifiedCacheRequest_803734A0
{
    /* 0x00 */ unsigned int field00;
    /* 0x04 */ void* field04;
    /* 0x08 */ UnidentifiedCacheCallback_80372F14 field08;
    /* 0x0C */ UnidentifiedCacheCallback_80372F14 field0C;
    /* 0x10 */ unsigned int field10;
    /* 0x14 */ void* field14;
    /* 0x18 */ UnidentifiedCacheEntry_80373588* field18;
}; // size 0x1C

class UnidentifiedTask_803730D8 : public nlTask
{
public:
    virtual void Run(float dt);
    virtual const char* GetName();

    /* 0x020 */ nlAVLTreeSlotPool<unsigned int, UnidentifiedCacheEntry_80373588, DefaultKeyCompare<unsigned int> > field20;
    /* 0x044 */ unsigned int field44;
    /* 0x048 */ unsigned int field48;
    /* 0x04C */ UnidentifiedCacheRequest_803734A0* field4C;
    /* 0x050 */ unsigned int field50;
    /* 0x054 */ unsigned int field54;
    /* 0x058 */ unsigned int field58;
    /* 0x05C */ UnidentifiedCacheRequest_803734A0 field5C[50];
    /* 0x5D4 */ bool field5D4;
}; // size 0x5D8

extern "C" bool fn_80372B4C(const char* filename, LoadAsyncCallback callback, void* user_data,
    unsigned int alignment, eAllocType type, void* buffer, unsigned long bufferSize, MemoryAllocator* allocator);

#endif // NL_TU_80372B4C_H
