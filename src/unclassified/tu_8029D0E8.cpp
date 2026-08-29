#include "types.h"

extern u8 lbl_80525020[];
extern u8 lbl_806DE8D4;

extern "C" void* fn_8029D144(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029D184()
{
    if (lbl_806DE8D4 != 0)
    {
        return;
    }
    lbl_806DE8D4 = 1;
}

extern "C" void fn_8029D19C()
{
}

extern "C" void* fn_8029D7B4()
{
    return lbl_80525020;
}
