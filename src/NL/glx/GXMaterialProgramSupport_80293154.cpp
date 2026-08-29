#include <revolution/gx.h>

#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"

extern "C"
{
    int fn_801820FC();
    void* fn_80182240(int, int);
    void* fn_8018230C(int, int);
    void fn_80182ED0(void*, GLView*, int);
    void fn_801832F4(int, void*, int);
    void fn_801833D0(int, void*, const nlMatrix4*, float);
    void fn_80183654(int, void*);
    void fn_801836FC(int);
    void fn_801837DC(int, int);
    void fn_80183A98();
    void fn_80183B40(unsigned long matrix);
}

bool lbl_806DF080 = true;
bool lbl_806DF081 = true;

nlMatrix4 lbl_8057B1D8;
bool lbl_806E1B50;
void* lbl_806E1B54;
float lbl_806E1B58;
int lbl_806E1B5C;

extern "C" void fn_80293154(int mode)
{
    lbl_806E1B5C = mode;
    fn_80183654(1, lbl_806E1B54);

    if (mode == 1)
    {
        fn_801836FC(1);
        fn_801832F4(1, lbl_806E1B54, 1);
    }

    gxSetNumChans(2);
    gxSetNumTexGens(4);
    gxSetNumTevStages(8);

    gxSetTevOrder(0, 2, 2, 255);
    gxSetTevOrder(1, 1, 1, 255);
    gxSetTevOrder(2, 0, 0, 255);
    gxSetTevOrder(3, 255, 255, 4);
    gxSetTevOrder(4, 3, 3, 255);
    gxSetTevOrder(5, 255, 255, 5);
    gxSetTevOrder(6, 255, 255, 255);
    gxSetTevOrder(7, 255, 255, 255);

    gxSetTevColourOp(0, 0, 0, 0, true, 1);
    gxSetTevColourOp(3, 0, 0, 0, true, 2);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKColorSel(GX_TEVSTAGE4, GX_TEV_KCSEL_K1);
    GXSetTevKColorSel(GX_TEVSTAGE6, GX_TEV_KCSEL_K2);

    gxSetTevColourIn(0, 15, 14, 8, 15);
    gxSetTevColourIn(1, 15, 8, 2, 15);
    gxSetTevColourIn(2, 8, 15, 2, 0);
    gxSetTevColourIn(3, 15, 0, 10, 15);
    gxSetTevColourIn(4, 15, 14, 8, 15);
    gxSetTevColourIn(5, 15, 0, 10, 15);
    gxSetTevColourIn(6, 15, 0, 14, 15);
    gxSetTevColourIn(7, 15, 12, 4, 0);

    gxSetTevAlphaIn(0, 7, 7, 7, 7);
    gxSetTevAlphaIn(1, 7, 7, 7, 7);
    gxSetTevAlphaIn(2, 7, 7, 7, 4);
    gxSetTevAlphaIn(3, 7, 7, 7, 0);
    gxSetTevAlphaIn(4, 7, 7, 7, 0);
    gxSetTevAlphaIn(5, 7, 7, 7, 0);
    gxSetTevAlphaIn(6, 7, 7, 7, 0);
    gxSetTevAlphaIn(7, 7, 7, 7, 0);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029D0E8>::Activate(GLView* view)
{
    lbl_806E1B5C = 2;
    static_cast<GXMaterialProgram_8029D0E8*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057B1D8);
    lbl_806E1B54 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1B54, view, 0);
    lbl_806E1B58 = 0.0f;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029D0E8>::Deactivate()
{
    if (lbl_806E1B50)
    {
        fn_80183A98();
        lbl_806E1B50 = false;
    }

    fn_801836FC(0);
    fn_801832F4(0, lbl_806E1B54, 1);
    fn_80183654(0, lbl_806E1B54);
    gxSetNumChans(1);
    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevColourOp(3, 0, 0, 0, true, 0);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029D0E8>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_80293154
{
    float c[4];
};

static inline GXColor ConvertColour_80293154(
    const FloatColour_80293154& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029D0E8>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DF080)
        return;

    static_cast<GXMaterialProgram_8029D0E8*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_8029D0E8*>(this)->BindParameters(packet);

    int mode;
    if (*(int*)((unsigned char*)packet->unknown20 + 60) != 0
        && fn_801820FC() != 0)
        mode = 1;
    else
        mode = 0;

    if (lbl_806E1B5C != mode)
    {
        Deactivate();
        fn_80293154(mode);
    }

    unsigned char* parameters = (unsigned char*)packet->unknown20;
    float alpha = 1.0f - *(float*)(parameters + 32);
    float intensity = *(float*)(parameters + 36);
    float attenuation = *(float*)(parameters + 40);

    FloatColour_80293154 colour0 = { { alpha, alpha, alpha, alpha } };
    GXColor gxColour0 = ConvertColour_80293154(colour0);
    GXSetTevKColor(GX_KCOLOR0, gxColour0);

    FloatColour_80293154 colour1 = {
        { intensity, intensity, intensity, intensity }
    };
    GXColor gxColour1 = ConvertColour_80293154(colour1);
    GXSetTevKColor(GX_KCOLOR1, gxColour1);

    FloatColour_80293154& colour2 = *(FloatColour_80293154*)(parameters + 44);
    GXColor gxColour2 = ConvertColour_80293154(colour2);
    GXSetTevKColor(GX_KCOLOR2, gxColour2);

    if (lbl_806E1B58 != attenuation && intensity != 0.0f)
    {
        lbl_806E1B58 = attenuation;
        for (int i = 0; i < (int)lbl_806E1B54; ++i)
        {
            void* light = fn_8018230C(i, 0);
            fn_801833D0(i, light, &lbl_8057B1D8, attenuation);
        }
    }

    fn_80183B40(packet->matrix);

    bool enableState = false;
    if (*(int*)(parameters + 64) == 1 && lbl_806DF081)
        enableState = true;

    if (enableState)
    {
        if (!lbl_806E1B50)
        {
            fn_801837DC(0, 0);
            lbl_806E1B50 = true;
        }
    }
    else if (lbl_806E1B50)
    {
        fn_80183A98();
        lbl_806E1B50 = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_8029D0E8*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_8029D0E8*>(this)->DrawDirect(packet);
}
