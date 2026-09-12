#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXFixedLightMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

static nlMatrix4 sFixedLightViewMatrix;

template <>
void GXMaterialProgramImpl<GXFixedLightMaterialProgram>::Activate(GLView* view)
{
    gxSetNumChans(1);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
    GXSetChanCtrl(GX_COLOR0A0, true, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    static_cast<GXFixedLightMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sFixedLightViewMatrix);
}

template <>
void GXMaterialProgramImpl<GXFixedLightMaterialProgram>::Deactivate()
{
    GXSetChanCtrl(GX_COLOR0, false, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_NONE, GX_AF_NONE);
}

template <>
void GXMaterialProgramImpl<GXFixedLightMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXFixedLightParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXFixedLightMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    nlMatrix4 packetMatrix;
    GXLightObj light;

    static_cast<GXFixedLightMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXFixedLightMaterialProgram*>(this)->BindParameters(packet);
    glGetMatrix(packet->matrix, packetMatrix);

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
        static_cast<GXFixedLightMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXFixedLightMaterialProgram*>(this)->DrawDirect(packet);
}
