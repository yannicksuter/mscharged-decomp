#include "Game/Physics/PhysicsWorld.h"

#include "Game/DebugWriteCache.h"
#include "Game/Physics/CollisionSpace.h"
#include "Game/Physics/PhysicsObject.h"
#include "ode/NLGAdditions.h"

extern "C" void fn_80358B08(dBodyID, void*, DebugWriteCache*);
extern "C" void* memset(void*, int, unsigned long);

PhysicsWorld::PhysicsWorld()
{
    m_World = dWorldCreate();
    m_ContactGroup = dJointGroupCreate(0);
    m_SpaceList = 0;
    m_SyncLogContext = 0;
    m_SyncLogCache = 0;
    dWorldSetAutoDisableFlag(m_World, 0);
}

PhysicsWorld::~PhysicsWorld()
{
    dJointGroupDestroy(m_ContactGroup);
    dWorldDestroy(m_World);
}

void PhysicsWorld::SetCFM(float cfm)
{
    dWorldSetCFM(m_World, cfm);
}

void PhysicsWorld::SetERP(float erp)
{
    dWorldSetERP(m_World, erp);
}

void PhysicsWorld::AddCollisionSpace(CollisionSpace* collisionSpace)
{
    collisionSpace->m_nextCollisionSpace = m_SpaceList;
    m_SpaceList = collisionSpace;
}

void PhysicsWorld::PreCollide(CollisionSpace* collisionSpace)
{
    collisionSpace->PreCollide();
}

void PhysicsWorld::PreCollide(PhysicsObject* object)
{
    object->PreCollide();
}

void PhysicsWorld::PreUpdate(CollisionSpace* collisionSpace)
{
    collisionSpace->PreUpdate();
}

void PhysicsWorld::PreUpdate(PhysicsObject* object)
{
    object->PreUpdate();
}

void PhysicsWorld::PostUpdate(PhysicsObject* object)
{
    object->PostUpdate();
}

void PhysicsWorld::DoCollide(CollisionSpace* collisionSpace)
{
    collisionSpace->DoCollide(this, SpaceCollideCallback);
}

extern PhysicsWorld::PhysicsObjectCallback s_PreCollideObjectCallback;
extern PhysicsWorld::CollisionSpaceCallback s_PreCollideSpaceCallback;
extern PhysicsWorld::CollisionSpaceCallback s_DoCollideCallback;
extern PhysicsWorld::CollisionSpaceCallback s_PreUpdateSpaceCallback;
extern PhysicsWorld::PhysicsObjectCallback s_PreUpdateObjectCallback;
extern PhysicsWorld::PhysicsObjectCallback s_PostUpdateObjectCallback;

void PhysicsWorld::Collide()
{
    PhysicsObjectCallback bodyCallback = s_PreCollideObjectCallback;
    for (dBodyID body = dWorldGetFirstBody(m_World); body != 0; body = dBodyGetNextBody(body))
    {
        (this->*bodyCallback)((PhysicsObject*)dBodyGetData(body));
    }

    CollisionSpaceCallback spaceCallback = s_PreCollideSpaceCallback;
    for (CollisionSpace* space = m_SpaceList; space != 0; space = space->m_nextCollisionSpace)
    {
        (this->*spaceCallback)(space);
    }

    CollisionSpaceCallback collideCallback = s_DoCollideCallback;
    for (CollisionSpace* space = m_SpaceList; space != 0; space = space->m_nextCollisionSpace)
    {
        (this->*collideCallback)(space);
    }
}

void PhysicsWorld::PreUpdate()
{
    CollisionSpaceCallback spaceCallback = s_PreUpdateSpaceCallback;
    for (CollisionSpace* space = m_SpaceList; space != 0; space = space->m_nextCollisionSpace)
    {
        (this->*spaceCallback)(space);
    }

    PhysicsObjectCallback bodyCallback = s_PreUpdateObjectCallback;
    for (dBodyID body = dWorldGetFirstBody(m_World); body != 0; body = dBodyGetNextBody(body))
    {
        (this->*bodyCallback)((PhysicsObject*)dBodyGetData(body));
    }
}

void PhysicsWorld::PostUpdate()
{
    PhysicsObjectCallback bodyCallback = s_PostUpdateObjectCallback;
    for (dBodyID body = dWorldGetFirstBody(m_World); body != 0; body = dBodyGetNextBody(body))
    {
        (this->*bodyCallback)((PhysicsObject*)dBodyGetData(body));
    }
}

void PhysicsWorld::Update(float quickStepSize, bool clearAccumulators)
{
    dWorldSetClearAccumulators(m_World, clearAccumulators ? 1 : 0);
    dWorldQuickStep(m_World, quickStepSize);
    dJointGroupEmpty(m_ContactGroup);
}

void PhysicsWorld::SpaceCollideCallback(void* data, dxGeom* geom1, dxGeom* geom2)
{
    if (dGeomIsSpace(geom1) || dGeomIsSpace(geom2))
    {
        dSpaceCollide2(geom1, geom2, data, SpaceCollideCallback);
        if (dGeomIsSpace(geom1))
        {
            dSpaceCollide((dSpaceID)geom1, data, SpaceCollideCallback);
        }
        if (dGeomIsSpace(geom2))
        {
            dSpaceCollide((dSpaceID)geom2, data, SpaceCollideCallback);
        }
        return;
    }

    dContact* contact;
    dContact contacts[20];
    dBodyID body1;

    body1 = dGeomGetBody(geom1);
    dBodyID body2 = dGeomGetBody(geom2);

    if (body1 != 0 && body2 != 0)
    {
        if (dAreConnectedExcluding(body1, body2, 4) != 0)
        {
            return;
        }
    }

    PhysicsObject* object1 = (PhysicsObject*)dGeomGetData(geom1);
    PhysicsObject* object2 = (PhysicsObject*)dGeomGetData(geom2);

    bool canCollide1 = object1->SetContactInfo(&contacts[0], object2, true);
    bool canCollide2 = object2->SetContactInfo(&contacts[0], object1, false);
    if (!canCollide1 || !canCollide2)
    {
        return;
    }

    int numContacts = dCollide(geom1, geom2, 20, (dContactGeom*)((unsigned char*)contacts + 0x2C), sizeof(dContact));
    if (numContacts <= 0)
    {
        return;
    }

    ContactType result1 = object1->Contact(object2, contacts, numContacts);
    ContactType result2 = object2->Contact(object1, contacts, numContacts);

    ContactType transformedResult2;
    if (result2 == ONE_WAY_CONTACT_OTHER)
    {
        transformedResult2 = ONE_WAY_CONTACT_THIS;
    }
    else if (result2 == ONE_WAY_CONTACT_THIS)
    {
        transformedResult2 = ONE_WAY_CONTACT_OTHER;
    }
    else
    {
        transformedResult2 = result2;
    }

    int combinedResult = result1 & transformedResult2;
    if (combinedResult == 0)
    {
        return;
    }
    if (combinedResult == ONE_WAY_CONTACT_OTHER)
    {
        body1 = 0;
    }
    if (combinedResult == ONE_WAY_CONTACT_THIS)
    {
        body2 = 0;
    }

    int i;
    contact = &contacts[0];
    i = 0;
    goto checkContact;

processContact:
    if (i != 0)
    {
        contact->surface.mode = contacts[0].surface.mode;
        contact->surface.mu = contacts[0].surface.mu;
        contact->surface.mu2 = contacts[0].surface.mu2;
        contact->surface.bounce = contacts[0].surface.bounce;
        contact->surface.bounce_vel = contacts[0].surface.bounce_vel;
        contact->surface.soft_erp = contacts[0].surface.soft_erp;
        contact->surface.soft_cfm = contacts[0].surface.soft_cfm;
        contact->surface.motion1 = contacts[0].surface.motion1;
        contact->surface.motion2 = contacts[0].surface.motion2;
        contact->surface.slip1 = contacts[0].surface.slip1;
        contact->surface.slip2 = contacts[0].surface.slip2;
    }

    PhysicsWorld* world = (PhysicsWorld*)data;
    dJointID joint = dJointCreateContact(world->m_World, world->m_ContactGroup, contact);
    dJointAttach(joint, body1, body2);
    contact++;
    i++;

checkContact:
    if (i < numContacts)
    {
        goto processContact;
    }
}

void PhysicsWorld::LogBody(PhysicsObject* object)
{
    fn_80358B08(object->m_bodyID, m_SyncLogContext, m_SyncLogCache);
    object->SyncLog(m_SyncLogContext, m_SyncLogCache);
}

void PhysicsWorld::LogSpace(CollisionSpace* collisionSpace)
{
    collisionSpace->SyncLogSpace();
}

extern PhysicsWorld::PhysicsObjectCallback s_LogBodyCallback;
extern PhysicsWorld::CollisionSpaceCallback s_LogSpaceCallback;

struct GenGeomTypeState
{
    unsigned short type;
    unsigned short padding;
};

extern GenGeomTypeState s_GenGeomType;
extern char s_CollideBitsName[12];
extern char s_CategoryBitsName[12];
extern char s_GeomFlagsName[9];
extern char s_RotationName[4];
extern char s_PositionName[8];
extern char s_TypeName[8];
extern char s_GenGeomName[8];

void PhysicsWorld::LogGeom(PhysicsObject* object)
{
    void* context;
    DebugWriteCache* cache;

    struct GenGeom
    {
        int m_type;
        int m_gflags;
        dVector3 m_pos;
        dMatrix3 m_R;
        unsigned long m_catBits;
        unsigned long m_collBits;
    } geomData;

    dGeomID geom = object->m_geomID;
    geomData.m_type = dGeomGetClass(geom);
    geomData.m_gflags = dGeomGetGFlags(geom);

    if (!dGeomIsSpace(geom) && dGeomIsPlaceable(geom))
    {
        dGeomGetPosition(geom, geomData.m_pos);
        dGeomGetRotation(geom, geomData.m_R);
        geomData.m_catBits = dGeomGetCategoryBits(geom);
        geomData.m_collBits = dGeomGetCollideBits(geom);
    }
    else
    {
        memset(geomData.m_pos, 0, sizeof(geomData.m_pos));
        memset(geomData.m_R, 0, sizeof(geomData.m_R));
        geomData.m_catBits = 0;
        geomData.m_collBits = 0;
    }

    cache = m_SyncLogCache;
    context = m_SyncLogContext;
    if (s_GenGeomType.type == 0xFFFF)
    {
        s_GenGeomType.type = fn_80338EBC(cache, s_GenGeomName);
        fn_80338F88(cache, 8, lbl_80533C98[8].size, 0, s_TypeName);
        fn_80338F88(cache, 8, lbl_80533C98[8].size, (unsigned char*)&geomData.m_gflags - (unsigned char*)&geomData, s_GeomFlagsName);
        fn_80338F88(cache, 27, lbl_80533C98[27].size, (unsigned char*)&geomData.m_pos - (unsigned char*)&geomData, s_PositionName);
        fn_80338F88(cache, 30, lbl_80533C98[30].size, (unsigned char*)&geomData.m_R - (unsigned char*)&geomData, s_RotationName);
        fn_80338F88(cache, 13, lbl_80533C98[13].size, (unsigned char*)&geomData.m_catBits - (unsigned char*)&geomData, s_CategoryBitsName);
        fn_80338F88(cache, 13, lbl_80533C98[13].size, (unsigned char*)&geomData.m_collBits - (unsigned char*)&geomData, s_CollideBitsName);
        fn_80338F78(cache);
    }

    fn_80339450(cache, s_GenGeomType.type, &geomData, context);
    fn_8033930C(cache, s_GenGeomType.type, &geomData, sizeof(geomData));
}

void PhysicsWorld::SyncLog(void* context, DebugWriteCache* cache)
{
    m_SyncLogContext = context;
    m_SyncLogCache = cache;

    PhysicsObjectCallback bodyCallback = s_LogBodyCallback;
    for (dBodyID body = dWorldGetFirstBody(m_World); body != 0; body = dBodyGetNextBody(body))
    {
        (this->*bodyCallback)((PhysicsObject*)dBodyGetData(body));
    }

    CollisionSpaceCallback spaceCallback = s_LogSpaceCallback;
    for (CollisionSpace* space = m_SpaceList; space != 0; space = space->m_nextCollisionSpace)
    {
        (this->*spaceCallback)(space);
    }
}

void PhysicsWorld::DoCollisions(
    PhysicsObject* object, nlListContainer<PhysicsObject*>& container)
{
    ListEntry<PhysicsObject*>* entry = container.m_Head;
    dGeomID geom = object->m_geomID;
    for (; entry != 0; entry = entry->next)
    {
        dGeomCollideAABBs(geom, entry->entry->m_geomID, this, SpaceCollideCallback);
    }
}

PhysicsWorld::PhysicsObjectCallback s_PreCollideObjectCallback = &PhysicsWorld::PreCollide;
PhysicsWorld::CollisionSpaceCallback s_PreCollideSpaceCallback = &PhysicsWorld::PreCollide;
PhysicsWorld::CollisionSpaceCallback s_DoCollideCallback = &PhysicsWorld::DoCollide;
PhysicsWorld::CollisionSpaceCallback s_PreUpdateSpaceCallback = &PhysicsWorld::PreUpdate;
PhysicsWorld::PhysicsObjectCallback s_PreUpdateObjectCallback = &PhysicsWorld::PreUpdate;
PhysicsWorld::PhysicsObjectCallback s_PostUpdateObjectCallback = &PhysicsWorld::PostUpdate;
PhysicsWorld::PhysicsObjectCallback s_LogBodyCallback = &PhysicsWorld::LogBody;
PhysicsWorld::CollisionSpaceCallback s_LogSpaceCallback = &PhysicsWorld::LogSpace;

GenGeomTypeState s_GenGeomType = { 0xFFFF, 0 };
char s_CollideBitsName[12] = "m_collBits";
char s_CategoryBitsName[12] = "m_catBits";
char s_GeomFlagsName[9] = "m_gflags";
char s_RotationName[4] = "m_R";
char s_PositionName[8] = "m_pos";
char s_TypeName[8] = "m_type";
char s_GenGeomName[8] = "GenGeom";
