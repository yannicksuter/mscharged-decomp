#include "types.h"

extern u8 lbl_80525398[];
extern u8 lbl_806DE904;

extern "C" void* fn_8029F60C(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029F64C()
{
    if (lbl_806DE904 != 0)
    {
        return;
    }
    lbl_806DE904 = 1;
}

extern "C" void fn_8029F664()
{
}

extern "C" void* fn_8029FBE0()
{
    return lbl_80525398;
}
