#ifndef GAME_PHYSICS_PHYSICS_WALUIGI_WALL_H
#define GAME_PHYSICS_PHYSICS_WALUIGI_WALL_H

#include "Game/EventConnection.h"
#include "Game/Physics/PhysicsBox.h"
#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"

class cFielder;
class EmissionController;

class AvoidablePolygon;

extern float gWaluigiWallWidth;
extern unsigned int gWaluigiWallNextID;

class PhysicsWaluigiWall : public PhysicsBox
{
public:
    PhysicsWaluigiWall(cFielder* owner, float width, float height);
    virtual ~PhysicsWaluigiWall();

    virtual int GetObjectType() const { return 0x1D; }
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    AvoidablePolygon* GetAvoidablePolygon() const { return mAvoidable; }
    const nlVector3& GetStartPoint() const { return mStartPoint; }
    const nlVector3& GetEndPoint() const { return mEndPoint; }
    unsigned int GetID() const { return mID; }
    void SetEndPoint(const nlVector3& end);
    void Initialize(const nlMatrix3& rotation, float height);
    void ApplyDamage(float damage);
    void Shrink(float dt);
    ContactType FielderContact(cFielder* player);

    static void* operator new(unsigned long)
    {
        PhysicsWaluigiWall* pObject = 0;
        pool.Allocate(pObject);
        return pObject;
    }

    static void operator delete(void* pObject)
    {
        pool.Free((PhysicsWaluigiWall*)pObject);
    }

    static SlotPool<PhysicsWaluigiWall> pool;

    /* 0x38 */ nlVector3 mStartPoint;
    /* 0x44 */ nlVector3 mEndPoint;
    /* 0x50 */ cFielder* mOwner;
    /* 0x54 */ EmissionController* mEmitter;
    /* 0x58 */ float mHealth;
    /* 0x5C */ float mAge;
    /* 0x60 */ unsigned int mID;
    /* 0x64 */ bool mDestroy;
    /* 0x65 */ unsigned char mPadding065[3];
    /* 0x68 */ AvoidablePolygon* mAvoidable;
}; // size: 0x6C

class WaluigiWallManager
{
public:
    WaluigiWallManager();
    ~WaluigiWallManager();

    PhysicsWaluigiWall* CreateWall(cFielder* owner, float width, float height);
    void ClearWalls();
    void Update(float dt);
    void EndWall();
    PhysicsWaluigiWall* GetWall(int index);
    PhysicsWaluigiWall* FindWall(unsigned int id);

    /* 0x00 */ PhysicsWaluigiWall* mWalls[20];
    /* 0x50 */ PhysicsWaluigiWall* mCurrentWall;
    /* 0x54 */ PhysicsWaluigiWall* mPreviousWall;
    /* 0x58 */ cFielder* mOwner;
    /* 0x5C */ bool mEmitterStarted;
    /* 0x5D */ unsigned char mPadding05D[3];
    /* 0x60 */ float mHeightUpdateDelay;
    /* 0x64 */ UnidentifiedEventConnectionOwner mStartConnection;
    /* 0x68 */ UnidentifiedEventConnectionOwner mEndConnection;
    /* 0x6C */ UnidentifiedEventConnectionOwner mAbortConnection;
    /* 0x70 */ UnidentifiedEventConnectionOwner mMegastrikeConnection;
}; // size: 0x74

#endif // GAME_PHYSICS_PHYSICS_WALUIGI_WALL_H
