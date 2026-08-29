#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A4B28>::Activate(
    GLView*)
{
    gxSetNumChans(0);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTevOrder(0, 0, 0, 255);
    gxSetTevColourIn(0, 15, 12, 8, 15);
    gxSetTevAlphaIn(0, 7, 6, 4, 7);
    static_cast<GXMaterialProgram_802A4B28*>(this)->ConfigureVertexFormat(true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A4B28>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A4B28>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A4B28>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_802A4B28*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A4B28*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A4B28*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_802A4B28*>(this)->DrawDirect(packet);
}
