#include <revolution/gx.h>

#include <string.h>

#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"

extern "C"
{
    int fn_801820FC();
    int fn_80182104(int);
    int fn_80182118();
    void* fn_80182240(int, int);
    unsigned long fn_80182EB8();
    void fn_80182ED0(void*, GLView*, int);
    void fn_801832F4(int, void*, int);
    void fn_801836FC(int);
    void fn_801837DC(int, int);
    void fn_80183A98();
    void fn_80183B40(unsigned long matrix);
    float fn_80277DB0();
}

bool lbl_806DF078 = true;
bool lbl_806DF079 = true;

const Mtx lbl_804E8790 = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
};

nlMatrix4 lbl_8057B198;
bool lbl_806E1B40;
void* lbl_806E1B44;
int lbl_806E1B48;

extern "C" void fn_80292440(
    int mode, GXMaterialProgram_8029C9F0*)
{
    lbl_806E1B48 = mode;

    if (mode == 1)
        fn_801836FC(1);

    if (mode != 0)
        fn_801832F4(1, lbl_806E1B44, 1);

    if (mode == 2)
    {
        gxSetNumTevStages(7);
        gxSetNumTexGens(5);
        fn_80182118();

        gxSetTevOrder(0, 3, 3, 4);
        gxSetTevOrder(1, 2, 2, 255);
        gxSetTevOrder(2, 1, 1, 255);
        gxSetTevOrder(3, 0, 0, 255);
        gxSetTevOrder(4, 255, 255, 255);
        gxSetTevOrder(5, 3, 3, 255);
        gxSetTevOrder(6, 255, 255, 255);
        gxSetTexCoordGen(3, 10, 19, 60);

        gxSetTevColourOp(0, 0, 0, 0, true, 1);
        gxSetTevColourIn(0, 15, 15, 15, 8);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
        gxSetTevColourOp(1, 0, 0, 0, true, 2);
        gxSetTevColourIn(1, 15, 14, 8, 15);
        gxSetTevAlphaIn(1, 7, 7, 7, 7);
        gxSetTevColourIn(2, 15, 8, 4, 15);
        gxSetTevAlphaIn(2, 7, 7, 7, 7);
        gxSetTevColourIn(3, 8, 15, 4, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 4);
        gxSetTevColourOp(4, 0, 0, 0, true, 1);
        gxSetTevColourIn(4, 15, 0, 2, 15);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
        GXSetTevKColorSel(GX_TEVSTAGE5, GX_TEV_KCSEL_K1);
        gxSetTevColourIn(5, 12, 8, 14, 15);
        gxSetTevAlphaIn(5, 7, 7, 7, 0);
        gxSetTevColourIn(6, 15, 2, 0, 15);
        gxSetTevAlphaIn(6, 7, 7, 7, 0);
    }
    else
    {
        gxSetNumTevStages(6);
        gxSetNumTexGens(4);

        gxSetTevOrder(0, 2, 2, 255);
        gxSetTevOrder(1, 1, 1, 255);
        gxSetTevOrder(2, 0, 0, 255);
        gxSetTevOrder(3, 255, 255, 4);
        gxSetTevOrder(4, 3, 3, 255);
        gxSetTevOrder(5, 255, 255, 255);

        gxSetTevColourOp(0, 0, 0, 0, true, 1);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(0, 15, 14, 8, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevColourIn(1, 15, 8, 2, 15);
        gxSetTevAlphaIn(1, 7, 7, 7, 7);
        gxSetTevColourIn(2, 8, 15, 2, 0);
        gxSetTevAlphaIn(2, 7, 7, 7, 4);
        gxSetTevColourOp(3, 0, 0, 0, true, 1);
        gxSetTevColourIn(3, 15, 0, 10, 15);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
        GXSetTevKColorSel(GX_TEVSTAGE4, GX_TEV_KCSEL_K1);
        gxSetTevColourIn(4, 12, 8, 14, 15);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
        gxSetTevColourIn(5, 15, 2, 0, 15);
        gxSetTevAlphaIn(5, 7, 7, 7, 0);
    }

    gxSetTexCoordGen(0, 0, 4, 30);
    gxSetTexCoordGen(1, 0, 5, 33);
    gxSetTexCoordGen(2, 0, 6, 36);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029C9F0>::Activate(GLView* view)
{
    lbl_806E1B48 = 3;
    static_cast<GXMaterialProgram_8029C9F0*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057B198);
    lbl_806E1B44 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1B44, view, 0);
    gxSetNumChans(1);

    UnidentifiedTextureState texture;
    texture.texture = 0;
    texture.textureIndex = 0xFFFF;
    texture.flags = 0;
    texture.unknown07 = 0;
    texture.texture = fn_80182EB8();
    texture.textureIndex = 0xFFFF;
    texture.SetWrapS(true);
    texture.SetWrapT(true);
    texture.unknown07 = 0;
    fn_8036BE88(3, &texture);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029C9F0>::Deactivate()
{
    if (lbl_806E1B40)
    {
        fn_80183A98();
        lbl_806E1B40 = false;
    }

    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTexCoordGen(0, 0, 4, 60);
    gxSetTexCoordGen(1, 0, 5, 60);
    gxSetTexCoordGen(2, 0, 6, 60);
    fn_801832F4(0, lbl_806E1B44, 1);

    if (lbl_806E1B48 == 2)
    {
        gxSetTexCoordGen(3, 1, 7, 60);
        gxSetTevColourOp(0, 0, 0, 0, true, 0);
        gxSetTevColourOp(1, 0, 0, 0, true, 0);
        gxSetTevColourOp(4, 0, 0, 0, true, 0);
    }
    else
    {
        gxSetTevColourOp(0, 0, 0, 0, true, 0);
        gxSetTevColourOp(3, 0, 0, 0, true, 0);
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029C9F0>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_80292440
{
    float c[4];
};

static inline GXColor ConvertColour_80292440(
    const FloatColour_80292440& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

static inline float WrapTextureOffset_80292440(float value)
{
    int scaled = (int)(value * 1024.0f);
    return (float)(scaled % 1024) * (1.0f / 1024.0f);
}

static inline void LoadScrollingTextureMatrix_80292440(
    unsigned int matrix, float speedX, float speedY)
{
    float time = fn_80277DB0();
    Mtx textureMatrix;
    memcpy(textureMatrix, lbl_804E8790, sizeof(Mtx));
    textureMatrix[0][3] = WrapTextureOffset_80292440(time * speedX);
    textureMatrix[1][3] = WrapTextureOffset_80292440(time * speedY);
    GXLoadTexMtxImm(textureMatrix, matrix, GX_MTX3x4);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029C9F0>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DF078)
        return;

    unsigned char* parameters = (unsigned char*)packet->unknown20;
    float alpha = 1.0f - *(float*)(parameters + 56);

    int mode;
    if (*(int*)(parameters + 60) == 0 || fn_801820FC() == 0)
    {
        mode = 0;
    }
    else
    {
        mode = 1;
        if (fn_80182104(0) != 0)
            mode = 2;
    }

    if (lbl_806E1B48 != mode)
    {
        Deactivate();
        fn_80292440(mode, static_cast<GXMaterialProgram_8029C9F0*>(this));
    }

    FloatColour_80292440 colour0 = { { alpha, alpha, alpha, alpha } };
    GXColor gxColour0 = ConvertColour_80292440(colour0);
    GXSetTevKColor(GX_KCOLOR0, gxColour0);

    float value = *(float*)(parameters + 64);
    FloatColour_80292440 colour1 = { { value, value, value, value } };
    GXColor gxColour1 = ConvertColour_80292440(colour1);
    GXSetTevKColor(GX_KCOLOR1, gxColour1);

    static_cast<GXMaterialProgram_8029C9F0*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_8029C9F0*>(this)->BindParameters(packet);

    LoadScrollingTextureMatrix_80292440(
        30, *(float*)(parameters + 32), *(float*)(parameters + 36));
    LoadScrollingTextureMatrix_80292440(
        33, *(float*)(parameters + 40), *(float*)(parameters + 44));
    LoadScrollingTextureMatrix_80292440(
        36, *(float*)(parameters + 48), *(float*)(parameters + 52));

    fn_80183B40(packet->matrix);

    bool enableState = false;
    if (*(int*)(parameters + 68) == 1 && lbl_806DF079)
        enableState = true;

    if (enableState)
    {
        if (!lbl_806E1B40)
        {
            fn_801837DC(0, 0);
            lbl_806E1B40 = true;
        }
    }
    else if (lbl_806E1B40)
    {
        fn_80183A98();
        lbl_806E1B40 = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_8029C9F0*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_8029C9F0*>(this)->DrawDirect(packet);
}
