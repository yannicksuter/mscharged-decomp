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
#include "Game/UnidentifiedStaticStorage.h"

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
    const float negativeHalfWidth = -halfWidth;
    const float negativeHalfHeight = -halfHeight;
    nlVector3 topLeft = { 0.0f, 0.0f, 0.0f };
    const float cellZ = topLeft.z;
    const nlVector3& photoCentre = photo->centre;
    const float xStep = (float)((2.0 * halfWidth) / 3.0);
    const float yStep = (float)((2.0 * negativeHalfHeight) / 3.0);

    const float z = photoCentre.z;
    const float bottom = photoCentre.y + negativeHalfHeight;
    const float right = photoCentre.x + halfWidth;
    const float top = photoCentre.y + halfHeight;
    const float left = photoCentre.x + negativeHalfWidth;

    photo->delay = delay;
    nlVec3Set(photo->corners[0], left, bottom, z);
    nlVec3Set(photo->corners[1], right, bottom, z);
    nlVec3Set(photo->corners[2], right, top, z);
    nlVec3Set(photo->corners[3], left, top, z);
    nlVec2Set(*(nlVector2*)&topLeft, photo->centre.x - halfWidth,
        photo->centre.y - negativeHalfHeight);

    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            nlVec3Set(photo->cells[x][y].world[0],
                topLeft.x + xStep * (float)x,
                topLeft.y + yStep * (float)(y + 1), cellZ);
            nlVec3Set(photo->cells[x][y].world[1],
                topLeft.x + xStep * (float)(x + 1),
                topLeft.y + yStep * (float)(y + 1), cellZ);
            nlVec3Set(photo->cells[x][y].world[2],
                topLeft.x + xStep * (float)(x + 1),
                topLeft.y + yStep * (float)y, cellZ);
            nlVec3Set(photo->cells[x][y].world[3],
                topLeft.x + xStep * (float)x,
                topLeft.y + yStep * (float)y, cellZ);
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
    RLView* view = GetLayerView(eCLV_Characters);
    view->m_Target = 8;
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
                RLView* view = GetLayerView(eCLV_Characters);
                view->m_Target = 0;
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
    glModel* model;

    if (photo->state == 0)
    {
        return;
    }

    float alpha;
    if (photo->state == 2)
    {
        float elapsed = photo->fadeTime / gPeachPhotoFadeTime;
        if (elapsed > 1.0f)
        {
            elapsed = 1.0f;
        }
        alpha = 1.0f - elapsed;
    }
    else
    {
        alpha = 1.0f;
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
        int i;
        for (i = 0; i < 4; ++i)
        {
            nlVector3 projected;
            glViewProjectPoint(GetLayerView(eCLV_Characters),
                photo->corners[i],
                projected);
            photo->projectedCorners[i].x = 0.5f * (1.0f + projected.x);
            photo->projectedCorners[i].y = 0.5f * (1.0f + projected.y);
        }

        float oneThird = 1.0f / 3.0f;
        for (i = 0; i < 3; ++i)
        {
            for (int x = 0; x < 3; ++x)
            {
                for (int corner = 0; corner < 4; ++corner)
                {
                    nlVector3 projected;
                    glViewProjectPoint(GetLayerView(eCLV_Characters),
                        photo->cells[i][x].world[corner],
                        projected);
                    photo->cells[i][x].projected[corner].x =
                        0.5f * (1.0f + projected.x);
                    photo->cells[i][x].projected[corner].y =
                        0.5f * (1.0f + projected.y);

                    nlVec2Set(photo->cells[i][x].texture[0],
                        oneThird * (float)i,
                        oneThird * (float)(x + 1));
                    nlVec2Set(photo->cells[i][x].texture[1],
                        oneThird * (float)(i + 1),
                        oneThird * (float)(x + 1));
                    nlVec2Set(photo->cells[i][x].texture[2],
                        oneThird * (float)(i + 1),
                        oneThird * (float)x);
                    nlVec2Set(photo->cells[i][x].texture[3],
                        oneThird * (float)i,
                        oneThird * (float)x);
                }
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
    texture[0] = sPeachPhotoTexcoords[0];
    texture[1] = sPeachPhotoTexcoords[1];
    texture[2] = sPeachPhotoTexcoords[2];
    texture[3] = sPeachPhotoTexcoords[3];

    if (writer.Begin(4, 3, 0))
    {
        for (int i = 0; i < 4; ++i)
        {
            writer.Texcoord0(photo->projectedCorners[i].x,
                photo->projectedCorners[i].y);

            const nlVector2& tex = texture[(i + gPeachPhotoTextureRotation) % 4];
            writer.Texcoord1(tex.x, tex.y);
            writer.Texcoord2(tex.x, tex.y);
            writer.Colour(colour);

            nlVector3 position = photo->corners[i];
            position.z += gPeachPhotoDepthOffset;
            writer.Vertex(position);
        }

        if (writer.End())
        {
            writer.Texture(0, glGetTexture(
                gPeachPhotoDisableImage ? sPeachPhotoWhiteTexture : sPeachPhotoTexture));

            writer.Texture(1, glGetTexture(
                gPeachPhotoDisableMasks ? sPeachPhotoWhiteTexture : sPeachPhotoMaskTexture1));

            writer.Texture(2, glGetTexture(
                gPeachPhotoDisableMasks ? sPeachPhotoWhiteTexture : sPeachPhotoMaskTexture2));

            model = writer.GetModel();
            for (glModelPacket* packet = model->packets;
                 packet < model->packets + model->numPackets;
                 ++packet)
            {
                glSetRasterState(
                    packet->rasterState, GLS_AlphaTest, 1);
                glSetRasterState(
                    packet->rasterState, GLS_AlphaTestRef, 0);
            }

            GetLayerView(eCLV_PeachPhoto3D)->AttachModel(
                writer.GetModel(), 0);
        }
    }
}
