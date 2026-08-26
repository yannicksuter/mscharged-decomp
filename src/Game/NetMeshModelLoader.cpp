#include "Game/Physics/NetMeshModelLoader.h"

#include "Game/Drawable/DrawableModel.h"
#include "Game/MathHelpers.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMemory.h"

typedef NetMeshModelLoader::NetMeshVertex NetMeshVertex;
typedef NetMeshModelLoader::NetMeshEdge NetMeshEdge;
typedef NetMeshModelLoader::VertexTree VertexTree;
typedef NetMeshModelLoader::EdgeTree EdgeTree;
typedef NetMeshModelLoader::VertexEntry VertexEntry;
typedef NetMeshModelLoader::EdgeEntry EdgeEntry;
typedef nlAVLTreeIterator<NetMeshVertex, int,
    DefaultKeyCompare<NetMeshVertex> >
    VertexIterator;
typedef nlAVLTreeIterator<NetMeshEdge, int,
    DefaultKeyCompare<NetMeshEdge> >
    EdgeIterator;

inline void* operator new(unsigned long, void* memory)
{
    return memory;
}

extern "C" RenderObject* fn_8027725C(unsigned long);
extern "C" unsigned long fn_802CC7E4(const glModelPacket*, unsigned long);
extern unsigned long lbl_806E1F0C;

static int s_initialEdgeCount = 1;
static int s_initialVertexCount = 1;
static unsigned char sbPullGoalsOut;

NetMeshModelLoader::NetMeshModelLoader(
    NetMesh& netMesh, unsigned long netMeshDrawableObjectID)
    : m_NetMesh(netMesh)
    , m_NetMeshDrawableObjectID(netMeshDrawableObjectID)
    , m_NumParticles(0)
    , m_TriStripIndices(0)
    , m_CurrentTriStripIndex(0)
{
    LoadGeometryFromModel();
}

NetMeshModelLoader::~NetMeshModelLoader()
{
    m_EdgeList->Clear();
    m_VertexList->Clear();
    delete m_EdgeList;
    delete m_VertexList;
    delete[] m_TriStripIndices;
}

void NetMeshModelLoader::LoadGeometryFromModel()
{
    m_EdgeList = new (nlMalloc(sizeof(EdgeTree), 8, false))
        EdgeTree(0x10, 0x10);
    m_VertexList = new (nlMalloc(sizeof(VertexTree), 8, false))
        VertexTree(0x10, 0x10);

    DrawableModel* pDrawable = (DrawableModel*)fn_8027725C(m_NetMeshDrawableObjectID);
    u16 numPackets = (u16)pDrawable->m_pModel->numPackets;

    m_NumTriStripIndices = 0;
    m_CurrentTriStripIndex = 0;

    for (int packetSumIndex = 0; packetSumIndex < numPackets; ++packetSumIndex)
    {
        m_NumTriStripIndices += pDrawable->m_pModel->packets[packetSumIndex].numVertices;
    }

    m_TriStripIndices = (u16*)nlMalloc(
        m_NumTriStripIndices * sizeof(u16), 8, false);
    for (int i = 0; i < m_NumTriStripIndices; ++i)
        m_TriStripIndices[i] = 0xFFFF;

    for (int packetIndex = 0; packetIndex < numPackets; ++packetIndex)
    {
        glModelPacket& pPacket = pDrawable->m_pModel->packets[packetIndex];
        ReadVerticesFromGeometryPacket(pPacket);
        ReadEdgesFromGeometryPacket(pPacket);
    }

    CreateNetMeshFromVertexList();
}

void NetMeshModelLoader::ReadVerticesFromGeometryPacket(
    const glModelPacket& packet)
{
    DisplayList* pList = packet.displayList;
    u16 vertexOffset = (u16)m_NumParticles;

    int i = 0;
    while (i < packet.numVertices)
    {
        u16* ptr;
        if (pList->hasColorStream != 0)
        {
            u16 ns = pList->numStreams;
            int stride = (ns - 1) * 2 + 1;
            ptr = (u16*)((u8*)pList->list + stride * i + 4);
        }
        else
        {
            u16 ns = pList->numStreams;
            int stride = ns * 2;
            ptr = (u16*)((u8*)pList->list + i * stride + 3);
        }

        m_TriStripIndices[m_CurrentTriStripIndex] = *ptr + vertexOffset;
        ++m_CurrentTriStripIndex;
        ++i;
    }

    m_NetMesh.SetTexture(fn_802CC7E4(&packet, lbl_806E1F0C));

    for (int i2 = 0; i2 < packet.numUniqueVertices; ++i2)
    {
        NetMeshVertex vertex;
        vertex.mpPacket = &packet;
        vertex.mIndex = (u16)i2;
        vertex.mParticleIndex = m_NumParticles;
        m_VertexList->Add(vertex, s_initialVertexCount);
        ++m_NumParticles;
    }
}

inline void NetMeshModelLoader::AddEdge(
    const glModelPacket& packet, unsigned short idx1, unsigned short idx2)
{
    NetMeshEdge edge;
    NetMeshVertex vertex2;
    NetMeshVertex vertex1;

    int* pValue;
    NetMeshVertex* pVertex2;
    NetMeshVertex* pVertex1;

    vertex1.mpPacket = &packet;
    vertex1.mIndex = idx1;
    vertex2.mpPacket = &packet;
    vertex2.mIndex = idx2;

    m_VertexList->Find(vertex1, &pValue, &pVertex1);
    m_VertexList->Find(vertex2, &pValue, &pVertex2);

    edge.mpPacket = &packet;
    NetMeshVertex* v2 = pVertex2;
    if (pVertex1->mIndex < v2->mIndex)
    {
        edge.mpVertex1 = pVertex1;
        edge.mpVertex2 = v2;
    }
    else
    {
        edge.mpVertex1 = v2;
        edge.mpVertex2 = pVertex1;
    }

    int* pRefCount = m_EdgeList->Add(edge, s_initialEdgeCount);
    if (pRefCount != 0)
        ++*pRefCount;
}

inline void NetMeshModelLoader::AddTriangleFromGeometry(
    const glModelPacket& packet, unsigned short* vertexIndices)
{
    unsigned char isThin = 0;
    if (vertexIndices[0] == vertexIndices[1]
        || vertexIndices[1] == vertexIndices[2]
        || vertexIndices[0] == vertexIndices[2])
    {
        isThin = 1;
    }

    if (!isThin)
    {
        for (int j = 0; j < 3; ++j)
            AddEdge(packet, vertexIndices[j], vertexIndices[(j + 1) % 3]);
    }
}

void NetMeshModelLoader::ReadEdgesFromGeometryPacket(
    const glModelPacket& packet)
{
    u16 maxVertex = 0;
    if (packet.primType != 1)
        return;

    DisplayList* pList = packet.displayList;
    int i = 2;
    while (i < packet.numVertices)
    {
        u16 vertexIndices[3];
        u16* pVtx = vertexIndices;
        u16* ptr;
        u16 ns;
        int vertOff;
        int stride;
        int offset;
        u8* ptr8;

        for (int j = 0; j < 3; ++j)
        {
            if (pList->hasColorStream != 0)
            {
                int base = i - 2;
                ns = pList->numStreams;
                vertOff = base + j;
                stride = (ns - 1) * 2 + 1;
                offset = stride * vertOff;
                ptr8 = (u8*)pList->list + offset;
                ptr = (u16*)ptr8;
                ptr8 = (u8*)ptr;
                ptr8 += 4;
                ptr = (u16*)ptr8;
            }
            else
            {
                ns = pList->numStreams;
                vertOff = i;
                vertOff += j;
                vertOff -= 2;
                stride = ns * 2;
                offset = vertOff * stride;
                ptr8 = (u8*)pList->list + offset;
                ptr = (u16*)ptr8;
                ptr8 = (u8*)ptr;
                ptr8 += 3;
                ptr = (u16*)ptr8;
            }

            *pVtx = *ptr;
            if (*pVtx > maxVertex)
                maxVertex = *pVtx;
            ++pVtx;
        }

        AddTriangleFromGeometry(packet, vertexIndices);
        ++i;
    }

    ProcessEdges(packet, (int)maxVertex);
}

void NetMeshModelLoader::ProcessEdges(
    const glModelPacket& packet, int maxVertex)
{
    unsigned char* allReady = (unsigned char*)nlMalloc(maxVertex + 1, 8, false);
    for (int i = 0; i < maxVertex; ++i)
        allReady[i] = 0;

    unsigned char* readyIndicator = (unsigned char*)nlMalloc(maxVertex + 1, 8, false);
    for (int i = 0; i < maxVertex; ++i)
        readyIndicator[i] = 0;

    EdgeIterator* iter = m_EdgeList->GetIterator();
    while (iter->IsValid())
    {
        EdgeEntry* edgeEntry = iter->Current();
        if (edgeEntry->key.mpPacket == &packet)
        {
            unsigned short index2;
            unsigned short index1;
            int refCount = edgeEntry->value;
            index1 = edgeEntry->key.mpVertex1->mIndex;
            index2 = edgeEntry->key.mpVertex2->mIndex;
            if (refCount == 1)
            {
                allReady[index1] = 1;
                allReady[index2] = 1;
                edgeEntry->key.mpVertex1->mbIsConstrained = 1;
                edgeEntry->key.mpVertex2->mbIsConstrained = 1;
            }
            readyIndicator[index1] = 1;
            readyIndicator[index2] = 1;
        }
        iter->Next();
    }

    if (iter != 0)
        delete iter;
    delete readyIndicator;
    delete allReady;
}

void NetMeshModelLoader::CreateNetMeshFromVertexList()
{
    NetMeshVertex* vertex;
    int numEdges = 0;
    int numVertices = 0;
    int numConstrainedVertices = 0;

    VertexIterator* vertexIter = m_VertexList->GetIterator();
    while (vertexIter->IsValid())
    {
        VertexEntry* vertexEntry = vertexIter->Current();
        ++numVertices;
        if (vertexEntry->key.mbIsConstrained != 0)
            ++numConstrainedVertices;
        vertexIter->Next();
    }
    if (vertexIter != 0)
        delete vertexIter;

    EdgeIterator* edgeIter = m_EdgeList->GetIterator();
    while (edgeIter->IsValid())
    {
        ++numEdges;
        edgeIter->Next();
    }
    if (edgeIter != 0)
        delete edgeIter;

    m_NumParticles = numVertices;
    m_NetMesh.Allocate(
        m_NumParticles, numEdges, numConstrainedVertices);

    DrawableModel* pObject = (DrawableModel*)fn_8027725C(m_NetMeshDrawableObjectID);

    nlVector3 minimum;
    minimum.x = 10000.0f;
    minimum.y = 10000.0f;
    minimum.z = 100000.0f;
    nlVector3 maximum;
    maximum.x = -10000.0f;
    maximum.y = -10000.0f;
    maximum.z = -100000.0f;

    nlVector3 position;
    nlVector3 normal;
    nlVector3 position1;
    nlVector3 position2;

    vertexIter = m_VertexList->GetIterator();
    float pullDistance;
    float scale = 1024.0f;
    int index = 0;
    pullDistance = 5.0f;
    while (vertexIter->IsValid())
    {
        VertexEntry* vertexEntry = vertexIter->Current();
        vertex = &vertexEntry->key;

        position = *vertex->GetPosition();
        nlVector2 texCoord;
        shortVector2 shortCoord;
        vertex->GetNormal(normal);
        vertex->GetTextureCoord(texCoord);
        shortCoord.e[0] = (s16)(scale * texCoord.x);
        shortCoord.e[1] = (s16)(scale * texCoord.y);

        minimum.x = nlMinEquals(minimum.x, position.x);
        maximum.x = nlMaxEquals(maximum.x, position.x);
        minimum.y = nlMinEquals(minimum.y, position.y);
        maximum.y = nlMaxEquals(maximum.y, position.y);
        minimum.z = nlMinEquals(minimum.z, position.z);
        maximum.z = nlMaxEquals(maximum.z, position.z);

        if (sbPullGoalsOut != 0)
            position.x -= pullDistance;

        nlVector3 transformedPosition;
        nlMultPosVectorMatrix(
            transformedPosition, position, pObject->GetWorldMatrix());
        position = transformedPosition;

        nlVector3 transformedNormal;
        nlMultDirVectorMatrix(
            transformedNormal, normal, pObject->GetWorldMatrix());
        normal = transformedNormal;

        int particle = m_NetMesh.m_NumParticles;
        NetMesh* pNetMesh = &m_NetMesh;
        pNetMesh->m_v3Position[particle] = position;
        pNetMesh->m_v3Normal[particle] = normal;
        pNetMesh->m_v2TextureCoords[particle] = shortCoord;
        particle = pNetMesh->m_NumParticles;
        pNetMesh->m_NumParticles = particle + 1;

        if (vertex->mbIsConstrained != 0)
            m_NetMesh.SetPositionConstraint(index, position);

        vertex->mParticleIndex = index;
        vertexIter->Next();
        ++index;
    }
    if (vertexIter != 0)
        delete vertexIter;

    edgeIter = m_EdgeList->GetIterator();
    int index2;
    int index1;
    EdgeEntry* edgeEntry;
    while (edgeIter->IsValid())
    {
        edgeEntry = edgeIter->Current();
        if (edgeEntry->value > 1)
        {
            index1 = edgeEntry->key.mpVertex1->mParticleIndex;
            index2 = edgeEntry->key.mpVertex2->mParticleIndex;
            position1 = *edgeEntry->key.mpVertex1->GetPosition();
            position2 = *edgeEntry->key.mpVertex2->GetPosition();
            nlVector3 delta;
            nlVec3Sub(delta, position1, position2);
            float distance = nlVec3Length(delta);
            m_NetMesh.SetDistanceConstraint(index1, index2, distance);
        }
        edgeIter->Next();
    }
    if (edgeIter != 0)
        delete edgeIter;

    m_NetMesh.SetTriStripIndices(
        m_NumTriStripIndices, m_TriStripIndices);
}
