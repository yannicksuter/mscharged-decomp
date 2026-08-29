#include "NL/gl/gl.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

#include "types.h"

extern "C" GLView* fn_8027267C(int);
extern "C" void nlZeroMemory(void*, unsigned long);

struct ModelWriter_802A1BF4
{
    int count;
    glModel* model;
    void* allocator;
    float* positions;
    short* textureCoordinates;
    u32* colours;
};

extern "C" void fn_802A1BF4(ModelWriter_802A1BF4*);
extern "C" void* fn_802A1C14(ModelWriter_802A1BF4*, int);
extern "C" bool fn_802A1C54(
    ModelWriter_802A1BF4*, int, int, void*);
extern "C" bool fn_802A1E00(ModelWriter_802A1BF4*);

struct WarbleConfiguration
{
    u32 values[4];
    float blobScale;
    float duration;
    float values18[6];
}; // size: 0x30

struct WarbleInstance : public WarbleConfiguration
{
    float elapsed;
    int active;
}; // size: 0x38

struct WarbleOwner
{
    WarbleInstance* instance;
};

struct WarbleState
{
    GLView* view;
    u32 values04[3];
    float blobScale;
    float duration;
    float values18[6];
}; // size: 0x30

static char sWarbleBlobTexture[] = "global/warble_blob";
static char sWarbleTexture[] = "target/warbletexture";
static char sWarbleColourTexture[] = "target/warblecolour";
static char sWarbleTweakCategory[] = "/Rendering/Effects/Warble";
static char sWarbleFrequencyName[] = "gfWarbleFreq";
static char sWarbleRateName[] = "gfWarbleRate";

static int sWarbleOutputExtent = 400;
static float sWarbleDisplacementScale = 128.0f;
static float sWarbleAmplitude = 0.85f;

static WarbleState sWarbleState;
static float sWarbleBlob[64][64];

static float sWarbleLeft;
static float sWarbleTop;
static int sWarbleInputExtent;
bool lbl_806E16D4;
static float sWarblePhase;
static u32 sWarbleColourHandle;
static bool sWarbleColourLoaded;

static GXMaterialFloatTweak_804F4190 sWarbleFrequency(
    sWarbleFrequencyName, sWarbleTweakCategory, 60.0f);
static GXMaterialFloatTweak_804F4190 sWarbleRate(
    sWarbleRateName, sWarbleTweakCategory, 10.0f);

extern "C" void fn_801B369C(WarbleOwner* owner)
{
    owner->instance = 0;
    nlZeroMemory(&sWarbleState, sizeof(sWarbleState));
    sWarbleState.view = fn_8027267C(0x1F);
    sWarbleState.blobScale = 5.0f;
    sWarbleState.duration = 2.0f;
    sWarbleState.values18[0] = 3.0f;
    sWarbleState.values18[1] = 12.0f;
    sWarbleState.values18[2] = 32.0f;
    sWarbleState.values18[3] = 32.0f;
    sWarbleState.values18[4] = 1.0f;
    sWarbleState.values18[5] = 0.0f;
}

static void ClearWarble(WarbleOwner* owner)
{
    if (owner->instance != 0)
    {
        delete owner->instance;
        owner->instance = 0;
    }
}

extern "C" void fn_801B3724(WarbleOwner* owner)
{
    ClearWarble(owner);
}

extern "C" void fn_801B3768(WarbleOwner* owner)
{
    ClearWarble(owner);
}

extern "C" void fn_801B37AC(WarbleOwner* owner, float dt)
{
    WarbleInstance* instance = owner->instance;
    if (instance == 0)
        return;

    if (instance->elapsed >= instance->duration)
    {
        delete instance;
        owner->instance = 0;
        return;
    }

    instance->elapsed += dt;
    if (instance->elapsed >= instance->duration)
        instance->active = true;
}

extern "C" void fn_801B3834()
{
}

extern "C" void fn_801B3838(
    WarbleOwner* owner, WarbleInstance* instance)
{
    owner->instance = instance;
}

extern "C" void fn_801B3840(
    WarbleInstance* instance, const WarbleConfiguration* configuration)
{
    *static_cast<WarbleConfiguration*>(instance) = *configuration;
    instance->elapsed = 0.0f;
    instance->active = false;
}

static int DecodeWarblePaletteValue(u16 colour)
{
    if ((colour & 0x8000) != 0)
    {
        const int first = (colour >> 10) & 0x1F;
        const int second = (colour >> 5) & 0x1F;
        return ((second - first) * 255) / 31;
    }

    const int first = (colour >> 8) & 0xF;
    const int second = (colour >> 4) & 0xF;
    return ((second - first) * 255) / 15;
}

extern "C" void fn_801B38C4()
{
    PlatTexture* texture = glx_GetTex(glGetTexture(sWarbleBlobTexture));
    const u8* indices = static_cast<const u8*>(texture->m_SwizzledData);
    const u16* palette = texture->m_PaletteData;

    for (int y = 0; y < 64; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            const int tile = (y >> 2) * (texture->m_Width >> 3) + (x >> 3);
            const int offset = tile * 32 + (y & 3) * 8 + (x & 7);
            const int value = DecodeWarblePaletteValue(palette[indices[offset]]);
            sWarbleBlob[y][x] = (float)value / 255.0f;
        }
    }

    for (int y = 0; y < 64; ++y)
        for (int x = 0; x < 64; ++x)
            sWarbleBlob[y][x] *= 127.0f;

    sWarbleBlob[63][63] = 0.0f;
}

static int SwizzledIA8Offset(int x, int y)
{
    return (((y >> 2) * 16 + (x >> 2)) * 16 + (y & 3) * 4 + (x & 3)) * 2;
}

extern "C" void fn_801B3B0C(
    float phase, float frequency, float amplitude)
{
    PlatTexture* texture = glx_GetTex(glGetTexture(sWarbleTexture));
    u8* output = static_cast<u8*>(texture->m_SwizzledData);

    for (int y = 0; y < 32; ++y)
    {
        const float dy = 0.5f - (float)y * (1.0f / 64.0f);
        for (int x = 0; x < 32; ++x)
        {
            const float source = sWarbleBlob[y][x];
            int displacement = 124;
            if (source != 0.0f)
            {
                const float dx = 0.5f - (float)x * (1.0f / 64.0f);
                const float radius = nlSqrt(dx * dx + dy * dy, false);
                const float wave = nlSin(
                    (u16)((radius * frequency + phase) * 10430.378f));
                displacement = (int)(128.0f + amplitude * (sWarbleDisplacementScale * (dy / radius) * wave));
            }

            const float normalized = (float)(u8)(int)source / 255.0f;
            const int mapped = (int)((float)sWarbleInputExtent + normalized * (float)(sWarbleOutputExtent - sWarbleInputExtent));
            const int offset = SwizzledIA8Offset(x, y);
            output[offset] = (u8)mapped;
            output[offset + 1] = (u8)displacement;
        }
    }

    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 32; ++x)
        {
            const int source = SwizzledIA8Offset(x, y);
            const int mirrorX = SwizzledIA8Offset(63 - x, y);
            const int mirrorY = SwizzledIA8Offset(x, 63 - y);
            const int mirrorXY = SwizzledIA8Offset(63 - x, 63 - y);
            output[mirrorX] = output[source];
            output[mirrorX + 1] = output[source + 1];
            output[mirrorY] = output[source];
            output[mirrorY + 1] = output[source + 1];
            output[mirrorXY] = output[source];
            output[mirrorXY + 1] = output[source + 1];
        }
    }
}

extern "C" void fn_801B3EF4()
{
    fn_801B3B0C(
        sWarblePhase, sWarbleFrequency.value, sWarbleAmplitude);
}

extern "C" void fn_801B3F0C(float dt)
{
    float phase = sWarblePhase;
    float rate = sWarbleRate.value;
    float scaled = dt * rate;
    sWarblePhase = phase - scaled;
}

static void WriteWarbleVertex(ModelWriter_802A1BF4& writer, int index,
    float x, float y, short u, short v)
{
    writer.colours[index] = 0xFFFFFFFF;
    writer.textureCoordinates[index * 2] = u;
    writer.textureCoordinates[index * 2 + 1] = v;
    writer.positions[index * 3] = x;
    writer.positions[index * 3 + 1] = y;
    writer.positions[index * 3 + 2] = 0.0f;
}

extern "C" void fn_801B3F2C()
{
    if (!sWarbleColourLoaded)
    {
        sWarbleColourHandle = glGetTexture(sWarbleColourTexture);
        sWarbleColourLoaded = true;
    }

    ModelWriter_802A1BF4 writer;
    fn_802A1BF4(&writer);
    glSetDefaultState(false);

    const float left = sWarbleLeft;
    const float top = sWarbleTop;
    const float right = left + glGetOrthographicWidth();
    const float bottom = top + glGetOrthographicHeight();

    if (fn_802A1C54(&writer, 4, 3, 0))
    {
        WriteWarbleVertex(writer, 0, left, top, 0, 0);
        WriteWarbleVertex(writer, 1, left, bottom, 0, 0x400);
        WriteWarbleVertex(writer, 2, right, bottom, 0x400, 0x400);
        WriteWarbleVertex(writer, 3, right, top, 0x400, 0);

        UnidentifiedTextureState* textureState = static_cast<UnidentifiedTextureState*>(
            writer.model->packets->unknown20);
        textureState->texture = sWarbleColourHandle;
        textureState->textureIndex = 0xFFFF;
        textureState->SetWrapS(true);
        textureState->SetWrapT(true);
        textureState->unknown07 = 0;

        if (fn_802A1E00(&writer))
            fn_8027267C(0x20)->AttachModel(writer.model, 0);
    }

    fn_802A1C14(&writer, -1);
}

extern "C" void fn_801B4234()
{
    fn_801B38C4();
}

extern "C" void fn_801B4238()
{
}
