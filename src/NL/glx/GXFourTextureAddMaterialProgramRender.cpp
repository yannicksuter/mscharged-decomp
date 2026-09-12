#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/glx/GXFourTextureAddMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

template <>
void GXMaterialProgramImpl<GXFourTextureAddMaterialProgram>::Activate(GLView*)
{
    gxSetNumChans(1);
    gxSetNumTevStages(4);
    gxSetNumTexGens(4);
    static_cast<GXFourTextureAddMaterialProgram*>(this)->ConfigureVertexFormat(true);

    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevOrder(1, 1, 1, 4);
    gxSetTevOrder(2, 2, 2, 4);
    gxSetTevOrder(3, 3, 3, 4);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevColourIn(1, 15, 10, 8, 0);
    gxSetTevColourIn(2, 15, 10, 8, 0);
    gxSetTevColourIn(3, 15, 10, 8, 0);
    gxSetTevAlphaIn(0, 7, 5, 4, 7);
    gxSetTevAlphaIn(1, 7, 5, 4, 0);
    gxSetTevAlphaIn(2, 7, 5, 4, 0);
    gxSetTevAlphaIn(3, 7, 5, 4, 0);
}

template <>
void GXMaterialProgramImpl<GXFourTextureAddMaterialProgram>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXFourTextureAddMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXFourTextureAddParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXFourTextureAddMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXFourTextureAddMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXFourTextureAddMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXFourTextureAddMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXFourTextureAddMaterialProgram*>(this)->DrawDirect(packet);
}
