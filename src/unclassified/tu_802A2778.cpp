#include "NL/gl/glModel.h"
#include "NL/glx/glxGX.h"

extern "C" void GXSetChanCtrl(int, int, int, int, int, int, int);
extern "C" void fn_802A546C(void*, bool);
extern "C" void fn_802A5564(void*, const glModelPacket*);
extern "C" void fn_802A55CC(void*, const glModelPacket*);
extern "C" void fn_802A5800(void*, const glModelPacket*);
extern "C" void fn_802A5878(void*, const glModelPacket*);
extern "C" void fn_802CC978(void*, const glModelPacket*, u32);

extern "C" void fn_802A2778()
{
    gxSetNumChans(1);
    gxSetNumTevStages(3);
    gxSetNumTexGens(2);
    gxSetTexCoordGen(0, 1, 4, 60);
    gxSetTexCoordGen(1, 1, 5, 60);
    gxSetTevOrder(0, 0, 0, 255);
    gxSetTevOrder(1, 1, 1, 255);
    gxSetTevOrder(2, 255, 255, 4);
    gxSetTevColourOp(0, 0, 0, 0, true, 1);
    gxSetTevColourOp(1, 0, 0, 0, true, 2);
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
    gxSetTevColourIn(0, 15, 15, 15, 8);
    gxSetTevAlphaIn(0, 7, 7, 7, 4);
    gxSetTevColourIn(1, 2, 8, 9, 15);
    gxSetTevAlphaIn(1, 7, 7, 7, 6);
    gxSetTevColourIn(2, 15, 4, 10, 15);
    gxSetTevAlphaIn(2, 7, 7, 7, 6);
}

extern "C" void fn_802A28F4(void* renderer)
{
    GXSetChanCtrl(4, 0, 0, 1, 1, 0, 2);
    fn_802A546C(renderer, true);
    fn_802A2778();
}

extern "C" void fn_802A294C(void*)
{
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
}

extern "C" void fn_802A2978(void* renderer, const glModelPacket* packet)
{
    u32* parameter = static_cast<u32*>(packet->unknown20);
    fn_802CC978(renderer, packet, *parameter);
}

extern "C" void fn_802A2984(void* renderer, const glModelPacket* packet)
{
    fn_802A5564(renderer, packet);
    fn_802A5878(renderer, packet);

    if (packet->indexBuffer != 0)
    {
        fn_802A55CC(renderer, packet);
    }
    else
    {
        fn_802A5800(renderer, packet);
    }
}
