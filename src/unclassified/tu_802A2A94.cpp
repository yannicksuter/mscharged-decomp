#include <revolution/gx.h>

#include "NL/gl/glMatrix.h"
#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"

unsigned int gxSetNumChans(unsigned int);
unsigned int gxSetNumTevStages(unsigned int);
unsigned int gxSetNumTexGens(unsigned int);
void gxSetTevOrder(int, int, int, int);
void gxSetTevColourIn(int, int, int, int, int);
void gxSetTevAlphaIn(int, int, int, int, int);

class MatrixProvider_802A2A94
{
public:
    virtual void getMatrix(nlMatrix4*);
};

struct SetupContext_802A2A94
{
    u8 unknown00[0x50];
    MatrixProvider_802A2A94* matrixProvider;
};

extern nlMatrix4 lbl_8057B430;
extern float lbl_806E5FA0;

extern "C" void fn_802A59A0(void*, bool);
extern "C" void fn_802A5A68(void*, const glModelPacket*);
extern "C" void fn_802A5AC0(void*, const glModelPacket*);
extern "C" void fn_802A5CAC(void*, const glModelPacket*);
extern "C" void fn_802A5D20(void*, const glModelPacket*);
extern "C" void fn_802CC978(void*, const glModelPacket*, u32);

extern "C" void fn_802A2A94(void* renderer, SetupContext_802A2A94* context)
{
    gxSetNumChans(1);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourIn(0, 15, 15, 15, 10);
    gxSetTevAlphaIn(0, 7, 7, 7, 6);
    GXSetChanCtrl(GX_COLOR0A0, GX_TRUE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    fn_802A59A0(renderer, true);

    MatrixProvider_802A2A94* provider = context->matrixProvider;
    provider->getMatrix(&lbl_8057B430);
}

extern "C" void fn_802A2B6C()
{
    GXSetChanCtrl(GX_COLOR0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_NONE, GX_AF_NONE);
}

extern "C" void fn_802A2B8C(void* renderer, const glModelPacket* packet)
{
    u32* parameter = static_cast<u32*>(packet->unknown20);
    fn_802CC978(renderer, packet, *parameter);
}

extern "C" void fn_802A2B98(void* renderer, const glModelPacket* packet)
{
    fn_802A5A68(renderer, packet);
    fn_802A5D20(renderer, packet);

    nlMatrix4 matrix;
    glGetMatrix(packet->matrix, matrix);

    GXLightObj light;
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
    nlVec3Set(lightPosition, lbl_806E5FA0, lbl_806E5FA0, lbl_806E5FA0);
    GXInitLightPos(&light, lightPosition.x, lightPosition.y, lightPosition.z);
    GXLoadLightObjImm(&light, GX_LIGHT0);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        fn_802A5AC0(renderer, packet);
    }
    else
    {
        fn_802A5CAC(renderer, packet);
    }
}
