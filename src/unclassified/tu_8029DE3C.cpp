#include "types.h"

struct State_8029E300
{
    u8 pad_00[0x20];
    u32 value_20;
};

extern u8 lbl_80525178[];
extern u8 lbl_806DE8E4;
extern "C" void fn_8036BE88(int, u32);

extern "C" void* fn_8029DE98(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029DED8()
{
    if (lbl_806DE8E4 != 0)
    {
        return;
    }
    lbl_806DE8E4 = 1;
}

extern "C" void fn_8029DEF0()
{
}

extern "C" void fn_8029E300(void*, const State_8029E300* state)
{
    fn_8036BE88(0, state->value_20);
}

extern "C" void* fn_8029E30C()
{
    return lbl_80525178;
}
