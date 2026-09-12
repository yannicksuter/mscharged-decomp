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
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 5, 4, 7);
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
