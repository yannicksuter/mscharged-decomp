#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlColour.h"

extern "C"
{
    void fn_8036A800(unsigned int index, const void* first, const void* second);
    void fn_8036A938(const void* value);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A5D58>::Activate(
    GLView*)
{
    static_cast<GXMaterialProgram_802A5D58*>(this)->ConfigureVertexFormat(true);
    gxSetNumChans(1);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 6, 4, 7);
    GXSetChanCtrl(GX_COLOR0, true, GX_SRC_REG, GX_SRC_REG, (GXLightID)(GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2), GX_DF_CLAMP, GX_AF_NONE);

    nlColour colour = { { 255, 255, 255, 255 } };
    gxSetChanMatColour(0, colour);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A5D58>::Deactivate()
{
    GXSetChanCtrl(GX_COLOR0, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)(GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2), GX_DF_NONE, GX_AF_NONE);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A5D58>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A5D58>::Draw(
    const glModelPacket* packet)
{
    fn_8036A800(0, (unsigned char*)packet->unknown20 + 8, (unsigned char*)packet->unknown20 + 44);
    fn_8036A800(1, (unsigned char*)packet->unknown20 + 20, (unsigned char*)packet->unknown20 + 60);
    fn_8036A800(2, (unsigned char*)packet->unknown20 + 32, (unsigned char*)packet->unknown20 + 76);
    fn_8036A938((unsigned char*)packet->unknown20 + 92);

    static_cast<GXMaterialProgram_802A5D58*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A5D58*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A5D58*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_802A5D58*>(this)->DrawDirect(packet);
}
