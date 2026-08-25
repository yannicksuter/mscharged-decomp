#include "Game/AI/Powerups.h"

#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Physics/PhysicsSphere.h"
#include "NL/globalpad.h"
#include "NL/nlFunction.h"
#include "NL/nlPrint.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"

extern "C" DrawableObject* fn_8027725C(unsigned long);
extern "C" void fn_802772A4(DrawableObject*);
extern "C" bool fn_8003877C(cFielder*);
extern "C" bool fn_8003886C(cFielder*);
extern "C" bool fn_800EBBFC(int, unsigned long, const void*, void*);
extern "C" PlayerTweaks* fn_8003E6E4(cFielder*);
extern "C" float fn_8002CFF0(PlayerTweaks*);
extern "C" void fn_80146964(void*);
extern "C" void fn_8014777C(void*);
extern "C" void fn_801478C4(void*);
extern "C" void fn_80147A0C(void*);
extern "C" void fn_80147B54(void*);
extern "C" EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);
extern "C" EmissionController* fn_802E7FE4(EmissionManager*, EffectsGroup*, int, bool, bool);
extern "C" void fn_8009F1B8(EmissionController&);

struct CollisionBallShellData
{
    cBall* pBall;
    PowerupBase* pPowerup;
    nlVector3 v3CollisionVelocity;
};

struct CollisionPlayerBananaData
{
    cFielder* pPlayer;
    cFielder* pThrower;
    int nThrowerPadID;
    nlVector3 v3CollisionLocation;
};

struct CollisionPlayerShellData
{
    cFielder* pPlayer;
    cFielder* pThrower;
    u8 nThrowerPadID;
    bool bIsExploder;
    int eSize;
    nlVector3 v3CollisionLocation;
    nlVector3 v3CollisionVelocity;
};

struct CollisionPlayerFreezeData
{
    cFielder* pPlayer;
    cFielder* pThrower;
    int nThrowerPadID;
    int eSize;
};

struct PowerupHitPlayerEventData
{
    ePowerUpType Type;
    cPlayer* Thrower;
    cPlayer* Target;
};

extern SlotPool<CollisionBallShellData> lbl_80571618;
extern SlotPool<CollisionPlayerShellData> lbl_80571668;
extern SlotPool<CollisionPlayerFreezeData> lbl_80571690;
extern SlotPool<CollisionPlayerBananaData> lbl_805716B8;
extern SlotPool<PowerupHitPlayerEventData> lbl_805719D8;

static int gBobombAnticipationVoiceID;
static float lbl_806DBDE0 = 0.5f;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

static PowerupSounds powerupSounds[NUM_POWER_UPS] = {
    { 0x4E4E16EA, 0xA82B862F, 0x00000000, 0x00000000, 0xA807A02B, 0x00000000, 0x5FCB9348, 0x00000000 },
    { 0x4E4E16EA, 0xA82B862F, 0x27BD7734, 0x00000000, 0xA807A02B, 0x00000000, 0x5FCB9348, 0x00000000 },
    { 0x4E4E16EA, 0xA82B862F, 0x00000000, 0x00000000, 0xA807A02B, 0x00000000, 0x5FCB9348, 0x00000000 },
    { 0x4E4E16EA, 0xA82B862F, 0x00000000, 0x00000000, 0xA807A02B, 0x00000000, 0x5FCB9348, 0x1DFB5C33 },
    { 0x4E4E16EA, 0x8D7345C6, 0x00000000, 0x3F34E69A, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0x4E4E16EA, 0x48F70F34, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x5FCB9348, 0x00000000 },
    { 0x4E4E16EA, 0x96033ECB, 0x36088C3A, 0x302EE87F, 0x00000000, 0xDB5B98D6, 0x00000000, 0x00000000 },
    { 0x4E4E16EA, 0x56C25EB1, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xBCFAF477 },
    { 0x4E4E16EA, 0x00000000, 0x70886861, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
};

namespace
{
struct Pair
{
    /* 0x0 */ unsigned long hashId;
    /* 0x4 */ const PowerupBase* powerup;
}; // total size: 0x8

struct PowerupRegistry
{
    PowerupRegistry()
    {
        for (int i = 0; i < NUM_POWERUP_REGISTRY_ENTRIES; i++)
        {
            registry[i].hashId = 0;
        }
    }

    enum
    {
        NUM_POWERUP_REGISTRY_ENTRIES = 25
    };

    /* 0x0 */ Pair registry[NUM_POWERUP_REGISTRY_ENTRIES];
}; // total size: 0xC8

PowerupModelPool powerupModelPool;
PowerupRegistry powerupRegistry;
}

static u8 gbAlwaysSurround = true;

namespace
{
const unsigned long uFREEZE_SHELL_MASTER_OBJECT = nlStringLowerHash("gameplay/blueshell");
const unsigned long uSPINY_SHELL_MASTER_OBJECT = nlStringLowerHash("gameplay/spikeshell");
const unsigned long uGREEN_SHELL_MASTER_OBJECT = nlStringLowerHash("gameplay/greenshell");
const unsigned long uRED_SHELL_MASTER_OBJECT = nlStringLowerHash("gameplay/redshell");
const unsigned long uBANANA_MASTER_OBJECT = nlStringLowerHash("gameplay/banana");
const unsigned long uBOBOMB_MASTER_OBJECT = nlStringLowerHash("gameplay/bobomb");
const char* uBANANA_STREAK_TEXTURE;
}

/**
 * Offset/Address/Size: 0x2214 | 0x8009B874 | size: 0x8
 */
float PowerupBase::GetRadius() const
{
    return ((PhysicsSphere*)m_pPhysicsObject)->GetRadius();
}

/**
 * Offset/Address/Size: 0x2DE0 | 0x8009C440 | size: 0x680
 */
void PowerupBase::CollisionCallback(PhysicsObject* pObjA,
    PhysicsObject* pObjB, const nlVector3& v3Pos, void* pParam)
{
    PowerupBase* pObj = (PowerupBase*)pParam;
    cCharacter* pCharacter = 0;
    cPlayer* pPlayerTarget = 0;

    if (pObj->m_unk44.m_uPackedTime != 0)
    {
        return;
    }

    int type = pObjB->GetObjectType();
    switch (type)
    {
    case 0x04:
        pCharacter = ((PhysicsCharacter*)pObjB->m_parentObject)->m_pAICharacter;
        break;
    case 0x10:
    {
        cBall* pBall = ((PhysicsBall*)pObjB)->m_pBall;
        if (pBall->m_bBallPathChangeCount != 0)
        {
            if (pBall->m_pOwner == 0)
            {
                CollisionBallShellData* pData = 0;
                lbl_80571618.Allocate(pData);
                pData->pPowerup = pObj;
                pData->pBall = pBall;
                pData->v3CollisionVelocity = pObj->m_v3Velocity;
                fn_80146964(pData);
            }
            else if (pBall->GetOwnerFielder() != 0)
            {
                pCharacter = pBall->GetOwnerFielder();
            }
        }
        break;
    }
    case 0x14:
    {
        if (((PhysicsShell*)pObjB)->m_pPowerupObject->m_unk44.m_uPackedTime != 0)
        {
            break;
        }

        ePowerupSize otherSize = ((PhysicsShell*)pObjB)->m_pPowerupObject->meSize;
        if (pObj->meSize > otherSize)
        {
            ((PhysicsShell*)pObjB)->m_pPowerupObject->m_bShouldDestroy = true;
        }
        else if (pObj->meSize == otherSize)
        {
            if (pObj->m_eType == POWER_UP_BANANA)
            {
                pObj->m_bShouldDestroy = true;
            }
            else if (pObj->m_eType == POWER_UP_SPINY_SHELL
                     && ((PhysicsShell*)pObjB)->m_pPowerupObject->m_eType == POWER_UP_SPINY_SHELL)
            {
            }
            else if (pObj->m_eType == POWER_UP_RED_SHELL
                     && ((PhysicsShell*)pObjB)->m_pPowerupObject->m_eType == POWER_UP_RED_SHELL)
            {
            }
            else if (pObj->m_eType == POWER_UP_SPINY_SHELL)
            {
                ((PhysicsShell*)pObjB)->m_pPowerupObject->m_bShouldDestroy = true;
            }
            else if (((PhysicsShell*)pObjB)->m_pPowerupObject->m_eType == POWER_UP_SPINY_SHELL)
            {
                pObj->m_bShouldDestroy = true;
            }
            else
            {
                ((PhysicsShell*)pObjB)->m_pPowerupObject->m_bShouldDestroy = true;
                pObj->m_bShouldDestroy = true;
            }
        }
        else
        {
            pObj->m_bShouldDestroy = true;
        }
        break;
    }
    case 0x15:
    {
        PowerupBase* pOther = ((PhysicsBanana*)pObjB)->m_pPowerupObject;
        if (pOther->m_unk44.m_uPackedTime != 0)
        {
            break;
        }

        ePowerupSize otherSize = pOther->meSize;
        if (pObj->meSize > otherSize)
        {
            pOther->m_bShouldDestroy = true;
        }
        else if (pObj->meSize == otherSize)
        {
            pOther->m_bShouldDestroy = true;
            if (pObj->m_eType != POWER_UP_SPINY_SHELL)
            {
                pObj->m_bShouldDestroy = true;
            }
        }
        else
        {
            pObj->m_bShouldDestroy = true;
        }
        break;
    }
    default:
        if (pObj->m_eType == POWER_UP_BOBOMB
            && pObjB->GetObjectType() == 0x12
            && !((Bobomb*)pObj)->mbIsMine)
        {
            ((Bobomb*)pObj)->mbIsMine = true;
            ((Bobomb*)pObj)->m_unkAC = lbl_806DBDE0;
            pObj->m_v3Velocity = v3Zero;
            pObj->m_pPhysicsObject->SetLinearVelocity(v3Zero);

            EffectsGroup* pEffectsGroup;
            if (pObj->meSize == POWERUPSIZE_SMALL)
            {
                pEffectsGroup = fn_802E7CDC(
                    EmissionManager::Instance(), "bobomb_ground");
            }
            else
            {
                pEffectsGroup = fn_802E7CDC(
                    EmissionManager::Instance(), "bobomb_ground_large");
            }

            EmissionController* pController = fn_802E7FE4(
                EmissionManager::Instance(), pEffectsGroup, 3, true, false);
            pController->SetPosition(pObj->m_v3Position);
            pController->m_uUserData = (u32)pObj;
            Function1<void, EmissionController&> callback(fn_8009F1B8);
            pController->SetUpdateCallback(callback);
        }
        break;
    }

    if (pCharacter != 0)
    {
        if (pCharacter->m_eClassType == GOALIE)
        {
            pObj->m_bShouldDestroy = true;
        }
        else
        {
            pPlayerTarget = (cPlayer*)pCharacter;
            if (((cFielder*)pCharacter)->mbWasHitByPowerupThisFrame)
            {
                return;
            }
            ((cFielder*)pCharacter)->mbWasHitByPowerupThisFrame = true;

            unsigned long soundID = powerupSounds[pObj->m_eType].sndHit;
            if (soundID != 0)
            {
                fn_800EBBFC(0x10, soundID, 0, 0);
            }

            {
                if (pObj->m_eType == POWER_UP_BANANA)
                {
                    CollisionPlayerBananaData* pData = 0;
                    lbl_805716B8.Allocate(pData);
                    pData->pPlayer = (cFielder*)pCharacter;
                    pData->pThrower = pObj->m_pThrower;
                    pData->nThrowerPadID = pObj->m_nThrowerPadID;
                    pData->v3CollisionLocation = v3Pos;
                    fn_801478C4(pData);
                    pObj->m_bShouldDestroy = true;
                }
                else if (pObj->m_eType != POWER_UP_BOBOMB)
                {
                    bool bUnknown = fn_8003877C((cFielder*)pCharacter);
                    if (pObj->m_eType != POWER_UP_FREEZE_SHELL)
                    {
                        if (!bUnknown || pObj->m_eType == POWER_UP_SPINY_SHELL)
                        {
                            CollisionPlayerShellData* pData = 0;
                            lbl_80571668.Allocate(pData);
                            pData->pPlayer = (cFielder*)pCharacter;
                            pData->eSize = (int)pObj->meSize;
                            pData->pThrower = pObj->m_pThrower;
                            pData->nThrowerPadID = (u8)pObj->m_nThrowerPadID;
                            if (pObj->mbExploder)
                            {
                                pData->bIsExploder = true;
                            }
                            else
                            {
                                pData->bIsExploder = false;
                            }
                            pData->v3CollisionLocation = v3Pos;
                            pData->v3CollisionVelocity = pObj->m_v3Velocity;
                            fn_80147A0C(pData);
                        }
                    }
                    else
                    {
                        CollisionPlayerFreezeData* pData = 0;
                        lbl_80571690.Allocate(pData);
                        pData->pPlayer = (cFielder*)pCharacter;
                        pData->eSize = (int)pObj->meSize;
                        pData->pThrower = pObj->m_pThrower;
                        pData->nThrowerPadID = pObj->m_nThrowerPadID;
                        fn_80147B54(pData);
                    }

                    if (!bUnknown)
                    {
                        if (pObj->meSize != POWERUPSIZE_LARGE)
                        {
                            pObj->m_bShouldDestroy = true;
                        }
                        else
                        {
                            pObj->m_pTarget = 0;
                        }
                    }
                }
                else if (pObj->m_eType == POWER_UP_BOBOMB)
                {
                    pObj->m_bShouldDestroy = true;
                }
            }

            bool bInvincible = !fn_8003886C((cFielder*)pCharacter)
                            && (((cFielder*)pCharacter)->muInvincibleStatus & 8) != 0;
            if (bInvincible)
            {
                pObj->m_bShouldDestroy = true;
            }
        }
    }

    if (pPlayerTarget != 0)
    {
        PowerupHitPlayerEventData* pData = 0;
        lbl_805719D8.Allocate(pData);
        pData->Type = pObj->m_eType;
        pData->Thrower = (cPlayer*)pObj->m_pThrower;
        pData->Target = pPlayerTarget;
        fn_8014777C(pData);
    }
}

/**
 * Offset/Address/Size: 0x352C | 0x8009CB8C | size: 0x330
 */
void PowerupBase::ThrowAt(cFielder* pThrower, Bowser* pBowser)
{
    unsigned long soundID = powerupSounds[m_eType].sndActivate;
    if (soundID != 0)
    {
        if (this != 0)
        {
            fn_800EBBFC(0x10, soundID, "Powerup", this);
        }
        else
        {
            fn_800EBBFC(0x10, soundID, 0, 0);
        }
    }

    int nNumSolutions;
    float pSolutions[2];
    nlVector3 v3TargetPos;
    nlVector3 v3TargetVel;

    v3TargetPos = pThrower->m_v3Position;
    v3TargetVel = pThrower->m_v3Velocity;

    if (m_pTarget != 0)
    {
        v3TargetPos = m_pTarget->m_v3Position;
        v3TargetVel = m_pTarget->m_v3Velocity;
    }

    float speed = fn_8002CFF0(fn_8003E6E4(pThrower));

    if (gbAlwaysSurround || pThrower->GetGlobalPad() == 0)
    {
        nlVector3 v3Direction;
        v3Direction.Sub2D(v3TargetPos, m_v3Position);
        v3Direction.z = v3TargetPos.z - m_v3Position.z;
        float invDist = nlRecipSqrt(v3Direction.GetLengthSq3D(), true);
        nlVec3Scale(v3Direction, invDist);

        CalcInterceptXY(m_v3Position, speed, 0.0f, v3TargetPos, v3TargetVel, nNumSolutions, pSolutions);

        if (nNumSolutions != 0)
        {
            float t;
            if (nNumSolutions == 2)
            {
                t = (pSolutions[0] < pSolutions[1]) ? pSolutions[0] : pSolutions[1];
            }
            else
            {
                t = pSolutions[0];
            }

            nlVector3 v3BobombVelocity;
            nlVector2 v2TargetPos;
            v2TargetPos.x = v3TargetVel.x * t + v3TargetPos.x;
            v2TargetPos.y = v3TargetVel.y * t + v3TargetPos.y;

            v3BobombVelocity.x = (v2TargetPos.x - m_v3Position.x) / t;
            v3BobombVelocity.y = (v2TargetPos.y - m_v3Position.y) / t;
            v3BobombVelocity.z = 0.0f;

            m_v3Velocity = v3BobombVelocity;
            m_pPhysicsObject->SetLinearVelocity(v3BobombVelocity);
        }
        else
        {
            nlVector3 v3BobombVelocity;
            nlVec3Scale(v3BobombVelocity, v3Direction, speed);
            v3BobombVelocity.z = 0.0f;

            m_v3Velocity = v3BobombVelocity;
            m_pPhysicsObject->SetLinearVelocity(v3BobombVelocity);
        }
    }
    else
    {
        unsigned short aDirection = pThrower->m_aActualFacingDirection;
        if (pThrower->m_pController != 0
            && pThrower->m_pController->GetMovementStickMagnitude() > 0.01f)
        {
            aDirection = pThrower->m_pController->GetMovementStickDirection();
        }

        nlVector3 v3BobombVelocity;
        nlPolarToCartesian(v3BobombVelocity.x, v3BobombVelocity.y, aDirection, 1.0f);
        v3BobombVelocity.z = 0.0f;
        nlVec3Scale(v3BobombVelocity, v3BobombVelocity, speed);

        m_v3Velocity = v3BobombVelocity;
        m_pPhysicsObject->SetLinearVelocity(v3BobombVelocity);
    }
}

/**
 * Offset/Address/Size: 0x401C | 0x8009D67C | size: 0xD0
 */
void PowerupBase::PreThrow(cFielder* pFielder)
{
    nlVector3 pos;

    if (pFielder->m_nPowerupAnimID == 0x59 || pFielder->m_nPowerupAnimID == 0x5B)
    {
        pos = pFielder->GetJointPosition(pFielder->m_nLeftHandJointIndex);
    }
    else
    {
        pos = pFielder->GetJointPosition(pFielder->m_nRightHandJointIndex);
    }

    m_v3Position = pos;
    m_pPhysicsObject->SetPosition(m_v3Position, PhysicsObject::WORLD_COORDINATES);
    m_v3Velocity = v3Zero;
    m_pPhysicsObject->SetLinearVelocity(v3Zero);
}

static inline PowerupBase* FindPowerUpImpl(unsigned long hashOfDrawable)
{
    const Pair* entry = powerupRegistry.registry;
    for (int i = 0; i < 25; ++i)
    {
        if (hashOfDrawable == entry->hashId)
        {
            return const_cast<PowerupBase*>(powerupRegistry.registry[i].powerup);
        }
        ++entry;
    }
    return 0;
}

static inline void RegisterPowerup(unsigned long hashID, PowerupBase* powerup)
{
    int j = 0;
    for (; j < 25; j++)
    {
        if (powerupRegistry.registry[j].hashId == 0)
        {
            powerupRegistry.registry[j].hashId = hashID;
            powerupRegistry.registry[j].powerup = powerup;
            return;
        }
    }

    extern void nlBreak();
    nlBreak();
}

static inline DrawableObject* AcquirePowerupModel(int type)
{
    for (int i = 0; i < 25; i++)
    {
        if (powerupModelPool.mFree[type][i])
        {
            powerupModelPool.mFree[type][i] = false;
            return powerupModelPool.mObjs[type][i];
        }
    }

    return 0;
}

/**
 * Offset/Address/Size: 0x3A44 | 0x8009DADC | size: 0x158
 */
void PowerupBase::Init(cFielder* pFielder, Bowser*)
{
    m_pDrawableObj = AcquirePowerupModel(m_eType);

    {
        DrawableObject* pD = m_pDrawableObj;
        pD->m_uObjectFlags |= 2;
        unsigned long hashID = m_pDrawableObj->GetHashID();
        RegisterPowerup(hashID, this);
    }

    PreThrow(pFielder);

    m_pThrower = pFielder;

    if (pFielder != 0)
    {
        s32 padID;
        bool bHasPad = pFielder->GetGlobalPad() != 0;
        padID = bHasPad ? pFielder->GetGlobalPad()->fn_80332748() : -1;
        m_nThrowerPadID = padID;
    }
}

/**
 * Offset/Address/Size: 0x4194 | 0x8009DC34 | size: 0x1C
 */
unsigned long PowerupBase::GetSoundType(ePowerUpType type, PowerupSound powerupSnd)
{
    return (&powerupSounds[type].sndAcquire)[powerupSnd];
}

/**
 * Offset/Address/Size: 0x15FC | 0x8009AC5C | size: 0x4C
 */
PowerupBase* FindPowerUp(unsigned long hashOfDrawable)
{
    return FindPowerUpImpl(hashOfDrawable);
}

inline void PowerupModelPool::Initialize(int type, unsigned long objHashName)
{
    DrawableObject* obj;
    int i;
    char name[32];

    obj = fn_8027725C(objHashName);
    i = 0;

    obj->m_uObjectFlags &= ~1;

    for (; i < 25; i++)
    {
        nlSNPrintf(name, sizeof(name), "powerup_generated_%d", mNum);
        mObjs[type][i] = obj->Clone(nlStringLowerHash(name));
        mObjs[type][i]->m_uObjectFlags &= ~1;
        fn_802772A4(mObjs[type][i]);
        mFree[type][i] = true;
        mNum++;
    }
}

/**
 * Offset/Address/Size: 0x1648 | 0x8009ACA8 | size: 0x404
 */
void InitializePowerups()
{
    powerupModelPool.mNum = 0;
    powerupModelPool.Initialize(POWER_UP_FREEZE_SHELL, uFREEZE_SHELL_MASTER_OBJECT);
    powerupModelPool.Initialize(POWER_UP_SPINY_SHELL, uSPINY_SHELL_MASTER_OBJECT);
    powerupModelPool.Initialize(POWER_UP_GREEN_SHELL, uGREEN_SHELL_MASTER_OBJECT);
    powerupModelPool.Initialize(POWER_UP_RED_SHELL, uRED_SHELL_MASTER_OBJECT);
    powerupModelPool.Initialize(POWER_UP_BANANA, uBANANA_MASTER_OBJECT);
    powerupModelPool.Initialize(POWER_UP_BOBOMB, uBOBOMB_MASTER_OBJECT);
}
