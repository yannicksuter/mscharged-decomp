#include "Game/Render/ImpostorLighting.h"
#include "Game/GameObjectLighting.h"
#include "Game/Render/CrowdImpostorManager.h"
#include "Game/Render/Impostor.h"
#include "Game/Render/ImpostorCharacter.h"
#include "Game/Render/ImpostorManager.h"
#include "Game/TweakValue.h"
#include "Game/TweakValueInt.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/glx/glxTexture.h"
#include "NL/gl/glTexture.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "Game/Render/LightingLookup.h"
#include "Game/Render/CrowdImpostors.h"
#include "Game/TweakValueFloat.h"


static TweakValueInt g_ShadowRed(
    "g_ShadowRed", "/Render/Impostor/Lookup/Tint", 0);
static TweakValueInt g_ShadowGreen(
    "g_ShadowGreen", gLastTweakCategory, 0);
static TweakValueInt g_ShadowBlue(
    "g_ShadowBlue", gLastTweakCategory, 0);
static TweakValueInt g_HighlightRed(
    "g_HighlightRed", gLastTweakCategory, 255);
static TweakValueInt g_HighlightGreen(
    "g_HighlightGreen", gLastTweakCategory, 255);
static TweakValueInt g_HighlightBlue(
    "g_HighlightBlue", gLastTweakCategory, 255);

static u32 sImpostorLightingTexture = -1;
LightingLookup* spImpostorLightingLookup;

void UpdateImpostorLighting()
{
    if (sImpostorLightingTexture == (u32)-1)
    {
        return;
    }

    ImpostorManager* impostors = ImpostorManager::GetInstance();
    int count = impostors->GetNumImpostors();
    Impostor* entry = ImpostorManager::GetInstance()->mImpostors;
    for (int i = 0; i < count; ++i, ++entry)
    {
        entry->mColour = GetImpostorLightingColour(&entry->mPosition);
    }
}

void SetImpostorLightingTexture(u32 textureHandle)
{
    if (textureHandle == (u32)-1)
    {
        sImpostorLightingTexture = -1;
    }
    else
    {
        sImpostorLightingTexture = glTextureLoad(textureHandle)
                         ? textureHandle
                         : (u32)-1;
    }

    if (sImpostorLightingTexture != (u32)-1 && spImpostorLightingLookup == 0)
    {
        spImpostorLightingLookup = new (8, false) LightingLookup;
        spImpostorLightingLookup->LoadTexture(textureHandle);
    }
}

void FreeImpostorLighting()
{
    if (spImpostorLightingLookup != 0)
    {
        delete spImpostorLightingLookup;
        spImpostorLightingLookup = 0;
    }
}

LightingLookup::LightingLookup()
    : mValues(0)
    , mWidth(0)
    , mHeight(0)
{
}

LightingLookup::~LightingLookup()
{
    if (mValues != 0)
    {
        delete[] mValues;
    }
}

void LightingLookup::LoadTexture(u32 textureHandle)
{
    if (mValues != 0)
    {
        delete[] mValues;
        mValues = 0;
    }

    if (glTextureLoad(textureHandle))
    {
        int height;
        int width;
        int y;
        int x;
        PlatTexture* texture = glx_GetTex(textureHandle);
        width = texture->m_Width;
        mWidth = width;
        height = texture->m_Height;
        mHeight = height;
        mValues = new (8, false) u8[width * height];

        u8* output = mValues;
        for (y = 0; y < height; ++y)
        {
            for (x = 0; x < width; ++x, ++output)
            {
                *output = ReadTextureIntensity(texture, x, y);
            }
        }
    }
    else
    {
        mWidth = 0;
        mHeight = 0;
    }
}

nlColour LightingLookup::SampleColour(
    int x, int y, bool tint) const
{
    if (x < 0)
    {
        x = 0;
    }
    if (y < 0)
    {
        y = 0;
    }
    if (x >= mWidth)
    {
        x = mWidth - 1;
    }
    if (y >= mHeight)
    {
        y = mHeight - 1;
    }

    u8 value = mValues[y * mWidth + x];
    nlColour colour;
    if (tint)
    {
        u8 shadowColour[4] = { 0, 0, 0, 255 };
        shadowColour[0] = (u8)g_ShadowRed.value;
        shadowColour[1] = (u8)g_ShadowGreen.value;
        shadowColour[2] = (u8)g_ShadowBlue.value;
        nlFloatColour shadow;
        shadow.c[0] = shadowColour[0] * (1.0f / 255.0f);
        shadow.c[1] = shadowColour[1] * (1.0f / 255.0f);
        shadow.c[2] = shadowColour[2] * (1.0f / 255.0f);
        shadow.c[3] = shadowColour[3] * (1.0f / 255.0f);
        u8 highlightColour[4] = { 0, 0, 0, 255 };
        highlightColour[0] = (u8)g_HighlightRed.value;
        highlightColour[1] = (u8)g_HighlightGreen.value;
        highlightColour[2] = (u8)g_HighlightBlue.value;
        nlFloatColour highlight;
        highlight.c[0] = highlightColour[0] * (1.0f / 255.0f);
        highlight.c[1] = highlightColour[1] * (1.0f / 255.0f);
        highlight.c[2] = highlightColour[2] * (1.0f / 255.0f);
        highlight.c[3] = highlightColour[3] * (1.0f / 255.0f);
        float inverseFactor = 1.0f - value / 255.0f;
        nlFloatColour result;
        result.c[0] = inverseFactor * shadow.c[0] + (value / 255.0f) * highlight.c[0];
        result.c[1] = inverseFactor * shadow.c[1] + (value / 255.0f) * highlight.c[1];
        result.c[2] = inverseFactor * shadow.c[2] + (value / 255.0f) * highlight.c[2];
        result.c[3] = inverseFactor * shadow.c[3] + (value / 255.0f) * highlight.c[3];
        ConvertColour(colour, result);
    }
    else
    {
        nlColourSet(colour, value, value, value, 255);
    }
    return colour;
}

nlColour LightingLookup::SampleFilteredColour(
    float x, float y, bool tint) const
{
    int ix = (int)x;
    int iy = (int)y;
    nlColour samples[5];
    samples[0] = SampleColour(ix, iy, tint);
    samples[1] = SampleColour(ix, iy - 1, tint);
    samples[2] = SampleColour(ix, iy + 1, tint);
    samples[3] = SampleColour(ix - 1, iy, tint);
    samples[4] = SampleColour(ix + 1, iy, tint);
    int red = 3 * samples[0][0];
    int green = 3 * samples[0][1];
    int blue = 3 * samples[0][2];
    int alpha = 3 * samples[0][3];

    red += samples[1][0];
    green += samples[1][1];
    blue += samples[1][2];
    alpha += samples[1][3];

    red += samples[2][0];
    green += samples[2][1];
    blue += samples[2][2];
    alpha += samples[2][3];

    red += samples[3][0];
    green += samples[3][1];
    blue += samples[3][2];
    alpha += samples[3][3];

    red += samples[4][0];
    green += samples[4][1];
    blue += samples[4][2];
    alpha += samples[4][3];

    nlColour result;
    result[0] = red / 7;
    result[1] = green / 7;
    result[2] = blue / 7;
    result[3] = alpha / 7;
    return result;
}

u8 LightingLookup::ReadTextureIntensity(
    const PlatTexture* texture, int x, int y) const
{
    int tileX = x >> 3;
    int tileY = y >> 2;
    int tilesPerRow = texture->m_Width >> 3;
    int tile = tileY * tilesPerRow + tileX;
    int texelX = x & 7;
    int texelY = y & 3;
    int offset = (tile << 5) + (texelY << 3) + texelX;
    u8 paletteIndex = ((u8*)texture->m_SwizzledData)[offset];
    u16 value = texture->m_PaletteData[paletteIndex];
    if (value & 0x8000)
    {
        int component = (value >> 10) & 0x1F;
        return component * 255 / 31;
    }
    else
    {
        int component = (value >> 8) & 0x0F;
        return component * 255 / 15;
    }
}
