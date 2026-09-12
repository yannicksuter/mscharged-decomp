#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/glx/GXVertexColourTextureMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

template <>
void GXMaterialProgramImpl<GXVertexColourTextureMaterialProgram>::Activate(
    GLView*)
{
    static_cast<GXVertexColourTextureMaterialProgram*>(this)->ConfigureVertexFormat(true);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetNumChans(1);
    gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
}

template <>
void GXMaterialProgramImpl<GXVertexColourTextureMaterialProgram>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXVertexColourTextureMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXVertexColourTextureParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXVertexColourTextureMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXVertexColourTextureMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXVertexColourTextureMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXVertexColourTextureMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXVertexColourTextureMaterialProgram*>(this)->DrawDirect(packet);
}
