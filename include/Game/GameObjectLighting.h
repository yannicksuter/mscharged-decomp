#ifndef _GAMEOBJECTLIGHTING_H_
#define _GAMEOBJECTLIGHTING_H_

#include "NL/nlColour.h"
#include "types.h"

class GLView;

class nlVector2;
u32 GetGameObjectLightRamp();
nlColour fn_80183C9C(const nlVector2* arg0, bool arg1);
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
