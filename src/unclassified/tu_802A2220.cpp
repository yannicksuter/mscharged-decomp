#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"

struct Parameter_802A22C8
{
    u8 pad_00[8];
    float red;
    float green;
    float blue;
    float alpha;
};

struct GXColor_802A22C8
{
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

extern float lbl_806E5F88;

extern "C" void GXSetTevKColorSel(int, int);
extern "C" void GXSetTevKAlphaSel(int, int);
extern "C" void GXSetTevKColor(int, GXColor_802A22C8);
extern "C" void GXCallDisplayList(const void*, u32);
extern "C" void fn_802A4418(void*, bool);
extern "C" void fn_802A44B0(void*, const glModelPacket*);
extern "C" void fn_802A44F8(void*, const glModelPacket*);
extern "C" void fn_802A469C(void*, const glModelPacket*);
extern "C" void fn_802A470C(void*, const glModelPacket*);
extern "C" void fn_802CC978(void*, const glModelPacket*, u32);

extern "C" void fn_802A2220(void* renderer)
{
    fn_802A4418(renderer, true);
    gxSetNumChans(0);
    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 0, 0, 255);
    GXSetTevKColorSel(0, 12);
    GXSetTevKAlphaSel(0, 28);
    gxSetTevColourIn(0, 15, 14, 8, 15);
    gxSetTevAlphaIn(0, 7, 6, 4, 7);
}

extern "C" void fn_802A22B8(void*)
{
}

extern "C" void fn_802A22BC(void* renderer, const glModelPacket* packet)
{
    u32* parameter = static_cast<u32*>(packet->unknown20);
    fn_802CC978(renderer, packet, *parameter);
}

extern "C" void fn_802A22C8(void* renderer, const glModelPacket* packet)
{
    Parameter_802A22C8* parameter = static_cast<Parameter_802A22C8*>(packet->unknown20);
    GXColor_802A22C8 colour;
    colour.r = static_cast<u8>(lbl_806E5F88 * parameter->red);
    colour.g = static_cast<u8>(lbl_806E5F88 * parameter->green);
    colour.b = static_cast<u8>(lbl_806E5F88 * parameter->blue);
    colour.a = static_cast<u8>(lbl_806E5F88 * parameter->alpha);
    GXSetTevKColor(0, colour);

    fn_802A44B0(renderer, packet);
    fn_802A470C(renderer, packet);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        fn_802A44F8(renderer, packet);
    }
    else
    {
        fn_802A469C(renderer, packet);
    }
}
