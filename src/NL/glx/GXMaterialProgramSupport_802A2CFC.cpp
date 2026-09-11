#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

struct GXMaterialProgramParameters_802A5D58
{
    /* 0x00 */ glTextureBinding texture0;
    /* 0x08 */ nlVector3 values8[3];
    /* 0x2C */ nlFloatColour values44[3];
    /* 0x5C */ nlFloatColour value92;
}; // size: 0x6C

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
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXMaterialProgramParameters_802A5D58*>(packet->materialParameters)->texture0.texture);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A5D58>::Draw(
    const glModelPacket* packet)
{
    glx_LoadDirectionalLight(0, &static_cast<const GXMaterialProgramParameters_802A5D58*>(packet->materialParameters)->values8[0], &static_cast<const GXMaterialProgramParameters_802A5D58*>(packet->materialParameters)->values44[0]);
    glx_LoadDirectionalLight(1, &static_cast<const GXMaterialProgramParameters_802A5D58*>(packet->materialParameters)->values8[1], &static_cast<const GXMaterialProgramParameters_802A5D58*>(packet->materialParameters)->values44[1]);
    glx_LoadDirectionalLight(2, &static_cast<const GXMaterialProgramParameters_802A5D58*>(packet->materialParameters)->values8[2], &static_cast<const GXMaterialProgramParameters_802A5D58*>(packet->materialParameters)->values44[2]);
    glx_SetAmbientColour(&static_cast<const GXMaterialProgramParameters_802A5D58*>(packet->materialParameters)->value92);

    static_cast<GXMaterialProgram_802A5D58*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A5D58*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A5D58*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_802A5D58*>(this)->DrawDirect(packet);
}
