#include "Game/GL/GLVertexAnim.h"

#include "NL/nlMemory.h"

#include <string.h>

extern "C" void fn_802D3A00(glModelStream* pStream, void* address);

struct UnidentifiedGLVertexAnimHeader_802D3D48
{
    u32 m_00;
    u32 m_04;
    u32 m_08;
    u32 m_0C;
    u32 m_10;
    u32 m_14;
};

GLVertexAnim::GLVertexAnim(const void* data, const void* extraData)
{
    const UnidentifiedGLVertexAnimHeader_802D3D48* header =
        (const UnidentifiedGLVertexAnimHeader_802D3D48*)data;
    m_uHashID = header->m_00;
    m_nNumFrames = header->m_04;
    m_nNumVertices = header->m_08;
    m_Unknown0C = header->m_0C;
    m_Unknown10 = header->m_10;
    m_Unknown14 = header->m_14;

    m_eMode = GLVAnimMode_Loop;
    m_bDone = false;
    m_fTimeScale = 1.0f;
    m_fFrame = 0.0f;
    m_fFrameRate = 30.0f;
    m_pVertices = 0;
    m_pModel = 0;

    m_Unknown18 = (s32*)nlMalloc(m_Unknown14 * sizeof(s32), 8, false);
    memcpy(m_Unknown18, extraData, m_Unknown14 * sizeof(s32));
}

GLVertexAnim::~GLVertexAnim()
{
    delete m_Unknown18;
}

glModel* GLVertexAnim::GetModel(int frame)
{
    int actualFrame = (frame < 0) ? (int)m_fFrame : frame;

    glModel* model = glModelDupNoStreams(m_pModel, false, 0);
    u8* vertices = m_pVertices
                 + m_nNumVertices * actualFrame * m_Unknown0C;

    for (glModelPacket* packet = model->packets;
         packet < model->packets + model->numPackets; packet++)
    {
        u32 offset = 0;
        glModelStream* streams = packet->streams;
        glModelStream* endVertexData = streams + packet->numStreams;
        while (streams < endVertexData)
        {
            for (int i = 0; i < m_Unknown14; i++)
            {
                if (m_Unknown18[i] == streams->id)
                {
                    fn_802D3A00(streams,
                        vertices + offset * packet->numUniqueVertices);
                    offset += streams->stride;
                    break;
                }
            }
            streams++;
        }
        vertices += offset * packet->numUniqueVertices;
    }

    return model;
}

void GLVertexAnim::Update(float dt)
{
    if (m_bDone)
    {
        return;
    }

    m_fFrame += m_fFrameRate * (m_fTimeScale * dt);

    if (m_fFrame >= m_nNumFrames)
    {
        if (m_eMode == GLVAnimMode_Hold)
        {
            m_bDone = true;
            m_fFrame = m_nNumFrames - 1;
        }
        else
        {
            m_fFrame = 0.0f;
        }
    }
}
