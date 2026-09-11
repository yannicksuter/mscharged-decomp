#include <revolution/gx.h>

#include "NL/gl/glModel.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMemory.h"
#include "Game/TweakRegistry.h"
#include "NL/glx/GXMaterialShadowTweaks.h"

void gxSetTevColourOp(int, int, int, int, bool, int);
void gxSetTevAlphaOp(int, int, int, int, bool, int);
void gxSetTevColourIn(int, int, int, int, int);
void gxSetTevAlphaIn(int, int, int, int, int);
void gxSetTexCoordGen(int, int, int, unsigned int);
void gxSetZMode(bool, int, bool);
void gxSetAlphaCompare(int, u8);
unsigned int gxSetNumChans(unsigned int);
unsigned int gxSetNumTevStages(unsigned int);
unsigned int gxSetNumTexGens(unsigned int);
void gxSetTevOrder(int, int, int, int);

struct Parameters_802A39D4
{
    u32 value;
    u32 unknown04;
    u32 mode;
};

extern s32 lbl_806E1CE0;
extern char lbl_806DF190[];
extern char lbl_806DF194[];
extern char lbl_806DF19C[];
extern char lbl_806DF1A4[];
extern char lbl_8052A878[];
extern u8 lbl_806E5FC0;
extern u8 lbl_806E5FC1;
extern u8 lbl_806E5FC2;
extern u8 lbl_806E5FC3;

extern "C" void fn_802C764C(TweakValueInt*);
extern "C" void fn_802A7468(void*, bool);
extern "C" void fn_802A7530(void*, const glModelPacket*);
extern "C" void fn_802A7588(void*, const glModelPacket*);
extern "C" void fn_802A7774(void*, const glModelPacket*);
extern "C" void fn_802A77E8(void*, const glModelPacket*);

extern "C" void SetShadowVolumeMode(s32 mode)
{
    if (lbl_806E1CE0 == mode)
    {
        return;
    }

    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaIn(0, 7, 7, 7, 7);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevColourIn(0, 15, 15, 15, 15);

    switch (mode)
    {
    case 2:
        gxSetTexCoordGen(0, 1, 4, 60);
        gxSetZMode(false, 4, false);
        gxSetAlphaCompare(7, 0);
        gxSetTevAlphaOp(0, 14, 0, 0, true, 0);
        gxSetTevAlphaIn(0, 4, 7, 1, 7);
        gxSetTevColourIn(0, 15, 15, 15, 2);
        break;
    case 3:
        gxSetTevColourIn(0, 15, 15, 15, 4);
        gxSetTevAlphaIn(0, 7, 7, 7, 2);
        break;
    }

    lbl_806E1CE0 = mode;
}

extern "C" void fn_802A38A0(const GXColor* colour)
{
    sShadowVolumeRed.value = colour->r;
    sShadowVolumeGreen.value = colour->g;
    sShadowVolumeBlue.value = colour->b;
    sShadowVolumeAlpha.value = colour->a;
}

extern "C" void fn_802A38E4(void* renderer)
{
    GXColor colour;
    colour.r = static_cast<u8>(sShadowVolumeRed.value);
    colour.g = static_cast<u8>(sShadowVolumeGreen.value);
    colour.b = static_cast<u8>(sShadowVolumeBlue.value);
    colour.a = static_cast<u8>(sShadowVolumeAlpha.value);
    GXSetTevColor(GX_TEVREG0, colour);

    GXColor secondColour;
    secondColour.r = lbl_806E5FC0;
    secondColour.g = lbl_806E5FC1;
    secondColour.b = lbl_806E5FC2;
    secondColour.a = lbl_806E5FC3;
    GXSetTevColor(GX_TEVREG1, secondColour);

    fn_802A7468(renderer, true);
    gxSetNumChans(0);
    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 0, 0, 255);
    SetShadowVolumeMode(3);
}

extern "C" void fn_802A39CC()
{
    SetShadowVolumeMode(1);
}

extern "C" void fn_802A39D4(
    void* renderer, const glModelPacket* packet)
{
    Parameters_802A39D4* parameters = static_cast<Parameters_802A39D4*>(packet->materialParameters);
    glSetMaterialTextureAlphaState(renderer, packet, parameters->value);
}

extern "C" void fn_802A39E0(
    void* renderer, const glModelPacket* packet)
{
    Parameters_802A39D4* parameters = static_cast<Parameters_802A39D4*>(packet->materialParameters);
    if (parameters->mode == 0)
    {
        SetShadowVolumeMode(2);
    }
    else
    {
        SetShadowVolumeMode(3);
    }

    fn_802A7530(renderer, packet);
    fn_802A77E8(renderer, packet);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        fn_802A7588(renderer, packet);
    }
    else
    {
        fn_802A7774(renderer, packet);
    }
}

extern "C" void fn_802A3A94()
{
    TweakValueInt* states[4] = {
        &sShadowVolumeRed, &sShadowVolumeGreen, &sShadowVolumeBlue, &sShadowVolumeAlpha
    };
    const char* names[4] = {
        lbl_806DF190, lbl_806DF194, lbl_806DF19C, lbl_806DF1A4
    };

    for (u32 i = 0; i < 4; ++i)
    {
        TweakValueInt* state = states[i];
        fn_802C764C(state);
        state->mName = names[i];
        state->value = 0;
        state->mUnidentified009 = 0;

        if (!IsTweakRegistryInitialized())
        {
            TweakPendingValue* entry = (TweakPendingValue*)nlMalloc(0x18, 8, true);
            if (entry != 0)
            {
                QueueTweakValue(entry, state, lbl_8052A878);
            }
        }
        else
        {
            TweakEntry* entry = FindOrCreateTweakPath(
                GetTweakRoot(), lbl_8052A878, 0);
            if (entry != 0)
            {
                AddTweakValue(entry, state);
            }
        }
        gLastTweakCategory = lbl_8052A878;
    }
}
