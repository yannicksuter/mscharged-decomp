#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/glx/GXUnlitTextureMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

template <>
void GXMaterialProgramImpl<GXUnlitTextureMaterialProgram>::Activate(
    GLView*)
{
    gxSetNumChans(0);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
    static_cast<GXUnlitTextureMaterialProgram*>(this)->ConfigureVertexFormat(true);
}

template <>
void GXMaterialProgramImpl<GXUnlitTextureMaterialProgram>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXUnlitTextureMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXUnlitTextureParameters*>(packet->materialParameters)->texture.texture);
}

template <>
void GXMaterialProgramImpl<GXUnlitTextureMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXUnlitTextureMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXUnlitTextureMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXUnlitTextureMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXUnlitTextureMaterialProgram*>(this)->DrawDirect(packet);
}
