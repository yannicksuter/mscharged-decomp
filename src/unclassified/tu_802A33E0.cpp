#include <revolution/gx.h>

#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"

struct ScissorParameters_802A34F8
{
    u32 value;
    u8 pad_04[4];
    float x;
    float y;
    float width;
    float height;
};

extern float lbl_806E5FB8;

extern "C" u32 fn_80369394();
extern "C" u32 fn_803693A4();
extern "C" void fn_802A6C24(void*, bool);
extern "C" void fn_802A6CEC(void*, const glModelPacket*);
extern "C" void fn_802A6D44(void*, const glModelPacket*);
extern "C" void fn_802A6F30(void*, const glModelPacket*);
extern "C" void fn_802A6FA4(void*, const glModelPacket*);
extern "C" void fn_802CC978(void*, const glModelPacket*, u32);

extern "C" void fn_802A33E0(void* renderer)
{
    gxSetNumChans(1);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 5, 4, 7);
    fn_802A6C24(renderer, true);
}

extern "C" void fn_802A34A8()
{
    u32 height = fn_803693A4();
    u32 width = fn_80369394();
    GXSetScissor(0, 0, width, height);
}

extern "C" void fn_802A34EC(void* renderer, const glModelPacket* packet)
{
    ScissorParameters_802A34F8* parameters = static_cast<ScissorParameters_802A34F8*>(packet->unknown20);
    fn_802CC978(renderer, packet, parameters->value);
}

extern "C" void fn_802A34F8(void* renderer, const glModelPacket* packet)
{
    ScissorParameters_802A34F8* parameters = static_cast<ScissorParameters_802A34F8*>(packet->unknown20);
    if (parameters->x <= lbl_806E5FB8)
    {
        u32 height = fn_803693A4();
        u32 width = fn_80369394();
        GXSetScissor(0, 0, width, height);
    }
    else
    {
        s32 x = static_cast<s32>(parameters->x);
        s32 y = static_cast<s32>(parameters->y);
        s32 width = static_cast<s32>(parameters->width);
        s32 height = static_cast<s32>(parameters->height);
        GXSetScissor(x, y, width, height);
    }

    fn_802A6CEC(renderer, packet);
    fn_802A6FA4(renderer, packet);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        fn_802A6D44(renderer, packet);
    }
    else
    {
        fn_802A6F30(renderer, packet);
    }
}
