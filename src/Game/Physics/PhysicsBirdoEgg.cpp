#include "Game/Physics/PhysicsBirdoEgg.h"

#include "Game/AI/Fielder.h"
#include "Game/BirdoEggObject.h"
#include "Game/AI/Powerups.h"
#include "Game/EventDataTypes.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsNPC.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "NL/nlSlotPool.h"

struct CollisionCrackEggData
{
    void* mUnidentified00;
    cFielder* player;
    void* source;
    void* mUnidentified0C;
    void* mUnidentified10;
};

extern SlotPool<CollisionCrackEggData> lbl_80570188;

extern "C" bool fn_8003E73C(cFielder*);
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
    g_CollisionBirdoEggEndDataPool.Allocate(eventData);
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
            if (fielder->IsCharacterInAir(GetPosition().z + radius))
            {
                return NO_CONTACT;
            }
            if (fielder == mBirdoEgg->unknown_3C)
            {
                break;
            }

            CollisionBirdoShotBallPlayerData* eventData = 0;
            g_CollisionBirdoShotBallPlayerDataPool.Allocate(eventData);
            eventData->player = fielder;
            eventData->egg = mBirdoEgg;
            fn_80146FCC(eventData);

            if (!fn_8003E73C(fielder)
                && mBirdoEgg->unknown_3C->m_pBall == 0)
            {
                QueueBirdoEggEnd(mBirdoEgg, false);
            }
        }
        else
        {
            CollisionBirdoEggGoalieData* eventData = 0;
            g_CollisionBirdoEggGoalieDataPool.Allocate(eventData);
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
        if (powerup->m_pThrower == mBirdoEgg->unknown_3C
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
            if (!fielder->IsStuck()
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
        if (*(cFielder**)(hammer + 0x34) == mBirdoEgg->unknown_3C
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
