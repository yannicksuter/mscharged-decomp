#include <revolution/gx.h>
#include <string.h>

#include "NL/glx/glxLight.h"
#include "NL/glx/glxGX.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

static GXLightObj glx_LightObjects[8];
static GXLightObj glx_LoadedLightObjects[8];

static inline void LoadLightIfChanged(unsigned int index, GXLightObj* light)
{
    GXLightObj* loaded = &glx_LoadedLightObjects[index];
    if (memcmp(light, loaded, sizeof(GXLightObj)) != 0)
    {
        GXLoadLightObjImm(light, (GXLightID)(1 << index));
        memcpy(loaded, light, sizeof(GXLightObj));
    }
}

static inline GXColor ConvertLightColour(const nlFloatColour& input)
{
    nlColour colour;
    ConvertColour(colour, input);
    return *(GXColor*)&colour;
}

void glx_LoadDirectionalLight(unsigned int index, const nlVector3* vector, nlFloatColour* pColour)
{
    GXColor lightColour = ConvertLightColour(*pColour);
    GXInitLightDir(&glx_LightObjects[index], vector->x, vector->y, vector->z);
    GXInitLightPos(&glx_LightObjects[index], 100000.0f * vector->x, 100000.0f * vector->y, 100000.0f * vector->z);
    GXInitLightColor(&glx_LightObjects[index], lightColour);

    LoadLightIfChanged(index, &glx_LightObjects[index]);
}

void glx_SetAmbientColour(nlFloatColour* pColour)
{
    nlColour colour;
    ConvertColour(colour, *pColour);
    gxSetChanAmbColour(0, colour);
}

void glx_LoadPointLight(unsigned int index, const nlVector3* vector, nlFloatColour* pColour, float value)
{
    GXColor lightColour = ConvertLightColour(*pColour);
    GXLightObj* light = &glx_LightObjects[index];
    GXInitLightPos(light, vector->x, vector->y, vector->z);
    GXInitLightColor(light, lightColour);
    GXInitLightAttnA(light, 1.0f, 0.0f, 0.0f);
    if (value > 0.0f)
        GXInitLightDistAttn(light, value, 1.0f / 256.0f, GX_DA_STEEP);
    else
        GXInitLightDistAttn(light, 0.0f, 0.0f, GX_DA_OFF);

    LoadLightIfChanged(index, light);
}

void glx_LoadSpecular(unsigned int index, const nlVector3* vector, nlFloatColour* pColour, float value)
{
    GXColor lightColour = ConvertLightColour(*pColour);
    GXLightObj* light = &glx_LightObjects[index];
    GXInitLightColor(light, lightColour);
    GXInitSpecularDir(light, vector->x, vector->y, vector->z);
    float half = 0.5f;
    GXInitLightAttn(light, 0.0f, 0.0f, 1.0f, value * half, 0.0f, 1.0f - value * half);

    LoadLightIfChanged(index, light);
}
