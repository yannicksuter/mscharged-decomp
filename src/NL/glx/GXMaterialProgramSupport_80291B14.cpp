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
    unsigned long fn_80182EB8();
    void fn_80182ED0(void*, GLView*, int);
    void fn_801832F4(int, void*, int);
    void fn_801836FC(int);
    void fn_801837DC(int, int);
    void fn_80183A98();
    void fn_80183B40(unsigned long matrix);
}

bool lbl_806DF070 = true;
bool lbl_806DF071 = true;

nlMatrix4 lbl_8057B158;
bool lbl_806E1B30;
void* lbl_806E1B34;
int lbl_806E1B38;

extern "C" void fn_80291B14(
    int mode, GXMaterialProgram_8029C2F8*)
{
    lbl_806E1B38 = mode;

    if (mode == 1)
        fn_801836FC(1);

    if (mode != 0)
        fn_801832F4(1, lbl_806E1B34, 1);

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
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029C2F8>::Activate(GLView* view)
{
    lbl_806E1B38 = 3;
    static_cast<GXMaterialProgram_8029C2F8*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057B158);
    lbl_806E1B34 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1B34, view, 0);
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
void GXMaterialProgramImpl<GXMaterialProgram_8029C2F8>::Deactivate()
{
    if (lbl_806E1B30)
    {
        fn_80183A98();
        lbl_806E1B30 = false;
    }

    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    fn_801832F4(0, lbl_806E1B34, 1);

    if (lbl_806E1B38 == 2)
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
void GXMaterialProgramImpl<GXMaterialProgram_8029C2F8>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_80291B14
{
    float c[4];
};

static inline GXColor ConvertColour_80291B14(
    const FloatColour_80291B14& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029C2F8>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DF070)
        return;

    unsigned char* parameters = (unsigned char*)packet->unknown20;
    float alpha = 1.0f - *(float*)(parameters + 32);

    int mode;
    if (*(int*)(parameters + 36) == 0 || fn_801820FC() == 0)
    {
        mode = 0;
    }
    else
    {
        mode = 1;
        if (fn_80182104(0) != 0)
            mode = 2;
    }

    if (lbl_806E1B38 != mode)
    {
        Deactivate();
        fn_80291B14(mode, static_cast<GXMaterialProgram_8029C2F8*>(this));
    }

    FloatColour_80291B14 colour0 = { { alpha, alpha, alpha, alpha } };
    GXColor gxColour0 = ConvertColour_80291B14(colour0);
    GXSetTevKColor(GX_KCOLOR0, gxColour0);

    float value = *(float*)(parameters + 40);
    FloatColour_80291B14 colour1 = { { value, value, value, value } };
    GXColor gxColour1 = ConvertColour_80291B14(colour1);
    GXSetTevKColor(GX_KCOLOR1, gxColour1);

    static_cast<GXMaterialProgram_8029C2F8*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_8029C2F8*>(this)->BindParameters(packet);
    fn_80183B40(packet->matrix);

    bool enableState = false;
    if (*(int*)(parameters + 44) == 1 && lbl_806DF071)
        enableState = true;

    if (enableState)
    {
        if (!lbl_806E1B30)
        {
            fn_801837DC(0, 0);
            lbl_806E1B30 = true;
        }
    }
    else if (lbl_806E1B30)
    {
        fn_80183A98();
        lbl_806E1B30 = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_8029C2F8*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_8029C2F8*>(this)->DrawDirect(packet);
}
