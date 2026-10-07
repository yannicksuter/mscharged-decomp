#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "Game/GameObjectLighting.h"
#include "NL/glx/GXShadowedDiffuseMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

static bool sShadowedDiffuseShadowStageEnabled;

template <>
void GXMaterialProgramImpl<GXShadowedDiffuseMaterialProgram>::Activate(GLView*)
{
    static_cast<GXShadowedDiffuseMaterialProgram*>(this)->ConfigureVertexFormat(true);
    SetGameObjectShadowModelMatrix(-1);
    sShadowedDiffuseShadowStageEnabled = false;
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetNumChans(1);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
}

template <>
void GXMaterialProgramImpl<GXShadowedDiffuseMaterialProgram>::Deactivate()
{
    if (sShadowedDiffuseShadowStageEnabled)
    {
        RestoreGameObjectShadowLighting();
        sShadowedDiffuseShadowStageEnabled = false;
    }
}

template <>
void GXMaterialProgramImpl<GXShadowedDiffuseMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXShadowedDiffuseParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXShadowedDiffuseMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXShadowedDiffuseMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXShadowedDiffuseMaterialProgram*>(this)->BindParameters(packet);

    if (static_cast<const GXShadowedDiffuseParameters*>(packet->materialParameters)->receiveShadows == 1)
    {
        SetGameObjectShadowModelMatrix(packet->matrix);
        if (!sShadowedDiffuseShadowStageEnabled)
        {
            ApplyGameObjectShadowLighting(0, 0);
            sShadowedDiffuseShadowStageEnabled = true;
        }
    }
    else if (sShadowedDiffuseShadowStageEnabled)
    {
        RestoreGameObjectShadowLighting();
        sShadowedDiffuseShadowStageEnabled = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXShadowedDiffuseMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXShadowedDiffuseMaterialProgram*>(this)->DrawDirect(packet);
}
