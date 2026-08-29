#include <revolution/gx.h>

#include "Game/Camera/CameraMan.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"

extern "C"
{
    void* fn_80182240(int, int);
    void fn_80182ED0(void*, GLView*, int);
    void fn_801832F4(int, void*, int);
    void fn_801836FC(int);
    void fn_801837DC(int, int);
    void fn_80183A98();
    void fn_80183B40(unsigned long matrix);
}

int lbl_806DF0B0 = 4;
bool lbl_806DF0B4 = true;

nlMatrix4 lbl_8057B330;
void* lbl_806E1BB8;
bool lbl_806E1BBC;
bool lbl_806E1BBD;

extern "C" void fn_80295E00()
{
    gxSetNumChans(1);
    gxSetNumTevStages(lbl_806DF0B0);
    gxSetNumTexGens(3);

    gxSetTevOrder(0, 1, 1, 255);
    gxSetTevOrder(1, 255, 255, 255);
    gxSetTevOrder(2, 2, 2, 255);
    gxSetTevOrder(3, 0, 0, 4);

    gxSetTexCoordGen(0, 1, 4, 60);
    gxSetTexCoordGen(1, 1, 0, 33);
    gxSetTexCoordGen(2, 1, 6, 60);
    gxSetTevKColourSel(1, 12);

    gxSetTevColourIn(0, 15, 8, 9, 15);
    gxSetTevAlphaIn(0, 7, 7, 7, 7);
    gxSetTevColourIn(1, 15, 0, 14, 15);
    gxSetTevAlphaIn(1, 7, 7, 7, 7);
    gxSetTevColourIn(2, 15, 0, 8, 15);
    gxSetTevAlphaIn(2, 7, 7, 7, 7);
    gxSetTevColourIn(3, 15, 10, 8, 0);
    gxSetTevAlphaIn(3, 7, 7, 7, 4);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029EF54>::Activate(GLView* view)
{
    static_cast<GXMaterialProgram_8029EF54*>(this)->ConfigureVertexFormat(true);
    fn_80295E00();
    view->m_Interface->GetViewMatrix(lbl_8057B330);
    lbl_806E1BB8 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1BB8, view, 0);
    lbl_806E1BBC = false;
    fn_801832F4(0, lbl_806E1BB8, 1);
    fn_801836FC(0);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029EF54>::Deactivate()
{
    if (lbl_806E1BBD)
    {
        fn_80183A98();
        lbl_806E1BBD = false;
    }

    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTexCoordGen(0, 1, 4, 60);
    gxSetTexCoordGen(1, 1, 5, 60);
    gxSetTexCoordGen(2, 1, 6, 60);

    if (lbl_806E1BBC)
    {
        fn_801832F4(0, lbl_806E1BB8, 1);
        fn_801836FC(0);
        lbl_806E1BBC = false;
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029EF54>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_80295E00
{
    float c[4];
};

static inline GXColor ConvertColour_80295E00(
    const FloatColour_80295E00& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029EF54>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_8029EF54*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_8029EF54*>(this)->BindParameters(packet);

    const nlVector3& cameraPosition = cCameraManager::PeekCamera()->GetCameraPosition();
    float scale = *(float*)((unsigned char*)packet->unknown20 + 24);
    float scroll = *(float*)((unsigned char*)packet->unknown20 + 28);
    float reciprocal = scale == 0.0f ? 1.0f : 1.0f / scale;

    Mtx textureMatrix;
    textureMatrix[0][0] = reciprocal;
    textureMatrix[0][1] = 0.0f;
    textureMatrix[0][2] = 0.0f;
    textureMatrix[0][3] = 0.5f - scroll * cameraPosition.x * reciprocal;
    textureMatrix[1][0] = 0.0f;
    textureMatrix[1][1] = reciprocal;
    textureMatrix[1][2] = 0.0f;
    textureMatrix[1][3] = 0.5f - scroll * cameraPosition.y * reciprocal;
    textureMatrix[2][0] = 0.0f;
    textureMatrix[2][1] = 0.0f;
    textureMatrix[2][2] = 1.0f;
    textureMatrix[2][3] = 0.0f;
    GXLoadTexMtxImm(textureMatrix, 33, GX_MTX3x4);

    if (*(int*)((unsigned char*)packet->unknown20 + 40) == 1)
    {
        if (!lbl_806E1BBC)
        {
            fn_801832F4(1, lbl_806E1BB8, 1);
            fn_801836FC(1);
            lbl_806E1BBC = true;
        }
    }
    else if (lbl_806E1BBC)
    {
        fn_801832F4(0, lbl_806E1BB8, 1);
        fn_801836FC(0);
        lbl_806E1BBC = false;
    }

    fn_80183B40(packet->matrix);

    bool enableState = false;
    if (*(int*)((unsigned char*)packet->unknown20 + 44) == 1
        && lbl_806DF0B4)
        enableState = true;

    if (enableState)
    {
        if (!lbl_806E1BBD)
        {
            fn_801837DC(0, 0);
            lbl_806E1BBD = true;
        }
    }
    else if (lbl_806E1BBD)
    {
        fn_80183A98();
        lbl_806E1BBD = false;
    }

    if (*(int*)((unsigned char*)packet->unknown20 + 36) == 1)
    {
        UnidentifiedTextureState* texture = (UnidentifiedTextureState*)packet->unknown20;
        texture->SetWrapS(true);
        texture->SetWrapT(true);
    }

    float value = *(float*)((unsigned char*)packet->unknown20 + 32);
    FloatColour_80295E00 colour = { { value, value, value, value } };
    GXColor gxColour = ConvertColour_80295E00(colour);
    GXSetTevKColor(GX_KCOLOR0, gxColour);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_8029EF54*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_8029EF54*>(this)->DrawDirect(packet);
}
