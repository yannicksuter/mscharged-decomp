#include "Game/Physics/PhysicsBulletBill.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/BulletBill.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Player.h"
#include "NL/nlSlotPool.h"

#include <math.h>

struct CollisionBulletBillData
{
    cCharacter* player;
    BulletBillObject* bulletBill;
};

extern SlotPool<CollisionBulletBillData> lbl_805712F8;

extern "C" bool fn_8003886C(cFielder*);
extern "C" void fn_80147C9C(CollisionBulletBillData*);
extern "C" void fn_80147DE4(CollisionBulletBillData*);
extern "C" void fn_80147F2C(CollisionBulletBillData*);

PhysicsBulletBill::PhysicsBulletBill(
    BulletBillObject* bulletBill, float radius, float)
    : PhysicsSphere(g_CollisionSpace, 0, radius)
    , mUnidentified38(0)
    , mBulletBill(bulletBill)
{
    SetCollide(0x62);
    SetCategory(2);
}

ContactType PhysicsBulletBill::Contact(
    PhysicsObject* other, dContact*, int)
{
    nlVector3 position;
    GetPosition(&position);

    switch (other->GetObjectType())
    {
    case 4:
    {
        cCharacter* character
            = ((PhysicsCharacter*)other->m_parentObject)->m_pAICharacter;
        cFielder* target = mBulletBill->target;
        if (character == target || target == 0)
        {
            return NO_CONTACT;
        }

        if (character->m_eClassType == FIELDER)
        {
            bool canHit = false;
            if (!fn_8003886C(target)
                && (target->muInvincibleStatus & 1) != 0)
            {
                canHit = true;
            }
            if (character != target && canHit)
            {
                CollisionBulletBillData* eventData = 0;
                lbl_805712F8.Allocate(eventData);
                eventData->player = character;
                eventData->bulletBill = mBulletBill;
                fn_80147C9C(eventData);
            }
        }
        else
        {
            CollisionBulletBillData* eventData = 0;
            lbl_805712F8.Allocate(eventData);
            eventData->player = character;
            eventData->bulletBill = mBulletBill;
            fn_80147F2C(eventData);
        }
        break;
    }
    case 0x10:
    {
        cPlayer* owner = ((PhysicsAIBall*)other)->m_pAIBall->m_pOwner;
        cFielder* target = mBulletBill->target;
        if ((owner != 0 && owner == target) || target == 0)
        {
            return NO_CONTACT;
        }

        if (owner == 0)
        {
            return ONE_WAY_CONTACT_OTHER;
        }

        if (owner->m_eClassType == FIELDER)
        {
            bool canHit = false;
            if (!fn_8003886C(target)
                && (target->muInvincibleStatus & 1) != 0)
            {
                canHit = true;
            }
            if (canHit)
            {
                CollisionBulletBillData* eventData = 0;
                lbl_805712F8.Allocate(eventData);
                eventData->player = owner;
                eventData->bulletBill = mBulletBill;
                fn_80147C9C(eventData);
            }
        }
        else
        {
            CollisionBulletBillData* eventData = 0;
            lbl_805712F8.Allocate(eventData);
            eventData->player = owner;
            eventData->bulletBill = mBulletBill;
            fn_80147F2C(eventData);
        }
        break;
    }
    case 0x14:
    case 0x15:
    {
        PowerupBase* powerup = other->GetObjectType() == 0x14
                                 ? ((PhysicsShell*)other)->m_pPowerupObject
                                 : ((PhysicsBanana*)other)->m_pPowerupObject;
        if (powerup->mtNoHitTimer.m_uPackedTime != 0
            && powerup->m_pThrower == mBulletBill->target)
        {
            return NO_CONTACT;
        }

        if (powerup->m_eType == POWER_UP_FREEZE_SHELL)
        {
            CollisionBulletBillData* eventData = 0;
            lbl_805712F8.Allocate(eventData);
            eventData->player = mBulletBill->target;
            eventData->bulletBill = mBulletBill;
            fn_80147DE4(eventData);
            powerup->m_bShouldDestroy = true;
            return NO_CONTACT;
        }

        if (powerup->m_eType == POWER_UP_BANANA
            && powerup->meSize == POWERUPSIZE_SMALL)
        {
            powerup->m_bShouldDestroy = true;
            return NO_CONTACT;
        }

        CollisionBulletBillData* eventData = 0;
        lbl_805712F8.Allocate(eventData);
        eventData->player = 0;
        eventData->bulletBill = mBulletBill;
        fn_80147F2C(eventData);
        break;
    }
    case 0x17:
        if (mBulletBill->target->m_pBall == 0)
        {
            CollisionBulletBillData* eventData = 0;
            lbl_805712F8.Allocate(eventData);
            eventData->player = 0;
            eventData->bulletBill = mBulletBill;
            fn_80147F2C(eventData);
        }
        break;
    case 0x18:
    case 0x19:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
    case 0x21:
    case 0x22:
    case 0x24:
    {
        CollisionBulletBillData* eventData = 0;
        lbl_805712F8.Allocate(eventData);
        eventData->player = 0;
        eventData->bulletBill = mBulletBill;
        fn_80147F2C(eventData);
        break;
    }
    default:
        break;
    }

    return NO_CONTACT;
}

bool PhysicsBulletBill::SetContactInfo(
    dContact* contact, PhysicsObject*, bool first)
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

void PhysicsBulletBill::PreCollide()
{
    if (mBulletBill->active && g_pGame->IsGameplayOrOvertime()
        && fabsf(GetPosition().x) > cField::GetGoalLineX(1U) + 2.5f)
    {
        CollisionBulletBillData* eventData = 0;
        lbl_805712F8.Allocate(eventData);
        eventData->player = mBulletBill->target;
        eventData->bulletBill = mBulletBill;
        fn_80147F2C(eventData);
    }
}

PhysicsBulletBill::~PhysicsBulletBill()
{
}
