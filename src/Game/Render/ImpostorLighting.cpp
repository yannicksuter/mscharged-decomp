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
static LightingLookup* spImpostorLightingLookup;

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

CrowdModelArray::~CrowdModelArray()
{
    delete[] data;
}

CrowdCharacterArray::~CrowdCharacterArray()
{
    delete[] data;
}

CrowdDefinitionArray::~CrowdDefinitionArray()
{
    delete[] data;
}

void CrowdLayoutObject::SetWorldMatrix(const nlMatrix4& matrix)
{
    mTransform = matrix;
}

nlColour GetImpostorLightingColour(const nlVector3* position)
{
    nlColour colour;
    if (spImpostorLightingLookup == 0)
    {
        nlColourSet(colour, 255, 255, 255, 255);
        return colour;
    }

    float x = position->x * gShadowLookupScaleX.value;
    x += gShadowLookupTransX.value;
    x = 0.5f * x + 0.5f;
    float y = position->y * gShadowLookupScaleY.value;
    y += gShadowLookupTransY.value;
    y = -0.5f * y + 0.5f;
    int lookupX = (int)(x * (spImpostorLightingLookup->mWidth - 1));
    int lookupY = (int)(y * (spImpostorLightingLookup->mHeight - 1));
    if (lookupX >= spImpostorLightingLookup->mWidth)
    {
        lookupX = spImpostorLightingLookup->mWidth - 1;
    }
    if (lookupY >= spImpostorLightingLookup->mHeight)
    {
        lookupY = spImpostorLightingLookup->mHeight - 1;
    }
    colour = spImpostorLightingLookup->SampleColour(lookupX, lookupY, true);
    return colour;
}

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

void LightingLookup::LoadTexture(u32 textureHandle)
{
    if (mValues != 0)
    {
        delete[] mValues;
        mValues = 0;
    }

    if (glTextureLoad(textureHandle))
    {
        PlatTexture* texture = glx_GetTex(textureHandle);
        mWidth = texture->m_Width;
        mHeight = texture->m_Height;
        mValues = new (8, false) u8[mWidth * mHeight];

        u8* output = mValues;
        for (int y = 0; y < mHeight; ++y)
        {
            for (int x = 0; x < mWidth; ++x)
            {
                *output = ReadTextureIntensity(texture, x, y);
                ++output;
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
        nlColour shadowColour = { {
            (u8)g_ShadowRed.value,
            (u8)g_ShadowGreen.value,
            (u8)g_ShadowBlue.value,
            255,
        } };
        nlColour highlightColour = { {
            (u8)g_HighlightRed.value,
            (u8)g_HighlightGreen.value,
            (u8)g_HighlightBlue.value,
            255,
        } };
        nlFloatColour shadow;
        shadow.c[0] = shadowColour.c[0] * (1.0f / 255.0f);
        shadow.c[1] = shadowColour.c[1] * (1.0f / 255.0f);
        shadow.c[2] = shadowColour.c[2] * (1.0f / 255.0f);
        shadow.c[3] = shadowColour.c[3] * (1.0f / 255.0f);
        nlFloatColour highlight;
        highlight.c[0] = highlightColour.c[0] * (1.0f / 255.0f);
        highlight.c[1] = highlightColour.c[1] * (1.0f / 255.0f);
        highlight.c[2] = highlightColour.c[2] * (1.0f / 255.0f);
        highlight.c[3] = highlightColour.c[3] * (1.0f / 255.0f);
        float factor = value / 255.0f;
        float inverseFactor = 1.0f - factor;
        nlFloatColour result;
        result.c[0] = inverseFactor * shadow.c[0] + factor * highlight.c[0];
        result.c[1] = inverseFactor * shadow.c[1] + factor * highlight.c[1];
        result.c[2] = inverseFactor * shadow.c[2] + factor * highlight.c[2];
        result.c[3] = inverseFactor * shadow.c[3] + factor * highlight.c[3];
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
    nlColour centre = SampleColour(ix, iy, tint);
    nlColour up = SampleColour(ix, iy - 1, tint);
    nlColour down = SampleColour(ix, iy + 1, tint);
    nlColour left = SampleColour(ix - 1, iy, tint);
    nlColour right = SampleColour(ix + 1, iy, tint);
    nlColour result;
    result.c[0] = (3 * centre.c[0] + up.c[0] + down.c[0]
                      + left.c[0] + right.c[0])
                / 7;
    result.c[1] = (3 * centre.c[1] + up.c[1] + down.c[1]
                      + left.c[1] + right.c[1])
                / 7;
    result.c[2] = (3 * centre.c[2] + up.c[2] + down.c[2]
                      + left.c[2] + right.c[2])
                / 7;
    result.c[3] = (3 * centre.c[3] + up.c[3] + down.c[3]
                      + left.c[3] + right.c[3])
                / 7;
    return result;
}

u8 LightingLookup::ReadTextureIntensity(
    const PlatTexture* texture, int x, int y) const
{
    int block = (y >> 2) * (texture->m_Width >> 3) + (x >> 3);
    int offset = (block << 5) + ((y & 3) << 3) + (x & 7);
    u8 paletteIndex = ((u8*)texture->m_SwizzledData)[offset];
    u16 value = texture->m_PaletteData[paletteIndex];
    if (value & 0x8000)
    {
        int component = (value >> 10) & 0x1F;
        return component * 255 / 31;
    }

    int component = (value >> 8) & 0x0F;
    return component * 255 / 15;
}
