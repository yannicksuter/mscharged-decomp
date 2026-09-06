#ifndef GAME_RENDER_TU_80278EA4_H
#define GAME_RENDER_TU_80278EA4_H

#include "Game/TweakValue.h"

class StadiumTweakValues_80278EA4
{
public:
    StadiumTweakValues_80278EA4(const char* category, const char* szBaseName);

    /* 0x00 */ TweakValueImpl_804F4DC8 fGoalpostRadius;
    /* 0x10 */ TweakValueImpl_804F4DC8 fGoalpostOffset;
    /* 0x20 */ TweakValueImpl_804F4DC8 fNetWidth;
    /* 0x30 */ TweakValueImpl_804F4DC8 fNetHeight;
    /* 0x40 */ TweakValueImpl_804F4DC8 fPhysNetWidth;
    /* 0x50 */ TweakValueImpl_804F4DC8 fPhysNetHeight;
    /* 0x60 */ TweakValueImpl_804F4DC8 fPhysNetDepth;
    /* 0x70 */ TweakValueImpl_804F4DC8 fSoftness;
    /* 0x80 */ TweakValueBoolImpl_804F4538 bDontUseLowest;
    /* 0x90 */ TweakValueImpl_804F4DC8 fShadowHeight;
    /* 0xA0 */ TweakValueImpl_804F4DC8 fShadowOpacity;
}; // total size: 0xB0

#endif // GAME_RENDER_TU_80278EA4_H
