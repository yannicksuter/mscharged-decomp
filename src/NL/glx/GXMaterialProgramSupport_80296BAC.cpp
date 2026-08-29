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
    void* fn_8018230C(int, int);
    unsigned long fn_80182EB8();
    void fn_80182ED0(void*, GLView*, int);
    void fn_801832F4(int, void*, int);
    void fn_801833D0(int, void*, float, nlMatrix4*);
    void fn_80183654(int, void*);
    void fn_801836FC(int);
    void fn_801837DC(int, int);
    void fn_80183A98();
    void fn_80183B40(unsigned long matrix);
    float fn_80277DB0();
}

bool lbl_806DF0C0 = true;
bool lbl_806DF0C1 = true;
bool lbl_806DF0C2 = true;
bool lbl_806DF0C3 = true;

const Mtx lbl_804E8880 = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
};

nlMatrix4 lbl_8057B3B0;
bool lbl_806E1BC8;
void* lbl_806E1BCC;
nlVector2 lbl_806E1BD0;
nlVector2 lbl_806E1BD8;
int lbl_806E1BE0[2];

extern "C" void fn_80296BAC(int mode)
{
    lbl_806E1BE0[0] = mode;
    gxSetTexCoordGen(0, 0, 4, 30);
    fn_80183654(1, lbl_806E1BCC);
    fn_801832F4(mode != 0, lbl_806E1BCC, 1);

    if (mode == 3)
    {
        int useSubtract = fn_80182118() != 0;
        gxSetNumChans(2);
        gxSetNumTexGens(3);
        gxSetNumTevStages(5);
        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 255, 255, 5);
        gxSetTevOrder(2, 2, 2, 4);
        gxSetTevOrder(3, 0, 0, 255);
        gxSetTevOrder(4, 255, 255, 255);
        gxSetTexCoordGen(2, 10, 19, 60);
        gxSetTevColourOp(1, 0, 0, 0, true, 1);
        gxSetTevColourOp(3, 0, 0, useSubtract, true, 0);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE4, GX_TEV_KCSEL_K1);
        gxSetTevColourIn(0, 15, 14, 8, 15);
        gxSetTevColourIn(1, 15, 0, 10, 15);
        gxSetTevColourIn(2, 15, 15, 15, 8);
        gxSetTevColourIn(3, 15, 0, 8, 15);
        gxSetTevColourIn(4, 15, 14, 2, 0);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 7);
        gxSetTevAlphaIn(2, 7, 5, 4, 7);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
    }
    else
    {
        if ((unsigned int)(mode - 1) <= 1)
            fn_801836FC(1);

        if (mode == 2)
        {
            gxSetNumChans(2);
            gxSetNumTexGens(2);
            gxSetNumTevStages(5);
            gxSetTevOrder(0, 1, 1, 255);
            gxSetTevOrder(1, 255, 255, 5);
            gxSetTevOrder(2, 255, 255, 255);
            gxSetTevOrder(3, 0, 0, 4);
            gxSetTevOrder(4, 255, 255, 255);
            gxSetTevColourOp(2, 0, 0, 0, true, 1);
            gxSetTevColourOp(3, 0, 0, 1, true, 0);
            GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
            GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K1);
            gxSetTevColourIn(0, 15, 14, 8, 15);
            gxSetTevColourIn(1, 15, 0, 10, 15);
            gxSetTevColourIn(2, 15, 0, 14, 15);
            gxSetTevColourIn(3, 15, 10, 8, 15);
            gxSetTevColourIn(4, 15, 12, 0, 2);
            gxSetTevAlphaIn(0, 7, 7, 7, 7);
            gxSetTevAlphaIn(1, 7, 7, 7, 7);
            gxSetTevAlphaIn(2, 7, 7, 7, 7);
            gxSetTevAlphaIn(3, 7, 5, 4, 7);
            gxSetTevAlphaIn(4, 7, 7, 7, 0);
        }
        else
        {
            gxSetNumChans(2);
            gxSetNumTexGens(2);
            gxSetNumTevStages(4);
            gxSetTevOrder(0, 1, 1, 255);
            gxSetTevOrder(1, 255, 255, 5);
            gxSetTevOrder(2, 255, 255, 255);
            gxSetTevOrder(3, 0, 0, 4);
            GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
            GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K1);
            gxSetTevColourIn(0, 15, 14, 8, 15);
            gxSetTevColourIn(1, 15, 0, 10, 15);
            gxSetTevColourIn(2, 15, 0, 14, 15);
            gxSetTevColourIn(3, 15, 10, 8, 0);
            gxSetTevAlphaIn(0, 7, 7, 7, 7);
            gxSetTevAlphaIn(1, 7, 7, 7, 7);
            gxSetTevAlphaIn(2, 7, 7, 7, 7);
            gxSetTevAlphaIn(3, 7, 5, 4, 7);
        }
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029FC0C>::Activate(GLView* view)
{
    lbl_806E1BE0[0] = 4;
    lbl_806E1BD8.x = 10000000000.0f;
    lbl_806E1BD8.y = 10000000000.0f;
    static_cast<GXMaterialProgram_8029FC0C*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057B3B0);
    lbl_806E1BCC = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1BCC, view, 0);
    lbl_806E1BD0.x = 0.0f;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029FC0C>::Deactivate()
{
    if (lbl_806E1BC8)
    {
        fn_80183A98();
        lbl_806E1BC8 = false;
    }

    gxSetTexCoordGen(0, 1, 4, 60);
    gxSetTexCoordGen(1, 1, 5, 60);
    fn_801836FC(0);
    gxSetNumChans(1);
    fn_801832F4(0, lbl_806E1BCC, 1);
    fn_80183654(0, lbl_806E1BCC);

    if (lbl_806E1BE0[0] == 3)
    {
        gxSetTexCoordGen(2, 1, 6, 60);
        gxSetTevColourOp(1, 0, 0, 0, true, 0);
        gxSetTevColourOp(3, 0, 0, 0, true, 0);
    }
    else if (lbl_806E1BE0[0] == 2)
    {
        gxSetTevColourOp(2, 0, 0, 0, true, 0);
        gxSetTevColourOp(3, 0, 0, 0, true, 0);
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029FC0C>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_80296BAC
{
    float c[4];
};

static inline GXColor ConvertColour_80296BAC(
    const FloatColour_80296BAC& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

static inline float WrapTextureOffset_80296BAC(float value)
{
    int scaled = (int)(value * 1024.0f);
    return (float)(scaled % 1024) * (1.0f / 1024.0f);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029FC0C>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DF0C0)
        return;

    GXMaterialProgram_8029FC0C* program = static_cast<GXMaterialProgram_8029FC0C*>(this);
    program->BindVertexArrays(packet);
    program->BindParameters(packet);
    unsigned char* parameters = (unsigned char*)packet->unknown20;

    int mode;
    if (*(int*)(parameters + 52) == 0 || fn_801820FC() == 0)
    {
        mode = 0;
    }
    else if (fn_80182104(1) != 0)
    {
        mode = 3;
    }
    else
    {
        mode = 1;
        if (fn_80182118() != 0)
            mode = 2;
    }

    if (lbl_806E1BE0[0] != mode)
    {
        Deactivate();
        fn_80296BAC(mode);
    }

    if (lbl_806E1BE0[0] == 3)
    {
        UnidentifiedTextureState texture;
        texture.texture = fn_80182EB8();
        texture.textureIndex = 0xFFFF;
        texture.flags = 3;
        texture.unknown07 = 0;
        fn_8036BE88(2, &texture);
    }

    float alpha = lbl_806DF0C1 ? *(float*)(parameters + 16) : 0.0f;
    float lighting = *(float*)(parameters + 20);
    int useTextureMatrix = *(int*)(parameters + 48);

    FloatColour_80296BAC colour0 = { { alpha, alpha, alpha, alpha } };
    GXColor gxColour0 = ConvertColour_80296BAC(colour0);
    GXSetTevKColor(GX_KCOLOR0, gxColour0);

    FloatColour_80296BAC& colour1 = *(FloatColour_80296BAC*)(parameters + 24);
    GXColor gxColour1 = ConvertColour_80296BAC(colour1);
    GXSetTevKColor(GX_KCOLOR1, gxColour1);

    float speedX = lbl_806DF0C2 ? *(float*)(parameters + 40) : 0.0f;
    float speedY = lbl_806DF0C2 ? *(float*)(parameters + 44) : 0.0f;
    float time = fn_80277DB0();
    float offsetX = WrapTextureOffset_80296BAC(time * speedX);
    float offsetY = WrapTextureOffset_80296BAC(time * speedY);
    Mtx textureMatrix;
    memcpy(textureMatrix, lbl_804E8880, sizeof(Mtx));
    textureMatrix[0][3] = offsetX;
    textureMatrix[1][3] = offsetY;
    GXLoadTexMtxImm(textureMatrix, 30, GX_MTX_3x4);

    if (useTextureMatrix == 1)
        gxSetTexCoordGen(1, 0, 5, 30);
    else
        gxSetTexCoordGen(1, 1, 5, 60);

    if (lbl_806E1BD0.x != lighting && alpha != 0.0f)
    {
        lbl_806E1BD0.x = lighting;
        for (int i = 0; i < (int)lbl_806E1BCC; ++i)
        {
            void* light = fn_8018230C(i, 0);
            fn_801833D0(i, light, lighting, &lbl_8057B3B0);
        }
    }

    fn_80183B40(packet->matrix);

    bool enableState = false;
    if (*(int*)(parameters + 56) == 1 && lbl_806DF0C3)
        enableState = true;

    if (enableState)
    {
        if (!lbl_806E1BC8)
        {
            fn_801837DC(0, 0);
            lbl_806E1BC8 = true;
        }
    }
    else if (lbl_806E1BC8)
    {
        fn_80183A98();
        lbl_806E1BC8 = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        program->DrawIndexed(packet);
    else
        program->DrawDirect(packet);
}
