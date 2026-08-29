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
    unsigned long fn_80182EB8();
    void fn_80182ED0(void*, GLView*, int);
    void fn_801832F4(int, void*, int);
    void fn_801836FC(int);
    void fn_801837DC(int, unsigned long);
    void fn_80183A98();
    void fn_80183B40(unsigned long matrix);
    void fn_80183BF4(const nlMatrix4* matrix);
    void* fn_80364020();
    void fn_803640E0(void*, const char*);
    void fn_8036D774(const nlMatrix4* matrix);
    void fn_8036D7EC(
        const void* matrices, unsigned long count, const nlMatrix4* matrix,
        int unknown);
}

struct GXMaterialProgramParameters_802981F0
{
    /* 0x00 */ UnidentifiedTextureState texture0;
    /* 0x08 */ UnidentifiedTextureState texture1;
    /* 0x10 */ const void* matrices;
    /* 0x14 */ unsigned long matricesSize;
    /* 0x18 */ float value24;
    /* 0x1C */ float value28;
    /* 0x20 */ unsigned long value32;
    /* 0x24 */ int value36;
}; // size: 0x28

static bool lbl_806DEFE0 = true;
static bool lbl_806DEFE1 = true;
static bool lbl_806DEFE2 = true;
static float lbl_806DEFE4 = 1.0f;
static float lbl_806DEFE8 = 1.0f;
static bool lbl_806DEFEC = true;

static nlMatrix4 lbl_8057AD48;
static bool lbl_806E1A50;
static void* lbl_806E1A54;
static bool lbl_806E1A58;

extern "C" void fn_8028A98C(bool enabled)
{
    unsigned int numChans;
    unsigned int numTexGens;
    unsigned int numTevStages;

    if (enabled)
        fn_801832F4(1, lbl_806E1A54, 0);

    if (enabled && lbl_806E1A50)
    {
        numChans = 1;
        numTexGens = 3;
        numTevStages = 3;
        bool subtract = lbl_806DEFE2;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 2, 2, 4);
        gxSetTexCoordGen(2, 10, 19, 60);
        gxSetTevColourOp(2, 0, 0, subtract, true, 0);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 8, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
    }
    else if (enabled)
    {
        fn_801836FC(1);
        numChans = 1;
        numTexGens = 2;
        numTevStages = 3;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 4);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        if (fn_80182118() != 0)
            gxSetTevColourOp(2, 0, 0, 1, true, 0);
        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 10, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
    }
    else
    {
        numChans = 0;
        numTexGens = 2;
        numTevStages = 3;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 255);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 12, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
    }

    gxSetNumChans(numChans);
    gxSetNumTexGens(numTexGens);
    gxSetNumTevStages(numTevStages);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802981F0>::Activate(GLView* view)
{
    static_cast<GXMaterialProgram_802981F0*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057AD48);
    fn_80183BF4(&lbl_8057AD48);
    fn_80183B40(-1);
    lbl_806E1A54 = fn_80182240(1, 0);
    fn_80182ED0(lbl_806E1A54, view, 1);
    lbl_806E1A58 = lbl_806DEFE1;
    fn_8028A98C(lbl_806DEFE1);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802981F0>::Deactivate()
{
    gxSetCurrentMtx(0, true);
    fn_801836FC(0);
    fn_801832F4(0, lbl_806E1A54, 1);
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
    gxSetTexCoordGen(2, 1, 6, 60);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802981F0>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_8028A98C
{
    float c[4];
};

static inline GXColor ConvertColour_8028A98C(
    const FloatColour_8028A98C& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802981F0>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DEFE0)
        return;

    GXMaterialProgramParameters_802981F0* parameters = (GXMaterialProgramParameters_802981F0*)packet->unknown20;
    float value24 = lbl_806DEFE4;
    if (value24 == 1.0f)
        value24 = parameters->value24;
    float value28 = lbl_806DEFE8;
    if (value28 == 1.0f)
        value28 = parameters->value28;
    if (value28 == 0.0f)
        return;

    FloatColour_8028A98C source24 = { { value24, value24, value24, value24 } };
    GXColor colour24 = ConvertColour_8028A98C(source24);
    GXSetTevKColor(GX_KCOLOR0, colour24);
    FloatColour_8028A98C source28 = { { value28, value28, value28, value28 } };
    GXColor colour28 = ConvertColour_8028A98C(source28);
    GXSetTevKColor(GX_KCOLOR1, colour28);

    static_cast<GXMaterialProgram_802981F0*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802981F0*>(this)->BindParameters(packet);

    bool enabled = parameters->value36 == 1;
    if (enabled != lbl_806E1A58)
    {
        lbl_806E1A58 = enabled;
        Deactivate();
        fn_8028A98C(enabled);
    }

    if (lbl_806DEFE1 && lbl_806E1A50)
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

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, lbl_8057AD48);
    fn_80183B40(packet->matrix);

    if (*(unsigned long*)packet->unknown28 == 0)
    {
        fn_803640E0(
            fn_80364020(), "CharacterSkinCustom - RLXLoadSkinMatrices");
        fn_8036D7EC(parameters->matrices, parameters->matricesSize / 48, &modelview, 1);
        fn_803640E0(fn_80364020(),
            "CharacterSkinCustom - After RLXLoadSkinMatrices");
    }
    else
    {
        fn_8036D774(&modelview);
    }

    fn_801837DC(1, parameters->value32);

    if (lbl_806DEFE1 && value28 != 1.0f)
    {
        gxSaveZMode();
        if (lbl_806DEFEC)
        {
            bool colourUpdate = gxSetColourUpdate(false);
            GXCallDisplayList(
                packet->displayList->list, packet->displayList->size);
            gxSetColourUpdate(colourUpdate);
            gxSetZMode(true, GX_EQUAL, true);
        }

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
