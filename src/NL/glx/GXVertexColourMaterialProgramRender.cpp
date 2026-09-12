#include <revolution/gx.h>

#include "NL/glx/GXVertexColourMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

template <>
void GXMaterialProgramImpl<GXVertexColourMaterialProgram>::Activate(
    GLView*)
{
    static_cast<GXVertexColourMaterialProgram*>(this)->ConfigureVertexFormat(true);
    gxSetNumChans(1);
    gxSetNumTexGens(0);
    gxSetNumTevStages(1);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
}

template <>
void GXMaterialProgramImpl<GXVertexColourMaterialProgram>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXVertexColourMaterialProgram>::Prepare(
    glModelPacket*)
{
}

template <>
void GXMaterialProgramImpl<GXVertexColourMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXVertexColourMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXVertexColourMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXVertexColourMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXVertexColourMaterialProgram*>(this)->DrawDirect(packet);
}
