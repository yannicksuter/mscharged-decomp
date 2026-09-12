#ifndef _GAMEOBJECTLIGHTING_H_
#define _GAMEOBJECTLIGHTING_H_

#include "NL/nlColour.h"
#include "types.h"

class GLView;
struct GameObjectLight;
class nlMatrix4;
class ImpostorModel;
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
void fn_80183F78(ImpostorModel*, glModel*);
void UpdateGameObjectLighting();
void InitializeGameObjectLighting();
bool AlwaysUseCameraRelativeCharacterLighting();

// Shared lighting hooks used by the material programs.
extern "C"
{
    int IsGameObjectLightingEnabled();
    int ShouldUseGameObjectLightTexture(int character);
    int ShouldDoubleGameObjectLighting();
    int GetGameObjectLightCount(int character, int includeEffects);
    GameObjectLight* GetGameObjectLight(s32 index, bool character);
    void LoadGameObjectSpecularLight(s32 index, GameObjectLight* light, f32 exponent, const nlMatrix4& viewMatrix);
    void SetGameObjectSpecularLightingEnabled(s32 enabled, s32 count);
    unsigned long GetGameObjectLightTexture();
    void LoadGameObjectLights(s32 count, GLView* view, bool character);
    void SetGameObjectLightingEnabled(s32 enabled, s32 count, s32 useVertexColour);
    void SetGameObjectAmbientLightingEnabled(s32 enabled);
    void ApplyGameObjectShadowLighting(s32 skinned, u32 shadowLevel);
    void RestoreGameObjectShadowLighting();
    void SetGameObjectShadowModelMatrix(u32 matrix);
    void SetGameObjectShadowViewMatrix(const nlMatrix4* matrix);
}

#endif // _GAMEOBJECTLIGHTING_H_
