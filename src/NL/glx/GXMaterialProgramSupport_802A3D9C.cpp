#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

static bool glx_CompiledDraw = true;
static bool glx_AllowUncompiledDraws = true;

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A7820>::Activate(
    GLView*)
{
    static_cast<GXMaterialProgram_802A7820*>(this)->ConfigureVertexFormat(true);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetNumChans(1);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 5, 4, 7);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A7820>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A7820>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A7820>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_802A7820*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A7820*>(this)->BindParameters(packet);

    if (packet->displayList != 0 && glx_CompiledDraw)
    {
        GXCallDisplayList(
            packet->displayList->list, packet->displayList->size);
    }
    else if (glx_AllowUncompiledDraws)
    {
        if (packet->indexBuffer == 0)
            static_cast<GXMaterialProgram_802A7820*>(this)->DrawDirect(packet);
        else
            static_cast<GXMaterialProgram_802A7820*>(this)->DrawIndexed(packet);
    }
}
