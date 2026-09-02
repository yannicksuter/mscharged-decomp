#include "Game/BulletBill.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Game.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsSphere.h"
#include "NL/nlSlotPool.h"

#include "types.h"

class cFielder;
class EffectsGroup;

extern "C" bool fn_800976F8(cFielder*, float);
extern "C" bool fn_800977A4(cFielder*, float);
extern "C" void fn_800F026C(
    const nlVector3&, float, float);
extern "C" EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);
extern "C" EmissionController* fn_802E7FE4(
    EmissionManager*, EffectsGroup*, int, bool, bool);

struct PhysicsEvent_8014A2BC
{
    PhysicsObject* source;
    PhysicsObject* target;
    u32 unknown08;
};

extern "C" SlotPool<PhysicsEvent_8014A2BC> lbl_805701B0;
extern "C" void fn_8014A2BC(PhysicsEvent_8014A2BC*);

struct PhysicsObjectType28_80510968 : PhysicsObject
{
    /* 0x38 */ u8 unknown38[0x10];
    /* 0x48 */ u32 unknown48;
};

static const float sInitialRadius = 0.5f;
static const float sRadiusOvershoot = 0.01f;

class PhysicsSphere_80175F8C : public PhysicsSphere
{
public:
    PhysicsSphere_80175F8C(void* owner, const nlVector3& position,
        int effectType, float maximumRadius, float growthRate)
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

    virtual ~PhysicsSphere_80175F8C();
    virtual int GetObjectType() const { return 0x23; }
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    static void* operator new(unsigned long)
    {
        PhysicsSphere_80175F8C* object = 0;
        pool.Allocate(object);
        return object;
    }

    static void operator delete(void* object)
    {
        pool.Free((PhysicsSphere_80175F8C*)object);
    }

    static SlotPool<PhysicsSphere_80175F8C> pool;

    /* 0x38 */ void* owner;
    /* 0x3C */ nlVector3 position;
    /* 0x48 */ int effectType;
    /* 0x4C */ float currentRadius;
    /* 0x50 */ float maximumRadius;
    /* 0x54 */ float growthRate;
    /* 0x58 */ int sourceIndex;
    /* 0x5C */ bool finished;
};

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

static inline void QueuePhysicsEvent(
    PhysicsSphere_80175F8C* source, PhysicsObject* target)
{
    PhysicsEvent_8014A2BC* event = 0;
    lbl_805701B0.Allocate(event);
    event->source = source;
    event->target = target;
    fn_8014A2BC(event);
}

ContactType PhysicsSphere_80175F8C::Contact(
    PhysicsObject* other, dContact*, int)
{
    nlVector3 spherePosition;
    GetPosition(&spherePosition);

    switch (other->GetObjectType())
    {
    case 4:
    {
        cFielder* fielder =
            (cFielder*)((PhysicsCharacter*)other->m_parentObject)->m_pAICharacter;
        float radius = GetRadius();
        if (fn_800976F8(fielder, GetPosition().z + radius))
        {
            return NO_CONTACT;
        }

        radius = GetRadius();
        if (fn_800977A4(fielder, GetPosition().z - radius))
        {
            return NO_CONTACT;
        }

        QueuePhysicsEvent(this, other);
        break;
    }
    case 16:
    case 20:
    case 21:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
        QueuePhysicsEvent(this, other);
        break;
    case 24:
        return ONE_WAY_CONTACT_OTHER;
    case 28:
        if (((PhysicsObjectType28_80510968*)other)->unknown48 == 0)
        {
            QueuePhysicsEvent(this, other);
        }
        break;
    default:
        break;
    }

    return NO_CONTACT;
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

        int state = g_pGame->m_eGameState;
        if ((state != 5 && state != 6 && state != 3) || sphere->finished)
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

static inline PhysicsSphere_80175F8C* CreateSphere(void* owner,
    const nlVector3& position, int effectType, float maximumRadius,
    float growthRate)
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
    PhysicsSphere_80175F8C* sphere = CreateSphere(
        owner, *position, 0, maximumRadius, lbl_806DCAE8);
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
    return CreateSphere(
        owner, *position, 1, maximumRadius, lbl_806DCAEC);
}

extern "C" PhysicsSphere_80175F8C* fn_80176754(
    BulletBillObject* bulletBill)
{
    return CreateSphere(bulletBill->target, bulletBill->position, 4,
        lbl_806DCAD8, lbl_806DCADC);
}

extern "C" PhysicsSphere_80175F8C* fn_801768E0(
    const nlVector3* position, float maximumRadius)
{
    return CreateSphere(
        0, *position, 2, maximumRadius, lbl_806DCAF0);
}

static char sBowserExplodeEffect[] = "bowser_explode";
static char sDaisyFistExitEffect[] = "daisy_fist_exit";

static inline void EmitSphereEffect(
    const char* name, const nlVector3& position)
{
    EffectsGroup* group = fn_802E7CDC(EmissionManager::Instance(), name);
    if (group != 0)
    {
        EmissionController* controller = fn_802E7FE4(
            EmissionManager::Instance(), group, 3, true, false);
        controller->SetPosition(position);
    }
}

extern "C" PhysicsSphere_80175F8C* fn_80176A60(
    const nlVector3* position)
{
    EmitSphereEffect(sBowserExplodeEffect, *position);
    return CreateSphere(
        0, *position, 0, lbl_806DCAE0, lbl_806DCAF4);
}

extern "C" PhysicsSphere_80175F8C* fn_80176C18(
    const nlVector3* position, cCharacter* owner)
{
    EmitSphereEffect(sDaisyFistExitEffect, *position);
    fn_800F026C(sDaisyCameraShake, lbl_806DCAFC, lbl_806DCB00);
    return CreateSphere(
        owner, *position, 5, lbl_806DCAE4, lbl_806DCAF8);
}

SlotPool<PhysicsSphere_80175F8C> PhysicsSphere_80175F8C::pool(16, 16);
