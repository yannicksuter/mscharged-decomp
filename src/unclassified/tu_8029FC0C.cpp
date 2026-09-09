#include "NL/glx/glxTexture.h"
#include "types.h"

struct State_802A015C
{
    u8 pad_00[0x20];
    u32 value_20;
};

extern u8 lbl_80525450[];
extern u8 lbl_806DE90C;

extern "C" void* fn_8029FC68(void* state, int shouldDelete)
{
    if (state != 0 && shouldDelete > 0)
    {
        ::operator delete(state);
    }
    return state;
}

extern "C" void fn_8029FCA8()
{
    if (lbl_806DE90C != 0)
    {
        return;
    }
    lbl_806DE90C = 1;
}

extern "C" void fn_8029FCC0()
{
}

extern "C" void fn_802A015C(void*, const State_802A015C* state)
{
    glx_BindTexture(0, (glTextureBinding*)(state->value_20));
    glx_BindTexture(1, (glTextureBinding*)(state->value_20 + 8));
}

extern "C" void* fn_802A01A0()
{
    return lbl_80525450;
}
