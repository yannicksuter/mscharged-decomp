#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"

struct Parameter_802A31A0
{
    u32 value;
};

extern "C" void fn_802A6490(void*, bool);
extern "C" void fn_802A6558(void*, const glModelPacket*);
extern "C" void fn_802A65B0(void*, const glModelPacket*);
extern "C" void fn_802A679C(void*, const glModelPacket*);
extern "C" void fn_802A6810(void*, const glModelPacket*);
extern "C" void fn_802CC978(void*, const glModelPacket*, u32);
extern "C" void GXCallDisplayList(const void*, u32);

extern "C" void fn_802A30E4(void* renderer)
{
    fn_802A6490(renderer, true);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetNumChans(1);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 5, 4, 7);
}

extern "C" void fn_802A319C(void*)
{
}

extern "C" void fn_802A31A0(void* renderer, const glModelPacket* packet)
{
    Parameter_802A31A0* parameter = static_cast<Parameter_802A31A0*>(packet->unknown20);
    fn_802CC978(renderer, packet, parameter->value);
}

extern "C" void fn_802A31AC(void* renderer, const glModelPacket* packet)
{
    fn_802A6558(renderer, packet);
    fn_802A6810(renderer, packet);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        fn_802A65B0(renderer, packet);
    }
    else
    {
        fn_802A679C(renderer, packet);
    }
}
