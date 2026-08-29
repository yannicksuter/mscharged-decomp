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

bool lbl_806DF098 = true;
bool lbl_806DF099 = true;

nlMatrix4 lbl_8057B298;
bool lbl_806E1B88;
void* lbl_806E1B8C;
int lbl_806E1B90;

extern "C" void fn_80294B28(int mode, GXMaterialProgram_8029E338*)
{
    lbl_806E1B90 = mode;
    gxSetNumChans(1);

    if (mode != 0)
        fn_801832F4(1, lbl_806E1B8C, 1);

    if (mode == 3)
    {
        gxSetNumTexGens(3);
        gxSetNumTevStages(4);

        int useSubtract = fn_80182118() != 0;
        gxSetTevOrder(0, 2, 2, 4);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTevOrder(2, 1, 1, 255);
        gxSetTevOrder(3, 255, 255, 255);
        gxSetTexCoordGen(2, 10, 19, 60);
        gxSetTevColourOp(1, 0, 0, useSubtract, true, 1);
        GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K0);

        gxSetTevColourIn(0, 15, 12, 8, 15);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 12, 8, 14, 15);
        gxSetTevColourIn(3, 15, 2, 0, 15);

        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
    }
    else
    {
        if (mode != 0)
            fn_801836FC(1);

        gxSetNumTexGens(2);
        gxSetNumTevStages(3);
        gxSetTevOrder(0, 0, 0, 4);
        gxSetTevOrder(1, 1, 1, 255);
        gxSetTevOrder(2, 255, 255, 255);
        gxSetTevColourOp(0, 0, 0, mode == 2, true, 1);
        GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);

        gxSetTevColourIn(0, 15, 10, 8, 15);
        gxSetTevColourIn(1, 12, 8, 14, 15);
        gxSetTevColourIn(2, 15, 2, 0, 15);

        gxSetTevAlphaIn(0, 7, 5, 4, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 0);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029E338>::Activate(GLView* view)
{
    lbl_806E1B90 = 4;
    static_cast<GXMaterialProgram_8029E338*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057B298);
    lbl_806E1B8C = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1B8C, view, 0);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029E338>::Deactivate()
{
    if (lbl_806E1B88)
    {
        fn_80183A98();
        lbl_806E1B88 = false;
    }

    if (lbl_806E1B90 == 3)
    {
        gxSetTevColourOp(1, 0, 0, 0, true, 0);
        gxSetTexCoordGen(2, 1, 5, 60);
    }
    else
    {
        fn_801836FC(0);
        gxSetTevColourOp(0, 0, 0, 0, true, 0);
    }

    fn_801832F4(0, lbl_806E1B8C, 1);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029E338>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_80294B28
{
    float c[4];
};

static inline GXColor ConvertColour_80294B28(
    const FloatColour_80294B28& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029E338>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DF098)
        return;

    static_cast<GXMaterialProgram_8029E338*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_8029E338*>(this)->BindParameters(packet);

    int mode;
    if (*(int*)((unsigned char*)packet->unknown20 + 20) == 0)
    {
        mode = 0;
    }
    else if (fn_801820FC() == 0)
    {
        mode = 0;
    }
    else
    {
        if (fn_80182104(0) != 0)
            mode = 3;
        else
        {
            mode = 1;
            if (fn_80182118() != 0)
                mode = 2;
        }
    }

    if (lbl_806E1B90 != mode)
    {
        Deactivate();
        fn_80294B28(
            mode, static_cast<GXMaterialProgram_8029E338*>(this));
    }

    if (lbl_806E1B90 == 3)
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

    float value = *(float*)((unsigned char*)packet->unknown20 + 16);
    FloatColour_80294B28 colour = { { value, value, value, value } };
    GXColor gxColour = ConvertColour_80294B28(colour);
    GXSetTevKColor(GX_KCOLOR0, gxColour);

    fn_80183B40(packet->matrix);

    bool enableState = false;
    if (*(int*)((unsigned char*)packet->unknown20 + 24) == 1
        && lbl_806DF099)
        enableState = true;

    if (enableState)
    {
        if (!lbl_806E1B88)
        {
            fn_801837DC(0, 0);
            lbl_806E1B88 = true;
        }
    }
    else if (lbl_806E1B88)
    {
        fn_80183A98();
        lbl_806E1B88 = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_8029E338*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_8029E338*>(this)->DrawDirect(packet);
}
