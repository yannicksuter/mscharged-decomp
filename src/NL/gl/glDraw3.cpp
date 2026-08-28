#include "NL/gl/glDraw3.h"

#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"

struct MeshWriter_802A8218;

extern "C" void fn_802A8218(MeshWriter_802A8218*);
extern "C" void* fn_802A8238(MeshWriter_802A8218*, int);
extern "C" bool fn_802A8278(MeshWriter_802A8218*, int, int, void*);
extern "C" bool fn_802A8424(MeshWriter_802A8218*);

extern u32 lbl_806E1F34;

struct MeshWriter_802A8218
{
    int count;
    glModel* model;
    void* resource;
    float* position;
    short* texcoord;
    u32* colour;

    bool Begin(int count, int primitive, void* resource)
    {
        return fn_802A8278(this, count, primitive, resource);
    }

    bool End()
    {
        return fn_802A8424(this);
    }

    glModel* GetModel() const
    {
        return model;
    }

    void Colour(const nlColour& c)
    {
        *colour++ = *(const u32*)&c;
    }

    void Texcoord(const nlVector2& uv)
    {
        short u = (short)(uv.x * 1024.0f);
        short v = (short)(uv.y * 1024.0f);
        *texcoord++ = u;
        *texcoord++ = v;
    }

    void Vertex(const nlVector3& pos)
    {
        float x;
        float y;
        float z;
        z = pos.z;
        y = pos.y;
        x = pos.x;
        *position++ = x;
        *position++ = y;
        *position++ = z;
    }
};

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
    MeshWriter_802A8218 writer;
    fn_802A8218(&writer);
    unsigned long texconfig = gl_GetCurrentStateBundle()->texconfig;

    if (glHasQuads())
    {
        if (!writer.Begin(4, 3, 0))
        {
            fn_802A8238(&writer, -1);
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
        if (!writer.Begin(4, 1, 0))
        {
            fn_802A8238(&writer, -1);
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
        UnidentifiedTextureState* state =
            (UnidentifiedTextureState*)writer.model->packets->unknown20;
        state->texture = lbl_806E1F34;
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
        UnidentifiedTextureState* state =
            (UnidentifiedTextureState*)writer.model->packets->unknown20;
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
    fn_802A8238(&writer, -1);
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
    nlVector3 sp8;
    {
        float f3 = 0.5f;
        float temp_f31;
        float temp_f29;
        temp_f29 = -w * f3;
        temp_f31 = -h0 * f3;
        nlVec3Set(sp8, temp_f29, temp_f31, 0.0f);
    }
    nlMultPosVectorMatrix(sp8, mtx);
    m_pos[0] = sp8;

    {
        float temp_f30 = w;
        float temp_f31 = -h1;
        float f3 = 0.5f;
        nlVec3Set(sp8, temp_f30 * f3, temp_f31 * f3, 0.0f);
    }
    nlMultPosVectorMatrix(sp8, mtx);
    m_pos[1] = sp8;

    {
        float f3 = 0.5f;
        float temp_f30 = w;
        float temp_f31_2 = h1;
        nlVec3Set(sp8, temp_f30 * f3, temp_f31_2 * f3, 0.0f);
    }
    nlMultPosVectorMatrix(sp8, mtx);
    m_pos[2] = sp8;

    {
        float f3 = 0.5f;
        float temp_f29 = -w;
        float temp_f31_2 = h0;
        nlVec3Set(sp8, temp_f29 * f3, temp_f31_2 * f3, 0.0f);
    }
    nlMultPosVectorMatrix(sp8, mtx);
    m_pos[3] = sp8;

    float var_f0 = flipU ? 1.0f : 0.0f;
    float var_f1 = flipU ? 0.0f : 1.0f;
    float var_f2 = flipV ? 1.0f : 0.0f;
    float var_f3 = flipV ? 0.0f : 1.0f;

    m_uv[0].x = var_f0;
    m_uv[0].y = var_f2;
    m_uv[1].x = var_f1;
    m_uv[1].y = var_f2;
    m_uv[2].x = var_f1;
    m_uv[2].y = var_f3;
    m_uv[3].x = var_f0;
    m_uv[3].y = var_f3;

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
