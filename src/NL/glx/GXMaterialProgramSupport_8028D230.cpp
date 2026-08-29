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
    void fn_80182ED0(void*, GLView*, int);
    void fn_801832F4(int, void*, int);
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

struct GXMaterialProgramParameters_80298EE0
{
    /* 0x00 */ UnidentifiedTextureState texture0;
    /* 0x08 */ UnidentifiedTextureState texture1;
    /* 0x10 */ UnidentifiedTextureState texture2;
    /* 0x18 */ const void* matrices;
    /* 0x1C */ unsigned long matricesSize;
    /* 0x20 */ float value32;
    /* 0x24 */ float value36;
    /* 0x28 */ float value40;
    /* 0x2C */ unsigned long value44;
    /* 0x30 */ int value48;
}; // size: 0x34

static nlMatrix4 lbl_8057AEC0;
static void* lbl_806E1AD0;
static bool lbl_806E1AD4;

extern "C" void fn_8028D230(bool enabled)
{
    unsigned int numChans;
    unsigned int numTexGens;
    unsigned int numTevStages;

    if (enabled)
    {
        fn_801832F4(1, lbl_806E1AD0, 0);
        fn_801836FC(1);
        numChans = 1;
        numTexGens = 3;
        numTevStages = 4;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 4);
        gxSetTevOrder(3, 2, 2, 255);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);

        if (fn_80182118() != 0)
            gxSetTevColourOp(2, 0, 0, 1, true, 0);

        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 10, 15);
        gxSetTevColourIn(3, 0, 8, 14, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
    }
    else
    {
        numChans = 0;
        numTexGens = 3;
        numTevStages = 4;

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 255, 255, 255);
        gxSetTevOrder(3, 2, 2, 255);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);

        gxSetTevColourIn(0, 15, 12, 8, 14);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 12, 15);
        gxSetTevColourIn(3, 0, 8, 14, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
    }

    gxSetNumChans(numChans);
    gxSetNumTexGens(numTexGens);
    gxSetNumTevStages(numTevStages);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80298EE0>::Activate(GLView* view)
{
    static_cast<GXMaterialProgram_80298EE0*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057AEC0);
    fn_80183BF4(&lbl_8057AEC0);
    fn_80183B40(-1);
    lbl_806E1AD0 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1AD0, view, 0);
    lbl_806E1AD4 = true;
    fn_8028D230(true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80298EE0>::Deactivate()
{
    gxSetCurrentMtx(0, true);
    fn_801836FC(0);
    fn_801832F4(0, lbl_806E1AD0, 1);
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80298EE0>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_8028D230
{
    float c[4];
};

static inline GXColor ConvertColour_8028D230(
    const FloatColour_8028D230& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80298EE0>::Draw(
    const glModelPacket* packet)
{
    GXMaterialProgramParameters_80298EE0* parameters = (GXMaterialProgramParameters_80298EE0*)packet->unknown20;
    float value32 = parameters->value32;
    float value36 = parameters->value36;
    float value40 = parameters->value40;

    if (value36 == 0.0f)
        return;

    FloatColour_8028D230 source32 = { { value32, value32, value32, value32 } };
    GXColor colour32 = ConvertColour_8028D230(source32);
    GXSetTevKColor(GX_KCOLOR0, colour32);

    FloatColour_8028D230 source36 = { { value36, value36, value36, value36 } };
    GXColor colour36 = ConvertColour_8028D230(source36);
    GXSetTevKColor(GX_KCOLOR1, colour36);

    FloatColour_8028D230 source40 = { { value40, value40, value40, value40 } };
    GXColor colour40 = ConvertColour_8028D230(source40);
    GXSetTevKColor(GX_KCOLOR2, colour40);

    static_cast<GXMaterialProgram_80298EE0*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_80298EE0*>(this)->BindParameters(packet);

    bool enabled = parameters->value48 == 1;
    if (enabled != lbl_806E1AD4)
    {
        lbl_806E1AD4 = enabled;
        Deactivate();
        fn_8028D230(enabled);
    }

    gxSetNumTevStages(value40 == 0.0f ? 3 : 4);

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, lbl_8057AEC0);
    fn_80183B40(packet->matrix);

    if (packet->unknown28 == 0)
    {
        fn_8036D7EC(parameters->matrices, parameters->matricesSize / 48, &modelview, 0);
    }
    else
    {
        fn_8036D774(&modelview);
    }

    fn_801837DC(1, parameters->value44);

    if (enabled && value36 != 1.0f)
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
