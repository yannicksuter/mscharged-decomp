#include <revolution/gx.h>

#include <string.h>

#include "NL/gl/glState.h"
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

bool lbl_806DF090 = true;
bool lbl_806DF091 = true;
bool lbl_806DF092 = true;

const Mtx lbl_804E87F0 = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
};

nlMatrix4 lbl_8057B258;
bool lbl_806E1B70;
void* lbl_806E1B74;
int lbl_806E1B78;
float lbl_806E1B80;
float lbl_806E1B84;

extern "C" void fn_80294404(int mode)
{
    lbl_806E1B78 = mode;
    gxSetNumChans(1);

    if (mode != 0)
        fn_801832F4(1, lbl_806E1B74, 1);

    gxSetTexCoordGen(0, 0, 4, 30);

    if (mode == 3)
    {
        gxSetNumTexGens(2);
        gxSetNumTevStages(2);
        int useSubtract = fn_80182118() != 0;

        gxSetTevOrder(0, 1, 1, 4);
        gxSetTevOrder(1, 0, 0, 255);
        gxSetTexCoordGen(1, 10, 19, 60);
        gxSetTevColourOp(1, 0, 0, useSubtract, true, 0);
        gxSetTevColourIn(0, 15, 12, 8, 15);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 4);
    }
    else
    {
        if (mode != 0)
        {
            fn_801836FC(1);
            if (mode == 2)
                gxSetTevColourOp(0, 0, 0, 1, true, 0);
        }

        gxSetNumTexGens(1);
        gxSetNumTevStages(1);
        gxSetTevOrder(0, 0, 0, 4);
        gxSetTevColourIn(0, 15, 10, 8, 15);
        gxSetTevAlphaIn(0, 7, 5, 4, 7);
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029DE3C>::Activate(GLView* view)
{
    lbl_806E1B78 = 4;
    lbl_806E1B80 = 10000000000.0f;
    lbl_806E1B84 = 10000000000.0f;
    static_cast<GXMaterialProgram_8029DE3C*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057B258);
    lbl_806E1B74 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1B74, view, 0);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029DE3C>::Deactivate()
{
    if (lbl_806E1B70)
    {
        fn_80183A98();
        lbl_806E1B70 = false;
    }

    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTexCoordGen(0, 1, 4, 60);

    if (lbl_806E1B78 == 3)
    {
        gxSetTevColourOp(1, 0, 0, 0, true, 0);
        gxSetTexCoordGen(1, 1, 5, 60);
    }
    else if (lbl_806E1B78 != 0)
    {
        fn_801836FC(0);
        if (lbl_806E1B78 == 2)
            gxSetTevColourOp(0, 0, 0, 0, true, 0);
    }

    fn_801832F4(0, lbl_806E1B74, 1);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029DE3C>::Prepare(
    const glModelPacket* packet)
{
    unsigned char* parameters = (unsigned char*)packet->unknown20;
    fn_802CC978(this, packet, *(unsigned long*)parameters);
    unsigned int& rasterState = *(unsigned int*)&packet->rasterState;

    if (*(int*)(parameters + 28) != 0)
        glSetRasterState(rasterState, GLS_Culling, 0);
    if (*(int*)(parameters + 32) != 0)
        glSetRasterState(rasterState, GLS_DepthWrite, 1);
}

static inline float WrapTextureOffset_80294404(float value)
{
    int scaled = (int)(value * 1024.0f);
    return (float)(scaled % 1024) * (1.0f / 1024.0f);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029DE3C>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DF090)
        return;

    GXMaterialProgram_8029DE3C* program = static_cast<GXMaterialProgram_8029DE3C*>(this);
    program->BindVertexArrays(packet);
    program->BindParameters(packet);

    unsigned char* parameters = (unsigned char*)packet->unknown20;
    if (*(int*)(parameters + 20) == 1)
    {
        UnidentifiedTextureState* texture = (UnidentifiedTextureState*)parameters;
        texture->SetWrapS(true);
        texture->SetWrapT(true);
    }

    int mode;
    if (*(int*)(parameters + 24) == 0 || fn_801820FC() == 0)
    {
        mode = 0;
    }
    else if (fn_80182104(0) != 0)
    {
        mode = 3;
    }
    else
    {
        mode = 1;
        if (fn_80182118() != 0)
            mode = 2;
    }

    if (lbl_806E1B78 != mode)
    {
        Deactivate();
        fn_80294404(mode);
    }

    fn_80183B40(packet->matrix);

    bool enableState = false;
    if (*(int*)(parameters + 16) == 1 && lbl_806DF092)
        enableState = true;

    if (enableState)
    {
        if (!lbl_806E1B70)
        {
            fn_801837DC(0, 0);
            lbl_806E1B70 = true;
        }
    }
    else if (lbl_806E1B70)
    {
        fn_80183A98();
        lbl_806E1B70 = false;
    }

    if (lbl_806E1B78 == 3)
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
        fn_8036BE88(1, &texture);
    }

    float speedX = lbl_806DF091 ? *(float*)(parameters + 8) : 0.0f;
    float speedY = lbl_806DF091 ? *(float*)(parameters + 12) : 0.0f;
    float time = fn_80277DB0();
    float offsetX = WrapTextureOffset_80294404(time * speedX);
    float offsetY = WrapTextureOffset_80294404(time * speedY);

    Mtx textureMatrix;
    memcpy(textureMatrix, lbl_804E87F0, sizeof(Mtx));
    textureMatrix[0][3] = offsetX;
    textureMatrix[1][3] = offsetY;
    GXLoadTexMtxImm(textureMatrix, 30, GX_MTX_3x4);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        program->DrawIndexed(packet);
    else
        program->DrawDirect(packet);
}
