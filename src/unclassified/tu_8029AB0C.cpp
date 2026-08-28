#include "types.h"

extern u8 lbl_80524B80[];
extern u8 lbl_806DE89C;

extern "C" void* fn_8029AB68(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029ABA8()
{
    if (lbl_806DE89C != 0)
    {
        return;
    }
    lbl_806DE89C = 1;
}

extern "C" void fn_8029ABC0()
{
}

extern "C" void* fn_8029AF98()
{
    return lbl_80524B80;
}
