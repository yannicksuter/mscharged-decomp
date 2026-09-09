#ifndef GAME_RENDER_STADIUM_TWEAKS_H
#define GAME_RENDER_STADIUM_TWEAKS_H

#include "Game/TweakValue.h"

class StadiumTweaks
{
public:
    StadiumTweaks(const char* category, const char* szBaseName);

    /* 0x00 */ TweakFloatBinding fGoalpostRadius;
    /* 0x10 */ TweakFloatBinding fGoalpostOffset;
    /* 0x20 */ TweakFloatBinding fNetWidth;
    /* 0x30 */ TweakFloatBinding fNetHeight;
    /* 0x40 */ TweakFloatBinding fPhysNetWidth;
    /* 0x50 */ TweakFloatBinding fPhysNetHeight;
    /* 0x60 */ TweakFloatBinding fPhysNetDepth;
    /* 0x70 */ TweakFloatBinding fSoftness;
    /* 0x80 */ TweakBoolBinding bDontUseLowest;
    /* 0x90 */ TweakFloatBinding fShadowHeight;
    /* 0xA0 */ TweakFloatBinding fShadowOpacity;
}; // total size: 0xB0

#endif // GAME_RENDER_STADIUM_TWEAKS_H
