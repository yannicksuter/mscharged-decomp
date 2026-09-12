#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glPlat.h"
#include "NL/glx/GXScissoredVertexColourTextureMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

template <>
void GXMaterialProgramImpl<GXScissoredVertexColourTextureMaterialProgram>::Activate(
    GLView*)
{
    gxSetNumChans(1);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
    static_cast<GXScissoredVertexColourTextureMaterialProgram*>(this)->ConfigureVertexFormat(true);
}

template <>
void GXMaterialProgramImpl<GXScissoredVertexColourTextureMaterialProgram>::Deactivate()
{
    GXSetScissor(0, 0, glplatGetFrameBufferWidth(), glplatGetFrameBufferHeight());
}

template <>
void GXMaterialProgramImpl<GXScissoredVertexColourTextureMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    const GXScissoredTextureParameters* parameters = static_cast<const GXScissoredTextureParameters*>(packet->materialParameters);
    glSetMaterialTextureAlphaState(this, packet, parameters->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXScissoredVertexColourTextureMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    const GXScissoredTextureParameters& parameters = *static_cast<const GXScissoredTextureParameters*>(packet->materialParameters);
    float scissorX = parameters.scissorX;

    if (scissorX <= -0.1f)
    {
        GXSetScissor(0, 0, glplatGetFrameBufferWidth(), glplatGetFrameBufferHeight());
    }
    else
    {
        GXSetScissor((int)scissorX,
            (int)parameters.scissorY,
            (int)parameters.scissorWidth,
            (int)parameters.scissorHeight);
    }

    static_cast<GXScissoredVertexColourTextureMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXScissoredVertexColourTextureMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        static_cast<GXScissoredVertexColourTextureMaterialProgram*>(this)->DrawIndexed(packet);
    }
    else
    {
        static_cast<GXScissoredVertexColourTextureMaterialProgram*>(this)->DrawDirect(packet);
    }
}
