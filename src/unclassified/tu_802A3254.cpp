#include <revolution/gx.h>

#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"

extern u8 lbl_806E5FB0;
extern u8 lbl_806E5FB1;
extern u8 lbl_806E5FB2;
extern u8 lbl_806E5FB3;

extern "C" void fn_802A68FC(void*, bool);
extern "C" void fn_802A6964(void*, const glModelPacket*);
extern "C" void fn_802A6978(void*, const glModelPacket*);
extern "C" void fn_802A6AD4(void*, const glModelPacket*);
extern "C" void fn_802A6B40(void*, const glModelPacket*);

extern "C" void fn_802A3254(void* renderer)
{
    gxSetNumChans(1);
    gxSetNumTexGens(0);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 255, 255, 4);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
    gxSetTevColourIn(0, 15, 12, 14, 15);
    gxSetTevAlphaIn(0, 7, 6, 6, 7);
    fn_802A68FC(renderer, true);
}

extern "C" void fn_802A32FC(void*)
{
}

extern "C" void fn_802A3300(void*, const glModelPacket*)
{
}

extern "C" void fn_802A3304(void* renderer, const glModelPacket* packet)
{
    GXColor colour;
    colour.r = lbl_806E5FB0;
    colour.g = lbl_806E5FB1;
    colour.b = lbl_806E5FB2;
    colour.a = lbl_806E5FB3;
    GXSetTevKColor(GX_KCOLOR0, colour);

    fn_802A6964(renderer, packet);
    fn_802A6B40(renderer, packet);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        fn_802A6978(renderer, packet);
    }
    else
    {
        fn_802A6AD4(renderer, packet);
    }
}
