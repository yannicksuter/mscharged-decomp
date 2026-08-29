#include "types.h"

struct State_802A09DC
{
    u8 pad_00[0x20];
    u32 value_20;
};

extern u8 lbl_80525530[];
extern u8 lbl_806DE91C;
extern "C" void fn_8036BE88(int, u32);

extern "C" void* fn_802A0600(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_802A0640()
{
    if (lbl_806DE91C != 0)
    {
        return;
    }
    lbl_806DE91C = 1;
}

extern "C" void fn_802A0658()
{
}

extern "C" void fn_802A09DC(void*, const State_802A09DC* state)
{
    fn_8036BE88(0, state->value_20);
}

extern "C" void* fn_802A09E8()
{
    return lbl_80525530;
}
