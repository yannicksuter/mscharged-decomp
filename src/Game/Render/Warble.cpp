#include "Game/TweakValueFloat.h"
#include "Game/GL/GLWarbleMeshWriter.h"
#include "Game/Render/RLView.h"
#include "NL/gl/gl.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "Game/TweakValue.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include "types.h"

#include "Game/Render/Warble.h"


static char sWarbleBlobTexture[] = "global/warble_blob";
static char sWarbleTexture[] = "target/warbletexture";
static char sWarbleColourTexture[] = "target/warblecolour";
static char sWarbleTweakCategory[] = "/Rendering/Effects/Warble";
static char sWarbleFrequencyName[] = "gfWarbleFreq";
static char sWarbleRateName[] = "gfWarbleRate";

static int sWarbleOutputExtent = 400;
static float sWarbleDisplacementScale = 128.0f;
static float sWarbleAmplitude = 0.85f;

static WarbleConfiguration sWarbleConfiguration;
static float sWarbleBlob[64][64];

static float sWarbleLeft;
static float sWarbleTop;
static int sWarbleInputExtent;
bool gWarbleEnabled;
static float sWarblePhase;
static u32 sWarbleColourHandle;
static char sWarbleColourLoaded;

static TweakValueFloat sWarbleFrequency(
    sWarbleFrequencyName, sWarbleTweakCategory, 60.0f);
static TweakValueFloat sWarbleRate(
    sWarbleRateName, sWarbleTweakCategory, 10.0f);

void InitializeWarble(WarbleOwner* owner)
{
    owner->instance = 0;
    nlZeroMemory(&sWarbleConfiguration, sizeof(sWarbleConfiguration));
    sWarbleConfiguration.view = GetLayerView(eCLV_Warble);
    sWarbleConfiguration.blobScale = 5.0f;
    sWarbleConfiguration.duration = 2.0f;
    sWarbleConfiguration.values18[0] = 3.0f;
    sWarbleConfiguration.values18[1] = 12.0f;
    sWarbleConfiguration.values18[2] = 32.0f;
    sWarbleConfiguration.values18[3] = 32.0f;
    sWarbleConfiguration.values18[4] = 1.0f;
    sWarbleConfiguration.values18[5] = 0.0f;
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

    bool expired = instance->UnidentifiedQuery();
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

static inline u8 ReadWarbleBlobValue(
    const PlatTexture* texture, int x, int y)
{
    int block =
        (y >> 2) * (texture->m_Width >> 3) + (x >> 3);
    const u8 (*tiles)[4][8] =
        static_cast<const u8 (*)[4][8]>(texture->m_SwizzledData);
    const u8 paletteIndex =
        tiles[block][y & 3][x & 7];
    const u16 colour = texture->m_PaletteData[paletteIndex];
    if ((colour & 0x8000) != 0)
    {
        const unsigned int value = (colour >> 10) & 0x1F;
        return (value * 255) / 31;
    }

    const unsigned int value = (colour >> 8) & 0xF;
    return (value * 255) / 15;
}

void LoadWarbleBlob()
{
    PlatTexture* texture = glx_GetTex(glGetTexture(sWarbleBlobTexture));
    int x;
    int y;

    for (y = 0; y < 64; ++y)
    {
        for (x = 0; x < 64; ++x)
        {
            const u8 value = ReadWarbleBlobValue(texture, x, y);
            sWarbleBlob[(unsigned int)y][(unsigned int)x] =
                (float)value / 255.0f;
        }
    }

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

void GenerateWarbleTexture(
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

void UpdateWarbleTexture(bool*)
{
    GenerateWarbleTexture(
        sWarblePhase, sWarbleFrequency.value, sWarbleAmplitude);
}

void UpdateWarblePhase(bool*, float dt)
{
    float phase = sWarblePhase;
    float rate = sWarbleRate.value;
    float scaled = dt * rate;
    sWarblePhase = phase - scaled;
}

static inline void WriteWarbleVertex(GLWarbleMeshWriter& writer,
    float x, float y, short u, short v)
{
    nlColour colour;
    nlColourSet(colour, 0xFF, 0xFF, 0xFF, 0xFF);
    writer.Colour(colour);
    writer.Texcoord(u, v);
    writer.Position(x, y, 0.0f);
}

void RenderWarbleQuad(bool*)
{
    if (!sWarbleColourLoaded)
    {
        sWarbleColourHandle = glGetTexture(sWarbleColourTexture);
        sWarbleColourLoaded = true;
    }

    GLWarbleMeshWriter writer;
    glSetDefaultState(false);

    const float left = sWarbleLeft;
    const float top = sWarbleTop;
    const float right = sWarbleLeft + glGetOrthographicWidth();
    const float bottom = sWarbleTop + glGetOrthographicHeight();

    if (writer.Begin(4, 3, 0))
    {
        WriteWarbleVertex(writer, left, top, 0, 0);
        WriteWarbleVertex(writer, left, bottom, 0, 0x400);
        const u32 colourHandle = sWarbleColourHandle;
        WriteWarbleVertex(writer, right, bottom, 0x400, 0x400);
        WriteWarbleVertex(writer, right, top, 0x400, 0);

        glTextureBinding* textureState = static_cast<glTextureBinding*>(
            writer.model->packets->materialParameters);
        textureState->texture = colourHandle;
        textureState->textureIndex = 0xFFFF;
        textureState->SetWrapS(true);
        textureState->SetWrapT(true);
        textureState->unknown07 = 0;

        if (writer.End())
            GetLayerView(eCLV_WarbleBlend)->AttachModel(writer.model, 0);
    }
}

void InitializeWarbleRendering(bool*)
{
    LoadWarbleBlob();
}

void ShutdownWarbleRendering(bool*)
{
}
