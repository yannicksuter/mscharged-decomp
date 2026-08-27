#include "Game/Physics/Physics.h"

#include "Game/Ball.h"
#include "Game/Field.h"
#include "Game/Inventory.h"
#include "Game/Physics/CollisionSpace.h"
#include "Game/Physics/LoadablePhysicsMesh.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Physics/PhysicsGroundPlane.h"
#include "Game/Physics/PhysicsNet.h"
#include "Game/Physics/PhysicsRoundedCorner.h"
#include "Game/Physics/PhysicsWall.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "ode/NLGAdditions.h"
#include "ode/ext/dColumn.h"
#include "ode/ext/dCylinder.h"
#include "ode/ext/dFinitePlane.h"
#include "ode/ext/dRoundedCorner.h"
#include "ode/memory.h"

inline void* operator new(unsigned long, void* memory)
{
    return memory;
}

extern "C" void* memcpy(void*, const void*, unsigned long);
extern "C" bool fn_8013E2E4();
extern "C" void fn_80144130(void*);
extern void* lbl_806E11F0;

class SimpleCollisionSpace : public CollisionSpace
{
public:
    SimpleCollisionSpace(PhysicsWorld*, bool);
    virtual ~SimpleCollisionSpace() { }
};

static bool sbDisableCollisionDetection;
static bool sbNonMovingAABBsInitialized;
PhysicsMesh* g_TerrainMesh;
PhysicsWorld* g_PhysicsWorld;
CollisionSpace* g_CollisionSpace;

nlListContainer<PhysicsObject*> g_StaticPhysicsPrimitives;
nlListContainer<PhysicsObject*> g_NetPhysicsObjects;
static cInventory<LoadablePhysicsMesh> s_PhysicsMeshes;
static PhysicsRoundedCorner* corners[4];

void* ODEAlloc(unsigned long size)
{
    return nlMalloc(size, 8, false);
}

void* ODERealloc(void* oldPtr, unsigned long oldSize, unsigned long newSize)
{
    void* newPtr = nlMalloc(newSize, 8, false);
    if (oldSize != 0)
    {
        memcpy(newPtr, oldPtr, oldSize);
    }
    nlFree(oldPtr);
    return newPtr;
}

void ODEFree(void* ptr, unsigned long)
{
    nlFree(ptr);
}

extern "C" void fn_8013D7A0()
{
    dSetAllocHandler(ODEAlloc);
    dSetReallocHandler(ODERealloc);
    dSetFreeHandler(ODEFree);
}

extern "C" void fn_8013D7E0()
{
    dGeomDestroy(dCreateColumn(0, 1.0f));
    dGeomDestroy(dCreateCylinder(0, 1.0f, 1.0f));
    dGeomDestroy(dCreateFinitePlane(0, -1.0f, 1.0f, -1.0f, 1.0f, true, -1.0f));
    dGeomDestroy(dCreateRoundedCorner(0, 1.0f, true, true));
}

extern "C" void fn_8013D85C()
{
    fn_8013E2E4();

    g_PhysicsWorld = new (nlMalloc(sizeof(PhysicsWorld), 8, false)) PhysicsWorld();
    g_CollisionSpace = new (nlMalloc(sizeof(SimpleCollisionSpace), 8, false))
        SimpleCollisionSpace(g_PhysicsWorld, true);

    g_PhysicsWorld->SetCFM(0.00001f);
    g_PhysicsWorld->SetERP(0.2f);
}

extern "C" void fn_8013D8DC()
{
    g_StaticPhysicsPrimitives.AddEnd(
        new (nlMalloc(sizeof(PhysicsGroundPlane), 8, false)) PhysicsGroundPlane(g_CollisionSpace));

    int i;
    for (i = 0; i < 4; i++)
    {
        const sSideLinePlane& sideline = cField::GetSideline(i);
        g_StaticPhysicsPrimitives.AddEnd(
            new (nlMalloc(sizeof(PhysicsWall), 8, false)) PhysicsWall(
                g_CollisionSpace, sideline.vNormal.x, sideline.vNormal.y, sideline.fDistance));
    }

    for (i = 0; i < 4; i++)
    {
        const sCornerSegment& corner = cField::GetCorner(i);
        PhysicsRoundedCorner* roundedCorner
            = new (nlMalloc(sizeof(PhysicsRoundedCorner), 8, false)) PhysicsRoundedCorner(
                g_CollisionSpace,
                corner.vCenter,
                corner.fRadius,
                corner.vCenter.x > 0.0f,
                corner.vCenter.y > 0.0f);
        corners[i] = roundedCorner;
    }

    PhysicsNet::StaticInit(g_CollisionSpace);

    if (NetMesh::s_bAnimatedNetMeshEnabled)
    {
        unsigned long positiveMesh = nlStringLowerHash("netmesh");
        unsigned long negativeMesh = nlStringLowerHash("netmesh01");
        PhysicsNet::spPhysNetPositiveX->mpNetMesh->Initialize(positiveMesh);
        PhysicsNet::spPhysNetNegativeX->mpNetMesh->Initialize(negativeMesh);
    }

    sbNonMovingAABBsInitialized = false;
}

extern "C" void fn_8013DB18()
{
    PhysicsNet::StaticDestroy();

    for (int i = 0; i < 4; i++)
    {
        delete corners[i];
    }

    ListEntry<PhysicsObject*>* entry = g_StaticPhysicsPrimitives.m_Head;
    while (entry != 0)
    {
        delete entry->entry;
        entry = entry->next;
    }

    g_StaticPhysicsPrimitives.Clear();
    g_NetPhysicsObjects.Clear();
    s_PhysicsMeshes.Clear();
    g_TerrainMesh = 0;
}

extern "C" void fn_8013DDD4()
{
    delete g_CollisionSpace;
    g_CollisionSpace = 0;

    delete g_PhysicsWorld;
    g_PhysicsWorld = 0;

    dClearCachedData();
}

void PhysicsUpdate(PhysicsWorld* world, float dt)
{
    if (!sbDisableCollisionDetection)
    {
        world->Collide();
    }

    if (world == g_PhysicsWorld && !sbDisableCollisionDetection)
    {
        int ballFlags = dGeomGetGFlags(g_pBall->m_pPhysicsBall->m_geomID);

        if (!sbNonMovingAABBsInitialized)
        {
            ListEntry<PhysicsObject*>* entry = g_NetPhysicsObjects.m_Head;
            while (entry != 0)
            {
                dGeomComputeAABB(entry->entry->m_geomID);
                entry = entry->next;
            }
            sbNonMovingAABBsInitialized = true;
        }
        else
        {
            ListEntry<PhysicsObject*>* entry = g_NetPhysicsObjects.m_Head;
            while (entry != 0)
            {
                dGeomMarkAABBAsValid(entry->entry->m_geomID);
                entry = entry->next;
            }
        }

        PhysicsBall* physicsBall = g_pBall->m_pPhysicsBall;
        if (physicsBall->m_bInsideNet)
        {
            dGeomComputeAABB(physicsBall->m_geomID);
            g_PhysicsWorld->DoCollisions(physicsBall, g_NetPhysicsObjects);
        }

        dGeomSetGFlags(g_pBall->m_pPhysicsBall->m_geomID, ballFlags);
    }

    world->PreUpdate();
    world->Update(dt, true);
    world->PostUpdate();

    if (world == g_PhysicsWorld)
    {
        fn_80144130(lbl_806E11F0);
    }
}
