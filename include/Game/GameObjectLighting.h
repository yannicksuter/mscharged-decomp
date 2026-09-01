#ifndef _GAMEOBJECTLIGHTING_H_
#define _GAMEOBJECTLIGHTING_H_

#include "NL/nlColour.h"
#include "types.h"

class nlVector2;
u32 GetGameObjectLightRamp();
nlColour fn_80183C9C(const nlVector2* arg0, bool arg1);
void UpdateGameObjectLighting();
void InitializeGameObjectLighting();
bool AlwaysUseCameraRelativeCharacterLighting();

#endif // _GAMEOBJECTLIGHTING_H_
