#include "Game/Debug/ShapeRender.h"
#include "Game/Render/RLView.h"
#include "NL/gl/gl.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "unclassified/tu_802A15D4.h"

#include <string.h>

struct LoadFrame
{
    int type;
    char _004[4];
    u8* position;
};

struct SaveFrame
{
    int type;
    char _004[4];
    u8* position;
};

struct UnidentifiedReplayState_801AD15C
{
    int value;
    u8 _004[0x9C];
};

struct UnidentifiedPeachPhotoCell
{
    nlVector3 world[4];
    nlVector2 projected[4];
    nlVector2 texture[4];
};

struct UnidentifiedPeachPhotoState
{
    int state;
    nlVector3 centre;
    nlVector3 corners[4];
    float displacement;
    bool firstFrameSeen;
    bool textureReady;
    u8 _046[2];
    unsigned int lastFrame;
    bool projected;
    u8 _04D[3];
    nlVector2 projectedCorners[4];
    float fadeTime;
    float delay;
    UnidentifiedPeachPhotoCell cells[3][3];
};

extern "C"
{
    UnidentifiedReplayState_801AD15C lbl_80573C08;
    UnidentifiedPeachPhotoState lbl_80573CA8;

    char lbl_80514430[] = "target/grayscale";
    char lbl_80514444[] = "global/white";
    char lbl_80514454[] = "global/peach_mask1";
    char lbl_80514468[] = "global/peach_mask2";

    int lbl_806DD010 = 0xA0;
    float lbl_806DD014 = 5.0f;
    float lbl_806DD018 = 0.3f;
    int lbl_806DD01C = 0x30;
    int lbl_806DD020 = 1;
    bool lbl_806DD024 = true;
    float lbl_806DD028 = 0.005f;

    unsigned int lbl_806E1618;
    bool lbl_806E161C;
    bool lbl_806E161D;
    bool lbl_806E161E;

    extern const nlVector2 lbl_804DCEE8[4];

    GLView* fn_8027267C(int index);
    void fn_802CE528(
        GLView* view, const nlVector3* world, nlVector3* projected);
}

extern "C" void fn_801AD15C(
    UnidentifiedReplayState_801AD15C*, void*)
{
}

extern "C" void fn_801AD160(UnidentifiedReplayState_801AD15C*)
{
}

extern "C" void fn_801AD164(
    UnidentifiedReplayState_801AD15C*, LoadFrame* frame)
{
    int value = 0;
    if (frame->type == 1)
    {
        memcpy(&value, frame->position, sizeof(value));
        frame->position += sizeof(value);
    }
}

extern "C" void fn_801AD1C0(
    UnidentifiedReplayState_801AD15C* replay, SaveFrame* frame)
{
    int value = replay->value;
    if (frame->type == 1)
    {
        memcpy(frame->position, &value, sizeof(value));
        frame->position += sizeof(value);
    }
}

extern "C" void fn_801AD21C(
    UnidentifiedReplayState_801AD15C*, float)
{
}

extern "C" void fn_801AD220(UnidentifiedPeachPhotoState* photo,
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
            UnidentifiedPeachPhotoCell& cell = photo->cells[y][x];
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

    photo->displacement = nlRandomf(2.0f * lbl_806DD014)
                        - lbl_806DD014;
    photo->state = 1;
    photo->firstFrameSeen = false;
    photo->textureReady = false;
    photo->lastFrame = glGetCurrentFrame();
    photo->projected = false;
    photo->fadeTime = 0.0f;
    photo->delay = delay;
    fn_8027267C(eCLV_Characters)->m_Target = 8;
}

extern "C" void fn_801AD484(
    UnidentifiedPeachPhotoState* photo, bool immediate)
{
    if (photo->state != 0)
    {
        if (immediate)
        {
            photo->state = 0;
            fn_8027267C(eCLV_Characters)->m_Target = 0;
        }
        else
        {
            photo->state = 2;
            photo->fadeTime = 0.0f;
        }
    }
}

extern "C" void fn_801AD4E4(
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

extern "C" void fn_801AD7E4(
    UnidentifiedPeachPhotoState* photo, float dt, int)
{
    if (photo->state == 1)
    {
        const unsigned long texture = glGetTexture(lbl_80514430);
        const unsigned short border =
            (unsigned short)((lbl_806E1618 << 8) | lbl_806DD01C);
        fn_801AD4E4(border, texture);

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
                fn_8027267C(eCLV_Characters)->m_Target = 0;
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
            if (photo->fadeTime > lbl_806DD018)
            {
                photo->state = 0;
            }
        }
    }
}

extern "C" void fn_801AD914(UnidentifiedPeachPhotoState* photo)
{
    if (photo->state == 0)
    {
        return;
    }

    float alpha = 1.0f;
    if (photo->state == 2)
    {
        float elapsed = photo->fadeTime / lbl_806DD018;
        if (elapsed > 1.0f)
        {
            elapsed = 1.0f;
        }
        alpha = 1.0f - elapsed;
    }

    if (lbl_806E161C)
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

    glSetDefaultState(lbl_806DD024);
    glSetCurrentTexture(glGetTexture(lbl_80514430), GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetCurrentRasterState(glHandleizeRasterState());

    if (!photo->projected)
    {
        for (int i = 0; i < 4; ++i)
        {
            nlVector3 projected;
            fn_802CE528(fn_8027267C(eCLV_Characters),
                &photo->corners[i], &projected);
            photo->projectedCorners[i].x = 0.5f * (1.0f + projected.x);
            photo->projectedCorners[i].y = 0.5f * (1.0f + projected.y);
        }

        for (int y = 0; y < 3; ++y)
        {
            for (int x = 0; x < 3; ++x)
            {
                UnidentifiedPeachPhotoCell& cell = photo->cells[y][x];
                for (int i = 0; i < 4; ++i)
                {
                    nlVector3 projected;
                    fn_802CE528(fn_8027267C(eCLV_Characters),
                        &cell.world[i], &projected);
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
        (unsigned char)(alpha * (float)lbl_806DD010)
    } };

    State_802A15D4 writer;
    fn_802A15D4(&writer);
    nlVector2 texture[4];
    memcpy(texture, lbl_804DCEE8, sizeof(texture));

    if (fn_802A163C(&writer, 4, 3, 0))
    {
        for (int i = 0; i < 4; ++i)
        {
            *writer.value_10++ = photo->projectedCorners[i].x;
            *writer.value_10++ = photo->projectedCorners[i].y;

            const nlVector2& tex = texture[(i + lbl_806DD020) % 4];
            *writer.value_14++ = tex.x;
            *writer.value_14++ = tex.y;
            *writer.value_18++ = tex.x;
            *writer.value_18++ = tex.y;
            *writer.value_1C++ = *(u32*)&colour;

            *writer.value_0C++ = photo->corners[i].x;
            *writer.value_0C++ = photo->corners[i].y;
            *writer.value_0C++ = photo->corners[i].z + lbl_806DD028;
        }

        if (fn_802A1878(&writer))
        {
            UnidentifiedTextureState* states =
                static_cast<UnidentifiedTextureState*>(
                    writer.model->packets->unknown20);
            states[0].texture = glGetTexture(
                lbl_806E161D ? lbl_80514444 : lbl_80514430);
            states[0].textureIndex = 0xFFFF;
            states[0].SetWrapS(true);
            states[0].SetWrapT(true);
            states[0].unknown07 = 0;

            states[1].texture = glGetTexture(
                lbl_806E161E ? lbl_80514444 : lbl_80514454);
            states[1].textureIndex = 0xFFFF;
            states[1].SetWrapS(true);
            states[1].SetWrapT(true);
            states[1].unknown07 = 0;

            states[2].texture = glGetTexture(
                lbl_806E161E ? lbl_80514444 : lbl_80514468);
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

            fn_8027267C(eCLV_PeachPhoto3D)->AttachModel(
                writer.model, 0);
        }
    }

    fn_802A15FC(&writer, -1);
}
