#include <revolution/gx.h>

#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

static nlMatrix4 mview;

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A58E8>::Activate(GLView* view)
{
    gxSetNumChans(1);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourIn(0, 15, 15, 15, 10);
    gxSetTevAlphaIn(0, 7, 7, 7, 6);
    GXSetChanCtrl(GX_COLOR0A0, true, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    static_cast<GXMaterialProgram_802A58E8*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(mview);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A58E8>::Deactivate()
{
    GXSetChanCtrl(GX_COLOR0, false, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_NONE, GX_AF_NONE);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A58E8>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A58E8>::Draw(
    const glModelPacket* packet)
{
    nlMatrix4 matrix;
    GXLightObj light;

    static_cast<GXMaterialProgram_802A58E8*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A58E8*>(this)->BindParameters(packet);
    glGetMatrix(packet->matrix, matrix);

    GXColor lightColour;
    lightColour.r = 125;
    lightColour.g = 125;
    lightColour.b = 150;
    lightColour.a = 255;
    GXInitLightColor(&light, lightColour);

    GXColor ambientColour;
    ambientColour.r = 0;
    ambientColour.g = 0;
    ambientColour.b = 0;
    ambientColour.a = 255;
    GXSetChanAmbColor(GX_COLOR0A0, ambientColour);

    GXColor materialColour;
    materialColour.a = 255;
    materialColour.b = 255;
    materialColour.g = 255;
    materialColour.r = 255;
    GXSetChanMatColor(GX_COLOR0A0, materialColour);

    nlVector3 lightPosition;
    lightPosition.x = 0.0f;
    lightPosition.y = 0.0f;
    lightPosition.z = 0.0f;
    GXInitLightPos(
        &light, lightPosition.x, lightPosition.y, lightPosition.z);
    GXLoadLightObjImm(&light, GX_LIGHT0);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A58E8*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_802A58E8*>(this)->DrawDirect(packet);
}
