#include "types.h"

extern u8 lbl_805254F0[];
extern u8 lbl_806DE914;

extern "C" void* fn_802A0228(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_802A0268()
{
    if (lbl_806DE914 != 0)
    {
        return;
    }
    lbl_806DE914 = 1;
}

extern "C" void fn_802A0280()
{
}

extern "C" void* fn_802A0578()
{
    return lbl_805254F0;
}
