#include "types.h"

extern u8 lbl_80524C60[];
extern u8 lbl_806DE8B4;

extern "C" void* fn_8029BA60(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029BAA0()
{
    if (lbl_806DE8B4 != 0)
    {
        return;
    }
    lbl_806DE8B4 = 1;
}

extern "C" void fn_8029BAB8()
{
}

extern "C" void* fn_8029BC70()
{
    return lbl_80524C60;
}
