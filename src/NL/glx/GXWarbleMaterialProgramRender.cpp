#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glState.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

static float glx_WarbleMatrix[2][3] = {
    { 0.0f, 0.0f, 0.0625f },
    { 0.0f, 0.0625f, 0.0f },
};

bool glx_UseSolidWarbleTexture;
bool glx_UseBlackWarbleTexture;

void glx_EnableWarble(bool enabled)
{
    unsigned long texture = glGetTexture("target/warbleoffset");
    if (glx_UseSolidWarbleTexture)
    {
        if (glx_UseBlackWarbleTexture)
            texture = glGetTexture("global/black");
        else
            texture = glGetTexture("global/white");
    }

    if (enabled)
    {
        glTextureBinding textureState;
        textureState.texture = texture;
        textureState.textureIndex = 0xFFFF;
        textureState.flags = 0;
        textureState.SetWrapS(true);
        textureState.SetWrapT(true);
        textureState.unknown07 = 0;
        glx_BindTexture(1, &textureState);

        GXSetNumIndStages(1);
        GXSetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD0, GX_TEXMAP1);
        GXSetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_4, GX_ITS_4);
        GXSetTevIndWarp(GX_TEVSTAGE0, GX_INDTEXSTAGE0, true, false, GX_ITM_0);
        GXSetIndTexMtx(GX_ITM_0, glx_WarbleMatrix, 1);
    }
    else
    {
        GXSetNumIndStages(0);
        GXSetTevDirect(GX_TEVSTAGE0);
    }
}

template <>
void GXMaterialProgramImpl<GXWarbleMaterialProgram>::Activate(GLView*)
{
    static_cast<GXWarbleMaterialProgram*>(this)->ConfigureVertexFormat(true);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetNumChans(1);
    gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
    glx_EnableWarble(true);
}

template <>
void GXMaterialProgramImpl<GXWarbleMaterialProgram>::Deactivate()
{
    glx_EnableWarble(false);
}

template <>
void GXMaterialProgramImpl<GXWarbleMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, *(unsigned long*)packet->materialParameters);
}

template <>
void GXMaterialProgramImpl<GXWarbleMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXWarbleMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXWarbleMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXWarbleMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXWarbleMaterialProgram*>(this)->DrawDirect(packet);
}
