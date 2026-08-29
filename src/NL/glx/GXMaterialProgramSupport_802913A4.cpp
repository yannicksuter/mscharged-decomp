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

bool lbl_806DF068 = true;
bool lbl_806DF069 = true;

nlMatrix4 lbl_8057B118;
bool lbl_806E1B20;
void* lbl_806E1B24;
int lbl_806E1B28;

extern "C" void fn_802913A4(
    int mode, GXMaterialProgram_8029BC9C*)
{
    lbl_806E1B28 = mode;

    if (mode == 1)
        fn_801836FC(1);

    if (mode != 0)
        fn_801832F4(1, lbl_806E1B24, 1);

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
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029BC9C>::Activate(GLView* view)
{
    lbl_806E1B28 = 3;
    static_cast<GXMaterialProgram_8029BC9C*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057B118);
    lbl_806E1B24 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1B24, view, 0);
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
void GXMaterialProgramImpl<GXMaterialProgram_8029BC9C>::Deactivate()
{
    if (lbl_806E1B20)
    {
        fn_80183A98();
        lbl_806E1B20 = false;
    }

    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    fn_801832F4(0, lbl_806E1B24, 1);

    if (lbl_806E1B28 == 2)
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
void GXMaterialProgramImpl<GXMaterialProgram_8029BC9C>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_802913A4
{
    float c[4];
};

static inline GXColor ConvertColour_802913A4(
    const FloatColour_802913A4& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029BC9C>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DF068)
        return;

    unsigned char* parameters = (unsigned char*)packet->unknown20;
    float alpha = 1.0f - *(float*)(parameters + 24);

    int mode;
    if (*(int*)(parameters + 28) == 0 || fn_801820FC() == 0)
    {
        mode = 0;
    }
    else
    {
        mode = 1;
        if (fn_80182104(0) != 0)
            mode = 2;
    }

    if (lbl_806E1B28 != mode)
    {
        Deactivate();
        fn_802913A4(mode, static_cast<GXMaterialProgram_8029BC9C*>(this));
    }

    FloatColour_802913A4 colour = { { alpha, alpha, alpha, alpha } };
    GXColor gxColour = ConvertColour_802913A4(colour);
    GXSetTevKColor(GX_KCOLOR0, gxColour);

    static_cast<GXMaterialProgram_8029BC9C*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_8029BC9C*>(this)->BindParameters(packet);
    fn_80183B40(packet->matrix);

    bool enableState = false;
    if (*(int*)(parameters + 32) == 1 && lbl_806DF069)
        enableState = true;

    if (enableState)
    {
        if (!lbl_806E1B20)
        {
            fn_801837DC(0, 0);
            lbl_806E1B20 = true;
        }
    }
    else if (lbl_806E1B20)
    {
        fn_80183A98();
        lbl_806E1B20 = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_8029BC9C*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_8029BC9C*>(this)->DrawDirect(packet);
}
