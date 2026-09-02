#include "Game/Physics/PhysicsBirdoEgg.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsNPC.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "NL/nlSlotPool.h"

struct BirdoEggObject
{
    u8 mUnidentified00[0x3C];
    cFielder* mShooter;
};

struct CollisionBirdoShotBallPlayerData
{
    BirdoEggObject* egg;
    cFielder* player;
};

struct CollisionBirdoEggGoalieData
{
    BirdoEggObject* egg;
    cCharacter* goalie;
};

struct CollisionBirdoEggEndData
{
    BirdoEggObject* egg;
    bool cracked;
};

struct CollisionCrackEggData
{
    void* mUnidentified00;
    cFielder* player;
    void* source;
    void* mUnidentified0C;
    void* mUnidentified10;
};

extern SlotPool<CollisionBirdoShotBallPlayerData> lbl_80571578;
extern SlotPool<CollisionBirdoEggGoalieData> lbl_805715A0;
extern SlotPool<CollisionBirdoEggEndData> lbl_805715F0;
extern SlotPool<CollisionCrackEggData> lbl_80570188;

extern "C" bool fn_8003886C(cFielder*);
extern "C" bool fn_8003E73C(cFielder*);
extern "C" bool fn_800976F8(cFielder*, float);
extern "C" void fn_80146FCC(CollisionBirdoShotBallPlayerData*);
extern "C" void fn_80147114(CollisionBirdoEggGoalieData*);
extern "C" void fn_8014725C(CollisionBirdoEggEndData*);
extern "C" void fn_801490E0(PowerupBase*);
extern "C" void fn_80149358(void*);
extern "C" void fn_801495D0(cFielder*);
extern "C" void fn_80149EFC(CollisionCrackEggData*);

static inline void QueueBirdoEggEnd(BirdoEggObject* egg, bool cracked)
{
    CollisionBirdoEggEndData* eventData = 0;
    lbl_805715F0.Allocate(eventData);
    eventData->egg = egg;
    eventData->cracked = cracked;
    fn_8014725C(eventData);
}

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

            float radius = GetRadius();
            if (fn_800976F8(fielder, GetPosition().z + radius))
            {
                return NO_CONTACT;
            }
            if (fielder == mBirdoEgg->mShooter)
            {
                break;
            }

            CollisionBirdoShotBallPlayerData* eventData = 0;
            lbl_80571578.Allocate(eventData);
            eventData->player = fielder;
            eventData->egg = mBirdoEgg;
            fn_80146FCC(eventData);

            if (!fn_8003E73C(fielder)
                && mBirdoEgg->mShooter->m_pBall == 0)
            {
                QueueBirdoEggEnd(mBirdoEgg, false);
            }
        }
        else
        {
            CollisionBirdoEggGoalieData* eventData = 0;
            lbl_805715A0.Allocate(eventData);
            eventData->goalie = character;
            eventData->egg = mBirdoEgg;
            fn_80147114(eventData);
        }
        break;
    }
    case 0x14:
    case 0x15:
    {
        PowerupBase* powerup = other->GetObjectType() == 0x14
                                 ? ((PhysicsShell*)other)->m_pPowerupObject
                                 : ((PhysicsBanana*)other)->m_pPowerupObject;
        if (powerup->m_pThrower == mBirdoEgg->mShooter
            && powerup->mtNoHitTimer.m_uPackedTime != 0)
        {
            return NO_CONTACT;
        }

        fn_801490E0(powerup);
        if (other->GetObjectType() == 0x15
            && powerup->m_eType == POWER_UP_BOBOMB)
        {
            QueueBirdoEggEnd(mBirdoEgg, false);
        }
        break;
    }
    case 0x18:
    {
        SkinAnimatedNPC* npc
            = (SkinAnimatedNPC*)((PhysicsNPC*)other)->mpAINPC;
        if (npc->GetSkinAnimatedNPC_Type() == SkinAnimatedNPC_CHAIN_CHOMP)
        {
            QueueBirdoEggEnd(mBirdoEgg, true);
        }
        if (npc->GetSkinAnimatedNPC_Type() == SkinAnimatedNPC_BOWSER)
        {
            QueueBirdoEggEnd(mBirdoEgg, true);
        }
        break;
    }
    case 0x1C:
    {
        u8* object = (u8*)other;
        if (object[0x65] != 0)
        {
            break;
        }

        switch (*(u32*)(object + 0x48))
        {
        case 1:
        case 3:
        case 8:
            QueueBirdoEggEnd(mBirdoEgg, true);
            break;
        case 6:
        {
            cFielder* fielder = *(cFielder**)(object + 0x4C);
            bool protectedFromEgg = false;
            if (!fn_8003886C(fielder)
                && (*(u32*)((u8*)fielder + 0x454) & 0x1F) == 0x1F)
            {
                protectedFromEgg = true;
            }
            if (!protectedFromEgg)
            {
                fn_801495D0(fielder);
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case 0x1E:
    case 0x23:
        QueueBirdoEggEnd(mBirdoEgg, false);
        break;
    case 0x1F:
    {
        u8* hammer = *(u8**)((u8*)other + 0x38);
        if (*(cFielder**)(hammer + 0x34) == mBirdoEgg->mShooter
            && *(float*)(hammer + 0x1C) > 0.0f)
        {
            return NO_CONTACT;
        }
        fn_80149358(hammer);
        break;
    }
    case 0x20:
    {
        u8* source = *(u8**)((u8*)other + 0x3C);
        if (*(bool*)(source + 0x28))
        {
            QueueBirdoEggEnd(mBirdoEgg, true);

            CollisionCrackEggData* eventData = 0;
            lbl_80570188.Allocate(eventData);
            eventData->mUnidentified00 = 0;
            eventData->player = *(cFielder**)(source + 0x34);
            eventData->source = source;
            eventData->mUnidentified0C = 0;
            eventData->mUnidentified10 = 0;
            fn_80149EFC(eventData);
        }
        break;
    }
    case 0x24:
        QueueBirdoEggEnd(mBirdoEgg, true);
        break;
    default:
        break;
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

PhysicsBirdoEgg::~PhysicsBirdoEgg()
{
}
