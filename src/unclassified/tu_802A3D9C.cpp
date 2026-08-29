#include <revolution/gx.h>

#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"

unsigned int gxSetNumChans(unsigned int);
unsigned int gxSetNumTevStages(unsigned int);
unsigned int gxSetNumTexGens(unsigned int);
void gxSetTevOrder(int, int, int, int);
void gxSetTevColourIn(int, int, int, int, int);
void gxSetTevAlphaIn(int, int, int, int, int);

struct Parameters_802A3E20
{
    u32 value;
};

extern u8 lbl_806DF1B0;
extern u8 lbl_806DF1B1;

extern "C" void fn_802A78D8(void*, bool);
extern "C" void fn_802A79A0(void*, const glModelPacket*);
extern "C" void fn_802A79F8(void*, const glModelPacket*);
extern "C" void fn_802A7BE4(void*, const glModelPacket*);
extern "C" void fn_802A7C58(void*, const glModelPacket*);
extern "C" void fn_802CC978(void*, const glModelPacket*, u32);

extern "C" void fn_802A3D9C(void* renderer)
{
    fn_802A78D8(renderer, true);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetNumChans(1);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 5, 4, 7);
}

extern "C" void fn_802A3E1C(void*)
{
}

extern "C" void fn_802A3E20(
    void* renderer, const glModelPacket* packet)
{
    Parameters_802A3E20* parameters = static_cast<Parameters_802A3E20*>(packet->unknown20);
    fn_802CC978(renderer, packet, parameters->value);
}

extern "C" void fn_802A3E2C(
    void* renderer, const glModelPacket* packet)
{
    fn_802A79A0(renderer, packet);
    fn_802A7C58(renderer, packet);

    DisplayList* displayList = packet->displayList;
    if (displayList != 0 && lbl_806DF1B0 != 0)
    {
        GXCallDisplayList(displayList->list, displayList->size);
    }
    else if (lbl_806DF1B1 != 0)
    {
        if (packet->indexBuffer == 0)
        {
            fn_802A7BE4(renderer, packet);
        }
        else
        {
            fn_802A79F8(renderer, packet);
        }
    }
}
