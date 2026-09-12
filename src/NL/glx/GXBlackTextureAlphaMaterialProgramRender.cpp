#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "Game/GameObjectLighting.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXBlackTextureAlphaMaterialProgram.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

bool glx_RenderBlackTextureAlpha = true;

static nlMatrix4 sBlackTextureAlphaViewMatrix;
static int sBlackTextureAlphaLightCount;

template <>
void GXMaterialProgramImpl<GXBlackTextureAlphaMaterialProgram>::Activate(GLView* view)
{
    static_cast<GXBlackTextureAlphaMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sBlackTextureAlphaViewMatrix);
    sBlackTextureAlphaLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sBlackTextureAlphaLightCount, view, 0);
    SetGameObjectLightingEnabled(1, sBlackTextureAlphaLightCount, 0);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
    gxSetNumChans(1);
    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 0, 0, 255);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 7, 7, 4);
}

template <>
void GXMaterialProgramImpl<GXBlackTextureAlphaMaterialProgram>::Deactivate()
{
    SetGameObjectLightingEnabled(0, sBlackTextureAlphaLightCount, 1);
    gxSetCurrentMtx(0, true);
}

template <>
void GXMaterialProgramImpl<GXBlackTextureAlphaMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXBlackTextureAlphaParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXBlackTextureAlphaMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!glx_RenderBlackTextureAlpha)
        return;

    static_cast<GXBlackTextureAlphaMaterialProgram*>(this)->BindVertexArrays(packet);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
    static_cast<GXBlackTextureAlphaMaterialProgram*>(this)->BindParameters(packet);

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, sBlackTextureAlphaViewMatrix);

    if (packet->skinnedVertices == 0)
    {
        const GXBlackTextureAlphaParameters* parameters = static_cast<const GXBlackTextureAlphaParameters*>(packet->materialParameters);
        glx_LoadSkinMatrices(parameters->skinMatrices,
            parameters->skinMatricesSize / 48,
            &modelview,
            0);
    }
    else
    {
        glx_LoadDefaultSkinMatrices(&modelview);
    }

    GXCallDisplayList(packet->displayList->list, packet->displayList->size);
}
