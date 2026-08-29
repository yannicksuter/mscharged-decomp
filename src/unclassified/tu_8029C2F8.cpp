#include "types.h"

extern u8 lbl_80524EB8[];
extern u8 lbl_806DE8C4;

extern "C" void* fn_8029C354(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029C394()
{
    if (lbl_806DE8C4 != 0)
    {
        return;
    }
    lbl_806DE8C4 = 1;
}

extern "C" void fn_8029C3AC()
{
}

extern "C" void* fn_8029C9C4()
{
    return lbl_80524EB8;
}
