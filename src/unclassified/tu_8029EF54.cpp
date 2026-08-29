#include "types.h"

extern u8 lbl_80525300[];
extern u8 lbl_806DE8FC;

extern "C" void* fn_8029EFB0(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029EFF0()
{
    if (lbl_806DE8FC != 0)
    {
        return;
    }
    lbl_806DE8FC = 1;
}

extern "C" void fn_8029F008()
{
}

extern "C" void* fn_8029F584()
{
    return lbl_80525300;
}
