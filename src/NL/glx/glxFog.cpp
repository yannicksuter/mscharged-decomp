#include <revolution/gx/GXPixel.h>

#include "Game/TweakValueFloat.h"
#include "Game/TweakValueInt.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/glx/glxSend.h"

static float glx_FogNear = 0.25f;
static float glx_FogFar = 130.0f;

static TweakValueFloat glx_FogStart(
    "sfFogStart", "/Render/Fog", 5.0f);
static TweakValueFloat glx_FogEnd(
    "sfFogEnd", "/Render/Fog", 160.0f);
static TweakValueInt glx_FogRed(
    "siFogRed", gLastTweakCategory, 255);
static TweakValueInt glx_FogGreen(
    "siFogGreen", gLastTweakCategory, 255);
static TweakValueInt glx_FogBlue(
    "siFogBlue", gLastTweakCategory, 255);
static TweakValueFloat glx_FogIntensity(
    "sfFogIntensity", gLastTweakCategory, 1.0f);
static TweakValueBool glx_bFog(
    "sbFogEnabled", gLastTweakCategory, false);
static TweakValueInt glx_FogType(
    "siFogType", gLastTweakCategory, 0);

static GXFogType fogtype[] = {
    GX_FOG_PERSP_LIN,
    GX_FOG_PERSP_EXP,
    GX_FOG_PERSP_EXP2,
    GX_FOG_PERSP_REVEXP,
    GX_FOG_PERSP_REVEXP2,
};

void glx_SetFogClipPlanes(float nearPlane, float farPlane)
{
    glx_FogNear = nearPlane;
    glx_FogFar = farPlane;
}

float glx_GetFogStart()
{
    return glx_FogStart.value;
}

void glx_SetFogStart(float value)
{
    glx_FogStart.value = value;
}

float glx_GetFogEnd()
{
    return glx_FogEnd.value;
}

void glx_SetFogEnd(float value)
{
    glx_FogEnd.value = value;
}

void glx_Fog(bool enable)
{
    if (enable && glx_bFog)
    {
        GXColor fogColour;
        float r = glx_FogRed.value;
        float rScaled = r * glx_FogIntensity.value;
        fogColour.r = (s32)rScaled;
        float g = glx_FogGreen.value;
        float gScaled = g * glx_FogIntensity.value;
        fogColour.g = (s32)gScaled;
        float b = glx_FogBlue.value;
        float bScaled = b * glx_FogIntensity.value;
        fogColour.b = (s32)bScaled;
        fogColour.a = 0xFF;
        GXSetFog(fogtype[glx_FogType.value], fogColour, glx_GetFogStart(), glx_GetFogEnd(), glx_FogNear, glx_FogFar);
    }
    else
    {
        GXColor fogColour;
        fogColour.r = 0xFF;
        fogColour.g = 0xFF;
        fogColour.b = 0xFF;
        fogColour.a = 0xFF;
        GXSetFog(GX_FOG_NONE, fogColour, 0.0f, 0.0f, 0.0f, 0.0f);
    }
}
