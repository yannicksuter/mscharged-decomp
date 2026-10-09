#include "NL/gl/glDraw3.h"
#include "NL/gl/glTexture.h"

#include "Game/GL/GLTexturedColourMeshWriter.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/gl/glTexture.h"
#include "Game/SharedStaticStorage.h"


bool glAttachQuad3(eGLView view, unsigned long count, glQuad3* quads)
{
    int i = 0;
    glQuad3* quad = quads;
    for (; i < count; i++, quad++)
    {
        ((GLView*)view)->AttachModel(quad->GetModel(), 0);
    }
    return true;
}

bool glAttachQuad3(
    eGLView view, int stream, unsigned long count, glQuad3* quads)
{
    glQuad3* quad = quads;
    int i = 0;
    for (; i < count; i++, quad++)
    {
        ((GLView*)view)->AttachModel(quad->GetModel(), stream);
    }
    return true;
}

const glModel* glQuad3::GetModel() const
{
    glModel* newModel = 0;
    GLTexturedColourMeshWriter writer;
    unsigned long texconfig = gl_GetCurrentStateBundle()->texconfig;

    if (glHasQuads())
    {
        if (!writer.Begin(4, GLP_QuadList, 0))
        {
            return 0;
        }

        for (int i = 0; i < 4; ++i)
        {
            writer.Colour(m_colour[i]);
            writer.Texcoord(m_uv[i]);
            writer.Vertex(m_pos[i]);
        }
    }
    else
    {
        if (!writer.Begin(4, GLP_TriStrip, 0))
        {
            return 0;
        }

        static int stripmap[4] = { 3, 0, 2, 1 };
        for (int i = 0; i < 4; ++i)
        {
            int srcIndex = stripmap[i];
            writer.Colour(m_colour[srcIndex]);
            writer.Texcoord(m_uv[srcIndex]);
            writer.Vertex(m_pos[srcIndex]);
        }
    }

    if (texconfig == 0)
    {
        glTextureBinding* state =
            (glTextureBinding*)writer.model->packets->materialParameters;
        state->texture = gWhiteTextureID;
        state->textureIndex = 0xFFFF;
        state->SetWrapS(true);
        state->SetWrapT(true);
        state->unknown07 = 0;
    }
    else
    {
        unsigned long textureState = glGetTextureState(GLTS_DiffuseWrap);
        unsigned char wrapS = true;
        unsigned char wrapT = true;
        if (textureState == 0)
        {
            wrapS = false;
            wrapT = false;
        }
        if (textureState == 1)
        {
            wrapS = false;
        }
        if (textureState == 2)
        {
            wrapT = false;
        }

        u32 texture = glGetCurrentTexture(GLTT_Diffuse);
        glTextureBinding* state =
            (glTextureBinding*)writer.model->packets->materialParameters;
        state->texture = texture;
        state->textureIndex = 0xFFFF;
        state->SetWrapS(wrapS);
        state->SetWrapT(wrapT);
        state->unknown07 = 0;
    }

    if (writer.End())
    {
        newModel = writer.GetModel();
    }
    return newModel;
}

bool glQuad3::Attach(eGLView view, int stream)
{
    ((GLView*)view)->AttachModel(GetModel(), stream);
    return true;
}

void glQuad3::SetupRotatedRectangle(
    float w, float h, const nlMatrix4& mtx, bool flipU, bool flipV)
{
    SetupRotatedRectangle(w, h, h, mtx, flipU, flipV);
}

void glQuad3::SetupRotatedRectangle(float w, float h0, float h1,
    const nlMatrix4& mtx, bool flipU, bool flipV)
{
    nlVector3 corner;
    {
        float half = 0.5f;
        float bottom;
        float left;
        left = -w * half;
        bottom = -h0 * half;
        nlVec3Set(corner, left, bottom, 0.0f);
    }
    nlMultPosVectorMatrix(corner, mtx);
    m_pos[0] = corner;

    {
        float right = w;
        float bottom = -h1;
        float half = 0.5f;
        nlVec3Set(corner, right * half, bottom * half, 0.0f);
    }
    nlMultPosVectorMatrix(corner, mtx);
    m_pos[1] = corner;

    {
        float half = 0.5f;
        float right = w;
        float top = h1;
        nlVec3Set(corner, right * half, top * half, 0.0f);
    }
    nlMultPosVectorMatrix(corner, mtx);
    m_pos[2] = corner;

    {
        float half = 0.5f;
        float left = -w;
        float top = h0;
        nlVec3Set(corner, left * half, top * half, 0.0f);
    }
    nlMultPosVectorMatrix(corner, mtx);
    m_pos[3] = corner;

    float leftU = flipU ? 1.0f : 0.0f;
    float rightU = flipU ? 0.0f : 1.0f;
    float bottomV = flipV ? 1.0f : 0.0f;
    float topV = flipV ? 0.0f : 1.0f;

    m_uv[0].x = leftU;
    m_uv[0].y = bottomV;
    m_uv[1].x = rightU;
    m_uv[1].y = bottomV;
    m_uv[2].x = rightU;
    m_uv[2].y = topV;
    m_uv[3].x = leftU;
    m_uv[3].y = topV;

    m_colour[0].c[0] = 0xFF;
    m_colour[0].c[1] = 0xFF;
    m_colour[0].c[2] = 0xFF;
    m_colour[0].c[3] = 0xFF;
    m_colour[1].c[0] = 0xFF;
    m_colour[1].c[1] = 0xFF;
    m_colour[1].c[2] = 0xFF;
    m_colour[1].c[3] = 0xFF;
    m_colour[2].c[0] = 0xFF;
    m_colour[2].c[1] = 0xFF;
    m_colour[2].c[2] = 0xFF;
    m_colour[2].c[3] = 0xFF;
    m_colour[3].c[0] = 0xFF;
    m_colour[3].c[1] = 0xFF;
    m_colour[3].c[2] = 0xFF;
    m_colour[3].c[3] = 0xFF;
}

void glQuad3::SetColour(const nlColour& c)
{
    unsigned char r = c.c[0];
    unsigned char g = c.c[1];
    unsigned char b = c.c[2];
    unsigned char a = c.c[3];

    m_colour[0].c[0] = r;
    m_colour[0].c[1] = g;
    m_colour[0].c[2] = b;
    m_colour[0].c[3] = a;
    m_colour[1].c[0] = r;
    m_colour[1].c[1] = g;
    m_colour[1].c[2] = b;
    m_colour[1].c[3] = a;
    m_colour[2].c[0] = r;
    m_colour[2].c[1] = g;
    m_colour[2].c[2] = b;
    m_colour[2].c[3] = a;
    m_colour[3].c[0] = r;
    m_colour[3].c[1] = g;
    m_colour[3].c[2] = b;
    m_colour[3].c[3] = a;
}

void glQuad3::SetColour(
    unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    m_colour[0].c[0] = r;
    m_colour[0].c[1] = g;
    m_colour[0].c[2] = b;
    m_colour[0].c[3] = a;
    m_colour[1].c[0] = r;
    m_colour[1].c[1] = g;
    m_colour[1].c[2] = b;
    m_colour[1].c[3] = a;
    m_colour[2].c[0] = r;
    m_colour[2].c[1] = g;
    m_colour[2].c[2] = b;
    m_colour[2].c[3] = a;
    m_colour[3].c[0] = r;
    m_colour[3].c[1] = g;
    m_colour[3].c[2] = b;
    m_colour[3].c[3] = a;
}
