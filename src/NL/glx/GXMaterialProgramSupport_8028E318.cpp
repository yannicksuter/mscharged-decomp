#include <revolution/gx.h>

#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"
#include "NL/nlMath.h"

extern "C"
{
    int fn_80182118();
    void* fn_80182240(int, int);
    void* fn_8018230C(int, int);
    void fn_80182ED0(void*, GLView*, int);
    void fn_801832F4(int, void*, int);
    void fn_801833D0(int, void*, float, const nlMatrix4*);
    void fn_80183654(int, void*);
    void fn_801836FC(int);
    void fn_801837DC(int, unsigned long);
    void fn_80183A98();
    void fn_80183B40(unsigned long matrix);
    void fn_80183BF4(const nlMatrix4* matrix);
    void fn_8036D774(const nlMatrix4* matrix);
    void fn_8036D7EC(
        const void* matrices, unsigned long count, const nlMatrix4* matrix,
        int unknown);
}

struct GXMaterialProgramParameters_80299490
{
    /* 0x00 */ UnidentifiedTextureState texture0;
    /* 0x08 */ UnidentifiedTextureState texture1;
    /* 0x10 */ UnidentifiedTextureState texture2;
    /* 0x18 */ UnidentifiedTextureState texture3;
    /* 0x20 */ const void* matrices;
    /* 0x24 */ unsigned long matricesSize;
    /* 0x28 */ float value40;
    /* 0x2C */ float value44;
    /* 0x30 */ float value48;
    /* 0x34 */ float value52;
    /* 0x38 */ float colour56[4];
    /* 0x48 */ float value72;
    /* 0x4C */ unsigned long value76;
    /* 0x50 */ int value80;
}; // size: 0x54

static nlMatrix4 lbl_8057AF40;
static void* lbl_806E1AE8;
static float lbl_806E1AEC;
static bool lbl_806E1AF0;

extern "C" void fn_8028E318(bool enabled)
{
    unsigned int numChans;
    unsigned int numTexGens;
    unsigned int numTevStages;

    fn_80183654(1, lbl_806E1AE8);
    if (enabled)
    {
        fn_801836FC(1);
        fn_801832F4(enabled, lbl_806E1AE8, 0);
        bool subtract = fn_80182118() != 0;
        numChans = 2;
        numTexGens = 4;
        numTevStages = 6;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 4);
        gxSetTevOrder(3, 2, 2, 255);
        gxSetTevOrder(4, 255, 255, 5);
        gxSetTevOrder(5, 3, 3, 255);
        gxSetTevColourOp(2, 0, 0, subtract, true, 1);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
        GXSetTevKColorSel(GX_TEVSTAGE5, GX_TEV_KCSEL_K3);
        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 10, 15);
        gxSetTevColourIn(3, 15, 14, 8, 15);
        gxSetTevColourIn(4, 15, 0, 10, 2);
        gxSetTevColourIn(5, 0, 8, 14, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
        gxSetTevAlphaIn(5, 7, 7, 7, 0);
    }
    else
    {
        numChans = 2;
        numTexGens = 4;
        numTevStages = 6;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 255);
        gxSetTevOrder(3, 2, 2, 255);
        gxSetTevOrder(4, 255, 255, 5);
        gxSetTevOrder(5, 3, 3, 255);
        gxSetTevColourOp(2, 0, 0, 0, true, 1);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
        GXSetTevKColorSel(GX_TEVSTAGE5, GX_TEV_KCSEL_K3);
        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 12, 15);
        gxSetTevColourIn(3, 15, 14, 8, 15);
        gxSetTevColourIn(4, 15, 0, 10, 2);
        gxSetTevColourIn(5, 0, 8, 14, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
        gxSetTevAlphaIn(5, 7, 7, 7, 0);
    }

    gxSetNumChans(numChans);
    gxSetNumTexGens(numTexGens);
    gxSetNumTevStages(numTevStages);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80299490>::Activate(GLView* view)
{
    lbl_806E1AEC = 0.0f;
    static_cast<GXMaterialProgram_80299490*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057AF40);
    fn_80183BF4(&lbl_8057AF40);
    fn_80183B40(-1);
    lbl_806E1AE8 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1AE8, view, 0);
    lbl_806E1AF0 = true;
    fn_8028E318(true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80299490>::Deactivate()
{
    gxSetCurrentMtx(0, true);
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
    fn_801836FC(0);
    fn_801832F4(0, lbl_806E1AE8, 1);
    fn_80183654(0, lbl_806E1AE8);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80299490>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_8028E318
{
    float c[4];
};

static inline GXColor ConvertColour_8028E318(
    const FloatColour_8028E318& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80299490>::Draw(
    const glModelPacket* packet)
{
    GXMaterialProgramParameters_80299490* parameters = (GXMaterialProgramParameters_80299490*)packet->unknown20;
    float value44 = parameters->value44;
    FloatColour_8028E318 colour56 = { { parameters->colour56[0],
        parameters->colour56[1],
        parameters->colour56[2],
        parameters->colour56[3] } };
    float value40 = parameters->value40;
    float value72 = parameters->value72;
    float value52 = parameters->value52;
    float value48 = parameters->value48;

    if (value44 == 0.0f)
        return;

    FloatColour_8028E318 source40 = { { value40, value40, value40, value40 } };
    GXColor gxColour40 = ConvertColour_8028E318(source40);
    GXSetTevKColor(GX_KCOLOR0, gxColour40);
    FloatColour_8028E318 source44 = { { value44, value44, value44, value44 } };
    GXColor gxColour44 = ConvertColour_8028E318(source44);
    GXSetTevKColor(GX_KCOLOR1, gxColour44);

    colour56.c[0] *= value48;
    colour56.c[1] *= value48;
    colour56.c[2] *= value48;
    colour56.c[3] *= value48;
    GXColor gxColour56 = ConvertColour_8028E318(colour56);
    GXSetTevKColor(GX_KCOLOR2, gxColour56);

    FloatColour_8028E318 source72 = { { value72, value72, value72, value72 } };
    GXColor gxColour72 = ConvertColour_8028E318(source72);
    GXSetTevKColor(GX_KCOLOR3, gxColour72);

    if (lbl_806E1AEC != value52)
    {
        lbl_806E1AEC = value52;
        for (int i = 0; i < (int)lbl_806E1AE8; ++i)
        {
            void* value = fn_8018230C(i, 0);
            fn_801833D0(i, value, value52, &lbl_8057AF40);
        }
    }

    static_cast<GXMaterialProgram_80299490*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_80299490*>(this)->BindParameters(packet);

    bool enabled = parameters->value80 == 1;
    if (enabled != lbl_806E1AF0)
    {
        lbl_806E1AF0 = enabled;
        Deactivate();
        fn_8028E318(enabled);
    }

    gxSetNumTevStages(value72 == 0.0f ? 5 : 6);

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, lbl_8057AF40);
    fn_80183B40(packet->matrix);

    if (packet->unknown28 == 0)
    {
        fn_8036D7EC(parameters->matrices, parameters->matricesSize / 48, &modelview, 0);
    }
    else
    {
        fn_8036D774(&modelview);
    }

    fn_801837DC(1, parameters->value76);

    if (value44 != 1.0f)
    {
        gxSaveZMode();
        bool colourUpdate = gxSetColourUpdate(false);
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
        gxSetColourUpdate(colourUpdate);
        gxSetZMode(true, GX_EQUAL, true);
        gxSaveBlendMode();
        gxSetBlendMode(true, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, false);
        GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K1_A);
        gxSetTevAlphaIn(1, 7, 7, 7, 6);
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
        gxRestoreBlendMode();
        gxRestoreZMode();
    }
    else
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }

    fn_80183A98();
}
