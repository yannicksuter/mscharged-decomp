#include "Game/Physics/PhysicsShell.h"
#include "Game/Terrain.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/BirdoEgg.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Net.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsNPC.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Render/NetMesh.h"
#include "Game/Render/SkinAnimatedMovableNPC.h"
#include "NL/nlSlotPool.h"
#include "Game/Render/KoopaShellObject.h"

#include <math.h>
#include "Game/UnidentifiedStaticStorage.h"


extern "C" bool fn_800167A8(cBall*);
extern "C" bool fn_800977A4(cFielder*, float);
extern "C" void fn_801473A4(CollisionPowerupGroundData*);
extern "C" void fn_801474EC(CollisionPowerupGroundData*);
extern "C" void fn_80147634(CollisionPowerupWallData*);

static const nlVector3 v3Unidentified = { 0.0f, 0.0f, 160.0f };
static const nlVector3 v3Direction = { 0.0f, 0.0f, 1.0f };

PhysicsShell::PhysicsShell(float radius)
    : PhysicsSphere(g_CollisionSpace, g_PhysicsWorld, radius)
{
    m_pTriggerCallbackFunc = 0;
    m_pCallbackParam = 0;
    m_pPowerupObject = 0;
    mbIsInNet = false;
    m_bIsSupportedByGround = false;
    mUnidentified046 = false;
    mUnidentified048 = 0;
    mUnidentified04C = 0.0f;

    SetCollide(0x1F062);
    SetCategory(0x2000);
    m_gravity = -16.0f;
}

void PhysicsShell::PreUpdate()
{
    PhysicsObject::PreUpdate();

    if (g_pBall->m_pPhysicsBall->mbUseTiltForce)
    {
        AddForceAtCentreOfMass(g_pBall->m_pPhysicsBall->mv3TiltForce);
    }
    if (g_pBall->m_pPhysicsBall->mbUseWindForce)
    {
        AddForceAtCentreOfMass(g_pBall->m_pPhysicsBall->mv3WindForce);
    }

    if (mUnidentified048 > 0)
    {
        nlVector3 velocity;
        GetLinearVelocity(&velocity);
        nlVec3Scale(velocity, mUnidentified04C);
        SetLinearVelocity(velocity);
    }
}

ContactType PhysicsShell::Contact(
    PhysicsObject* obj, dContact* info, int numContacts)
{
    nlVector3 myPos;
    cBall* ball;
    bool bWasRicochet;
    cFielder* pFielder;
    ContactType eType;

    GetPosition(&myPos);

    if (mUnidentified046)
    {
        return NO_CONTACT;
    }
    if (m_pPowerupObject->m_unk44.m_uPackedTime != 0)
    {
        return ONE_WAY_CONTACT_OTHER;
    }

    bWasRicochet = false;
    eType = TWO_WAY_CONTACT;

    switch (obj->GetObjectType())
    {
    case 4:
    {
        cCharacter* character
            = ((PhysicsCharacter*)obj->m_parentObject)->m_pAICharacter;
        if (character->m_eClassType == FIELDER)
        {
            pFielder = (cFielder*)character;
            if (!pFielder->mbTangible)
            {
                return NO_CONTACT;
            }

            PowerupBase* powerup = m_pPowerupObject;
            float upperHeight = powerup->m_v3Position.z + powerup->GetRadius();
            powerup = m_pPowerupObject;
            float lowerHeight = powerup->m_v3Position.z - powerup->GetRadius();
            if ((m_pPowerupObject->mtNoHitTimer.m_uPackedTime != 0
                    && m_pPowerupObject->m_pThrower == pFielder)
                || pFielder->IsCharacterInAir(upperHeight)
                || fn_800977A4(pFielder, lowerHeight))
            {
                return NO_CONTACT;
            }

            if (m_pTriggerCallbackFunc != 0)
            {
                nlVector3 v3Pos;
                nlVec3Set(v3Pos, info->geom.pos[0], info->geom.pos[1], info->geom.pos[2]);
                m_pTriggerCallbackFunc(
                    this, obj, v3Pos, m_pCallbackParam);
            }

            if (m_pPowerupObject->meSize == POWERUPSIZE_LARGE
                || m_pPowerupObject->GetType() == POWER_UP_SPINY_SHELL)
            {
                return NO_CONTACT;
            }
        }
        else
        {
            bWasRicochet = true;
            CollisionPowerupGroundData* eventData = 0;
            g_CollisionPowerupGroundDataPool.Allocate(eventData);
            GetPosition(&eventData->position);
            eventData->eType = m_pPowerupObject->GetType();
            fn_801474EC(eventData);
        }
        break;
    }

    case 0x10:
    {
        ball = ((PhysicsAIBall*)obj)->m_pAIBall;
        numContacts = eType;
        if (ball->m_pOwner != 0)
        {
            if (ball->m_pOwner->m_eClassType == FIELDER)
            {
                cFielder* pFielder = (cFielder*)ball->m_pOwner;
                if (!pFielder->mbTangible)
                {
                    return NO_CONTACT;
                }
                if (m_pPowerupObject->mtNoHitTimer.m_uPackedTime != 0
                    && m_pPowerupObject->m_pThrower == pFielder)
                {
                    return NO_CONTACT;
                }
            }
            else
            {
                bWasRicochet = true;
            }

            if (m_pPowerupObject->meSize == POWERUPSIZE_LARGE
                || m_pPowerupObject->GetType() == POWER_UP_SPINY_SHELL)
            {
                return NO_CONTACT;
            }
        }
        else
        {
            if (gNPCManager->mUnidentified02C != 0
                && gNPCManager->mUnidentified02C->mVisible)
            {
                return NO_CONTACT;
            }
            if (gNPCManager->mpBirdoEgg != 0
                && gNPCManager->mpBirdoEgg->mVisible)
            {
                return NO_CONTACT;
            }

            if (m_pPowerupObject->meSize != POWERUPSIZE_SMALL)
            {
                numContacts = ONE_WAY_CONTACT_OTHER;
            }
        }

        if (ball->m_tLightningTimer.m_uPackedTime != 0
            && fn_800167A8(ball))
        {
            return NO_CONTACT;
        }

        if (!bWasRicochet && m_pTriggerCallbackFunc != 0)
        {
            nlVector3 v3Pos;
            nlVec3Set(v3Pos, info->geom.pos[0], info->geom.pos[1], info->geom.pos[2]);
            m_pTriggerCallbackFunc(this, obj, v3Pos, m_pCallbackParam);
        }

        if (numContacts == ONE_WAY_CONTACT_OTHER)
        {
            ++ball->m_bBallDeflectCount;
            ++ball->m_bBallPathChangeCount;
            FakeBallWorld::InvalidateBallCache();
            return ONE_WAY_CONTACT_OTHER;
        }
        break;
    }

    case 0x14:
    {
        if (m_pPowerupObject->mtNoHitTimer.m_uPackedTime != 0)
        {
            if (((PhysicsShell*)obj)->m_pPowerupObject->m_pThrower
                == m_pPowerupObject->m_pThrower)
            {
                return NO_CONTACT;
            }
        }

        if (m_pTriggerCallbackFunc != 0)
        {
            nlVector3 v3Pos;
            nlVec3Set(v3Pos, info->geom.pos[0], info->geom.pos[1], info->geom.pos[2]);
            m_pTriggerCallbackFunc(this, obj, v3Pos, m_pCallbackParam);
        }

        ePowerupSize otherShellSize
            = ((PhysicsShell*)obj)->m_pPowerupObject->meSize;
        ePowerupSize myShellSize = m_pPowerupObject->meSize;
        if (myShellSize > otherShellSize)
        {
            return NO_CONTACT;
        }
        break;
    }

    case 0x15:
    {
        if (m_pPowerupObject->mtNoHitTimer.m_uPackedTime != 0)
        {
            if (((PhysicsBanana*)obj)->m_pPowerupObject->m_pThrower
                == m_pPowerupObject->m_pThrower)
            {
                return NO_CONTACT;
            }
        }

        if (m_pTriggerCallbackFunc != 0)
        {
            nlVector3 v3Pos;
            nlVec3Set(v3Pos, info->geom.pos[0], info->geom.pos[1], info->geom.pos[2]);
            m_pTriggerCallbackFunc(this, obj, v3Pos, m_pCallbackParam);
        }

        if (((PhysicsBanana*)obj)->m_pPowerupObject->meSize
            != POWERUPSIZE_LARGE)
        {
            return NO_CONTACT;
        }
        break;
    }

    case 0x18:
    {
        bool isChainChomp
            = ((SkinAnimatedNPC*)((PhysicsNPC*)obj)->mpAINPC)
                  ->GetSkinAnimatedNPC_Type()
           == SkinAnimatedNPC_CHAIN_CHOMP;
        if (isChainChomp)
        {
            QueueCollisionChainCrowd(
                (UnidentifiedEventData28*)((PhysicsNPC*)obj)->mpAINPC);
        }
        break;
    }

    case 0x1D:
        return ONE_WAY_CONTACT_THIS;

    case 0x1C:
    {
        int value = ((PhysicsPatch*)obj)->m_Type;
        PhysicsPatchInfo* patchInfo = GetPhysicsPatchInfo(value);
        if (patchInfo->mFriction != 0.0f)
        {
            if (mUnidentified048 != 2
                || patchInfo->mFriction > mUnidentified04C)
            {
                mUnidentified04C = patchInfo->mFriction;
            }
            mUnidentified048 = 2;
        }

        if (patchInfo->mType == 8
            || patchInfo->mType == 9)
        {
            m_pPowerupObject->m_bShouldDestroy = true;
        }
        return ONE_WAY_CONTACT_THIS;
    }

    default:
    {
        if (obj->GetObjectType() == 0x12)
        {
            if (mUnidentified046)
            {
                return NO_CONTACT;
            }
            for (int i = 0; i < numContacts; i++)
            {
                if (info[i].geom.pos[2] <= myPos.z
                    && info[i].geom.normal[2] > 0.9f)
                {
                    if (!m_bIsSupportedByGround)
                    {
                        m_bIsSupportedByGround = true;
                    }

                    if (!mbIsInNet
                        && m_pPowerupObject->mtNoHitTimer.m_uPackedTime == 0)
                    {
                        nlVector3 v3IncidentVel;
                        GetLinearVelocity(&v3IncidentVel);
                        if (v3IncidentVel.z < -1.0f)
                        {
                            CollisionPowerupGroundData* eventData = 0;
                            g_CollisionPowerupGroundDataPool.Allocate(eventData);
                            GetPosition(&eventData->position);
                            eventData->fVecZComponent = v3IncidentVel.z;
                            eventData->eType = m_pPowerupObject->GetType();
                            fn_801473A4(eventData);
                        }
                    }
                    break;
                }
            }
        }

        nlVector3 v3PowerupPosition;
        GetPosition(&v3PowerupPosition);
        float fPowerupRadius = GetRadius();

        if (mbIsInNet)
        {
            if (fabsf(v3PowerupPosition.y) > 0.5f * cNet::GetNetWidth() - fPowerupRadius
                && fabsf(v3PowerupPosition.x)
                       > fabsf(cField::GetGoalLineX(1U)))
            {
                m_pPowerupObject->m_bShouldDestroy = true;
                return NO_CONTACT;
            }

            if (!NetMesh::s_bAnimatedNetMeshEnabled)
            {
                return NO_CONTACT;
            }

            float fMaxX;
            if (v3PowerupPosition.x > 0.0f)
            {
                fMaxX = NetMesh::spPositiveXNetMesh->GetOuterX(v3PowerupPosition);
            }
            else
            {
                fMaxX = NetMesh::spNegativeXNetMesh->GetOuterX(v3PowerupPosition);
            }

            if (fabsf(v3PowerupPosition.x)
                > fabsf(fMaxX) - 2.0f * fPowerupRadius)
            {
                m_pPowerupObject->m_bShouldDestroy = true;
                return NO_CONTACT;
            }

            if (fabsf(v3PowerupPosition.z) > cNet::GetNetHeight() - fPowerupRadius)
            {
                m_pPowerupObject->m_bShouldDestroy = true;
                return NO_CONTACT;
            }

            if (fabsf(v3PowerupPosition.x)
                <= cField::GetGoalLineX(1U) - fPowerupRadius)
            {
                mbIsInNet = false;
            }
        }

        if (fabsf(v3PowerupPosition.y) < 0.5f * cNet::GetNetWidth() - fPowerupRadius
            && fabsf(v3PowerupPosition.x)
                   > cField::GetGoalLineX(1U) - fPowerupRadius
            && fabsf(v3PowerupPosition.z) < cNet::GetNetHeight() - fPowerupRadius)
        {
            mbIsInNet = true;
            return NO_CONTACT;
        }

        if (obj->GetObjectType() == 0x17 || obj->GetObjectType() == 5)
        {
            for (int i = 0; i < numContacts; i++)
            {
                if (info[i].geom.normal[2] < 0.08f)
                {
                    if (m_pPowerupObject->mtActiveTimer.m_uPackedTime == 0)
                    {
                        m_pPowerupObject->m_bShouldDestroy = true;
                    }
                    else if (!mbIsInNet)
                    {
                        bWasRicochet = true;
                    }
                }
            }

            if (bWasRicochet
                && GameInfoManager::Instance()->GetStadium() == 0x0B)
            {
                nlVector3 contactPos;
                nlVec3Set(contactPos, info->geom.pos[0], info->geom.pos[1], info->geom.pos[2]);
                float height
                    = contactPos.z - m_pPowerupObject->GetRadius();
                bool belowGroundAndSmall = height < 0.36f;
                belowGroundAndSmall = belowGroundAndSmall
                    && m_pPowerupObject->meSize == POWERUPSIZE_SMALL;
                float sidelineDistance
                    = fabsf(contactPos.y) - m_pPowerupObject->GetRadius();
                bool beyondSideline
                    = sidelineDistance > cField::GetSidelineY(1U);
                bool insideGoalLine
                    = fabsf(contactPos.x) < cField::GetGoalLineX(1U) - 0.5f;
                if (insideGoalLine && !belowGroundAndSmall)
                {
                    if (beyondSideline
                        && m_pPowerupObject->mtNoHitTimer.m_uPackedTime == 0)
                    {
                        if (height < 0.72f)
                        {
                            nlVector3 v3Force = v3Unidentified;
                            AddForceAtCentreOfMass(v3Force);
                        }
                        mUnidentified046 = true;
                    }
                    return NO_CONTACT;
                }
            }
        }
        break;
    }
    }

    if (bWasRicochet)
    {
        if (mUnidentified046)
        {
            eType = NO_CONTACT;
        }
        else
        {
            if (obj->GetObjectType() == 0x17
                || obj->GetObjectType() == 5)
            {
                CollisionPowerupWallData* eventData = 0;
                g_CollisionPowerupWallDataPool.Allocate(eventData);
                eventData->eSize = m_pPowerupObject->meSize;
                eventData->eType = m_pPowerupObject->GetType();
                nlVec3Set(eventData->position, info->geom.pos[0], info->geom.pos[1], info->geom.pos[2]);
                nlVec3Set(eventData->normal, info->geom.normal[0], info->geom.normal[1], info->geom.normal[2]);
                fn_80147634(eventData);

                if ((m_pPowerupObject->GetType() == POWER_UP_GREEN_SHELL
                        || m_pPowerupObject->GetType() == POWER_UP_RED_SHELL)
                    && m_pPowerupObject->meSize == POWERUPSIZE_SMALL)
                {
                    m_pPowerupObject->m_bShouldDestroy = true;
                }
            }

            if (obj->GetObjectType() != 0x10
                || (obj->GetObjectType() == 0x10
                    && m_pPowerupObject->m_pTarget != 0))
            {
                EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("shell_ricochet");
                EmissionController* pControl = EmissionManager::Instance()->Create(pGroup, 3, true, 0);

                nlVector3 v3Position;
                nlVector3 v3Velocity;
                nlVector3 v3DirectionCopy = v3Direction;
                GetPosition(&v3Position);
                GetLinearVelocity(&v3Velocity);

                pControl->SetPosition(v3Position);
                pControl->SetDirection(v3DirectionCopy);
                pControl->SetVelocity(v3Velocity);
            }

            m_pPowerupObject->m_pTarget = 0;
        }
    }

    return eType;
}

void PhysicsShell::PostUpdate()
{
    PhysicsObject::PostUpdate();

    nlVector3 velocity;
    GetLinearVelocity(&velocity);

    nlVector3& pos = GetPosition();
    if (pos.z > 20.0f && velocity.z > 0.0f)
    {
        velocity.z *= 0.9f;
        SetLinearVelocity(velocity);
    }

    if (mUnidentified048 > 0)
    {
        nlVector3 scaledVelocity;
        GetLinearVelocity(&scaledVelocity);
        nlVec3Scale(scaledVelocity, 1.0f / mUnidentified04C);
        SetLinearVelocity(scaledVelocity);
        if (--mUnidentified048 == 0)
        {
            mUnidentified04C = 0.0f;
        }
    }
}

bool PhysicsShell::SetContactInfo(
    dContact* contact, PhysicsObject* other, bool first)
{
    if (first)
    {
        SetDefaultContactInfo(contact);
    }

    if (other->GetObjectType() == 0x12)
    {
        contact->surface.bounce = g_pGame->mpTerrain->GetRestitution(gGameTweaks.m_pGameTweaks->fShellBounceGround);
        contact->surface.mu = 0.005f;
    }
    else
    {
        contact->surface.bounce
            = gGameTweaks.m_pGameTweaks->fShellBounce;
        contact->surface.mu = 0.005f;
    }

    contact->surface.bounce_vel = 0.0f;
    return true;
}
