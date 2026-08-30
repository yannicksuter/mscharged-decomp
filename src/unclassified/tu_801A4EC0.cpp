#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/glx/GXMaterialShadowTweaks.h"
#include "NL/glx/glxTexture.h"
#include "NL/gl/glTexture.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

class ArrayOwner_801A4EC0
{
public:
    ~ArrayOwner_801A4EC0();
    u8* data;
};

class ArrayOwner_801A4F18
{
public:
    ~ArrayOwner_801A4F18();
    u8* data;
};

class ArrayOwner_801A4F70
{
public:
    ~ArrayOwner_801A4F70();
    u8* data;
};

struct MatrixOwner_801A4FC8
{
    char _000[0x20];
    nlMatrix4 matrix;
};

struct FloatReferences_801A505C
{
    char _000[0x48];
    float* value48;
    char _04C[0x0C];
    float* value58;
    char _05C[0x0C];
    float* value68;
};

class Lookup_801A537C
{
public:
    Lookup_801A537C();
    ~Lookup_801A537C();
    void fn_801A53F0(u32 textureHandle);
    nlColour fn_801A54D0(int x, int y, bool tint) const;
    nlColour fn_801A5760(float x, float y, bool tint) const;
    u8 fn_801A59EC(const PlatTexture* texture, int x, int y) const;

    /* 0x00 */ u8* mValues;
    /* 0x04 */ int mWidth;
    /* 0x08 */ int mHeight;
}; // size: 0x0C

struct ImpostorEntry_801A51D8
{
    char _000[0x0C];
    nlVector2 lookupPosition;
    char _014[0x0E];
    nlColour colour;
    char _026[0x0A];
}; // size: 0x30

struct ImpostorEntries_801A51D8
{
    char _000[4];
    ImpostorEntry_801A51D8* entries;
};

extern GXMaterialFloatTweak_804F4190 lbl_80570968;
extern GXMaterialFloatTweak_804F4190 lbl_80570988;
extern GXMaterialFloatTweak_804F4190 lbl_805709A8;
extern GXMaterialFloatTweak_804F4190 lbl_805709C8;

extern "C"
{
    ImpostorEntries_801A51D8* fn_802D5F6C();
    int fn_802D6118(ImpostorEntries_801A51D8* entries);
}

static GXMaterialColourTweak_804FC520 g_ShadowRed(
    "g_ShadowRed", "/Render/Impostor/Lookup/Tint");
static GXMaterialColourTweak_804FC520 g_ShadowGreen(
    "g_ShadowGreen", lbl_806E1E90);
static GXMaterialColourTweak_804FC520 g_ShadowBlue(
    "g_ShadowBlue", lbl_806E1E90);
static GXMaterialColourTweak_804FC520 g_HighlightRed(
    "g_HighlightRed", lbl_806E1E90, 255);
static GXMaterialColourTweak_804FC520 g_HighlightGreen(
    "g_HighlightGreen", lbl_806E1E90, 255);
static GXMaterialColourTweak_804FC520 g_HighlightBlue(
    "g_HighlightBlue", lbl_806E1E90, 255);

static u32 lbl_806DCF90 = -1;
static Lookup_801A537C* lbl_806E15B8;

ArrayOwner_801A4EC0::~ArrayOwner_801A4EC0()
{
    delete[] data;
}

ArrayOwner_801A4F18::~ArrayOwner_801A4F18()
{
    delete[] data;
}

ArrayOwner_801A4F70::~ArrayOwner_801A4F70()
{
    delete[] data;
}

extern "C" void fn_801A4FC8(
    MatrixOwner_801A4FC8* owner, const nlMatrix4& matrix)
{
    owner->matrix = matrix;
}

extern "C" float fn_801A505C(FloatReferences_801A505C* references)
{
    return *references->value58;
}

extern "C" float fn_801A5068(FloatReferences_801A505C* references)
{
    return *references->value68;
}

extern "C" float fn_801A5074(FloatReferences_801A505C* references)
{
    return *references->value48;
}

extern "C" void fn_801A5080(
    FloatReferences_801A505C* references, float value)
{
    *references->value48 = value;
}

extern "C" nlColour fn_801A508C(const nlVector2* position)
{
    nlColour colour;
    if (lbl_806E15B8 == 0)
    {
        nlColourSet(colour, 255, 255, 255, 255);
        return colour;
    }

    float x = position->x * lbl_80570968.value;
    x += lbl_805709A8.value;
    x = 0.5f * x + 0.5f;
    float y = position->y * lbl_80570988.value;
    y += lbl_805709C8.value;
    y = -0.5f * y + 0.5f;
    int lookupX = (int)(x * (lbl_806E15B8->mWidth - 1));
    int lookupY = (int)(y * (lbl_806E15B8->mHeight - 1));
    if (lookupX >= lbl_806E15B8->mWidth)
    {
        lookupX = lbl_806E15B8->mWidth - 1;
    }
    if (lookupY >= lbl_806E15B8->mHeight)
    {
        lookupY = lbl_806E15B8->mHeight - 1;
    }
    colour = lbl_806E15B8->fn_801A54D0(lookupX, lookupY, true);
    return colour;
}

extern "C" void fn_801A51D8()
{
    if (lbl_806DCF90 == (u32)-1)
    {
        return;
    }

    ImpostorEntries_801A51D8* impostors = fn_802D5F6C();
    int count = fn_802D6118(impostors);
    ImpostorEntry_801A51D8* entry = fn_802D5F6C()->entries;
    for (int i = 0; i < count; ++i, ++entry)
    {
        entry->colour = fn_801A508C(&entry->lookupPosition);
    }
}

extern "C" void fn_801A5284(u32 textureHandle)
{
    if (textureHandle == (u32)-1)
    {
        lbl_806DCF90 = -1;
    }
    else
    {
        lbl_806DCF90 = glTextureLoad(textureHandle)
                         ? textureHandle
                         : (u32)-1;
    }

    if (lbl_806DCF90 != (u32)-1 && lbl_806E15B8 == 0)
    {
        lbl_806E15B8 = new (8, false) Lookup_801A537C;
        lbl_806E15B8->fn_801A53F0(textureHandle);
    }
}

extern "C" void fn_801A5328()
{
    if (lbl_806E15B8 != 0)
    {
        delete lbl_806E15B8;
        lbl_806E15B8 = 0;
    }
}

Lookup_801A537C::Lookup_801A537C()
    : mValues(0)
    , mWidth(0)
    , mHeight(0)
{
}

Lookup_801A537C::~Lookup_801A537C()
{
    if (mValues != 0)
    {
        delete[] mValues;
    }
}

void Lookup_801A537C::fn_801A53F0(u32 textureHandle)
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
                *output = fn_801A59EC(texture, x, y);
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

nlColour Lookup_801A537C::fn_801A54D0(
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

nlColour Lookup_801A537C::fn_801A5760(
    float x, float y, bool tint) const
{
    int ix = (int)x;
    int iy = (int)y;
    nlColour centre = fn_801A54D0(ix, iy, tint);
    nlColour up = fn_801A54D0(ix, iy - 1, tint);
    nlColour down = fn_801A54D0(ix, iy + 1, tint);
    nlColour left = fn_801A54D0(ix - 1, iy, tint);
    nlColour right = fn_801A54D0(ix + 1, iy, tint);
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

u8 Lookup_801A537C::fn_801A59EC(
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
