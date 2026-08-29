#include <revolution/gx.h>

#include <string.h>

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
    float fn_80277DB0();
}

int lbl_806DF0B8 = 4;
bool lbl_806DF0BC = true;

const Mtx lbl_804E8850 = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
};

nlMatrix4 lbl_8057B370;
void* lbl_806E1BC0;
bool lbl_806E1BC4;
bool lbl_806E1BC5;

extern "C" void fn_802963B4()
{
    gxSetNumChans(1);
    gxSetNumTevStages(lbl_806DF0B8);
    gxSetNumTexGens(3);

    gxSetTevOrder(0, 1, 1, 255);
    gxSetTevOrder(1, 255, 255, 255);
    gxSetTevOrder(2, 2, 2, 255);
    gxSetTevOrder(3, 0, 0, 4);

    gxSetTexCoordGen(0, 1, 5, 30);
    gxSetTexCoordGen(1, 1, 0, 33);
    gxSetTexCoordGen(2, 1, 6, 36);
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
void GXMaterialProgramImpl<GXMaterialProgram_8029F5B0>::Activate(GLView* view)
{
    static_cast<GXMaterialProgram_8029F5B0*>(this)->ConfigureVertexFormat(true);
    fn_802963B4();
    view->m_Interface->GetViewMatrix(lbl_8057B370);
    lbl_806E1BC0 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1BC0, view, 0);
    lbl_806E1BC4 = false;
    fn_801832F4(0, lbl_806E1BC0, 1);
    fn_801836FC(0);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029F5B0>::Deactivate()
{
    if (lbl_806E1BC5)
    {
        fn_80183A98();
        lbl_806E1BC5 = false;
    }

    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTexCoordGen(0, 1, 4, 60);
    gxSetTexCoordGen(1, 1, 5, 60);
    gxSetTexCoordGen(2, 1, 6, 60);

    if (lbl_806E1BC4)
    {
        fn_801832F4(0, lbl_806E1BC0, 1);
        fn_801836FC(0);
        lbl_806E1BC4 = false;
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029F5B0>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

static inline float WrapTextureOffset_802963B4(float value)
{
    int scaled = (int)(value * 1024.0f);
    return (float)(scaled % 1024) * (1.0f / 1024.0f);
}

static inline void LoadScrollingTextureMatrix_802963B4(
    unsigned int matrix, const nlVector2& speed)
{
    float time = fn_80277DB0();
    Mtx textureMatrix;
    memcpy(textureMatrix, lbl_804E8850, sizeof(Mtx));
    textureMatrix[0][3] = WrapTextureOffset_802963B4(time * speed.x);
    textureMatrix[1][3] = WrapTextureOffset_802963B4(time * speed.y);
    GXLoadTexMtxImm(textureMatrix, matrix, GX_MTX_3x4);
}

struct FloatColour_802963B4
{
    float c[4];
};

static inline GXColor ConvertColour_802963B4(
    const FloatColour_802963B4& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029F5B0>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_8029F5B0*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_8029F5B0*>(this)->BindParameters(packet);

    const nlVector3& cameraPosition = cCameraManager::PeekCamera()->GetCameraPosition();
    float reciprocal = 1.0f / *(float*)((unsigned char*)packet->unknown20 + 24);
    float scroll = *(float*)((unsigned char*)packet->unknown20 + 28);
    Mtx cameraTextureMatrix;
    cameraTextureMatrix[0][0] = reciprocal;
    cameraTextureMatrix[0][1] = 0.0f;
    cameraTextureMatrix[0][2] = 0.0f;
    cameraTextureMatrix[0][3] = 0.5f - scroll * cameraPosition.x * reciprocal;
    cameraTextureMatrix[1][0] = 0.0f;
    cameraTextureMatrix[1][1] = reciprocal;
    cameraTextureMatrix[1][2] = 0.0f;
    cameraTextureMatrix[1][3] = 0.5f - scroll * cameraPosition.y * reciprocal;
    cameraTextureMatrix[2][0] = 0.0f;
    cameraTextureMatrix[2][1] = 0.0f;
    cameraTextureMatrix[2][2] = 1.0f;
    cameraTextureMatrix[2][3] = 0.0f;
    GXLoadTexMtxImm(cameraTextureMatrix, 33, GX_MTX_3x4);

    nlVector2 parameterSpeed;
    parameterSpeed.x = *(float*)((unsigned char*)packet->unknown20 + 44);
    parameterSpeed.y = *(float*)((unsigned char*)packet->unknown20 + 48);
    nlVector2 speed;
    nlVec2Set(speed, 0.0f, 0.0f);
    if ((unsigned char*)packet->unknown20 + 52 != 0)
        speed = parameterSpeed;

    LoadScrollingTextureMatrix_802963B4(30, speed);
    LoadScrollingTextureMatrix_802963B4(36, speed);

    if (*(int*)((unsigned char*)packet->unknown20 + 40) == 1)
    {
        if (!lbl_806E1BC4)
        {
            fn_801832F4(1, lbl_806E1BC0, 1);
            fn_801836FC(1);
            lbl_806E1BC4 = true;
        }
    }
    else if (lbl_806E1BC4)
    {
        fn_801832F4(0, lbl_806E1BC0, 1);
        fn_801836FC(0);
        lbl_806E1BC4 = false;
    }

    fn_80183B40(packet->matrix);

    bool enableState = false;
    if (*(int*)((unsigned char*)packet->unknown20 + 56) == 1
        && lbl_806DF0BC)
        enableState = true;

    if (enableState)
    {
        if (!lbl_806E1BC5)
        {
            fn_801837DC(0, 0);
            lbl_806E1BC5 = true;
        }
    }
    else if (lbl_806E1BC5)
    {
        fn_80183A98();
        lbl_806E1BC5 = false;
    }

    if (*(int*)((unsigned char*)packet->unknown20 + 36) == 1)
    {
        UnidentifiedTextureState* texture = (UnidentifiedTextureState*)packet->unknown20;
        texture->SetWrapS(true);
        texture->SetWrapT(true);
    }

    float value = *(float*)((unsigned char*)packet->unknown20 + 32);
    FloatColour_802963B4 colour = { { value, value, value, value } };
    GXColor gxColour = ConvertColour_802963B4(colour);
    GXSetTevKColor(GX_KCOLOR0, gxColour);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_8029F5B0*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_8029F5B0*>(this)->DrawDirect(packet);
}
