#include <revolution/types.h>

#include "NL/gl/glRenderList.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glMaterialProgram.h"
#include "NL/platvmath.h"

unsigned long UnidentifiedPacketSorter_8052E2D8::fn_14(
    GLView*, const glModelPacket* pPacket)
{
    return ((GLMaterialProgram*)pPacket->unknown10)->programHash;
}

unsigned long UnidentifiedPacketSorter_8052E2C0::fn_14(
    GLView* view, const glModelPacket* pPacket)
{
    nlMatrix4 packetMatrix;
    glGetMatrix(pPacket->matrix, packetMatrix);

    nlVector3 pos = packetMatrix.GetTranslation();
    nlMultPosVectorMatrix(pos, *view->m_Interface->GetViewMatrix());
    pos.z = 0.0f <= pos.z ? 0.0f : pos.z;

    return (unsigned long)(int)(-pos.z * 2147483648.0f);
}

unsigned long UnidentifiedPacketSorter_8052E2A8::fn_14(
    GLView* view, const glModelPacket* pPacket)
{
    nlMatrix4 packetMatrix;
    nlVector3 out;
    glGetMatrix(pPacket->matrix, packetMatrix);
    nlMultPosVectorMatrix(out, *(nlVector3*)&packetMatrix.e2[3][0],
        *view->m_Interface->GetViewMatrix());
    return ((unsigned long)(int)(-out.z * 100.0f) << 12)
        | (m_Sequence++ & 0xFFF);
}

const glModelPacket* UnidentifiedPacketSorter_802CCBBC::fn_08()
{
    m_Iterator.Initialize(m_Tree.m_Root);
    return fn_0C();
}

const glModelPacket* UnidentifiedPacketSorter_802CCBBC::fn_0C()
{
    if (!m_Iterator.IsValid())
        return 0;

    const glModelPacket* pPacket = m_Iterator.Current()->value;
    m_Iterator.Next();
    return pPacket;
}

void UnidentifiedPacketSorter_802CCBBC::fn_10(
    GLView* view, const glModelPacket* pPacket)
{
    UnidentifiedPacketSortKey sortKey =
        ((UnidentifiedPacketSortKey)fn_14(view, pPacket) << 32)
        | (unsigned long)pPacket;
    m_Tree.Add(sortKey, pPacket);
}

PacketCallbackManager::PacketCallbackManager(
    GLView* view, GLViewPacketCallback cb)
    : m_View(view)
    , m_Cb(cb)
    , m_LastRaster((unsigned long)-1)
    , m_LastMatrix((unsigned long)-1)
    , m_LastProgram(0)
{
}

void PacketCallbackManager::DoCallback(
    const glModelPacket* p, unsigned int count)
{
    unsigned long flags = 0;

    if (m_LastRaster != p->rasterState)
    {
        flags |= 2;
        m_LastRaster = p->rasterState;
    }

    if (m_LastMatrix != p->matrix)
    {
        flags |= 4;
        m_LastMatrix = p->matrix;
    }

    if (p->unknown10 != m_LastProgram)
    {
        flags |= 8;
        m_LastProgram = p->unknown10;
    }

    unsigned long stage = flags | 0x80;
    while (count != 0)
    {
        m_Cb(m_View, stage, p);
        --count;
    }
}
