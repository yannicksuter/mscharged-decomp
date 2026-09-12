#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/glx/GXMaskedDiffuseBlendMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

template <>
void GXMaterialProgramImpl<GXMaskedDiffuseBlendMaterialProgram>::Activate(GLView*)
{
    static_cast<GXMaskedDiffuseBlendMaterialProgram*>(this)->ConfigureVertexFormat(true);
    gxSetNumChans(1);
    gxSetNumTevStages(4);
    gxSetNumTexGens(3);
    gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_C0, GX_CC_CPREV, GX_CC_TEXA, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
}

template <>
void GXMaterialProgramImpl<GXMaskedDiffuseBlendMaterialProgram>::Deactivate()
{
    gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
}

template <>
void GXMaterialProgramImpl<GXMaskedDiffuseBlendMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXMaskedDiffuseBlendParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXMaskedDiffuseBlendMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaskedDiffuseBlendMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXMaskedDiffuseBlendMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaskedDiffuseBlendMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaskedDiffuseBlendMaterialProgram*>(this)->DrawDirect(packet);
}
