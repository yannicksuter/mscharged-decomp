#include "types.h"

extern u8 lbl_80525268[];
extern u8 lbl_806DE8F4;

extern "C" void* fn_8029E954(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029E994()
{
    if (lbl_806DE8F4 != 0)
    {
        return;
    }
    lbl_806DE8F4 = 1;
}

extern "C" void fn_8029E9AC()
{
}

extern "C" void* fn_8029EF28()
{
    return lbl_80525268;
}
