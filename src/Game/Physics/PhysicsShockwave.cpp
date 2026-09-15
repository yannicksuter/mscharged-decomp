#include "Game/AI/Fielder.h"
#include "Game/Render/BulletBill.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/EventDataTypes.h"
#include "Game/Game.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Physics/PhysicsSphere.h"
#include "NL/nlSlotPool.h"
#include "Game/Physics/PhysicsShockwave.h"
#include "Game/UnidentifiedStaticStorage.h"

#include "types.h"

class cFielder;
class EffectsGroup;

extern "C" bool fn_800977A4(cFielder*, float);
extern "C" void fn_800F026C(
    const nlVector3&, float, float);

extern SlotPool<CollisionShockwaveData> gCollisionShockwaveDataPool;
extern "C" void QueueCollisionShockwave(CollisionShockwaveData*);

static const float sInitialShockwaveRadius = 0.5f;
static const float sShockwaveRadiusOvershoot = 0.01f;

PhysicsShockwave::PhysicsShockwave(void* owner,
    const nlVector3& position, int type, float maximumRadius,
    float expansionRate)
    : PhysicsSphere(g_CollisionSpace, 0, sInitialShockwaveRadius)
    , mOwner(owner)
    , mType(type)
    , mRadius(sInitialShockwaveRadius)
    , mMaximumRadius(maximumRadius)
    , mExpansionRate(expansionRate)
    , mSourceIndex(-1)
    , mFinished(false)
{
    SetCategory(0x10000);
    SetCollide(0xF060);
    mPosition = position;
    SetPosition(mPosition, WORLD_COORDINATES);
}

static nlVector3 sDaisyFistCameraShakeAmplitude = { 0.15f, 0.4f, 0.0f };

static PhysicsShockwave* sActiveShockwaves[20];
static bool sShockwavesInitialized;

float gBulletBillShockwaveRadius = 7.5f;
float gBulletBillShockwaveExpansionRate = 48.0f;
float gExplosionShockwaveRadius = 4.0f;
float gDaisyFistShockwaveRadius = 3.7f;
float gPowerupShockwaveExpansionRate = 20.0f;
float gHitShockwaveExpansionRate = 20.0f;
float gLightningShockwaveExpansionRate = 30.0f;
float gExplosionShockwaveExpansionRate = 15.0f;
float gDaisyFistShockwaveExpansionRate = 20.0f;
float gDaisyFistCameraShakeFrequency = 30.0f;
float gDaisyFistCameraShakeDuration = 1.0f;

ContactType PhysicsShockwave::Contact(
    PhysicsObject* other, dContact*, int)
{
    nlVector3 spherePosition;
    CollisionShockwaveData* eventData;
    GetPosition(&spherePosition);

    ContactType result = NO_CONTACT;

    switch (other->GetObjectType())
    {
    case 4:
    {
        cFielder* fielder =
            (cFielder*)((PhysicsCharacter*)other->m_parentObject)->m_pAICharacter;
        if (fielder->IsCharacterInAir(GetPosition().z + GetRadius()))
        {
            return NO_CONTACT;
        }

        if (fn_800977A4(fielder, GetPosition().z - GetRadius()))
        {
            return NO_CONTACT;
        }

        eventData = 0;
        gCollisionShockwaveDataPool.Allocate(eventData);
        eventData->pShockwave = this;
        eventData->pObject = other;
        QueueCollisionShockwave(eventData);
        break;
    }
    case 28:
        if (((PhysicsPatch*)other)->m_Type != 0)
        {
            break;
        }
    // fall through
    case 16:
    case 20:
    case 21:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    {
        eventData = 0;
        gCollisionShockwaveDataPool.Allocate(eventData);
        eventData->pShockwave = this;
        eventData->pObject = other;
        QueueCollisionShockwave(eventData);
        break;
    }
    case 24:
        result = ONE_WAY_CONTACT_OTHER;
        break;
    }

    return result;
}

bool PhysicsShockwave::SetContactInfo(
    dContact* contact, PhysicsObject*, bool first)
{
    if (first)
    {
        SetDefaultContactInfo(contact);
    }
    return true;
}

extern "C" void InitializeShockwaves()
{
    for (int i = 0; i < 20; ++i)
    {
        sActiveShockwaves[i] = 0;
    }
    sShockwavesInitialized = true;
}

extern "C" void ShutdownShockwaves()
{
    if (!sShockwavesInitialized)
    {
        return;
    }

    for (int i = 0; i < 20; ++i)
    {
        if (sActiveShockwaves[i] != 0)
        {
            delete sActiveShockwaves[i];
            sActiveShockwaves[i] = 0;
        }
    }

    sShockwavesInitialized = false;
    PhysicsShockwave::pool.FreeBlocks();
}

PhysicsShockwave::~PhysicsShockwave()
{
}

extern "C" void UpdateShockwaves(float dt)
{
    for (int i = 0; i < 20; ++i)
    {
        PhysicsShockwave* shockwave = sActiveShockwaves[i];
        if (shockwave == 0)
        {
            continue;
        }

        if ((!g_pGame->IsGameplayOrOvertime() && g_pGame->m_eGameState != 3)
            || shockwave->mFinished)
        {
            delete shockwave;
            sActiveShockwaves[i] = 0;
            continue;
        }

        if (shockwave->mRadius >= shockwave->mMaximumRadius)
        {
            shockwave->mFinished = true;
            continue;
        }

        shockwave->mRadius += shockwave->mExpansionRate * dt;
        if (shockwave->mRadius >= shockwave->mMaximumRadius)
        {
            shockwave->mRadius = shockwave->mMaximumRadius + sShockwaveRadiusOvershoot;
        }
        shockwave->SetRadius(shockwave->mRadius);
    }
}

static inline PhysicsShockwave* CreateShockwave(const nlVector3& position,
    int type, float maximumRadius, float expansionRate, void* owner)
{
    for (int i = 0; i < 20; ++i)
    {
        if (sActiveShockwaves[i] == 0)
        {
            PhysicsShockwave* shockwave = new PhysicsShockwave(
                owner, position, type, maximumRadius, expansionRate);
            sActiveShockwaves[i] = shockwave;
            return shockwave;
        }
    }
    return 0;
}

extern "C" PhysicsShockwave* CreatePowerupShockwave(
    const nlVector3* position, cFielder* owner, bool frozen,
    int sourceIndex, float maximumRadius)
{
    PhysicsShockwave* shockwave = CreateShockwave(*position,
        SHOCKWAVE_EXPLOSION, maximumRadius,
        gPowerupShockwaveExpansionRate, owner);
    if (shockwave != 0)
    {
        shockwave->mSourceIndex = sourceIndex;
        if (frozen)
        {
            shockwave->mType = SHOCKWAVE_FREEZE;
        }
    }
    return shockwave;
}

extern "C" PhysicsShockwave* CreateHitShockwave(
    cFielder* owner, const nlVector3* position, float maximumRadius)
{
    return CreateShockwave(*position, SHOCKWAVE_HIT, maximumRadius,
        gHitShockwaveExpansionRate, owner);
}

extern "C" PhysicsShockwave* CreateBulletBillShockwave(
    BulletBillObject* bulletBill)
{
    return CreateShockwave(bulletBill->position, SHOCKWAVE_BULLET_BILL,
        gBulletBillShockwaveRadius, gBulletBillShockwaveExpansionRate,
        bulletBill->target);
}

extern "C" PhysicsShockwave* CreateLightningShockwave(
    const nlVector3* position, float maximumRadius)
{
    return CreateShockwave(*position, SHOCKWAVE_LIGHTNING, maximumRadius,
        gLightningShockwaveExpansionRate, 0);
}

static char sBowserExplodeEffectName[] = "bowser_explode";
static char sDaisyFistExitEffectName[] = "daisy_fist_exit";

extern "C" PhysicsShockwave* CreateExplosionShockwave(
    const nlVector3* position)
{
    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(sBowserExplodeEffectName);
    if (group != 0)
    {
        EmissionController* controller = EmissionManager::Instance()->Create(group, 3, true, 0);
        controller->SetPosition(*position);
    }
    return CreateShockwave(*position, SHOCKWAVE_EXPLOSION,
        gExplosionShockwaveRadius, gExplosionShockwaveExpansionRate, 0);
}

PhysicsShockwave* CreateDaisyFistImpact(
    const nlVector3* position, cCharacter* owner)
{
    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(sDaisyFistExitEffectName);
    if (group != 0)
    {
        EmissionController* controller = EmissionManager::Instance()->Create(group, 3, true, 0);
        controller->SetPosition(*position);
    }
    fn_800F026C(sDaisyFistCameraShakeAmplitude,
        gDaisyFistCameraShakeFrequency, gDaisyFistCameraShakeDuration);
    return CreateShockwave(*position, SHOCKWAVE_DAISY_FIST,
        gDaisyFistShockwaveRadius, gDaisyFistShockwaveExpansionRate, owner);
}

SlotPool<PhysicsShockwave> PhysicsShockwave::pool(16, 16);
