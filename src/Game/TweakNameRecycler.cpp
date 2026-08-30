#include "Game/TweakRegistry.h"

#include "NL/nlMemory.h"
#include "NL/nlSmallBlockAllocator.h"

class TweakNameAllocator_8052BE68
{
public:
    virtual void* UnidentifiedVirtual08();
    virtual void UnidentifiedVirtual0C(void* ptr);
};

class UnknownHashTable_80307620
{
public:
    UnknownHashTable_80307620(int capacity, TweakNameAllocator_8052BE68* allocator);
    virtual ~UnknownHashTable_80307620();

    /* 0x04 */ u8 m_Unk4[0x14];
}; // size: 0x18

class TweakNameTable_8052BE58 : public UnknownHashTable_80307620
{
public:
    TweakNameTable_8052BE58(int capacity, TweakNameAllocator_8052BE68* allocator)
        : UnknownHashTable_80307620(capacity, allocator)
    {
    }
    virtual ~TweakNameTable_8052BE58();
};

extern "C" void fn_80307748(void* table, TweakRecycledName* entry);

nlSlotPoolFixed<0x10> lbl_8057C66C;

TweakNameTable_8052BE58::~TweakNameTable_8052BE58()
{
}

void* TweakNameAllocator_8052BE68::UnidentifiedVirtual08()
{
    if (fn_802C0F04() != 0)
    {
        return lbl_8057C66C.Allocate();
    }
    return nlMalloc(0x10, 8, false);
}

void TweakNameAllocator_8052BE68::UnidentifiedVirtual0C(void* ptr)
{
    nlFree(ptr);
}

void fn_802C3970(void)
{
    TweakRecycledName* entry = lbl_806E1E68;
    TweakRecycledName* next = entry == 0 ? 0 : entry->m_Next;
    while (entry != 0)
    {
        static TweakNameAllocator_8052BE68 sRecycledNameAllocator;
        static TweakNameTable_8052BE58 sRecycledNameTable(2000, &sRecycledNameAllocator);
        fn_80307748(&sRecycledNameTable, entry);
        lbl_8057C66C.Free(entry);
        entry = next;
        next = entry == 0 ? 0 : entry->m_Next;
    }
    lbl_806E1E68 = 0;
    lbl_806E1E6C = 0;
}

extern "C"
{
    TweakRecycledName* lbl_806E1E68;
    TweakRecycledName* lbl_806E1E6C;
}
