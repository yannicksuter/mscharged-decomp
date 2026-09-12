#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXSkinnedUnlitTextureMaterialProgram.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/platvmath.h"
#include "Game/UnidentifiedStaticStorage.h"

static nlMatrix4 sSkinnedUnlitTextureViewMatrix;

template <>
void GXMaterialProgramImpl<GXSkinnedUnlitTextureMaterialProgram>::Activate(GLView* view)
{
    gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    gxSetNumTexGens(1);
    gxSetNumChans(0);
    gxSetNumTevStages(1);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
    static_cast<GXSkinnedUnlitTextureMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sSkinnedUnlitTextureViewMatrix);
}

template <>
void GXMaterialProgramImpl<GXSkinnedUnlitTextureMaterialProgram>::Deactivate()
{
    gxSetCurrentMtx(GX_PNMTX0, true);
}

template <>
void GXMaterialProgramImpl<GXSkinnedUnlitTextureMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXSkinnedUnlitTextureParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXSkinnedUnlitTextureMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    nlMatrix4 modelMatrix;
    nlMatrix4 modelViewMatrix;

    static_cast<GXSkinnedUnlitTextureMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXSkinnedUnlitTextureMaterialProgram*>(this)->BindParameters(packet);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
    glGetMatrix(packet->matrix, modelMatrix);
    nlMultMatrices(modelViewMatrix, modelMatrix, sSkinnedUnlitTextureViewMatrix);

    if (packet->skinnedVertices == 0)
    {
        const GXSkinnedUnlitTextureParameters* parameters = static_cast<const GXSkinnedUnlitTextureParameters*>(packet->materialParameters);
        glx_LoadSkinMatrices(parameters->skinMatrices,
            parameters->skinMatrixBytes / sizeof(*parameters->skinMatrices),
            &modelViewMatrix, false);
    }
    else
    {
        glx_LoadDefaultSkinMatrices(&modelViewMatrix);
    }

    GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    gxSetCurrentMtx(GX_PNMTX0, true);
}
