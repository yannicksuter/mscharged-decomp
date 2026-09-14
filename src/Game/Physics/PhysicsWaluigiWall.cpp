#include "Game/AI/AvoidableObject.h"
#include "Game/Physics/PhysicsWaluigiWall.h"

#include "Game/AI/Fielder.h"
#include "Game/Ball.h"
#include "Game/CharacterTweaks.h"
#include "Game/Drawable/DrawableCharacter.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Effects/EmitterCallbacks.h"
#include "Game/EventRegistry.h"
#include "Game/Game.h"
#include "Game/MathHelpers.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBirdoEgg.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsKoopaShell.h"
#include "Game/Physics/PhysicsNPC.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Render/SkinAnimatedMovableNPC.h"
#include "Game/ReplayManager.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Team.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlMemory.h"
#include "NL/platvmath.h"

extern "C" bool fn_802B6BC8(const nlVector3*, const nlVector3*,
    const nlVector3*, const nlVector3*, float*, float*);
extern "C" void fn_8014A180(cFielder*);
extern "C" bool fn_800167A8(cBall*);
extern "C" float fn_800156A8(cBall*);
extern "C" void fn_8007CB78(cPlayer*, int, unsigned int);
extern "C" void fn_80060608(cGame*, cFielder*);
extern "C" void fn_80146060(UnidentifiedEventData24*);
extern "C" void fn_801461A8();
extern SlotPool<UnidentifiedEventData24> lbl_80570138;
float Interpolate(float, float, float);

void OnWaluigiWallEffectFinished(EmissionController&, int);
void UpdateWaluigiWallEmitter(EmissionController&);
void OnWaluigiWallStart(cPlayer*);
void OnWaluigiWallEnd(cPlayer*);
void OnWaluigiWallAbort(cPlayer*);
void OnWaluigiWallMegastrikeStart(void*);

extern char gWaluigiWallEffectName[];
extern char gWaluigiWallStartEventName[];
extern char gWaluigiWallEndEventName[];
extern char gWaluigiWallAbortEventName[];
extern char gWaluigiWallMegastrikeEventName[];

float gWaluigiWallWidth = 1.0f;
float gWaluigiWallHeight = 1.2f;
float gWaluigiWallShrinkSpeed = 16.5f;
float gWaluigiWallShrinkDelay = 4.2f;
float gWaluigiWallPatchDamage = 0.5f;
float gWaluigiWallDamage_806DCB24 = 0.5f;
float gWaluigiWallYoshiEggDamage = 0.5f;
float gWaluigiWallChainChompDamage = 0.5f;
float gWaluigiWallBowserDamage = 0.75f;

unsigned int gWaluigiWallManagerCount;
unsigned int gWaluigiWallNextID;
bool gWaluigiWallShrinkAll;
float gWaluigiWallDecayRate;
float gWaluigiWallShellBananaDamage;
float gWaluigiWallDamage_806E12FC;
float gWaluigiWallDamage_806E1300;
float gWaluigiWallGoalieDamage;
float gWaluigiWallFielderDamage;
float gWaluigiWallSlideAttackDamage;
float gWaluigiWallMinHitDamage;
float gWaluigiWallMaxHitDamage;
float gWaluigiWallMinBallDamage;
float gWaluigiWallMaxBallDamage;

PhysicsWaluigiWall::PhysicsWaluigiWall(cFielder* owner, float width, float height)
    : PhysicsBox(g_CollisionSpace, 0, 0.1f, width, height)
    , mOwner(owner)
    , mEmitter(0)
    , mHealth(1.0f)
    , mDestroy(false)
{
    nlMatrix3 rotation;
    nlMakeRotationMatrixZ(rotation,
        0.0000958738f * owner->mUnidentified024.m_aActualFacingDirection);
    nlVector3 direction;
    rotation.GetRow_(0, direction);
    nlVec3ScaleAdd(mStartPoint, -0.1f, direction, owner->mUnidentified024.m_v3Position);
    nlVec3ScaleAdd(mEndPoint, -0.0f, direction, owner->mUnidentified024.m_v3Position);
    nlVector3 position;
    nlVecLerp(position, mStartPoint, mEndPoint, 0.5f);
    position.z = 0.5f * height;
    SetPosition(position, WORLD_COORDINATES);
    SetRotation(rotation, WORLD_COORDINATES);
    mAge = 0.0f;
    mID = ++gWaluigiWallNextID;
    SetCategory(0x4000);
    SetCollide(0xB062);
    mAvoidable = new (nlMalloc(sizeof(AvoidablePolygon), 8, false))
        AvoidablePolygon(4, mStartPoint, mEndPoint, gWaluigiWallWidth);
}

PhysicsWaluigiWall::~PhysicsWaluigiWall()
{
    delete mAvoidable;
    if (mEmitter != 0)
        mEmitter->m_uUserData = 0;
}

ContactType PhysicsWaluigiWall::Contact(PhysicsObject* other, dContact*, int)
{
    switch (other->GetObjectType())
    {
    case 4:
    {
        cPlayer* player = (cPlayer*)((PhysicsCharacter*)other->m_parentObject)->m_pAICharacter;
        if (player->m_eClassType == 2)
        {
            if (((cFielder*)player)->mbTangible)
                return FielderContact((cFielder*)player);
            return NO_CONTACT;
        }
        if (player->m_eClassType == 3)
        {
            cFielder* goalie = (cFielder*)player;
            fn_8007CB78(goalie, 1, mID);
            return FielderContact(goalie);
        }
        return ONE_WAY_CONTACT_OTHER;
    }
    case 16:
    {
        cBall* ball = ((PhysicsAIBall*)other)->m_pAIBall;
        cPlayer* player = ball->m_pOwner;
        if (player != 0)
        {
            if (player->m_eClassType == 2)
            {
                if (((cFielder*)player)->mbTangible)
                    return FielderContact((cFielder*)player);
                return NO_CONTACT;
            }
            if (player->m_eClassType == 3)
                return FielderContact((cFielder*)player);
            return ONE_WAY_CONTACT_OTHER;
        }
        if (ball->m_tLightningTimer.m_uPackedTime != 0 || ball->HasActivePassTarget())
        {
            if (fn_800167A8(ball))
                return NO_CONTACT;
            ++ball->m_bBallPathChangeCount;
            ++ball->m_bBallDeflectCount;
            FakeBallWorld::InvalidateBallCache();
            ApplyDamage(Interpolate(gWaluigiWallMinBallDamage, gWaluigiWallMaxBallDamage, fn_800156A8(ball)));
            fn_801461A8();
            return ONE_WAY_CONTACT_OTHER;
        }
        return ONE_WAY_CONTACT_OTHER;
    }
    case 28:
    {
        PhysicsPatch* patch = (PhysicsPatch*)other;
        if (patch->m_Type == 1)
            ApplyDamage(gWaluigiWallPatchDamage);
        else if (patch->m_Type == 6)
        {
            UnidentifiedEventData24* data = 0;
            lbl_80570138.Allocate(data);
            data->mUnidentified0C = patch->m_pOwner;
            data->mUnidentified10 = patch;
            fn_80146060(data);
        }
        return NO_CONTACT;
    }
    case 18:
    case 23:
    case 25:
    case 29:
        return NO_CONTACT;
    case 20:
    case 21:
        ApplyDamage(gWaluigiWallShellBananaDamage);
        return ONE_WAY_CONTACT_OTHER;
    case 33:
    {
        CollisionBirdoEggEndData* data = 0;
        g_CollisionBirdoEggEndDataPool.Allocate(data);
        data->egg = ((PhysicsBirdoEgg*)other)->mBirdoEgg;
        data->cracked = true;
        QueueCollisionBirdoEggEnd(data);
        return ONE_WAY_CONTACT_OTHER;
    }
    case 34:
    {
        CollisionKoopaShellEndData* data = 0;
        g_CollisionKoopaShellEndDataPool.Allocate(data);
        data->shell = ((PhysicsKoopaShell*)other)->mKoopaShell;
        data->cracked = true;
        QueueCollisionKoopaShellEnd(data);
        return ONE_WAY_CONTACT_OTHER;
    }
    case 24:
    {
        PhysicsNPC* npc = (PhysicsNPC*)other;
        SkinAnimatedNPC* object = npc->mpAINPC;
        if (object != 0)
        {
            bool chainChomp = object->GetSkinAnimatedNPC_Type() == SkinAnimatedNPC_CHAIN_CHOMP;
            if (chainChomp)
                ApplyDamage(gWaluigiWallChainChompDamage);
        }
        object = npc->mpAINPC;
        if (object != 0)
        {
            bool bowser = object->GetSkinAnimatedNPC_Type() == SkinAnimatedNPC_BOWSER;
            if (bowser)
                ApplyDamage(gWaluigiWallBowserDamage);
        }
        return NO_CONTACT;
    }
    case 27:
        ApplyDamage(gWaluigiWallDamage_806E12FC);
        return NO_CONTACT;
    case 26:
        ApplyDamage(gWaluigiWallDamage_806E1300);
        return ONE_WAY_CONTACT_OTHER;
    case 32:
        ApplyDamage(gWaluigiWallYoshiEggDamage);
        return NO_CONTACT;
    default:
        return NO_CONTACT;
    }
}

bool PhysicsWaluigiWall::SetContactInfo(dContact* contact, PhysicsObject*, bool first)
{
    if (first)
        SetDefaultContactInfo(contact);
    contact->surface.bounce = 0.15f;
    contact->surface.mu = 0.005f;
    contact->surface.bounce_vel = 0.0f;
    return true;
}

void PhysicsWaluigiWall::PreCollide()
{
    const nlVector3& velocity = g_pBall->m_v3Velocity;
    if (g_pBall->m_pOwner == 0 && !fn_800167A8(g_pBall)
        && nlVec3DotProduct(velocity, velocity) > 400.0f
        && g_pBall->meBallState != 10)
    {
        const nlVector3& start = g_pBall->GetPosition();
        nlVector3 end;
        nlVec3ScaleAdd(end, FixedUpdateTask::GetPhysicsUpdateTick(), velocity, start);
        float height = gWaluigiWallHeight + g_pBall->m_pPhysicsBall->GetRadius();
        if (start.z < height || end.z < height)
        {
            nlVector3 delta;
            nlVector3 normal;
            nlVec3Set(normal, mStartPoint.y - mEndPoint.y, mEndPoint.x - mStartPoint.x, 0.0f);
            nlVec3Sub(delta, end, start);
            float inverseLength = nlRecipSqrt(nlVec3DotProduct(normal, normal), true);
            nlVec3Set(normal, inverseLength * normal.x, inverseLength * normal.y, inverseLength * normal.z);
            float halfWidth = 0.5f * gWaluigiWallWidth;
            nlVector2 a, b, c, d;
            nlVec2Set(a, mStartPoint.x + halfWidth * normal.x, mStartPoint.y + halfWidth * normal.y);
            nlVec2Set(b, mStartPoint.x + -halfWidth * normal.x, mStartPoint.y + -halfWidth * normal.y);
            nlVec2Set(c, mEndPoint.x + halfWidth * normal.x, mEndPoint.y + halfWidth * normal.y);
            nlVec2Set(d, mEndPoint.x + -halfWidth * normal.x, mEndPoint.y + -halfWidth * normal.y);
            float time1, wallTime1, time2, wallTime2;
            bool hit1 = fn_802B6BC8(&start, &end, (const nlVector3*)&a, (const nlVector3*)&c, &time1, &wallTime1);
            bool hit2 = fn_802B6BC8(&start, &end, (const nlVector3*)&b, (const nlVector3*)&d, &time2, &wallTime2);
            if (hit1 || hit2)
            {
                float time = nlMinEquals(time1, time2);
                if (time < -2.0f)
                    time = nlMaxEquals(time1, time2);
                nlVector3 position;
                nlVec3ScaleAdd(position, time, delta, start);
                if (position.z <= height)
                {
                    nlVector3 angularVelocity;
                    g_pBall->m_pPhysicsBall->GetAngularVelocity(&angularVelocity);
                    g_pBall->SetPosition(position);
                    nlVector3 reflected;
                    nlVector3 projection;
                    nlVec3Project(projection, velocity, normal);
                    nlVec3ScaleAdd(reflected, -2.0f, projection, velocity);
                    nlVec3Scale(reflected, reflected, 0.15f);
                    nlVec3Scale(angularVelocity, angularVelocity, 0.8f);
                    g_pBall->SetVelocity(reflected, SPINTYPE_PARAMETER, &angularVelocity);
                    PhysicsAIBall* physics = g_pBall->m_pPhysicsBall;
                    physics->mbUseMagnusEffect = false;
                    physics->mfChargeBonus = 0.0f;
                }
            }
        }
    }
}

void PhysicsWaluigiWall::SetEndPoint(const nlVector3& end)
{
    mEndPoint = end;
    nlVector3 position;
    nlVecLerp(position, mStartPoint, mEndPoint, 0.5f);
    float sides[3];
    GetSides(sides);
    position.z = 0.5f * sides[2];
    SetPosition(position, WORLD_COORDINATES);

    nlVector3 direction;
    nlVec3Set(direction, mEndPoint.x - mStartPoint.x, mEndPoint.y - mStartPoint.y, 0.0f);
    float length = nlVec3Length(direction);
    if (length > 0.01f)
        nlVec3Scale(direction, direction, nlRecipSqrt(nlVec3DotProduct(direction, direction), true));
    else
    {
        nlVec3Set(direction, 1.0f, 0.0f, 0.0f);
        length = 1.0f;
    }
    SetSides(length, sides[1], sides[2]);
    nlMatrix3 rotation;
    rotation.m11 = direction.x;
    rotation.m12 = direction.y;
    rotation.m13 = 0.0f;
    rotation.m21 = -direction.y;
    rotation.m22 = direction.x;
    rotation.m23 = 0.0f;
    rotation.m31 = 0.0f;
    rotation.m32 = 0.0f;
    rotation.m33 = 1.0f;
    SetRotation(rotation, WORLD_COORDINATES);
    mAvoidable->Update(*(const nlVector2*)&mStartPoint, *(const nlVector2*)&mEndPoint, gWaluigiWallWidth);
}

void PhysicsWaluigiWall::ApplyDamage(float damage)
{
    mHealth -= damage;
    if (mHealth <= 0.0f)
    {
        mHealth = 0.0f;
        mDestroy = true;
    }
}

inline void WaluigiWallManager::EndWall()
{
    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(gWaluigiWallEffectName);
    if (EmissionManager::Instance()->IsPlaying((unsigned long)mCurrentWall, group))
        EmissionManager::Instance()->Kill((unsigned long)mCurrentWall, group);
    mEmitterStarted = false;
    mCurrentWall->mAge = 0.0f;
    PhysicsWaluigiWall* wall = mCurrentWall;
    nlVector2 delta;
    delta.x = wall->mStartPoint.x - wall->mEndPoint.x;
    delta.y = wall->mStartPoint.y - wall->mEndPoint.y;
    if (nlVec2DotProduct(delta, delta) < 0.09f)
        wall->mDestroy = true;
    else
        mPreviousWall = wall;
    mCurrentWall = 0;
}

ContactType PhysicsWaluigiWall::FielderContact(cFielder* player)
{
    if (player->m_eClassType == 2)
    {
        if (player == mOwner)
        {
            WaluigiWallManager* manager = player->mUnidentified3F8.mUnidentified08;
            float gracePeriod = 0.2f;
            if (manager->mCurrentWall == this)
                return NO_CONTACT;
            if (mAge < gracePeriod)
                return ONE_WAY_CONTACT_OTHER;
            OnWaluigiWallAbort(player);
        }
        else if (player->fn_8003E74C())
            ApplyDamage(gWaluigiWallDamage_806DCB24);
        else if (player->m_eActionState == ACTION_HIT)
            ApplyDamage(Interpolate(gWaluigiWallMinHitDamage, gWaluigiWallMaxHitDamage, player->GetTweaks()->mUnidentified064));
        else if (player->m_eActionState == ACTION_SLIDE_ATTACK)
            ApplyDamage(gWaluigiWallSlideAttackDamage);
        else
            ApplyDamage(gWaluigiWallFielderDamage);
    }
    else
        ApplyDamage(gWaluigiWallGoalieDamage);
    return ONE_WAY_CONTACT_OTHER;
}

char gWaluigiWallEffectName[] = "waluigi_super_ability";
char gWaluigiWallStartEventName[] = "WaluigiWallStart";
char gWaluigiWallEndEventName[] = "WaluigiWallEnd";
char gWaluigiWallAbortEventName[] = "WaluigiWallAbort";
char gWaluigiWallMegastrikeEventName[] = "MegastrikeStart";

WaluigiWallManager::WaluigiWallManager()
    : mCurrentWall(0)
    , mPreviousWall(0)
    , mOwner(0)
    , mEmitterStarted(false)
    , mHeightUpdateDelay(0.0f)
{
    {
        Function<cPlayer*> callback(OnWaluigiWallStart);
        EventRegistryValue* ppEvent;
        unsigned int uHash;
        uHash = HashEventName(gWaluigiWallStartEventName, -1);
        ppEvent = 0;
        g_pEventRegistry->Find(uHash, &ppEvent, 0);
        UnidentifiedEventBase* pEvent = ppEvent != 0 ? ppEvent->event : 0;
        ((UnidentifiedTypedEvent<cPlayer>*)pEvent)
            ->Add(callback, (unsigned int)&mStartConnection, -1);
    }
    {
        Function<cPlayer*> callback(OnWaluigiWallEnd);
        EventRegistryValue* ppEvent;
        unsigned int uHash;
        uHash = HashEventName(gWaluigiWallEndEventName, -1);
        ppEvent = 0;
        g_pEventRegistry->Find(uHash, &ppEvent, 0);
        UnidentifiedEventBase* pEvent = ppEvent != 0 ? ppEvent->event : 0;
        ((UnidentifiedTypedEvent<cPlayer>*)pEvent)
            ->Add(callback, (unsigned int)&mEndConnection, -1);
    }
    {
        Function<cPlayer*> callback(OnWaluigiWallAbort);
        EventRegistryValue* ppEvent;
        unsigned int uHash;
        uHash = HashEventName(gWaluigiWallAbortEventName, -1);
        ppEvent = 0;
        g_pEventRegistry->Find(uHash, &ppEvent, 0);
        UnidentifiedEventBase* pEvent = ppEvent != 0 ? ppEvent->event : 0;
        ((UnidentifiedTypedEvent<cPlayer>*)pEvent)
            ->Add(callback, (unsigned int)&mAbortConnection, -1);
    }
    {
        Function<void*> callback(OnWaluigiWallMegastrikeStart);
        EventRegistryValue* ppEvent;
        unsigned int uHash;
        uHash = HashEventName(gWaluigiWallMegastrikeEventName, -1);
        ppEvent = 0;
        g_pEventRegistry->Find(uHash, &ppEvent, 0);
        UnidentifiedEventBase* pEvent = ppEvent != 0 ? ppEvent->event : 0;
        ((UnidentifiedTypedEvent<void>*)pEvent)
            ->Add(callback, (unsigned int)&mMegastrikeConnection, -1);
    }
    for (unsigned int i = 0; i < 20; ++i)
        mWalls[i] = 0;
    ++gWaluigiWallManagerCount;
}

WaluigiWallManager::~WaluigiWallManager()
{
    --gWaluigiWallManagerCount;
    ClearWalls();
    if (gWaluigiWallManagerCount == 0)
    {
        PhysicsWaluigiWall::pool.FreeBlocks();
        gWaluigiWallNextID = 0;
    }
}

PhysicsWaluigiWall* WaluigiWallManager::CreateWall(cFielder* owner, float width, float height)
{
    for (int i = 0; i < 20; ++i)
    {
        if (mWalls[i] == 0)
        {
            PhysicsWaluigiWall* wall = new PhysicsWaluigiWall(owner, width, height);
            mWalls[i] = wall;
            if (mCurrentWall != 0)
            {
                mPreviousWall = mCurrentWall;
                fn_80060608(g_pGame, mCurrentWall->mOwner);
            }
            mCurrentWall = wall;
            mOwner = owner;
            if (!mEmitterStarted)
            {
                EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(gWaluigiWallEffectName);
                if (group != 0)
                {
                    EmissionController* controller = EmissionManager::Instance()->Create(group, 3, true, 0);
                    controller->SetPosition(owner->mUnidentified024.m_v3Position);
                    controller->SetVelocity(owner->mUnidentified024.m_v3Velocity);
                    controller->m_uUserData = (unsigned long)wall;
                    controller->SetUpdateCallback(Function1<void, EmissionController&>(UpdateWaluigiWallEmitter));
                    controller->SetFinishedCallback(Function2<void, EmissionController&, int>(OnWaluigiWallEffectFinished));
                    wall->mEmitter = controller;
                    mEmitterStarted = true;
                }
            }
            return wall;
        }
    }
    return 0;
}

void WaluigiWallManager::ClearWalls()
{
    for (unsigned int i = 0; i < 20; ++i)
    {
        if (mWalls[i] != 0)
        {
            delete mWalls[i];
            mWalls[i] = 0;
        }
    }
    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(gWaluigiWallEffectName);
    EmissionManager::Instance()->Destroy(group);
    mCurrentWall = 0;
    mPreviousWall = 0;
}

inline void PhysicsWaluigiWall::Shrink(float dt)
{
    float distance = gWaluigiWallShrinkSpeed * dt;
    nlVector2 direction;
    direction.x = mStartPoint.x - mEndPoint.x;
    direction.y = mStartPoint.y - mEndPoint.y;
    float length = nlVec2Length(direction);
    if (distance >= length)
        mDestroy = true;
    else
    {
        nlVector3 start;
        nlVecLerp(start, mStartPoint, mEndPoint, distance / length);
        mStartPoint = start;
        SetEndPoint(mEndPoint);
    }
}

void WaluigiWallManager::Update(float dt)
{
    PhysicsWaluigiWall* oldest = 0;
    if (mHeightUpdateDelay > 0.0f)
        mHeightUpdateDelay -= dt;
    for (unsigned int i = 0; i < 20; ++i)
    {
        PhysicsWaluigiWall* wall = mWalls[i];
        if (wall != 0)
        {
            if (wall != mCurrentWall)
                wall->mAge += dt;
            wall->ApplyDamage(gWaluigiWallDecayRate * dt);
            if (gWaluigiWallShrinkAll && !(wall->mAge < gWaluigiWallShrinkDelay))
                wall->Shrink(dt);
            if (wall->mDestroy)
            {
                if (wall->mEmitter != 0 && wall->mHealth < 0.2f)
                {
                    if (wall->mOwner->mUnidentified3DC)
                        wall->mOwner->fn_8005001C(true);
                    wall->mEmitter->m_uUserData = 0;
                    wall->mEmitter->m_TimeScale = 5.0f;
                    wall->mEmitter = 0;
                }
                if (wall == mCurrentWall)
                    mCurrentWall = 0;
                if (wall == mPreviousWall)
                    mPreviousWall = 0;
                delete wall;
                mWalls[i] = 0;
            }
            else if (mHeightUpdateDelay <= 0.0f)
            {
                float sides[3];
                nlVector3 position = wall->GetPosition();
                wall->GetSides(sides);
                sides[2] = gWaluigiWallHeight * wall->mHealth;
                wall->SetSides(sides[0], sides[1], sides[2]);
                position.z = 0.5f * sides[2];
                wall->SetPosition(position, PhysicsObject::WORLD_COORDINATES);
            }
        }
        wall = mWalls[i];
        if (wall != 0 && (oldest == 0 || oldest->mID > wall->mID))
            oldest = wall;
    }
    if (!gWaluigiWallShrinkAll && oldest != 0 && oldest != mCurrentWall
        && mHeightUpdateDelay <= 0.0f && !(oldest->mAge < gWaluigiWallShrinkDelay))
        oldest->Shrink(dt);
    if (mCurrentWall != 0 && mCurrentWall->mOwner != 0)
    {
        nlMatrix4 rotation;
        mCurrentWall->GetRotation(&rotation);
        nlVector3 direction;
        rotation.GetRow_(0, direction);
        nlVector3 end;
        nlVec3ScaleAdd(end, -0.0f, direction, mCurrentWall->mOwner->mUnidentified024.m_v3Position);
        mCurrentWall->SetEndPoint(end);
        if (nlVec3DistanceSquared2D(mCurrentWall->mStartPoint, mCurrentWall->mEndPoint) > 25.0f)
            CreateWall(mCurrentWall->mOwner, gWaluigiWallWidth, gWaluigiWallHeight);
    }
}

PhysicsWaluigiWall* WaluigiWallManager::GetWall(int index)
{
    return mWalls[index];
}

PhysicsWaluigiWall* WaluigiWallManager::FindWall(unsigned int id)
{
    for (int i = 0; i < 20; ++i)
    {
        PhysicsWaluigiWall* wall = mWalls[i];
        if (wall != 0 && id == wall->mID)
            return wall;
    }
    return 0;
}

void OnWaluigiWallEffectFinished(EmissionController& controller, int reason)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
        return;
    if (!controller.m_Replaying && reason == 2)
    {
        PhysicsWaluigiWall* wall = (PhysicsWaluigiWall*)controller.m_uUserData;
        if (wall != 0)
            wall->mEmitter = 0;
    }
}

void UpdateWaluigiWallEmitter(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
        return;
    if (!controller.m_Replaying && ReplayManager::Instance()->mRender != 0)
    {
        PhysicsWaluigiWall* wall = (PhysicsWaluigiWall*)controller.m_uUserData;
        if (wall != 0)
        {
            DrawableCharacter* character = GetReplayDrawableCharacter(wall->mOwner);
            controller.SetPosition(character->position);
            controller.SetVelocity(character->velocity);
            controller.SetPoseAccumulator(*character->poseAccumulator);
            controller.SetAnimController(character->GetAnimController());
        }
    }
}

void OnWaluigiWallStart(cPlayer* player)
{
    static_cast<cFielder*>(player)->mUnidentified3F8.mUnidentified08->CreateWall(
        static_cast<cFielder*>(player), gWaluigiWallWidth, gWaluigiWallHeight);
}

void OnWaluigiWallEnd(cPlayer* player)
{
    WaluigiWallManager* manager = static_cast<cFielder*>(player)->mUnidentified3F8.mUnidentified08;
    if (manager->mCurrentWall != 0)
        manager->EndWall();
}

void OnWaluigiWallAbort(cPlayer* player)
{
    WaluigiWallManager* manager = static_cast<cFielder*>(player)->mUnidentified3F8.mUnidentified08;
    if (manager->mCurrentWall != 0)
    {
        manager->EndWall();
        fn_8014A180(static_cast<cFielder*>(player));
    }
}

void OnWaluigiWallMegastrikeStart(void*)
{
    for (int teamIndex = 0; teamIndex < 2; ++teamIndex)
    {
        cTeam* team = g_pTeams[teamIndex];
        if (team == 0)
            continue;
        for (int i = 0; i < 4; ++i)
        {
            cFielder* player = team->GetFielder(i);
            if (player != 0 && player->mUnidentified3F8.mUnidentified08 != 0)
                player->mUnidentified3F8.mUnidentified08->ClearWalls();
        }
    }
}

SlotPool<PhysicsWaluigiWall> PhysicsWaluigiWall::pool(16, 16);
