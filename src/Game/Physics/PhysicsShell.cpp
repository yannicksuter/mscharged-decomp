#include "Game/Physics/PhysicsShell.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Net.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsNPC.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Render/NetMesh.h"
#include "Game/Render/SkinAnimatedMovableNPC.h"
#include "NL/nlSlotPool.h"
#include "unclassified/tu_801A5F10.h"

#include <math.h>

struct CollisionPowerupGroundData
{
    nlVector3 position;
    float fVecZComponent;
    ePowerUpType eType;
};

struct CollisionPowerupWallData
{
    PowerupBase* pPowerup;
    ePowerupSize eSize;
    ePowerUpType eType;
    nlVector3 position;
    nlVector3 normal;
};

extern SlotPool<CollisionPowerupGroundData> lbl_80571460;
extern SlotPool<CollisionPowerupWallData> lbl_805714B0;
extern void* lbl_806E1608;

extern "C" bool fn_800167A8(cBall*);
extern "C" bool fn_800976F8(cFielder*, float);
extern "C" bool fn_800977A4(cFielder*, float);
extern "C" float fn_800A92C8(void*, float);
extern "C" void fn_801473A4(CollisionPowerupGroundData*);
extern "C" void fn_801474EC(CollisionPowerupGroundData*);
extern "C" void fn_80147634(CollisionPowerupWallData*);
extern "C" void fn_801481BC(SkinAnimatedNPC*);
extern "C" EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);
extern "C" EmissionController* fn_802E7FE4(
    EmissionManager*, EffectsGroup*, int, bool, bool);

static const nlVector3 v3Unidentified = { 0.0f, 0.0f, 160.0f };
static const nlVector3 v3Direction = { 0.0f, 0.0f, 1.0f };

UnidentifiedPhysicsPatchInfo_80510BF0 lbl_80510BF0[13] = {
    { 0, "GAS BALL", "gas_cloud", 0x13062, 0, 3.0f, 0.0f, 0.1f },
    { 1, "FIRE BALL", "bowser_fire", 0x40, 0, 0.001f, 0.0f, 0.0f },
    { 2, "HEAVENLY LIGHT", "heavenly_light", 0x40, 0, 0.0f, 0.0f, 0.001f },
    { 3, "MUCK BALL", "fx_mud_ball", 0x202, 0, 8.0f, 0.0f, 0.0f },
    { 4, "MUCK HOLE", "fx_mud_hole", 0x60, 0, 0.0f, 0.15f, 0.0f },
    { 5, "YOSHI YOKE", "yoshi_yoke", 0x60, 0, 0.0f, 0.15f, 0.0f },
    { 6, "YOSHI TONGUE", 0, 0x40, 0, 0.0f, 0.0f, 0.0f },
    { 7, "SHRINKER", "shrill_shrinker", 0x40, 0, 0.0f, 0.0f, 0.0f },
    { 8, "LAVA BALL", "fx_lava_ball", 0x2, 0, 8.0f, 0.0f, 0.0f },
    { 9, "LAVA HOLE", "fx_lava_hole", 0x60, 0, 0.0f, 0.0f, 0.0f },
    { 10, "CHAINLIGHTNING", "fx_electric_ground_effect", 0x40, 0, 0.0f, 0.0f, 0.0f },
    { 11, "SANDPATCH", 0, 0x40, 0, 0.0f, 0.6f, 0.0f },
    { 12, "SPEEDER", "yoshi_yoke", 0x40, 0, 0.0f, 1.5f, 0.001f },
};

PhysicsSphere_80174F04::PhysicsSphere_80174F04(float radius)
    : PhysicsSphere(g_CollisionSpace, 0, radius)
{
    m_pTriggerCallbackFunc = 0;
    m_pCallbackParam = 0;
    SetCollide(0x60);
    SetCategory(2);
}

ContactType PhysicsSphere_80174F04::Contact(
    PhysicsObject* other, dContact* info, int numContacts)
{
    nlVector3 position;
    nlVec3Set(position, info->geom.pos[0], info->geom.pos[1], info->geom.pos[2]);

    switch (other->GetObjectType())
    {
    case 0x18:
    {
        bool isChainChomp
            = ((SkinAnimatedNPC*)((PhysicsNPC*)other)->mpAINPC)
                  ->GetSkinAnimatedNPC_Type()
           == SkinAnimatedNPC_CHAIN_CHOMP;
        if (isChainChomp)
        {
            fn_801481BC(
                (SkinAnimatedNPC*)((PhysicsNPC*)other)->mpAINPC);
        }
        break;
    }
    default:
    {
        if (m_pTriggerCallbackFunc != 0)
        {
            m_pTriggerCallbackFunc(
                this, other, position, m_pCallbackParam);
        }
        break;
    }
    }

    return NO_CONTACT;
}

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
                || fn_800976F8(pFielder, upperHeight)
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
                || m_pPowerupObject->m_eType == POWER_UP_SPINY_SHELL)
            {
                return NO_CONTACT;
            }
        }
        else
        {
            bWasRicochet = true;
            CollisionPowerupGroundData* eventData = 0;
            lbl_80571460.Allocate(eventData);
            GetPosition(&eventData->position);
            eventData->eType = m_pPowerupObject->m_eType;
            fn_801474EC(eventData);
        }
        break;
    }

    case 0x10:
    {
        numContacts = eType;
        cPlayer* owner = ((PhysicsAIBall*)obj)->m_pAIBall->m_pOwner;
        if (owner != 0)
        {
            if (owner->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)owner;
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
                || m_pPowerupObject->m_eType == POWER_UP_SPINY_SHELL)
            {
                return NO_CONTACT;
            }
        }
        else
        {
            KoopaShellObject* koopaShell
                = *(KoopaShellObject**)((u8*)lbl_806E1608 + 0x2C);
            if (koopaShell != 0 && koopaShell->mVisible)
            {
                return NO_CONTACT;
            }
            void* egg = *(void**)((u8*)lbl_806E1608 + 0x28);
            if (egg != 0 && *(bool*)((u8*)egg + 0x30))
            {
                return NO_CONTACT;
            }

            if (m_pPowerupObject->meSize != POWERUPSIZE_SMALL)
            {
                numContacts = ONE_WAY_CONTACT_OTHER;
            }
        }

        cBall* ball = ((PhysicsAIBall*)obj)->m_pAIBall;
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
            fn_801481BC((SkinAnimatedNPC*)((PhysicsNPC*)obj)->mpAINPC);
        }
        break;
    }

    case 0x1D:
        return ONE_WAY_CONTACT_THIS;

    case 0x1C:
    {
        int value = ((PhysicsPatch*)obj)->m_Type;
        UnidentifiedPhysicsPatchInfo_80510BF0* patchInfo
            = fn_80174ED4(&value);
        if (patchInfo->mUnidentified18 != 0.0f)
        {
            if (mUnidentified048 != 2
                || patchInfo->mUnidentified18 > mUnidentified04C)
            {
                mUnidentified04C = patchInfo->mUnidentified18;
            }
            mUnidentified048 = 2;
        }

        if (patchInfo->mUnidentified00 == 8
            || patchInfo->mUnidentified00 == 9)
        {
            m_pPowerupObject->m_bShouldDestroy = true;
        }
        return ONE_WAY_CONTACT_THIS;
    }

    default:
    {
        if (obj->GetObjectType() == 0x12 && !mUnidentified046)
        {
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
                            lbl_80571460.Allocate(eventData);
                            GetPosition(&eventData->position);
                            eventData->fVecZComponent = v3IncidentVel.z;
                            eventData->eType = m_pPowerupObject->m_eType;
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
            float fNetWidth = cNet::m_fNetWidth;
            float fNetLimitY = 0.5f * fNetWidth - fPowerupRadius;
            if (fabsf(v3PowerupPosition.y) > fNetLimitY
                && fabsf(v3PowerupPosition.x)
                       > fabsf(cField::GetGoalLineX(1U)))
            {
                m_pPowerupObject->m_bShouldDestroy = true;
                return NO_CONTACT;
            }

            if (NetMesh::s_bAnimatedNetMeshEnabled)
            {
                return NO_CONTACT;
            }

            float fMaxX;
            if (v3PowerupPosition.x > 0.0f)
            {
                NetMesh* pMesh = NetMesh::spPositiveXNetMesh;
                if (v3PowerupPosition.x > 0.0f)
                {
                    fMaxX = pMesh->mfMaxX;
                }
                else
                {
                    fMaxX = pMesh->mfMinX;
                }
            }
            else
            {
                NetMesh* pMesh = NetMesh::spNegativeXNetMesh;
                if (v3PowerupPosition.x > 0.0f)
                {
                    fMaxX = pMesh->mfMaxX;
                }
                else
                {
                    fMaxX = pMesh->mfMinX;
                }
            }

            if (fabsf(v3PowerupPosition.x)
                > fabsf(fMaxX) - 2.0f * fPowerupRadius)
            {
                m_pPowerupObject->m_bShouldDestroy = true;
                return NO_CONTACT;
            }

            float fNetLimit = cNet::m_fNetHeight - fPowerupRadius;
            if (fabsf(v3PowerupPosition.z) > fNetLimit)
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

        float fNetWidth = cNet::m_fNetWidth;
        float fNetLimitY = 0.5f * fNetWidth - fPowerupRadius;
        if (fabsf(v3PowerupPosition.y) < fNetLimitY
            && fabsf(v3PowerupPosition.x)
                   > cField::GetGoalLineX(1U) - fPowerupRadius
            && fabsf(v3PowerupPosition.z)
                   < cNet::m_fNetHeight - fPowerupRadius)
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

            if (!bWasRicochet
                && GameInfoManager::Instance()->GetStadium() == 0x0B)
            {
                nlVector3 contactPos;
                nlVec3Set(contactPos, info->geom.pos[0], info->geom.pos[1], info->geom.pos[2]);
                float height
                    = contactPos.z - m_pPowerupObject->GetRadius();
                bool aboveGroundAndSmall
                    = height > 0.36f
                   && m_pPowerupObject->meSize == POWERUPSIZE_SMALL;
                float sidelineDistance
                    = fabsf(contactPos.y) - m_pPowerupObject->GetRadius();
                bool beyondSideline
                    = sidelineDistance > cField::GetSidelineY(1U);
                bool insideGoalLine
                    = fabsf(contactPos.x) < cField::GetGoalLineX(1U) - 0.5f;
                if (insideGoalLine && aboveGroundAndSmall)
                {
                    if (beyondSideline
                        && m_pPowerupObject->mtNoHitTimer.m_uPackedTime == 0)
                    {
                        if (height <= 0.72f)
                        {
                            AddForceAtCentreOfMass(v3Unidentified);
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
                lbl_805714B0.Allocate(eventData);
                eventData->eSize = m_pPowerupObject->meSize;
                eventData->eType = m_pPowerupObject->m_eType;
                nlVec3Set(eventData->position, info->geom.pos[0], info->geom.pos[1], info->geom.pos[2]);
                nlVec3Set(eventData->normal, info->geom.normal[0], info->geom.normal[1], info->geom.normal[2]);
                fn_80147634(eventData);

                if ((m_pPowerupObject->m_eType == POWER_UP_GREEN_SHELL
                        || m_pPowerupObject->m_eType == POWER_UP_RED_SHELL)
                    && m_pPowerupObject->meSize == POWERUPSIZE_SMALL)
                {
                    m_pPowerupObject->m_bShouldDestroy = true;
                }
            }

            if (obj->GetObjectType() != 0x10
                || (obj->GetObjectType() == 0x10
                    && m_pPowerupObject->m_pTarget != 0))
            {
                EffectsGroup* pGroup = fn_802E7CDC(
                    EmissionManager::Instance(), "shell_ricochet");
                EmissionController* pControl = fn_802E7FE4(
                    EmissionManager::Instance(), pGroup, 3, true, false);

                nlVector3 v3Position;
                nlVector3 v3Velocity;
                GetPosition(&v3Position);
                GetLinearVelocity(&v3Velocity);

                pControl->SetPosition(v3Position);
                pControl->SetDirection(v3Direction);
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
        contact->surface.bounce = fn_800A92C8(g_pGame->mUnidentified10D8,
            lbl_8056CF08.m_pGameTweaks->fShellBounceGround);
        contact->surface.mu = 0.005f;
    }
    else
    {
        contact->surface.bounce
            = lbl_8056CF08.m_pGameTweaks->fShellBounce;
        contact->surface.mu = 0.005f;
    }

    contact->surface.bounce_vel = 0.0f;
    return true;
}

extern "C" UnidentifiedPhysicsPatchInfo_80510BF0* fn_80174ED4(
    const int* type)
{
    if (*type > -1 && *type < 13)
    {
        return &lbl_80510BF0[*type];
    }
    return 0;
}
