#include "types.h"

extern u8 lbl_80524F50[];
extern u8 lbl_806DE8CC;

extern "C" void* fn_8029CA4C(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029CA8C()
{
    if (lbl_806DE8CC != 0)
    {
        return;
    }
    lbl_806DE8CC = 1;
}

extern "C" void fn_8029CAA4()
{
}

extern "C" void* fn_8029D0BC()
{
    return lbl_80524F50;
}
