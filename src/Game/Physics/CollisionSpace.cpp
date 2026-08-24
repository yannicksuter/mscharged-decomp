#include "Game/Physics/CollisionSpace.h"

#include "Game/Physics/PhysicsObject.h"

class SimpleCollisionSpace : public CollisionSpace
{
public:
    SimpleCollisionSpace(PhysicsWorld*, bool);
    virtual ~SimpleCollisionSpace() { }
};

CollisionSpace::~CollisionSpace()
{
    dSpaceDestroy(m_spaceID);
}

void CollisionSpace::DoCollide(void* data, dNearCallback* callback)
{
    dSpaceCollide(m_spaceID, data, callback);
}

void CollisionSpace::CallPreCollide(PhysicsObject* object)
{
    object->PreCollide();
}

void CollisionSpace::PreCollide()
{
    void (CollisionSpace::*callback)(PhysicsObject*) = &CollisionSpace::CallPreCollide;
    int numGeoms = dSpaceGetNumGeoms(m_spaceID);

    for (int i = 0; i < numGeoms; i++)
    {
        dGeomID geom = dSpaceGetGeom(m_spaceID, i);
        int geomClass = dGeomGetClass(geom);

        if (geomClass != dRayClass && geomClass != dSimpleSpaceClass && geomClass != dHashSpaceClass)
        {
            (this->*callback)((PhysicsObject*)dGeomGetData(geom));
        }
    }
}

void CollisionSpace::PreUpdate()
{
}

SimpleCollisionSpace::SimpleCollisionSpace(PhysicsWorld* physicsWorld, bool addToWorld)
    : CollisionSpace(physicsWorld, addToWorld)
{
    m_spaceID = dSimpleSpaceCreate(0);
}

void CollisionSpace::CallLogGeoms(PhysicsObject* object)
{
    m_physicsWorld->LogGeom(object);
}

void CollisionSpace::SyncLogSpace()
{
    void (CollisionSpace::*callback)(PhysicsObject*) = &CollisionSpace::CallLogGeoms;
    int numGeoms = dSpaceGetNumGeoms(m_spaceID);

    for (int i = 0; i < numGeoms; i++)
    {
        dGeomID geom = dSpaceGetGeom(m_spaceID, i);
        int geomClass = dGeomGetClass(geom);

        if (geomClass != dRayClass && geomClass != dSimpleSpaceClass && geomClass != dHashSpaceClass)
        {
            (this->*callback)((PhysicsObject*)dGeomGetData(geom));
        }
    }
}
