#include <revolution/gx.h>

#include "NL/gl/glState.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

static float lbl_80524420[2][3] = {
    { 0.0f, 0.0f, 0.0625f },
    { 0.0f, 0.0625f, 0.0f },
};

bool lbl_806E1BF8;
bool lbl_806E1BF9;

void fn_80297F70(bool enabled)
{
    unsigned long texture = glGetTexture("target/warbleoffset");
    if (lbl_806E1BF8)
    {
        if (lbl_806E1BF9)
            texture = glGetTexture("global/black");
        else
            texture = glGetTexture("global/white");
    }

    if (enabled)
    {
        UnidentifiedTextureState textureState;
        textureState.texture = texture;
        textureState.textureIndex = 0xFFFF;
        textureState.flags = 0;
        textureState.SetWrapS(true);
        textureState.SetWrapT(true);
        textureState.unknown07 = 0;
        fn_8036BE88(1, &textureState);

        GXSetNumIndStages(1);
        GXSetIndTexOrder(GX_INDTEXSTAGE0, GX_TEXCOORD0, GX_TEXMAP1);
        GXSetIndTexCoordScale(GX_INDTEXSTAGE0, GX_ITS_4, GX_ITS_4);
        GXSetTevIndWarp(GX_TEVSTAGE0, GX_INDTEXSTAGE0, true, false, GX_ITM_0);
        GXSetIndTexMtx(GX_ITM_0, lbl_80524420, 1);
    }
    else
    {
        GXSetNumIndStages(0);
        GXSetTevDirect(GX_TEVSTAGE0);
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A05A4>::Activate(GLView*)
{
    static_cast<GXMaterialProgram_802A05A4*>(this)->ConfigureVertexFormat(true);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetNumChans(1);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 5, 4, 7);
    fn_80297F70(true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A05A4>::Deactivate()
{
    fn_80297F70(false);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A05A4>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A05A4>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_802A05A4*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A05A4*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A05A4*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_802A05A4*>(this)->DrawDirect(packet);
}
