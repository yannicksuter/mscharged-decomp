#include "types.h"

struct State_802A77E8
{
    u8 pad_00[0x20];
    u32 value_20;
};

extern "C" void fn_8036BE88(int, u32);
extern u8 lbl_8052AC08[];

extern "C" void* fn_802A740C(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_802A7464(void*)
{
}

extern "C" void fn_802A77E8(void*, const State_802A77E8* state)
{
    fn_8036BE88(0, state->value_20);
}

extern "C" void* fn_802A77F4()
{
    return lbl_8052AC08;
}
