#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/glx/GXThreeLightDiffuseMaterialProgram.h"
#include "NL/glx/glxLight.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

template <>
void GXMaterialProgramImpl<GXThreeLightDiffuseMaterialProgram>::Activate(
    GLView*)
{
    static_cast<GXThreeLightDiffuseMaterialProgram*>(this)->ConfigureVertexFormat(true);
    gxSetNumChans(1);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
    GXSetChanCtrl(GX_COLOR0, true, GX_SRC_REG, GX_SRC_REG, (GXLightID)(GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2), GX_DF_CLAMP, GX_AF_NONE);

    nlColour materialColour = { { 255, 255, 255, 255 } };
    gxSetChanMatColour(0, materialColour);
}

template <>
void GXMaterialProgramImpl<GXThreeLightDiffuseMaterialProgram>::Deactivate()
{
    GXSetChanCtrl(GX_COLOR0, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)(GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2), GX_DF_NONE, GX_AF_NONE);
}

template <>
void GXMaterialProgramImpl<GXThreeLightDiffuseMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXThreeLightDiffuseParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXThreeLightDiffuseMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    glx_LoadDirectionalLight(0,
        &static_cast<const GXThreeLightDiffuseParameters*>(packet->materialParameters)->lightDirections[0],
        &static_cast<const GXThreeLightDiffuseParameters*>(packet->materialParameters)->lightColours[0]);
    glx_LoadDirectionalLight(1,
        &static_cast<const GXThreeLightDiffuseParameters*>(packet->materialParameters)->lightDirections[1],
        &static_cast<const GXThreeLightDiffuseParameters*>(packet->materialParameters)->lightColours[1]);
    glx_LoadDirectionalLight(2,
        &static_cast<const GXThreeLightDiffuseParameters*>(packet->materialParameters)->lightDirections[2],
        &static_cast<const GXThreeLightDiffuseParameters*>(packet->materialParameters)->lightColours[2]);
    glx_SetAmbientColour(
        &static_cast<const GXThreeLightDiffuseParameters*>(packet->materialParameters)->ambientColour);

    static_cast<GXThreeLightDiffuseMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXThreeLightDiffuseMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXThreeLightDiffuseMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXThreeLightDiffuseMaterialProgram*>(this)->DrawDirect(packet);
}
