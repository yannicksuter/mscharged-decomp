#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029A4A0>::Activate(GLView*)
{
    gxSetNumChans(1);
    gxSetNumTevStages(4);
    gxSetNumTexGens(4);
    static_cast<GXMaterialProgram_8029A4A0*>(this)->ConfigureVertexFormat(true);

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
void GXMaterialProgramImpl<GXMaterialProgram_8029A4A0>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029A4A0>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029A4A0>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_8029A4A0*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_8029A4A0*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_8029A4A0*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_8029A4A0*>(this)->DrawDirect(packet);
}
