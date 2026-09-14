#include "Game/Physics/PhysicsNPC.h"
#include "Game/Render/ChainChomp.h"

#include "Game/Ball.h"
#include "Game/Character.h"
#include "Game/Field.h"
#include "Game/GameInfo.h"
#include "Game/Physics/CollisionSpace.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Render/SkinAnimatedMovableNPC.h"

#include "math.h"

extern CollisionSpace* g_CollisionSpace;

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
    {
        if (mpTriggerCallbackFunc != 0)
        {
            nlVector3 contactPosition;
            nlVec3Set(contactPosition, contact->geom.pos[0], contact->geom.pos[1], contact->geom.pos[2]);
            mpTriggerCallbackFunc(this, object, contactPosition);
        }
        break;
    }
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
    {
        if (object->GetObjectType() == 0x17
            && GameInfoManager::Instance()->GetStadium() == 0x0B)
        {
            bool isChainChomp
                = ((SkinAnimatedNPC*)mpAINPC)->GetSkinAnimatedNPC_Type()
               == SkinAnimatedNPC_CHAIN_CHOMP;
            if (isChainChomp)
            {
                ChainChomp* chainChomp = (ChainChomp*)mpAINPC;
                float bottom = chainChomp->mv3Position.y
                             - chainChomp->mpPhysObj->GetRadius();
                bool isPastSideline = bottom > cField::GetSidelineY(1U);
                bool isInsideGoalLine = fabsf(chainChomp->mv3Position.x)
                                     < cField::GetGoalLineX(1U) - 0.5f;
                if (isInsideGoalLine && isPastSideline)
                {
                    chainChomp->Fall();
                    mUnidentified040 = true;
                }
            }
        }

        if (object->GetObjectType() == 0x1C)
        {
            bool isChainChomp
                = ((SkinAnimatedNPC*)mpAINPC)->GetSkinAnimatedNPC_Type()
               == SkinAnimatedNPC_CHAIN_CHOMP;
            if (isChainChomp)
            {
                int type = ((PhysicsPatch*)object)->m_Type;
                UnidentifiedPhysicsPatchInfo_80510BF0* info = fn_80174ED4(&type);
                if (info->mUnidentified18 != 0.0f)
                {
                    if (mUnidentified044 != 2
                        || info->mUnidentified18 > mUnidentified048)
                    {
                        mUnidentified048 = info->mUnidentified18;
                    }
                    mUnidentified044 = 2;
                }
            }
        }
        break;
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
