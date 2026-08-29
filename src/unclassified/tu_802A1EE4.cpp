#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"

struct Parameter_802A1FA0
{
    u32 value;
};

extern "C" void fn_802A3FA8(void*, bool);
extern "C" void fn_802A4070(void*, const glModelPacket*);
extern "C" void fn_802A40C8(void*, const glModelPacket*);
extern "C" void fn_802A42B4(void*, const glModelPacket*);
extern "C" void fn_802A4328(void*, const glModelPacket*);
extern "C" void fn_802CC978(void*, const glModelPacket*, u32);
extern "C" void GXCallDisplayList(const void*, u32);

extern "C" void fn_802A1EE4(void* renderer)
{
    fn_802A3FA8(renderer, true);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetNumChans(1);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 5, 4, 7);
}

extern "C" void fn_802A1F9C(void*)
{
}

extern "C" void fn_802A1FA0(void* renderer, const glModelPacket* packet)
{
    Parameter_802A1FA0* parameter = static_cast<Parameter_802A1FA0*>(packet->unknown20);
    fn_802CC978(renderer, packet, parameter->value);
}

extern "C" void fn_802A1FAC(void* renderer, const glModelPacket* packet)
{
    fn_802A4070(renderer, packet);
    fn_802A4328(renderer, packet);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        fn_802A40C8(renderer, packet);
    }
    else
    {
        fn_802A42B4(renderer, packet);
    }
}
