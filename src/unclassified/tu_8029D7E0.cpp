#include "types.h"

extern u8 lbl_805250C0[];
extern u8 lbl_806DE8DC;

extern "C" void* fn_8029D83C(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029D87C()
{
    if (lbl_806DE8DC != 0)
    {
        return;
    }
    lbl_806DE8DC = 1;
}

extern "C" void fn_8029D894()
{
}

extern "C" void* fn_8029DE10()
{
    return lbl_805250C0;
}
