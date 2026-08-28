#include <revolution/gx.h>

#include "NL/gl/glModel.h"

struct State_802A79A0
{
    u8 pad_00[0x0C];
    glModelStream* streams;
};

struct State_802A7C58
{
    u8 pad_00[0x20];
    u32 value_20;
};

extern u8 lbl_8052AC48[];

extern "C" void fn_8036BE88(int, u32);

extern "C" void fn_802A78D8(void*, bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(
        GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(
        GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(
        GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    }
}

extern "C" void fn_802A79A0(void*, const State_802A79A0* state)
{
    glModelStream* streams = state->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 8);
}

extern "C" void fn_802A7C58(void*, const State_802A7C58* state)
{
    fn_8036BE88(0, state->value_20);
}

extern "C" void* fn_802A7C64()
{
    return lbl_8052AC48;
}
