#include "Game/Physics/PhysicsKoopaShell.h"

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
#include "unclassified/tu_801A0E64.h"
#include "Game/Render/KoopaShellObject.h"
#include "Game/Render/YoshiEggObject.h"
#include "Game/UnidentifiedStaticStorage.h"

PhysicsKoopaShell::PhysicsKoopaShell(KoopaShellObject* shell, float radius)
    : PhysicsSphere(g_CollisionSpace, 0, radius)
    , mUnidentified38(0)
    , mKoopaShell(shell)
{
    SetCategory(0x4000);
    SetCollide(0x1F042);
}

ContactType PhysicsKoopaShell::Contact(PhysicsObject* other, dContact*, int)
{
    nlVector3 shellPosition;
    YoshiEggObject* egg;
    GetPosition(&shellPosition);

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
            if (fielder == mKoopaShell->mOwner)
            {
                break;
            }

            bool isInvincible = false;
            if (!fielder->IsStuck()
                && (fielder->muInvincibleStatus & 0x1F) == 0x1F)
            {
                isInvincible = true;
            }
            if (isInvincible)
            {
                CollisionKoopaShellEndData* eventData = 0;
                g_CollisionKoopaShellEndDataPool.Allocate(eventData);
                eventData->shell = mKoopaShell;
                eventData->cracked = true;
                QueueCollisionKoopaShellEnd(eventData);
                return NO_CONTACT;
            }

            CollisionKoopaShotBallPlayerData* eventData = 0;
            g_CollisionKoopaShotBallPlayerDataPool.Allocate(eventData);
            eventData->player = fielder;
            eventData->shell = mKoopaShell;
            QueueCollisionKoopaShotBallPlayer(eventData);
        }
        else
        {
            CollisionKoopaShellGoalieData* eventData = 0;
            g_CollisionKoopaShellGoalieDataPool.Allocate(eventData);
            eventData->goalie = character;
            eventData->shell = mKoopaShell;
            QueueCollisionKoopaShellGoalie(eventData);
        }
        break;
    }
    case 0x17:
    {
        bool isGrowing = mKoopaShell->mRadiusTimer > 0.0f;
        if (isGrowing)
        {
            return NO_CONTACT;
        }
    }
    // fall through
    case 0x19:
    {
        CollisionKoopaShellEndData* eventData = 0;
        g_CollisionKoopaShellEndDataPool.Allocate(eventData);
        eventData->shell = mKoopaShell;
        eventData->cracked = false;
        QueueCollisionKoopaShellEnd(eventData);
        break;
    }
    case 0x15:
    {
        PowerupBase* powerup = ((PhysicsBanana*)other)->m_pPowerupObject;
        if (mKoopaShell->mOwner == powerup->m_pThrower
            && powerup->mtNoHitTimer.m_uPackedTime != 0)
        {
            return NO_CONTACT;
        }
        QueueKoopaShellDestroyPowerup((UnidentifiedEventData27*)powerup);
        if (((PhysicsBanana*)other)->m_pPowerupObject->m_eType == POWER_UP_BOBOMB
            || ((PhysicsBanana*)other)->m_pPowerupObject->m_unk44.m_uPackedTime != 0)
        {
            CollisionKoopaShellEndData* eventData = 0;
            g_CollisionKoopaShellEndDataPool.Allocate(eventData);
            eventData->shell = mKoopaShell;
            eventData->cracked = false;
            QueueCollisionKoopaShellEnd(eventData);
        }
        break;
    }
    case 0x14:
    {
        PowerupBase* powerup = ((PhysicsShell*)other)->m_pPowerupObject;
        if (mKoopaShell->mOwner == powerup->m_pThrower
            && powerup->mtNoHitTimer.m_uPackedTime != 0)
        {
            return NO_CONTACT;
        }
        QueueKoopaShellDestroyPowerup((UnidentifiedEventData27*)powerup);
        if (((PhysicsShell*)other)->m_pPowerupObject->m_unk44.m_uPackedTime != 0)
        {
            CollisionKoopaShellEndData* eventData = 0;
            g_CollisionKoopaShellEndDataPool.Allocate(eventData);
            eventData->shell = mKoopaShell;
            eventData->cracked = false;
            QueueCollisionKoopaShellEnd(eventData);
        }
        break;
    }
    case 0x1F:
    {
        HammerObject* hammer = ((PhysicsHammer*)other)->mHammer;
        if (mKoopaShell->mOwner == hammer->_034)
        {
            bool isTimerRunning = hammer->_01C > 0.0f;
            if (isTimerRunning)
            {
                return NO_CONTACT;
            }
        }
        QueueKoopaShellDestroyHammer((UnidentifiedEventData35*)hammer);
        break;
    }
    case 0x20:
    {
        egg = ((PhysicsYoshiEgg*)other)->mYoshiEgg;
        if (egg->mActive)
        {
            CollisionKoopaShellEndData* endData = 0;
            g_CollisionKoopaShellEndDataPool.Allocate(endData);
            endData->shell = mKoopaShell;
            endData->cracked = true;
            QueueCollisionKoopaShellEnd(endData);

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
    case 0x24:
    {
        CollisionKoopaShellEndData* eventData = 0;
        g_CollisionKoopaShellEndDataPool.Allocate(eventData);
        eventData->shell = mKoopaShell;
        eventData->cracked = true;
        QueueCollisionKoopaShellEnd(eventData);
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
            CollisionKoopaShellEndData* eventData = 0;
            g_CollisionKoopaShellEndDataPool.Allocate(eventData);
            eventData->shell = mKoopaShell;
            eventData->cracked = true;
            QueueCollisionKoopaShellEnd(eventData);
        }

        bool isBowser
            = ((SkinAnimatedNPC*)((PhysicsNPC*)other)->mpAINPC)
                  ->GetSkinAnimatedNPC_Type()
            == SkinAnimatedNPC_BOWSER;
        if (isBowser)
        {
            CollisionKoopaShellEndData* eventData = 0;
            g_CollisionKoopaShellEndDataPool.Allocate(eventData);
            eventData->shell = mKoopaShell;
            eventData->cracked = true;
            QueueCollisionKoopaShellEnd(eventData);
        }
        break;
    }
    case 0x1E:
    {
        CollisionKoopaShellEndData* eventData = 0;
        g_CollisionKoopaShellEndDataPool.Allocate(eventData);
        eventData->shell = mKoopaShell;
        eventData->cracked = false;
        QueueCollisionKoopaShellEnd(eventData);
        break;
    }
    case 0x23:
    {
        CollisionKoopaShellEndData* eventData = 0;
        g_CollisionKoopaShellEndDataPool.Allocate(eventData);
        eventData->shell = mKoopaShell;
        eventData->cracked = false;
        QueueCollisionKoopaShellEnd(eventData);
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
        case 6:
        {
            cFielder* fielder = (cFielder*)patch->m_pOwner;
            if (!fielder->IsInvincible())
            {
                QueueKoopaShellKnockYoshiTongue(fielder);
            }
            break;
        }
        }
        break;
    }
    }

    return NO_CONTACT;
}

bool PhysicsKoopaShell::SetContactInfo(
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

void PhysicsKoopaShell::PreCollide()
{
}

int PhysicsKoopaShell::GetObjectType() const
{
    return 0x22;
}

PhysicsKoopaShell::~PhysicsKoopaShell()
{
}
