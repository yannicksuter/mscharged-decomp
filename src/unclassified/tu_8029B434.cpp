#include "types.h"

extern u8 lbl_80524C10[];
extern u8 lbl_806DE8AC;

extern "C" void* fn_8029B490(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029B4D0()
{
    if (lbl_806DE8AC != 0)
    {
        return;
    }
    lbl_806DE8AC = 1;
}

extern "C" void fn_8029B4E8()
{
}

extern "C" void* fn_8029B9D8()
{
    return lbl_80524C10;
}
