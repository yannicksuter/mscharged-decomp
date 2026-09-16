#include "Game/Physics/PhysicsBirdoEgg.h"
#include "Game/Render/BirdoEgg.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/EventDataTypes.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/PhysicsHammer.h"
#include "Game/Physics/PhysicsNPC.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Physics/PhysicsYoshiEgg.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "Game/Render/HammerObject.h"
#include "Game/Render/YoshiEggObject.h"
#include "NL/nlSlotPool.h"
#include "Game/UnidentifiedStaticStorage.h"

PhysicsBirdoEgg::PhysicsBirdoEgg(BirdoEggObject* egg, float radius)
    : PhysicsSphere(g_CollisionSpace, 0, radius)
    , mUnidentified38(0)
    , mBirdoEgg(egg)
{
    SetCategory(0x4000);
    SetCollide(0x1F042);
}

ContactType PhysicsBirdoEgg::Contact(
    PhysicsObject* other, dContact*, int)
{
    nlVector3 eggPosition;
    CollisionBirdoShotBallPlayerData* shotData;
    CollisionBirdoEggGoalieData* goalieData;
    CollisionBirdoEggEndData* endData;
    GetPosition(&eggPosition);

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
            if (fielder->IsCharacterInAir(GetPosition().z + GetRadius()))
            {
                return NO_CONTACT;
            }
            if (fielder == mBirdoEgg->mShooter)
            {
                break;
            }

            shotData = 0;
            g_CollisionBirdoShotBallPlayerDataPool.Allocate(shotData);
            shotData->player = fielder;
            shotData->egg = mBirdoEgg;
            QueueCollisionBirdoShotBallPlayer(shotData);

            if (!fielder->fn_8003E73C() && mBirdoEgg->mShooter->m_pBall == 0)
            {
                endData = 0;
                g_CollisionBirdoEggEndDataPool.Allocate(endData);
                endData->egg = mBirdoEgg;
                endData->cracked = false;
                QueueCollisionBirdoEggEnd(endData);
            }
        }
        else
        {
            goalieData = 0;
            g_CollisionBirdoEggGoalieDataPool.Allocate(goalieData);
            goalieData->goalie = character;
            goalieData->egg = mBirdoEgg;
            QueueCollisionBirdoEggGoalie(goalieData);
        }
        break;
    }
    case 0x15:
    {
        PowerupBase* powerup = ((PhysicsBanana*)other)->m_pPowerupObject;
        if (mBirdoEgg->mShooter == powerup->m_pThrower
            && powerup->mtNoHitTimer.m_uPackedTime != 0)
        {
            return NO_CONTACT;
        }
        QueueBirdoEggDestroyPowerup((UnidentifiedEventData27*)powerup);
        if (((PhysicsBanana*)other)->m_pPowerupObject->m_eType == POWER_UP_BOBOMB)
        {
            endData = 0;
            g_CollisionBirdoEggEndDataPool.Allocate(endData);
            endData->egg = mBirdoEgg;
            endData->cracked = false;
            QueueCollisionBirdoEggEnd(endData);
        }
        break;
    }
    case 0x14:
    {
        PowerupBase* powerup = ((PhysicsShell*)other)->m_pPowerupObject;
        if (mBirdoEgg->mShooter == powerup->m_pThrower
            && powerup->mtNoHitTimer.m_uPackedTime != 0)
        {
            return NO_CONTACT;
        }
        QueueBirdoEggDestroyPowerup((UnidentifiedEventData27*)powerup);
        break;
    }
    case 0x1F:
    {
        HammerObject* hammer = ((PhysicsHammer*)other)->mHammer;
        if (mBirdoEgg->mShooter == hammer->mOwner)
        {
            bool isTimerRunning = hammer->mRadiusTimer > 0.0f;
            if (isTimerRunning)
            {
                return NO_CONTACT;
            }
        }
        QueueBirdoEggDestroyHammer((UnidentifiedEventData35*)hammer);
        break;
    }
    case 0x20:
    {
        YoshiEggObject* egg = ((PhysicsYoshiEgg*)other)->mYoshiEgg;
        if (egg->mActive)
        {
            endData = 0;
            g_CollisionBirdoEggEndDataPool.Allocate(endData);
            endData->egg = mBirdoEgg;
            endData->cracked = true;
            QueueCollisionBirdoEggEnd(endData);

            UnidentifiedEventData34* crackData = 0;
            lbl_80570188.Allocate(crackData);
            crackData->mUnidentified00 = 0;
            crackData->mUnidentified04 = egg->mFielder;
            crackData->mUnidentified08 = egg;
            crackData->mUnidentified0C = 0;
            crackData->mUnidentified10 = 0;
            QueueCollisionCrackEgg(crackData);
        }
        break;
    }
    case 0x18:
    {
        bool isChainChomp
            = ((SkinAnimatedNPC*)((PhysicsNPC*)other)->mpAINPC)
                  ->GetSkinAnimatedNPC_Type()
            == SkinAnimatedNPC_CHAIN_CHOMP;
        if (isChainChomp)
        {
            CollisionBirdoEggEndData* eventData = 0;
            g_CollisionBirdoEggEndDataPool.Allocate(eventData);
            eventData->egg = mBirdoEgg;
            eventData->cracked = true;
            QueueCollisionBirdoEggEnd(eventData);
        }

        bool isBowser
            = ((SkinAnimatedNPC*)((PhysicsNPC*)other)->mpAINPC)
                  ->GetSkinAnimatedNPC_Type()
            == SkinAnimatedNPC_BOWSER;
        if (isBowser)
        {
            CollisionBirdoEggEndData* eventData = 0;
            g_CollisionBirdoEggEndDataPool.Allocate(eventData);
            eventData->egg = mBirdoEgg;
            eventData->cracked = true;
            QueueCollisionBirdoEggEnd(eventData);
        }
        break;
    }
    case 0x24:
    {
        CollisionBirdoEggEndData* eventData = 0;
        g_CollisionBirdoEggEndDataPool.Allocate(eventData);
        eventData->egg = mBirdoEgg;
        eventData->cracked = true;
        QueueCollisionBirdoEggEnd(eventData);
        break;
    }
    case 0x1E:
    {
        CollisionBirdoEggEndData* eventData = 0;
        g_CollisionBirdoEggEndDataPool.Allocate(eventData);
        eventData->egg = mBirdoEgg;
        eventData->cracked = false;
        QueueCollisionBirdoEggEnd(eventData);
        break;
    }
    case 0x23:
    {
        CollisionBirdoEggEndData* eventData = 0;
        g_CollisionBirdoEggEndDataPool.Allocate(eventData);
        eventData->egg = mBirdoEgg;
        eventData->cracked = false;
        QueueCollisionBirdoEggEnd(eventData);
        break;
    }
    case 0x1C:
    {
        PhysicsPatch* patch = (PhysicsPatch*)other;
        if (patch->m_bKillMe)
        {
            break;
        }

        switch (patch->m_Type)
        {
        case 1:
        case 3:
        case 8:
        {
            CollisionBirdoEggEndData* eventData = 0;
            g_CollisionBirdoEggEndDataPool.Allocate(eventData);
            eventData->egg = mBirdoEgg;
            eventData->cracked = true;
            QueueCollisionBirdoEggEnd(eventData);
            break;
        }
        case 6:
        {
            cFielder* fielder = (cFielder*)patch->m_pOwner;
            if (!fielder->IsInvincible())
            {
                QueueBirdoEggKnockYoshiTongue(fielder);
            }
            break;
        }
        }
        break;
    }
    }

    return NO_CONTACT;
}

bool PhysicsBirdoEgg::SetContactInfo(
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

void PhysicsBirdoEgg::PreCollide()
{
}

int PhysicsBirdoEgg::GetObjectType() const
{
    return 0x21;
}

PhysicsBirdoEgg::~PhysicsBirdoEgg()
{
}
