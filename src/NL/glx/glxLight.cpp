#include <revolution/gx.h>
#include <string.h>

#include "NL/glx/glxLight.h"
#include "NL/glx/glxGX.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

static GXLightObj glx_LightObjects[8];
static GXLightObj glx_LoadedLightObjects[8];

void glx_LoadDirectionalLight(unsigned int index, const nlVector3* vector, const nlFloatColour* pColour)
{
    GXLightObj* light = &glx_LightObjects[index];
    nlColour colour;
    ConvertColour(colour, *pColour);
    GXInitLightDir(light, vector->x, vector->y, vector->z);
    GXInitLightPos(light, 100000.0f * vector->x, 100000.0f * vector->y, 100000.0f * vector->z);
    GXInitLightColor(light, *(GXColor*)&colour);

    if (memcmp(light, &glx_LoadedLightObjects[index], sizeof(GXLightObj)) != 0)
    {
        GXLoadLightObjImm(light, (GXLightID)(1 << index));
        memcpy(&glx_LoadedLightObjects[index], light, sizeof(GXLightObj));
    }
}

void glx_SetAmbientColour(const nlFloatColour* pColour)
{
    nlColour colour;
    ConvertColour(colour, *pColour);
    gxSetChanAmbColour(0, colour);
}

void glx_LoadPointLight(unsigned int index, const nlVector3* vector, const nlFloatColour* pColour, float value)
{
    GXLightObj* light = &glx_LightObjects[index];
    nlColour colour;
    ConvertColour(colour, *pColour);
    GXInitLightPos(light, vector->x, vector->y, vector->z);
    GXInitLightColor(light, *(GXColor*)&colour);
    GXInitLightAttnA(light, 1.0f, 0.0f, 0.0f);
    if (value > 0.0f)
        GXInitLightDistAttn(light, value, 1.0f / 256.0f, GX_DA_STEEP);
    else
        GXInitLightDistAttn(light, 0.0f, 0.0f, GX_DA_OFF);

    if (memcmp(light, &glx_LoadedLightObjects[index], sizeof(GXLightObj)) != 0)
    {
        GXLoadLightObjImm(light, (GXLightID)(1 << index));
        memcpy(&glx_LoadedLightObjects[index], light, sizeof(GXLightObj));
    }
}

void glx_LoadSpecular(unsigned int index, const nlVector3* vector, const nlFloatColour* pColour, float value)
{
    GXLightObj* light = &glx_LightObjects[index];
    nlColour colour;
    ConvertColour(colour, *pColour);
    GXInitLightColor(light, *(GXColor*)&colour);
    GXInitSpecularDir(light, vector->x, vector->y, vector->z);
    GXInitLightAttn(light, 0.0f, 0.0f, 1.0f, value * 0.5f, 0.0f, 1.0f - value * 0.5f);

    if (memcmp(light, &glx_LoadedLightObjects[index], sizeof(GXLightObj)) != 0)
    {
        GXLoadLightObjImm(light, (GXLightID)(1 << index));
        memcpy(&glx_LoadedLightObjects[index], light, sizeof(GXLightObj));
    }
}
