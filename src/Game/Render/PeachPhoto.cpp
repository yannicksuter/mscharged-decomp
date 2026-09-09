#include "Game/Debug/ShapeRender.h"
#include "Game/Replay.h"
#include "Game/Render/RLView.h"
#include "NL/gl/gl.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "Game/Render/CrowdManager.h"
#include "Game/Render/PeachPhoto.h"
#include "NL/gl/glMultiTextureModelWriter.h"

#include <string.h>

char sPeachPhotoTexture[] = "target/grayscale";
char sPeachPhotoWhiteTexture[] = "global/white";
char sPeachPhotoMaskTexture1[] = "global/peach_mask1";
char sPeachPhotoMaskTexture2[] = "global/peach_mask2";

int gPeachPhotoAlpha = 0xA0;
float gPeachPhotoDisplacementRange = 5.0f;
float gPeachPhotoFadeTime = 0.3f;
int gPeachPhotoBorderAlpha = 0x30;
int gPeachPhotoTextureRotation = 1;
bool gPeachPhotoResetState = true;
float gPeachPhotoDepthOffset = 0.005f;

unsigned int gPeachPhotoBorderIntensity;
bool gPeachPhotoDebugBounds;
bool gPeachPhotoDisableImage;
bool gPeachPhotoDisableMasks;

extern const nlVector2 sPeachPhotoTexcoords[4];

CrowdManager CrowdManager::instance;
PeachPhotoState gPeachPhotoState;

void CrowdManager::Initialize(void*)
{
}

void CrowdManager::Uninitialize()
{
}

void CrowdManager::Replay(LoadFrame& frame)
{
    int replayState = 0;
    Replayable<1, LoadFrame, int>(frame, replayState);
}

void CrowdManager::Replay(SaveFrame& frame)
{
    int state = m_State;
    Replayable<1, SaveFrame, int>(frame, state);
}

void CrowdManager::Update(float)
{
}

void StartPeachPhoto(PeachPhotoState* photo,
    const nlVector3* centre, float delay, float halfWidth,
    float halfHeight)
{
    photo->centre = *centre;

    const float left = photo->centre.x - halfWidth;
    const float right = photo->centre.x + halfWidth;
    const float top = photo->centre.y + halfHeight;
    const float bottom = photo->centre.y - halfHeight;
    const float z = photo->centre.z;

    nlVec3Set(photo->corners[0], left, bottom, z);
    nlVec3Set(photo->corners[1], right, bottom, z);
    nlVec3Set(photo->corners[2], right, top, z);
    nlVec3Set(photo->corners[3], left, top, z);

    const float xStep = (float)((2.0 * halfWidth) / 3.0);
    const float yStep = (float)((-2.0 * halfHeight) / 3.0);
    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            PeachPhotoCell& cell = photo->cells[y][x];
            const float x0 = left + xStep * (float)x;
            const float x1 = left + xStep * (float)(x + 1);
            const float y0 = top + yStep * (float)y;
            const float y1 = top + yStep * (float)(y + 1);
            nlVec3Set(cell.world[0], x0, y0, 0.0f);
            nlVec3Set(cell.world[1], x1, y0, 0.0f);
            nlVec3Set(cell.world[2], x1, y1, 0.0f);
            nlVec3Set(cell.world[3], x0, y1, 0.0f);
        }
    }

    photo->displacement = nlRandomf(2.0f * gPeachPhotoDisplacementRange)
                        - gPeachPhotoDisplacementRange;
    photo->state = 1;
    photo->firstFrameSeen = false;
    photo->textureReady = false;
    photo->lastFrame = glGetCurrentFrame();
    photo->projected = false;
    photo->fadeTime = 0.0f;
    photo->delay = delay;
    GetLayerView(eCLV_Characters)->m_Target = 8;
}

void EndPeachPhoto(PeachPhotoState* photo, bool immediate)
{
    if (photo->state != 0)
    {
        if (immediate)
        {
            photo->state = 0;
            GetLayerView(eCLV_Characters)->m_Target = 0;
        }
        else
        {
            photo->state = 2;
            photo->fadeTime = 0.0f;
        }
    }
}

void SetPeachPhotoTextureBorder(
    unsigned short colour, unsigned long textureHandle)
{
    PlatTexture* texture = glx_GetTex(textureHandle);
    unsigned short* data =
        static_cast<unsigned short*>(texture->m_SwizzledData);
    const int width = texture->m_Width;
    const int height = texture->m_Height;

    for (int x = 0; x < width; ++x)
    {
        data[(x / 4) * 16 + (x & 3)] = colour;
        const int y = height - 1;
        data[((y / 4) * (width >> 2) + (x / 4)) * 16
            + ((y & 3) << 2) + (x & 3)] = colour;
    }

    for (int y = 0; y < height; ++y)
    {
        data[((y / 4) * (width >> 2)) * 16
            + ((y & 3) << 2)] = colour;
        const int x = width - 1;
        data[((y / 4) * (width >> 2) + (x / 4)) * 16
            + ((y & 3) << 2) + (x & 3)] = colour;
    }
}

void UpdatePeachPhoto(
    PeachPhotoState* photo, float dt, int)
{
    if (photo->state == 1)
    {
        const unsigned long texture = glGetTexture(sPeachPhotoTexture);
        const unsigned short border =
            (unsigned short)((gPeachPhotoBorderIntensity << 8) | gPeachPhotoBorderAlpha);
        SetPeachPhotoTextureBorder(border, texture);

        photo->delay -= dt;
        if (photo->delay < 0.0f)
        {
            photo->delay = 0.0f;
            if (photo->state != 0)
            {
                photo->state = 2;
                photo->fadeTime = 0.0f;
            }
        }
    }

    if (photo->state != 0)
    {
        const unsigned int currentFrame = glGetCurrentFrame();
        if (currentFrame != photo->lastFrame)
        {
            if (photo->firstFrameSeen || photo->textureReady)
            {
                photo->lastFrame = currentFrame;
                photo->textureReady = true;
                GetLayerView(eCLV_Characters)->m_Target = 0;
            }
            else
            {
                photo->lastFrame = currentFrame;
                photo->firstFrameSeen = true;
            }
        }

        if (photo->state == 2)
        {
            photo->fadeTime += dt;
            if (photo->fadeTime > gPeachPhotoFadeTime)
            {
                photo->state = 0;
            }
        }
    }
}

void RenderPeachPhoto(PeachPhotoState* photo)
{
    if (photo->state == 0)
    {
        return;
    }

    float alpha = 1.0f;
    if (photo->state == 2)
    {
        float elapsed = photo->fadeTime / gPeachPhotoFadeTime;
        if (elapsed > 1.0f)
        {
            elapsed = 1.0f;
        }
        alpha = 1.0f - elapsed;
    }

    if (gPeachPhotoDebugBounds)
    {
        nlColour red = { { 0xFF, 0, 0, 0xFF } };
        g_ShapeRenderer.DrawLine3D(
            photo->corners[0], photo->corners[1], red, true);
        g_ShapeRenderer.DrawLine3D(
            photo->corners[1], photo->corners[2], red, true);
        g_ShapeRenderer.DrawLine3D(
            photo->corners[2], photo->corners[3], red, true);
        g_ShapeRenderer.DrawLine3D(
            photo->corners[3], photo->corners[0], red, true);
    }

    glSetDefaultState(gPeachPhotoResetState);
    glSetCurrentTexture(glGetTexture(sPeachPhotoTexture), GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetCurrentRasterState(glHandleizeRasterState());

    if (!photo->projected)
    {
        for (int i = 0; i < 4; ++i)
        {
            nlVector3 projected;
            glViewProjectPoint(GetLayerView(eCLV_Characters),
                photo->corners[i],
                projected);
            photo->projectedCorners[i].x = 0.5f * (1.0f + projected.x);
            photo->projectedCorners[i].y = 0.5f * (1.0f + projected.y);
        }

        for (int y = 0; y < 3; ++y)
        {
            for (int x = 0; x < 3; ++x)
            {
                PeachPhotoCell& cell = photo->cells[y][x];
                for (int i = 0; i < 4; ++i)
                {
                    nlVector3 projected;
                    glViewProjectPoint(GetLayerView(eCLV_Characters),
                        cell.world[i],
                        projected);
                    cell.projected[i].x = 0.5f * (1.0f + projected.x);
                    cell.projected[i].y = 0.5f * (1.0f + projected.y);
                }

                const float x0 = (1.0f / 3.0f) * (float)y;
                const float x1 = (1.0f / 3.0f) * (float)(y + 1);
                const float y0 = (1.0f / 3.0f) * (float)x;
                const float y1 = (1.0f / 3.0f) * (float)(x + 1);
                nlVec2Set(cell.texture[0], x0, y1);
                nlVec2Set(cell.texture[1], x1, y1);
                nlVec2Set(cell.texture[2], x1, y0);
                nlVec2Set(cell.texture[3], x0, y0);
            }
        }
        photo->projected = true;
    }

    nlColour colour = { {
        0xFF, 0xFF, 0xFF,
        (unsigned char)(alpha * (float)gPeachPhotoAlpha)
    } };

    glMultiTextureModelWriter writer;
    nlVector2 texture[4];
    memcpy(texture, sPeachPhotoTexcoords, sizeof(texture));

    if (writer.Begin(4, 3, 0))
    {
        for (int i = 0; i < 4; ++i)
        {
            *writer.texcoords0++ = photo->projectedCorners[i].x;
            *writer.texcoords0++ = photo->projectedCorners[i].y;

            const nlVector2& tex = texture[(i + gPeachPhotoTextureRotation) % 4];
            *writer.texcoords1++ = tex.x;
            *writer.texcoords1++ = tex.y;
            *writer.texcoords2++ = tex.x;
            *writer.texcoords2++ = tex.y;
            *writer.colours++ = *(u32*)&colour;

            *writer.positions++ = photo->corners[i].x;
            *writer.positions++ = photo->corners[i].y;
            *writer.positions++ = photo->corners[i].z + gPeachPhotoDepthOffset;
        }

        if (writer.End())
        {
            glTextureBinding* states =
                static_cast<glTextureBinding*>(
                    writer.model->packets->unknown20);
            states[0].texture = glGetTexture(
                gPeachPhotoDisableImage ? sPeachPhotoWhiteTexture : sPeachPhotoTexture);
            states[0].textureIndex = 0xFFFF;
            states[0].SetWrapS(true);
            states[0].SetWrapT(true);
            states[0].unknown07 = 0;

            states[1].texture = glGetTexture(
                gPeachPhotoDisableMasks ? sPeachPhotoWhiteTexture : sPeachPhotoMaskTexture1);
            states[1].textureIndex = 0xFFFF;
            states[1].SetWrapS(true);
            states[1].SetWrapT(true);
            states[1].unknown07 = 0;

            states[2].texture = glGetTexture(
                gPeachPhotoDisableMasks ? sPeachPhotoWhiteTexture : sPeachPhotoMaskTexture2);
            states[2].textureIndex = 0xFFFF;
            states[2].SetWrapS(true);
            states[2].SetWrapT(true);
            states[2].unknown07 = 0;

            for (glModelPacket* packet = writer.model->packets;
                 packet < writer.model->packets
                        + writer.model->numPackets;
                 ++packet)
            {
                glSetRasterState(
                    packet->rasterState, GLS_AlphaTest, 1);
                glSetRasterState(
                    packet->rasterState, GLS_AlphaTestRef, 0);
            }

            GetLayerView(eCLV_PeachPhoto3D)->AttachModel(
                writer.model, 0);
        }
    }
}
