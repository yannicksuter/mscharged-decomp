#include <revolution/gx.h>

#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlColour.h"

unsigned int gxSetNumChans(unsigned int);
void gxSetTevOrder(int, int, int, int);
void gxSetTevColourIn(int, int, int, int, int);
void gxSetTevAlphaIn(int, int, int, int, int);
nlColour gxSetChanMatColour(int, const nlColour&);

extern nlColour lbl_806E5FA8;

extern "C" void fn_802A5E10(void*, bool);
extern "C" void fn_802A5ED8(void*, const glModelPacket*);
extern "C" void fn_802A5F30(void*, const glModelPacket*);
extern "C" void fn_802A611C(void*, const glModelPacket*);
extern "C" void fn_802A6190(void*, const glModelPacket*);
extern "C" void fn_802CC978(void*, const glModelPacket*, u32);
extern "C" void fn_8036A800(int, void*, void*);
extern "C" void fn_8036A938(void*);

extern "C" void fn_802A2CFC(void* renderer)
{
    fn_802A5E10(renderer, true);
    gxSetNumChans(1);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 6, 4, 7);
    GXSetChanCtrl(GX_COLOR0, GX_TRUE, GX_SRC_REG, GX_SRC_REG, static_cast<GXLightID>(GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2), GX_DF_CLAMP, GX_AF_NONE);

    nlColour materialColour = lbl_806E5FA8;
    gxSetChanMatColour(0, materialColour);
}

extern "C" void fn_802A2DB8()
{
    GXSetChanCtrl(GX_COLOR0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, static_cast<GXLightID>(GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2), GX_DF_NONE, GX_AF_NONE);
}

extern "C" void fn_802A2DD8(void* renderer, const glModelPacket* packet)
{
    u32* parameter = static_cast<u32*>(packet->unknown20);
    fn_802CC978(renderer, packet, *parameter);
}

extern "C" void fn_802A2DE4(void* renderer, const glModelPacket* packet)
{
    u8* parameters = static_cast<u8*>(packet->unknown20);
    fn_8036A800(0, parameters + 8, parameters + 0x2C);
    parameters = static_cast<u8*>(packet->unknown20);
    fn_8036A800(1, parameters + 0x14, parameters + 0x3C);
    parameters = static_cast<u8*>(packet->unknown20);
    fn_8036A800(2, parameters + 0x20, parameters + 0x4C);
    parameters = static_cast<u8*>(packet->unknown20);
    fn_8036A938(parameters + 0x5C);

    fn_802A5ED8(renderer, packet);
    fn_802A6190(renderer, packet);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        fn_802A5F30(renderer, packet);
    }
    else
    {
        fn_802A611C(renderer, packet);
    }
}
