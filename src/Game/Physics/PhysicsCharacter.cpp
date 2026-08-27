#include "Game/Physics/PhysicsCharacter.h"

#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/GameInfo.h"
#include "Game/Physics/CollisionSpace.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Physics/PhysicsColumn.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Player.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "math.h"
#include "types.h"

extern PhysicsWorld* g_PhysicsWorld;

extern "C" void fn_8013F854(const char*, ...);
extern "C" int strcmpi(const char*, const char*);
extern "C" bool fn_800344DC(cFielder*, const nlVector3*);
extern "C" bool fn_800345EC(cFielder*, cFielder*);
extern "C" bool fn_80034894(cFielder*, cFielder*);
extern "C" bool fn_8003886C(cFielder*);
extern "C" bool fn_80038918(cFielder*);
extern "C" bool fn_8003E74C(cCharacter*);
extern "C" bool fn_8003E948(cFielder*);
extern "C" bool fn_800976F8(cFielder*, float);
extern void* lbl_806E1608;

struct CollisionPlayerBallData
{
    cPlayer* pPlayer;
    cBall* pBall;
    nlVector3 velocity;
    PhysicsBoneID boneID;
};

struct CollisionPlayerPlayerData
{
    cPlayer* player1;
    cPlayer* player2;
    nlVector3 velocity1;
    nlVector3 velocity2;
};

extern SlotPool<CollisionPlayerWallData> lbl_80571348;
extern SlotPool<CollisionPlayerBallData> lbl_805714D8;
extern SlotPool<CollisionPlayerPlayerData> lbl_80571258;
extern "C" void fn_80145F18(CollisionPlayerWallData*);
extern "C" void fn_801462DC(CollisionPlayerBallData*);
extern "C" void fn_80145DD0(CollisionPlayerPlayerData*);

static CollisionPlayerPlayerData* sPlayerPlayerCollisionData[100];
static bool sbDoDKBallStuckHack = true;
static float sfBallStuckHackShoveMagnitude = 10.0f;

inline void* operator new(unsigned long, void* memory)
{
    return memory;
}

static inline unsigned int& CharacterFlags(PhysicsCharacter* character)
{
    return *(unsigned int*)((char*)character + 0x98);
}

static inline int CharacterID(const cCharacter* character)
{
    return *(const int*)((const char*)character + 0x120);
}

static inline bool ReadBool(const void* object, unsigned int offset)
{
    return *(const bool*)((const char*)object + offset);
}

static inline unsigned int ReadU32(const void* object, unsigned int offset)
{
    return *(const unsigned int*)((const char*)object + offset);
}

static inline int ReadS32(const void* object, unsigned int offset)
{
    return *(const int*)((const char*)object + offset);
}

static inline void* ReadPointer(const void* object, unsigned int offset)
{
    return *(void* const*)((const char*)object + offset);
}

extern "C" void fn_80142A1C()
{
    for (int i = 0; i < 100; ++i)
    {
        sPlayerPlayerCollisionData[i] = 0;
    }
}

PhysicsCharacter::PhysicsCharacter(float radius, float heightScale)
    : PhysicsCharacterBase(
          g_CollisionSpace, g_PhysicsWorld, radius + heightScale / 2.0f)
{
    CharacterFlags(this) |= 0xE8000000;
    m_nDKBallStuckHackCounter = 0;
    m_bInsideNet = false;
    m_bWasInsideNet = false;
    m_pAICharacter = 0;

    m_gravity = 0.0f;
    SetMass(100.0f);

    PhysicsColumn* column = new (nlMalloc(sizeof(PhysicsColumn), 8, false))
        PhysicsColumn(g_CollisionSpace, g_PhysicsWorld, radius);
    m_pPlayerPlayerColumn = column;
    m_pPlayerPlayerColumn->SetCategory(0x40);
    m_pPlayerPlayerColumn->SetCollide(0x1F05F);
    AddObject(m_pPlayerPlayerColumn);
}

void PhysicsCharacter::Unknown0()
{
    unsigned int flags = CharacterFlags(this);
    m_nDKBallStuckHackCounter = 0;
    flags |= 0xE0000000;
    flags &= ~0x10000000;
    m_bSupportingBallThisFrame = false;
    flags |= 0x08000000;
    m_bInsideNet = false;
    CharacterFlags(this) = flags;
    m_bWasInsideNet = false;
    PhysicsCharacterBase::Unknown0();
}

void PhysicsCharacter::DisablePhysicsColumn()
{
    m_pPlayerPlayerColumn->DisableCollisions();
}

void PhysicsCharacter::EnablePhysicsColumn()
{
    m_pPlayerPlayerColumn->EnableCollisions();
}

void PhysicsCharacter::GetRadius(float* radius)
{
    m_pPlayerPlayerColumn->GetRadius(radius);
}

bool PhysicsCharacter::SetContactInfo(
    dContact* contact, PhysicsObject* other, bool first)
{
    bool result = BaseSetContactInfo(contact, other, first);
    int objectType = other->GetObjectType();
    if (objectType == 0x10 || objectType == 0x11)
    {
        contact->surface.bounce = 0.2f;
    }
    return result;
}

ContactType PhysicsCharacter::Contact(PhysicsObject* other,
    dContact* contacts, int numContacts, PhysicsObject* originalOther)
{
    int objectType = other->GetObjectType();
    fn_8013F854(
        "PhysChar Contact objID %d numContacts %d\n", objectType, numContacts);

    if (objectType == 0x13)
    {
        fn_8013F854("PhysChar PHYSOBJ_TRIGGER_VOLUME\n");
        return NO_CONTACT;
    }
    if (objectType == 0x07)
    {
        fn_8013F854("PhysChar PHYSOBJ_FINITEPLANE\n");
        return NO_CONTACT;
    }
    if (objectType == 0x1D)
    {
        if (m_pAICharacter->m_eClassType == FIELDER
            && !((cFielder*)m_pAICharacter)->mbTangible)
        {
            fn_8013F854("PhysChar PHYSOBJ_TRON_WALL IsTangible\n");
            return NO_CONTACT;
        }
        fn_8013F854("PhysChar PHYSOBJ_TRON_WALL\n");
        return ONE_WAY_CONTACT_THIS;
    }
    if (objectType == 0x1F && m_pAICharacter->m_eClassType == GOALIE)
    {
        void* hammer = *(void**)((char*)other + 0x38);
        bool onGround = *(float*)((char*)hammer + 0x48) > 0.0f;
        if (onGround)
        {
            fn_8013F854("PhysChar PHYSOBJ_HAMMER OnGround\n");
            int actionState = *(int*)((char*)m_pAICharacter + 0x328);
            if (actionState != 0x20 && actionState != 4)
            {
                fn_8013F854("PhysChar PHYSOBJ_HAMMER Goalie return\n");
                return ONE_WAY_CONTACT_THIS;
            }
        }
    }

    if (objectType == 0x17 || objectType == 5)
    {
        nlVector3 contactPosition;
        nlVec3Set(contactPosition, contacts->geom.pos[0], contacts->geom.pos[1], contacts->geom.pos[2]);
        bool sidelineCollision = fabsf(contactPosition.x) < cField::GetGoalLineX(1) - 1.0f;
        fn_8013F854("PhysChar WALL or corner %d conpos %f %f %f\n",
            sidelineCollision,
            contacts->geom.pos[0],
            contacts->geom.pos[1],
            contacts->geom.pos[2]);

        for (int i = 0; i < numContacts; ++i)
        {
            if (contacts[i].geom.normal[2] < 0.08f)
            {
                CollisionPlayerWallData* wallData = 0;
                lbl_80571348.Allocate(wallData);
                wallData->pPlayer = (cPlayer*)m_pAICharacter;
                wallData->contactPoint = contactPosition;
                nlVec3Set(wallData->wallNormal,
                    contacts->geom.normal[0],
                    contacts->geom.normal[1],
                    contacts->geom.normal[2]);
                fn_80145F18(wallData);
            }
        }

        if (!m_CanCollideWithWall)
        {
            fn_8013F854("!m_CanCollideWithWall\n");
            return NO_CONTACT;
        }

        if (sidelineCollision && objectType == 0x17
            && GameInfoManager::Instance()->GetStadium() == 0x0B
            && m_pAICharacter->m_eClassType == FIELDER)
        {
            cFielder* fielder = (cFielder*)m_pAICharacter;
            if (fielder->m_pBall == 0 || fielder->m_eActionState == 1
                || (fn_8003E948(fielder) && ReadBool(fielder, 0x3DC)))
            {
                int actionState = fielder->m_eActionState;
                bool superWall = fn_8003E948(fielder);
                fn_8013F854(
                    "bSidelineCollision action state %d superwal %d\n",
                    actionState,
                    superWall);
                return NO_CONTACT;
            }
        }

        float radius;
        m_pPlayerPlayerColumn->GetRadius(&radius);
        if (fabsf(contactPosition.x) > cField::GetGoalLineX(1) - radius
            && (m_pAICharacter->m_eClassType == FIELDER
                || !m_CanCollideWithGoalLine))
        {
            fn_8013F854(
                "Goal check radius %f x %f y %f GetCanCollideWithGoalLine %d\n",
                radius,
                contactPosition.x,
                contactPosition.y,
                m_CanCollideWithGoalLine);

            float netDepth = cNet::m_fNetDepth;
            float absoluteX = fabsf(contactPosition.x);
            if (absoluteX
                    >= netDepth + cField::GetGoalLineX(1) - radius
                || fabsf(contactPosition.y)
                       >= 0.5f * cNet::m_fNetWidth - radius
                || contactPosition.z
                       >= cNet::m_fNetHeight - cNet::m_fNetPostRadius)
            {
                fn_8013F854("UseClipping\n");
                if (m_bInsideNet)
                {
                    fn_8013F854("IsInsideNet\n");
                    m_bWasInsideNet = true;
                    return NO_CONTACT;
                }
            }
            else
            {
                fn_8013F854("SetInsideNet true\n");
                m_bInsideNet = true;
                cFielder* fielder = (cFielder*)m_pAICharacter;
                if (fielder->m_pBall != 0)
                    g_pBall->m_pPhysicsBall->m_bInsideNet = true;
                return NO_CONTACT;
            }
        }
        else
        {
            fn_8013F854("SetInsideNet false\n");
            m_bInsideNet = false;
            m_bWasInsideNet = false;
        }
    }

    if (objectType == 0x10)
    {
        if (!m_CanCollideWithBall)
        {
            fn_8013F854("PhysChar Ball !m_CanCollideWithBall\n");
            return NO_CONTACT;
        }
        if (m_pAICharacter->m_eClassType == FIELDER
            && !((cFielder*)m_pAICharacter)->mbTangible)
        {
            fn_8013F854("PhysChar Ball !IsTangible\n");
            return NO_CONTACT;
        }

        if (contacts->geom.normal[2] > contacts->geom.normal[0]
            && contacts->geom.normal[2] > contacts->geom.normal[1])
        {
            fn_8013F854("PhysChar Ball m_bSupportingBallThisFrame\n");
            m_bSupportingBallThisFrame = true;
        }

        PhysicsBall* physicsBall = (PhysicsBall*)other;
        cBall* ball = physicsBall->m_pBall;
        cCharacter* character = m_pAICharacter;
        if ((!physicsBall->m_bCollideWithFielders
                && character->m_eClassType == FIELDER)
            || (!physicsBall->m_bCollideWithGoalies
                && character->m_eClassType == GOALIE))
        {
            fn_8013F854("PhysChar Ball NoContactS2S\n");
            return NO_CONTACT;
        }

        if (other->m_parentObject == 0)
            FakeBallWorld::InvalidateBallCache();

        if (ball->GetOwnerFielder() != 0
            && m_pAICharacter->m_eClassType == FIELDER)
        {
            fn_8013F854("PhysChar Fielder\n");
            cFielder* fielder = (cFielder*)m_pAICharacter;
            if (fielder->IsFallenDown()
                && *(void**)((char*)fielder + 0x254) == 0)
            {
                fn_8013F854("PhysChar Fallen down not on fire\n");
                if (fn_800345EC(ball->GetOwnerFielder(), fielder)
                    || fielder->m_eAnimID == 0x76)
                {
                    fn_8013F854("PhysChar Electro1200\n");
                    return NO_CONTACT;
                }
                if (fn_8003E948(ball->GetOwnerFielder())
                    && ReadBool(ball->GetOwnerFielder(), 0x3DC))
                {
                    fn_8013F854("PhysChar SuperWal\n");
                    return ONE_WAY_CONTACT_THIS;
                }
                fn_8013F854("PhysChar Other\n");
                return ONE_WAY_CONTACT_OTHER;
            }

            bool invincible = !fn_8003886C(fielder)
                           && (fielder->muInvincibleStatus & 1) != 0;
            if (invincible)
            {
                fn_8013F854("PhysChar IsInvincibleChars\n");
                return ONE_WAY_CONTACT_THIS;
            }
            if (fn_800344DC(fielder, &ball->m_v3Position))
            {
                fn_8013F854("PhysChar IsInvincibleCharsDirect\n");
                return ONE_WAY_CONTACT_THIS;
            }
            if (fn_8003886C(fielder))
            {
                fn_8013F854("PhysChar IsStuck\n");
                return ONE_WAY_CONTACT_OTHER;
            }
            fn_8013F854("PhysChar Other2\n");
            return TWO_WAY_CONTACT;
        }

        fn_8013F854("PhysChar Not fielder\n");
        void* koopaShell = ReadPointer(lbl_806E1608, 0x2C);
        if (koopaShell != 0 && ReadBool(koopaShell, 0x20))
        {
            fn_8013F854("PhysChar KoopaShell->IsVisible\n");
            return NO_CONTACT;
        }
        void* egg = ReadPointer(lbl_806E1608, 0x28);
        if (egg != 0 && ReadBool(egg, 0x30))
        {
            fn_8013F854("PhysChar Egg->IsVisible\n");
            return NO_CONTACT;
        }

        if (!m_HasCollidedWithBall)
        {
            CollisionPlayerBallData* ballData = 0;
            lbl_805714D8.Allocate(ballData);
            ballData->pPlayer = (cPlayer*)m_pAICharacter;
            ballData->pBall = ball;
            ballData->velocity = other->GetLinearVelocity();
            ballData->boneID = GetBoneIDForSubObject(originalOther);
            m_HasCollidedWithBall = true;
            fn_801462DC(ballData);
        }

        fn_8013F854("PhysChar IncrementBallDeflectCount\n");
        ++ball->mUnidentified008;
        ++ball->m_bBallDeflectCount;
        return ONE_WAY_CONTACT_OTHER;
    }

    ContactType contactType = TWO_WAY_CONTACT;
    if (objectType == 4 || objectType == 0x0D || objectType == 0x0E)
    {
        if (!m_CanCollideWithCharacters)
            return NO_CONTACT;

        PhysicsCharacter* otherCharacter = (PhysicsCharacter*)other->m_parentObject;
        cCharacter* thisPlayer = m_pAICharacter;

        if (thisPlayer->m_eClassType == GOALIE
            && otherCharacter->m_pAICharacter->m_eClassType == FIELDER)
        {
            cCharacter* otherPlayer = otherCharacter->m_pAICharacter;
            float height = thisPlayer
                               ->GetJointPosition(ReadS32(thisPlayer, 0xD8))
                               .z
                         + 0.25f;
            if (fn_800976F8((cFielder*)otherPlayer, height))
                return NO_CONTACT;
        }
        else if (thisPlayer->m_eClassType == FIELDER
                 && otherCharacter->m_pAICharacter->m_eClassType == GOALIE)
        {
            cCharacter* otherPlayer = otherCharacter->m_pAICharacter;
            float height = otherPlayer
                               ->GetJointPosition(ReadS32(otherPlayer, 0xD8))
                               .z
                         + 0.25f;
            if (fn_800976F8((cFielder*)thisPlayer, height))
                return NO_CONTACT;
        }
        else if (thisPlayer->m_eClassType == FIELDER
                 && otherCharacter->m_pAICharacter->m_eClassType == FIELDER)
        {
            cCharacter* otherPlayer = otherCharacter->m_pAICharacter;
            cFielder* fielder = (cFielder*)thisPlayer;
            cFielder* otherFielder = (cFielder*)otherPlayer;

            if (fn_800345EC(fielder, otherFielder)
                || fn_800345EC(otherFielder, fielder))
            {
                return NO_CONTACT;
            }

            bool invincible = !fn_8003886C(fielder)
                           && (fielder->muInvincibleStatus & 1) != 0;
            if (invincible
                || fn_800344DC(fielder, &otherFielder->m_v3Position)
                || fn_8003886C(fielder)
                || (fn_8003E948(fielder) && ReadBool(fielder, 0x3DC)))
            {
                contactType = ONE_WAY_CONTACT_OTHER;
            }
            else
            {
                invincible = !fn_8003886C(otherFielder)
                          && (otherFielder->muInvincibleStatus & 1) != 0;
                if (invincible
                    || fn_800344DC(otherFielder, &fielder->m_v3Position)
                    || fn_8003886C(otherFielder))
                {
                    contactType = ONE_WAY_CONTACT_THIS;
                }
                else if (fn_80034894(fielder, otherFielder))
                {
                    contactType = ONE_WAY_CONTACT_OTHER;
                }
                else if (fn_80034894(otherFielder, fielder))
                {
                    contactType = ONE_WAY_CONTACT_THIS;
                }
                else if (fn_80038918(fielder)
                         && fn_80038918(otherFielder))
                {
                    contactType = NO_CONTACT;
                }
                else if (fielder->IsFallenDown()
                         && ReadPointer(fielder, 0x254) == 0)
                {
                    if (fielder->m_eAnimID == 0x76)
                        contactType = NO_CONTACT;
                    else if (fielder->m_eAnimID == 0x56)
                        contactType = NO_CONTACT;
                    else
                        contactType = ONE_WAY_CONTACT_THIS;
                }
                else if (otherFielder->IsFallenDown()
                         && ReadPointer(otherFielder, 0x254) == 0)
                {
                    if (otherFielder->m_eAnimID == 0x76)
                        contactType = NO_CONTACT;
                    else
                    {
                        contactType = ONE_WAY_CONTACT_OTHER;
                        if (otherFielder->m_eAnimID == 0x56)
                            contactType = NO_CONTACT;
                    }
                }
            }
        }

        cCharacter* collisionPlayer1 = m_pAICharacter;
        cCharacter* collisionPlayer2 = otherCharacter->m_pAICharacter;
        int id1 = CharacterID(collisionPlayer1);
        int id2 = CharacterID(collisionPlayer2);
        int collisionIndex = id1 * 10 + id2;
        if (sPlayerPlayerCollisionData[collisionIndex] == 0)
        {
            CollisionPlayerPlayerData* data = 0;
            lbl_80571258.Allocate(data);
            sPlayerPlayerCollisionData[collisionIndex] = data;
            data->player1 = (cPlayer*)collisionPlayer1;
            data->player2 = (cPlayer*)collisionPlayer2;
            data->velocity1 = m_pAICharacter->m_v3Velocity;
            data->velocity2 = otherCharacter->m_pAICharacter->m_v3Velocity;
            fn_80145DD0(data);
        }
    }
    return contactType;
}

void PhysicsCharacter::PreCollide()
{
    BasePreCollide();
    m_HasCollidedWithBall = false;

    nlVector3 force = { 0.0f, 0.0f, 0.0f };
    force.y = -sfBallStuckHackShoveMagnitude;
    if (sbDoDKBallStuckHack && m_nDKBallStuckHackCounter > 5)
    {
        g_pBall->m_pPhysicsBall->AddForceAtCentreOfMass(force);
    }
    m_bSupportingBallThisFrame = false;
}

PhysicsBoneID PhysicsCharacter::ResolvePhysicsBoneIDFromName(const char* name)
{
    if (strcmpi("PHYSBONE_R_ARM", name) == 0)
        return PHYSBONE_FIELDER_R_ARM;
    if (strcmpi("PHYSBONE_L_ARM", name) == 0)
        return PHYSBONE_FIELDER_L_ARM;
    if (strcmpi("PHYSBONE_R_LEG", name) == 0)
        return PHYSBONE_FIELDER_R_LEG;
    if (strcmpi("PHYSBONE_L_LEG", name) == 0)
        return PHYSBONE_FIELDER_L_LEG;
    if (strcmpi("PHYSBONE_HEAD", name) == 0)
        return PHYSBONE_FIELDER_HEAD;
    if (strcmpi("PhySphere_Lhand", name) == 0)
        return PHYSBONE_GOALIE_L_HAND;
    if (strcmpi("PhySphere_Lwrist", name) == 0)
        return PHYSBONE_GOALIE_L_WRIST;
    if (strcmpi("PhySphere_Lforearm", name) == 0)
        return PHYSBONE_GOALIE_L_FOREARM;
    if (strcmpi("PhySphere_Lbicep", name) == 0)
        return PHYSBONE_GOALIE_L_BICEP;
    if (strcmpi("PhySphere_Lshoulder", name) == 0)
        return PHYSBONE_GOALIE_L_SHOULDER;
    if (strcmpi("PhySphere_Lthigh", name) == 0)
        return PHYSBONE_GOALIE_L_THIGH;
    if (strcmpi("PhySphere_Lthighlower", name) == 0)
        return PHYSBONE_GOALIE_L_THIGHLOWER;
    if (strcmpi("PhySphere_Lcalfupper", name) == 0)
        return PHYSBONE_GOALIE_L_CALFUPPER;
    if (strcmpi("PhySphere_Lheel", name) == 0)
        return PHYSBONE_GOALIE_L_HEEL;
    if (strcmpi("PhySphere_Ltoe", name) == 0)
        return PHYSBONE_GOALIE_L_TOE;
    if (strcmpi("PhySphere_Rhand", name) == 0)
        return PHYSBONE_GOALIE_R_HAND;
    if (strcmpi("PhySphere_Rwrist", name) == 0)
        return PHYSBONE_GOALIE_R_WRIST;
    if (strcmpi("PhySphere_Rforearm", name) == 0)
        return PHYSBONE_GOALIE_R_FOREARM;
    if (strcmpi("PhySphere_Rbicep", name) == 0)
        return PHYSBONE_GOALIE_R_BICEP;
    if (strcmpi("PhySphere_Rshoulder", name) == 0)
        return PHYSBONE_GOALIE_R_SHOULDER;
    if (strcmpi("PhySphere_Rthigh", name) == 0)
        return PHYSBONE_GOALIE_R_THIGH;
    if (strcmpi("PhySphere_Rthighlower", name) == 0)
        return PHYSBONE_GOALIE_R_THIGHLOWER;
    if (strcmpi("PhySphere_Rcalfupper", name) == 0)
        return PHYSBONE_GOALIE_R_CALFUPPER;
    if (strcmpi("PhySphere_Rheel", name) == 0)
        return PHYSBONE_GOALIE_R_HEEL;
    if (strcmpi("PhySphere_Rtoe", name) == 0)
        return PHYSBONE_GOALIE_R_TOE;
    if (strcmpi("PhySphere_head", name) == 0)
        return PHYSBONE_GOALIE_HEAD;
    return strcmpi("PhySphere_stomach", name) != 0
             ? PHYSBONE_UNKNOWN
             : PHYSBONE_GOALIE_STOMACH;
}

static inline float Clamp(float value, float low, float high)
{
    value = value >= low ? value : low;
    return value <= high ? value : high;
}

void PhysicsCharacter::PostUpdate()
{
    PhysicsObject::PostUpdate();

    nlVector3 position;
    GetPosition(&position);

    float radius;
    m_pPlayerPlayerColumn->GetRadius(&radius);
    if (fabsf(position.x) < cField::GetGoalLineX(1) - radius)
    {
        m_bInsideNet = false;
        m_bWasInsideNet = false;
    }

    if (m_bWasInsideNet)
    {
        float netWidth = cNet::m_fNetWidth;
        float halfWidth = 0.5f * netWidth - radius;
        position.y = Clamp(position.y, -halfWidth, halfWidth);

        float netDepth = cNet::m_fNetDepth;
        float netBack = netDepth + cField::GetGoalLineX(1) - radius;
        position.x = Clamp(position.x, -netBack, netBack);
    }

    if (m_pAICharacter->m_eClassType == FIELDER
        && fn_8003E74C(m_pAICharacter))
    {
        float goalLine = cField::GetGoalLineX(1) - radius;
        position.x = Clamp(position.x, -goalLine, goalLine);
    }

    nlVector3 characterPosition;
    characterPosition.x = position.x;
    characterPosition.y = position.y;
    characterPosition.z = 0.0f;
    SetCharacterPosition(characterPosition);
    m_pAICharacter->PostPhysicsUpdate();

    if (m_bSupportingBallThisFrame)
        ++m_nDKBallStuckHackCounter;
    else
        m_nDKBallStuckHackCounter = 0;
}

void PhysicsCharacter::GetCharacterPositionXY(nlVector3* pos)
{
    float z = pos->z;
    GetPosition(pos);
    pos->z = z;
}

void PhysicsCharacter::SetCharacterPositionXY(const nlVector3& pos)
{
    nlVector3 value;
    value.x = pos.x;
    value.y = pos.y;
    value.z = 0.0f;
    SetCharacterPosition(value);
}

void PhysicsCharacter::GetCharacterVelocityXY(nlVector3* vel)
{
    float z = vel->z;
    GetLinearVelocity(vel);
    vel->z = z;
}

void PhysicsCharacter::SetCharacterVelocityXY(const nlVector3& vel)
{
    nlVector3 value;
    value.x = vel.x;
    value.y = vel.y;
    value.z = 0.0f;
    SetLinearVelocity(value);
}
