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
#include "Game/UnidentifiedStaticStorage.h"
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
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mBallFallEvent;
    UnidentifiedQueuedEvent<CollisionPlayerPlayerData> mCollisionPlayerPlayerEvent;
    UnidentifiedQueuedEvent<CollisionPlayerWallData> mCollisionPlayerWallEvent;
    UnidentifiedQueuedEvent<CollisionPlayerBallData> mCollisionPlayerBallEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData04> mCollisionBallNetmeshEvent;
    UnidentifiedQueuedEvent<CollisionBallGroundData> mCollisionBallGroundEvent;
    UnidentifiedQueuedEvent<CollisionBallWallData> mCollisionBallWallEvent;
    UnidentifiedQueuedEvent<CollisionBallGoalpostData> mCollisionBallGoalpostEvent;
    UnidentifiedQueuedEvent<CollisionBallShellData> mCollisionBallShellEvent;
    UnidentifiedQueuedEvent<CollisionBallChainData> mCollisionBallChainEvent;
    UnidentifiedQueuedEvent<CollisionKoopaShotBallPlayerData> mCollisionKoopaShotBallPlayerEvent;
    UnidentifiedQueuedEvent<CollisionKoopaShellGoalieData> mCollisionKoopaShellGoalieEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData12> mCollisionKoopaShellEndEvent;
    UnidentifiedQueuedEvent<CollisionBirdoShotBallPlayerData> mCollisionBirdoShotBallPlayerEvent;
    UnidentifiedQueuedEvent<CollisionBirdoEggGoalieData> mCollisionBirdoEggGoalieEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData15> mCollisionBirdoEggEndEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData16> mCollisionHammerbroShotBallPlayerEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData17> mCollisionPowerupGroundEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData17> mCollisionPowerupGoalieEvent;
    UnidentifiedQueuedEvent<CollisionPowerupWallData> mCollisionPowerupWallEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData19> mPowerupHitEvent;
    UnidentifiedQueuedEvent<CollisionPlayerBananaData> mCollisionPlayerBananaEvent;
    UnidentifiedQueuedEvent<CollisionPlayerShellData> mCollisionPlayerShellEvent;
    UnidentifiedQueuedEvent<CollisionPlayerFreezeData> mCollisionPlayerFreezeEvent;
    UnidentifiedQueuedEvent<CollisionBulletBillData> mCollisionBulletBillPlayerEvent;
    UnidentifiedQueuedEvent<CollisionBulletBillData> mCollisionBulletBillFreezeEvent;
    UnidentifiedQueuedEvent<CollisionBulletBillData> mExplosionBulletBillEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData24> mCollisionTongueEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mCollisionBallTronWallEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData25> mPowerupUsedEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mCollisionFireballPlayerEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mCollisionFireballBallEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mCollisionFireballGroundEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData27> mCollisionFireballPowerupEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mCollisionFireballChainEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mCollisionChainCrowdEvent;
    UnidentifiedQueuedEvent<CollisionChainPowerupData> mCollisionChainPowerupEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData24> mCollisionPatchPlayerEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData24> mCollisionPatchGroundEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData30> mCollisionPatchPowerupEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mCollisionPatchChainEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData24> mCollisionPatchPatchEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData31> mCollisionPatchBallEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData31> mCollisionPatchWallEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mCollisionHammerPlayerEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mCollisionHammerBallEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mCollisionHammerGroundEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData27> mCollisionHammerPowerupEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mCollisionHammerChainEvent;
    UnidentifiedQueuedEvent<CollisionThwompPlayerData> mCollisionThwompPlayerEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData33> mCollisionThwompBallEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mCollisionThwompChainEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mCollisionEggBallEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mCollisionEggPlayerEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mCollisionEggPowerupEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mCollisionEggChainEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mCollisionCrackEggEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData27> mDestroyPowerupEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData35> mDestroyHammerEvent;
    UnidentifiedQueuedEvent<cFielder> mKnockYoshiTongueEvent;
    UnidentifiedQueuedEvent<UnidentifiedNPC_801B43F8> mCollisionDebrisBallEvent;
    UnidentifiedQueuedEvent<cFielder> mCollisionWaluigiWallEvent;
    UnidentifiedQueuedEvent<CollisionShockwaveData> mCollisionShockwaveEvent;
};

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
float gfDaisyFistShotTime = 1.0f;

SlotPool<UnidentifiedEventData26> g_UnidentifiedEventData26Pool(16, 16);
SlotPool<UnidentifiedEventData24> g_UnidentifiedEventData24Pool(16, 16);
SlotPool<UnidentifiedEventData30> g_UnidentifiedEventData30Pool(16, 16);
SlotPool<UnidentifiedEventData34> g_UnidentifiedEventData34Pool(16, 16);
SlotPool<CollisionShockwaveData> gCollisionShockwaveDataPool(16, 16);

PhysicsEventQueue* gPhysicsEventQueue;

void FreePhysicsEventDataPools()
{
    g_UnidentifiedEventData26Pool.FreeBlocks();
    g_UnidentifiedEventData24Pool.FreeBlocks();
    g_UnidentifiedEventData30Pool.FreeBlocks();
    g_UnidentifiedEventData34Pool.FreeBlocks();
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
    UnidentifiedFindEvent<void>("CollisionPatchPowerup", -1)->Add(Function<void*>(HandleCollisionPatchPowerup), 0, -1);
    UnidentifiedFindEvent<void>("CollisionHammerPowerup", -1)->Add(Function<void*>(HandleCollisionHammerPowerup), 0, -1);
    UnidentifiedFindEvent<void>("CollisionFireballPowerup", -1)->Add(Function<void*>(HandleCollisionFireballPowerup), 0, -1);
    UnidentifiedFindEvent<void>("CollisionCrackEgg", -1)->Add(Function<void*>(HandleCollisionCrackEgg), 0, -1);
    UnidentifiedFindEvent<void>("CollisionShockwave", -1)->Add(Function<void*>((void (*)(void*))HandleCollisionShockwave), 0, -1);
    UnidentifiedFindEvent<void>("CollisionKoopaShellEnd", -1)->Add(Function<void*>(HandleCollisionKoopaShellEnd), 0, -1);
    UnidentifiedFindEvent<void>("CollisionBirdoEggEnd", -1)->Add(Function<void*>(HandleCollisionBirdoEggEnd), 0, -1);
    UnidentifiedFindEvent<void>("CollisionPatchPatch", -1)->Add(Function<void*>(HandleCollisionPatchPatch), 0, -1);
    UnidentifiedFindEvent<void>("DestroyPowerup", -1)->Add(Function<void*>(HandleDestroyPowerup), 0, -1);
    UnidentifiedFindEvent<void>("DestroyHammer", -1)->Add(Function<void*>(HandleDestroyHammer), 0, -1);
    UnidentifiedFindEvent<void>("CollisionWaluigiWall", -1)->Add(Function<void*>(HandleCollisionWaluigiWall), 0, -1);
}

extern "C" void fn_800156F8(cBall* pBall, cPlayer* pShooter);
extern "C" void fn_80015B38(cBall* pBall, bool bParam);
extern "C" void fn_8002E5F4(cFielder* pFielder, int nParam);
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
        fn_8002E5F4((cFielder*)object, 0);
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
        case 3:
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
    case 4:
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
                fn_80032534(pFielder, pShockwave->mPosition);
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
                    g_pGame->mUnidentified49C.mEvent31.Queue(pStats,
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
                    != GOALIEACTION_UNIDENTIFIED_29)
                {
                    pGoalie->fn_8008EC2C();
                }
                break;
            case SHOCKWAVE_LIGHTNING:
                if (pGoalie->mGoalieActionState
                    != GOALIEACTION_ELECTROCUTION)
                {
                    pGoalie->fn_8008E130();
                }
                break;
            }
        }
        break;
    }
    case 16:
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
                ((cCharacter*)pShockwave->mOwner)->mUnidentified024.m_v3Position, gfDaisyFistShotTime);
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
    case 21:
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
    case 20:
        if (shockwaveType == SHOCKWAVE_HIT)
        {
            ((PhysicsShell*)pObject)->m_pPowerupObject->fn_8009CEBC(
                pShockwave->mPosition);
            break;
        }
        ((PhysicsShell*)pObject)->m_pPowerupObject->m_bShouldDestroy = true;
        break;
    case 31:
        ((PhysicsHammer*)pObject)->mHammer->Deactivate(true);
        break;
    case 29:
        ((PhysicsWaluigiWall*)pObject)->ApplyDamage(0.35f);
        break;
    case 32:
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
            fn_8002E5F4(
                ((PhysicsYoshiEgg*)pObject)->mYoshiEgg->mFielder,
                0);
        }
        break;
    case 30:
        ((PhysicsBulletBill*)pObject)->mBulletBill->Hide(false);
        break;
    case 28:
        if (((PhysicsPatch*)pObject)->m_Type == 0
            && !((PhysicsPatch*)pObject)->m_bKillMe)
        {
            CreateExplosionShockwave(&pObject->GetPosition());
            pObject->Unknown0();
        }
        break;
    case 33:
    case 34:
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

void QueueCollisionTongue(UnidentifiedEventData24* data)
{
    gPhysicsEventQueue->mCollisionTongueEvent.Queue(
        data, Function<UnidentifiedEventData24*>((void (*)(UnidentifiedEventData24*))FreeUnidentifiedEventData24));
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
            (UnidentifiedEventData04*)data, Function<UnidentifiedEventData04*>((void (*)(UnidentifiedEventData04*))FreeBallNetmeshEventData));
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
        (UnidentifiedEventData12*)data, Function<UnidentifiedEventData12*>((void (*)(UnidentifiedEventData12*))FreeCollisionKoopaShellEndData));
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
        (UnidentifiedEventData15*)data, Function<UnidentifiedEventData15*>((void (*)(UnidentifiedEventData15*))FreeCollisionBirdoEggEndData));
}

void QueueCollisionPowerupGround(CollisionPowerupGroundData* data)
{
    gPhysicsEventQueue->mCollisionPowerupGroundEvent.Queue(
        (UnidentifiedEventData17*)data, Function<UnidentifiedEventData17*>((void (*)(UnidentifiedEventData17*))FreeCollisionPowerupGroundData));
}

void QueueCollisionPowerupGoalie(CollisionPowerupGroundData* data)
{
    gPhysicsEventQueue->mCollisionPowerupGoalieEvent.Queue(
        (UnidentifiedEventData17*)data, Function<UnidentifiedEventData17*>((void (*)(UnidentifiedEventData17*))FreeCollisionPowerupGroundData));
}

void QueueCollisionPowerupWall(CollisionPowerupWallData* data)
{
    gPhysicsEventQueue->mCollisionPowerupWallEvent.Queue(
        data, Function<CollisionPowerupWallData*>(FreeCollisionPowerupWallData));
}

void QueuePowerupHit(PowerupHitPlayerEventData* data)
{
    gPhysicsEventQueue->mPowerupHitEvent.Queue(
        (UnidentifiedEventData19*)data, Function<UnidentifiedEventData19*>((void (*)(UnidentifiedEventData19*))FreePowerupHitPlayerEventData));
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
        (UnidentifiedEventData25*)data, Function<UnidentifiedEventData25*>((void (*)(UnidentifiedEventData25*))FreePowerupUsedEventData));
}

void QueueCollisionChainCrowd(UnidentifiedEventData28* data)
{
    gPhysicsEventQueue->mCollisionChainCrowdEvent.Queue(data, Function<UnidentifiedEventData28*>());
}

void QueueCollisionChainPowerup(CollisionChainPowerupData* data)
{
    gPhysicsEventQueue->mCollisionChainPowerupEvent.Queue(
        data, Function<CollisionChainPowerupData*>(FreeCollisionChainPowerupData));
}

void QueueCollisionPatchPlayer(UnidentifiedEventData24* data)
{
    gPhysicsEventQueue->mCollisionPatchPlayerEvent.Queue(
        data, Function<UnidentifiedEventData24*>((void (*)(UnidentifiedEventData24*))FreeUnidentifiedEventData24));
}

void QueueCollisionPatchGround(UnidentifiedEventData24* data)
{
    gPhysicsEventQueue->mCollisionPatchGroundEvent.Queue(
        data, Function<UnidentifiedEventData24*>((void (*)(UnidentifiedEventData24*))FreeUnidentifiedEventData24));
}

void QueueCollisionPatchPowerup(UnidentifiedEventData30* data)
{
    gPhysicsEventQueue->mCollisionPatchPowerupEvent.Queue(
        data, Function<UnidentifiedEventData30*>((void (*)(UnidentifiedEventData30*))FreeUnidentifiedEventData30));
}

void QueueCollisionPatchChain(UnidentifiedEventData28* data)
{
    gPhysicsEventQueue->mCollisionPatchChainEvent.Queue(data, Function<UnidentifiedEventData28*>());
}

void QueueCollisionPatchPatch(UnidentifiedEventData24* data)
{
    gPhysicsEventQueue->mCollisionPatchPatchEvent.Queue(
        data, Function<UnidentifiedEventData24*>((void (*)(UnidentifiedEventData24*))FreeUnidentifiedEventData24));
}

void QueueCollisionPatchBall(UnidentifiedEventData31* data)
{
    gPhysicsEventQueue->mCollisionPatchBallEvent.Queue(data, Function<UnidentifiedEventData31*>());
}

void QueueCollisionPatchWall(UnidentifiedEventData31* data)
{
    gPhysicsEventQueue->mCollisionPatchWallEvent.Queue(data, Function<UnidentifiedEventData31*>());
}

void QueueCollisionHammerPlayer(UnidentifiedEventData26* data)
{
    gPhysicsEventQueue->mCollisionHammerPlayerEvent.Queue(
        data, Function<UnidentifiedEventData26*>(FreeUnidentifiedEventData26));
}

void QueueCollisionHammerGround(UnidentifiedEventData26* data)
{
    gPhysicsEventQueue->mCollisionHammerGroundEvent.Queue(
        data, Function<UnidentifiedEventData26*>(FreeUnidentifiedEventData26));
}

void QueueCollisionHammerPowerup(UnidentifiedEventData27* data)
{
    gPhysicsEventQueue->mCollisionHammerPowerupEvent.Queue(data, Function<UnidentifiedEventData27*>());
}

void QueueBirdoEggDestroyPowerup(UnidentifiedEventData27* data)
{
    gPhysicsEventQueue->mDestroyPowerupEvent.Queue(data, Function<UnidentifiedEventData27*>());
}

void QueueKoopaShellDestroyPowerup(UnidentifiedEventData27* data)
{
    gPhysicsEventQueue->mDestroyPowerupEvent.Queue(data, Function<UnidentifiedEventData27*>());
}

void QueueBirdoEggDestroyHammer(UnidentifiedEventData35* data)
{
    gPhysicsEventQueue->mDestroyHammerEvent.Queue(data, Function<UnidentifiedEventData35*>());
}

void QueueKoopaShellDestroyHammer(UnidentifiedEventData35* data)
{
    gPhysicsEventQueue->mDestroyHammerEvent.Queue(data, Function<UnidentifiedEventData35*>());
}

void QueueBirdoEggKnockYoshiTongue(cFielder* data)
{
    gPhysicsEventQueue->mKnockYoshiTongueEvent.Queue(data, Function<cFielder*>());
}

void QueueKoopaShellKnockYoshiTongue(cFielder* data)
{
    gPhysicsEventQueue->mKnockYoshiTongueEvent.Queue(data, Function<cFielder*>());
}

void QueueCollisionHammerChain(UnidentifiedEventData28* data)
{
    gPhysicsEventQueue->mCollisionHammerChainEvent.Queue(data, Function<UnidentifiedEventData28*>());
}

void QueueCollisionThwompPlayer(void* source, cCharacter* target)
{
    CollisionThwompPlayerData* data = 0;
    g_CollisionThwompPlayerDataPool.Allocate(data);
    data->source = source;
    data->sourceValue = *(void**)source;
    data->target = target;
    gPhysicsEventQueue->mCollisionThwompPlayerEvent.Queue(
        data,
        Function<CollisionThwompPlayerData*>(FreeCollisionThwompPlayerData));
}

void QueueCollisionThwompBall(UnidentifiedEventData33* data)
{
    gPhysicsEventQueue->mCollisionThwompBallEvent.Queue(data, Function<UnidentifiedEventData33*>());
}

void QueueCollisionEggBall(UnidentifiedEventData34* data)
{
    gPhysicsEventQueue->mCollisionEggBallEvent.Queue(
        data, Function<UnidentifiedEventData34*>(FreeUnidentifiedEventData34));
}

void QueueCollisionEggPlayer(UnidentifiedEventData34* data)
{
    gPhysicsEventQueue->mCollisionEggPlayerEvent.Queue(
        data, Function<UnidentifiedEventData34*>(FreeUnidentifiedEventData34));
}

void QueueCollisionCrackEgg(UnidentifiedEventData34* data)
{
    gPhysicsEventQueue->mCollisionCrackEggEvent.Queue(
        data, Function<UnidentifiedEventData34*>(FreeUnidentifiedEventData34));
}

void QueueCollisionDebrisBall(UnidentifiedNPC_801B43F8* data)
{
    gPhysicsEventQueue->mCollisionDebrisBallEvent.Queue(data, Function<UnidentifiedNPC_801B43F8*>());
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
