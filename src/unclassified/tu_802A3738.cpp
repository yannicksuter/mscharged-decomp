#include <revolution/gx.h>

#include "NL/gl/glModel.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMemory.h"

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

struct TweakState_802A38A0
{
    void* vtable;
    const char* name;
    u8 unknown08;
    u8 unknown09;
    u8 pad_0A[2];
    u32 value;
};

struct Parameters_802A39D4
{
    u32 value;
    u32 unknown04;
    u32 mode;
};

extern s32 lbl_806E1CE0;
extern TweakState_802A38A0 lbl_8057B4C0;
extern TweakState_802A38A0 lbl_8057B4E0;
extern TweakState_802A38A0 lbl_8057B500;
extern TweakState_802A38A0 lbl_8057B520;
extern char lbl_806DF190[];
extern char lbl_806DF194[];
extern char lbl_806DF19C[];
extern char lbl_806DF1A4[];
extern char lbl_8052A878[];
extern const char* lbl_806E1E90;
extern u8 lbl_806E5FC0;
extern u8 lbl_806E5FC1;
extern u8 lbl_806E5FC2;
extern u8 lbl_806E5FC3;

extern "C" void fn_802A7468(void*, bool);
extern "C" void fn_802A7530(void*, const glModelPacket*);
extern "C" void fn_802A7588(void*, const glModelPacket*);
extern "C" void fn_802A7774(void*, const glModelPacket*);
extern "C" void fn_802A77E8(void*, const glModelPacket*);
extern "C" void fn_802CC978(void*, const glModelPacket*, u32);
extern "C" void fn_802C764C(TweakState_802A38A0*);
extern "C" bool fn_802C0F04();
extern "C" void* fn_802C0E30();
extern "C" void fn_802C2DF4(void*, TweakState_802A38A0*, const char*);
extern "C" void* fn_802C4504(void*, const char*, int);
extern "C" void fn_802C5780(void*, TweakState_802A38A0*);

extern "C" void fn_802A3738(s32 mode)
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
    lbl_8057B4C0.value = colour->r;
    lbl_8057B4E0.value = colour->g;
    lbl_8057B500.value = colour->b;
    lbl_8057B520.value = colour->a;
}

extern "C" void fn_802A38E4(void* renderer)
{
    GXColor colour;
    colour.r = static_cast<u8>(lbl_8057B4C0.value);
    colour.g = static_cast<u8>(lbl_8057B4E0.value);
    colour.b = static_cast<u8>(lbl_8057B500.value);
    colour.a = static_cast<u8>(lbl_8057B520.value);
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
    fn_802A3738(3);
}

extern "C" void fn_802A39CC()
{
    fn_802A3738(1);
}

extern "C" void fn_802A39D4(
    void* renderer, const glModelPacket* packet)
{
    Parameters_802A39D4* parameters = static_cast<Parameters_802A39D4*>(packet->unknown20);
    fn_802CC978(renderer, packet, parameters->value);
}

extern "C" void fn_802A39E0(
    void* renderer, const glModelPacket* packet)
{
    Parameters_802A39D4* parameters = static_cast<Parameters_802A39D4*>(packet->unknown20);
    if (parameters->mode == 0)
    {
        fn_802A3738(2);
    }
    else
    {
        fn_802A3738(3);
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
    TweakState_802A38A0* states[4] = {
        &lbl_8057B4C0, &lbl_8057B4E0, &lbl_8057B500, &lbl_8057B520
    };
    const char* names[4] = {
        lbl_806DF190, lbl_806DF194, lbl_806DF19C, lbl_806DF1A4
    };

    for (u32 i = 0; i < 4; ++i)
    {
        TweakState_802A38A0* state = states[i];
        fn_802C764C(state);
        state->name = names[i];
        state->value = 0;
        state->unknown09 = 0;

        if (!fn_802C0F04())
        {
            void* entry = nlMalloc(0x18, 8, true);
            if (entry != 0)
            {
                fn_802C2DF4(entry, state, lbl_8052A878);
            }
        }
        else
        {
            void* entry = fn_802C4504(
                fn_802C0E30(), lbl_8052A878, 0);
            if (entry != 0)
            {
                fn_802C5780(entry, state);
            }
        }
        lbl_806E1E90 = lbl_8052A878;
    }
}
