#include "types.h"

extern u8 lbl_80525200[];
extern u8 lbl_806DE8EC;

extern "C" void* fn_8029E394(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029E3D4()
{
    if (lbl_806DE8EC != 0)
    {
        return;
    }
    lbl_806DE8EC = 1;
}

extern "C" void fn_8029E3EC()
{
}

extern "C" void* fn_8029E8CC()
{
    return lbl_80525200;
}
