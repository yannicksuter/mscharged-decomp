#include "NL/nlSmallBlockAllocator.h"

nlSlotPoolFixed<0x10> lbl_8057B57C(0x40, 0);
nlSlotPoolFixed<0x20> lbl_8057B5CC(0x200, 0);
nlSlotPoolFixed<0x40> lbl_8057B61C(0x40, 0);

extern "C" void fn_802B1A30()
{
    lbl_8057B57C.PushState();
    lbl_8057B5CC.PushState();
    lbl_8057B61C.PushState();
}

extern "C" void fn_802B1AE0()
{
}

extern "C" void fn_802B1AE4()
{
    lbl_8057B57C.FreeBlocks();
    lbl_8057B5CC.FreeBlocks();
    lbl_8057B61C.FreeBlocks();
}

extern "C" void fn_802B1B50()
{
    lbl_8057B57C.PopState();
    lbl_8057B5CC.PopState();
    lbl_8057B61C.PopState();
}

extern "C" void* fn_802B1C4C(unsigned long size)
{
    void* result;

    if (size <= 0x10)
    {
        result = lbl_8057B57C.Allocate();
    }
    else if (size <= 0x20)
    {
        result = lbl_8057B5CC.Allocate();
    }
    else
    {
        result = lbl_8057B61C.Allocate();
    }

    return result;
}

extern "C" void fn_802B1D4C(void* entry, unsigned long size)
{
    if (size <= 0x10)
    {
        lbl_8057B57C.Free(entry);
    }
    else if (size <= 0x20)
    {
        lbl_8057B5CC.Free(entry);
    }
    else
    {
        lbl_8057B61C.Free(entry);
    }
}
