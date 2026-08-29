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

bool lbl_806DF088 = true;

const Mtx lbl_804E87C0 = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
};

nlMatrix4 lbl_8057B218;
bool lbl_806E1B60;
void* lbl_806E1B64;
int lbl_806E1B68;

extern "C" void fn_802938B8(
    int mode, GXMaterialProgram_8029D7E0*)
{
    lbl_806E1B68 = mode;

    if (mode == 1)
        fn_801836FC(1);

    if (mode != 0)
        fn_801832F4(1, lbl_806E1B64, 1);

    if (mode == 2)
    {
        gxSetNumTevStages(5);
        gxSetNumTexGens(4);
        int useSubtract = fn_80182118() != 0;

        gxSetTevOrder(0, 3, 3, 4);
        gxSetTevOrder(1, 2, 2, 255);
        gxSetTevOrder(2, 1, 1, 255);
        gxSetTevOrder(3, 0, 0, 255);
        gxSetTevOrder(4, 255, 255, 255);
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
        gxSetTevColourOp(4, 0, 0, useSubtract, true, 0);
        gxSetTevColourIn(4, 15, 0, 2, 15);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
    }
    else
    {
        gxSetNumTevStages(4);
        gxSetNumTexGens(3);
        gxSetTevOrder(0, 2, 2, 255);
        gxSetTevOrder(1, 1, 1, 255);
        gxSetTevOrder(2, 0, 0, 255);
        gxSetTevOrder(3, 255, 255, 4);
        gxSetTevColourOp(0, 0, 0, 0, true, 1);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(0, 15, 14, 8, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevColourIn(1, 15, 8, 2, 15);
        gxSetTevAlphaIn(1, 7, 7, 7, 7);
        gxSetTevColourIn(2, 8, 15, 2, 0);
        gxSetTevAlphaIn(2, 7, 7, 7, 4);
        gxSetTevColourIn(3, 15, 0, 10, 15);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
    }

    gxSetTexCoordGen(0, 0, 4, 30);
    gxSetTexCoordGen(1, 0, 5, 33);
    gxSetTexCoordGen(2, 0, 6, 36);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029D7E0>::Activate(GLView* view)
{
    lbl_806E1B68 = 3;
    static_cast<GXMaterialProgram_8029D7E0*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057B218);
    lbl_806E1B64 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1B64, view, 0);
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
void GXMaterialProgramImpl<GXMaterialProgram_8029D7E0>::Deactivate()
{
    if (lbl_806E1B60)
    {
        fn_80183A98();
        lbl_806E1B60 = false;
    }

    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    fn_801832F4(0, lbl_806E1B64, 1);
    gxSetTexCoordGen(0, 0, 4, 60);
    gxSetTexCoordGen(1, 0, 5, 60);
    gxSetTexCoordGen(2, 0, 6, 60);

    if (lbl_806E1B68 == 2)
    {
        gxSetTexCoordGen(3, 1, 7, 60);
        gxSetTevColourOp(0, 0, 0, 0, true, 0);
        gxSetTevColourOp(1, 0, 0, 0, true, 0);
        gxSetTevColourOp(4, 0, 0, 0, true, 0);
    }
    else
    {
        gxSetTevColourOp(0, 0, 0, 0, true, 0);
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029D7E0>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_802938B8
{
    float c[4];
};

static inline GXColor ConvertColour_802938B8(
    const FloatColour_802938B8& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

static inline float WrapTextureOffset_802938B8(float value)
{
    int scaled = (int)(value * 1024.0f);
    return (float)(scaled % 1024) * (1.0f / 1024.0f);
}

static inline void LoadScrollingTextureMatrix_802938B8(
    unsigned int matrix, float speedX, float speedY)
{
    float time = fn_80277DB0();
    Mtx textureMatrix;
    memcpy(textureMatrix, lbl_804E87C0, sizeof(Mtx));
    textureMatrix[0][3] = WrapTextureOffset_802938B8(time * speedX);
    textureMatrix[1][3] = WrapTextureOffset_802938B8(time * speedY);
    GXLoadTexMtxImm(textureMatrix, matrix, GX_MTX3x4);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029D7E0>::Draw(
    const glModelPacket* packet)
{
    unsigned char* parameters = (unsigned char*)packet->unknown20;
    float alpha = 1.0f - *(float*)(parameters + 48);

    int mode;
    if (*(int*)(parameters + 52) == 0 || fn_801820FC() == 0)
    {
        mode = 0;
    }
    else
    {
        mode = 1;
        if (fn_80182104(0) != 0)
            mode = 2;
    }

    if (lbl_806E1B68 != mode)
    {
        Deactivate();
        fn_802938B8(mode, static_cast<GXMaterialProgram_8029D7E0*>(this));
    }

    FloatColour_802938B8 colour = { { alpha, alpha, alpha, alpha } };
    GXColor gxColour = ConvertColour_802938B8(colour);
    GXSetTevKColor(GX_KCOLOR0, gxColour);

    static_cast<GXMaterialProgram_8029D7E0*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_8029D7E0*>(this)->BindParameters(packet);

    LoadScrollingTextureMatrix_802938B8(
        30, *(float*)(parameters + 24), *(float*)(parameters + 28));
    LoadScrollingTextureMatrix_802938B8(
        33, *(float*)(parameters + 32), *(float*)(parameters + 36));
    LoadScrollingTextureMatrix_802938B8(
        36, *(float*)(parameters + 40), *(float*)(parameters + 44));

    fn_80183B40(packet->matrix);

    bool enableState = false;
    if (*(int*)(parameters + 56) == 1 && lbl_806DF088)
        enableState = true;

    if (enableState)
    {
        if (!lbl_806E1B60)
        {
            fn_801837DC(0, 0);
            lbl_806E1B60 = true;
        }
    }
    else if (lbl_806E1B60)
    {
        fn_80183A98();
        lbl_806E1B60 = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_8029D7E0*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_8029D7E0*>(this)->DrawDirect(packet);
}
