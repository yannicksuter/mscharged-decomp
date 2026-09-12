#include "Game/Render/ChainChomp.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Audio/GameStreams.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/UnidentifiedRegistryPools.h"
#include "Game/Ball.h"
#include "Game/BasicStadium.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Camera/animcam.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/Render/RLView.h"
#include "NL/gl/glDraw3.h"
#include "Game/Effects/EffectsGroup.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Event.h"
#include "Game/EventRegistry.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Inventory.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Player.h"
#include "Game/PoseAccumulator.h"
#include "Game/Render/PeachPhoto.h"
#include "Game/RenderSnapshot.h"
#include "Game/ReplayManager.h"
#include "Game/RumbleActions.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/Team.h"
#include "Game/Terrain.h"
#include "NL/gl/glState.h"
#include "NL/nlColour.h"
#include "NL/nlString.h"

static cAnimCamera sNisCam;
float gfChainChompWindupSpeedScale;
static unsigned char gbChainChompProjectedShadow;
RLView* gpChainChompShadowView;
const nlVector3 gv3HomePosition = { 0.0f, 0.0f, -10.0f };

float gfChainChompGroundHeight = -0.45f;
float gfChainChompAttackSpeedScale = 1.75f;
float gfChainChompAttackRange = 1.75f;
float gfChainChompAcceleration = 100.0f;
float gfChainChompDeceleration = 100.0f;
float gfChainChompTargetDistanceWeight = 0.75f;

void CollisionFireballChain(void*);
void CollisionChainCrowd(void*);
void CollisionChainPowerup(CollisionChainPowerupData*);

void UpdateChainEmitter(EmissionController& controller);

ChainChomp::ChainChomp(cSHierarchy& pHierarchy, int nModelID,
    PhysicsNPC& rPhysObj, cInventory<cSAnim>* pInventorySAnim, void* resource)
    : SkinAnimatedMovableNPC(pHierarchy, nModelID, rPhysObj, resource)
    , mpTarget(0)
    , meChainChompState(CHAIN_STATE_HIDDEN)
{
    mpInEffectSFX = 0;
    mfChaseSpeed = 0.0f;
    gpChainChompShadowView = GetUnshadowedView();
    mtStateTimer.SetSeconds(0.0f);
    mpIdleAnim = pInventorySAnim->Find((unsigned int)nlStringHash("chain_idle"));
    mpRecoverAnim = pInventorySAnim->Find((unsigned int)nlStringHash("landing"));
    mpDropAnim = pInventorySAnim->Find((unsigned int)nlStringHash("chomp"));
    mpPhysObj->mpAINPC = this;

    if (mpInEffectSFX != 0)
    {
        mpInEffectSFX = 0;
    }
    EmissionManager::Instance()->Destroy((unsigned long)this,
        EmissionManager::Instance()->GetEffectsGroup("chainchomp_trail"));
    EmissionManager::Instance()->Destroy((unsigned long)this,
        EmissionManager::Instance()->GetEffectsGroup("freeze"));
    meChainChompState = CHAIN_STATE_HIDDEN;
    mfDesiredSpeed = 0.0f;
    SetAnimState(*mpIdleAnim, 0.0f, PM_CYCLIC);
    SetPosition(gv3HomePosition);
    maFacingDirection = 0;
    mv3Velocity = v3Zero;
    mpPhysObj->DisableCollisions();
    mpThrower = 0;
    mnThrowerPadID = -1;
    mbIsVisible = false;

    {
        Function<void*> callback(CollisionFireballChain);
        EventRegistryValue* foundEvent;
        unsigned int hash;
        hash = HashEventName("CollisionFireballChain", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? foundEvent->event : 0;
        ((UnidentifiedTypedEvent<void>*)event)->Add(callback, 0, -1);
    }
    {
        Function<void*> callback(CollisionChainCrowd);
        EventRegistryValue* foundEvent;
        unsigned int hash;
        hash = HashEventName("CollisionChainCrowd", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? foundEvent->event : 0;
        ((UnidentifiedTypedEvent<void>*)event)->Add(callback, 0, -1);
    }
    {
        Function<CollisionChainPowerupData*> callback(CollisionChainPowerup);
        EventRegistryValue* foundEvent;
        unsigned int hash;
        hash = HashEventName("CollisionChainPowerup", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? foundEvent->event : 0;
        ((UnidentifiedTypedEvent<CollisionChainPowerupData>*)event)->Add(callback, 0, -1);
    }
    mpAvoidable = 0;
}

ChainChomp::~ChainChomp()
{
    if (mpAvoidable != 0)
    {
        delete mpAvoidable;
        mpAvoidable = 0;
    }
}

static inline u16 UnidentifiedDeltaToAngle(float y, float x)
{
    return (u16)(s32)(10430.378f * nlATan2f(y, x));
}

static inline void EmitTrail(ChainChomp* pChomp)
{
    EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("chainchomp_trail");
    SetEffectsGroupFountainLife(pGroup, 12.0f);

    EmissionController* pControl = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
    pControl->SetPosition(pChomp->mv3Position);
    pControl->m_uUserData = (u32)pChomp;
    pControl->SetUpdateCallback(Function<EmissionController&>(UpdateChainEmitter));
}

static inline void StartChasing(ChainChomp* pChomp)
{
    pChomp->mfChaseSpeed = gGameTweaks.m_pGameTweaks->fChainChompSpeed.GetValue();
    if (pChomp->mfChaseSpeed > 25.0f)
    {
        pChomp->mfChaseSpeed = 25.0f;
    }

    float fSpeedScale = InterpolateRangeClamped(0.8f, 1.2f, 0.0f, 1.0f, g_pGame->mpTerrain->GetSpeedFactor());
    pChomp->meChainChompState = CHAIN_STATE_CHASE;
    pChomp->mfDesiredSpeed = pChomp->mfChaseSpeed * fSpeedScale;
    pChomp->SetAnimState(*pChomp->mpIdleAnim, 0.0f, PM_CYCLIC);
}

void ChainChomp::Update(float fDeltaT)
{
    EffectsGroup* pGroup;
    u16 aDesiredFacing;
    nlVector3 v3FallVelocity;
    nlVector3 v3RecoverPosition;
    nlVector3 v3UpdatedPosition;

    switch (meChainChompState)
    {
    case CHAIN_STATE_FROZEN:
        if (mtStateTimer.m_uPackedTime == 0)
        {
            break;
        }
        if (mtStateTimer.Countdown(fDeltaT, 0.0f))
        {
            pGroup = EmissionManager::Instance()->GetEffectsGroup("freeze");
            EmissionManager::Instance()->Destroy((unsigned long)this, pGroup);
            mpPhysObj->EnableCollisions();

            EmitTrail(this);
            StartChasing(this);
            mtStateTimer.SetSeconds(0.5f * gGameTweaks.m_pGameTweaks->fChainChompActiveTime.GetValue());
        }
        break;

    case CHAIN_STATE_FALL:
        if (fabsf(mv3Position.y) > 100.0f || fabsf(mv3Position.x) > 100.0f)
        {
            nlVec3Set(v3FallVelocity, 0.0f, 0.0f, 0.0f);
            mv3Velocity = v3FallVelocity;
        }
        if (mtStateTimer.m_uPackedTime == 0)
        {
            break;
        }
        if (mtStateTimer.Countdown(fDeltaT, 0.0f))
        {
            if (mpInEffectSFX != 0)
            {
                mpInEffectSFX = 0;
            }
            pGroup = EmissionManager::Instance()->GetEffectsGroup("chainchomp_trail");
            EmissionManager::Instance()->Kill((unsigned long)this, pGroup);

            pGroup = EmissionManager::Instance()->GetEffectsGroup("freeze");
            EmissionManager::Instance()->Kill((unsigned long)this, pGroup);

            meChainChompState = CHAIN_STATE_HIDDEN;
            mfDesiredSpeed = 0.0f;
            SetAnimState(*mpIdleAnim, 0.0f, PM_CYCLIC);
            SetPosition(gv3HomePosition);
            maFacingDirection = 0;
            mv3Velocity = v3Zero;
            mpPhysObj->DisableCollisions();
            mpThrower = 0;
            mnThrowerPadID = -1;
            mbIsVisible = false;
        }
        break;

    case CHAIN_STATE_RECOVER:
        if (mpAnimController->m_fTime < 0.75f)
        {
            maDesiredFacingDirection = 0xC000;
            maFacingDirection = 0xC000;

            v3RecoverPosition = v3Zero;
            if (mpTarget != 0)
            {
                v3RecoverPosition = mpTarget->mUnidentified024.m_v3Position;
            }
            v3RecoverPosition.z = gfChainChompGroundHeight;
            cField::FixOutOfBoundsPosition(v3RecoverPosition,
                gGameTweaks.m_pGameTweaks->fChainChompRadius.GetValue(),
                true);
            SetPosition(v3RecoverPosition);
        }
        if (mpAnimController->TestTrigger(0.6875f))
        {
            mpPhysObj->EnableCollisions();
        }
        else if (mpAnimController->TestTrigger(0.75f))
        {
            if (mpAvoidable == 0)
            {
                mpAvoidable = new (8, false) AvoidablePowerup(this);
            }
            pGroup = EmissionManager::Instance()->GetEffectsGroup("chainchomp_land");
            EmissionController* pControl = EmissionManager::Instance()->Create(pGroup, 3, true, 0);
            pControl->SetPosition(mv3Position);

            PowerupBase::PlayPowerupSound(POWER_UP_CHAIN_CHOMP,
                PowerupBase::PWRUP_SOUND_BOUNCE_GROUND,
                mpPhysObj,
                0.0f,
                0);
            FireCameraRumbleFilter(0.0f, 0.2f, 5000.0f, 10.0f);
            for (int i = 0; i < 2; i++)
            {
                cTeam* pTeam = g_pTeams[i];
                if (pTeam != 0)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        cFielder* pFielder = pTeam->GetFielder(j);
                        if (pFielder != 0)
                        {
                            PlayRumbleAction(3, pFielder->GetGlobalPad());
                        }
                    }
                }
            }

            EmitTrail(this);
        }
        else
        {
            bool bRecoverDone = mpAnimController->m_ePlayMode == PM_HOLD
                && mpAnimController->m_fTime == 1.0f;
            if (bRecoverDone)
            {
                PowerupBase::PlayPowerupSound(POWER_UP_CHAIN_CHOMP,
                    PowerupBase::PWRUP_SOUND_IN_EFFECT,
                    mv3Position,
                    0.0f,
                    0);
                StartChasing(this);
                mtStateTimer.SetSeconds(gGameTweaks.m_pGameTweaks->fChainChompActiveTime.GetValue());
            }
        }
        break;

    case CHAIN_STATE_CHASE:
        if (mtStateTimer.m_uPackedTime == 0)
        {
            break;
        }
        if (mpTarget == 0
            || (mpTarget != 0 && mpTarget->IsFallenDown() && mpTarget->m_tFireTimer.m_uPackedTime == 0)
            || fabsf(mpTarget->mUnidentified024.m_v3Position.x) > cField::GetGoalLineX(1U))
        {
            mpTarget = FindTarget(mpThrower->m_pTeam->GetOtherTeam());
        }
        {
            float fDeltaX = mpTarget->mUnidentified024.m_v3Position.x - mv3Position.x;
            float fDeltaY = mpTarget->mUnidentified024.m_v3Position.y - mv3Position.y;
            aDesiredFacing = (u16)(s32)(10430.378f * nlATan2f(fDeltaY, fDeltaX));
            maDesiredFacingDirection = aDesiredFacing;
            Move(fDeltaT);
            if (mpTarget != 0)
            {
                s16 angleDiff = (s16)(aDesiredFacing - maFacingDirection);
                u16 absDelta = (u16)(angleDiff < 0 ? -angleDiff : angleDiff);
                if (absDelta < 0x1800)
                {
                    const nlVector3& v3ChompPosition = mv3Position;
                    float fRadius = gfChainChompAttackRange + gGameTweaks.m_pGameTweaks->fChainChompRadius.GetValue();
                    if (nlVec3DistanceSquared2D(v3ChompPosition, mpTarget->mUnidentified024.m_v3Position) < fRadius * fRadius)
                    {
                        mfDesiredSpeed = gfChainChompWindupSpeedScale * gGameTweaks.m_pGameTweaks->fChainChompSpeed.GetValue();
                        PowerupBase::PlayPowerupSound(POWER_UP_CHAIN_CHOMP,
                            PowerupBase::PWRUP_SOUND_IN_EFFECT,
                            v3ChompPosition,
                            0.0f,
                            0);
                        meChainChompState = CHAIN_STATE_CHOMP;
                        SetAnimState(*mpDropAnim, 0.0f, PM_HOLD);
                    }
                }
            }
        }
        if (mtStateTimer.Countdown(fDeltaT, 0.0f))
        {
            Leave();
        }
        break;

    case CHAIN_STATE_CHOMP:
        if (mpTarget != 0 && mpAnimController->m_fTime < 3.0f / mpAnimController->m_pSAnim->m_nNumKeys)
        {
            float fDeltaX = mpTarget->mUnidentified024.m_v3Position.x - mv3Position.x;
            float fDeltaY = mpTarget->mUnidentified024.m_v3Position.y - mv3Position.y;
            maDesiredFacingDirection = (u16)(s32)(10430.378f * nlATan2f(fDeltaY, fDeltaX));
        }
        Move(fDeltaT);
        if (mpAnimController->TestFrameTrigger(3.0f))
        {
            mfDesiredSpeed = gfChainChompAttackSpeedScale * gGameTweaks.m_pGameTweaks->fChainChompSpeed.GetValue();
        }
        if (mpAnimController->TestFrameTrigger(6.0f))
        {
            PowerupBase::PlayPowerupSound(POWER_UP_CHAIN_CHOMP,
                PowerupBase::PWRUP_SOUND_HIT,
                mpPhysObj,
                0.0f,
                0);
        }
        {
            bool bBiteDone = mpAnimController->m_ePlayMode == PM_HOLD
                && mpAnimController->m_fTime == 1.0f;
            if (bBiteDone)
            {
                StartChasing(this);
            }
        }
        break;

    case CHAIN_STATE_LEAVE:
        Move(fDeltaT);
        if (fabsf(mv3Position.x) > 22.5f)
        {
            if (mpAvoidable != 0)
            {
                delete mpAvoidable;
                mpAvoidable = 0;
            }
            ShotAtGoalData* pData = g_ShotAtGoalDataPool.Allocate();
            pData->pShooter = mpThrower;
            g_pGame->QueueChainNisEnd(pData);
            if (mpInEffectSFX != 0)
            {
                mpInEffectSFX = 0;
            }
            pGroup = EmissionManager::Instance()->GetEffectsGroup("chainchomp_trail");
            EmissionManager::Instance()->Kill((unsigned long)this, pGroup);

            pGroup = EmissionManager::Instance()->GetEffectsGroup("freeze");
            EmissionManager::Instance()->Kill((unsigned long)this, pGroup);

            meChainChompState = CHAIN_STATE_HIDDEN;
            mfDesiredSpeed = 0.0f;
            SetAnimState(*mpIdleAnim, 0.0f, PM_CYCLIC);
            SetPosition(gv3HomePosition);
            maFacingDirection = 0;
            mv3Velocity = v3Zero;
            mpPhysObj->DisableCollisions();
            mpThrower = 0;
            mnThrowerPadID = -1;
            mbIsVisible = false;
        }
        break;

    case CHAIN_STATE_HIDDEN:
    default:
        break;
    }

    v3UpdatedPosition.x = mv3Position.x + fDeltaT * mv3Velocity.x;
    v3UpdatedPosition.y = mv3Position.y + fDeltaT * mv3Velocity.y;
    v3UpdatedPosition.z = gfChainChompGroundHeight;
    if (GameInfoManager::Instance()->GetStadium() != 0xB && meChainChompState != CHAIN_STATE_LEAVE)
    {
        cField::FixOutOfBoundsPosition(v3UpdatedPosition,
            gGameTweaks.m_pGameTweaks->fChainChompRadius.GetValue(),
            true);
    }
    SetPosition(v3UpdatedPosition);
    if (meChainChompState != CHAIN_STATE_FROZEN)
    {
        SkinAnimatedNPC::Update(fDeltaT);
    }
}

void ChainChomp::CollisionCallback(
    PhysicsObject* pPhysObj, PhysicsObject* pObjA, const nlVector3& v3Pos)
{
    ChainChomp* pChainChomp = (ChainChomp*)((PhysicsNPC*)pPhysObj)->mpAINPC;
    if (pChainChomp->meChainChompState == CHAIN_STATE_FROZEN)
    {
        return;
    }

    cFielder* pFielder = 0;
    switch (pObjA->GetObjectType())
    {
    case 0x04:
        pFielder = (cFielder*)((PhysicsCharacter*)pObjA->m_parentObject)->m_pAICharacter;
        break;
    case 0x10:
    {
        cBall* pBall = ((PhysicsAIBall*)pObjA)->m_pAIBall;
        cFielder* pOwner = (cFielder*)pBall->m_pOwner;
        if (pOwner != 0 && pOwner->m_eClassType == FIELDER
            && pOwner->IsOnSameTeam(pChainChomp->mpTarget)
            && pOwner->mbTangible)
        {
            pFielder = (cFielder*)pBall->m_pOwner;
        }
        if (pFielder == 0 && pBall->meBallState != 10)
        {
            CollisionBallChainData* pData = g_CollisionBallChainDataPool.Allocate();
            pData->pChain = pChainChomp;
            pData->pBall = pBall;
            QueueCollisionBallChain(pData);
        }
        break;
    }
    case 0x14:
    {
        CollisionChainPowerupData* pData = g_CollisionChainPowerupDataPool.Allocate();
        pData->pChain = pChainChomp;
        pData->pPowerup = ((PhysicsShell*)pObjA)->m_pPowerupObject;
        QueueCollisionChainPowerup(pData);
        break;
    }
    case 0x15:
    {
        CollisionChainPowerupData* pData = g_CollisionChainPowerupDataPool.Allocate();
        pData->pChain = pChainChomp;
        pData->pPowerup = ((PhysicsBanana*)pObjA)->m_pPowerupObject;
        QueueCollisionChainPowerup(pData);
        break;
    }
    }

    if (pFielder != 0 && pFielder->m_eClassType == FIELDER
        && (pChainChomp->meChainChompState == CHAIN_STATE_RECOVER
            || (pFielder->mbTangible
                && !pFielder->IsCharacterInAir(pChainChomp->mv3Position.z
                                               + pChainChomp->mpPhysObj->GetRadius()))))
    {
        CollisionChainPlayerData* pData = g_CollisionChainPlayerDataPool.Allocate();
        pData->pFielder = pFielder;
        pData->pChain = pChainChomp;
        g_pGame->mUnidentified49C.mEvent33.Queue(pData,
            Function<CollisionChainPlayerData*>(FreeCollisionChainPlayerData));
    }
}

void UpdateChainEmitter(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }
    if (ReplayManager::Instance()->mRender != 0)
    {
        {
            ReplayManager* replayManager = ReplayManager::Instance();
            controller.SetPosition(replayManager->mRender->mChainChomp.position);
        }
        {
            ReplayManager* replayManager = ReplayManager::Instance();
            controller.SetVelocity(replayManager->mRender->mChainChomp.velocity);
        }
        nlVector3 direction = { 0.0f, 0.0f, 1.0f };
        controller.SetDirection(direction);
    }
}

cFielder* ChainChomp::FindTarget(cTeam* pTeam)
{
    float fTempScore;
    float fBestScore = 99999.9f;
    cFielder* pBestCandidate = 0;
    cFielder* pFielder;
    cFielder* pFielder2;
    int i;
    cFielder* pCandidate;

    if (g_pBall->GetOwnerFielder() != 0)
    {
        pFielder = g_pBall->GetOwnerFielder();
        if (pFielder->IsOnSameTeam(pTeam->GetStriker())
            && !pFielder->IsFrozen() && !pFielder->IsShattered()
            && (!pFielder->IsFallenDown() || pFielder->m_tFireTimer.m_uPackedTime != 0)
            && pFielder != mpTarget)
        {
            pBestCandidate = g_pBall->GetOwnerFielder();
        }
    }
    else if (g_pBall->GetPassTargetFielder() != 0)
    {
        pFielder2 = g_pBall->GetPassTargetFielder();
        if (pFielder2->IsOnSameTeam(pTeam->GetStriker())
            && !pFielder2->IsFrozen() && !pFielder2->IsShattered()
            && (!pFielder2->IsFallenDown() || pFielder2->m_tFireTimer.m_uPackedTime != 0)
            && pFielder2 != mpTarget)
        {
            pBestCandidate = pFielder2;
        }
    }
    if (pBestCandidate == 0)
    {
        for (i = 0; i < 4; i++)
        {
            fTempScore = 999999.9f;
            pCandidate = pTeam->GetFielder(i);
            if ((!pCandidate->IsFallenDown() || pCandidate->m_tFireTimer.m_uPackedTime != 0)
                && !pCandidate->IsFrozen() && !pCandidate->IsShattered()
                && pCandidate != mpTarget
                && fabsf(pCandidate->mUnidentified024.m_v3Position.x) < cField::GetGoalLineX(1U))
            {
                nlVector2 v2Delta = meChainChompState == CHAIN_STATE_RECOVER
                                      ? *(nlVector2*)&mpThrower->mUnidentified024.m_v3Position
                                      : *(nlVector2*)&mv3Position;
                nlVec2Sub(v2Delta, *(nlVector2*)&pCandidate->mUnidentified024.m_v3Position, v2Delta);
                float fDist = nlSqrt(nlGetLengthSquared2D(v2Delta.x, v2Delta.y), true);
                s16 angleDiff = (s16)(maFacingDirection
                                      - UnidentifiedDeltaToAngle(v2Delta.y, v2Delta.x));
                u16 absDelta = (u16)(angleDiff < 0 ? -angleDiff : angleDiff);
                float fAngleScore = InterpolateRangeClamped(0.0f, 1.0f, 8192.0f, 32768.0f, (float)absDelta);
                float fDistScore = InterpolateRangeClamped(0.0f, 1.0f, 0.0f, 20.0f, fDist);
                fTempScore = fDistScore * gfChainChompTargetDistanceWeight + (1.0f - gfChainChompTargetDistanceWeight) * fAngleScore;
            }
            if (fTempScore < fBestScore)
            {
                pBestCandidate = pCandidate;
                fBestScore = fTempScore;
            }
        }
    }
    if (pBestCandidate == 0)
    {
        mtStateTimer.SetSeconds(0.0f);
        if (meChainChompState != CHAIN_STATE_RECOVER)
        {
            Leave();
            pBestCandidate = pTeam->GetStriker();
        }
    }
    return pBestCandidate;
}

void ChainChomp::Fall()
{
    nlVector3 v3FallVelocity;
    v3FallVelocity.x = mv3Velocity.x;
    v3FallVelocity.y = mv3Velocity.y;
    v3FallVelocity.z = -12.5f;
    mpTarget = 0;
    meChainChompState = CHAIN_STATE_FALL;
    mv3Velocity = v3FallVelocity;
    mtStateTimer.SetSeconds(1.5f);
    mpPhysObj->DisableCollisions();
    EmissionManager::Instance()->Kill((unsigned long)this,
        EmissionManager::Instance()->GetEffectsGroup("chainchomp_trail"));
}

void ChainChomp::Spawn(cFielder* pThrower)
{
    meChainChompState = CHAIN_STATE_RECOVER;
    mpThrower = pThrower;
    bool bHasGlobalPad = pThrower->GetGlobalPad() != 0;
    mnThrowerPadID = bHasGlobalPad ? pThrower->GetGlobalPad()->GetPadID() : -1;
    mbIsVisible = true;
    SetAnimState(*mpRecoverAnim, 0.0f, PM_HOLD);
    mpTarget = 0;
    mpTarget = FindTarget(pThrower->m_pTeam->GetOtherTeam());
    mpPhysObj->DisableCollisions();
    mpPhysObj->mUnidentified040 = false;
    nlVector3 v3StartPosition = v3Zero;
    if (mpTarget != 0)
    {
        v3StartPosition = mpTarget->mUnidentified024.m_v3Position;
    }
    v3StartPosition.z = 0.0f;
    SetPosition(v3StartPosition);
    mv3Velocity = v3Zero;
    PowerupBase::PlayPowerupSound(POWER_UP_CHAIN_CHOMP,
        PowerupBase::PWRUP_SOUND_ACTIVATE,
        mpPhysObj,
        0.0f,
        0);
    PowerupBase::PlayPowerupSound(POWER_UP_CHAIN_CHOMP,
        PowerupBase::PWRUP_SOUND_IN_EFFECT,
        mv3Position,
        0.0f,
        0);
    if (GetStadiumUnknown0x10(GameInfoManager::Instance()->GetStadium()))
    {
        unsigned long hash = pThrower->m_pTeam->m_nSide == 0 ? 0x5AB49659 : 0x56C11782;
        PlayCrowdReaction(hash);
    }
}

void ChainChomp::Freeze(float, bool)
{
}

/**
 * Offset/Address/Size: 0x0 | 0x8019C988 | size: 0x10
 */
bool ChainChomp::IsHidden() const
{
    return meChainChompState == CHAIN_STATE_HIDDEN;
}

bool ChainChomp::IsFrozen() const
{
    return meChainChompState == CHAIN_STATE_FROZEN;
}

/**
 * Offset/Address/Size: 0x0 | 0x8019C9AC | size: 0x11C
 */
void ChainChomp::Leave()
{
    meChainChompState = CHAIN_STATE_LEAVE;

    float fSpeedScale = InterpolateRangeClamped(0.8f,
        1.2f,
        0.0f,
        1.0f,
        g_pGame->mpTerrain->GetSpeedFactor());
    mfDesiredSpeed = mfChaseSpeed * fSpeedScale;
    SetAnimState(*mpIdleAnim, 0.0f, PM_CYCLIC);

    nlVector3 v3TargetPosition;
    if (0.5f * g_pBall->m_v3Velocity.x
            + g_pBall->m_v3Position.x
        < 0.0f)
    {
        v3TargetPosition.x = 40.0f;
    }
    else
    {
        v3TargetPosition.x = -40.0f;
    }

    if (mv3Position.y < 0.0f)
    {
        v3TargetPosition.y = -8.0f;
    }
    else
    {
        v3TargetPosition.y = 8.0f;
    }
    v3TargetPosition.z = 0.0f;

    float fTargetX = v3TargetPosition.x - mv3Position.x;
    float fTargetY = v3TargetPosition.y - mv3Position.y;
    float fAngle = nlATan2f(fTargetY, fTargetX);
    maDesiredFacingDirection = (u16)(s32)(10430.378f * fAngle);

    PowerupBase::PlayPowerupSound(POWER_UP_CHAIN_CHOMP,
        PowerupBase::PWRUP_SOUND_IN_EFFECT,
        mv3Position,
        0.0f,
        0);
}

/**
 * Offset/Address/Size: 0x0 | 0x8019CAC8 | size: 0xE0
 */
void ChainChomp::Move(float fDeltaT)
{
    nlPolar aChainSpeed;
    nlVector3 v3NewVelocity;
    float fDesiredSpeed;

    nlCartesianToPolar(aChainSpeed, mv3Velocity);

    if (aChainSpeed.r < 0.05f)
    {
        aChainSpeed.a = maFacingDirection;
    }

    fDesiredSpeed = mfDesiredSpeed;
    float fNewSpeed = SeekSpeed(
        aChainSpeed.r, fDesiredSpeed, gfChainChompAcceleration, gfChainChompDeceleration, fDeltaT);

    maFacingDirection = SeekDirection(
        aChainSpeed.a, maDesiredFacingDirection, 60000.0f, 3000.0f, fDeltaT);

    nlPolarToCartesian(
        v3NewVelocity.x, v3NewVelocity.y, maFacingDirection, fNewSpeed);
    v3NewVelocity.z = mv3Velocity.z;
    mv3Velocity = v3NewVelocity;
}

void CollisionFireballChain(void*)
{
}

void CollisionChainCrowd(void*)
{
}

void CollisionChainPowerup(CollisionChainPowerupData* collision)
{
    PowerupBase* pPowerup = collision->pPowerup;
    if (pPowerup->m_eType != POWER_UP_SPINY_SHELL)
    {
        pPowerup->m_bShouldDestroy = true;
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x8019CBCC | size: 0x2AC
 */
void ChainChomp::DrawShadow(
    const cPoseAccumulator& pa, const nlMatrix4& worldMatrix)
{
    switch (meChainChompState)
    {
    default:
        if (gbChainChompProjectedShadow)
        {
            SkinAnimatedNPC::DrawShadow(mpLastModel, mWorldMatrix);
            return;
        }

        nlMatrix4& nodeMatrix = pa.GetNodeMatrix(4);
        nlVector3 v3ModelPosition;
        v3ModelPosition = nodeMatrix.GetTranslation();
        float x = v3ModelPosition.x;
        float y = v3ModelPosition.y;
        float half_dim;
        float frac;

        frac = 0.0f;
        if (meChainChompState == CHAIN_STATE_RECOVER)
        {
            frac = 1.0f - mpAnimController->get_fTime();
        }
        if (frac < 0.0f)
        {
            frac = 0.0f;
        }
        if (frac > 1.0f)
        {
            frac = 1.0f;
        }

        float radius = gGameTweaks.m_pGameTweaks->fChainChompRadius.GetValue();
        radius = 1.5f * radius;
        float fAlpha = 150.0f * (1.0f - frac) + 60.0f * frac;
        half_dim = 6.0f * frac + (1.0f - frac) * radius;

        int alpha = (int)fAlpha;
        if (alpha < 0)
        {
            alpha = 0;
        }
        if (alpha > 255)
        {
            alpha = 255;
        }

        BasicStadium* stadium = BasicStadium::GetCurrentStadium();
        float groundHeight = 0.0f;
        if (stadium != 0)
        {
            groundHeight = stadium->m_shadowHeight;
        }

        nlVector3 position;
        position.x = x;
        position.y = y;
        position.z = 0.015625f + groundHeight;

        float minX;
        float minY = position.y - half_dim;
        minX = position.x - half_dim;
        float maxY = position.y + half_dim;
        float maxX = position.x + half_dim;

        nlColour c;
        c.c[0] = 255;
        c.c[1] = 255;
        c.c[2] = 255;
        c.c[3] = (unsigned char)alpha;

        glQuad3 quad;
        quad.m_pos[0].x = minX;
        quad.m_pos[0].y = minY;
        quad.m_pos[0].z = position.z;
        quad.m_pos[1].x = minX;
        quad.m_pos[1].y = maxY;
        quad.m_pos[1].z = position.z;
        quad.m_pos[2].x = maxX;
        quad.m_pos[2].y = maxY;
        quad.m_pos[2].z = position.z;
        quad.m_pos[3].x = maxX;
        quad.m_pos[3].y = minY;
        quad.m_pos[3].z = position.z;

        quad.m_uv[0].x = 1.0f;
        quad.m_uv[0].y = 1.0f;
        quad.m_uv[1].x = 0.0f;
        quad.m_uv[1].y = 1.0f;
        quad.m_uv[2].x = 0.0f;
        quad.m_uv[2].y = 0.0f;
        quad.m_uv[3].x = 1.0f;
        quad.m_uv[3].y = 0.0f;

        *(u32*)&quad.m_colour[3] = *(u32*)&c;
        *(u32*)&quad.m_colour[2] = *(u32*)&c;
        *(u32*)&quad.m_colour[1] = *(u32*)&c;
        *(u32*)&quad.m_colour[0] = *(u32*)&c;

        glSetDefaultState(true);
        glSetRasterState(GLS_AlphaBlend, 1);
        glSetRasterState(GLS_Culling, 0);
        glSetRasterState(GLS_DepthWrite, 0);
        glSetCurrentRasterState(glHandleizeRasterState());
        glSetCurrentTexture(glGetTexture("global/ball_shadow"), GLTT_Diffuse);
        glSetTextureState(GLTS_DiffuseWrap, 3);
        glSetCurrentTextureState(glHandleizeTextureState());

        void* renderContext;
        if (gPeachPhotoState.state == 1)
        {
            renderContext = GetLayerView(eCLV_MoreCharacters);
        }
        else
        {
            renderContext = gpChainChompShadowView;
        }
        quad.Attach((eGLView)(u32)renderContext, 0);
        break;
    case CHAIN_STATE_HIDDEN:
    case CHAIN_STATE_FALL:
    case CHAIN_STATE_FROZEN:
        return;
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x8019CE78 | size: 0x120
 */
void ChainChomp::Hide()
{
    if (mpInEffectSFX != 0)
    {
        mpInEffectSFX = 0;
    }

    EffectsGroup* pEffectsGroup = EmissionManager::Instance()->GetEffectsGroup("chainchomp_trail");
    EmissionManager::Instance()->Destroy(
        reinterpret_cast<unsigned long>(this), pEffectsGroup);

    pEffectsGroup = EmissionManager::Instance()->GetEffectsGroup("freeze");
    EmissionManager::Instance()->Destroy(
        reinterpret_cast<unsigned long>(this), pEffectsGroup);

    meChainChompState = CHAIN_STATE_HIDDEN;
    mfDesiredSpeed = 0.0f;
    SetAnimState(*mpIdleAnim, 0.0f, PM_CYCLIC);
    SetPosition(gv3HomePosition);
    maFacingDirection = 0;
    mv3Velocity = v3Zero;
    mpPhysObj->DisableCollisions();

    mpThrower = 0;
    mnThrowerPadID = -1;
    mbIsVisible = false;
    mpTarget = 0;
    mtStateTimer.m_unk0 = mtStateTimer.m_uPackedTime != 0;
    mtStateTimer.m_uPackedTime = 0;
    mfChaseSpeed = 0.0f;
}

#include "NL/nlBind_impl.h"
