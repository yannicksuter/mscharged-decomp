#include <revolution/gx.h>

#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"

unsigned int gxSetNumChans(unsigned int);
unsigned int gxSetNumTevStages(unsigned int);
unsigned int gxSetNumTexGens(unsigned int);
void gxSetTevOrder(int, int, int, int);

extern "C" void fn_802A7090(void*, bool);
extern "C" void fn_802A7128(void*, const glModelPacket*);
extern "C" void fn_802A7170(void*, const glModelPacket*);
extern "C" void fn_802A7314(void*, const glModelPacket*);
extern "C" void fn_802A7384(void*, const glModelPacket*);

extern "C" void fn_802A362C(void* renderer)
{
    fn_802A7090(renderer, true);
    gxSetNumChans(1);
    gxSetNumTexGens(0);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 255, 255, 4);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
}

extern "C" void fn_802A3688(void*)
{
}

extern "C" void fn_802A368C(void*, const glModelPacket*)
{
}

extern "C" void fn_802A3690(void* renderer, const glModelPacket* packet)
{
    fn_802A7128(renderer, packet);
    fn_802A7384(renderer, packet);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        fn_802A7170(renderer, packet);
    }
    else
    {
        fn_802A7314(renderer, packet);
    }
}
