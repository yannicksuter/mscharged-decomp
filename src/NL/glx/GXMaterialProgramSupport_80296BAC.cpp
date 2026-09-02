#include <revolution/gx.h>

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

enum MaterialMode_80296BAC
{
    MATERIAL_MODE_0_80296BAC = 0,
    MATERIAL_MODE_1_80296BAC = 1,
    MATERIAL_MODE_2_80296BAC = 2,
    MATERIAL_MODE_3_80296BAC = 3,
};

struct GXMaterialProgramParameters_8029FC0C
{
    /* 0x00 */ UnidentifiedTextureState texture0;
    /* 0x08 */ UnidentifiedTextureState texture1;
    /* 0x10 */ float value16;
    /* 0x14 */ float value20;
    /* 0x18 */ nlFloatColour colour24;
    /* 0x28 */ float value40;
    /* 0x2C */ float value44;
    /* 0x30 */ int value48;
    /* 0x34 */ int value52;
    /* 0x38 */ int value56;
}; // size: 0x3C

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

static inline float WrapTextureOffset_80296BAC(float value)
{
    return (float)((int)(value * 1024.0f) % 1024) / 1024.0f;
}

static inline void LoadScrollingTextureMatrix_80296BAC(
    unsigned int matrix, const nlVector2& speed)
{
    float time = fn_80277DB0();
    nlVector2 offset;
    offset.x = WrapTextureOffset_80296BAC(time * speed.x);
    offset.y = WrapTextureOffset_80296BAC(time * speed.y);
    Mtx textureMatrix = {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
    };
    textureMatrix[0][3] = offset.x;
    textureMatrix[1][3] = offset.y;
    GXLoadTexMtxImm(textureMatrix, matrix, GX_MTX3x4);
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

    MaterialMode_80296BAC mode;
    if (((GXMaterialProgramParameters_8029FC0C*)packet->unknown20)->value52 == 0
        || fn_801820FC() == 0)
    {
        mode = MATERIAL_MODE_0_80296BAC;
    }
    else if (fn_80182104(1) != 0)
    {
        mode = MATERIAL_MODE_3_80296BAC;
    }
    else
    {
        mode = (fn_80182118() != 0)
            ? MATERIAL_MODE_2_80296BAC
            : MATERIAL_MODE_1_80296BAC;
    }

    if (mode != lbl_806E1BE0[0])
    {
        Deactivate();
        fn_80296BAC(mode);
    }

    if (lbl_806E1BE0[0] == 3)
    {
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
        fn_8036BE88(2, &texture);
    }

    float alpha = lbl_806DF0C1
        ? ((GXMaterialProgramParameters_8029FC0C*)packet->unknown20)->value16
        : 0.0f;
    GXMaterialProgramParameters_8029FC0C* parameters =
        (GXMaterialProgramParameters_8029FC0C*)packet->unknown20;
    float lighting = parameters->value20;
    int useTextureMatrix = parameters->value48;

    nlFloatColour colour0 = { { alpha, alpha, alpha, alpha } };
    nlColour gxColour0;
    ConvertColour(gxColour0, colour0);
    GXSetTevKColor(GX_KCOLOR0, *(GXColor*)&gxColour0);

    nlFloatColour& colour1 = parameters->colour24;
    nlColour gxColour1;
    ConvertColour(gxColour1, colour1);
    GXSetTevKColor(GX_KCOLOR1, *(GXColor*)&gxColour1);

    nlVector2 speed;
    speed.x = lbl_806DF0C2
        ? parameters->value40
        : 0.0f;
    speed.y = lbl_806DF0C2
        ? parameters->value44
        : 0.0f;
    LoadScrollingTextureMatrix_80296BAC(30, speed);

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
    if (((GXMaterialProgramParameters_8029FC0C*)packet->unknown20)->value56 == 1
        && lbl_806DF0C3)
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
