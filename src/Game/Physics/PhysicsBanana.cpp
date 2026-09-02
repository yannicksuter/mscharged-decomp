#include "Game/AI/Powerups.h"
#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/Field.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Net.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsNPC.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "NL/nlSlotPool.h"
#include "unclassified/tu_801A5F10.h"
#include "math.h"

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
extern "C" void fn_801473A4(CollisionPowerupGroundData*);
extern "C" void fn_80147634(CollisionPowerupWallData*);
extern "C" void fn_801481BC(SkinAnimatedNPC*);
extern "C" int* fn_80174ED4(int*);

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

PhysicsBanana::PhysicsBanana(float radius)
    : PhysicsSphere(g_CollisionSpace, g_PhysicsWorld, radius)
{
    m_pTriggerCallbackFunc = 0;
    m_pCallbackParam = 0;
    m_pPowerupObject = 0;
    m_bIsSupportedByGround = false;
    mUnidentified045[0] = 0;

    SetCollide(0x1F062);
    SetCategory(0x2000);
    m_gravity = -16.0f;
}

void PhysicsBanana::PreUpdate()
{
    PhysicsObject::PreUpdate();

    if (m_pPowerupObject->m_eType == POWER_UP_BANANA)
    {
        if (!mUnidentified045[0])
        {
            nlVector3 linVel = GetLinearVelocity();
            float len = nlSqrt((linVel.x * linVel.x)
                                   + (linVel.y * linVel.y)
                                   + (linVel.z * linVel.z),
                true);

            if (m_bIsSupportedByGround)
            {
                if (nlVec3LengthSquared(linVel) > 0.1f)
                {
                    float dumping
                        = -lbl_8056CF08.m_pGameTweaks->fBananaResistance / len;
                    nlVec3Scale(linVel, linVel, dumping);
                    AddForceAtCentreOfMass(linVel);
                }
                else
                {
                    SetLinearVelocity(v3Zero);
                }
            }
        }

        if (g_pBall->m_pPhysicsBall->mbUseTiltForce)
        {
            AddForceAtCentreOfMass(g_pBall->m_pPhysicsBall->mv3TiltForce);
        }
        if (g_pBall->m_pPhysicsBall->mbUseWindForce)
        {
            AddForceAtCentreOfMass(g_pBall->m_pPhysicsBall->mv3WindForce);
        }
    }
}

void PhysicsBanana::PreCollide()
{
    m_bIsSupportedByGround = false;
}

ContactType PhysicsBanana::Contact(
    PhysicsObject* other, dContact* contact, int numContacts)
{
    nlVector3 bananaPos;
    GetPosition(&bananaPos);

    if (m_pPowerupObject->m_unk44.m_uPackedTime != 0)
    {
        return ONE_WAY_CONTACT_OTHER;
    }

    if (other->GetObjectType() == 0x12 && !mUnidentified045[0])
    {
        for (int i = 0; i < numContacts; i++)
        {
            if (contact[i].geom.pos[2] <= bananaPos.z
                && contact[i].geom.normal[2] > 0.9f)
            {
                if (!m_bIsSupportedByGround)
                {
                    m_bIsSupportedByGround = true;
                }

                nlVector3 linVel;
                GetLinearVelocity(&linVel);

                if (linVel.z < -1.0f)
                {
                    CollisionPowerupGroundData* eventData = 0;
                    lbl_80571460.Allocate(eventData);
                    GetPosition(&eventData->position);
                    eventData->fVecZComponent = linVel.z;
                    eventData->eType = m_pPowerupObject->m_eType;
                    fn_801473A4(eventData);
                }

                if (m_pTriggerCallbackFunc != 0)
                {
                    nlVector3 contactPos;
                    nlVec3Set(contactPos, contact->geom.pos[0], contact->geom.pos[1], contact->geom.pos[2]);
                    m_pTriggerCallbackFunc(
                        this, other, contactPos, m_pCallbackParam);
                }
                break;
            }
        }
    }

    bool hasWallContact = false;
    switch (other->GetObjectType())
    {
    case 4:
    {
        cCharacter* character
            = ((PhysicsCharacter*)other->m_parentObject)->m_pAICharacter;
        if (character->m_eClassType == FIELDER)
        {
            if (!((cFielder*)character)->mbTangible)
            {
                return NO_CONTACT;
            }

            PowerupBase* powerup = m_pPowerupObject;
            float upperHeight = powerup->m_v3Position.z + powerup->GetRadius();
            powerup = m_pPowerupObject;
            float lowerHeight = powerup->m_v3Position.z - powerup->GetRadius();
            if ((m_pPowerupObject->mtNoHitTimer.m_uPackedTime != 0
                    && m_pPowerupObject->m_pThrower == (cFielder*)character)
                || fn_800976F8((cFielder*)character, upperHeight)
                || fn_800977A4((cFielder*)character, lowerHeight))
            {
                return NO_CONTACT;
            }
            if (m_pTriggerCallbackFunc != 0)
            {
                nlVector3 contactPos;
                nlVec3Set(contactPos, contact->geom.pos[0], contact->geom.pos[1], contact->geom.pos[2]);
                m_pTriggerCallbackFunc(
                    this, other, contactPos, m_pCallbackParam);
            }
            return NO_CONTACT;
        }

        if (m_pTriggerCallbackFunc != 0)
        {
            nlVector3 contactPos;
            nlVec3Set(contactPos, contact->geom.pos[0], contact->geom.pos[1], contact->geom.pos[2]);
            m_pTriggerCallbackFunc(this, other, contactPos, m_pCallbackParam);
        }
        break;
    }
    case 0x10:
    {
        cBall* ball = ((PhysicsAIBall*)other)->m_pAIBall;
        if (ball->m_pOwner != 0 && ball->m_pOwner->m_eClassType == FIELDER)
        {
            cFielder* fielder = (cFielder*)ball->m_pOwner;
            if (!fielder->mbTangible)
            {
                return NO_CONTACT;
            }
            if (fielder->m_eActionState == 0x0C)
            {
                return NO_CONTACT;
            }
            if (m_pPowerupObject->mtNoHitTimer.m_uPackedTime != 0
                && m_pPowerupObject->m_pThrower == fielder)
            {
                return NO_CONTACT;
            }
        }

        if (ball->m_tLightningTimer.m_uPackedTime != 0 && fn_800167A8(ball))
        {
            return NO_CONTACT;
        }

        KoopaShellObject* koopaShell =
            *(KoopaShellObject**)((u8*)lbl_806E1608 + 0x2C);
        if (koopaShell != 0 && koopaShell->mVisible)
        {
            return NO_CONTACT;
        }
        void* egg = *(void**)((u8*)lbl_806E1608 + 0x28);
        if (egg != 0 && *(bool*)((u8*)egg + 0x30))
        {
            return NO_CONTACT;
        }

        if (m_pTriggerCallbackFunc != 0)
        {
            nlVector3 contactPos;
            nlVec3Set(contactPos, contact->geom.pos[0], contact->geom.pos[1], contact->geom.pos[2]);
            m_pTriggerCallbackFunc(this, other, contactPos, m_pCallbackParam);
        }

        FakeBallWorld::InvalidateBallCache();
        ++ball->m_bBallDeflectCount;
        ++ball->m_bBallPathChangeCount;
        break;
    }
    case 0x14:
    {
        if (m_pPowerupObject->mtNoHitTimer.m_uPackedTime != 0)
        {
            PowerupBase* otherPowerup
                = ((PhysicsShell*)other)->m_pPowerupObject;
            if (otherPowerup->m_pThrower == m_pPowerupObject->m_pThrower)
            {
                return NO_CONTACT;
            }
        }
        if (m_pTriggerCallbackFunc != 0)
        {
            nlVector3 contactPos;
            nlVec3Set(contactPos, contact->geom.pos[0], contact->geom.pos[1], contact->geom.pos[2]);
            m_pTriggerCallbackFunc(this, other, contactPos, m_pCallbackParam);
        }
        return NO_CONTACT;
    }
    case 0x15:
    {
        if (m_pPowerupObject->mtNoHitTimer.m_uPackedTime != 0)
        {
            PowerupBase* otherPowerup
                = ((PhysicsBanana*)other)->m_pPowerupObject;
            if (otherPowerup->m_pThrower == m_pPowerupObject->m_pThrower)
            {
                return NO_CONTACT;
            }
        }
        if (m_pTriggerCallbackFunc != 0)
        {
            nlVector3 contactPos;
            nlVec3Set(contactPos, contact->geom.pos[0], contact->geom.pos[1], contact->geom.pos[2]);
            m_pTriggerCallbackFunc(this, other, contactPos, m_pCallbackParam);
        }
        return NO_CONTACT;
    }
    case 0x18:
    {
        bool isChainChomp = ((SkinAnimatedNPC*)((PhysicsNPC*)other)->mpAINPC)->GetSkinAnimatedNPC_Type() == SkinAnimatedNPC_CHAIN_CHOMP;
        if (isChainChomp)
        {
            fn_801481BC((SkinAnimatedNPC*)((PhysicsNPC*)other)->mpAINPC);
        }
        break;
    }
    case 0x1D:
        return ONE_WAY_CONTACT_THIS;
    case 0x1C:
    {
        int value = *(int*)((u8*)other + 0x48);
        int result = *fn_80174ED4(&value);
        if (result == 8 || result == 9)
        {
            m_pPowerupObject->m_bShouldDestroy = true;
        }
        return ONE_WAY_CONTACT_THIS;
    }
    default:
    {
        if (other->GetObjectType() == 0x17
            || other->GetObjectType() == 5)
        {
            for (int i = 0; i < numContacts; i++)
            {
                if (contact[i].geom.normal[2] < 0.08f)
                {
                    hasWallContact = true;
                }
            }

            nlVector3 contactPos;
            nlVec3Set(contactPos, contact->geom.pos[0], contact->geom.pos[1], contact->geom.pos[2]);

            nlVector3 powerupPos;
            GetPosition(&powerupPos);
            float radius = GetRadius();
            if (fabsf(powerupPos.x)
                    > cField::GetGoalLineX(1U) - 2.0f * radius
                && fabsf(powerupPos.y)
                       < 0.5f * cNet::m_fNetWidth - radius
                && fabsf(powerupPos.z) < cNet::m_fNetHeight - radius)
            {
                return NO_CONTACT;
            }

            if (hasWallContact
                && GameInfoManager::Instance()->GetStadium() == 0x0B)
            {
                float sidelineDistance
                    = fabsf(contactPos.y) - m_pPowerupObject->GetRadius();
                bool beyondSideline
                    = sidelineDistance > cField::GetSidelineY(1U);
                bool insideGoalLine = fabsf(contactPos.x)
                                    < cField::GetGoalLineX(1U) - 0.5f;
                if (insideGoalLine)
                {
                    if (beyondSideline)
                    {
                        mUnidentified045[0] = true;
                    }
                    return NO_CONTACT;
                }
            }
        }
        break;
    }
    }

    nlVector3& linVel = GetLinearVelocity();
    float velSq = nlVec3LengthSquared(linVel);
    if (hasWallContact && velSq > 1.0f && !mUnidentified045[0])
    {
        if (other->GetObjectType() == 0x17
            || other->GetObjectType() == 5)
        {
            CollisionPowerupWallData* eventData = 0;
            lbl_805714B0.Allocate(eventData);
            eventData->eSize = m_pPowerupObject->meSize;
            eventData->eType = m_pPowerupObject->m_eType;
            nlVec3Set(eventData->position, contact->geom.pos[0], contact->geom.pos[1], contact->geom.pos[2]);
            nlVec3Set(eventData->normal, contact->geom.normal[0], contact->geom.normal[1], contact->geom.normal[2]);
            fn_80147634(eventData);
        }
    }

    if (mUnidentified045[0])
    {
        return NO_CONTACT;
    }
    return TWO_WAY_CONTACT;
}

bool PhysicsBanana::SetContactInfo(dContact* contact, PhysicsObject* other, bool first)
{
    if (first)
    {
        SetDefaultContactInfo(contact);
    }

    if (other->GetObjectType() == 0x12)
    {
        contact->surface.bounce = lbl_8056CF08.m_pGameTweaks->fShellBounceGround;
    }
    else
    {
        contact->surface.bounce = 0.001f;
    }
    contact->surface.bounce_vel = 0.0f;
    contact->surface.mu = 100.0f;
    return true;
}

PhysicsBanana::~PhysicsBanana()
{
}
