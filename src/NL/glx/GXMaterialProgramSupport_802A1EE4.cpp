#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A3EF0>::Activate(
    GLView*)
{
    static_cast<GXMaterialProgram_802A3EF0*>(this)->ConfigureVertexFormat(true);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetNumChans(1);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 5, 4, 7);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A3EF0>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A3EF0>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A3EF0>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_802A3EF0*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A3EF0*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A3EF0*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_802A3EF0*>(this)->DrawDirect(packet);
}
