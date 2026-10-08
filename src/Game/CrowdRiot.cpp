#include "NL/nlDLListContainer.inl"
#include "Game/Sys/audio.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/CrowdRiot.h"
#include "Game/Goalie.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/DebugWriteCache.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/EventRegistry.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/MathHelpers.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsTriggerVolume.h"
#include "NL/nlBind.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"

#include <math.h>
#include "NL/nlFunction.inl"

struct CrowdRiotGenerator
{
    void RegisterDebugFields(u16* type, DebugWriteCache* cache);
    /* 0x00 */ nlVector2 v2Location;
    /* 0x08 */ bool bIsOn;
    /* 0x0C */ float fTimeToExplode;
}; // total size: 0x10

float gCrowdRiotGeneratorGoalLineOffset;
float gCrowdRiotGeneratorSidelineOffset;

void QueueCrowdRiotCollision(PhysicsObject*, PhysicsObject*, const nlVector3&, void*);
void OnCrowdRiotGoalScored(void*);
void OnCrowdRiotMegastrikeEnd(void*);
void OnCrowdRiotGameOver(void*);
void OnCrowdRiotCollision(void*);

static float sRiotCollisionRadius = 2.45f;
static float sRiotExitSpeedMultiplier = 4.0f;
static float sRiotSpeed = 2.0f;
static float sRiotResumeStateTime = 1.0f;
static float sRiotPathX = 10.4f;
static float sRiotSidelineOffset = 3.0f;
static unsigned short sCrowdRiotType = 0xFFFF;
static unsigned short sGeneratorsType = 0xFFFF;

CrowdRiotGenerator gCrowdRiotGenerators[6];

CrowdRiot::CrowdRiot(bool enableRiot)
    : mpTriggerVolume(0)
{
    ResetGenerators();
    if (enableRiot)
    {
        Reset(false);
    }
    else
    {
        mfRiotTime = -1.0f;
        meState = STATE_DISABLED;
        mfStateTime = -1.0f;
        maDesiredFacingDirection = 0;
        mv3Position.x = 0.0f;
        mv3Position.y = 0.0f;
        mv3Position.z = -10.0f;
        mv3Velocity.x = 0.0f;
        mv3Velocity.y = 0.0f;
        mv3Velocity.z = 0.0f;
        mv3Target.x = 0.0f;
        mv3Target.y = 0.0f;
        mv3Target.z = 0.0f;
    }

    UnidentifiedFindEvent<void>("CollisionCrowd", -1)->Add(Function<void*>(OnCrowdRiotCollision), 0, -1);
    UnidentifiedFindEvent<void>("GoalScored", -1)->Add(Function<void*>(OnCrowdRiotGoalScored), 0, -1);
    UnidentifiedFindEvent<void>("MegastrikeEnd", -1)->Add(Function<void*>(OnCrowdRiotMegastrikeEnd), 0, -1);
    UnidentifiedFindEvent<void>("GameOver", -1)->Add(Function<void*>(OnCrowdRiotGameOver), 0, -1);
}

CrowdRiot::~CrowdRiot()
{
    if (mpTriggerVolume != 0)
    {
        delete mpTriggerVolume;
        mpTriggerVolume = 0;
    }
}

inline void CrowdRiotGenerator::RegisterDebugFields(u16* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("Generators");
    cache->AddField(21, gDebugFieldTypes[21].size, 0, "v2Location");
    cache->AddField(16, gDebugFieldTypes[16].size, (u8*)&bIsOn - (u8*)this, "bIsOn");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&fTimeToExplode - (u8*)this, "fTimeToExplode");
    cache->EndType();
}

inline void CrowdRiot::RegisterDebugFields(u16* type, DebugWriteCache* cache)
{
    *type = cache->BeginType("CrowdRiot");
    cache->AddField(17, gDebugFieldTypes[17].size, 0, "mfStateTime");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&mfRiotTime - (u8*)this, "mfRiotTime");
    cache->AddField(22, gDebugFieldTypes[22].size, (u8*)&mv3Target - (u8*)this, "mv3Target");
    cache->AddField(22, gDebugFieldTypes[22].size, (u8*)&mv3Position - (u8*)this, "mv3Position");
    cache->AddField(22, gDebugFieldTypes[22].size, (u8*)&mv3Velocity - (u8*)this, "mv3Velocity");
    cache->AddField(19, gDebugFieldTypes[19].size, (u8*)&maDesiredFacingDirection - (u8*)this, "maDesiredFacingDirection");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&meState - (u8*)this, "meState");
    cache->EndType();
}

void CrowdRiot::SyncLog(void* context, DebugWriteCache* cache)
{
    if (sCrowdRiotType == 0xFFFF)
    {
        RegisterDebugFields(&sCrowdRiotType, cache);
    }

    cache->ChecksumData(sCrowdRiotType, this, context);
    cache->WriteData(sCrowdRiotType, this, sizeof(CrowdRiot));

    for (int i = 0; i < 6; i++)
    {
        CrowdRiotGenerator* generator = &gCrowdRiotGenerators[i];
        if (sGeneratorsType == 0xFFFF)
        {
            generator->RegisterDebugFields(&sGeneratorsType, cache);
        }

        cache->ChecksumData(sGeneratorsType, generator, context);
        cache->WriteData(sGeneratorsType, generator, sizeof(CrowdRiotGenerator));
    }
}

void CrowdRiot::ResetGenerators()
{
    for (int i = 0; i < 6; i++)
    {
        CrowdRiotGenerator* generator = &gCrowdRiotGenerators[i];
        generator->bIsOn = true;
        generator->fTimeToExplode = -1.0f;

        float goalLineX = gCrowdRiotGeneratorGoalLineOffset + cField::GetGoalLineX(1U);
        float sidelineY = gCrowdRiotGeneratorSidelineOffset + cField::GetSidelineY(1U);

        if (i == 0 || i == 3)
        {
            generator->v2Location.x = goalLineX;
        }
        else if (i == 2 || i == 5)
        {
            generator->v2Location.x = -goalLineX;
        }
        else
        {
            generator->v2Location.x = 0.0f;
        }

        if (i < 3)
        {
            generator->v2Location.y = sidelineY;
        }
        else
        {
            generator->v2Location.y = -sidelineY;
        }
    }
}

void InterpolateRiotBallPosition(nlVector3& result, const nlVector3& riotPosition,
    const nlVector3& ballPosition, float time)
{
    nlVecLerp(result, riotPosition, ballPosition, time);
}

void CrowdRiot::InitializeRiotMotion()
{
    for (int i = 0; i < 6; i++)
    {
        CrowdRiotGenerator* generator = &gCrowdRiotGenerators[i];
        if (!generator->bIsOn)
        {
            nlVector3 position;
            position.x = sRiotPathX
                       * AIsgn(generator->v2Location.x);
            position.y = sRiotSidelineOffset
                       + fabsf(generator->v2Location.y);
            position.y *= AIsgn(generator->v2Location.y);
            position.z = 0.0f;
            mv3Position = position;

            nlVector3 velocity;
            velocity.x = 0.0f;
            velocity.y = -sRiotSpeed;
            velocity.y *= AIsgn(generator->v2Location.y);
            velocity.z = 0.0f;
            mv3Velocity = velocity;
            maDesiredFacingDirection = nlVector3ToAngle(velocity);
        }
    }
}

enum CrowdRiotEffect
{
    GENERATOR_BROKEN,
    GENERATOR_EXPLOSION,
    CROWD_RIOT,
    CROWD_RIOT_WITH_FADE
};

EffectsGroup* GetCrowdRiotEffectGroup(CrowdRiotEffect effect)
{
    switch (effect)
    {
    case CROWD_RIOT_WITH_FADE:
        return EmissionManager::Instance()->GetEffectsGroup("crowd_riot_with_fade");
    case CROWD_RIOT:
        return EmissionManager::Instance()->GetEffectsGroup("crowd_riot");
    case GENERATOR_EXPLOSION:
        return EmissionManager::Instance()->GetEffectsGroup("generator_explode");
    case GENERATOR_BROKEN:
        return EmissionManager::Instance()->GetEffectsGroup("generator_broken");
    }
    return 0;
}

void PlayCrowdRiotSound(CrowdRiot* crowdRiot)
{
    PlaySound(13, 0x198B7ED3, "CrowdRiot", crowdRiot);
}

void CrowdRiot::Reset(bool preserveActiveRiot)
{
    if (meState == STATE_DISABLED)
    {
        return;
    }

    EffectsGroup* group;
    EmissionController* controller;
    bool resumeRiot = false;
    if (mfRiotTime > 0.0f && preserveActiveRiot)
    {
        resumeRiot = true;
    }
    else
    {
        mfRiotTime = -1.0f;
        ResetGenerators();
    }

    if (mpTriggerVolume != 0)
    {
        delete mpTriggerVolume;
        mpTriggerVolume = 0;
    }

    meState = STATE_READY;
    mfStateTime = -1.0f;
    maDesiredFacingDirection = 0;
    mv3Position.x = 0.0f;
    mv3Position.y = 0.0f;
    mv3Position.z = -10.0f;
    mv3Velocity.x = 0.0f;
    mv3Velocity.y = 0.0f;
    mv3Velocity.z = 0.0f;
    mv3Target.x = 0.0f;
    mv3Target.y = 0.0f;
    mv3Target.z = 0.0f;

    group = GetCrowdRiotEffectGroup(GENERATOR_BROKEN);
    EmissionManager::Instance()->Kill((unsigned long)this, group);
    group = GetCrowdRiotEffectGroup(GENERATOR_EXPLOSION);
    EmissionManager::Instance()->Kill((unsigned long)this, group);
    group = GetCrowdRiotEffectGroup(CROWD_RIOT);
    EmissionManager::Instance()->Kill((unsigned long)this, group);
    group = GetCrowdRiotEffectGroup(CROWD_RIOT_WITH_FADE);
    EmissionManager::Instance()->Kill((unsigned long)this, group);
    StopSound(0x198B7ED3, this);

    if (resumeRiot && meState != STATE_DISABLED && meState == STATE_READY)
    {
        InitializeRiotMotion();
        meState = STATE_ACTIVE;
        mfStateTime = sRiotResumeStateTime;

        if (mpTriggerVolume == 0)
        {
            PhysicsTriggerVolume* physicsObject
                = new (8, false) PhysicsTriggerVolume(
                    sRiotCollisionRadius);
            mpTriggerVolume = physicsObject;
            physicsObject->m_pTriggerCallbackFunc
                = QueueCrowdRiotCollision;
            physicsObject->m_pCallbackParam = this;
            mpTriggerVolume->SetPosition(
                mv3Position, PhysicsObject::WORLD_COORDINATES);
            mpTriggerVolume->EnableCollisions();
        }

        group = GetCrowdRiotEffectGroup(CROWD_RIOT_WITH_FADE);
        controller = EmissionManager::Instance()->Create(group, 3, true, 0);
        controller->SetPosition(mv3Position);
        controller->SetVelocity(mv3Velocity);
        {
            Function<EmissionController&> callback(
                BindExp2<void,
                    Detail::MemFunImpl<void,
                        void (CrowdRiot::*)(EmissionController&)>,
                    CrowdRiot*,
                    Placeholder<0> >(
                    Detail::MemFunImpl<void,
                        void (CrowdRiot::*)(EmissionController&)>(
                        &CrowdRiot::UpdateEmissionPosition),
                    this,
                    placeholder0));
            controller->SetUpdateCallback(callback);
        }
        controller->m_uUserData = (u32)this;
        PlayCrowdRiotSound(this);
    }
}

static inline void StartCrowdRiotExit()
{
    CrowdRiot* crowdRiot
        = (CrowdRiot*)g_pGame->mpCrowdRiot;
    if (crowdRiot->meState == CrowdRiot::STATE_ACTIVE)
    {
        nlVector3 velocity;
        velocity.x = sRiotExitSpeedMultiplier * crowdRiot->mv3Velocity.x;
        velocity.y = sRiotExitSpeedMultiplier * crowdRiot->mv3Velocity.y;
        velocity.z = 0.0f;
        crowdRiot->mv3Velocity = velocity;

        nlVector3 target;
        target.x = sRiotPathX
                 * AIsgn(crowdRiot->mv3Position.x);
        float sideline = sRiotSidelineOffset
                       + cField::GetSidelineY(1U);
        float sign = AIsgn(crowdRiot->mv3Position.y);
        target.y = sign * sideline;
        target.y = -target.y;
        target.z = 0.0f;
        crowdRiot->mv3Target = target;
        crowdRiot->meState = CrowdRiot::STATE_EXITING;
        crowdRiot->mfStateTime = -1.0f;
    }
}

void HandleCrowdRiotBallCollision(cBall* ball, CrowdRiot* crowdRiot)
{
    if (ball->mbStuckInRiotDone)
    {
        nlVector3 velocity;
        MakeRandomDirection2D(velocity, 10.0f);
        velocity.z = 10.0f + nlRandomf(5.0f);
        ball->SetVelocity(velocity, SPINTYPE_NONE, 0);
        ball->mtStuckInRiotTimer.UnidentifiedClear();
        ball->mbStuckInRiotDone = false;
    }
    else if (ball->mtStuckInRiotTimer.m_uPackedTime == 0)
    {
        if (ball->m_tNoPickupTimer.m_uPackedTime == 0)
        {
            ball->mtStuckInRiotTimer.SetSeconds(1.0f);
            ball->mbStuckInRiotDone = false;
            ball->m_tNoPickupTimer.SetSeconds(1.25f);
        }
    }
    else
    {
        float time = ball->mtStuckInRiotTimer.GetSeconds();
        nlVector3 position;
        InterpolateRiotBallPosition(position, crowdRiot->mv3Position, ball->m_v3Position, time);
        position.z = 0.18f;
        ball->SetPosition(position);
    }
}

void OnCrowdRiotGoalScored(void*)
{
    StartCrowdRiotExit();
}

void OnCrowdRiotMegastrikeEnd(void*)
{
    StartCrowdRiotExit();
}

void OnCrowdRiotGameOver(void*)
{
    StartCrowdRiotExit();
}

void OnCrowdRiotCollision(void* param)
{
    CollisionCrowdData* event
        = (CollisionCrowdData*)param;
    CrowdRiot* crowdRiot = event->pCrowdRiot;
    PhysicsObject* object = event->pObject;

    switch (object->GetObjectType())
    {
    case 4:
    {
        PhysicsObject* parent = object->m_parentObject;
        cFielder* fielder
            = (cFielder*)((PhysicsCharacter*)parent)->m_pAICharacter;
        if (fielder->m_eClassType == FIELDER
            && fielder->m_eActionState != ACTION_SHOOT_TO_SCORE
            && fielder->m_eActionState != ACTION_SHOT)
        {
            fielder->fn_80043ADC();
        }
        break;
    }
    case 16:
    {
        cBall* ball = ((PhysicsAIBall*)object)->m_pAIBall;
        cPlayer* player = ball->m_pOwner;
        if (player != 0)
        {
            if (player->m_eClassType == FIELDER)
            {
                ((cFielder*)player)->fn_80043ADC();
            }
            else
            {
                static_cast<Goalie*>(player)->FumbleBall();
            }
        }
        else
        {
            HandleCrowdRiotBallCollision(ball, crowdRiot);
        }
        break;
    }
    case 20:
        break;
    case 21:
        ((PhysicsBanana*)object)->m_pPowerupObject->m_bShouldDestroy = true;
        break;
    case 24:
    case 28:
        break;
    }
}

void QueueCrowdRiotCollision(PhysicsObject*, PhysicsObject* other,
    const nlVector3& position, void* context)
{
    switch (other->GetObjectType())
    {
    case 4:
    case 16:
    case 20:
    case 21:
    case 24:
    case 28:
    {
        CrowdRiot* crowdRiot = (CrowdRiot*)context;
        if (crowdRiot->meState != CrowdRiot::STATE_READY)
        {
            CollisionCrowdData* event = 0;
            g_CollisionCrowdDataPool.Allocate(event);
            event->pObject = other;
            event->v3Position = position;
            event->pCrowdRiot = crowdRiot;
            QueueCollisionCrowdEvent(g_pGame, event);
        }
        break;
    }
    }
}

void CrowdRiot::UpdateEmissionPosition(EmissionController& controller)
{
    controller.SetPosition(mv3Position);
}
