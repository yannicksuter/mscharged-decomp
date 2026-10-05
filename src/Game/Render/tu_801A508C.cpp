#include "Game/Render/ImpostorLighting.h"
#include "Game/GameObjectLighting.h"
#include "Game/Render/LightingLookup.h"
#include "Game/TweakValueFloat.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

static inline void GetImpostorLightingCoordinate(nlVector2& coordinate, const nlVector3* position)
{
    float x = position->x;
    x = x * gShadowLookupScaleX.value;
    x = x + gShadowLookupTransX.value;
    float y = position->y;
    y = y * gShadowLookupScaleY.value;
    y = y + gShadowLookupTransY.value;
    nlVec2Set(coordinate, 0.5f * x + 0.5f, -0.5f * y + 0.5f);
}

static inline nlColour SampleImpostorLighting(LightingLookup* lookup, const nlVector2& coordinate)
{
    int height = lookup->mHeight;
    int width = lookup->mWidth;
    int x = (int)(coordinate.x * (width - 1));
    int y = (int)(coordinate.y * (height - 1));
    if (x >= width)
    {
        x = width - 1;
    }
    if (y >= height)
    {
        y = height - 1;
    }
    return lookup->SampleColour(x, y, true);
}

nlColour GetImpostorLightingColour(const nlVector3* position)
{
    nlColour colour;
    if (spImpostorLightingLookup == 0)
    {
        nlColourSet(colour, 255, 255, 255, 255);
    }
    else
    {
        LightingLookup* lookup = spImpostorLightingLookup;
        nlVector2 coordinate;
        GetImpostorLightingCoordinate(coordinate, position);
        colour = SampleImpostorLighting(lookup, coordinate);
    }
    return colour;
}
