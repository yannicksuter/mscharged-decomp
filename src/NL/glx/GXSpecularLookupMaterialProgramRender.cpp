#include <revolution/gx.h>
#include <revolution/mtx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXSpecularLookupMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxMatrix.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

Mtx glx_SpecularLookupTextureMatrix = {
    { 0.5f, 0.0f, 0.0f, 0.5f },
    { 0.0f, -0.5f, 0.0f, 0.5f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

static nlMatrix4 sSpecularLookupViewMatrix;
static unsigned long sSpecularLookupModelMatrix;

template <>
void GXMaterialProgramImpl<GXSpecularLookupMaterialProgram>::Activate(GLView* view)
{
    static_cast<GXSpecularLookupMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sSpecularLookupViewMatrix);
    GXLoadTexMtxImm(glx_SpecularLookupTextureMatrix, GX_PTTEXMTX0, GX_MTX3x4);
    GXSetTexCoordGen2(
        GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, true, GX_PTTEXMTX0);
    sSpecularLookupModelMatrix = -1;

    gxSetNumChans(0);
    gxSetNumTexGens(2);
    gxSetNumTevStages(3);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP2, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_CPREV);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
}

template <>
void GXMaterialProgramImpl<GXSpecularLookupMaterialProgram>::Deactivate()
{
    gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
}

template <>
void GXMaterialProgramImpl<GXSpecularLookupMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXSpecularLookupParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXSpecularLookupMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXSpecularLookupMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXSpecularLookupMaterialProgram*>(this)->BindParameters(packet);

    if (packet->matrix != sSpecularLookupModelMatrix)
    {
        sSpecularLookupModelMatrix = packet->matrix;
        nlMatrix4 model;
        nlMatrix4 modelview;
        Mtx modelViewTransform;
        Mtx normalMatrix;
        glGetMatrix(packet->matrix, model);
        nlMultMatrices(modelview, model, sSpecularLookupViewMatrix);
        glxCopyMatrix(modelViewTransform, modelview);
        PSMTXInvXpose(modelViewTransform, normalMatrix);
        GXLoadTexMtxImm(normalMatrix, GX_TEXMTX0, GX_MTX3x4);
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXSpecularLookupMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXSpecularLookupMaterialProgram*>(this)->DrawDirect(packet);
}
