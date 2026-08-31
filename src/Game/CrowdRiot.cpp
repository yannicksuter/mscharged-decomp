#include "Game/CrowdRiot.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Event.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsSphere.h"
#include "NL/nlAVLTree.h"
#include "NL/nlBind.h"
#include "NL/nlMemory.h"
#include "NL/nlSlotPool.h"

#include <math.h>

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

class Goalie;

class PhysicsSphere_80174F04 : public PhysicsSphere
{
public:
    PhysicsSphere_80174F04(float radius);
    virtual int GetObjectType() const;

    /* 0x38 */ void (*mUnidentified38)(PhysicsObject*, PhysicsObject*,
        nlVector3&, void*);
    /* 0x3C */ void* mUnidentified3C;
}; // total size: 0x40

extern "C" DebugFieldType lbl_80533C98[];
extern "C" UnidentifiedEventRegistry* lbl_806E1D90;
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void* fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);
extern "C" float lbl_806E0C40;
extern "C" float lbl_806E0C44;
extern "C" EffectsGroup* fn_802E7CDC(
    EmissionManager*, const char*);
extern "C" EmissionController* fn_802E7FE4(
    EmissionManager*, EffectsGroup*, int, bool, bool);
extern "C" void fn_800EC12C(unsigned long, void*);
extern "C" void fn_800EBBFC(
    int, unsigned long, const char*, void*);
extern "C" void fn_8007EB90(Goalie*);
extern "C" void fn_8005E29C(cGame*, void*);
extern "C" SlotPool<UnidentifiedEventData_80066B08> lbl_80571438;

extern "C" void fn_800298D8(void*);
extern "C" void fn_800299C4(void*);
extern "C" void fn_80029AB0(void*);
extern "C" void fn_80029B9C(void*);
extern "C" void fn_800297B8(cBall*, CrowdRiot*);
extern "C" void fn_80029C80(
    PhysicsObject*, PhysicsObject*, nlVector3&, void*);

static float sUnidentifiedFloat0 = 2.45f;
static float sUnidentifiedFloat1 = 4.0f;
static float sUnidentifiedFloat2 = 2.0f;
static float sUnidentifiedFloat3 = 1.0f;
static float sUnidentifiedFloat4 = 10.4f;
static float sUnidentifiedFloat5 = 3.0f;
static unsigned short sCrowdRiotType = 0xFFFF;
static unsigned short sGeneratorsType = 0xFFFF;

extern "C" Generators lbl_8056B890[6];
Generators lbl_8056B890[6];

static inline void UnidentifiedRegisterEventCallback(
    const char* name, void (*callback)(void*))
{
    Function<void*> function(callback);
    unsigned int hash = fn_802B289C(name, -1);
    UnidentifiedEventBase** foundEvent = 0;
    lbl_806E1D90->Find(hash, &foundEvent, 0);
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

CrowdRiot::CrowdRiot(bool param1)
    : mUnidentified30(0)
{
    fn_8002921C();
    if (param1)
    {
        fn_80029460(false);
    }
    else
    {
        mfRiotTime = -1.0f;
        meState = 0;
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

    UnidentifiedRegisterEventCallback("CollisionCrowd", fn_80029B9C);
    UnidentifiedRegisterEventCallback("GoalScored", fn_800298D8);
    UnidentifiedRegisterEventCallback("MegastrikeEnd", fn_800299C4);
    UnidentifiedRegisterEventCallback("GameOver", fn_80029AB0);
}

CrowdRiot::~CrowdRiot()
{
    if (mUnidentified30 != 0)
    {
        delete mUnidentified30;
        mUnidentified30 = 0;
    }
}

void CrowdRiot::SyncLog(void* context, DebugWriteCache* cache)
{
    if (sCrowdRiotType == 0xFFFF)
    {
        sCrowdRiotType = fn_80338EBC(cache, "CrowdRiot");
        fn_80338F88(cache, 17, lbl_80533C98[17].size, 0, "mfStateTime");
        fn_80338F88(cache, 17, lbl_80533C98[17].size, (u8*)&mfRiotTime - (u8*)this, "mfRiotTime");
        fn_80338F88(cache, 22, lbl_80533C98[22].size, (u8*)&mv3Target - (u8*)this, "mv3Target");
        fn_80338F88(cache, 22, lbl_80533C98[22].size, (u8*)&mv3Position - (u8*)this, "mv3Position");
        fn_80338F88(cache, 22, lbl_80533C98[22].size, (u8*)&mv3Velocity - (u8*)this, "mv3Velocity");
        fn_80338F88(cache, 19, lbl_80533C98[19].size, (u8*)&maDesiredFacingDirection - (u8*)this, "maDesiredFacingDirection");
        fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&meState - (u8*)this, "meState");
        fn_80338F78(cache);
    }

    fn_80339450(cache, sCrowdRiotType, this, context);
    fn_8033930C(cache, sCrowdRiotType, this, sizeof(CrowdRiot));

    for (int i = 0; i < 6; i++)
    {
        Generators* generator = &lbl_8056B890[i];
        if (sGeneratorsType == 0xFFFF)
        {
            sGeneratorsType = fn_80338EBC(cache, "Generators");
            fn_80338F88(cache, 21, lbl_80533C98[21].size, 0, "v2Location");
            fn_80338F88(cache, 16, lbl_80533C98[16].size, (u8*)&generator->bIsOn - (u8*)generator, "bIsOn");
            fn_80338F88(cache, 17, lbl_80533C98[17].size, (u8*)&generator->fTimeToExplode - (u8*)generator, "fTimeToExplode");
            fn_80338F78(cache);
        }

        fn_80339450(cache, sGeneratorsType, generator, context);
        fn_8033930C(cache, sGeneratorsType, generator, sizeof(Generators));
    }
}

void CrowdRiot::fn_8002921C()
{
    for (int i = 0; i < 6; i++)
    {
        Generators* generator = &lbl_8056B890[i];
        generator->bIsOn = true;
        generator->fTimeToExplode = -1.0f;

        float goalLineX = lbl_806E0C40 + cField::GetGoalLineX(1U);
        float sidelineY = lbl_806E0C44 + cField::GetSidelineY(1U);

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

void CrowdRiot::fn_80029320()
{
    for (int i = 0; i < 6; i++)
    {
        Generators* generator = &lbl_8056B890[i];
        if (!generator->bIsOn)
        {
            nlVector3 position;
            position.x = sUnidentifiedFloat4
                       * AIsgn(generator->v2Location.x);
            position.y = sUnidentifiedFloat5
                       + (float)fabs(generator->v2Location.y);
            position.y *= AIsgn(generator->v2Location.y);
            position.z = 0.0f;
            mv3Position = position;

            nlVector3 velocity;
            velocity.x = 0.0f;
            velocity.y = -sUnidentifiedFloat2;
            velocity.y *= AIsgn(generator->v2Location.y);
            velocity.z = 0.0f;
            mv3Velocity = velocity;
            maDesiredFacingDirection = (u16)(s32)(10430.378f
                                                  * nlATan2f(velocity.y, velocity.x));
        }
    }
}

void CrowdRiot::fn_80029460(bool param1)
{
    if (meState == 0)
    {
        return;
    }

    bool resumeRiot = false;
    if (mfRiotTime > 0.0f && param1)
    {
        resumeRiot = true;
    }
    else
    {
        mfRiotTime = -1.0f;
        fn_8002921C();
    }

    if (mUnidentified30 != 0)
    {
        delete mUnidentified30;
        mUnidentified30 = 0;
    }

    meState = 1;
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

    EffectsGroup* group = fn_802E7CDC(
        EmissionManager::Instance(), "generator_broken");
    EmissionManager::Instance()->Kill((unsigned long)this, group);
    group = fn_802E7CDC(
        EmissionManager::Instance(), "generator_explode");
    EmissionManager::Instance()->Kill((unsigned long)this, group);
    group = fn_802E7CDC(
        EmissionManager::Instance(), "crowd_riot");
    EmissionManager::Instance()->Kill((unsigned long)this, group);
    group = fn_802E7CDC(
        EmissionManager::Instance(), "crowd_riot_with_fade");
    EmissionManager::Instance()->Kill((unsigned long)this, group);
    fn_800EC12C(0x198B7ED3, this);

    if (resumeRiot && meState != 0 && meState == 1)
    {
        fn_80029320();
        meState = 2;
        mfStateTime = sUnidentifiedFloat3;

        if (mUnidentified30 == 0)
        {
            PhysicsSphere_80174F04* physicsObject
                = new (8, false) PhysicsSphere_80174F04(
                    sUnidentifiedFloat0);
            mUnidentified30 = physicsObject;
            physicsObject->mUnidentified38 = fn_80029C80;
            physicsObject->mUnidentified3C = this;
            mUnidentified30->SetPosition(
                mv3Position, PhysicsObject::WORLD_COORDINATES);
            mUnidentified30->EnableCollisions();
        }

        group = fn_802E7CDC(
            EmissionManager::Instance(), "crowd_riot_with_fade");
        EmissionController* controller = fn_802E7FE4(
            EmissionManager::Instance(), group, 3, true, false);
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
                        &CrowdRiot::fn_80029D78),
                    this,
                    placeholder0));
            controller->SetUpdateCallback(callback);
        }
        controller->m_uUserData = (u32)this;
        fn_800EBBFC(13, 0x198B7ED3, "CrowdRiot", this);
    }
}

static inline void UnidentifiedInline_800298D8()
{
    CrowdRiot* crowdRiot
        = (CrowdRiot*)g_pGame->mUnidentified10E0;
    if (crowdRiot->meState == 2)
    {
        nlVector3 velocity;
        velocity.x = sUnidentifiedFloat1 * crowdRiot->mv3Velocity.x;
        velocity.y = sUnidentifiedFloat1 * crowdRiot->mv3Velocity.y;
        velocity.z = 0.0f;
        crowdRiot->mv3Velocity = velocity;

        nlVector3 target;
        target.x = sUnidentifiedFloat4
                 * AIsgn(crowdRiot->mv3Position.x);
        float sideline = sUnidentifiedFloat5
                       + cField::GetSidelineY(1U);
        float sign = AIsgn(crowdRiot->mv3Position.y);
        target.y = sign * sideline;
        target.y = -target.y;
        target.z = 0.0f;
        crowdRiot->mv3Target = target;
        crowdRiot->meState = 4;
        crowdRiot->mfStateTime = -1.0f;
    }
}

void fn_800297B8(cBall* ball, CrowdRiot* crowdRiot)
{
    if (ball->mbStuckInRiotDone)
    {
        nlVector3 velocity;
        MakeRandomDirection2D(velocity, 10.0f);
        velocity.z = 10.0f + nlRandomf(5.0f);
        ball->SetVelocity(velocity, SPINTYPE_NONE, 0);
        u32 packedTime = ball->mtNoChargeLossTimer.m_uPackedTime;
        bool wasRunning = packedTime != 0;
        ball->mtNoChargeLossTimer.m_uPackedTime = 0;
        ball->mtNoChargeLossTimer.m_unk0 = wasRunning;
        ball->mbStuckInRiotDone = false;
    }
    else if (ball->mtNoChargeLossTimer.m_uPackedTime == 0)
    {
        if (ball->m_tNoPickupTimer.m_uPackedTime == 0)
        {
            ball->mtNoChargeLossTimer.SetSeconds(1.0f);
            ball->mbStuckInRiotDone = false;
            ball->m_tNoPickupTimer.SetSeconds(1.25f);
        }
    }
    else
    {
        float time = ball->mtNoChargeLossTimer.GetSeconds();
        nlVector3 position;
        position.x = (1.0f - time) * crowdRiot->mv3Position.x
                   + time * ball->m_v3Position.x;
        position.y = (1.0f - time) * crowdRiot->mv3Position.y
                   + time * ball->m_v3Position.y;
        position.z = 0.18f;
        ball->SetPosition(position);
    }
}

void fn_800298D8(void*)
{
    UnidentifiedInline_800298D8();
}

void fn_800299C4(void*)
{
    UnidentifiedInline_800298D8();
}

void fn_80029AB0(void*)
{
    UnidentifiedInline_800298D8();
}

void fn_80029B9C(void* param)
{
    UnidentifiedEventData_80066B08* event
        = (UnidentifiedEventData_80066B08*)param;
    CrowdRiot* crowdRiot = event->mUnidentified10;
    PhysicsObject* object = event->mUnidentified0C;

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
        cBall* ball = ((PhysicsBall*)object)->m_pBall;
        cPlayer* player = ball->m_pOwner;
        if (player != 0)
        {
            if (player->m_eClassType == FIELDER)
            {
                ((cFielder*)player)->fn_80043ADC();
            }
            else
            {
                fn_8007EB90((Goalie*)player);
            }
        }
        else
        {
            fn_800297B8(ball, crowdRiot);
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

void fn_80029C80(PhysicsObject*, PhysicsObject* other,
    nlVector3& position, void* context)
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
        if (crowdRiot->meState != 1)
        {
            UnidentifiedEventData_80066B08* event = 0;
            lbl_80571438.Allocate(event);
            event->mUnidentified0C = other;
            event->mUnidentified00 = position;
            event->mUnidentified10 = crowdRiot;
            fn_8005E29C(g_pGame, event);
        }
        break;
    }
    }
}

void CrowdRiot::fn_80029D78(EmissionController& controller)
{
    controller.SetPosition(mv3Position);
}
