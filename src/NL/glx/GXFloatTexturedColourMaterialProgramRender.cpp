#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/glx/GXFloatTexturedColourMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

static bool sUseFloatTexturedColourDisplayLists = true;
static bool sAllowFloatTexturedColourUncompiledDraws = true;

template <>
void GXMaterialProgramImpl<GXFloatTexturedColourMaterialProgram>::Activate(
    GLView*)
{
    static_cast<GXFloatTexturedColourMaterialProgram*>(this)->ConfigureVertexFormat(true);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetNumChans(1);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
}

template <>
void GXMaterialProgramImpl<GXFloatTexturedColourMaterialProgram>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXFloatTexturedColourMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXFloatTexturedColourParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXFloatTexturedColourMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXFloatTexturedColourMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXFloatTexturedColourMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList == 0 || !sUseFloatTexturedColourDisplayLists)
    {
        if (sAllowFloatTexturedColourUncompiledDraws)
        {
            if (packet->indexBuffer == 0)
                static_cast<GXFloatTexturedColourMaterialProgram*>(this)->DrawDirect(packet);
            else
                static_cast<GXFloatTexturedColourMaterialProgram*>(this)->DrawIndexed(packet);
        }
    }
    else if (sUseFloatTexturedColourDisplayLists)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
}
