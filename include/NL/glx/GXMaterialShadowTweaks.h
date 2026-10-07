#ifndef NL_GLX_GX_MATERIAL_SHADOW_TWEAKS_H
#define NL_GLX_GX_MATERIAL_SHADOW_TWEAKS_H

#include <revolution/gx/GXTypes.h>

#include "Game/TweakValueInt.h"

extern TweakValueInt sShadowVolumeRed;
extern TweakValueInt sShadowVolumeGreen;
extern TweakValueInt sShadowVolumeBlue;
extern TweakValueInt sShadowVolumeAlpha;

void CopyShadowVolumeColour(const GXColor* colour);

#endif // NL_GLX_GX_MATERIAL_SHADOW_TWEAKS_H
