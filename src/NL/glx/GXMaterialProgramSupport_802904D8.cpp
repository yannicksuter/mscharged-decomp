#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

extern "C"
{
    void fn_801837DC(int, int);
    void fn_80183A98();
    void fn_80183B40(unsigned long matrix);
}

static bool sStateEnabled_806E1B18;

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029AFC4>::Activate(GLView*)
{
    static_cast<GXMaterialProgram_8029AFC4*>(this)->ConfigureVertexFormat(true);
    fn_80183B40(-1);
    sStateEnabled_806E1B18 = false;
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetNumChans(1);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 5, 4, 7);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029AFC4>::Deactivate()
{
    if (sStateEnabled_806E1B18)
    {
        fn_80183A98();
        sStateEnabled_806E1B18 = false;
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029AFC4>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029AFC4>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_8029AFC4*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_8029AFC4*>(this)->BindParameters(packet);

    if (*(int*)((unsigned char*)packet->unknown20 + 8) == 1)
    {
        fn_80183B40(packet->matrix);
        if (!sStateEnabled_806E1B18)
        {
            fn_801837DC(0, 0);
            sStateEnabled_806E1B18 = true;
        }
    }
    else if (sStateEnabled_806E1B18)
    {
        fn_80183A98();
        sStateEnabled_806E1B18 = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_8029AFC4*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_8029AFC4*>(this)->DrawDirect(packet);
}
