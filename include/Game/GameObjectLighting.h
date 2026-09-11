#ifndef _GAMEOBJECTLIGHTING_H_
#define _GAMEOBJECTLIGHTING_H_

#include "NL/nlColour.h"
#include "types.h"

class GLView;
class LightingLookup;
class TweakValueFloat;

extern TweakValueFloat gShadowLookupScaleX;
extern TweakValueFloat gShadowLookupScaleY;
extern TweakValueFloat gShadowLookupTransX;
extern TweakValueFloat gShadowLookupTransY;
extern LightingLookup* gpShadowLightingLookup;

class nlVector2;
class nlVector3;
struct glModel;
struct UnidentifiedLightingObject;
u32 GetGameObjectLightRamp();
nlColour fn_80183C9C(const nlVector2* arg0, bool arg1);
int fn_80183DEC(const nlVector3*);
void fn_80183E4C();
void fn_80183E8C(UnidentifiedLightingObject*, glModel*);
void fn_80183F78(UnidentifiedLightingObject*, glModel*);
void UpdateGameObjectLighting();
void InitializeGameObjectLighting();
bool AlwaysUseCameraRelativeCharacterLighting();

// Lighting hooks called from the material programs; the programs disagree on
// the parameter spellings, so they keep C linkage.
extern "C"
{
    void fn_80182ED0(s32, GLView*, bool);
    void fn_801832F4(s32, s32, s32);
    void fn_801837DC(s32, u32);
    void fn_80183A98();
    void fn_80183B40(u32 matrix);
}

#endif // _GAMEOBJECTLIGHTING_H_
