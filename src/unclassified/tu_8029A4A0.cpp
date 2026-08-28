#include "types.h"

extern u8 lbl_80524B28[];
extern u8 lbl_806DE894;

extern "C" void* fn_8029A4FC(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029A53C()
{
    if (lbl_806DE894 != 0)
    {
        return;
    }
    lbl_806DE894 = 1;
}

extern "C" void fn_8029A554()
{
}

extern "C" void* fn_8029AAE0()
{
    return lbl_80524B28;
}
