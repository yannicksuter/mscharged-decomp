#include "NL/gl/glDraw2.h"
#include "NL/gl/glTexture.h"

#include "Game/GL/GLTexturedColourMeshWriter.h"
#include "Game/GL/GLFloatTexturedColourMeshWriter.h"
#include "NL/platvmath.h"
#include "NL/nlString.h"

#include "NL/gl/gl.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glState.h"
#include "NL/gl/glStruct.h"
#include "NL/gl/glView.h"
#include "NL/gl/glMaterialParameters.h"

#include <string.h>
#include "NL/gl/glTexture.h"

static int QuadMap[4] = { 0, 1, 2, 3 };
static int TriListMap[6] = { 0, 1, 2, 3, 0, 2 };


extern "C" glModel* fn_802C834C(glPoly2* pPolys,
    unsigned long numPolys, unsigned long* pMatrixHandle, bool textured);
extern "C" glModel* fn_802C89F4(glPoly2* pPolys,
    unsigned long numPolys, unsigned long* pMatrixHandle, bool textured);

bool glPoly2::Attach(
    GLView* view, int layer, unsigned long* pMatrixHandle)
{
    if (view == 0)
        return false;

    GLTexturedColourMeshWriter writer;
    glModel* model = fn_802C834C(this, 1, pMatrixHandle, gl_GetCurrentStateBundle()->texconfig != 0);
    if (model != 0)
        view->AttachModel(model, layer);
    return model != 0;
}

extern "C" glModel* fn_802C834C(glPoly2* pPolys,
    unsigned long numPolys, unsigned long* pMatrixHandle, bool textured)
{
    GLTexturedColourMeshWriter writer;

    unsigned long oldMatrix;
    unsigned long numVerts;
    int* mapArray;
    int primitive;
    if (glHasQuads())
    {
        mapArray = QuadMap;
        numVerts = 4;
        primitive = 3;
    }
    else
    {
        mapArray = TriListMap;
        numVerts = 6;
        primitive = 0;
    }

    oldMatrix = glSetCurrentMatrix(
        pMatrixHandle != 0 ? *pMatrixHandle : glGetIdentityMatrix());

    if (textured)
    {
        if (writer.Begin(numVerts * numPolys, primitive, 0))
        {
            glPoly2* pPoly = pPolys;
            for (unsigned long i = 0; i < numPolys; ++i, ++pPoly)
            {
                for (unsigned long j = 0; j < numVerts; ++j)
                {
                    int index = mapArray[j];
                    writer.Colour(pPoly->m_colour[index]);
                    writer.Texcoord(pPoly->m_uv[index]);
                    writer.Vertex(pPoly->m_pos[index].x,
                        pPoly->m_pos[index].y,
                        pPoly->depth);
                }
            }
            unsigned long long currentTextureState = glGetCurrentTextureState();
            unsigned long textureState = glGetTextureState(
                currentTextureState, GLTS_DiffuseWrap);
            unsigned long clampS = textureState == 0 || textureState == 1;
            unsigned char wrapS = !clampS;
            unsigned long clampT = textureState == 2 || textureState == 0;
            unsigned char wrapT = !clampT;

            u32 texture = glGetCurrentTexture(GLTT_Diffuse);
            glTextureBinding* state = (glTextureBinding*)writer.model->packets->unknown20;
            state->texture = texture;
            state->textureIndex = 0xFFFF;
            state->SetWrapS(wrapS);
            state->SetWrapT(wrapT);
            state->unknown07 = 0;
        }
    }
    else
    {
        if (writer.Begin(numVerts * numPolys, primitive, 0))
        {
            glPoly2* pPoly = pPolys;
            for (unsigned long i = 0; i < numPolys; ++i, ++pPoly)
            {
                for (unsigned long j = 0; j < numVerts; ++j)
                {
                    int index = mapArray[j];
                    writer.Colour(pPoly->m_colour[index]);
                    writer.TexcoordZero();
                    writer.Vertex(pPoly->m_pos[index].x,
                        pPoly->m_pos[index].y,
                        pPoly->depth);
                }
            }
            glTextureBinding* state = (glTextureBinding*)writer.model->packets->unknown20;
            state->texture = gWhiteTextureID;
            state->textureIndex = 0xFFFF;
            state->SetWrapS(true);
            state->SetWrapT(true);
            state->unknown07 = 0;
        }
    }

    if (!writer.End())
    {
        return 0;
    }

    glSetCurrentMatrix(oldMatrix);
    glModel* model = writer.GetModel();
    return model;
}

bool glAttachPoly2(GLView* view, unsigned long numPolys, glPoly2* pPolys,
    unsigned long* pMatrixHandle)
{
    glModel* model = fn_802C834C(pPolys, numPolys, pMatrixHandle, gl_GetCurrentStateBundle()->texconfig == 1);
    if (model == 0)
        return false;
    view->AttachModel(model, 0);
    return true;
}

bool glAttachPoly2(GLView* view, int layer, unsigned long numPolys,
    glPoly2* pPolys, unsigned long* pMatrixHandle)
{
    glModel* model = fn_802C834C(pPolys, numPolys, pMatrixHandle, gl_GetCurrentStateBundle()->texconfig == 1);
    if (model == 0)
        return false;
    view->AttachModel(model, layer);
    return true;
}

bool glAttachPoly2(GLView* view, int layer, unsigned long numPolys,
    glPoly2* pPolys, const void* pUserData, unsigned long* pMatrixHandle)
{
    static unsigned long scissorbox = nlStringHash("scissorbox");
    glModel* model = fn_802C89F4(pPolys, numPolys, pMatrixHandle, gl_GetCurrentStateBundle()->texconfig == 1);
    if (model == 0)
        return false;

    void* data = glFrameAlloc(16, GLM_Header);
    memcpy(data, pUserData, 16);
    glModelPacket* packet = model->packets;
    while (packet < model->packets + model->numPackets)
    {
        glSetMaterialParameterArray(packet, scissorbox, data, 4);
        ++packet;
    }
    view->AttachModel(model, layer);
    return true;
}

extern "C" glModel* fn_802C89F4(glPoly2* pPolys,
    unsigned long numPolys, unsigned long* pMatrixHandle, bool textured)
{
    GLFloatTexturedColourMeshWriter writer;

    unsigned long oldMatrix;
    unsigned long numVerts;
    int* mapArray;
    int primitive;
    if (glHasQuads())
    {
        mapArray = QuadMap;
        numVerts = 4;
        primitive = 3;
    }
    else
    {
        mapArray = TriListMap;
        numVerts = 6;
        primitive = 0;
    }

    oldMatrix = glSetCurrentMatrix(
        pMatrixHandle != 0 ? *pMatrixHandle : glGetIdentityMatrix());

    if (textured)
    {
        if (writer.Begin(numVerts * numPolys, primitive, 0))
        {
            glPoly2* pPoly = pPolys;
            for (unsigned long i = 0; i < numPolys; ++i, ++pPoly)
            {
                for (unsigned long j = 0; j < numVerts; ++j)
                {
                    int index = mapArray[j];
                    writer.Colour(pPoly->m_colour[index]);
                    writer.Texcoord(pPoly->m_uv[index]);
                    writer.Vertex(pPoly->m_pos[index].x,
                        pPoly->m_pos[index].y,
                        pPoly->depth);
                }
            }
            unsigned long long currentTextureState = glGetCurrentTextureState();
            unsigned long textureState = glGetTextureState(
                currentTextureState, GLTS_DiffuseWrap);
            unsigned long clampS = textureState == 0 || textureState == 1;
            unsigned char wrapS = !clampS;
            unsigned long clampT = textureState == 2 || textureState == 0;
            unsigned char wrapT = !clampT;

            u32 texture = glGetCurrentTexture(GLTT_Diffuse);
            glTextureBinding* state = (glTextureBinding*)writer.model->packets->unknown20;
            state->texture = texture;
            state->textureIndex = 0xFFFF;
            state->SetWrapS(wrapS);
            state->SetWrapT(wrapT);
            state->unknown07 = 0;
        }
    }
    else
    {
        if (writer.Begin(numVerts * numPolys, primitive, 0))
        {
            glPoly2* pPoly = pPolys;
            for (unsigned long i = 0; i < numPolys; ++i, ++pPoly)
            {
                for (unsigned long j = 0; j < numVerts; ++j)
                {
                    int index = mapArray[j];
                    writer.Colour(pPoly->m_colour[index]);
                    writer.TexcoordZero();
                    writer.Vertex(pPoly->m_pos[index].x,
                        pPoly->m_pos[index].y,
                        pPoly->depth);
                }
            }
            glTextureBinding* state = (glTextureBinding*)writer.model->packets->unknown20;
            state->texture = gWhiteTextureID;
            state->textureIndex = 0xFFFF;
            state->SetWrapS(true);
            state->SetWrapT(true);
            state->unknown07 = 0;
        }
    }

    if (!writer.End())
    {
        return 0;
    }

    glSetCurrentMatrix(oldMatrix);
    glModel* model = writer.GetModel();
    return model;
}

void glPoly2::FullCoverage(const nlColour& col, float z)
{
    const float width = glGetOrthographicWidth() + 1.0f;
    const float height = glGetOrthographicHeight() + 1.0f;
    gl_ScreenInfo* screenInfo = glGetScreenInfo();
    const float border = screenInfo->PixelCentre;

    nlVec2Set(m_pos[0], -border, -border);
    nlVec2Set(m_pos[1], -border, height - border);
    nlVec2Set(m_pos[2], width - border, height - border);
    nlVec2Set(m_pos[3], width - border, -border);

    depth = z;

    nlVec2Set(m_uv[0], 0.0f, 0.0f);
    nlVec2Set(m_uv[1], 0.0f, 1.0f);
    nlVec2Set(m_uv[2], 1.0f, 1.0f);
    nlVec2Set(m_uv[3], 1.0f, 0.0f);

    m_colour[0] = col;
    m_colour[1] = col;
    m_colour[2] = col;
    m_colour[3] = col;
}

void glPoly2::SetupRectangle(float x, float y, float w, float h, float z)
{
    nlVec2Set(m_pos[0], x, y);
    nlVec2Set(m_pos[1], x, y + h);
    nlVec2Set(m_pos[2], x + w, y + h);
    nlVec2Set(m_pos[3], x + w, y);

    nlVec2Set(m_uv[0], 0.0f, 0.0f);
    nlVec2Set(m_uv[1], 0.0f, 1.0f);
    nlVec2Set(m_uv[2], 1.0f, 1.0f);
    nlVec2Set(m_uv[3], 1.0f, 0.0f);

    depth = z;
}

void glPoly2::SetupRotatedRectangle(
    float cx, float cy, float w, float h, float angle, float z)
{
    nlVector2 v;
    nlMatrix3 m;

    nlMakeRotationMatrixZ(m, angle);

    nlVec2Set(v, -w / 2.0f, -h / 2.0f);
    {
        nlVector2 temp;
        nlMultVectorMatrix(temp, v, m);
        v = temp;
    }
    nlVec2Set(m_pos[0], v.x + cx, v.y + cy);

    nlVec2Set(v, -w / 2.0f, h / 2.0f);
    {
        nlVector2 temp;
        nlMultVectorMatrix(temp, v, m);
        v = temp;
    }
    nlVec2Set(m_pos[1], v.x + cx, v.y + cy);

    nlVec2Set(v, w / 2.0f, h / 2.0f);
    {
        nlVector2 temp;
        nlMultVectorMatrix(temp, v, m);
        v = temp;
    }
    nlVec2Set(m_pos[2], v.x + cx, v.y + cy);

    nlVec2Set(v, w / 2.0f, -h / 2.0f);
    {
        nlVector2 temp;
        nlMultVectorMatrix(temp, v, m);
        v = temp;
    }
    nlVec2Set(m_pos[3], v.x + cx, v.y + cy);

    nlVec2Set(m_uv[0], 0.0f, 0.0f);
    nlVec2Set(m_uv[1], 0.0f, 1.0f);
    nlVec2Set(m_uv[2], 1.0f, 1.0f);
    nlVec2Set(m_uv[3], 1.0f, 0.0f);

    depth = z;
}

void glPoly2::SetColour(const nlColour& col)
{
    m_colour[0] = col;
    m_colour[1] = col;
    m_colour[2] = col;
    m_colour[3] = col;
}
