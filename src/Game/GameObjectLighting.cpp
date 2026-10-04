#include <revolution/gx/GXLight.h>
#include <revolution/gx/GXTev.h>
#include <revolution/gx/GXTransform.h>
#include <revolution/mtx/mtx.h>

#include "Game/GameObjectLighting.h"
#include "Game/Render/StadiumWorldObjects.h"
#include "Game/Render/ImpostorModel.h"

#include "Game/BasicStadium.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Drawable/DrawableCharacter.h"
#include "Game/Drawable/DrawableObj.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/MathHelpers.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/gl/glView.h"
#include "Game/TweakValue.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxMatrix.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/platvmath.h"
#include "Game/Render/LightingLookup.h"
#include "Game/TweakValueFloat.h"
#include "Game/TweakValueInt.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/UnidentifiedTweakAction.h"

// GameRenderTask defines this flag as u8; this unit only matches closer when
// it reads the byte as a bool.
extern bool g_bRenderWorldEffects;

struct StadiumLightingParams
{
    /* 0x00 */ u32 lightRamp;
    /* 0x04 */ s32 rampStartR;
    /* 0x08 */ s32 rampStartG;
    /* 0x0C */ s32 rampStartB;
    /* 0x10 */ s32 rampEndR;
    /* 0x14 */ s32 rampEndG;
    /* 0x18 */ s32 rampEndB;
    /* 0x1C */ f32 keyLightIntensity;
    /* 0x20 */ f32 fillLightIntensity;
    /* 0x24 */ f32 inGameKeyIntensity;
    /* 0x28 */ f32 inGameFillIntensity;
    /* 0x2C */ f32 inGameKeyRotYDeg;
    /* 0x30 */ f32 inGameKeyRotZDeg;
    /* 0x34 */ f32 inGameFillRotYDeg;
    /* 0x38 */ f32 inGameFillRotZDeg;
    /* 0x3C */ u32 unknown3C;
}; // total size: 0x40

struct GameObjectLight
{
    GameObjectLight();

    /* 0x00 */ bool useWorldPosition;
    /* 0x01 */ u8 useColour;
    /* 0x02 */ u8 isPointLight;
    /* 0x03 */ u8 _pad03;
    /* 0x04 */ f32 intensity;
    /* 0x08 */ f32 rotYDeg;
    /* 0x0C */ f32 rotZDeg;
    /* 0x10 */ nlVector3 worldPosition;
    /* 0x1C */ nlColour colour;
    /* 0x20 */ f32 radius;
}; // total size: 0x24

struct GameObjectLightArray
{
    GameObjectLight lights[2];
}; // total size: 0x48

static inline const char* GetLastTweakCategory()
{
    return gLastTweakCategory;
}

TweakValueFloat gShadowLookupScaleX(
    "Scale X", "/Rendering/Lighting/Shadow Lookup", 0.042f, false);
TweakValueFloat gShadowLookupScaleY(
    "Scale Y", GetLastTweakCategory(), 0.073f, false);
TweakValueFloat gShadowLookupTransX(
    "Trans X", GetLastTweakCategory(), 0.0f, false);
TweakValueFloat gShadowLookupTransY(
    "Trans Y", GetLastTweakCategory(), 0.0f, false);

bool gGameObjectLightingEnabled = true;
f32 gGameObjectLightBrightness = 1.0f;
bool gEffectsLightsEnabled = true;
u32 gGameObjectAmbientRed = 0x50;
u32 gGameObjectAmbientGreen = 0x50;
u32 gGameObjectAmbientBlue = 0x50;
bool gShadowLookupEnabled = true;
bool gShadowLookupInverseViewPostMultiply = true;
bool gShadowLookupSkinnedUsesInverseView = true;
s32 lbl_806DCC5C = 0x100;
u32 gGameObjectLightTexture = (u32)-1;
s32 gNumInGameLights = 2;
u8 gCameraRelativeLightingAllowed = 1;
u32 gShadowLookupTexture = (u32)-1;
u32 gShadowModelMatrix = (u32)-1;
s32 gShadowTexGen = -1;

bool gUseGameObjectLightTexture;
bool gUseCharacterLightTexture;
bool gDoubleGameObjectLighting;
bool gShadowLookupClampWrap;

u32 gLightRampTexture = glGetTexture("global/lightramp");
u32 gBlackTexture = glGetTexture("global/black");
u32 gWhiteTexture = glGetTexture("global/white");

StadiumLightingParams gStadiumGameObjectLightingParams = {
    glGetTexture("ThePalaceStadiumPlayerLightRamp"),
    0x28, 0x30, 0x28,
    0xAF, 0xAF, 0xAF,
    0.85f, 0.3f, 0.9f, 0.25f,
    55.0f, 60.0f, 55.0f, -120.0f,
    0,
};

Mtx gShadowLookupBiasMatrix = {
    { 0.5f, 0.0f, 0.0f, 0.5f },
    { 0.0f, -0.5f, 0.0f, 0.5f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

LightingLookup* gpShadowLightingLookup;
PlatTexture* g_pGameObjectLightRamp;
s32 gGameObjectLightingMode;
bool gAlwaysUseCameraRelativeCharacterLighting;

GameObjectLight gInGameLights[8];
GameObjectLight gCameraRelativeLights[2];
GameObjectLight gSTSLight;
TweakValueInt gNumCharacterInGameLights(
    "numCharacterInGameLights", "/Rendering/Lighting/Character", 2);
GameObjectLight gCharacterLight;
TweakValueInt gCharacterLightRed(
    "siCharacterLightRed", "/Rendering/Lighting/Character", 0);
TweakValueInt gCharacterLightGreen("siCharacterLightGreen", gLastTweakCategory, 0);
TweakValueInt gCharacterLightBlue("siCharacterLightBlue", gLastTweakCategory, 0);

void UpdateCharacterLightColour();

UnidentifiedTweakAction gCharacterLightRedAction(
    "Character Light Red", gLastTweakCategory, Function0<void>(UpdateCharacterLightColour));
UnidentifiedTweakAction gCharacterLightGreenAction(
    "Character Light Green", gLastTweakCategory, Function0<void>(UpdateCharacterLightColour));
UnidentifiedTweakAction gCharacterLightBlueAction(
    "Character Light Blue", gLastTweakCategory, Function0<void>(UpdateCharacterLightColour));

static const nlVector3 sInitialDirection = { 1.0f, 0.0f, 0.0f };

void FillInGameObjectLightRamp();
void SetCameraRelativeLightData(void* pLightData);
void LoadGameObjectLight(int lightId, const GameObjectLight* pLight, const nlMatrix4& mview);

int IsGameObjectLightingEnabled()
{
    return gGameObjectLightingEnabled;
}

int ShouldUseGameObjectLightTexture(int character)
{
    return character ? gUseCharacterLightTexture : gUseGameObjectLightTexture;
}

int ShouldDoubleGameObjectLighting()
{
    return gDoubleGameObjectLighting;
}

bool AlwaysUseCameraRelativeCharacterLighting()
{
    return gAlwaysUseCameraRelativeCharacterLighting;
}

void UpdateCharacterLightColour()
{
    gCharacterLight.colour.c[0] = gCharacterLightRed.value;
    gCharacterLight.colour.c[1] = gCharacterLightGreen.value;
    gCharacterLight.colour.c[2] = gCharacterLightBlue.value;
}

void fn_80182164()
{
}

void PrepareStadiumLight(StadiumLight* pLight)
{
    // This retained path prepares a light locally but does not publish it.
    GameObjectLight light;
    light.useColour = true;
    light.useWorldPosition = true;
    const nlMatrix4& matrix = *pLight->GetWorldMatrix();
    light.colour.c[0] = nlFloatColourToByte(pLight->m_colour.c[0]);
    light.colour.c[1] = nlFloatColourToByte(pLight->m_colour.c[1]);
    light.colour.c[2] = nlFloatColourToByte(pLight->m_colour.c[2]);
    light.colour.c[3] = nlFloatColourToByte(pLight->m_colour.c[3]);
    light.worldPosition = matrix.GetTranslation();
    light.intensity = pLight->GetIntensity();
}

int GetGameObjectLightCount(bool character, bool includeEffects)
{
    bool useEffectsLights = includeEffects && gEffectsLightsEnabled;
    int numEffectsLights = useEffectsLights ? GetEmissionManager()->GetNumLights() : 0;

    switch (gGameObjectLightingMode)
    {
    case 0:
        if (character)
            return numEffectsLights + gNumCharacterInGameLights.value;
        return gNumInGameLights + numEffectsLights;
    case 1:
        return gNumInGameLights + numEffectsLights;
    case 2:
        return 1;
    default:
        gGameObjectLightingMode = 0;
        return 2;
    }
}

GameObjectLight* GetGameObjectLight(int index, bool character)
{
    s32 numLights = character ? gNumCharacterInGameLights.value : gNumInGameLights;
    if (!gCameraRelativeLightingAllowed && gGameObjectLightingMode == 1)
    {
        gGameObjectLightingMode = 0;
    }

    switch (gGameObjectLightingMode)
    {
    case 0:
        if (index >= numLights)
        {
            EffectsLight* pLight = GetEmissionManager()->GetLight(index - gNumInGameLights);
            GameObjectLight* pResult = &gInGameLights[index];
            pResult->useWorldPosition = true;
            pResult->useColour = true;
            pResult->isPointLight = true;
            pResult->intensity = 1.0f;
            pResult->worldPosition = pLight->m_v3Position;
            pResult->colour.c[0] = pLight->m_Colour.c[0];
            pResult->colour.c[1] = pLight->m_Colour.c[1];
            pResult->colour.c[2] = pLight->m_Colour.c[2];
            pResult->colour.c[3] = pLight->m_Colour.c[3];
            pResult->radius = pLight->m_fRadius;
            return pResult;
        }
        if (character)
        {
            if (index == 0)
                return &gCharacterLight;
            s32 characterLightCount = gNumCharacterInGameLights.value;
            s32 lightIndex = index;
            if (lightIndex < characterLightCount)
                return &gInGameLights[lightIndex];
            lightIndex -= characterLightCount;
            return &gInGameLights[lightIndex + gNumInGameLights];
        }
        return &gInGameLights[index];

    case 1:
        if (index >= numLights)
        {
            EffectsLight* pLight = GetEmissionManager()->GetLight(index - gNumInGameLights);
            GameObjectLight* pResult = &gInGameLights[index];
            pResult->useWorldPosition = true;
            pResult->useColour = true;
            pResult->isPointLight = true;
            pResult->intensity = 1.0f;
            pResult->worldPosition = pLight->m_v3Position;
            pResult->colour.c[0] = pLight->m_Colour.c[0];
            pResult->colour.c[1] = pLight->m_Colour.c[1];
            pResult->colour.c[2] = pLight->m_Colour.c[2];
            pResult->colour.c[3] = pLight->m_Colour.c[3];
            pResult->radius = pLight->m_fRadius;
            return pResult;
        }
        return &gCameraRelativeLights[index];

    case 2:
        return &gSTSLight;

    default:
        return gInGameLights;
    }
}

void InitializeGameObjectLighting()
{
    StadiumLightingParams* pParams = &gStadiumGameObjectLightingParams;

    gNumInGameLights = 2;
    gInGameLights[0].intensity = pParams->inGameKeyIntensity;
    gInGameLights[0].rotYDeg = pParams->inGameKeyRotYDeg;
    gInGameLights[0].rotZDeg = pParams->inGameKeyRotZDeg;
    gInGameLights[1].intensity = pParams->inGameFillIntensity;
    gInGameLights[1].rotYDeg = pParams->inGameFillRotYDeg;
    gInGameLights[1].rotZDeg = pParams->inGameFillRotZDeg;

    gCharacterLight.useColour = true;
    gCharacterLight.isPointLight = false;
    gCharacterLight.useWorldPosition = true;
    gCharacterLight.intensity = 1.0f;
    nlColourSet(gCharacterLight.colour, 0, 0, 0, 255);
    if (BasicStadium::GetCurrentStadium() != 0)
    {
        gCharacterLight.worldPosition = BasicStadium::GetCurrentStadium()->m_shadowLightPosition;
    }
    gCharacterLight.colour.c[0] = gCharacterLightRed.value;
    gCharacterLight.colour.c[1] = gCharacterLightGreen.value;
    gCharacterLight.colour.c[2] = gCharacterLightBlue.value;

    gCameraRelativeLights[0].intensity = 1.0f;
    gCameraRelativeLights[1].intensity = 1.0f;
    gSTSLight.intensity = 1.0f;
    gSTSLight.useWorldPosition = true;
    nlVec3Set(gSTSLight.worldPosition, 0.0f, 0.0f, -1.0f);

    GLResourcePool* pResource = glGetCurrentResourcePool();
    g_pGameObjectLightRamp = glx_CreatePlatTexture(pResource);
    PlatTexture* pRampTexture = g_pGameObjectLightRamp;
    glRegisterTexture(pParams->lightRamp, pRampTexture, pResource);
    g_pGameObjectLightRamp->Create(0x100, 4, GXTex_RGBA8, pResource, 1, true, false);
    FillInGameObjectLightRamp();
}

void UpdateGameObjectLighting()
{
    if (!gCameraRelativeLightingAllowed)
        return;

    if (!DrawableCharacter::sCameraRelativeLighting && !gAlwaysUseCameraRelativeCharacterLighting && gGameObjectLightingMode != 1)
        return;

    SetCameraRelativeLightData(&gCameraRelativeLights);
}

void SetCameraRelativeLightData(void* pLightData)
{
    static nlVector3 keyLightInViewSpace;
    static nlVector3 fillLightInViewSpace;
    static bool initedLightInViewSpace;
    nlVector3 initialDirection;
    nlVector3 viewVec;
    nlVector3 transformedDir;
    nlVector3 keyDirection;
    nlVector3 fillDirection;
    nlMatrix4 matY;
    nlMatrix4 matZ;
    nlMatrix4 viewRotMat;

    if (!initedLightInViewSpace)
    {
        initialDirection = sInitialDirection;
        nlMakeRotationMatrixY(matY, 0.7853982f);
        nlMakeRotationMatrixZ(matZ, -0.69813174f);
        nlMultDirVectorMatrix(keyLightInViewSpace, initialDirection, matY);
        nlMultDirVectorMatrix(keyDirection, keyLightInViewSpace, matZ);
        keyLightInViewSpace = keyDirection;

        nlMakeRotationMatrixY(matY, 0.5235988f);
        nlMakeRotationMatrixZ(matZ, 0.34906587f);
        nlMultDirVectorMatrix(fillLightInViewSpace, initialDirection, matY);
        nlMultDirVectorMatrix(fillDirection, fillLightInViewSpace, matZ);
        fillLightInViewSpace = fillDirection;

        initedLightInViewSpace = true;
    }

    cCameraManager::GetViewVector(viewVec);

    u16 u16Angle = nlVector3ToAngle(viewVec);
    f32 radAngle = (f32)u16Angle * 0.0000958738f;

    nlMakeRotationMatrixZ(viewRotMat, radAngle);

    StadiumLightingParams* params = &gStadiumGameObjectLightingParams;
    GameObjectLightArray* pLights = (GameObjectLightArray*)pLightData;

    nlMultDirVectorMatrix(transformedDir, keyLightInViewSpace, viewRotMat);

    pLights->lights[0].useWorldPosition = true;
    nlVec3Set(pLights->lights[0].worldPosition, -transformedDir.x, -transformedDir.y, -transformedDir.z);
    pLights->lights[0].intensity = params->keyLightIntensity;

    nlMultDirVectorMatrix(transformedDir, fillLightInViewSpace, viewRotMat);

    pLights->lights[1].useWorldPosition = true;
    nlVec3Set(pLights->lights[1].worldPosition, -transformedDir.x, -transformedDir.y, -transformedDir.z);
    pLights->lights[1].intensity = params->fillLightIntensity;
}

void FillInGameObjectLightRamp()
{
    StadiumLightingParams* pRampParams = &gStadiumGameObjectLightingParams;
    u8* pTextureData;
    s32 i;
    f32 deltaR;
    f32 deltaG;
    f32 deltaB;

    deltaR = (f32)(pRampParams->rampEndR - pRampParams->rampStartR);
    deltaG = (f32)(pRampParams->rampEndG - pRampParams->rampStartG);
    deltaB = (f32)(pRampParams->rampEndB - pRampParams->rampStartB);

    pTextureData = (u8*)g_pGameObjectLightRamp->m_LinearData;
    for (i = 0; i < 0x100; i += 8)
    {
        {
            f32 t = (f32)i / 256.0f;
            pTextureData[0] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[1] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[2] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[3] = 0xFF;
        }
        {
            f32 t = (f32)(i + 1) / 256.0f;
            pTextureData[4] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[5] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[6] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[7] = 0xFF;
        }
        {
            f32 t = (f32)(i + 2) / 256.0f;
            pTextureData[8] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[9] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[10] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[11] = 0xFF;
        }
        {
            f32 t = (f32)(i + 3) / 256.0f;
            pTextureData[12] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[13] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[14] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[15] = 0xFF;
        }
        {
            f32 t = (f32)(i + 4) / 256.0f;
            pTextureData[16] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[17] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[18] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[19] = 0xFF;
        }
        {
            f32 t = (f32)(i + 5) / 256.0f;
            pTextureData[20] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[21] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[22] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[23] = 0xFF;
        }
        {
            f32 t = (f32)(i + 6) / 256.0f;
            pTextureData[24] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[25] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[26] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[27] = 0xFF;
        }
        {
            f32 t = (f32)(i + 7) / 256.0f;
            pTextureData[28] = (u8)(t * deltaR + (f32)pRampParams->rampStartR);
            pTextureData[29] = (u8)(t * deltaG + (f32)pRampParams->rampStartG);
            pTextureData[30] = (u8)(t * deltaB + (f32)pRampParams->rampStartB);
            pTextureData[31] = 0xFF;
        }
        pTextureData += 0x20;
    }

    for (i = 1; i < 4; i++)
    {
        memcpy((u8*)g_pGameObjectLightRamp->m_LinearData + (i * 0x400), g_pGameObjectLightRamp->m_LinearData, 0x400);
    }

    g_pGameObjectLightRamp->Swizzle(false);
    g_pGameObjectLightRamp->Prepare();
}

u32 GetGameObjectLightRamp()
{
    return gStadiumGameObjectLightingParams.lightRamp;
}

static const GXLightID sGameObjectLightIDs[6] = {
    GX_LIGHT0, GX_LIGHT1, GX_LIGHT2, GX_LIGHT3, GX_LIGHT4, GX_LIGHT5,
};

static const u32 sGameObjectLightMasks[6] = {
    GX_LIGHT0,
    GX_LIGHT0 | GX_LIGHT1,
    GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2,
    GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2 | GX_LIGHT3,
    GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2 | GX_LIGHT3 | GX_LIGHT4,
    GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2 | GX_LIGHT3 | GX_LIGHT4 | GX_LIGHT5,
};

static const GXLightID sSpecularLightIDs[2] = { GX_LIGHT6, GX_LIGHT7 };

nlMatrix4 gShadowInverseViewMatrix;

unsigned long GetGameObjectLightTexture()
{
    return gGameObjectLightTexture;
}

void SetGameObjectLightTexture(unsigned long texture)
{
    gGameObjectLightTexture = texture;
}

void SetGameObjectLightingMode(int mode)
{
    gGameObjectLightingMode = mode;
}

void LoadGameObjectLights(int count, GLView* pView, bool character)
{
    static GLView* sLastView;
    static bool sLastCharacter;
    s32 i;
    nlMatrix4 viewMatrix;

    if (sLastView == pView && character == sLastCharacter)
        return;

    GLViewInterface* pInterface = pView->m_Interface;
    sLastView = pView;
    pInterface->GetViewMatrix(viewMatrix);
    sLastCharacter = character;

    for (i = 0; i < count; i++)
    {
        GameObjectLight* pLight = GetGameObjectLight(i, character);
        LoadGameObjectLight(i, pLight, viewMatrix);
    }
}

void LoadGameObjectLight(int lightId, const GameObjectLight* pLight, const nlMatrix4& mview)
{
    GXLightObj light;
    nlVector3 rotated;
    nlVector3 direction;
    nlVector3 initialDirection;
    nlVector3 viewPos;
    nlVector3 viewDir;
    nlVector3 worldDir;

    if (pLight->useColour)
    {
        f32 brightness = 255.0f * pLight->intensity;
        s32 level = (s32)(brightness * gGameObjectLightBrightness);
        if (level > 255)
            level = 255;

        GXColor colour = {
            (u8)((level * pLight->colour.c[0]) >> 8),
            (u8)((level * pLight->colour.c[1]) >> 8),
            (u8)((level * pLight->colour.c[2]) >> 8),
            0xFF,
        };
        GXInitLightColor(&light, colour);
    }
    else
    {
        f32 brightness = 255.0f * pLight->intensity;
        s32 level = (s32)(brightness * gGameObjectLightBrightness);
        if (level > 255)
            level = 255;

        GXColor colour = { (u8)level, (u8)level, (u8)level, 0xFF };
        GXInitLightColor(&light, colour);
    }

    if (pLight->useWorldPosition)
    {
        direction = pLight->worldPosition;
    }
    else
    {
        float angleY = pLight->rotYDeg;
        angleY = (3.1415927f * angleY) / 180.0f;
        static const nlVector3 sDirection = { 1.0f, 0.0f, 0.0f };
        initialDirection = sDirection;
        nlMatrix4 matY;
        nlMatrix4 matZ;

        nlMakeRotationMatrixY(matY, angleY);
        float angleZ = pLight->rotZDeg;
        nlMakeRotationMatrixZ(
            matZ, (3.1415927f * angleZ) / 180.0f);
        nlMultDirVectorMatrix(rotated, initialDirection, matY);
        nlMultDirVectorMatrix(rotated, matZ);

        nlVec3Set(direction, -rotated.x, -rotated.y, -rotated.z);
    }

    if (pLight->isPointLight)
    {
        nlMultPosVectorMatrix(viewPos, direction, mview);
        GXInitLightPos(&light, viewPos.x, viewPos.y, viewPos.z);
        GXInitLightAttnA(&light, 1.0f, 0.0f, 0.0f);
        GXInitLightDistAttn(
            &light, 0.666f * pLight->radius, 0.5f, GX_DA_STEEP);
    }
    else
    {
        nlVector3 origin = {
            0.0f,
            0.0f,
            0.0f,
        };

        float worldY = direction.y - origin.y;
        float worldX = direction.x - origin.x;
        float worldZ = direction.z - origin.z;

        worldDir.x = worldX;
        worldDir.y = worldY;
        worldDir.z = worldZ;

        if (worldDir.GetLengthSq3D() <= 0.0f)
            worldDir.x = 1.0f;

        {
            float lengthSq = worldDir.GetLengthSq3D();
            float recipLength = nlRecipSqrt(lengthSq, true);

            nlVec3Scale(worldDir, recipLength);
        }

        nlMultDirVectorMatrix(viewDir, worldDir, mview);
        nlVec3Scale(viewDir, 1048576.0f);

        GXInitLightPos(&light, viewDir.x, viewDir.y, viewDir.z);
        GXInitLightAttnA(&light, 1.0f, 0.0f, 0.0f);
        GXInitLightDistAttn(&light, 1048576.0f, 1.0f, GX_DA_OFF);
    }

    GXLoadLightObjImm(&light, sGameObjectLightIDs[lightId]);
}

void SetGameObjectLightingEnabled(bool enabled, int count, bool useVertexColour)
{
    if (enabled)
    {
        if (!useVertexColour)
        {
            nlColour colour = {
                0xFF,
                0xFF,
                0xFF,
                0xFF,
            };
            gxSetChanMatColour(0, colour);
        }

        s32 maskIndex = count - 1;
        GXSetChanCtrl(GX_COLOR0, GX_TRUE, GX_SRC_REG, (GXColorSrc)(useVertexColour != 0),
            (GXLightID)sGameObjectLightMasks[maskIndex], GX_DF_CLAMP, GX_AF_SPOT);
    }
    else
    {
        s32 maskIndex = count - 1;
        GXSetChanCtrl(GX_COLOR0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX,
            (GXLightID)sGameObjectLightMasks[maskIndex], GX_DF_NONE, GX_AF_NONE);
    }
}

void LoadGameObjectSpecularLight(int index, GameObjectLight* lightData, float exponent, const nlMatrix4& viewMatrix)
{
    if (index < 0 || index >= 2)
        return;

    GXLightObj light;
    nlVector3 rotated;
    nlVector3 direction;
    nlVector3 initialDirection;
    nlVector3 viewDir;
    nlVector3 worldDir;

    s32 level = (s32)(255.0f * lightData->intensity);
    if (level > 255)
        level = 255;

    GXColor colour = { (u8)level, (u8)level, (u8)level, 0xFF };
    GXInitLightColor(&light, colour);

    if (lightData->useWorldPosition)
    {
        direction = lightData->worldPosition;
    }
    else
    {
        float angleY = lightData->rotYDeg;
        angleY = (3.1415927f * angleY) / 180.0f;
        static const nlVector3 sDirection = { 1.0f, 0.0f, 0.0f };
        initialDirection = sDirection;
        nlMatrix4 matY;
        nlMatrix4 matZ;

        nlMakeRotationMatrixY(matY, angleY);
        float angleZ = lightData->rotZDeg;
        nlMakeRotationMatrixZ(
            matZ, (3.1415927f * angleZ) / 180.0f);
        nlMultDirVectorMatrix(rotated, initialDirection, matY);
        nlMultDirVectorMatrix(rotated, matZ);

        nlVec3Set(direction, -rotated.x, -rotated.y, -rotated.z);
    }

    nlVector3 origin = {
        0.0f,
        0.0f,
        0.0f,
    };

    float worldY = direction.y - origin.y;
    float worldX = direction.x - origin.x;
    float worldZ = direction.z - origin.z;

    worldDir.x = worldX;
    worldDir.y = worldY;
    worldDir.z = worldZ;

    {
        float lengthSq = worldDir.GetLengthSq3D();
        float recipLength = nlRecipSqrt(lengthSq, true);

        nlVec3Scale(worldDir, recipLength);
    }

    nlMultDirVectorMatrix(viewDir, worldDir, viewMatrix);
    nlVec3Set(viewDir, -viewDir.x, -viewDir.y, -viewDir.z);
    GXInitSpecularDir(&light, viewDir.x, viewDir.y, viewDir.z);

    GXInitLightAttn(&light, 0.0f, 0.0f, 1.0f,
        exponent / 2.0f, 0.0f,
        1.0f - exponent / 2.0f);

    GXLoadLightObjImm(&light, sSpecularLightIDs[index]);
}

void SetGameObjectSpecularLightingEnabled(int enabled, int)
{
    if (enabled)
    {
        nlColour colour = {
            0xFF,
            0xFF,
            0xFF,
            0xFF,
        };
        gxSetChanMatColour(1, colour);

        nlColour ambient = {
            0,
            0,
            0,
            0,
        };
        gxSetChanAmbColour(1, ambient);

        GXSetChanCtrl(GX_COLOR1, GX_TRUE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT6,
            GX_DF_NONE, GX_AF_SPEC);
    }
    else
    {
        GXSetChanCtrl(GX_COLOR1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT6,
            GX_DF_NONE, GX_AF_NONE);
    }
}

void SetGameObjectAmbientLightingEnabled(int enabled)
{
    if (enabled)
    {
        gxSetChanAmbColour(0, (nlColour){
            (u8)gGameObjectAmbientRed,
            (u8)gGameObjectAmbientGreen,
            (u8)gGameObjectAmbientBlue,
            0,
        });
    }
    else
    {
        gxSetChanAmbColour(0, (nlColour){ 0, 0, 0, 0 });
    }
}

void LoadShadowLightingLookup(unsigned long textureHandle)
{
    if (textureHandle != (u32)-1 && glTextureLoad(textureHandle))
    {
        gShadowLookupTexture = textureHandle;
        gpShadowLightingLookup = new (8, false) LightingLookup;
        gpShadowLightingLookup->LoadTexture(textureHandle);
    }
    else
    {
        gShadowLookupTexture = -1;
    }
}

void ApplyGameObjectShadowLighting(int skinned, unsigned long shadowLevel)
{
    if (!IsShadowLookupActive())
        return;

    u32 numTevStages;
    u32 numTexGens;
    numTexGens = gxGetNumTexGens();
    numTevStages = gxGetNumTevStages();
    gxSetNumTexGens(numTexGens + 1);
    gxSetNumTevStages(numTevStages + 1);
    gShadowTexGen = numTexGens;

    if (shadowLevel != 0)
    {
        gxSetTevOrder(numTevStages, 0xFF, 0xFF, 0xFF);
        gxSetTevColourIn(numTevStages, 15, 0, 6, 15);
        gxSetTevAlphaIn(numTevStages, 7, 7, 7, 0);

        GXColor colour = *(GXColor*)&shadowLevel;
        GXSetTevColor(GX_TEVREG2, colour);
    }
    else
    {
        gxSetTevOrder(numTevStages, numTexGens, numTexGens, 0xFF);
        if (skinned && gShadowLookupSkinnedUsesInverseView)
            gxSetTexCoordGen(numTexGens, 1, 0, 0, false, 0x76);
        else
            gxSetTexCoordGen(numTexGens, 1, 0, 0x36, false, 0x76);

        gxSetTevColourIn(numTevStages, 15, 0, 8, 15);
        gxSetTevAlphaIn(numTevStages, 7, 7, 7, 0);

        glTextureBinding textureState(gShadowLookupTexture, !gShadowLookupClampWrap, !gShadowLookupClampWrap);
        glx_BindTexture(numTexGens, &textureState);

        nlMatrix4 transform;
        nlMakeScaleMatrix(transform, gShadowLookupScaleX,
            gShadowLookupScaleY, 1.0f);
        transform.SetRow4_(3, gShadowLookupTransX,
            gShadowLookupTransY, 0.0f, 1.0f);

        Mtx gxTransform;
        glxCopyMatrix(gxTransform, transform);

        Mtx textureMatrix;
        PSMTXConcat(gShadowLookupBiasMatrix, gxTransform, textureMatrix);

        if (skinned && gShadowLookupSkinnedUsesInverseView)
        {
            Mtx inverse;
            glxCopyMatrix(inverse, gShadowInverseViewMatrix);
            if (gShadowLookupInverseViewPostMultiply)
                PSMTXConcat(textureMatrix, inverse, textureMatrix);
            else
                PSMTXConcat(inverse, textureMatrix, textureMatrix);
        }

        GXLoadTexMtxImm(textureMatrix, 0x76, GX_MTX3x4);
    }
}

void RestoreGameObjectShadowLighting()
{
    if (IsShadowLookupActive() && gShadowTexGen >= 0)
    {
        gxSetNumTexGens(gxGetNumTexGens() - 1);
        gxSetNumTevStages(gxGetNumTevStages() - 1);
        gxSetTexCoordGen(gShadowTexGen, 1, gShadowTexGen + 4, 0x3C);
        gShadowTexGen = -1;
    }
}

void SetGameObjectShadowModelMatrix(unsigned long matrix)
{
    if (!IsShadowLookupActive())
        return;

    if (matrix == (u32)-1)
    {
        gShadowModelMatrix = -1;
    }
    else if (matrix != gShadowModelMatrix)
    {
        gShadowModelMatrix = matrix;

        nlMatrix4 transform;
        glGetMatrix(matrix, transform);

        Mtx textureMatrix;
        glxCopyMatrix(textureMatrix, transform);
        GXLoadTexMtxImm(textureMatrix, 0x36, GX_MTX3x4);
    }
}

void SetGameObjectShadowViewMatrix(const nlMatrix4* matrix)
{
    if (IsShadowLookupActive())
        nlInvertMatrix(gShadowInverseViewMatrix, *matrix);
}

bool IsShadowLookupActive()
{
    if (!gShadowLookupEnabled)
        return false;

    if (gShadowLookupTexture == (u32)-1)
        return false;

    if (gpShadowLightingLookup == 0)
        return false;

    if (!g_bRenderWorldEffects)
        return false;

    return true;
}

nlColour SampleShadowLookup(const nlVector2* pPosition, bool tint)
{
    if (!IsShadowLookupActive())
    {
        nlColour white;
        nlColourSet(white, 0xFF, 0xFF, 0xFF, 0xFF);
        return white;
    }

    if (lbl_806DCC5C <= 0xFF)
    {
        nlColour white;
        nlColourSet(white, 0xFF, 0xFF, 0xFF, 0xFF);
        return white;
    }

    f32 u = pPosition->x;
    u *= gShadowLookupScaleX.value;
    u = u + gShadowLookupTransX.value;
    f32 v = pPosition->y;
    v *= gShadowLookupScaleY.value;
    v = v + gShadowLookupTransY.value;
    u = 0.5f * u + 0.5f;
    v = -0.5f * v + 0.5f;
    u *= (f32)gpShadowLightingLookup->mWidth;
    v *= (f32)gpShadowLightingLookup->mHeight;
    return gpShadowLightingLookup->SampleFilteredColour(u, v, tint);
}

int GetShadowLookupLevel(const nlVector3* pPosition)
{
    nlColour colour = SampleShadowLookup((const nlVector2*)pPosition, true);
    return (colour.c[0] * 140 + colour.c[1] * 88 + colour.c[2] * 29) >> 8;
}

void ReleaseShadowLightingLookup()
{
    if (gpShadowLightingLookup != 0)
    {
        delete gpShadowLightingLookup;
        gpShadowLightingLookup = 0;
    }
    gShadowLookupTexture = -1;
}

void SetImpostorShadowLevel(ImpostorModel* pImpostor, glModel* pModel)
{
    int level = GetShadowLookupLevel(&pImpostor->mWorldMatrix.GetTranslation());
    nlColour colour;
    nlColourSet(colour, level, level, level, 1);
    unsigned long packedColour = *(unsigned long*)&colour;
    static unsigned long sShadowLevelHash = nlStringLowerHash("shadowLevel");
    for (glModelPacket* pPacket = pModel->packets; pPacket < pModel->packets + pModel->numPackets; ++pPacket)
    {
        glSetMaterialUnsignedParameter(pPacket, sShadowLevelHash, packedColour);
    }
}

void SetImpostorPacketShadowLevels(ImpostorModel*, glModel* pModel)
{
    static unsigned long sShadowLevelHash = nlStringLowerHash("shadowLevel");
    for (glModelPacket* pPacket = pModel->packets; pPacket < pModel->packets + pModel->numPackets; ++pPacket)
    {
        nlMatrix4 matrix;
        glGetMatrix(pPacket->matrix, matrix);
        nlVector3 position = matrix.GetTranslation();
        nlColour colour = SampleShadowLookup((const nlVector2*)&position, false);
        unsigned long packedColour = *(unsigned long*)&colour;
        if (glHasMaterialParameter(pPacket, sShadowLevelHash))
        {
            glSetMaterialUnsignedParameter(pPacket, sShadowLevelHash, packedColour);
        }
    }
}

GameObjectLight::GameObjectLight()
{
    useWorldPosition = false;
    useColour = false;
    isPointLight = false;
}
