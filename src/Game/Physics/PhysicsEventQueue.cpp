#include "NL/nlDLListContainer.inl"
#include "Game/Task/DispatchEventsTask.h"
#include "Game/Render/BirdoEgg.h"
#include "Game/Render/BulletBill.h"
#include "Game/Audio/GameStreams.h"
#include "Game/EventDispatcher.inl"

#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/EventRegistry.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Goalie.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsBulletBill.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Physics/PhysicsSphere.h"
#include "Game/Physics/PhysicsYoshiEgg.h"
#include "Game/Team.h"
#include "NL/nlBind.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/globalpad.h"
#include "Game/Physics/PhysicsShockwave.h"
#include "Game/Physics/PhysicsWaluigiWall.h"
#include "Game/Render/HammerObject.h"
#include "Game/Render/KoopaShellObject.h"
#include "Game/Render/YoshiEggObject.h"
#include "Game/SharedStaticStorage.h"
#include "Game/Physics/PhysicsEventQueue.inl"
#include "NL/nlFunction.inl"

class PhysicsEventQueue
{
public:
    PhysicsEventQueue();
    ~PhysicsEventQueue();

    void Dispatch(bool deliver)
    {
        mDispatcher.Dispatch(deliver);
    }

public:
    EventDispatcher mDispatcher;
    QueuedEvent<NoEventData> mBallFallEvent;
    QueuedEvent<CollisionPlayerPlayerData> mCollisionPlayerPlayerEvent;
    QueuedEvent<CollisionPlayerWallData> mCollisionPlayerWallEvent;
    QueuedEvent<CollisionPlayerBallData> mCollisionPlayerBallEvent;
    QueuedEvent<BallNetmeshEventData> mCollisionBallNetmeshEvent;
    QueuedEvent<CollisionBallGroundData> mCollisionBallGroundEvent;
    QueuedEvent<CollisionBallWallData> mCollisionBallWallEvent;
    QueuedEvent<CollisionBallGoalpostData> mCollisionBallGoalpostEvent;
    QueuedEvent<CollisionBallShellData> mCollisionBallShellEvent;
    QueuedEvent<CollisionBallChainData> mCollisionBallChainEvent;
    QueuedEvent<CollisionKoopaShotBallPlayerData> mCollisionKoopaShotBallPlayerEvent;
    QueuedEvent<CollisionKoopaShellGoalieData> mCollisionKoopaShellGoalieEvent;
    QueuedEvent<CollisionKoopaShellEndData> mCollisionKoopaShellEndEvent;
    QueuedEvent<CollisionBirdoShotBallPlayerData> mCollisionBirdoShotBallPlayerEvent;
    QueuedEvent<CollisionBirdoEggGoalieData> mCollisionBirdoEggGoalieEvent;
    QueuedEvent<CollisionBirdoEggEndData> mCollisionBirdoEggEndEvent;
    QueuedEvent<CollisionHammerbroShotBallPlayerData> mCollisionHammerbroShotBallPlayerEvent;
    QueuedEvent<CollisionPowerupGroundData> mCollisionPowerupGroundEvent;
    QueuedEvent<CollisionPowerupGroundData> mCollisionPowerupGoalieEvent;
    QueuedEvent<CollisionPowerupWallData> mCollisionPowerupWallEvent;
    QueuedEvent<PowerupHitPlayerEventData> mPowerupHitEvent;
    QueuedEvent<CollisionPlayerBananaData> mCollisionPlayerBananaEvent;
    QueuedEvent<CollisionPlayerShellData> mCollisionPlayerShellEvent;
    QueuedEvent<CollisionPlayerFreezeData> mCollisionPlayerFreezeEvent;
    QueuedEvent<CollisionBulletBillData> mCollisionBulletBillPlayerEvent;
    QueuedEvent<CollisionBulletBillData> mCollisionBulletBillFreezeEvent;
    QueuedEvent<CollisionBulletBillData> mExplosionBulletBillEvent;
    QueuedEvent<CollisionPatchData> mCollisionTongueEvent;
    QueuedEvent<NoEventData> mCollisionBallTronWallEvent;
    QueuedEvent<PowerupUsedEventData> mPowerupUsedEvent;
    QueuedEvent<CollisionProjectileData> mCollisionFireballPlayerEvent;
    QueuedEvent<CollisionProjectileData> mCollisionFireballBallEvent;
    QueuedEvent<CollisionProjectileData> mCollisionFireballGroundEvent;
    QueuedEvent<PowerupBase> mCollisionFireballPowerupEvent;
    QueuedEvent<ChainChomp> mCollisionFireballChainEvent;
    QueuedEvent<ChainChomp> mCollisionChainCrowdEvent;
    QueuedEvent<CollisionChainPowerupData> mCollisionChainPowerupEvent;
    QueuedEvent<CollisionPatchData> mCollisionPatchPlayerEvent;
    QueuedEvent<CollisionPatchData> mCollisionPatchGroundEvent;
    QueuedEvent<CollisionPatchPowerupData> mCollisionPatchPowerupEvent;
    QueuedEvent<ChainChomp> mCollisionPatchChainEvent;
    QueuedEvent<CollisionPatchData> mCollisionPatchPatchEvent;
    QueuedEvent<PhysicsPatch> mCollisionPatchBallEvent;
    QueuedEvent<PhysicsPatch> mCollisionPatchWallEvent;
    QueuedEvent<CollisionProjectileData> mCollisionHammerPlayerEvent;
    QueuedEvent<CollisionProjectileData> mCollisionHammerBallEvent;
    QueuedEvent<CollisionProjectileData> mCollisionHammerGroundEvent;
    QueuedEvent<PowerupBase> mCollisionHammerPowerupEvent;
    QueuedEvent<ChainChomp> mCollisionHammerChainEvent;
    QueuedEvent<CollisionThwompPlayerData> mCollisionThwompPlayerEvent;
    QueuedEvent<ThwompObject> mCollisionThwompBallEvent;
    QueuedEvent<ChainChomp> mCollisionThwompChainEvent;
    QueuedEvent<CollisionEggData> mCollisionEggBallEvent;
    QueuedEvent<CollisionEggData> mCollisionEggPlayerEvent;
    QueuedEvent<CollisionEggData> mCollisionEggPowerupEvent;
    QueuedEvent<CollisionEggData> mCollisionEggChainEvent;
    QueuedEvent<CollisionEggData> mCollisionCrackEggEvent;
    QueuedEvent<PowerupBase> mDestroyPowerupEvent;
    QueuedEvent<HammerObject> mDestroyHammerEvent;
    QueuedEvent<cFielder> mKnockYoshiTongueEvent;
    QueuedEvent<WindDebris> mCollisionDebrisBallEvent;
    QueuedEvent<cFielder> mCollisionWaluigiWallEvent;
    QueuedEvent<CollisionShockwaveData> mCollisionShockwaveEvent;
};

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
float gfDaisyFistShotTime = 1.0f;

SlotPool<CollisionProjectileData> g_CollisionProjectileDataPool(16, 16);
SlotPool<CollisionPatchData> g_CollisionPatchDataPool(16, 16);
SlotPool<CollisionPatchPowerupData> g_CollisionPatchPowerupDataPool(16, 16);
SlotPool<CollisionEggData> g_CollisionEggDataPool(16, 16);
SlotPool<CollisionShockwaveData> gCollisionShockwaveDataPool(16, 16);

PhysicsEventQueue* gPhysicsEventQueue;

void FreePhysicsEventDataPools()
{
    g_CollisionProjectileDataPool.FreeBlocks();
    g_CollisionPatchDataPool.FreeBlocks();
    g_CollisionPatchPowerupDataPool.FreeBlocks();
    g_CollisionEggDataPool.FreeBlocks();
    gCollisionShockwaveDataPool.FreeBlocks();
}

void CreatePhysicsEventQueue()
{
    if (gPhysicsEventQueue == 0)
    {
        gPhysicsEventQueue = new (nlMalloc(sizeof(PhysicsEventQueue), 8, false))
            PhysicsEventQueue;
        InitializeShockwaves();
    }
}

void DestroyPhysicsEventQueue()
{
    if (gPhysicsEventQueue != 0)
    {
        EventDispatcher& dispatcher = gPhysicsEventQueue->mDispatcher;
        dispatcher.Clear();

        BasicSlotPool<DLListEntry<EventCallback> >* pool =
            &dispatcher.callbacks.m_Allocator;
        fn_802B467C(pool);
        SlotPoolBase::BaseFreeBlocks(
            pool, sizeof(DLListEntry<EventCallback>));

        delete gPhysicsEventQueue;
        gPhysicsEventQueue = 0;
        ShutdownShockwaves();
    }
}

void DispatchPhysicsEvents(PhysicsEventQueue* queue)
{
    queue->Dispatch(true);
}

PhysicsEventQueue::PhysicsEventQueue()
    : mDispatcher("PhysicsEventQueue")
    , mBallFallEvent(&mDispatcher, "BallFall", -1)
    , mCollisionPlayerPlayerEvent(&mDispatcher, "CollisionPlayerPlayer", -1)
    , mCollisionPlayerWallEvent(&mDispatcher, "CollisionPlayerWall", -1)
    , mCollisionPlayerBallEvent(&mDispatcher, "CollisionPlayerBall", -1)
    , mCollisionBallNetmeshEvent(&mDispatcher, "CollisionBallNetmesh", -1)
    , mCollisionBallGroundEvent(&mDispatcher, "CollisionBallGround", -1)
    , mCollisionBallWallEvent(&mDispatcher, "CollisionBallWall", -1)
    , mCollisionBallGoalpostEvent(&mDispatcher, "CollisionBallGoalpost", -1)
    , mCollisionBallShellEvent(&mDispatcher, "CollisionBallShell", -1)
    , mCollisionBallChainEvent(&mDispatcher, "CollisionBallChain", -1)
    , mCollisionKoopaShotBallPlayerEvent(&mDispatcher, "CollisionKoopaShotBallPlayer", -1)
    , mCollisionKoopaShellGoalieEvent(&mDispatcher, "CollisionKoopaShellGoalie", -1)
    , mCollisionKoopaShellEndEvent(&mDispatcher, "CollisionKoopaShellEnd", -1)
    , mCollisionBirdoShotBallPlayerEvent(&mDispatcher, "CollisionBirdoShotBallPlayer", -1)
    , mCollisionBirdoEggGoalieEvent(&mDispatcher, "CollisionBirdoEggGoalie", -1)
    , mCollisionBirdoEggEndEvent(&mDispatcher, "CollisionBirdoEggEnd", -1)
    , mCollisionHammerbroShotBallPlayerEvent(&mDispatcher, "CollisionHammerbroShotBallPlayer", -1)
    , mCollisionPowerupGroundEvent(&mDispatcher, "CollisionPowerupGround", -1)
    , mCollisionPowerupGoalieEvent(&mDispatcher, "CollisionPowerupGoalie", -1)
    , mCollisionPowerupWallEvent(&mDispatcher, "CollisionPowerupWall", -1)
    , mPowerupHitEvent(&mDispatcher, "PowerupHit", -1)
    , mCollisionPlayerBananaEvent(&mDispatcher, "CollisionPlayerBanana", -1)
    , mCollisionPlayerShellEvent(&mDispatcher, "CollisionPlayerShell", -1)
    , mCollisionPlayerFreezeEvent(&mDispatcher, "CollisionPlayerFreeze", -1)
    , mCollisionBulletBillPlayerEvent(&mDispatcher, "CollisionBulletBillPlayer", -1)
    , mCollisionBulletBillFreezeEvent(&mDispatcher, "CollisionBulletBillFreeze", -1)
    , mExplosionBulletBillEvent(&mDispatcher, "ExplosionBulletBill", -1)
    , mCollisionTongueEvent(&mDispatcher, "CollisionTongue", -1)
    , mCollisionBallTronWallEvent(&mDispatcher, "CollisionBallTronWall", -1)
    , mPowerupUsedEvent(&mDispatcher, "PowerupUsed", -1)
    , mCollisionFireballPlayerEvent(&mDispatcher, "CollisionFireballPlayer", -1)
    , mCollisionFireballBallEvent(&mDispatcher, "CollisionFireballBall", -1)
    , mCollisionFireballGroundEvent(&mDispatcher, "CollisionFireballGround", -1)
    , mCollisionFireballPowerupEvent(&mDispatcher, "CollisionFireballPowerup", -1)
    , mCollisionFireballChainEvent(&mDispatcher, "CollisionFireballChain", -1)
    , mCollisionChainCrowdEvent(&mDispatcher, "CollisionChainCrowd", -1)
    , mCollisionChainPowerupEvent(&mDispatcher, "CollisionChainPowerup", -1)
    , mCollisionPatchPlayerEvent(&mDispatcher, "CollisionPatchPlayer", -1)
    , mCollisionPatchGroundEvent(&mDispatcher, "CollisionPatchGround", -1)
    , mCollisionPatchPowerupEvent(&mDispatcher, "CollisionPatchPowerup", -1)
    , mCollisionPatchChainEvent(&mDispatcher, "CollisionPatchChain", -1)
    , mCollisionPatchPatchEvent(&mDispatcher, "CollisionPatchPatch", -1)
    , mCollisionPatchBallEvent(&mDispatcher, "CollisionPatchBall", -1)
    , mCollisionPatchWallEvent(&mDispatcher, "CollisionPatchWall", -1)
    , mCollisionHammerPlayerEvent(&mDispatcher, "CollisionHammerPlayer", -1)
    , mCollisionHammerBallEvent(&mDispatcher, "CollisionHammerBall", -1)
    , mCollisionHammerGroundEvent(&mDispatcher, "CollisionHammerGround", -1)
    , mCollisionHammerPowerupEvent(&mDispatcher, "CollisionHammerPowerup", -1)
    , mCollisionHammerChainEvent(&mDispatcher, "CollisionHammerChain", -1)
    , mCollisionThwompPlayerEvent(&mDispatcher, "CollisionThwompPlayer", -1)
    , mCollisionThwompBallEvent(&mDispatcher, "CollisionThwompBall", -1)
    , mCollisionThwompChainEvent(&mDispatcher, "CollisionThwompChain", -1)
    , mCollisionEggBallEvent(&mDispatcher, "CollisionEggBall", -1)
    , mCollisionEggPlayerEvent(&mDispatcher, "CollisionEggPlayer", -1)
    , mCollisionEggPowerupEvent(&mDispatcher, "CollisionEggPowerup", -1)
    , mCollisionEggChainEvent(&mDispatcher, "CollisionEggChain", -1)
    , mCollisionCrackEggEvent(&mDispatcher, "CollisionCrackEgg", -1)
    , mDestroyPowerupEvent(&mDispatcher, "DestroyPowerup", -1)
    , mDestroyHammerEvent(&mDispatcher, "DestroyHammer", -1)
    , mKnockYoshiTongueEvent(&mDispatcher, "KnockYoshiTongue", -1)
    , mCollisionDebrisBallEvent(&mDispatcher, "CollisionDebrisBall", -1)
    , mCollisionWaluigiWallEvent(&mDispatcher, "CollisionWaluigiWall", -1)
    , mCollisionShockwaveEvent(&mDispatcher, "CollisionShockwave", -1)
{
    RegisterPhysicsEventHandlers();
}

PhysicsEventQueue::~PhysicsEventQueue()
{
}

void HandleCollisionFireballPowerup(void*);
void HandleDestroyPowerup(void*);
void HandleCollisionWaluigiWall(void*);
void HandleDestroyHammer(void*);
void HandleCollisionHammerPowerup(void*);
void HandleCollisionPatchPowerup(void*);
void HandleCollisionCrackEgg(void*);
void HandleCollisionKoopaShellEnd(void*);
void HandleCollisionBirdoEggEnd(void*);
void HandleCollisionPatchPatch(void*);
void HandleCollisionShockwave(CollisionShockwaveData*);

void RegisterPhysicsEventHandlers()
{
    FindEvent<void>("CollisionPatchPowerup", -1)->Add(Function<void*>(HandleCollisionPatchPowerup), 0, -1);
    FindEvent<void>("CollisionHammerPowerup", -1)->Add(Function<void*>(HandleCollisionHammerPowerup), 0, -1);
    FindEvent<void>("CollisionFireballPowerup", -1)->Add(Function<void*>(HandleCollisionFireballPowerup), 0, -1);
    FindEvent<void>("CollisionCrackEgg", -1)->Add(Function<void*>(HandleCollisionCrackEgg), 0, -1);
    FindEvent<void>("CollisionShockwave", -1)->Add(Function<void*>((void (*)(void*))HandleCollisionShockwave), 0, -1);
    FindEvent<void>("CollisionKoopaShellEnd", -1)->Add(Function<void*>(HandleCollisionKoopaShellEnd), 0, -1);
    FindEvent<void>("CollisionBirdoEggEnd", -1)->Add(Function<void*>(HandleCollisionBirdoEggEnd), 0, -1);
    FindEvent<void>("CollisionPatchPatch", -1)->Add(Function<void*>(HandleCollisionPatchPatch), 0, -1);
    FindEvent<void>("DestroyPowerup", -1)->Add(Function<void*>(HandleDestroyPowerup), 0, -1);
    FindEvent<void>("DestroyHammer", -1)->Add(Function<void*>(HandleDestroyHammer), 0, -1);
    FindEvent<void>("CollisionWaluigiWall", -1)->Add(Function<void*>(HandleCollisionWaluigiWall), 0, -1);
}

extern "C" void fn_800156F8(cBall* pBall, cPlayer* pShooter);
extern "C" void fn_80015B38(cBall* pBall, bool bParam);
void HandleCollisionFireballPowerup(void* object)
{
    ((unsigned char*)object)[4] = true;
}

void HandleDestroyPowerup(void* object)
{
    unsigned char* bytes = (unsigned char*)object;
    if (bytes[4] == false)
    {
        bytes[4] = true;
    }
}

void HandleCollisionWaluigiWall(void* object)
{
    nlVector3 direction;
    nlPolarToCartesian(direction.x, direction.y, *(unsigned short*)((unsigned char*)object + 0x62), 1.0f);
    direction.z = 0.0f;
    ((cFielder*)object)->InitActionShellReact(direction, v3Zero);
}

void HandleDestroyHammer(void* object)
{
    if (&HammerObject::fn_8016A650)
    {
        ((HammerObject*)object)->Reset(false);
    }
}

void HandleCollisionHammerPowerup(void* object)
{
    unsigned char* bytes = (unsigned char*)object;
    if (*(int*)(bytes + 0xA4) != 2 || *(int*)(bytes + 0x1C) != 2)
    {
        bytes[4] = true;
    }
}

void HandleCollisionPatchPowerup(void*)
{
}

void HandleCollisionCrackEgg(void* data)
{
    unsigned char* object = *(unsigned char**)((unsigned char*)data + 4);
    if (*(int*)(object + 0xF0) == 2)
    {
        ((cFielder*)object)->EndSuperPower(0);
    }
}

void HandleCollisionKoopaShellEnd(void* data)
{
    unsigned char* bytes = (unsigned char*)data;
    if (bytes[4] != false)
    {
        fn_800156F8(
            g_pBall, *(cPlayer**)(*(unsigned char**)bytes + 0x2C));
    }
    else
    {
        fn_80015B38(g_pBall, 0);
    }
    (*(KoopaShellObject**)bytes)->Deactivate(false);
}

void HandleCollisionBirdoEggEnd(void* data)
{
    unsigned char* bytes = (unsigned char*)data;
    if (bytes[4] != false)
    {
        fn_800156F8(
            g_pBall, *(cPlayer**)(*(unsigned char**)bytes + 0x3C));
    }
    else
    {
        fn_80015B38(g_pBall, 0);
    }
    (*(BirdoEggObject**)bytes)->Hide(false);
}

void HandleCollisionShockwave(CollisionShockwaveData* data)
{
    if (g_pGame == 0)
    {
        return;
    }
    if (!g_pGame->IsGameplayOrOvertime())
    {
        switch (g_pGame->m_eGameState)
        {
        case GS_END_GAME:
            break;
        default:
            return;
        }
    }

    CollisionPowerupStatsData* pStats;
    bool bInvincible;
    cBall* pBall;
    eSpinType spinType;
    PhysicsObject* pObject = data->pObject;
    PhysicsShockwave* pShockwave = data->pShockwave;
    int shockwaveType = pShockwave->mType;

    switch (pObject->GetObjectType())
    {
    case PHYSOBJ_COLUMN:
    {
        cCharacter* pCharacter =
            ((PhysicsCharacter*)pObject->m_parentObject)->m_pAICharacter;
        if (pCharacter->m_eClassType == FIELDER)
        {
            cFielder* pFielder = (cFielder*)pCharacter;
            switch (shockwaveType)
            {
            case SHOCKWAVE_HIT:
                if (pShockwave->mOwner == pFielder)
                {
                    break;
                }
                if (!pFielder->mbTangible)
                {
                    break;
                }
                if (pFielder->m_eActionState == ACTION_HIT_REACT)
                {
                    break;
                }
                pFielder->CollideWithShockwaveCallback(pShockwave->mPosition);
                break;
            case SHOCKWAVE_FREEZE:
                pFielder->CollideWithFreezeCallback();
                break;
            case SHOCKWAVE_EXPLOSION:
            case SHOCKWAVE_BULLET_BILL:
            case SHOCKWAVE_DAISY_FIST:
                if (pShockwave->mOwner == pFielder)
                {
                    break;
                }
                if (pFielder->m_eActionState == ACTION_BOMB_REACT
                    || pFielder->m_eActionState == ACTION_HIT_REACT)
                {
                    break;
                }
                if (pFielder->CollideWithBobombCallback(pShockwave->mPosition,
                        pShockwave->GetRadius())
                    && pShockwave->mOwner != 0
                    && shockwaveType != SHOCKWAVE_BULLET_BILL)
                {
                    pStats = 0;
                    g_CollisionPowerupStatsDataPool.Allocate(pStats);
                    pStats->pThrower = (cPlayer*)pShockwave->mOwner;
                    pStats->nThrowerPadID = pShockwave->mSourceIndex;
                    pStats->pPlayer = pFielder;
                    bool bHasPad = pFielder->GetGlobalPad() != 0;
                    pStats->nPlayerPadID =
                        bHasPad ? pFielder->GetGlobalPad()->GetPadID() : -1;
                    g_pGame->mEventQueue.mEvent31.Queue(pStats,
                        Function<CollisionPowerupStatsData*>(
                            FreeCollisionPowerupStatsData));
                }
                break;
            case SHOCKWAVE_LIGHTNING:
                if (!pFielder->mbTangible)
                {
                    break;
                }
                bInvincible = false;
                if (!pFielder->IsStuck()
                    && (pFielder->muInvincibleStatus & 0x1F) == 0x1F)
                {
                    bInvincible = true;
                }
                if (bInvincible)
                {
                    break;
                }
                if (pFielder->m_eActionState == ACTION_ELECTROCUTION)
                {
                    break;
                }
                pFielder->fn_800451B0(pShockwave->mPosition);
                if (GetStadiumUnknown0x10(
                        GameInfoManager::Instance()->GetStadium()))
                {
                    unsigned long soundID = 0x70A628D8;
                    if (pFielder->m_pTeam->m_nSide == 0)
                    {
                        soundID = 0xF68B3F0F;
                    }
                    PlayCrowdReaction(soundID);
                }
                break;
            }
        }
        else if (pCharacter->m_eClassType == GOALIE)
        {
            Goalie* pGoalie = (Goalie*)pCharacter;
            switch (shockwaveType)
            {
            case SHOCKWAVE_BULLET_BILL:
                if (pGoalie->mGoalieActionState
                    != GOALIEACTION_SHOCKWAVE_REACT)
                {
                    pGoalie->InitActionShockwaveReact();
                }
                break;
            case SHOCKWAVE_LIGHTNING:
                if (pGoalie->mGoalieActionState
                    != GOALIEACTION_ELECTROCUTION)
                {
                    pGoalie->InitActionElectrocution();
                }
                break;
            }
        }
        break;
    }
    case PHYSOBJ_AI_BALL:
    {
        pBall = ((PhysicsAIBall*)pObject)->m_pAIBall;
        if (shockwaveType == SHOCKWAVE_LIGHTNING)
        {
            break;
        }
        if (shockwaveType == SHOCKWAVE_BULLET_BILL)
        {
            cCharacter* pOwner = (cCharacter*)pShockwave->mOwner;
            if (pOwner != 0 && pOwner->m_eClassType == FIELDER)
            {
                fn_800156F8(pBall, (cPlayer*)pOwner);
            }
            break;
        }

        if (pBall->GetOwnerFielder() != 0)
        {
            cFielder* pOwner = (cFielder*)pShockwave->mOwner;
            if (pBall->GetOwnerFielder() == pOwner)
            {
                break;
            }
        }
        else
        {
            if (fn_800167A8(pBall))
            {
                break;
            }
            if (pBall->GetOwnerGoalie() != 0)
            {
                cPlayer* pGoalie = pBall->GetOwnerGoalie();
                pGoalie->ReleaseBall(0);
                static_cast<Goalie*>(pGoalie)->InitActionSTSRecover();
            }
        }

        if (nlRandom(2) != 0)
        {
            spinType = SPINTYPE_FORWARD;
        }
        else
        {
            spinType = SPINTYPE_BACK;
        }
        nlVector3 v3Velocity;
        if (shockwaveType == SHOCKWAVE_DAISY_FIST)
        {
            pBall->ShootAtFast(v3Velocity,
                ((cCharacter*)pShockwave->mOwner)->m_DetChar.m_v3Position, gfDaisyFistShotTime);
            nlRandom(2);
        }
        else
        {
            nlVec3Sub(v3Velocity, pBall->m_v3Position, pShockwave->mPosition);
            v3Velocity.z = 0.0f;
            float fLengthSquared = v3Velocity.GetLengthSq3D();
            if (fLengthSquared > 0.001f)
            {
                nlVec3Scale(v3Velocity, v3Velocity,
                    nlRecipSqrt(fLengthSquared, true));
                nlVec3Scale(v3Velocity, 6.0f + nlRandomf(6.0f));
            }
            else
            {
                v3Velocity.x = nlRandomf(6.0f) - 3.0f;
                v3Velocity.y = nlRandomf(6.0f) - 3.0f;
                v3Velocity.x += v3Velocity.x > 0.0f ? 6.0f : -6.0f;
                v3Velocity.y += v3Velocity.y > 0.0f ? 6.0f : -6.0f;
            }
            v3Velocity.z = 8.0f + nlRandomf(5.0f);
        }
        pBall->ShootRelease(v3Velocity, spinType);
        fn_80015B38(pBall, false);
        break;
    }
    case PHYSOBJ_BANANA:
        if (shockwaveType == SHOCKWAVE_HIT)
        {
            ((PhysicsBanana*)pObject)->m_pPowerupObject->fn_8009CEBC(
                pShockwave->mPosition);
            break;
        }
        if (shockwaveType == SHOCKWAVE_EXPLOSION
            && ((PhysicsBanana*)pObject)->m_pPowerupObject->m_eType
                == POWER_UP_BOBOMB)
        {
            break;
        }
        ((PhysicsBanana*)pObject)->m_pPowerupObject->m_bShouldDestroy = true;
        break;
    case PHYSOBJ_SHELL:
        if (shockwaveType == SHOCKWAVE_HIT)
        {
            ((PhysicsShell*)pObject)->m_pPowerupObject->fn_8009CEBC(
                pShockwave->mPosition);
            break;
        }
        ((PhysicsShell*)pObject)->m_pPowerupObject->m_bShouldDestroy = true;
        break;
    case PHYSOBJ_HAMMER:
        ((PhysicsHammer*)pObject)->mHammer->Deactivate(true);
        break;
    case PHYSOBJ_WALUIGI_WALL:
        ((PhysicsWaluigiWall*)pObject)->ApplyDamage(0.35f);
        break;
    case PHYSOBJ_YOSHI_EGG:
        if (((PhysicsYoshiEgg*)pObject)->mYoshiEgg->mFielder
            == pShockwave->mOwner)
        {
            break;
        }
        if (shockwaveType == SHOCKWAVE_FREEZE)
        {
            ((PhysicsYoshiEgg*)pObject)->mYoshiEgg->Suspend(true,
                gGameTweaks.m_pGameTweaks->fFreezeShellFrozenTime);
        }
        if (shockwaveType != SHOCKWAVE_LIGHTNING)
        {
            (((PhysicsYoshiEgg*)pObject)->mYoshiEgg->mFielder)->EndSuperPower(0);
        }
        break;
    case PHYSOBJ_BULLET_BILL:
        ((PhysicsBulletBill*)pObject)->mBulletBill->Hide(false);
        break;
    case PHYSOBJ_PATCH:
        if (((PhysicsPatch*)pObject)->m_Type == 0
            && !((PhysicsPatch*)pObject)->m_bKillMe)
        {
            CreateExplosionShockwave(&pObject->GetPosition());
            pObject->Unknown0();
        }
        break;
    case PHYSOBJ_BIRDO_EGG:
    case PHYSOBJ_KOOPA_SHELL:
        break;
    default:
        break;
    }
}

void HandleCollisionPatchPatch(void* data)
{
    unsigned char* object = *(unsigned char**)((unsigned char*)data + 0x10);
    if (*(int*)(object + 0x48) == 0 && object[0x65] == false)
    {
        CreateExplosionShockwave(&((PhysicsObject*)object)->GetPosition());
        ((PhysicsObject*)object)->Unknown0();
    }
}

void QueueBallFall()
{
    gPhysicsEventQueue->mBallFallEvent.Queue(Function<FnVoidVoid>());
}

void QueueCollisionPlayerPlayer(CollisionPlayerPlayerData* data)
{
    Function<CollisionPlayerPlayerData*> disposer(
        (void (*)(CollisionPlayerPlayerData*))FreeCollisionPlayerPlayerData);
    gPhysicsEventQueue->mCollisionPlayerPlayerEvent.Queue(data, disposer);
}

void QueueCollisionPlayerWall(CollisionPlayerWallData* data)
{
    gPhysicsEventQueue->mCollisionPlayerWallEvent.Queue(data, Function<CollisionPlayerWallData*>(FreeCollisionPlayerWallData));
}

void QueueCollisionTongue(CollisionPatchData* data)
{
    gPhysicsEventQueue->mCollisionTongueEvent.Queue(
        data, Function<CollisionPatchData*>((void (*)(CollisionPatchData*))FreeCollisionPatchData));
}

void QueueCollisionBallTronWall()
{
    gPhysicsEventQueue->mCollisionBallTronWallEvent.Queue(Function<FnVoidVoid>());
}

void QueueCollisionPlayerBall(CollisionPlayerBallData* data)
{
    gPhysicsEventQueue->mCollisionPlayerBallEvent.Queue(
        data, Function<CollisionPlayerBallData*>(FreeCollisionPlayerBallData));
}

void QueueCollisionBallNetmesh(BallNetmeshEventData* data, bool release)
{
    if (!release)
    {
        gPhysicsEventQueue->mCollisionBallNetmeshEvent.Queue(
            data, Function<BallNetmeshEventData*>((void (*)(BallNetmeshEventData*))FreeBallNetmeshEventData));
    }
    else
    {
        g_BallNetmeshEventDataPool.Free(data);
    }
}

void QueueCollisionBallGround(CollisionBallGroundData* data)
{
    gPhysicsEventQueue->mCollisionBallGroundEvent.Queue(
        data, Function<CollisionBallGroundData*>(FreeCollisionBallGroundData));
}

void QueueCollisionBallWall(CollisionBallWallData* data)
{
    gPhysicsEventQueue->mCollisionBallWallEvent.Queue(
        data, Function<CollisionBallWallData*>(FreeCollisionBallWallData));
}

void QueueCollisionBallGoalpost(CollisionBallGoalpostData* data)
{
    gPhysicsEventQueue->mCollisionBallGoalpostEvent.Queue(
        data, Function<CollisionBallGoalpostData*>(FreeCollisionBallGoalpostData));
}

void QueueCollisionBallShell(CollisionBallShellData* data)
{
    gPhysicsEventQueue->mCollisionBallShellEvent.Queue(
        data, Function<CollisionBallShellData*>(FreeCollisionBallShellData));
}

void QueueCollisionBallChain(CollisionBallChainData* data)
{
    gPhysicsEventQueue->mCollisionBallChainEvent.Queue(
        data, Function<CollisionBallChainData*>(FreeCollisionBallChainData));
}

void QueueCollisionKoopaShotBallPlayer(CollisionKoopaShotBallPlayerData* data)
{
    gPhysicsEventQueue->mCollisionKoopaShotBallPlayerEvent.Queue(
        data, Function<CollisionKoopaShotBallPlayerData*>(FreeCollisionKoopaShotBallPlayerData));
}

void QueueCollisionKoopaShellGoalie(CollisionKoopaShellGoalieData* data)
{
    gPhysicsEventQueue->mCollisionKoopaShellGoalieEvent.Queue(
        data, Function<CollisionKoopaShellGoalieData*>(FreeCollisionKoopaShellGoalieData));
}

void QueueCollisionKoopaShellEnd(CollisionKoopaShellEndData* data)
{
    gPhysicsEventQueue->mCollisionKoopaShellEndEvent.Queue(
        data, Function<CollisionKoopaShellEndData*>((void (*)(CollisionKoopaShellEndData*))FreeCollisionKoopaShellEndData));
}

void QueueCollisionBirdoShotBallPlayer(CollisionBirdoShotBallPlayerData* data)
{
    gPhysicsEventQueue->mCollisionBirdoShotBallPlayerEvent.Queue(
        data, Function<CollisionBirdoShotBallPlayerData*>(FreeCollisionBirdoShotBallPlayerData));
}

void QueueCollisionBirdoEggGoalie(CollisionBirdoEggGoalieData* data)
{
    gPhysicsEventQueue->mCollisionBirdoEggGoalieEvent.Queue(
        data, Function<CollisionBirdoEggGoalieData*>(FreeCollisionBirdoEggGoalieData));
}

void QueueCollisionBirdoEggEnd(CollisionBirdoEggEndData* data)
{
    gPhysicsEventQueue->mCollisionBirdoEggEndEvent.Queue(
        data, Function<CollisionBirdoEggEndData*>((void (*)(CollisionBirdoEggEndData*))FreeCollisionBirdoEggEndData));
}

void QueueCollisionPowerupGround(CollisionPowerupGroundData* data)
{
    gPhysicsEventQueue->mCollisionPowerupGroundEvent.Queue(
        data, Function<CollisionPowerupGroundData*>((void (*)(CollisionPowerupGroundData*))FreeCollisionPowerupGroundData));
}

void QueueCollisionPowerupGoalie(CollisionPowerupGroundData* data)
{
    gPhysicsEventQueue->mCollisionPowerupGoalieEvent.Queue(
        data, Function<CollisionPowerupGroundData*>((void (*)(CollisionPowerupGroundData*))FreeCollisionPowerupGroundData));
}

void QueueCollisionPowerupWall(CollisionPowerupWallData* data)
{
    gPhysicsEventQueue->mCollisionPowerupWallEvent.Queue(
        data, Function<CollisionPowerupWallData*>(FreeCollisionPowerupWallData));
}

void QueuePowerupHit(PowerupHitPlayerEventData* data)
{
    gPhysicsEventQueue->mPowerupHitEvent.Queue(
        data, Function<PowerupHitPlayerEventData*>((void (*)(PowerupHitPlayerEventData*))FreePowerupHitPlayerEventData));
}

void QueueCollisionPlayerBanana(CollisionPlayerBananaData* data)
{
    gPhysicsEventQueue->mCollisionPlayerBananaEvent.Queue(
        data, Function<CollisionPlayerBananaData*>(FreeCollisionPlayerBananaData));
}

void QueueCollisionPlayerShell(CollisionPlayerShellData* data)
{
    gPhysicsEventQueue->mCollisionPlayerShellEvent.Queue(
        data, Function<CollisionPlayerShellData*>(FreeCollisionPlayerShellData));
}

void QueueCollisionPlayerFreeze(CollisionPlayerFreezeData* data)
{
    gPhysicsEventQueue->mCollisionPlayerFreezeEvent.Queue(
        data, Function<CollisionPlayerFreezeData*>(FreeCollisionPlayerFreezeData));
}

void QueueCollisionBulletBillPlayer(CollisionBulletBillData* data)
{
    gPhysicsEventQueue->mCollisionBulletBillPlayerEvent.Queue(
        data, Function<CollisionBulletBillData*>(FreeCollisionBulletBillData));
}

void QueueCollisionBulletBillFreeze(CollisionBulletBillData* data)
{
    gPhysicsEventQueue->mCollisionBulletBillFreezeEvent.Queue(
        data, Function<CollisionBulletBillData*>(FreeCollisionBulletBillData));
}

void QueueExplosionBulletBill(CollisionBulletBillData* data)
{
    gPhysicsEventQueue->mExplosionBulletBillEvent.Queue(
        data, Function<CollisionBulletBillData*>(FreeCollisionBulletBillData));
}

void QueuePowerupUsed(PowerupUsedEventData* data)
{
    gPhysicsEventQueue->mPowerupUsedEvent.Queue(
        data, Function<PowerupUsedEventData*>((void (*)(PowerupUsedEventData*))FreePowerupUsedEventData));
}

void QueueCollisionChainCrowd(ChainChomp* data)
{
    gPhysicsEventQueue->mCollisionChainCrowdEvent.Queue(data, Function<ChainChomp*>());
}

void QueueCollisionChainPowerup(CollisionChainPowerupData* data)
{
    gPhysicsEventQueue->mCollisionChainPowerupEvent.Queue(
        data, Function<CollisionChainPowerupData*>(FreeCollisionChainPowerupData));
}

void QueueCollisionPatchPlayer(CollisionPatchData* data)
{
    gPhysicsEventQueue->mCollisionPatchPlayerEvent.Queue(
        data, Function<CollisionPatchData*>((void (*)(CollisionPatchData*))FreeCollisionPatchData));
}

void QueueCollisionPatchGround(CollisionPatchData* data)
{
    gPhysicsEventQueue->mCollisionPatchGroundEvent.Queue(
        data, Function<CollisionPatchData*>((void (*)(CollisionPatchData*))FreeCollisionPatchData));
}

void QueueCollisionPatchPowerup(CollisionPatchPowerupData* data)
{
    gPhysicsEventQueue->mCollisionPatchPowerupEvent.Queue(
        data, Function<CollisionPatchPowerupData*>((void (*)(CollisionPatchPowerupData*))FreeCollisionPatchPowerupData));
}

void QueueCollisionPatchChain(ChainChomp* data)
{
    gPhysicsEventQueue->mCollisionPatchChainEvent.Queue(data, Function<ChainChomp*>());
}

void QueueCollisionPatchPatch(CollisionPatchData* data)
{
    gPhysicsEventQueue->mCollisionPatchPatchEvent.Queue(
        data, Function<CollisionPatchData*>((void (*)(CollisionPatchData*))FreeCollisionPatchData));
}

void QueueCollisionPatchBall(PhysicsPatch* data)
{
    gPhysicsEventQueue->mCollisionPatchBallEvent.Queue(data, Function<PhysicsPatch*>());
}

void QueueCollisionPatchWall(PhysicsPatch* data)
{
    gPhysicsEventQueue->mCollisionPatchWallEvent.Queue(data, Function<PhysicsPatch*>());
}

void QueueCollisionHammerPlayer(CollisionProjectileData* data)
{
    gPhysicsEventQueue->mCollisionHammerPlayerEvent.Queue(
        data, Function<CollisionProjectileData*>(FreeCollisionProjectileData));
}

void QueueCollisionHammerGround(CollisionProjectileData* data)
{
    gPhysicsEventQueue->mCollisionHammerGroundEvent.Queue(
        data, Function<CollisionProjectileData*>(FreeCollisionProjectileData));
}

void QueueCollisionHammerPowerup(PowerupBase* data)
{
    gPhysicsEventQueue->mCollisionHammerPowerupEvent.Queue(data, Function<PowerupBase*>());
}

void QueueBirdoEggDestroyPowerup(PowerupBase* data)
{
    gPhysicsEventQueue->mDestroyPowerupEvent.Queue(data, Function<PowerupBase*>());
}

void QueueKoopaShellDestroyPowerup(PowerupBase* data)
{
    gPhysicsEventQueue->mDestroyPowerupEvent.Queue(data, Function<PowerupBase*>());
}

void QueueBirdoEggDestroyHammer(HammerObject* data)
{
    gPhysicsEventQueue->mDestroyHammerEvent.Queue(data, Function<HammerObject*>());
}

void QueueKoopaShellDestroyHammer(HammerObject* data)
{
    gPhysicsEventQueue->mDestroyHammerEvent.Queue(data, Function<HammerObject*>());
}

void QueueBirdoEggKnockYoshiTongue(cFielder* data)
{
    gPhysicsEventQueue->mKnockYoshiTongueEvent.Queue(data, Function<cFielder*>());
}

void QueueKoopaShellKnockYoshiTongue(cFielder* data)
{
    gPhysicsEventQueue->mKnockYoshiTongueEvent.Queue(data, Function<cFielder*>());
}

void QueueCollisionHammerChain(ChainChomp* data)
{
    gPhysicsEventQueue->mCollisionHammerChainEvent.Queue(data, Function<ChainChomp*>());
}

void QueueCollisionThwompPlayer(ThwompObject* thwomp, cCharacter* target)
{
    CollisionThwompPlayerData* data = 0;
    g_CollisionThwompPlayerDataPool.Allocate(data);
    data->thwomp = thwomp;
    data->state = thwomp->mState;
    data->target = target;
    gPhysicsEventQueue->mCollisionThwompPlayerEvent.Queue(
        data,
        Function<CollisionThwompPlayerData*>(FreeCollisionThwompPlayerData));
}

void QueueCollisionThwompBall(ThwompObject* data)
{
    gPhysicsEventQueue->mCollisionThwompBallEvent.Queue(data, Function<ThwompObject*>());
}

void QueueCollisionEggBall(CollisionEggData* data)
{
    gPhysicsEventQueue->mCollisionEggBallEvent.Queue(
        data, Function<CollisionEggData*>(FreeCollisionEggData));
}

void QueueCollisionEggPlayer(CollisionEggData* data)
{
    gPhysicsEventQueue->mCollisionEggPlayerEvent.Queue(
        data, Function<CollisionEggData*>(FreeCollisionEggData));
}

void QueueCollisionCrackEgg(CollisionEggData* data)
{
    gPhysicsEventQueue->mCollisionCrackEggEvent.Queue(
        data, Function<CollisionEggData*>(FreeCollisionEggData));
}

void QueueCollisionDebrisBall(WindDebris* data)
{
    gPhysicsEventQueue->mCollisionDebrisBallEvent.Queue(data, Function<WindDebris*>());
}

void QueueCollisionWaluigiWall(cFielder* data)
{
    gPhysicsEventQueue->mCollisionWaluigiWallEvent.Queue(data, Function<cFielder*>());
}

void QueueCollisionShockwave(CollisionShockwaveData* data)
{
    gPhysicsEventQueue->mCollisionShockwaveEvent.Queue(
        data, Function<CollisionShockwaveData*>(
                  (void (*)(CollisionShockwaveData*))FreeCollisionShockwaveData));
}

#include "NL/nlBind_impl.h"

#include "Game/EventBase.inl"
