#ifndef GAME_PHYSICS_LOADABLE_PHYSICS_MESH_H
#define GAME_PHYSICS_LOADABLE_PHYSICS_MESH_H

#include "Game/Physics/PhysicsObject.h"

struct PhysTriMeshHeader
{
};

struct dxTriMeshData
{
};

class PhysicsMesh : public PhysicsObject
{
public:
    /* 0x38 */ PhysTriMeshHeader* m_Header;
    /* 0x3C */ nlVector3* m_Vertices;
    /* 0x40 */ unsigned long* m_Indices;
    /* 0x44 */ unsigned char* m_MaterialIDs;
    /* 0x48 */ dxTriMeshData* m_TriMeshData;
};

class LoadablePhysicsMesh : public PhysicsMesh
{
public:
    typedef char* MemType;

    void Destroy();
};

#endif // GAME_PHYSICS_LOADABLE_PHYSICS_MESH_H
