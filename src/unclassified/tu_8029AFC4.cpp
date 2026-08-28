#include "types.h"

struct State_8029B3FC
{
    u8 pad_00[0x20];
    u32 value_20;
};

extern u8 lbl_80524BD0[];
extern u8 lbl_806DE8A4;
extern "C" void fn_8036BE88(int, u32);

extern "C" void* fn_8029B020(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029B060()
{
    if (lbl_806DE8A4 != 0)
    {
        return;
    }
    lbl_806DE8A4 = 1;
}

extern "C" void fn_8029B078()
{
}

extern "C" void fn_8029B3FC(void*, const State_8029B3FC* state)
{
    fn_8036BE88(0, state->value_20);
}

extern "C" void* fn_8029B408()
{
    return lbl_80524BD0;
}
