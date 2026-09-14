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
#include "unclassified/tu_80175F8C.h"
#include "Game/UnidentifiedStaticStorage.h"

#include "types.h"

class cFielder;
class EffectsGroup;

extern "C" bool fn_800977A4(cFielder*, float);
extern "C" void fn_800F026C(
    const nlVector3&, float, float);

extern "C" SlotPool<UnidentifiedEventData38> lbl_805701B0;
extern "C" void fn_8014A2BC(UnidentifiedEventData38*);

static const float sInitialRadius = 0.5f;
static const float sRadiusOvershoot = 0.01f;

PhysicsSphere_80175F8C::PhysicsSphere_80175F8C(void* owner,
    const nlVector3& position, int effectType, float maximumRadius,
    float growthRate)
    : PhysicsSphere(g_CollisionSpace, 0, sInitialRadius)
    , owner(owner)
    , effectType(effectType)
    , currentRadius(sInitialRadius)
    , maximumRadius(maximumRadius)
    , growthRate(growthRate)
    , sourceIndex(-1)
    , finished(false)
{
    SetCategory(0x10000);
    SetCollide(0xF060);
    this->position = position;
    SetPosition(this->position, WORLD_COORDINATES);
}

static nlVector3 sDaisyCameraShake = { 0.15f, 0.4f, 0.0f };

static PhysicsSphere_80175F8C* sActiveSpheres[20];
static bool sSpheresInitialized;

float lbl_806DCAD8 = 7.5f;
float lbl_806DCADC = 48.0f;
float lbl_806DCAE0 = 4.0f;
float lbl_806DCAE4 = 3.7f;
float lbl_806DCAE8 = 20.0f;
float lbl_806DCAEC = 20.0f;
float lbl_806DCAF0 = 30.0f;
float lbl_806DCAF4 = 15.0f;
float lbl_806DCAF8 = 20.0f;
float lbl_806DCAFC = 30.0f;
float lbl_806DCB00 = 1.0f;

ContactType PhysicsSphere_80175F8C::Contact(
    PhysicsObject* other, dContact*, int)
{
    nlVector3 spherePosition;
    UnidentifiedEventData38* eventData;
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
        lbl_805701B0.Allocate(eventData);
        eventData->mUnidentified00 = this;
        eventData->mUnidentified04 = other;
        fn_8014A2BC(eventData);
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
        lbl_805701B0.Allocate(eventData);
        eventData->mUnidentified00 = this;
        eventData->mUnidentified04 = other;
        fn_8014A2BC(eventData);
        break;
    }
    case 24:
        result = ONE_WAY_CONTACT_OTHER;
        break;
    }

    return result;
}

bool PhysicsSphere_80175F8C::SetContactInfo(
    dContact* contact, PhysicsObject*, bool first)
{
    if (first)
    {
        SetDefaultContactInfo(contact);
    }
    return true;
}

extern "C" void fn_8017617C()
{
    for (int i = 0; i < 20; ++i)
    {
        sActiveSpheres[i] = 0;
    }
    sSpheresInitialized = true;
}

extern "C" void fn_801761E0()
{
    if (!sSpheresInitialized)
    {
        return;
    }

    for (int i = 0; i < 20; ++i)
    {
        if (sActiveSpheres[i] != 0)
        {
            delete sActiveSpheres[i];
            sActiveSpheres[i] = 0;
        }
    }

    sSpheresInitialized = false;
    PhysicsSphere_80175F8C::pool.FreeBlocks();
}

PhysicsSphere_80175F8C::~PhysicsSphere_80175F8C()
{
}

extern "C" void fn_801762F0(float dt)
{
    for (int i = 0; i < 20; ++i)
    {
        PhysicsSphere_80175F8C* sphere = sActiveSpheres[i];
        if (sphere == 0)
        {
            continue;
        }

        if ((!g_pGame->IsGameplayOrOvertime() && g_pGame->m_eGameState != 3)
            || sphere->finished)
        {
            delete sphere;
            sActiveSpheres[i] = 0;
            continue;
        }

        if (sphere->currentRadius >= sphere->maximumRadius)
        {
            sphere->finished = true;
            continue;
        }

        sphere->currentRadius += sphere->growthRate * dt;
        if (sphere->currentRadius >= sphere->maximumRadius)
        {
            sphere->currentRadius = sphere->maximumRadius + sRadiusOvershoot;
        }
        sphere->SetRadius(sphere->currentRadius);
    }
}

static inline PhysicsSphere_80175F8C* CreateSphere(const nlVector3& position,
    int effectType, float maximumRadius, float growthRate, void* owner)
{
    for (int i = 0; i < 20; ++i)
    {
        if (sActiveSpheres[i] == 0)
        {
            PhysicsSphere_80175F8C* sphere = new PhysicsSphere_80175F8C(owner, position, effectType, maximumRadius, growthRate);
            sActiveSpheres[i] = sphere;
            return sphere;
        }
    }
    return 0;
}

extern "C" PhysicsSphere_80175F8C* fn_8017642C(
    const nlVector3* position, cFielder* owner, bool frozen,
    int sourceIndex, float maximumRadius)
{
    PhysicsSphere_80175F8C* sphere = CreateSphere(*position, 0, maximumRadius, lbl_806DCAE8, owner);
    if (sphere != 0)
    {
        sphere->sourceIndex = sourceIndex;
        if (frozen)
        {
            sphere->effectType = 3;
        }
    }
    return sphere;
}

extern "C" PhysicsSphere_80175F8C* fn_801765C8(
    cFielder* owner, const nlVector3* position, float maximumRadius)
{
    return CreateSphere(*position, 1, maximumRadius, lbl_806DCAEC, owner);
}

extern "C" PhysicsSphere_80175F8C* fn_80176754(
    BulletBillObject* bulletBill)
{
    return CreateSphere(bulletBill->position, 4, lbl_806DCAD8, lbl_806DCADC, bulletBill->target);
}

extern "C" PhysicsSphere_80175F8C* fn_801768E0(
    const nlVector3* position, float maximumRadius)
{
    return CreateSphere(*position, 2, maximumRadius, lbl_806DCAF0, 0);
}

static char sBowserExplodeEffect[] = "bowser_explode";
static char sDaisyFistExitEffect[] = "daisy_fist_exit";

extern "C" PhysicsSphere_80175F8C* fn_80176A60(
    const nlVector3* position)
{
    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(sBowserExplodeEffect);
    if (group != 0)
    {
        EmissionController* controller = EmissionManager::Instance()->Create(group, 3, true, 0);
        controller->SetPosition(*position);
    }
    return CreateSphere(*position, 0, lbl_806DCAE0, lbl_806DCAF4, 0);
}

PhysicsSphere_80175F8C* CreateDaisyFistImpact(
    const nlVector3* position, cCharacter* owner)
{
    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(sDaisyFistExitEffect);
    if (group != 0)
    {
        EmissionController* controller = EmissionManager::Instance()->Create(group, 3, true, 0);
        controller->SetPosition(*position);
    }
    fn_800F026C(sDaisyCameraShake, lbl_806DCAFC, lbl_806DCB00);
    return CreateSphere(*position, 5, lbl_806DCAE4, lbl_806DCAF8, owner);
}

SlotPool<PhysicsSphere_80175F8C> PhysicsSphere_80175F8C::pool(16, 16);
