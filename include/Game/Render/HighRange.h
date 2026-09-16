#ifndef GAME_RENDER_HIGH_RANGE_H
#define GAME_RENDER_HIGH_RANGE_H

#include "Game/TweakValue.h"
#include "NL/gl/glTarget.h"
#include "types.h"

class GLView;

struct HighRangeTweaks
{
    /* 0x00 */ TweakIntBinding miHighRangeIndex;
    /* 0x10 */ TweakFloatBinding mfHighRangeMult;
    /* 0x20 */ TweakFloatBinding mfHighRangeOffset;
    /* 0x30 */ TweakIntBinding miHighRangeGray;
    /* 0x40 */ TweakBoolBinding mbFineHighRange;
    /* 0x50 */ TweakIntBinding miRed;
    /* 0x60 */ TweakIntBinding miGreen;
    /* 0x70 */ TweakIntBinding miBlue;
    /* 0x80 */ TweakIntBinding miAlpha;
};

struct HighRange
{
    struct Viewport
    {
        /* 0x00 */ u32 x;
        /* 0x04 */ u32 y;
        /* 0x08 */ u32 width;
        /* 0x0C */ u32 height;
    };

    /* 0x000 */ GLView* mViews[7];
    /* 0x01C */ Viewport mViewports[7];
    /* 0x08C */ char mNames[7][0x10];
    /* 0x0FC */ u32 mTextures[7];
    /* 0x118 */ GLRenderPair mRenderPairs[7];
};

extern HighRange gHighRange;

void BindHighRangeTweaks(HighRangeTweaks*, const char*);
bool IsHighRangeEnabled(const HighRange*);
void InitializeHighRange(HighRange*);
void SetHighRangeTargetsEnabled(HighRange*, int);
void CompositeHighRange(HighRange*);
void RenderHighRangeChain(HighRange*);
void RenderHighRangePass(HighRange*, int, int);
HighRangeTweaks* GetHighRangeTweaks();

#endif // GAME_RENDER_HIGH_RANGE_H
