#include "NL/nlDLListContainer.inl"
#include "Game/SharedStaticStorage.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/EventDataTypes.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Physics/PhysicsThwomp.h"
#include "Game/Physics/PhysicsWaluigiWall.h"
#include "Game/Render/ThwompObject.h"


float gThwompBounce = 0.26f;
float gThwompFriction = 0.0f;

static unsigned char sPhysicsThwompStorage[8 * sizeof(PhysicsThwomp)];
nlArrayAllocator<PhysicsThwomp> gPhysicsThwompAllocator(
    reinterpret_cast<PhysicsThwomp*>(sPhysicsThwompStorage), 8);

PhysicsThwomp::PhysicsThwomp(
    ThwompObject* object, float lx, float ly, float lz)
    : PhysicsBox(g_CollisionSpace, g_PhysicsWorld, lx, ly, lz)
    , mHeight(lz)
    , mThwomp(object)
{
    SetCollide(0x1F062);
    SetCategory(0x8000);
    m_gravity = 0.0f;
}

PhysicsThwomp::~PhysicsThwomp()
{
}

ContactType PhysicsThwomp::Contact(PhysicsObject* other, dContact*, int)
{
    ThwompObject* thwomp = mThwomp;
    bool isDelayed = thwomp->mDelayTimer > 0.0f;
    if (isDelayed && other->GetObjectType() != PHYSOBJ_COLUMN)
    {
        return NO_CONTACT;
    }

    switch (other->GetObjectType())
    {
    case PHYSOBJ_COLUMN:
    {
        cCharacter* character
            = ((PhysicsCharacter*)other->m_parentObject)->m_pAICharacter;
        if (character->m_eClassType == FIELDER)
        {
            cFielder* fielder = (cFielder*)character;
            if (!fielder->mbTangible)
            {
                return NO_CONTACT;
            }
            if (fielder->IsCharacterInAir(GetPosition().z + mHeight / 2.0f))
            {
                return NO_CONTACT;
            }
            float centreOfMassHeight
                = character->m_pPhysicsCharacter->m_CentreOfMassHeight;
            if (2.0 * centreOfMassHeight < GetPosition().z - mHeight / 2.0f)
            {
                return NO_CONTACT;
            }
        }
        QueueCollisionThwompPlayer(thwomp, character);
        return ONE_WAY_CONTACT_OTHER;
    }
    case PHYSOBJ_AI_BALL:
    {
        cBall* ball = ((PhysicsAIBall*)other)->m_pAIBall;
        if (ball->m_pOwner == 0)
        {
            if (ball->meBallState == BALL_STATE_SKILLSHOT
                && ball->m_pShooter->m_DetChar.m_eCharacterClass == BOO)
            {
                return NO_CONTACT;
            }
            if (ball->meBallState != BALL_STATE_FALLING)
            {
                QueueCollisionThwompBall(mThwomp);
            }
        }
        ball = ((PhysicsAIBall*)other)->m_pAIBall;
        ++ball->m_bBallDeflectCount;
        ++ball->m_bBallPathChangeCount;
        FakeBallWorld::InvalidateBallCache();
        return ONE_WAY_CONTACT_OTHER;
    }
    case PHYSOBJ_GROUND_PLANE:
        thwomp->OnLanding();
        return ONE_WAY_CONTACT_THIS;
    case PHYSOBJ_BANANA:
        ((PhysicsBanana*)other)->m_pPowerupObject->m_bShouldDestroy = true;
        return NO_CONTACT;
    case PHYSOBJ_SHELL:
        ((PhysicsShell*)other)->m_pPowerupObject->m_bShouldDestroy = true;
        return NO_CONTACT;
    case PHYSOBJ_NPC:
        return ONE_WAY_CONTACT_OTHER;
    case PHYSOBJ_WALUIGI_WALL:
        if (thwomp->mState == THWOMP_STATE_FALLING)
        {
            ((PhysicsWaluigiWall*)other)->ApplyDamage(1.0f);
        }
        return NO_CONTACT;
    case PHYSOBJ_WALL:
        return NO_CONTACT;
    default:
        return NO_CONTACT;
    }
}

void PhysicsThwomp::PreCollide()
{
}

void PhysicsThwomp::PostUpdate()
{
    PhysicsObject::PostUpdate();
}

bool PhysicsThwomp::SetContactInfo(dContact* contact,
    PhysicsObject* otherObject, bool first)
{
    if (first)
    {
        SetDefaultContactInfo(contact);
    }

    contact->surface.bounce = gThwompBounce;
    contact->surface.mu = gThwompFriction;
    contact->surface.bounce_vel = 0.0f;
    return true;
}
