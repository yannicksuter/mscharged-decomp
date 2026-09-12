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
    GXLoadTexMtxImm(glx_SpecularLookupTextureMatrix, 64, GX_MTX3x4);
    GXSetTexCoordGen2(
        GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_NRM, 30, true, 64);
    sSpecularLookupModelMatrix = -1;

    gxSetNumChans(0);
    gxSetNumTexGens(2);
    gxSetNumTevStages(3);
    gxSetTevOrder(0, 1, 1, 255);
    gxSetTevOrder(1, 0, 2, 255);
    gxSetTevOrder(2, 0, 0, 255);
    gxSetTevColourIn(0, 15, 12, 8, 15);
    gxSetTevColourIn(1, 15, 0, 8, 15);
    gxSetTevColourIn(2, 15, 12, 8, 0);
    gxSetTevAlphaIn(0, 7, 7, 7, 7);
    gxSetTevAlphaIn(1, 7, 7, 7, 7);
    gxSetTevAlphaIn(2, 7, 7, 7, 4);
}

template <>
void GXMaterialProgramImpl<GXSpecularLookupMaterialProgram>::Deactivate()
{
    gxSetTexCoordGen(1, 1, 5, 60);
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
        GXLoadTexMtxImm(normalMatrix, 30, GX_MTX3x4);
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXSpecularLookupMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXSpecularLookupMaterialProgram*>(this)->DrawDirect(packet);
}
