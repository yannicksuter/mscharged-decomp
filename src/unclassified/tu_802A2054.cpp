#include <revolution/gx.h>

#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"

struct Parameter_802A20F4
{
    u8 pad_00[8];
    float red;
    float green;
    float blue;
    float alpha;
};

extern float lbl_806E5F80;

extern "C" void fn_802A47FC(void*, bool);
extern "C" void fn_802A4894(void*, const glModelPacket*);
extern "C" void fn_802A48DC(void*, const glModelPacket*);
extern "C" void fn_802A4A80(void*, const glModelPacket*);
extern "C" void fn_802A4AF0(void*, const glModelPacket*);

extern "C" void fn_802A2054(void* renderer)
{
    fn_802A47FC(renderer, true);
    gxSetNumChans(0);
    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 0, 0, 255);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
    gxSetTevColourIn(0, 15, 12, 8, 14);
    gxSetTevAlphaIn(0, 7, 6, 4, 6);
}

extern "C" void fn_802A20EC(void*)
{
}

extern "C" void fn_802A20F0(void*, const glModelPacket*)
{
}

extern "C" void fn_802A20F4(void* renderer, const glModelPacket* packet)
{
    Parameter_802A20F4* parameter = static_cast<Parameter_802A20F4*>(packet->unknown20);
    GXColor colour;
    colour.r = static_cast<u8>(lbl_806E5F80 * parameter->red);
    colour.g = static_cast<u8>(lbl_806E5F80 * parameter->green);
    colour.b = static_cast<u8>(lbl_806E5F80 * parameter->blue);
    colour.a = static_cast<u8>(lbl_806E5F80 * parameter->alpha);
    GXSetTevKColor(GX_KCOLOR0, colour);

    fn_802A4894(renderer, packet);
    fn_802A4AF0(renderer, packet);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        fn_802A48DC(renderer, packet);
    }
    else
    {
        fn_802A4A80(renderer, packet);
    }
}
