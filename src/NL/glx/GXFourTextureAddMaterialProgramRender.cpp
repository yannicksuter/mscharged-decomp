#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/glx/GXFourTextureAddMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

template <>
void GXMaterialProgramImpl<GXFourTextureAddMaterialProgram>::Activate(GLView*)
{
    gxSetNumChans(1);
    gxSetNumTevStages(4);
    gxSetNumTexGens(4);
    static_cast<GXFourTextureAddMaterialProgram*>(this)->ConfigureVertexFormat(true);

    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
    gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR0A0);
    gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR0A0);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_CPREV);
    gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_CPREV);
    gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_CPREV);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_APREV);
    gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_APREV);
    gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_APREV);
}

template <>
void GXMaterialProgramImpl<GXFourTextureAddMaterialProgram>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXFourTextureAddMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXFourTextureAddParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXFourTextureAddMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXFourTextureAddMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXFourTextureAddMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXFourTextureAddMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXFourTextureAddMaterialProgram*>(this)->DrawDirect(packet);
}
