#include "Game/Task/DispatchEventsTask.h"
#include "Game/Render/BirdoEgg.h"
#include "Game/Render/BulletBill.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Audio/GameStreams.h"
#include "Game/EventDispatcher.inl"

#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/Event.h"
#include "Game/EventDataTypes.h"
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
#include "NL/nlAVLTree.h"
#include "NL/nlBind.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"
#include "NL/globalpad.h"
#include "unclassified/tu_80175F8C.h"
#include "Game/Physics/PhysicsWaluigiWall.h"
#include "unclassified/tu_801A0E64.h"
#include "Game/Render/KoopaShellObject.h"
#include "Game/Render/YoshiEggObject.h"
#include "Game/UnidentifiedStaticStorage.h"

extern "C" void fn_8017617C();
extern "C" void fn_801761E0();
extern "C" void fn_80144AB8();


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
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent00;
    UnidentifiedQueuedEvent<CollisionPlayerPlayerData> mEvent01;
    UnidentifiedQueuedEvent<CollisionPlayerWallData> mEvent02;
    UnidentifiedQueuedEvent<CollisionPlayerBallData> mEvent03;
    UnidentifiedQueuedEvent<UnidentifiedEventData04> mEvent04;
    UnidentifiedQueuedEvent<CollisionBallGroundData> mEvent05;
    UnidentifiedQueuedEvent<CollisionBallWallData> mEvent06;
    UnidentifiedQueuedEvent<CollisionBallGoalpostData> mEvent07;
    UnidentifiedQueuedEvent<CollisionBallShellData> mEvent08;
    UnidentifiedQueuedEvent<CollisionBallChainData> mEvent09;
    UnidentifiedQueuedEvent<CollisionKoopaShotBallPlayerData> mEvent10;
    UnidentifiedQueuedEvent<CollisionKoopaShellGoalieData> mEvent11;
    UnidentifiedQueuedEvent<UnidentifiedEventData12> mEvent12;
    UnidentifiedQueuedEvent<CollisionBirdoShotBallPlayerData> mEvent13;
    UnidentifiedQueuedEvent<CollisionBirdoEggGoalieData> mEvent14;
    UnidentifiedQueuedEvent<UnidentifiedEventData15> mEvent15;
    UnidentifiedQueuedEvent<UnidentifiedEventData16> mEvent16;
    UnidentifiedQueuedEvent<UnidentifiedEventData17> mEvent17;
    UnidentifiedQueuedEvent<UnidentifiedEventData17> mEvent18;
    UnidentifiedQueuedEvent<CollisionPowerupWallData> mEvent19;
    UnidentifiedQueuedEvent<UnidentifiedEventData19> mEvent20;
    UnidentifiedQueuedEvent<CollisionPlayerBananaData> mEvent21;
    UnidentifiedQueuedEvent<CollisionPlayerShellData> mEvent22;
    UnidentifiedQueuedEvent<CollisionPlayerFreezeData> mEvent23;
    UnidentifiedQueuedEvent<CollisionBulletBillData> mEvent24;
    UnidentifiedQueuedEvent<CollisionBulletBillData> mEvent25;
    UnidentifiedQueuedEvent<CollisionBulletBillData> mEvent26;
    UnidentifiedQueuedEvent<UnidentifiedEventData24> mEvent27;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent28;
    UnidentifiedQueuedEvent<UnidentifiedEventData25> mEvent29;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mEvent30;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mEvent31;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mEvent32;
    UnidentifiedQueuedEvent<UnidentifiedEventData27> mEvent33;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mEvent34;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mEvent35;
    UnidentifiedQueuedEvent<CollisionChainPowerupData> mEvent36;
    UnidentifiedQueuedEvent<UnidentifiedEventData24> mEvent37;
    UnidentifiedQueuedEvent<UnidentifiedEventData24> mEvent38;
    UnidentifiedQueuedEvent<UnidentifiedEventData30> mEvent39;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mEvent40;
    UnidentifiedQueuedEvent<UnidentifiedEventData24> mEvent41;
    UnidentifiedQueuedEvent<UnidentifiedEventData31> mEvent42;
    UnidentifiedQueuedEvent<UnidentifiedEventData31> mEvent43;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mEvent44;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mEvent45;
    UnidentifiedQueuedEvent<UnidentifiedEventData26> mEvent46;
    UnidentifiedQueuedEvent<UnidentifiedEventData27> mEvent47;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mEvent48;
    UnidentifiedQueuedEvent<CollisionThwompPlayerData> mEvent49;
    UnidentifiedQueuedEvent<UnidentifiedEventData33> mEvent50;
    UnidentifiedQueuedEvent<UnidentifiedEventData28> mEvent51;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mEvent52;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mEvent53;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mEvent54;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mEvent55;
    UnidentifiedQueuedEvent<UnidentifiedEventData34> mEvent56;
    UnidentifiedQueuedEvent<UnidentifiedEventData27> mEvent57;
    UnidentifiedQueuedEvent<UnidentifiedEventData35> mEvent58;
    UnidentifiedQueuedEvent<cFielder> mEvent59;
    UnidentifiedQueuedEvent<UnidentifiedNPC_801B43F8> mEvent60;
    UnidentifiedQueuedEvent<cFielder> mEvent61;
    UnidentifiedQueuedEvent<UnidentifiedEventData38> mEvent62;
};

extern "C" PhysicsEventQueue* lbl_806E11F0;

PhysicsEventQueue::PhysicsEventQueue()
    : mDispatcher("PhysicsEventQueue")
    , mEvent00(&mDispatcher, "BallFall", -1)
    , mEvent01(&mDispatcher, "CollisionPlayerPlayer", -1)
    , mEvent02(&mDispatcher, "CollisionPlayerWall", -1)
    , mEvent03(&mDispatcher, "CollisionPlayerBall", -1)
    , mEvent04(&mDispatcher, "CollisionBallNetmesh", -1)
    , mEvent05(&mDispatcher, "CollisionBallGround", -1)
    , mEvent06(&mDispatcher, "CollisionBallWall", -1)
    , mEvent07(&mDispatcher, "CollisionBallGoalpost", -1)
    , mEvent08(&mDispatcher, "CollisionBallShell", -1)
    , mEvent09(&mDispatcher, "CollisionBallChain", -1)
    , mEvent10(&mDispatcher, "CollisionKoopaShotBallPlayer", -1)
    , mEvent11(&mDispatcher, "CollisionKoopaShellGoalie", -1)
    , mEvent12(&mDispatcher, "CollisionKoopaShellEnd", -1)
    , mEvent13(&mDispatcher, "CollisionBirdoShotBallPlayer", -1)
    , mEvent14(&mDispatcher, "CollisionBirdoEggGoalie", -1)
    , mEvent15(&mDispatcher, "CollisionBirdoEggEnd", -1)
    , mEvent16(&mDispatcher, "CollisionHammerbroShotBallPlayer", -1)
    , mEvent17(&mDispatcher, "CollisionPowerupGround", -1)
    , mEvent18(&mDispatcher, "CollisionPowerupGoalie", -1)
    , mEvent19(&mDispatcher, "CollisionPowerupWall", -1)
    , mEvent20(&mDispatcher, "PowerupHit", -1)
    , mEvent21(&mDispatcher, "CollisionPlayerBanana", -1)
    , mEvent22(&mDispatcher, "CollisionPlayerShell", -1)
    , mEvent23(&mDispatcher, "CollisionPlayerFreeze", -1)
    , mEvent24(&mDispatcher, "CollisionBulletBillPlayer", -1)
    , mEvent25(&mDispatcher, "CollisionBulletBillFreeze", -1)
    , mEvent26(&mDispatcher, "ExplosionBulletBill", -1)
    , mEvent27(&mDispatcher, "CollisionTongue", -1)
    , mEvent28(&mDispatcher, "CollisionBallTronWall", -1)
    , mEvent29(&mDispatcher, "PowerupUsed", -1)
    , mEvent30(&mDispatcher, "CollisionFireballPlayer", -1)
    , mEvent31(&mDispatcher, "CollisionFireballBall", -1)
    , mEvent32(&mDispatcher, "CollisionFireballGround", -1)
    , mEvent33(&mDispatcher, "CollisionFireballPowerup", -1)
    , mEvent34(&mDispatcher, "CollisionFireballChain", -1)
    , mEvent35(&mDispatcher, "CollisionChainCrowd", -1)
    , mEvent36(&mDispatcher, "CollisionChainPowerup", -1)
    , mEvent37(&mDispatcher, "CollisionPatchPlayer", -1)
    , mEvent38(&mDispatcher, "CollisionPatchGround", -1)
    , mEvent39(&mDispatcher, "CollisionPatchPowerup", -1)
    , mEvent40(&mDispatcher, "CollisionPatchChain", -1)
    , mEvent41(&mDispatcher, "CollisionPatchPatch", -1)
    , mEvent42(&mDispatcher, "CollisionPatchBall", -1)
    , mEvent43(&mDispatcher, "CollisionPatchWall", -1)
    , mEvent44(&mDispatcher, "CollisionHammerPlayer", -1)
    , mEvent45(&mDispatcher, "CollisionHammerBall", -1)
    , mEvent46(&mDispatcher, "CollisionHammerGround", -1)
    , mEvent47(&mDispatcher, "CollisionHammerPowerup", -1)
    , mEvent48(&mDispatcher, "CollisionHammerChain", -1)
    , mEvent49(&mDispatcher, "CollisionThwompPlayer", -1)
    , mEvent50(&mDispatcher, "CollisionThwompBall", -1)
    , mEvent51(&mDispatcher, "CollisionThwompChain", -1)
    , mEvent52(&mDispatcher, "CollisionEggBall", -1)
    , mEvent53(&mDispatcher, "CollisionEggPlayer", -1)
    , mEvent54(&mDispatcher, "CollisionEggPowerup", -1)
    , mEvent55(&mDispatcher, "CollisionEggChain", -1)
    , mEvent56(&mDispatcher, "CollisionCrackEgg", -1)
    , mEvent57(&mDispatcher, "DestroyPowerup", -1)
    , mEvent58(&mDispatcher, "DestroyHammer", -1)
    , mEvent59(&mDispatcher, "KnockYoshiTongue", -1)
    , mEvent60(&mDispatcher, "CollisionDebrisBall", -1)
    , mEvent61(&mDispatcher, "CollisionWaluigiWall", -1)
    , mEvent62(&mDispatcher, "CollisionShockwave", -1)
{
    fn_80144AB8();
}

PhysicsEventQueue::~PhysicsEventQueue()
{
}

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

struct UnidentifiedMemberFunction
{
    long thisDelta;
    long vtableOffset;
    void* function;
};

extern "C" UnidentifiedEventRegistry* g_pEventRegistry;
extern "C" long __ptmf_test(UnidentifiedMemberFunction*);
extern "C" UnidentifiedMemberFunction lbl_8050F58C;
extern "C" const nlVector3 lbl_804DCC60;

extern "C" void fn_800156F8(void*, void*);
extern "C" void fn_80015B38(void*, int);
extern "C" void fn_8002E5F4(void*, int);
extern "C" PhysicsSphere_80175F8C* fn_80176A60(const nlVector3*);

extern "C" void fn_801452F4(void* object)
{
    ((unsigned char*)object)[4] = true;
}

extern "C" void fn_80145300(void* object)
{
    unsigned char* bytes = (unsigned char*)object;
    if (bytes[4] == false)
    {
        bytes[4] = true;
    }
}

extern "C" void fn_80145318(void* object)
{
    nlVector3 direction;
    nlPolarToCartesian(direction.x, direction.y, *(unsigned short*)((unsigned char*)object + 0x62), 1.0f);
    direction.z = 0.0f;
    ((cFielder*)object)->InitActionShellReact(direction, lbl_804DCC60);
}

extern "C" void fn_80145370(void* object)
{
    if (__ptmf_test(&lbl_8050F58C))
    {
        fn_801A1CFC((HammerObject*)object, 0);
    }
}

extern "C" void fn_801453B8(void* object)
{
    unsigned char* bytes = (unsigned char*)object;
    if (*(int*)(bytes + 0xA4) != 2 || *(int*)(bytes + 0x1C) != 2)
    {
        bytes[4] = true;
    }
}

extern "C" void fn_801453DC(void*)
{
}

extern "C" void fn_801453E0(void* data)
{
    unsigned char* object = *(unsigned char**)((unsigned char*)data + 4);
    if (*(int*)(object + 0xF0) == 2)
    {
        fn_8002E5F4(object, 0);
    }
}

extern "C" void fn_801453FC(void* data)
{
    unsigned char* bytes = (unsigned char*)data;
    if (bytes[4] != false)
    {
        fn_800156F8(
            g_pBall, *(void**)(*(unsigned char**)bytes + 0x2C));
    }
    else
    {
        fn_80015B38(g_pBall, 0);
    }
    (*(KoopaShellObject**)bytes)->Deactivate(false);
}

extern "C" void fn_8014545C(void* data)
{
    unsigned char* bytes = (unsigned char*)data;
    if (bytes[4] != false)
    {
        fn_800156F8(
            g_pBall, *(void**)(*(unsigned char**)bytes + 0x3C));
    }
    else
    {
        fn_80015B38(g_pBall, 0);
    }
    (*(BirdoEggObject**)bytes)->Hide(false);
}

extern "C" void fn_80032534(cFielder*, const nlVector3&);
extern "C" bool fn_800167A8(cBall*);
extern "C" void fn_800ED92C(unsigned long soundID);
extern "C" void fn_80080EFC(cPlayer*);

float lbl_806DCA90 = 1.0f;

extern "C" void fn_801454BC(UnidentifiedEventData38* data)
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

    UnidentifiedEventData_80066A04* pStats;
    bool bInvincible;
    cBall* pBall;
    PhysicsObject* pObject = data->mUnidentified04;
    PhysicsSphere_80175F8C* pShockwave = data->mUnidentified00;
    int effectType = pShockwave->effectType;

    switch (pObject->GetObjectType())
    {
    case 4:
    {
        cCharacter* pCharacter =
            ((PhysicsCharacter*)pObject->m_parentObject)->m_pAICharacter;
        if (pCharacter->m_eClassType == FIELDER)
        {
            cFielder* pFielder = (cFielder*)pCharacter;
            switch (effectType)
            {
            case 1:
                if (pShockwave->owner == pFielder)
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
                fn_80032534(pFielder, pShockwave->position);
                break;
            case 3:
                pFielder->CollideWithFreezeCallback();
                break;
            case 0:
            case 4:
            case 5:
                if (pShockwave->owner == pFielder)
                {
                    break;
                }
                if (pFielder->m_eActionState == ACTION_BOMB_REACT
                    || pFielder->m_eActionState == ACTION_HIT_REACT)
                {
                    break;
                }
                if (pFielder->CollideWithBobombCallback(pShockwave->position,
                        pShockwave->GetRadius())
                    && pShockwave->owner != 0 && effectType != 4)
                {
                    pStats = 0;
                    g_CollisionPowerupStatsDataPool.Allocate(pStats);
                    pStats->mUnidentified08 = (cPlayer*)pShockwave->owner;
                    pStats->mUnidentified0C = pShockwave->sourceIndex;
                    pStats->mUnidentified00 = pFielder;
                    bool bHasPad = pFielder->GetGlobalPad() != 0;
                    pStats->mUnidentified04 =
                        bHasPad ? pFielder->GetGlobalPad()->GetPadID() : -1;
                    g_pGame->mUnidentified49C.mEvent31.Queue(pStats,
                        Function<UnidentifiedEventData_80066A04*>(
                            fn_80025A14));
                }
                break;
            case 2:
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
                pFielder->fn_800451B0(pShockwave->position);
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
            switch (effectType)
            {
            case 4:
                if (pGoalie->mGoalieActionState
                    != GOALIEACTION_UNIDENTIFIED_29)
                {
                    pGoalie->fn_8008EC2C();
                }
                break;
            case 2:
                if (pGoalie->mGoalieActionState
                    != GOALIEACTION_UNIDENTIFIED_27)
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
        if (effectType == 2)
        {
            break;
        }
        if (effectType == 4)
        {
            cCharacter* pOwner = (cCharacter*)pShockwave->owner;
            if (pOwner != 0 && pOwner->m_eClassType == FIELDER)
            {
                fn_800156F8(pBall, pOwner);
            }
            break;
        }

        if (pBall->GetOwnerFielder() != 0)
        {
            cFielder* pOwner = (cFielder*)pShockwave->owner;
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
                static_cast<Goalie*>(pGoalie)->fn_80080EFC();
            }
        }

        eSpinType spinType =
            nlRandom(2) != 0 ? SPINTYPE_FORWARD : SPINTYPE_BACK;
        nlVector3 v3Velocity;
        if (effectType == 5)
        {
            pBall->ShootAtFast(v3Velocity,
                ((cCharacter*)pShockwave->owner)->mUnidentified024.m_v3Position, lbl_806DCA90);
            nlRandom(2);
        }
        else
        {
            nlVec3Sub(v3Velocity, pBall->m_v3Position, pShockwave->position);
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
        if (effectType == 1)
        {
            ((PhysicsBanana*)pObject)->m_pPowerupObject->fn_8009CEBC(
                pShockwave->position);
            break;
        }
        if (effectType == 0
            && ((PhysicsBanana*)pObject)->m_pPowerupObject->m_eType
                == POWER_UP_BOBOMB)
        {
            break;
        }
        ((PhysicsBanana*)pObject)->m_pPowerupObject->m_bShouldDestroy = true;
        break;
    case 20:
        if (effectType == 1)
        {
            ((PhysicsShell*)pObject)->m_pPowerupObject->fn_8009CEBC(
                pShockwave->position);
            break;
        }
        ((PhysicsShell*)pObject)->m_pPowerupObject->m_bShouldDestroy = true;
        break;
    case 31:
        fn_801A1ED0(((PhysicsHammer*)pObject)->mHammer, true);
        break;
    case 29:
        ((PhysicsWaluigiWall*)pObject)->ApplyDamage(0.35f);
        break;
    case 32:
        if (((PhysicsYoshiEgg*)pObject)->mYoshiEgg->mFielder
            == pShockwave->owner)
        {
            break;
        }
        if (effectType == 3)
        {
            ((PhysicsYoshiEgg*)pObject)->mYoshiEgg->Suspend(true,
                gGameTweaks.m_pGameTweaks->fFreezeShellFrozenTime);
        }
        if (effectType != 2)
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
            fn_80176A60(&pObject->GetPosition());
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

extern "C" void fn_80145C3C(void* data)
{
    unsigned char* object = *(unsigned char**)((unsigned char*)data + 0x10);
    if (*(int*)(object + 0x48) == 0 && object[0x65] == false)
    {
        fn_80176A60(&((PhysicsObject*)object)->GetPosition());
        ((PhysicsObject*)object)->Unknown0();
    }
}

static void UnidentifiedRegisterEventCallback(const char* name,
    void (*callback)(void*))
{
    Function<void*> function(callback);
    unsigned int hash = HashEventName(name, -1);
    UnidentifiedEventBase** foundEvent = 0;
    g_pEventRegistry->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event;
    if (foundEvent != 0)
    {
        event = *foundEvent;
    }
    else
    {
        event = 0;
    }
    ((UnidentifiedTypedEvent<void>*)event)->Add(function, 0, -1);
}

extern "C" void fn_80144AB8()
{
    UnidentifiedRegisterEventCallback("CollisionPatchPowerup", fn_801453DC);
    UnidentifiedRegisterEventCallback("CollisionHammerPowerup", fn_801453B8);
    UnidentifiedRegisterEventCallback("CollisionFireballPowerup", fn_801452F4);
    UnidentifiedRegisterEventCallback("CollisionCrackEgg", fn_801453E0);
    UnidentifiedRegisterEventCallback(
        "CollisionShockwave", (void (*)(void*))fn_801454BC);
    UnidentifiedRegisterEventCallback("CollisionKoopaShellEnd", fn_801453FC);
    UnidentifiedRegisterEventCallback("CollisionBirdoEggEnd", fn_8014545C);
    UnidentifiedRegisterEventCallback("CollisionPatchPatch", fn_80145C3C);
    UnidentifiedRegisterEventCallback("DestroyPowerup", fn_80145300);
    UnidentifiedRegisterEventCallback("DestroyHammer", fn_80145370);
    UnidentifiedRegisterEventCallback("CollisionWaluigiWall", fn_80145318);
}

extern "C" void fn_800721AC(CollisionPlayerWallData*);
extern "C" void fn_8016A658(void*);
extern "C" void fn_8016A670(void*);
extern "C" void fn_8016A688(CollisionPlayerBallData*);
extern "C" void fn_8016A6A0(void*);
extern "C" void fn_8016A6B8(CollisionBallGroundData*);
extern "C" void fn_8016A6D0(CollisionBallWallData*);
extern "C" void fn_8016A6E8(CollisionBallGoalpostData*);
extern "C" void fn_8016A700(CollisionBallShellData*);
void FreeCollisionBallChainData(CollisionBallChainData*);
extern "C" void fn_8016A730(CollisionKoopaShotBallPlayerData*);
extern "C" void fn_8016A748(CollisionKoopaShellGoalieData*);
extern "C" void fn_8016A760(void*);
extern "C" void fn_8016A778(CollisionBirdoShotBallPlayerData*);
extern "C" void fn_8016A790(CollisionBirdoEggGoalieData*);
extern "C" void fn_8016A7A8(void*);
extern "C" void fn_8016A7C0(void*);
extern "C" void fn_8016A7D8(CollisionPowerupWallData*);
extern "C" void fn_8016A7F0(void*);
extern "C" void fn_8016A808(CollisionPlayerBananaData*);
extern "C" void fn_8016A820(CollisionPlayerShellData*);
extern "C" void fn_8016A838(CollisionPlayerFreezeData*);
extern "C" void fn_8016A850(CollisionBulletBillData*);
extern "C" void fn_8016A868(void*);
void FreeCollisionChainPowerupData(CollisionChainPowerupData*);
extern "C" void fn_8016A898(void*);
extern "C" void fn_8016A8B0(UnidentifiedEventData26*);
extern "C" void fn_8016A8C8(CollisionThwompPlayerData*);
extern "C" void fn_8016A8E0(UnidentifiedEventData34*);
extern "C" void fn_8016A8F8(void*);

extern "C" void fn_80145C9C()
{
    lbl_806E11F0->mEvent00.Queue(Function<FnVoidVoid>());
}

extern "C" void fn_80145DD0(CollisionPlayerPlayerData* data)
{
    Function<CollisionPlayerPlayerData*> disposer(
        (void (*)(CollisionPlayerPlayerData*))fn_8016A658);
    lbl_806E11F0->mEvent01.Queue(data, disposer);
}

extern "C" void fn_80145F18(CollisionPlayerWallData* data)
{
    lbl_806E11F0->mEvent02.Queue(data, Function<CollisionPlayerWallData*>(fn_800721AC));
}

extern "C" void fn_80146060(UnidentifiedEventData24* data)
{
    lbl_806E11F0->mEvent27.Queue(
        data, Function<UnidentifiedEventData24*>((void (*)(UnidentifiedEventData24*))fn_8016A670));
}

extern "C" void fn_801461A8()
{
    lbl_806E11F0->mEvent28.Queue(Function<FnVoidVoid>());
}

extern "C" void fn_801462DC(CollisionPlayerBallData* data)
{
    lbl_806E11F0->mEvent03.Queue(
        data, Function<CollisionPlayerBallData*>(fn_8016A688));
}

extern "C" void fn_80146424(UnidentifiedEventData04* data, bool release)
{
    if (!release)
    {
        lbl_806E11F0->mEvent04.Queue(
            data, Function<UnidentifiedEventData04*>((void (*)(UnidentifiedEventData04*))fn_8016A6A0));
    }
    else
    {
        g_BallNetmeshEventDataPool.Free((BallNetmeshEventData*)data);
    }
}

extern "C" void fn_8014658C(CollisionBallGroundData* data)
{
    lbl_806E11F0->mEvent05.Queue(
        data, Function<CollisionBallGroundData*>(fn_8016A6B8));
}

extern "C" void fn_801466D4(CollisionBallWallData* data)
{
    lbl_806E11F0->mEvent06.Queue(
        data, Function<CollisionBallWallData*>(fn_8016A6D0));
}

extern "C" void fn_8014681C(CollisionBallGoalpostData* data)
{
    lbl_806E11F0->mEvent07.Queue(
        data, Function<CollisionBallGoalpostData*>(fn_8016A6E8));
}

extern "C" void fn_80146964(CollisionBallShellData* data)
{
    lbl_806E11F0->mEvent08.Queue(
        data, Function<CollisionBallShellData*>(fn_8016A700));
}

void QueueCollisionBallChain(CollisionBallChainData* data)
{
    lbl_806E11F0->mEvent09.Queue(
        data, Function<CollisionBallChainData*>(FreeCollisionBallChainData));
}

void QueueCollisionKoopaShotBallPlayer(CollisionKoopaShotBallPlayerData* data)
{
    lbl_806E11F0->mEvent10.Queue(
        data, Function<CollisionKoopaShotBallPlayerData*>(fn_8016A730));
}

void QueueCollisionKoopaShellGoalie(CollisionKoopaShellGoalieData* data)
{
    lbl_806E11F0->mEvent11.Queue(
        data, Function<CollisionKoopaShellGoalieData*>(fn_8016A748));
}

void QueueCollisionKoopaShellEnd(CollisionKoopaShellEndData* data)
{
    lbl_806E11F0->mEvent12.Queue(
        (UnidentifiedEventData12*)data, Function<UnidentifiedEventData12*>((void (*)(UnidentifiedEventData12*))fn_8016A760));
}

void QueueCollisionBirdoShotBallPlayer(CollisionBirdoShotBallPlayerData* data)
{
    lbl_806E11F0->mEvent13.Queue(
        data, Function<CollisionBirdoShotBallPlayerData*>(fn_8016A778));
}

void QueueCollisionBirdoEggGoalie(CollisionBirdoEggGoalieData* data)
{
    lbl_806E11F0->mEvent14.Queue(
        data, Function<CollisionBirdoEggGoalieData*>(fn_8016A790));
}

void QueueCollisionBirdoEggEnd(CollisionBirdoEggEndData* data)
{
    lbl_806E11F0->mEvent15.Queue(
        (UnidentifiedEventData15*)data, Function<UnidentifiedEventData15*>((void (*)(UnidentifiedEventData15*))fn_8016A7A8));
}

extern "C" void fn_801473A4(UnidentifiedEventData17* data)
{
    lbl_806E11F0->mEvent17.Queue(
        data, Function<UnidentifiedEventData17*>((void (*)(UnidentifiedEventData17*))fn_8016A7C0));
}

extern "C" void fn_801474EC(UnidentifiedEventData17* data)
{
    lbl_806E11F0->mEvent18.Queue(
        data, Function<UnidentifiedEventData17*>((void (*)(UnidentifiedEventData17*))fn_8016A7C0));
}

extern "C" void fn_80147634(CollisionPowerupWallData* data)
{
    lbl_806E11F0->mEvent19.Queue(
        data, Function<CollisionPowerupWallData*>(fn_8016A7D8));
}

extern "C" void fn_8014777C(UnidentifiedEventData19* data)
{
    lbl_806E11F0->mEvent20.Queue(
        data, Function<UnidentifiedEventData19*>((void (*)(UnidentifiedEventData19*))fn_8016A7F0));
}

extern "C" void fn_801478C4(CollisionPlayerBananaData* data)
{
    lbl_806E11F0->mEvent21.Queue(
        data, Function<CollisionPlayerBananaData*>(fn_8016A808));
}

extern "C" void fn_80147A0C(CollisionPlayerShellData* data)
{
    lbl_806E11F0->mEvent22.Queue(
        data, Function<CollisionPlayerShellData*>(fn_8016A820));
}

extern "C" void fn_80147B54(CollisionPlayerFreezeData* data)
{
    lbl_806E11F0->mEvent23.Queue(
        data, Function<CollisionPlayerFreezeData*>(fn_8016A838));
}

extern "C" void fn_80147C9C(CollisionBulletBillData* data)
{
    lbl_806E11F0->mEvent24.Queue(
        data, Function<CollisionBulletBillData*>(fn_8016A850));
}

extern "C" void fn_80147DE4(CollisionBulletBillData* data)
{
    lbl_806E11F0->mEvent25.Queue(
        data, Function<CollisionBulletBillData*>(fn_8016A850));
}

extern "C" void fn_80147F2C(CollisionBulletBillData* data)
{
    lbl_806E11F0->mEvent26.Queue(
        data, Function<CollisionBulletBillData*>(fn_8016A850));
}

extern "C" void fn_80148074(UnidentifiedEventData25* data)
{
    lbl_806E11F0->mEvent29.Queue(
        data, Function<UnidentifiedEventData25*>((void (*)(UnidentifiedEventData25*))fn_8016A868));
}

void QueueCollisionChainCrowd(UnidentifiedEventData28* data)
{
    lbl_806E11F0->mEvent35.Queue(data, Function<UnidentifiedEventData28*>());
}

void QueueCollisionChainPowerup(CollisionChainPowerupData* data)
{
    lbl_806E11F0->mEvent36.Queue(
        data, Function<CollisionChainPowerupData*>(FreeCollisionChainPowerupData));
}

void QueueCollisionPatchPlayer(UnidentifiedEventData24* data)
{
    lbl_806E11F0->mEvent37.Queue(
        data, Function<UnidentifiedEventData24*>((void (*)(UnidentifiedEventData24*))fn_8016A670));
}

extern "C" void fn_80148588(UnidentifiedEventData24* data)
{
    lbl_806E11F0->mEvent38.Queue(
        data, Function<UnidentifiedEventData24*>((void (*)(UnidentifiedEventData24*))fn_8016A670));
}

extern "C" void fn_801486D0(UnidentifiedEventData30* data)
{
    lbl_806E11F0->mEvent39.Queue(
        data, Function<UnidentifiedEventData30*>((void (*)(UnidentifiedEventData30*))fn_8016A898));
}

extern "C" void fn_80148818(UnidentifiedEventData28* data)
{
    lbl_806E11F0->mEvent40.Queue(data, Function<UnidentifiedEventData28*>());
}

extern "C" void fn_80148954(UnidentifiedEventData24* data)
{
    lbl_806E11F0->mEvent41.Queue(
        data, Function<UnidentifiedEventData24*>((void (*)(UnidentifiedEventData24*))fn_8016A670));
}

extern "C" void fn_80148A9C(UnidentifiedEventData31* data)
{
    lbl_806E11F0->mEvent42.Queue(data, Function<UnidentifiedEventData31*>());
}

extern "C" void fn_80148BD8(UnidentifiedEventData31* data)
{
    lbl_806E11F0->mEvent43.Queue(data, Function<UnidentifiedEventData31*>());
}

void QueueCollisionHammerPlayer(UnidentifiedEventData26* data)
{
    lbl_806E11F0->mEvent44.Queue(
        data, Function<UnidentifiedEventData26*>(fn_8016A8B0));
}

void QueueCollisionHammerGround(UnidentifiedEventData26* data)
{
    lbl_806E11F0->mEvent46.Queue(
        data, Function<UnidentifiedEventData26*>(fn_8016A8B0));
}

void QueueCollisionHammerPowerup(UnidentifiedEventData27* data)
{
    lbl_806E11F0->mEvent47.Queue(data, Function<UnidentifiedEventData27*>());
}

void QueueBirdoEggDestroyPowerup(UnidentifiedEventData27* data)
{
    lbl_806E11F0->mEvent57.Queue(data, Function<UnidentifiedEventData27*>());
}

void QueueKoopaShellDestroyPowerup(UnidentifiedEventData27* data)
{
    lbl_806E11F0->mEvent57.Queue(data, Function<UnidentifiedEventData27*>());
}

void QueueBirdoEggDestroyHammer(UnidentifiedEventData35* data)
{
    lbl_806E11F0->mEvent58.Queue(data, Function<UnidentifiedEventData35*>());
}

void QueueKoopaShellDestroyHammer(UnidentifiedEventData35* data)
{
    lbl_806E11F0->mEvent58.Queue(data, Function<UnidentifiedEventData35*>());
}

void QueueBirdoEggKnockYoshiTongue(cFielder* data)
{
    lbl_806E11F0->mEvent59.Queue(data, Function<cFielder*>());
}

void QueueKoopaShellKnockYoshiTongue(cFielder* data)
{
    lbl_806E11F0->mEvent59.Queue(data, Function<cFielder*>());
}

void QueueCollisionHammerChain(UnidentifiedEventData28* data)
{
    lbl_806E11F0->mEvent48.Queue(data, Function<UnidentifiedEventData28*>());
}

extern "C" void fn_80149984(void* source, cCharacter* target)
{
    CollisionThwompPlayerData* data = 0;
    g_CollisionThwompPlayerDataPool.Allocate(data);
    data->source = source;
    data->sourceValue = *(void**)source;
    data->target = target;
    lbl_806E11F0->mEvent49.Queue(
        data,
        Function<CollisionThwompPlayerData*>(fn_8016A8C8));
}

extern "C" void fn_80149B30(UnidentifiedEventData33* data)
{
    lbl_806E11F0->mEvent50.Queue(data, Function<UnidentifiedEventData33*>());
}

void QueueCollisionEggBall(UnidentifiedEventData34* data)
{
    lbl_806E11F0->mEvent52.Queue(
        data, Function<UnidentifiedEventData34*>(fn_8016A8E0));
}

void QueueCollisionEggPlayer(UnidentifiedEventData34* data)
{
    lbl_806E11F0->mEvent53.Queue(
        data, Function<UnidentifiedEventData34*>(fn_8016A8E0));
}

void QueueCollisionCrackEgg(UnidentifiedEventData34* data)
{
    lbl_806E11F0->mEvent56.Queue(
        data, Function<UnidentifiedEventData34*>(fn_8016A8E0));
}

extern "C" void fn_8014A044(UnidentifiedNPC_801B43F8* data)
{
    lbl_806E11F0->mEvent60.Queue(data, Function<UnidentifiedNPC_801B43F8*>());
}

extern "C" void fn_8014A180(cFielder* data)
{
    lbl_806E11F0->mEvent61.Queue(data, Function<cFielder*>());
}

extern "C" void fn_8014A2BC(UnidentifiedEventData38* data)
{
    lbl_806E11F0->mEvent62.Queue(
        data, Function<UnidentifiedEventData38*>((void (*)(UnidentifiedEventData38*))fn_8016A8F8));
}

EventDispatcher::EventDispatcher(const char*)
    : EventDispatcherBase()
{
}

struct UnidentifiedPooledData08
{
    unsigned char data[0x08];
};

struct UnidentifiedPooledData0C
{
    unsigned char data[0x0C];
};

SlotPool<UnidentifiedEventData26> lbl_80570110(16, 16);
SlotPool<UnidentifiedEventData24> lbl_80570138(16, 16);
static SlotPool<UnidentifiedPooledData08> lbl_80570160(16, 16);
SlotPool<UnidentifiedEventData34> lbl_80570188(16, 16);
static SlotPool<UnidentifiedPooledData0C> lbl_805701B0(16, 16);

PhysicsEventQueue* lbl_806E11F0;

extern "C" void fn_80143FD4()
{
    lbl_80570110.FreeBlocks();
    lbl_80570138.FreeBlocks();
    lbl_80570160.FreeBlocks();
    lbl_80570188.FreeBlocks();
    lbl_805701B0.FreeBlocks();
}

extern "C" void fn_80144070()
{
    if (lbl_806E11F0 == 0)
    {
        lbl_806E11F0 = new (nlMalloc(sizeof(PhysicsEventQueue), 8, false))
            PhysicsEventQueue;
        fn_8017617C();
    }
}

extern "C" void fn_801440BC()
{
    if (lbl_806E11F0 != 0)
    {
        EventDispatcher& dispatcher = lbl_806E11F0->mDispatcher;
        dispatcher.Clear();

        BasicSlotPool<DLListEntry<EventCallback> >* pool =
            &dispatcher.callbacks.m_Allocator;
        fn_802B467C(pool);
        SlotPoolBase::BaseFreeBlocks(
            pool, sizeof(DLListEntry<EventCallback>));

        delete lbl_806E11F0;
        lbl_806E11F0 = 0;
        fn_801761E0();
    }
}

extern "C" void fn_80144130(EventDispatcher* dispatcher)
{
    dispatcher->Dispatch(true);
}

extern "C" unsigned char fn_8016A650(void* object)
{
    return ((unsigned char*)object)[0x24];
}

extern "C" void fn_8016A658(void* data)
{
    g_CollisionPlayerPlayerDataPool.Free((CollisionPlayerPlayerData*)data);
}

extern "C" void fn_8016A670(void* data)
{
    lbl_80570138.Free((UnidentifiedEventData24*)data);
}

extern "C" void fn_8016A688(CollisionPlayerBallData* data)
{
    g_CollisionPlayerBallDataPool.Free(data);
}

extern "C" void fn_8016A6A0(void* data)
{
    g_BallNetmeshEventDataPool.Free((BallNetmeshEventData*)data);
}

extern "C" void fn_8016A6B8(CollisionBallGroundData* data)
{
    g_CollisionBallGroundDataPool.Free(data);
}

extern "C" void fn_8016A6D0(CollisionBallWallData* data)
{
    g_CollisionBallWallDataPool.Free(data);
}

extern "C" void fn_8016A6E8(CollisionBallGoalpostData* data)
{
    g_CollisionBallGoalpostDataPool.Free(data);
}

extern "C" void fn_8016A700(CollisionBallShellData* data)
{
    g_CollisionBallShellDataPool.Free(data);
}

void FreeCollisionBallChainData(CollisionBallChainData* data)
{
    g_CollisionBallChainDataPool.Free(data);
}

extern "C" void fn_8016A730(CollisionKoopaShotBallPlayerData* data)
{
    g_CollisionKoopaShotBallPlayerDataPool.Free(data);
}

extern "C" void fn_8016A748(CollisionKoopaShellGoalieData* data)
{
    g_CollisionKoopaShellGoalieDataPool.Free(data);
}

extern "C" void fn_8016A760(void* data)
{
    g_CollisionKoopaShellEndDataPool.Free((CollisionKoopaShellEndData*)data);
}

extern "C" void fn_8016A778(CollisionBirdoShotBallPlayerData* data)
{
    g_CollisionBirdoShotBallPlayerDataPool.Free(data);
}

extern "C" void fn_8016A790(CollisionBirdoEggGoalieData* data)
{
    g_CollisionBirdoEggGoalieDataPool.Free(data);
}

extern "C" void fn_8016A7A8(void* data)
{
    g_CollisionBirdoEggEndDataPool.Free((CollisionBirdoEggEndData*)data);
}

extern "C" void fn_8016A7C0(void* data)
{
    g_CollisionPowerupGroundDataPool.Free((CollisionPowerupGroundData*)data);
}

extern "C" void fn_8016A7D8(CollisionPowerupWallData* data)
{
    g_CollisionPowerupWallDataPool.Free(data);
}

extern "C" void fn_8016A7F0(void* data)
{
    g_PowerupHitPlayerEventDataPool.Free((PowerupHitPlayerEventData*)data);
}

extern "C" void fn_8016A808(CollisionPlayerBananaData* data)
{
    g_CollisionPlayerBananaDataPool.Free(data);
}

extern "C" void fn_8016A820(CollisionPlayerShellData* data)
{
    g_CollisionPlayerShellDataPool.Free(data);
}

extern "C" void fn_8016A838(CollisionPlayerFreezeData* data)
{
    g_CollisionPlayerFreezeDataPool.Free(data);
}

extern "C" void fn_8016A850(CollisionBulletBillData* data)
{
    g_CollisionBulletBillDataPool.Free(data);
}

extern "C" void fn_8016A868(void* data)
{
    g_PowerupUsedEventDataPool.Free((PowerupUsedEventData*)data);
}

void FreeCollisionChainPowerupData(CollisionChainPowerupData* data)
{
    g_CollisionChainPowerupDataPool.Free(data);
}

extern "C" void fn_8016A898(void* data)
{
    lbl_80570160.Free((UnidentifiedPooledData08*)data);
}

extern "C" void fn_8016A8B0(UnidentifiedEventData26* data)
{
    lbl_80570110.Free(data);
}

extern "C" void fn_8016A8C8(CollisionThwompPlayerData* data)
{
    g_CollisionThwompPlayerDataPool.Free(data);
}

extern "C" void fn_8016A8E0(UnidentifiedEventData34* data)
{
    lbl_80570188.Free(data);
}

extern "C" void fn_8016A8F8(void* data)
{
    lbl_805701B0.Free((UnidentifiedPooledData0C*)data);
}

#include "NL/nlBind_impl.h"
