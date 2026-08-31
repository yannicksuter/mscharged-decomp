#include "Game/Physics/PhysicsCharacterBase.h"

#include "Game/Physics/CharacterPhysicsElement.h"
#include "Game/Physics/CollisionSpace.h"
#include "Game/Physics/PhysicsWorld.h"
#include "Game/PoseAccumulator.h"
#include "Game/SHierarchy.h"
#include "NL/nlMemory.h"
#include "NL/platvmath.h"

void dBodySetUpdateMode(dBodyID, int, int);
dJointID dJointCreateCharacter(dWorldID, dJointGroupID);
void dJointSetCharacterNoMotionDirection(dJointID, float*);

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

struct BoneVolumeTypeState
{
    unsigned short type;
    unsigned short padding;
};

extern "C" DebugFieldType lbl_80533C98[];
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void fn_8033930C(DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(DebugWriteCache*, unsigned short, void*, void*);

extern BoneVolumeTypeState s_BoneVolumeType;
extern char s_BoneIDName[5];
extern char s_BoneVolumeName[8];
extern char s_PrevPositionName[15];
extern char s_TransformHandleName[18];
extern char s_TransformName[12];
extern char s_BoneIndexName[12];

PhysicsCharacterBase::PhysicsCharacterBase(
    CollisionSpace*, PhysicsWorld* world, float centreOfMassHeight)
    : PhysicsCompositeObject(world)
    , m_CentreOfMassHeight(centreOfMassHeight)
    , m_Unknown94(0)
{
    dBodySetUpdateMode(m_bodyID, 1, 0);
    dBodySetGravityMode(m_bodyID, 0);

    m_CharacterJoint = dJointCreateCharacter(world->m_World, 0);
    dJointAttach(m_CharacterJoint, m_bodyID, 0);

    float direction[3];
    direction[0] = 0.0f;
    direction[1] = 0.0f;
    direction[2] = 1.0f;
    dJointSetCharacterNoMotionDirection(m_CharacterJoint, direction);

    dBodySetAutoDisableFlag(m_bodyID, 0);
}

PhysicsCharacterBase::~PhysicsCharacterBase()
{
    dJointDestroy(m_CharacterJoint);

    ListEntry<PhysicsBoneVolume*>* entry = m_BoneVolumes.m_Head;
    while (entry != 0)
    {
        delete entry->entry;
        entry = entry->next;
    }
}

void PhysicsCharacterBase::SetCharacterPosition(const nlVector3& position)
{
    nlVector3 adjustedPosition;
    adjustedPosition.x = position.x;
    adjustedPosition.y = position.y;
    adjustedPosition.z = position.z + m_CentreOfMassHeight;
    SetPosition(adjustedPosition, WORLD_COORDINATES);
}

static inline PhysicsCharacterBase* GetRootCharacter(PhysicsObject* object)
{
    PhysicsObject* root = object;
    int type = root->GetObjectType();

    if (type == 0xE || type == 0xD)
    {
        PhysicsObject* parent;
        while ((parent = root->m_parentObject) != 0)
        {
            root = parent;
        }
    }

    return root->GetObjectType() == 8 ? (PhysicsCharacterBase*)root : 0;
}

bool PhysicsCharacterBase::BaseSetContactInfo(
    dContact* contact, PhysicsObject* other, bool first)
{
    int objectType = other->GetObjectType();

    if (first)
    {
        SetDefaultContactInfo(contact);
        PhysicsCharacterBase* owner = GetRootCharacter(other);
        if (owner != 0)
        {
            contact->surface.mode &= ~dContactBounce;
            contact->surface.bounce = 0.0f;
            contact->surface.bounce_vel = 0.0f;
        }
    }

    contact->surface.mode |= dContactSoftERP | dContactSoftCFM;
    contact->surface.soft_erp = 0.2f;
    contact->surface.soft_cfm = 0.00001f;
    contact->surface.mu = 0.0f;

    if (objectType == 6 || objectType == 0xB)
    {
        contact->surface.mode &= ~dContactBounce;
        contact->surface.bounce = 0.0f;
        contact->surface.bounce_vel = 0.0f;
    }

    return true;
}

void PhysicsCharacterBase::BasePreCollide()
{
}

void PhysicsCharacterBase::ContainObject(PhysicsObject* object)
{
    if (object->m_parentObject != this)
    {
        m_SubObject.Attach(object, this);
    }
}

void PhysicsCharacterBase::ReleaseObject()
{
    m_SubObject.Release();
}

void PhysicsCharacterBase::SetFacingDirection(unsigned short angle)
{
    float radians = (float)angle * 0.0000958738f;
    nlMatrix4 rotation;
    nlMakeRotationMatrixZ(rotation, radians);
    SetRotation(rotation);
}

void PhysicsCharacterBase::Unknown0()
{
    PhysicsCompositeObject::Unknown0();
    for (ListEntry<PhysicsBoneVolume*>* entry = m_BoneVolumes.m_Head;
        entry != 0;
        entry = entry->next)
    {
        PhysicsBoneVolume* volume = entry->entry;
        volume->m_pObject->GetPosition(&volume->m_PrevPosition);
    }
}

void PhysicsCharacterBase::SyncLog(void* context, DebugWriteCache* cache)
{
    for (ListEntry<PhysicsBoneVolume*>* entry = m_BoneVolumes.m_Head;
        entry != 0;
        entry = entry->next)
    {
        PhysicsBoneVolume* volume = entry->entry;
        if (s_BoneVolumeType.type == 0xFFFF)
        {
            s_BoneVolumeType.type = fn_80338EBC(cache, s_BoneVolumeName);
            fn_80338F88(cache, 9, lbl_80533C98[9].size, (unsigned char*)&volume->m_BoneIndex - (unsigned char*)volume, s_BoneIndexName);
            fn_80338F88(cache, 26, lbl_80533C98[26].size, (unsigned char*)&volume->m_Transform - (unsigned char*)volume, s_TransformName);
            fn_80338F88(cache, 9, lbl_80533C98[9].size, (unsigned char*)&volume->m_TransformHandle - (unsigned char*)volume, s_TransformHandleName);
            fn_80338F88(cache, 22, lbl_80533C98[22].size, (unsigned char*)&volume->m_PrevPosition - (unsigned char*)volume, s_PrevPositionName);
            fn_80338F88(cache, 8, lbl_80533C98[8].size, (unsigned char*)&volume->m_ID - (unsigned char*)volume, s_BoneIDName);
            fn_80338F78(cache);
        }

        fn_80339450(cache, s_BoneVolumeType.type, volume, context);
        fn_8033930C(cache, s_BoneVolumeType.type, volume, sizeof(PhysicsBoneVolume));
    }
}

void PhysicsCharacterBase::UpdatePose(
    cPoseAccumulator* pose, float heightOffset, bool offsetCentreOfMass)
{
    for (ListEntry<PhysicsBoneVolume*>* entry = m_BoneVolumes.m_Head;
        entry != 0;
        entry = entry->next)
    {
        PhysicsBoneVolume* volume = entry->entry;
        volume->m_PrevPosition = volume->m_pObject->GetPosition();

        nlMatrix4 worldMatrix = pose->GetNodeMatrix(volume->m_BoneIndex);
        nlMultMatrices(worldMatrix, volume->m_Transform, pose->GetNodeMatrix(volume->m_BoneIndex));

        if (offsetCentreOfMass)
        {
            worldMatrix.m43 -= m_CentreOfMassHeight;
            worldMatrix.m43 += heightOffset;
        }

        nlMatrix4 normalizedMatrix = worldMatrix;
        for (int row = 0; row < 3; ++row)
        {
            nlVector3& matrixRow = *(nlVector3*)&normalizedMatrix.e[row * 4];
            nlVector3 basis;
            nlVec3Set(basis, matrixRow.x, matrixRow.y, matrixRow.z);
            nlVec3Scale(basis,
                nlRecipSqrt(nlVec3LengthSquared(basis), true));
            matrixRow.x = basis.x;
            matrixRow.y = basis.y;
            matrixRow.z = basis.z;
        }
        worldMatrix = normalizedMatrix;

        AdjustTransform(volume->m_TransformHandle, worldMatrix, offsetCentreOfMass != false);
    }
}

void PhysicsCharacterBase::GetBonePositions(
    PhysicsBoneID id, nlVector3& currentPosition, nlVector3& previousPosition)
{
    nlVec3Set(currentPosition, 0.0f, 0.0f, 0.0f);
    nlVec3Set(previousPosition, 0.0f, 0.0f, 0.0f);

    for (ListEntry<PhysicsBoneVolume*>* entry = m_BoneVolumes.m_Head;
        entry != 0;
        entry = entry->next)
    {
        PhysicsBoneVolume* volume = entry->entry;
        if (id == volume->m_ID)
        {
            currentPosition = volume->m_pObject->GetPosition();
            previousPosition = volume->m_PrevPosition;
        }
    }
}

inline PhysicsCharacterBase::PhysicsBoneVolume* PhysicsCharacterBase::AddBoneVolume(
    PhysicsObject* object, unsigned int boneIndex, unsigned int transformHandle,
    const nlMatrix4& transform, PhysicsBoneID id)
{
    return new (nlMalloc(sizeof(PhysicsBoneVolume), 8, false))
        PhysicsBoneVolume(object, boneIndex, transformHandle, transform, id);
}

void PhysicsCharacterBase::AddBoneVolumes(PhysicsWorld* world,
    CollisionSpace* collisionSpace, cPoseAccumulator* pose,
    const CharacterPhysicsData* data, unsigned long categoryBits,
    unsigned long collideBits)
{
    for (unsigned int i = 0; i < data->m_ElementCount; ++i)
    {
        CharacterPhysicsElement* element = &data->m_Elements[i];
        PhysicsObject* object = 0;

        switch (element->m_PrimitiveType)
        {
        case 1:
            object = new (nlMalloc(sizeof(PhysicsSphereBone), 8, false))
                PhysicsSphereBone(collisionSpace, world, element->m_Radius);
            break;
        case 2:
            object = new (nlMalloc(sizeof(PhysicsCapsuleBone), 8, false))
                PhysicsCapsuleBone(collisionSpace, world, element->m_Radius, element->m_Height);
            break;
        case 3:
            object = new (nlMalloc(sizeof(PhysicsCylinderBone), 8, false))
                PhysicsCylinderBone(collisionSpace, world, element->m_Radius, element->m_Height);
            break;
        default:
            continue;
        }

        object->SetCategory(categoryBits);
        object->SetCollide(collideBits);

        unsigned int parentNodeIndex = pose->m_pHierarchy->GetNodeIndexByID(element->m_ParentHashID);
        int transformHandle = AddObject(object);
        PhysicsBoneID id = ResolvePhysicsBoneIDFromName(element->m_Name);
        PhysicsBoneVolume* volume = AddBoneVolume(object, parentNodeIndex, transformHandle, element->m_LocalToParent, id);
        m_BoneVolumes.AddEntry(volume);

        switch (element->m_PrimitiveType)
        {
        case 1:
            ((PhysicsSphereBone*)object)->m_pBoneVolume = volume;
            break;
        case 2:
            ((PhysicsCapsuleBone*)object)->m_pBoneVolume = volume;
            break;
        case 3:
            ((PhysicsCylinderBone*)object)->m_pBoneVolume = volume;
            break;
        }
    }
}

PhysicsBoneID PhysicsCharacterBase::GetBoneIDForSubObject(
    const PhysicsObject* object) const
{
    switch (object->GetObjectType())
    {
    case 0xD:
    {
        const PhysicsSphereBone* bone = (const PhysicsSphereBone*)object;
        if (bone->m_pBoneVolume != 0)
        {
            return bone->m_pBoneVolume->m_ID;
        }
        break;
    }
    case 0xE:
    {
        const PhysicsCapsuleBone* bone = (const PhysicsCapsuleBone*)object;
        if (bone->m_pBoneVolume != 0)
        {
            return bone->m_pBoneVolume->m_ID;
        }
        break;
    }
    case 0xF:
    {
        const PhysicsCylinderBone* bone = (const PhysicsCylinderBone*)object;
        if (bone->m_pBoneVolume != 0)
        {
            return bone->m_pBoneVolume->m_ID;
        }
        break;
    }
    }
    return PHYSBONE_SINGLE_CAPSULE;
}

void PhysicsCharacterBase::SetBoneVolumeScale(float scale)
{
    PhysicsBoneVolume* volume;
    ListEntry<PhysicsBoneVolume*>* entry = m_BoneVolumes.m_Head;
    while (entry != 0)
    {
        volume = entry->entry;
        switch (volume->m_pObject->GetObjectType())
        {
        case 0xD:
        {
            ((PhysicsSphereBone*)volume->m_pObject)
                ->SetRadius(scale * ((PhysicsSphereBone*)volume->m_pObject)->m_Radius);
            break;
        }
        case 0xE:
        {
            PhysicsCapsuleBone* capsule = (PhysicsCapsuleBone*)volume->m_pObject;
            capsule->SetRadius(scale * capsule->m_Radius);
            capsule->SetLength(scale * capsule->m_Length);
            break;
        }
        case 0xF:
        {
            PhysicsCylinderBone* cylinder = (PhysicsCylinderBone*)volume->m_pObject;
            cylinder->SetRadius(scale * cylinder->m_Radius);
            cylinder->SetLength(scale * cylinder->m_Length);
            break;
        }
        }
        entry = entry->next;
    }
}

template <>
void ListContainerBase<PhysicsCharacterBase::PhysicsBoneVolume*,
    NewAdapter<ListEntry<PhysicsCharacterBase::PhysicsBoneVolume*> > >::DeleteEntry(ListEntry<PhysicsCharacterBase::PhysicsBoneVolume*>* entry)
{
    NewAdapter<ListEntry<PhysicsCharacterBase::PhysicsBoneVolume*> >::DeleteEntry(
        entry);
}

PhysicsCharacterBase::PhysicsSphereBone::~PhysicsSphereBone()
{
}

PhysicsCharacterBase::PhysicsCapsuleBone::~PhysicsCapsuleBone()
{
}

PhysicsCharacterBase::PhysicsCylinderBone::~PhysicsCylinderBone()
{
}

int PhysicsCharacterBase::GetObjectType() const
{
    return 8;
}

int PhysicsCharacterBase::PhysicsCylinderBone::GetObjectType() const
{
    return 0xF;
}

int PhysicsCharacterBase::PhysicsCapsuleBone::GetObjectType() const
{
    return 0xE;
}

int PhysicsCharacterBase::PhysicsSphereBone::GetObjectType() const
{
    return 0xD;
}

BoneVolumeTypeState s_BoneVolumeType = { 0xFFFF, 0 };
char s_BoneIDName[5] = "m_ID";
char s_BoneVolumeName[8] = "BoneVol";
