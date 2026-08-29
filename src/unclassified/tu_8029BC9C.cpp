#include "types.h"

extern u8 lbl_80524E48[];
extern u8 lbl_806DE8BC;

extern "C" void* fn_8029BCF8(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029BD38()
{
    if (lbl_806DE8BC != 0)
    {
        return;
    }
    lbl_806DE8BC = 1;
}

extern "C" void fn_8029BD50()
{
}

extern "C" void* fn_8029C2CC()
{
    return lbl_80524E48;
}
