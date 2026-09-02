#include "Game/Physics/PhysicsNPC.h"

#include "Game/Ball.h"
#include "Game/Character.h"
#include "Game/Field.h"
#include "Game/GameInfo.h"
#include "Game/Physics/CollisionSpace.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Render/SkinAnimatedMovableNPC.h"

#include "math.h"

extern CollisionSpace* g_CollisionSpace;
extern "C" void fn_8019C734(SkinAnimatedMovableNPC* npc);

PhysicsNPC::PhysicsNPC(float radius)
    : PhysicsSphere(g_CollisionSpace, (PhysicsWorld*)0, radius)
    , mpTriggerCallbackFunc(0)
    , mpAINPC(0)
    , mUnidentified040(false)
    , mUnidentified044(0)
    , mUnidentified048(0.0f)
{
    SetCollide(0x14062);
    SetCategory(2);
}

void PhysicsNPC::SetCallbackFunction(CallbackFn callback)
{
    mpTriggerCallbackFunc = callback;
}

ContactType PhysicsNPC::Contact(
    PhysicsObject* object, dContact* contact, int numContacts)
{
    nlVector3 position;
    GetPosition(&position);

    if (mUnidentified040)
    {
        return NO_CONTACT;
    }

    switch (object->GetObjectType())
    {
    case 0x04:
    {
        cCharacter* character
            = ((PhysicsCharacter*)object->m_parentObject)->m_pAICharacter;
        if (character->m_eClassType == FIELDER)
        {
            if (character != 0 && mpTriggerCallbackFunc != 0)
            {
                nlVector3 contactPosition;
                nlVec3Set(contactPosition, contact->geom.pos[0], contact->geom.pos[1], contact->geom.pos[2]);
                mpTriggerCallbackFunc(this, object, contactPosition);
                break;
            }
            return NO_CONTACT;
        }
        if (character->m_eClassType == GOALIE)
        {
            return ONE_WAY_CONTACT_THIS;
        }
        break;
    }
    case 0x10:
    {
        cBall* ball = ((PhysicsAIBall*)object)->m_pAIBall;
        if (!ball->m_pPhysicsBall->mbCanCollidePlayer)
        {
            return NO_CONTACT;
        }
        if (mpTriggerCallbackFunc != 0)
        {
            nlVector3 contactPosition;
            nlVec3Set(contactPosition, contact->geom.pos[0], contact->geom.pos[1], contact->geom.pos[2]);
            mpTriggerCallbackFunc(this, object, contactPosition);
        }
        ((PhysicsBall*)object)->mbUseMagnusEffect = false;
        ((PhysicsBall*)object)->mfChargeBonus = 0.0f;
        FakeBallWorld::InvalidateBallCache();
        ++ball->m_bBallPathChangeCount;
        return ONE_WAY_CONTACT_OTHER;
    }
    case 0x14:
    case 0x15:
    {
        if (mpTriggerCallbackFunc != 0)
        {
            nlVector3 contactPosition;
            nlVec3Set(contactPosition, contact->geom.pos[0], contact->geom.pos[1], contact->geom.pos[2]);
            mpTriggerCallbackFunc(this, object, contactPosition);
        }
        break;
    }
    case 0x23:
        return ONE_WAY_CONTACT_THIS;
    default:
        break;
    }

    if (object->GetObjectType() == 0x17
        && GameInfoManager::Instance()->GetStadium() == 0x0B
        && mpAINPC->GetSkinAnimatedNPC_Type() == SkinAnimatedNPC_CHAIN_CHOMP)
    {
        float bottom = mpAINPC->mv3Position.y - mpAINPC->mpPhysObj->GetRadius();
        if ((float)fabs(mpAINPC->mv3Position.x)
                < cField::GetGoalLineX(1U) - 0.5f
            && bottom > cField::GetSidelineY(1U))
        {
            fn_8019C734(mpAINPC);
            mUnidentified040 = true;
        }
    }

    return NO_CONTACT;
}

void PhysicsNPC::PreUpdate()
{
    PhysicsObject::PreUpdate();
    if (mUnidentified044 > 0)
    {
        nlVector3 velocity;
        GetLinearVelocity(&velocity);
        nlVec3Scale(velocity, mUnidentified048);
        SetLinearVelocity(velocity);
    }
}

void PhysicsNPC::PostUpdate()
{
    PhysicsObject::PostUpdate();
    if (mUnidentified044 > 0)
    {
        nlVector3 velocity;
        GetLinearVelocity(&velocity);
        nlVec3Scale(velocity, 1.0f / mUnidentified048);
        SetLinearVelocity(velocity);
        if (--mUnidentified044 == 0)
        {
            mUnidentified048 = 0.0f;
        }
    }
}

bool PhysicsNPC::SetContactInfo(
    dContact* contact, PhysicsObject* other, bool first)
{
    if (first)
    {
        SetDefaultContactInfo(contact);
    }

    contact->surface.bounce = 0.01f;
    contact->surface.bounce_vel = 0.0f;
    contact->surface.mu = 5.0f;
    return true;
}
