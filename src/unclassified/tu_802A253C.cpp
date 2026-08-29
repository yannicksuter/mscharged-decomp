#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"

struct DetailParameters_802A2668
{
    u8 diffuse[8];
    u8 detail[8];
    float blendAmount;
};

struct GXColor_802A2668
{
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

extern float lbl_806E5F90;

extern "C" void GXSetTevKColorSel(int, int);
extern "C" void GXSetTevKAlphaSel(int, int);
extern "C" void GXSetTevKColor(int, GXColor_802A2668);
extern "C" void GXCallDisplayList(const void*, u32);
extern "C" void fn_802A4FC4(void*, bool);
extern "C" void fn_802A508C(void*, const glModelPacket*);
extern "C" void fn_802A50E4(void*, const glModelPacket*);
extern "C" void fn_802A52D0(void*, const glModelPacket*);
extern "C" void fn_802A5344(void*, const glModelPacket*);
extern "C" void fn_802CC978(void*, const glModelPacket*, u32);

extern "C" void fn_802A253C(void* renderer)
{
    fn_802A4FC4(renderer, true);
    gxSetNumChans(0);
    gxSetNumTevStages(2);
    gxSetNumTexGens(2);
    gxSetTevOrder(0, 0, 0, 255);
    gxSetTevOrder(1, 1, 1, 255);
    GXSetTevKColorSel(0, 12);
    GXSetTevKAlphaSel(0, 28);
    GXSetTevKColorSel(1, 12);
    GXSetTevKAlphaSel(1, 28);
    gxSetTevColourIn(0, 8, 15, 14, 15);
    gxSetTevAlphaIn(0, 4, 7, 6, 7);
    gxSetTevColourIn(1, 15, 8, 14, 0);
    gxSetTevAlphaIn(1, 7, 4, 6, 0);
}

extern "C" void fn_802A2630(void*)
{
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
}

extern "C" void fn_802A265C(void* renderer, const glModelPacket* packet)
{
    u32* parameter = static_cast<u32*>(packet->unknown20);
    fn_802CC978(renderer, packet, *parameter);
}

extern "C" void fn_802A2668(void* renderer, const glModelPacket* packet)
{
    fn_802A508C(renderer, packet);
    fn_802A5344(renderer, packet);

    DetailParameters_802A2668* parameters = static_cast<DetailParameters_802A2668*>(packet->unknown20);
    u8 blend = static_cast<u8>(lbl_806E5F90 * parameters->blendAmount);
    GXColor_802A2668 colour = { blend, blend, blend, blend };
    GXSetTevKColor(0, colour);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        fn_802A50E4(renderer, packet);
    }
    else
    {
        fn_802A52D0(renderer, packet);
    }
}
