#ifndef UNCLASSIFIED_TU_801A2004_H
#define UNCLASSIFIED_TU_801A2004_H

#include "Game/TweakValue.h"
#include "NL/gl/glTarget.h"
#include "types.h"

class GLView;

struct HighRangeTweakValues_801A2004
{
    /* 0x00 */ TweakValueIntImpl_804FD898 miHighRangeIndex;
    /* 0x10 */ TweakValueImpl_804F4DC8 mfHighRangeMult;
    /* 0x20 */ TweakValueImpl_804F4DC8 mfHighRangeOffset;
    /* 0x30 */ TweakValueIntImpl_804FD898 miHighRangeGray;
    /* 0x40 */ TweakValueBoolImpl_804F4538 mbFineHighRange;
    /* 0x50 */ TweakValueIntImpl_804FD898 miRed;
    /* 0x60 */ TweakValueIntImpl_804FD898 miGreen;
    /* 0x70 */ TweakValueIntImpl_804FD898 miBlue;
    /* 0x80 */ TweakValueIntImpl_804FD898 miAlpha;
};

struct HighRangeState_801A2394
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

extern "C"
{
    extern HighRangeState_801A2394 lbl_80572020;

    void fn_801A2004(HighRangeTweakValues_801A2004*, const char*);
    bool fn_801A238C(const HighRangeState_801A2394*);
    void fn_801A2394(HighRangeState_801A2394*);
    void fn_801A2860(HighRangeState_801A2394*, int);
    void fn_801A28F0(HighRangeState_801A2394*);
    void fn_801A2A78(HighRangeState_801A2394*);
    void fn_801A2B80(HighRangeState_801A2394*, int, int);
}

#endif // UNCLASSIFIED_TU_801A2004_H
