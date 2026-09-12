#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/glx/GXVertexColourDetailBlendMaterialProgram.h"
#include "NL/nlColour.h"
#include "NL/glx/glxGX.h"
#include "Game/UnidentifiedStaticStorage.h"

static void ConfigureVertexColourDetailBlendStages()
{
    gxSetNumChans(1);
    gxSetNumTevStages(3);
    gxSetNumTexGens(2);
    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
    gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG1);
    gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_C0, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
    gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C1, GX_CC_RASC, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
}

template <>
void GXMaterialProgramImpl<GXVertexColourDetailBlendMaterialProgram>::Activate(
    GLView*)
{
    GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_NONE, GX_AF_NONE);
    static_cast<GXVertexColourDetailBlendMaterialProgram*>(this)->ConfigureVertexFormat(true);
    ConfigureVertexColourDetailBlendStages();
}

template <>
void GXMaterialProgramImpl<GXVertexColourDetailBlendMaterialProgram>::Deactivate()
{
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
}

template <>
void GXMaterialProgramImpl<GXVertexColourDetailBlendMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXVertexColourDetailBlendParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXVertexColourDetailBlendMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXVertexColourDetailBlendMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXVertexColourDetailBlendMaterialProgram*>(this)->BindParameters(packet);

    const GXVertexColourDetailBlendParameters* parameters =
        static_cast<const GXVertexColourDetailBlendParameters*>(packet->materialParameters);
    nlFloatColour blendColour = { { 0.0f, 0.0f, 0.0f, 255.0f } };
    blendColour.c[0] = parameters->blendAmount;
    blendColour.c[1] = parameters->blendAmount;
    blendColour.c[2] = parameters->blendAmount;
    // The draw paths do not use this converted colour.
    GXColor blendColour8;
    ConvertColour(*(nlColour*)&blendColour8, blendColour);

    if (packet->indexBuffer != 0)
        static_cast<GXVertexColourDetailBlendMaterialProgram*>(this)->DrawIndexed(
            packet);
    else
        static_cast<GXVertexColourDetailBlendMaterialProgram*>(this)->DrawDirect(
            packet);
}
