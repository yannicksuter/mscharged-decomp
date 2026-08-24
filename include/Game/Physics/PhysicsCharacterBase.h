#ifndef GAME_PHYSICS_PHYSICS_CHARACTER_BASE_H
#define GAME_PHYSICS_PHYSICS_CHARACTER_BASE_H

#include "Game/Physics/PhysicsCapsule.h"
#include "Game/Physics/PhysicsCompositeObject.h"
#include "Game/Physics/PhysicsCylinder.h"
#include "Game/Physics/PhysicsSphere.h"
#include "Game/Physics/PhysicsTransform.h"
#include "NL/nlList.h"

class CharacterPhysicsData;
class CollisionSpace;
class cPoseAccumulator;

enum PhysicsBoneID
{
    PHYSBONE_SINGLE_CAPSULE = 0,
    PHYSBONE_FIELDER_R_ARM = 1,
    PHYSBONE_FIELDER_L_ARM = 2,
    PHYSBONE_FIELDER_R_LEG = 3,
    PHYSBONE_FIELDER_L_LEG = 4,
    PHYSBONE_FIELDER_HEAD = 5,
    PHYSBONE_GOALIE_R_SHOULDER = 6,
    PHYSBONE_GOALIE_R_BICEP = 7,
    PHYSBONE_GOALIE_R_FOREARM = 8,
    PHYSBONE_GOALIE_R_WRIST = 9,
    PHYSBONE_GOALIE_R_HAND = 10,
    PHYSBONE_GOALIE_R_THIGH = 11,
    PHYSBONE_GOALIE_R_THIGHLOWER = 12,
    PHYSBONE_GOALIE_R_CALFUPPER = 13,
    PHYSBONE_GOALIE_R_HEEL = 14,
    PHYSBONE_GOALIE_R_TOE = 15,
    PHYSBONE_GOALIE_L_SHOULDER = 16,
    PHYSBONE_GOALIE_L_BICEP = 17,
    PHYSBONE_GOALIE_L_FOREARM = 18,
    PHYSBONE_GOALIE_L_WRIST = 19,
    PHYSBONE_GOALIE_L_HAND = 20,
    PHYSBONE_GOALIE_L_THIGH = 21,
    PHYSBONE_GOALIE_L_THIGHLOWER = 22,
    PHYSBONE_GOALIE_L_CALFUPPER = 23,
    PHYSBONE_GOALIE_L_HEEL = 24,
    PHYSBONE_GOALIE_L_TOE = 25,
    PHYSBONE_GOALIE_HEAD = 26,
    PHYSBONE_GOALIE_STOMACH = 27,
    NUM_PHYSBONES = 28,
    PHYSBONE_UNKNOWN = 29,
    PHYSBONE_ID_INVALID = 29,
};

class PhysicsCharacterBase : public PhysicsCompositeObject
{
public:
    class PhysicsBoneVolume
    {
    public:
        PhysicsBoneVolume(PhysicsObject* object, unsigned int boneIndex,
            unsigned int transformHandle, const nlMatrix4& transform,
            PhysicsBoneID id)
            : m_pObject(object)
            , m_BoneIndex(boneIndex)
            , m_Transform(transform)
            , m_TransformHandle(transformHandle)
            , m_ID(id)
        {
            m_pObject->GetPosition(&m_PrevPosition);
        }

        /* 0x00 */ PhysicsObject* m_pObject;
        /* 0x04 */ unsigned int m_BoneIndex;
        /* 0x08 */ nlMatrix4 m_Transform;
        /* 0x48 */ unsigned int m_TransformHandle;
        /* 0x4C */ nlVector3 m_PrevPosition;
        /* 0x58 */ PhysicsBoneID m_ID;
    }; // size: 0x5C

    PhysicsCharacterBase(CollisionSpace*, PhysicsWorld*, float);
    virtual ~PhysicsCharacterBase();

    virtual void Unknown0();
    virtual int GetObjectType() const;
    virtual void SyncLog(void*, DebugWriteCache*);
    virtual void UpdatePose(cPoseAccumulator*, float, bool);
    virtual PhysicsBoneID ResolvePhysicsBoneIDFromName(const char*) = 0;

    class PhysicsSphereBone : public PhysicsSphere
    {
    public:
        PhysicsSphereBone(CollisionSpace* space, PhysicsWorld* world,
            float radius)
            : PhysicsSphere(space, world, radius)
            , m_pBoneVolume(0)
            , m_Radius(radius)
        {
        }

        virtual ~PhysicsSphereBone();
        virtual int GetObjectType() const;

        /* 0x38 */ PhysicsBoneVolume* m_pBoneVolume;
        /* 0x3C */ float m_Radius;
    }; // size: 0x40

    class PhysicsCapsuleBone : public PhysicsCapsule
    {
    public:
        PhysicsCapsuleBone(CollisionSpace* space, PhysicsWorld* world,
            float radius, float length)
            : PhysicsCapsule(space, world, radius, length)
            , m_pBoneVolume(0)
            , m_Radius(radius)
            , m_Length(length)
        {
        }

        virtual ~PhysicsCapsuleBone();
        virtual int GetObjectType() const;

        /* 0x38 */ PhysicsBoneVolume* m_pBoneVolume;
        /* 0x3C */ float m_Radius;
        /* 0x40 */ float m_Length;
    }; // size: 0x44

    class PhysicsCylinderBone : public PhysicsCylinder
    {
    public:
        PhysicsCylinderBone(CollisionSpace* space, PhysicsWorld* world,
            float radius, float length)
            : PhysicsCylinder(space, world, radius, length)
            , m_pBoneVolume(0)
            , m_Length(length)
            , m_Radius(radius)
        {
        }

        virtual ~PhysicsCylinderBone();
        virtual int GetObjectType() const;

        /* 0x38 */ PhysicsBoneVolume* m_pBoneVolume;
        /* 0x3C */ float m_Length;
        /* 0x40 */ float m_Radius;
    }; // size: 0x44

    void SetCharacterPosition(const nlVector3&);
    bool BaseSetContactInfo(dContact*, PhysicsObject*, bool);
    void BasePreCollide();
    void ContainObject(PhysicsObject*);
    void ReleaseObject();
    void SetFacingDirection(unsigned short);
    void GetBonePositions(PhysicsBoneID, nlVector3&, nlVector3&);
    void AddBoneVolumes(PhysicsWorld*, CollisionSpace*, cPoseAccumulator*,
        const CharacterPhysicsData*, unsigned long, unsigned long);
    PhysicsBoneID GetBoneIDForSubObject(const PhysicsObject*) const;
    void SetBoneVolumeScale(float);

    PhysicsBoneVolume* AddBoneVolume(PhysicsObject*, unsigned int,
        unsigned int, const nlMatrix4&, PhysicsBoneID);

    /* 0x44 */ float m_CentreOfMassHeight;
    /* 0x48 */ dJointID m_CharacterJoint;
    /* 0x4C */ PhysicsTransform m_SubObject;
    /* 0x88 */ nlListContainer<PhysicsBoneVolume*> m_BoneVolumes;
    /* 0x94 */ unsigned int m_Unknown94;
}; // size: 0x98

template <>
void ListContainerBase<PhysicsCharacterBase::PhysicsBoneVolume*,
    NewAdapter<ListEntry<PhysicsCharacterBase::PhysicsBoneVolume*> > >::DeleteEntry(ListEntry<PhysicsCharacterBase::PhysicsBoneVolume*>*);

#endif
