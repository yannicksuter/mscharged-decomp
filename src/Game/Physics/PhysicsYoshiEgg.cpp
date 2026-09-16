#include "Game/Physics/PhysicsYoshiEgg.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/EventDataTypes.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/PhysicsHammer.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Physics/PhysicsThwomp.h"
#include "Game/Render/HammerObject.h"
#include "Game/Render/ThwompObject.h"
#include "Game/Render/YoshiEggObject.h"
#include "Game/UnidentifiedStaticStorage.h"

extern "C" bool fn_800167A8(cBall*);

PhysicsYoshiEgg::PhysicsYoshiEgg(YoshiEggObject* egg, float radius)
    : PhysicsSphere(g_CollisionSpace, 0, radius)
    , mUnidentified38(0)
    , mYoshiEgg(egg)
{
    SetCategory(0x4000);
    SetCollide(0x1F060);
}

ContactType PhysicsYoshiEgg::Contact(PhysicsObject* other, dContact* contact, int)
{
    nlVector3 eggPosition;
    UnidentifiedEventData34* eventData;
    UnidentifiedEventData34* crackData;
    UnidentifiedEventData24* patchData;
    GetPosition(&eggPosition);

    bool isTimerRunning = mYoshiEgg->mDelay > 0.0f;
    if (isTimerRunning)
    {
        return ONE_WAY_CONTACT_OTHER;
    }

    switch (other->GetObjectType())
    {
    case 4:
    {
        cCharacter* character
            = ((PhysicsCharacter*)other->m_parentObject)->m_pAICharacter;
        cFielder* player = mYoshiEgg->mFielder;
        if (character == player)
        {
            return NO_CONTACT;
        }

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

            eventData = 0;
            lbl_80570188.Allocate(eventData);
            eventData->mUnidentified00 = fielder;
            eventData->mUnidentified04 = player;
            eventData->mUnidentified08 = mYoshiEgg;
            eventData->mUnidentified0C = 0;
            eventData->mUnidentified10 = 0;
            QueueCollisionEggPlayer(eventData);

            if (fielder->fn_8003E74C())
            {
                return ONE_WAY_CONTACT_OTHER;
            }
        }
        else if (!player->IsOnSameTeam((cPlayer*)character))
        {
            crackData = 0;
            lbl_80570188.Allocate(crackData);
            crackData->mUnidentified00 = (cPlayer*)character;
            crackData->mUnidentified04 = player;
            crackData->mUnidentified08 = mYoshiEgg;
            crackData->mUnidentified0C = 0;
            crackData->mUnidentified10 = 0;
            QueueCollisionCrackEgg(crackData);
        }
        else
        {
            return TWO_WAY_CONTACT;
        }
        break;
    }
    case 0x10:
    {
        cPlayer* owner = ((PhysicsAIBall*)other)->m_pAIBall->m_pOwner;
        cFielder* player = mYoshiEgg->mFielder;
        if (owner != 0)
        {
            if (player == owner)
            {
                return NO_CONTACT;
            }

            if (owner->m_eClassType == FIELDER)
            {
                if (!((cFielder*)owner)->mbTangible)
                {
                    return NO_CONTACT;
                }
                if (owner->IsCharacterInAir(GetPosition().z + GetRadius()))
                {
                    return NO_CONTACT;
                }

                eventData = 0;
                lbl_80570188.Allocate(eventData);
                eventData->mUnidentified00 = owner;
                eventData->mUnidentified04 = player;
                eventData->mUnidentified08 = mYoshiEgg;
                eventData->mUnidentified0C = 0;
                eventData->mUnidentified10 = 0;
                QueueCollisionEggPlayer(eventData);
            }
            else if (!player->IsOnSameTeam(owner))
            {
                crackData = 0;
                lbl_80570188.Allocate(crackData);
                crackData->mUnidentified00 = owner;
                crackData->mUnidentified04 = player;
                crackData->mUnidentified08 = mYoshiEgg;
                crackData->mUnidentified0C = 0;
                crackData->mUnidentified10 = 0;
                QueueCollisionCrackEgg(crackData);
            }
            else
            {
                return TWO_WAY_CONTACT;
            }
            break;
        }

        if (!fn_800167A8(g_pBall))
        {
            eventData = 0;
            lbl_80570188.Allocate(eventData);
            eventData->mUnidentified00 = 0;
            eventData->mUnidentified04 = player;
            eventData->mUnidentified08 = mYoshiEgg;
            eventData->mUnidentified0C = 0;
            eventData->mUnidentified10 = 0;
            QueueCollisionEggBall(eventData);
            return ONE_WAY_CONTACT_OTHER;
        }
        return NO_CONTACT;
    }
    case 0x15:
        ((PhysicsBanana*)other)->m_pPowerupObject->m_bShouldDestroy = true;
        return NO_CONTACT;
    case 0x14:
        return ONE_WAY_CONTACT_OTHER;
    case 0x24:
    {
        if (((PhysicsThwomp*)other)->mThwomp->mState == THWOMP_STATE_FALLING)
        {
            crackData = 0;
            lbl_80570188.Allocate(crackData);
            crackData->mUnidentified00 = 0;
            crackData->mUnidentified04 = mYoshiEgg->mFielder;
            crackData->mUnidentified08 = mYoshiEgg;
            crackData->mUnidentified0C = 0;
            crackData->mUnidentified10 = 0;
            QueueCollisionCrackEgg(crackData);
        }

        nlVector3 normal;
        normal.x = contact->geom.normal[0];
        normal.y = contact->geom.normal[1];
        normal.z = contact->geom.normal[2];
        mYoshiEgg->SetPendingDisplacement(normal, contact->geom.depth);
        return ONE_WAY_CONTACT_THIS;
    }
    case 0x1C:
    {
        PhysicsPatch* patch = (PhysicsPatch*)other;
        int type = patch->GetType();
        PhysicsPatchInfo* info = GetPhysicsPatchInfo(type);
        if (patch->GetType() == 1 || patch->GetType() == 8 || patch->GetType() == 9)
        {
            crackData = 0;
            lbl_80570188.Allocate(crackData);
            crackData->mUnidentified00 = 0;
            crackData->mUnidentified04 = mYoshiEgg->mFielder;
            crackData->mUnidentified08 = mYoshiEgg;
            crackData->mUnidentified0C = 0;
            crackData->mUnidentified10 = 0;
            QueueCollisionCrackEgg(crackData);
        }
        else if (info->mFriction != 0.0f)
        {
            patchData = 0;
            lbl_80570138.Allocate(patchData);
            patchData->mUnidentified0C = mYoshiEgg->mFielder;
            patchData->mUnidentified10 = patch;
            QueueCollisionPatchPlayer(patchData);
        }
        return NO_CONTACT;
    }
    case 0x1F:
    {
        HammerObject* hammer = ((PhysicsHammer*)other)->mHammer;
        bool isLanded = hammer->mLandedTimer > 0.0f;
        if (isLanded)
        {
            hammer->Deactivate(true);
            break;
        }

        crackData = 0;
        lbl_80570188.Allocate(crackData);
        crackData->mUnidentified00 = 0;
        crackData->mUnidentified04 = mYoshiEgg->mFielder;
        crackData->mUnidentified08 = mYoshiEgg;
        crackData->mUnidentified0C = 0;
        crackData->mUnidentified10 = 0;
        QueueCollisionCrackEgg(crackData);
        break;
    }
    case 0x18:
    case 0x1E:
    {
        crackData = 0;
        lbl_80570188.Allocate(crackData);
        crackData->mUnidentified00 = 0;
        crackData->mUnidentified04 = mYoshiEgg->mFielder;
        crackData->mUnidentified08 = mYoshiEgg;
        crackData->mUnidentified0C = 0;
        crackData->mUnidentified10 = 0;
        QueueCollisionCrackEgg(crackData);
        break;
    }
    }

    return NO_CONTACT;
}

bool PhysicsYoshiEgg::SetContactInfo(
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

void PhysicsYoshiEgg::PreCollide()
{
}

int PhysicsYoshiEgg::GetObjectType() const
{
    return 0x20;
}

PhysicsYoshiEgg::~PhysicsYoshiEgg()
{
}
