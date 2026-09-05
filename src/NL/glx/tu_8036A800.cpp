#include <revolution/gx.h>
#include <string.h>

#include "NL/glx/tu_8036A800.h"
#include "NL/glx/glxGX.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

static GXLightObj lbl_80589DF8[8];
static GXLightObj lbl_80589FF8[8];

extern "C" void fn_8036A800(unsigned int index, const nlVector3* vector, const nlFloatColour* pColour)
{
    GXLightObj* light = &lbl_80589DF8[index];
    nlColour colour;
    ConvertColour(colour, *pColour);
    GXInitLightDir(light, vector->x, vector->y, vector->z);
    GXInitLightPos(light, 100000.0f * vector->x, 100000.0f * vector->y, 100000.0f * vector->z);
    GXInitLightColor(light, *(GXColor*)&colour);

    if (memcmp(light, &lbl_80589FF8[index], sizeof(GXLightObj)) != 0)
    {
        GXLoadLightObjImm(light, (GXLightID)(1 << index));
        memcpy(&lbl_80589FF8[index], light, sizeof(GXLightObj));
    }
}

extern "C" void fn_8036A938(const nlFloatColour* pColour)
{
    nlColour colour;
    ConvertColour(colour, *pColour);
    gxSetChanAmbColour(0, colour);
}

extern "C" void fn_8036A9C4(unsigned int index, const nlVector3* vector, const nlFloatColour* pColour, float value)
{
    GXLightObj* light = &lbl_80589DF8[index];
    nlColour colour;
    ConvertColour(colour, *pColour);
    GXInitLightPos(light, vector->x, vector->y, vector->z);
    GXInitLightColor(light, *(GXColor*)&colour);
    GXInitLightAttnA(light, 1.0f, 0.0f, 0.0f);
    if (value > 0.0f)
        GXInitLightDistAttn(light, value, 1.0f / 256.0f, GX_DA_STEEP);
    else
        GXInitLightDistAttn(light, 0.0f, 0.0f, GX_DA_OFF);

    if (memcmp(light, &lbl_80589FF8[index], sizeof(GXLightObj)) != 0)
    {
        GXLoadLightObjImm(light, (GXLightID)(1 << index));
        memcpy(&lbl_80589FF8[index], light, sizeof(GXLightObj));
    }
}

extern "C" void fn_8036AB40(unsigned int index, const nlVector3* vector, const nlFloatColour* pColour, float value)
{
    GXLightObj* light = &lbl_80589DF8[index];
    nlColour colour;
    ConvertColour(colour, *pColour);
    GXInitLightColor(light, *(GXColor*)&colour);
    GXInitSpecularDir(light, vector->x, vector->y, vector->z);
    GXInitLightAttn(light, 0.0f, 0.0f, 1.0f, value * 0.5f, 0.0f, 1.0f - value * 0.5f);

    if (memcmp(light, &lbl_80589FF8[index], sizeof(GXLightObj)) != 0)
    {
        GXLoadLightObjImm(light, (GXLightID)(1 << index));
        memcpy(&lbl_80589FF8[index], light, sizeof(GXLightObj));
    }
}
