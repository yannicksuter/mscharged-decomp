#include "Game/UnidentifiedStaticStorage.h"
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

extern "C" void fn_80149984(void* source, cCharacter* target);
extern "C" void fn_80149B30(UnidentifiedEventData33* data);

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
    if (isDelayed && other->GetObjectType() != 4)
    {
        return NO_CONTACT;
    }

    switch (other->GetObjectType())
    {
    case 4:
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
        fn_80149984(thwomp, character);
        return ONE_WAY_CONTACT_OTHER;
    }
    case 16:
    {
        cBall* ball = ((PhysicsAIBall*)other)->m_pAIBall;
        if (ball->m_pOwner == 0)
        {
            if (ball->meBallState == 8
                && ball->m_pShooter->mUnidentified024.m_eCharacterClass == 16)
            {
                return NO_CONTACT;
            }
            if (ball->meBallState != 10)
            {
                fn_80149B30((UnidentifiedEventData33*)mThwomp);
            }
        }
        ball = ((PhysicsAIBall*)other)->m_pAIBall;
        ++ball->m_bBallDeflectCount;
        ++ball->m_bBallPathChangeCount;
        FakeBallWorld::InvalidateBallCache();
        return ONE_WAY_CONTACT_OTHER;
    }
    case 18:
        thwomp->OnLanding();
        return ONE_WAY_CONTACT_THIS;
    case 21:
        ((PhysicsBanana*)other)->m_pPowerupObject->m_bShouldDestroy = true;
        return NO_CONTACT;
    case 20:
        ((PhysicsShell*)other)->m_pPowerupObject->m_bShouldDestroy = true;
        return NO_CONTACT;
    case 24:
        return ONE_WAY_CONTACT_OTHER;
    case 29:
        if (thwomp->mState == THWOMP_STATE_FALLING)
        {
            ((PhysicsWaluigiWall*)other)->ApplyDamage(1.0f);
        }
        return NO_CONTACT;
    case 23:
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
