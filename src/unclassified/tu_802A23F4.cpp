#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"

extern "C" void GXCallDisplayList(const void*, u32);
extern "C" void fn_802A4BE0(void*, bool);
extern "C" void fn_802A4C78(void*, const glModelPacket*);
extern "C" void fn_802A4CC0(void*, const glModelPacket*);
extern "C" void fn_802A4E64(void*, const glModelPacket*);
extern "C" void fn_802A4ED4(void*, const glModelPacket*);
extern "C" void fn_802CC978(void*, const glModelPacket*, u32);

extern "C" void fn_802A23F4(void* renderer)
{
    gxSetNumChans(0);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTevOrder(0, 0, 0, 255);
    gxSetTevColourIn(0, 15, 12, 8, 15);
    gxSetTevAlphaIn(0, 7, 6, 4, 7);
    fn_802A4BE0(renderer, true);
}

extern "C" void fn_802A2484(void*)
{
}

extern "C" void fn_802A2488(void* renderer, const glModelPacket* packet)
{
    u32* parameter = static_cast<u32*>(packet->unknown20);
    fn_802CC978(renderer, packet, *parameter);
}

extern "C" void fn_802A2494(void* renderer, const glModelPacket* packet)
{
    fn_802A4C78(renderer, packet);
    fn_802A4ED4(renderer, packet);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        fn_802A4CC0(renderer, packet);
    }
    else
    {
        fn_802A4E64(renderer, packet);
    }
}
