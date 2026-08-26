#ifndef GAME_PHYSICS_NET_MESH_MODEL_LOADER_H
#define GAME_PHYSICS_NET_MESH_MODEL_LOADER_H

#include "Game/Render/NetMesh.h"
#include "NL/gl/glModel.h"
#include "NL/nlAVLTree.h"

class NetMeshModelLoader
{
public:
    class NetMeshVertex
    {
    public:
        NetMeshVertex()
            : mpPacket(0)
            , mIndex(0xFFFF)
            , mParticleIndex(-1)
            , mbIsConstrained(0)
        {
        }

        NetMeshVertex(const glModelPacket* packet, unsigned short index)
            : mpPacket(packet)
            , mIndex(index)
            , mParticleIndex(-1)
            , mbIsConstrained(0)
        {
        }

        const nlVector3* GetPosition() const;
        void GetNormal(nlVector3& normal) const;
        void GetTextureCoord(nlVector2& txtCoord) const;

        bool operator==(const NetMeshVertex& o) const
        {
            return mpPacket == o.mpPacket && mIndex == o.mIndex;
        }

        bool operator<(const NetMeshVertex& o) const
        {
            return mpPacket < o.mpPacket
                || (mpPacket == o.mpPacket && mIndex < o.mIndex);
        }

        /* 0x00 */ const glModelPacket* mpPacket;
        /* 0x04 */ unsigned short mIndex;
        /* 0x08 */ int mParticleIndex;
        /* 0x0C */ unsigned char mbIsConstrained;
    }; // size: 0x10

    class NetMeshEdge
    {
    public:
        NetMeshEdge()
            : mpPacket(0)
            , mpVertex1(0)
            , mpVertex2(0)
        {
        }

        bool operator==(const NetMeshEdge& o) const
        {
            return mpPacket == o.mpPacket
                && mpVertex1->mIndex == o.mpVertex1->mIndex
                && mpVertex2->mIndex == o.mpVertex2->mIndex;
        }

        bool operator<(const NetMeshEdge& o) const
        {
            return mpPacket < o.mpPacket
                || (mpPacket == o.mpPacket
                    && mpVertex1->mIndex < o.mpVertex1->mIndex)
                || (mpPacket == o.mpPacket
                    && mpVertex1->mIndex == o.mpVertex1->mIndex
                    && mpVertex2->mIndex < o.mpVertex2->mIndex);
        }

        void operator=(const NetMeshEdge& o)
        {
            NetMeshVertex* v1;
            NetMeshVertex* v2;
            v2 = o.mpVertex2;
            v1 = o.mpVertex1;
            mpPacket = o.mpPacket;
            if (v1->mIndex < v2->mIndex)
            {
                mpVertex1 = v1;
                mpVertex2 = v2;
            }
            else
            {
                mpVertex1 = v2;
                mpVertex2 = v1;
            }
        }

        /* 0x00 */ const glModelPacket* mpPacket;
        /* 0x04 */ NetMeshVertex* mpVertex1;
        /* 0x08 */ NetMeshVertex* mpVertex2;
    }; // size: 0xC

    typedef nlAVLTreeSlotPool<NetMeshVertex, int,
        DefaultKeyCompare<NetMeshVertex> >
        VertexTree;
    typedef nlAVLTreeSlotPool<NetMeshEdge, int,
        DefaultKeyCompare<NetMeshEdge> >
        EdgeTree;
    typedef AVLTreeEntry<NetMeshVertex, int> VertexEntry;
    typedef AVLTreeEntry<NetMeshEdge, int> EdgeEntry;

    NetMeshModelLoader(
        NetMesh& netMesh, unsigned long netMeshDrawableObjectID);
    virtual ~NetMeshModelLoader();

    void LoadGeometryFromModel();
    void ReadVerticesFromGeometryPacket(const glModelPacket& packet);
    void AddEdge(const glModelPacket& packet, unsigned short idx1,
        unsigned short idx2);
    void AddTriangleFromGeometry(
        const glModelPacket& packet, unsigned short* vertexIndices);
    void ReadEdgesFromGeometryPacket(const glModelPacket& packet);
    void ProcessEdges(const glModelPacket& packet, int maxVertex);
    void CreateNetMeshFromVertexList();

private:
    /* 0x04 */ NetMesh& m_NetMesh;
    /* 0x08 */ unsigned long m_NetMeshDrawableObjectID;
    /* 0x0C */ EdgeTree* m_EdgeList;
    /* 0x10 */ VertexTree* m_VertexList;
    /* 0x14 */ int m_NumParticles;
    /* 0x18 */ u16* m_TriStripIndices;
    /* 0x1C */ int m_CurrentTriStripIndex;
    /* 0x20 */ int m_NumTriStripIndices;
}; // size: 0x24

#endif // GAME_PHYSICS_NET_MESH_MODEL_LOADER_H
