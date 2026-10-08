#include "NL/nlDLListContainer.inl"
#include "Game/SharedStaticStorage.h"
#include "Game/Player.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/EventDataTypes.h"
#include "Game/Net.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/PhysicsHammer.h"
#include "Game/Physics/PhysicsNPC.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Render/SkinAnimatedMovableNPC.h"
#include "Game/Render/HammerObject.h"
#include "math.h"


bool gLandedHammersBlockFielders = true;
bool gBreakHammerOnFielderHit;

static unsigned char sPhysicsHammerStorage[15 * sizeof(PhysicsHammer)];
nlArrayAllocator<PhysicsHammer> gPhysicsHammerAllocator(
    reinterpret_cast<PhysicsHammer*>(sPhysicsHammerStorage), 15);

static const nlVector3 sZeroVelocity = { 0.0f, 0.0f, 0.0f };

PhysicsHammer::PhysicsHammer(float radius)
    : PhysicsSphere(g_CollisionSpace, g_PhysicsWorld, radius)
    , mHammer(0)
{
    SetCollide(0x1F042);
    SetCategory(0x8000);
    m_gravity = -20.0f;
}

PhysicsHammer::~PhysicsHammer()
{
}

ContactType PhysicsHammer::Contact(PhysicsObject* other, dContact*, int)
{
    HammerObject* hammer = mHammer;
    cFielder* thrower = hammer->mOwner;
    bool isDelayed = hammer->mFreezeTimer > 0.0f;
    bool isLanded;
    if (isDelayed)
    {
        return ONE_WAY_CONTACT_OTHER;
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
            if (thrower == fielder)
            {
                return NO_CONTACT;
            }
            if (!fielder->mbTangible)
            {
                return NO_CONTACT;
            }
            if (fielder->IsInvincibleHammers() || fielder->IsInvincible())
            {
                return NO_CONTACT;
            }
            if (fielder->IsCharacterInAir(GetPosition().z + GetRadius()))
            {
                return NO_CONTACT;
            }
            if (IsPlayerBelowHeight(fielder, GetPosition().z - GetRadius()))
            {
                return NO_CONTACT;
            }
        }
        else
        {
            cPlayer* player = (cPlayer*)character;
            if (IsPlayerBelowHeight(player, GetPosition().z - GetRadius()))
            {
                return NO_CONTACT;
            }
            if (player->IsCharacterInAir(GetPosition().z + GetRadius()))
            {
                return NO_CONTACT;
            }
            isLanded = hammer->mLandedTimer > 0.0f;
            if (isLanded)
            {
                hammer->Deactivate(true);
                return NO_CONTACT;
            }
        }

        isLanded = hammer->mLandedTimer > 0.0f;
        if (isLanded && gLandedHammersBlockFielders)
        {
            return ONE_WAY_CONTACT_OTHER;
        }

        CollisionProjectileData* data = 0;
        g_CollisionProjectileDataPool.Allocate(data);
        data->pTarget = character;
        data->pFielder = thrower;
        data->v3Position = GetPosition();
        data->v3Velocity = GetLinearVelocity();
        QueueCollisionHammerPlayer(data);

        if (gBreakHammerOnFielderHit)
        {
            hammer->Deactivate(true);
        }
        return NO_CONTACT;
    }
    case PHYSOBJ_AI_BALL:
        return NO_CONTACT;
    case PHYSOBJ_GROUND_PLANE:
    {
        isLanded = hammer->mLandedTimer > 0.0f;
        if (!isLanded)
        {
            CollisionProjectileData* data = 0;
            g_CollisionProjectileDataPool.Allocate(data);
            data->pTarget = 0;
            data->pFielder = thrower;
            data->v3Position = GetPosition();
            data->v3Velocity = GetLinearVelocity();
            QueueCollisionHammerGround(data);
        }
        hammer->OnLanding();
        return NO_CONTACT;
    }
    case PHYSOBJ_BANANA:
    {
        PowerupBase* powerup = ((PhysicsBanana*)other)->m_pPowerupObject;
        if (hammer->mOwner == powerup->m_pThrower
            && powerup->mtNoHitTimer.m_uPackedTime != 0)
        {
            return NO_CONTACT;
        }
        QueueCollisionHammerPowerup(powerup);
        if (((PhysicsBanana*)other)->m_pPowerupObject->m_eType == POWER_UP_BOBOMB)
        {
            hammer->Deactivate(true);
        }
        return NO_CONTACT;
    }
    case PHYSOBJ_SHELL:
    {
        PowerupBase* powerup = ((PhysicsShell*)other)->m_pPowerupObject;
        if (hammer->mOwner == powerup->m_pThrower
            && powerup->mtNoHitTimer.m_uPackedTime != 0)
        {
            return NO_CONTACT;
        }
        QueueCollisionHammerPowerup(powerup);
        if (((PhysicsShell*)other)->m_pPowerupObject->meSize == POWERUPSIZE_LARGE)
        {
            hammer->Deactivate(true);
        }
        return NO_CONTACT;
    }
    case PHYSOBJ_NPC:
    {
        bool isChainChomp
            = ((SkinAnimatedNPC*)((PhysicsNPC*)other)->mpAINPC)
                  ->GetSkinAnimatedNPC_Type()
            == SkinAnimatedNPC_CHAIN_CHOMP;
        if (isChainChomp)
        {
            QueueCollisionHammerChain(
                (ChainChomp*)((PhysicsNPC*)other)->mpAINPC);
            hammer->Deactivate(true);
        }
        return NO_CONTACT;
    }
    case PHYSOBJ_WALUIGI_WALL:
        return NO_CONTACT;
    case PHYSOBJ_THWOMP:
        hammer->Deactivate(true);
        return NO_CONTACT;
    case PHYSOBJ_WALL:
    {
        float radius = GetRadius();
        float netWidth = cNet::m_fNetWidth;
        float netHeight = cNet::m_fNetHeight;
        float y = (float)fabs(hammer->GetPosition()->y);
        float z = (float)fabs(hammer->GetPosition()->z);
        if (y <= netWidth && z <= netHeight)
        {
            return NO_CONTACT;
        }
        return ONE_WAY_CONTACT_THIS;
    }
    }
    return NO_CONTACT;
}

void PhysicsHammer::PreCollide()
{
}

void PhysicsHammer::PostUpdate()
{
    PhysicsObject::PostUpdate();
}

bool PhysicsHammer::SetContactInfo(
    dContact* contact, PhysicsObject* otherObject, bool first)
{
    if (first)
    {
        SetDefaultContactInfo(contact);
    }

    contact->surface.bounce = 0.2f;
    contact->surface.mu = 3.0f;
    contact->surface.bounce_vel = 0.0f;
    return true;
}

void PhysicsHammer::EnableGravity()
{
    m_gravity = -20.0f;
}

void PhysicsHammer::Freeze()
{
    SetLinearVelocity(sZeroVelocity);
    SetAngularVelocity(sZeroVelocity);
    m_gravity = 0.0f;
}
