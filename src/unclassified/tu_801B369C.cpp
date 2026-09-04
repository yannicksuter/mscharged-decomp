#include "Game/GL/ModelWriter_802A1BF4.h"
#include "Game/Render/RLView.h"
#include "NL/gl/gl.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include "types.h"

#include "unclassified/tu_801B369C.h"


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
static char sWarbleColourLoaded;

static GXMaterialFloatTweak_804F4190 sWarbleFrequency(
    sWarbleFrequencyName, sWarbleTweakCategory, 60.0f);
static GXMaterialFloatTweak_804F4190 sWarbleRate(
    sWarbleRateName, sWarbleTweakCategory, 10.0f);

void InitializeWarble(WarbleOwner* owner)
{
    owner->instance = 0;
    nlZeroMemory(&sWarbleState, sizeof(sWarbleState));
    sWarbleState.view = GetLayerView(eCLV_Warble);
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

void ShutdownWarble(WarbleOwner* owner)
{
    ClearWarble(owner);
}

void ResetWarble(WarbleOwner* owner)
{
    ClearWarble(owner);
}

void UpdateWarble(WarbleOwner* owner, float dt)
{
    WarbleInstance* instance = owner->instance;
    if (instance == 0)
        return;

    bool expired = instance->elapsed >= instance->duration;
    if (expired)
    {
        delete instance;
        owner->instance = 0;
        return;
    }

    instance->elapsed += dt;
    if (instance->elapsed >= instance->duration)
        instance->active = true;
}

void RenderWarble(WarbleOwner* owner)
{
}

void SetWarbleInstance(
    WarbleOwner* owner, WarbleInstance* instance)
{
    owner->instance = instance;
}

WarbleInstance::WarbleInstance(const WarbleConfiguration& configuration)
{
    *static_cast<WarbleConfiguration*>(this) = configuration;
    elapsed = 0.0f;
    active = false;
}

static inline u8 DecodeWarblePaletteValue(u16 colour)
{
    if ((colour & 0x8000) != 0)
    {
        const unsigned int value = (colour >> 10) & 0x1F;
        return (value * 255) / 31;
    }

    const unsigned int value = (colour >> 8) & 0xF;
    return (value * 255) / 15;
}

extern "C" void fn_801B38C4()
{
    PlatTexture* texture = glx_GetTex(glGetTexture(sWarbleBlobTexture));

    for (int y = 0; y < 64; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            const int tile = (y >> 2) * (texture->m_Width >> 3) + (x >> 3);
            const int offset = tile * 32 + (y & 3) * 8 + (x & 7);
            const u8 value = DecodeWarblePaletteValue(
                texture->m_PaletteData[static_cast<const u8*>(texture->m_SwizzledData)[offset]]);
            sWarbleBlob[y][x] = (float)value / 255.0f;
        }
    }

    int y;
    int x;
    for (y = 0; y < 64; ++y)
        for (x = 0; x < 64; ++x)
            sWarbleBlob[y][x] *= 127.0f;

    sWarbleBlob[y >> 1][x >> 1] = 0.0f;
}

static inline int SwizzledIA8Offset(int x, int y)
{
    const int yOffset = ((y << 6) & ~0xFF) | ((y & 3) << 2);
    const int xOffset = ((x << 2) & ~0xF) | (x & 3);
    return (yOffset | xOffset) << 1;
}

extern "C" void fn_801B3B0C(
    float phase, float frequency, float amplitude)
{
    PlatTexture* texture = glx_GetTex(glGetTexture(sWarbleTexture));
    u8* output = static_cast<u8*>(texture->m_SwizzledData);

    for (int y = 0; y < 32; ++y)
    {
        const float dy = (float)y * (1.0f / 64.0f) - 0.5f;
        for (int x = 0; x < 32; ++x)
        {
            const float source = sWarbleBlob[y][x];
            int displacement;
            if (source == 0.0f)
            {
                displacement = 124;
            }
            else
            {
                const float dx = (float)x * (1.0f / 64.0f) - 0.5f;
                const float radius = nlSqrt(dx * dx + dy * dy, false);
                const float inverseRadius = 1.0f / radius;
                const float radialY = dy * inverseRadius;
                const float wave = nlSin(
                    (u16)((radius * frequency + phase) * 10430.378f));
                float scaledWave = radialY * wave;
                scaledWave = sWarbleDisplacementScale * scaledWave;
                displacement = (int)(amplitude * scaledWave + 128.0f);
            }

            const int offset = SwizzledIA8Offset(x, y);
            const float normalized = (float)(u8)(int)source / 255.0f;
            const int mapped = (int)((float)sWarbleInputExtent + normalized * (float)(sWarbleOutputExtent - sWarbleInputExtent));
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

static inline void WriteWarbleVertex(ModelWriter_802A1BF4& writer,
    float x, float y, short u, short v)
{
    nlColour colour;
    nlColourSet(colour, 0xFF, 0xFF, 0xFF, 0xFF);
    writer.Colour(colour);
    writer.Texcoord(u, v);
    writer.Vertex(x, y, 0.0f);
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
    const float right = sWarbleLeft + glGetOrthographicWidth();
    const float bottom = sWarbleTop + glGetOrthographicHeight();

    if (fn_802A1C54(&writer, 4, 3, 0))
    {
        WriteWarbleVertex(writer, left, top, 0, 0);
        WriteWarbleVertex(writer, left, bottom, 0, 0x400);
        const u32 colourHandle = sWarbleColourHandle;
        WriteWarbleVertex(writer, right, bottom, 0x400, 0x400);
        WriteWarbleVertex(writer, right, top, 0x400, 0);

        UnidentifiedTextureState* textureState = static_cast<UnidentifiedTextureState*>(
            writer.model->packets->unknown20);
        textureState->texture = colourHandle;
        textureState->textureIndex = 0xFFFF;
        textureState->SetWrapS(true);
        textureState->SetWrapT(true);
        textureState->unknown07 = 0;

        if (fn_802A1E00(&writer))
            GetLayerView(eCLV_WarbleBlend)->AttachModel(writer.model, 0);
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
