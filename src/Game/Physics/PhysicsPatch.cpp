#include "Game/Sys/audio.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/Physics/PhysicsPatch.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/DebugWriteCache.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Event.h"
#include "Game/Field.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Game.h"
#include "Game/MathHelpers.h"
#include "Game/Physics/CollisionSpace.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsBanana.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/PhysicsNPC.h"
#include "Game/Physics/PhysicsShell.h"
#include "Game/Player.h"
#include "Game/Render/SkinAnimatedNPC.h"
#include "NL/nlAVLTree.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlstring_tmpl.h"
#include "Game/NetworkSync.h"

#include <math.h>
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

class EffectsGroup;

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

extern CollisionSpace* g_CollisionSpace;
extern UnidentifiedEventRegistry* g_pEventRegistry;


extern "C" void fn_8017472C(void*);
extern "C" bool fn_800977A4(cFielder*, float);

extern SlotPool<UnidentifiedEventData30> lbl_80570160;

static const nlVector3 lbl_804DCCAC = { -2.0f, 0.0f, -5.0f };

unsigned short lbl_806DCAB8 = 0xFFFF;
float lbl_806DCABC = 0.25f;

SlotPool<PhysicsPatch> PhysicsPatch::lbl_805705D0(16, 16);
PhysicsPatchManager_801740D0* lbl_806E12C8;

static void fn_8017498C(EmissionController& controller);

inline void PhysicsPatch::UnidentifiedKillEffect()
{
    if (m_Type != -1)
    {
        PhysicsPatchInfo* info = GetPhysicsPatchInfo(m_Type);
        if (info->mEffectName != 0)
        {
            EffectsGroup* effects = EmissionManager::Instance()->GetEffectsGroup(info->mEffectName);
            if (effects != 0)
            {
                EmissionManager::Instance()->Kill((unsigned long)this, effects);
                EmissionController* controller = EmissionManager::Instance()->FindController((unsigned long)this, effects);
                if (controller != 0)
                {
                    controller->mUpdateCallback.Clear();
                }
            }
        }
    }
}

inline void PhysicsPatch::UnidentifiedDestroyEffect()
{
    if (m_Type != -1)
    {
        PhysicsPatchInfo* info = GetPhysicsPatchInfo(m_Type);
        if (info->mEffectName != 0)
        {
            EffectsGroup* effects = EmissionManager::Instance()->GetEffectsGroup(info->mEffectName);
            if (effects != 0)
            {
                EmissionManager::Instance()->Destroy((unsigned long)this, effects);
                EmissionController* controller = EmissionManager::Instance()->FindController((unsigned long)this, effects);
                if (controller != 0)
                {
                    controller->mUpdateCallback.Clear();
                }
            }
        }
    }
}

PhysicsPatch::PhysicsPatch()
    : PhysicsSphere(g_CollisionSpace, 0, 0.5f)
    , mUnidentified38()
    , mUnidentified40(0)
    , m_Type(-1)
    , m_bVisible(false)
    , m_Gravity(0.0f)
    , m_pTarget(0)
    , m_TargetSeekSpeed(0.0f)
    , m_fStartRadiusTime(0.0f)
    , m_fEndRadiusTime(1.0f)
    , m_bFrozen(false)
    , m_FreezeTimer(0.0f)
    , m_PathSpeed(0.0f)
    , m_CurrentPathPoint(0)
    , m_PathPointCount(0)
{
    m_Gravity = 0.0f;
    SetCategory(0x1000);
}

PhysicsPatch::~PhysicsPatch()
{
    if (mUnidentified44 != 0)
    {
        delete mUnidentified44;
    }
}

void PhysicsPatch::fn_80172EE0(const int* type)
{
    int view;
    m_Type = *type;
    m_fLifetime = 0.0f;
    m_fCurtime = 0.0f;
    m_bKillMe = false;
    m_bVisible = true;
    m_fStartRadiusTime = 0.0f;
    m_fEndRadiusTime = 1.0f;
    m_pTarget = 0;
    m_TargetSeekSpeed = 0.0f;

    PhysicsPatchInfo* info = GetPhysicsPatchInfo(m_Type);
    SetCollide(info->mCollisionMask);
    EnableCollisions();
    m_Gravity = info->mGravity;
    PlaySound(10, info->mSoundID, 0, 0);

    if (info->mEffectName != 0 && nlStrLen(info->mEffectName) != 0)
    {
        EffectsGroup* effects = EmissionManager::Instance()->GetEffectsGroup(info->mEffectName);
        if (effects != 0)
        {
            view = 3;
            if (*type >= 8)
            {
                view = 2;
            }
            EmissionController* controller = EmissionManager::Instance()->Create(effects, view, true, 0);
            controller->SetPosition(GetPosition());
            controller->m_uUserData = (unsigned long)this;
            controller->SetUpdateCallback(
                Function1<void, EmissionController&>(fn_8017498C));
            controller->m_fGround = 0.02f;
        }
    }

    switch (m_Type)
    {
    case 0:
    case 2:
    case 4:
    case 5:
    case 8:
    case 9:
    case 10:
        mUnidentified44
            = new (nlMalloc(sizeof(AvoidablePatch), 8, false))
                AvoidablePatch(this);
        break;
    default:
        mUnidentified44 = 0;
        break;
    }
}

void PhysicsPatch::Unknown0()
{
    UnidentifiedKillEffect();

    m_Type = -1;
    m_pOwner = 0;
    m_fStartRadius = 0.5f;
    m_fEndRadius = 0.5f;
    m_fLifetime = 0.0f;
    m_fCurtime = 0.0f;
    m_Index = -1;
    m_bKillMe = true;
    m_bVisible = false;
    SetRadius(0.5f);
    DisableCollisions();
    SetPosition(lbl_804DCCAC, WORLD_COORDINATES);
    m_Velocity = v3Zero;
    mUnidentified40 = 0;
    m_PathSpeed = 0.0f;
    m_PathPointCount = 0;
    m_CurrentPathPoint = 0;
    mUnidentified38.Clear();
}

ContactType PhysicsPatch::Contact(
    PhysicsObject* other, dContact*, int)
{
    UnidentifiedEventData24* eventData;
    GetPhysicsPatchInfo(m_Type);

    switch (other->GetObjectType())
    {
    case 4:
    {
        cFielder* fielder = (cFielder*)((PhysicsCharacter*)other->m_parentObject)->m_pAICharacter;
        if (fielder->IsCharacterInAir(GetPosition().z + GetRadius()))
        {
            return NO_CONTACT;
        }
        if (fn_800977A4(fielder, GetPosition().z - GetRadius()))
        {
            return NO_CONTACT;
        }
        if (fielder->m_eClassType == FIELDER)
        {
            if (!fielder->mbTangible)
            {
                return NO_CONTACT;
            }
            switch (m_Type)
            {
            case 4:
            case 5:
            case 9:
            case 12:
            {
                float height = lbl_806DCABC;
                if (fielder->mUnidentified024.m_eCharacterClass == (eCharacterClass)0x10)
                {
                    height = 0.5f;
                }
                if (fielder->fn_8003EA44())
                {
                    height = 2.25f;
                }
                if (fielder->IsCharacterInAir(height * fielder->mUnidentified024.m_fPlayerScale))
                {
                    return NO_CONTACT;
                }
                break;
            }
            }
        }
        eventData = 0;
        lbl_80570138.Allocate(eventData);
        eventData->mUnidentified0C = fielder;
        eventData->mUnidentified10 = this;
        QueueCollisionPatchPlayer(eventData);
        return NO_CONTACT;
    }
    case 16:
    {
        fn_80148A9C((UnidentifiedEventData31*)this);
        if (m_Type == 0 && !m_bKillMe && ((PhysicsAIBall*)other)->m_pAIBall->mbBallOnFire)
        {
            eventData = 0;
            lbl_80570138.Allocate(eventData);
            eventData->mUnidentified0C = 0;
            eventData->mUnidentified10 = this;
            fn_80148954(eventData);
        }
        return NO_CONTACT;
    }
    case 18:
    {
        eventData = 0;
        lbl_80570138.Allocate(eventData);
        eventData->mUnidentified0C = 0;
        eventData->mUnidentified10 = this;
        fn_80148588(eventData);
        return NO_CONTACT;
    }
    case 20:
    {
        UnidentifiedEventData30* data = 0;
        lbl_80570160.Allocate(data);
        data->mUnidentified00 = ((PhysicsShell*)other)->m_pPowerupObject;
        data->mUnidentified04 = this;
        fn_801486D0(data);
        return NO_CONTACT;
    }
    case 21:
    {
        UnidentifiedEventData30* data = 0;
        lbl_80570160.Allocate(data);
        data->mUnidentified00 = ((PhysicsBanana*)other)->m_pPowerupObject;
        data->mUnidentified04 = this;
        fn_801486D0(data);
        return NO_CONTACT;
    }
    case 24:
    {
        bool isChainChomp
            = ((SkinAnimatedNPC*)((PhysicsNPC*)other)->mpAINPC)
                  ->GetSkinAnimatedNPC_Type()
           == SkinAnimatedNPC_CHAIN_CHOMP;
        if (isChainChomp)
        {
            fn_80148818((UnidentifiedEventData28*)((PhysicsNPC*)other)->mpAINPC);
        }
        return NO_CONTACT;
    }
    case 29:
        return m_Type == 1 ? ONE_WAY_CONTACT_THIS : NO_CONTACT;
    case 23:
        fn_80148BD8((UnidentifiedEventData31*)this);
        return NO_CONTACT;
    case 28:
        if (m_Type == 0 && !m_bKillMe)
        {
            switch (((PhysicsPatch*)other)->m_Type)
            {
            case 1:
            case 8:
            case 9:
            {
                UnidentifiedEventData24* data = 0;
                lbl_80570138.Allocate(data);
                data->mUnidentified0C = m_pOwner;
                data->mUnidentified10 = this;
                fn_80148954(data);
                break;
            }
            }
        }
        break;
    }

    return NO_CONTACT;
}

void PhysicsPatch::Update(float dt)
{
    if (m_bVisible == true && !m_bFrozen)
    {
        PhysicsPatchInfo* info = GetPhysicsPatchInfo(m_Type);
        m_fCurtime += dt;
        if (m_fCurtime <= m_fLifetime)
        {
            float startTime = m_fLifetime * m_fStartRadiusTime;
            float endTime = m_fLifetime * m_fEndRadiusTime;
            float radius;
            if (m_fCurtime < startTime)
            {
                radius = m_fStartRadius;
            }
            else if (m_fCurtime > endTime)
            {
                radius = m_fEndRadius;
            }
            else
            {
                radius = InterpolateClamped(m_fStartRadius, m_fEndRadius, (m_fCurtime - startTime) / (endTime - startTime));
            }
            radius = nlMaxEquals(radius, 0.00001f);
            if (radius != GetRadius())
            {
                SetRadius(radius);
            }

            if (mUnidentified40 != 0)
            {
                fn_80173DA4(dt);
                return;
            }

            float damping = 1.0f - InterpolateRangeClamped(info->mFriction, 0.6f, 0.02f, 0.5f, dt);
            nlVec3Scale(m_Velocity, damping);
            if (m_pTarget != 0)
            {
                fn_80173B18();
            }
            m_Velocity.z -= m_Gravity * dt;

            nlVector3 position;
            position.x = GetPosition().x + m_Velocity.x * dt;
            position.y = GetPosition().y + m_Velocity.y * dt;
            position.z = GetPosition().z + m_Velocity.z * dt;
            if (info->mBounce > 0.0f)
            {
                float goalLineX = cField::GetGoalLineX(1u);
                float halfWidth = 0.5f * (2.0f * cField::mv3FieldPosition.y);
                if (position.z < 0.02f && position.x < goalLineX && position.x > -1.0f * goalLineX
                    && position.y < halfWidth && position.y > -1.0f * halfWidth)
                {
                    position.z = 0.02f;
                    m_Velocity.z *= -1.0f * info->mBounce;
                }
            }
            SetPosition(position, WORLD_COORDINATES);
        }
        else
        {
            Unknown0();
            m_bKillMe = true;
        }
    }
    else if (m_bFrozen == true)
    {
        m_FreezeTimer -= dt;
    }
}

void PhysicsPatch::fn_801739A4(const nlVector3& position)
{
    SetPosition(position, WORLD_COORDINATES);
}

bool PhysicsPatch::SetContactInfo(
    dContact* contact, PhysicsObject*, bool setDefault)
{
    if (setDefault)
    {
        SetDefaultContactInfo(contact);
    }

    PhysicsPatchInfo* info = GetPhysicsPatchInfo(m_Type);
    contact->surface.bounce = info->mBounce;
    contact->surface.mu = info->mFriction;
    contact->surface.bounce_vel = 0.0f;
    return true;
}

void PhysicsPatch::fn_80173A10(float)
{
    if (m_Type == 4)
    {
        UnidentifiedDestroyEffect();
        Unknown0();
    }
}

void PhysicsPatch::fn_80173AF4()
{
    m_pTarget = 0;
    m_TargetSeekSpeed = -1.0f;
}

void PhysicsPatch::fn_80173B08(float time)
{
    m_fEndRadiusTime = time;
}

void PhysicsPatch::fn_80173B10(float time)
{
    m_fStartRadiusTime = time;
}

void PhysicsPatch::fn_80173B18()
{
    nlVector3 newVelocity;
    nlVector2 delta;
    nlVector2 direction;
    nlVector2 desired;
    nlVector2 normalized;

    delta.x = m_pTarget->GetPosition().x - GetPosition().x;
    delta.y = m_pTarget->GetPosition().y - GetPosition().y;
    nlVec2Length(delta);
    GetPhysicsPatchInfo(m_Type);

    nlVec2Scale(direction, delta, 1.0f / nlVec2Length(delta));
    nlVec2Scale(delta, direction, m_TargetSeekSpeed);
    float speed = nlSqrt(nlGetLengthSquared1D(m_Velocity.x) + nlGetLengthSquared1D(m_Velocity.y), true);

    desired.x = delta.x + m_Velocity.x;
    desired.y = delta.y + m_Velocity.y;
    nlVec2Scale(normalized, desired, 1.0f / nlVec2Length(desired));
    nlVec2Scale(desired, normalized, speed);

    newVelocity.x = desired.x;
    newVelocity.y = desired.y;
    newVelocity.z = m_Velocity.z;
    m_Velocity = newVelocity;
}

void PhysicsPatch::fn_80173C9C(
    nlVector3* points, int pointCount, float speed)
{
    mUnidentified40 = points;
    m_PathPointCount = pointCount;
    m_PathSpeed = speed;
    m_CurrentPathPoint = 0;
    if (m_PathSpeed < 0.0f)
    {
        m_CurrentPathPoint = pointCount - 1;
    }
}

nlVector3 PhysicsPatch::fn_80173CCC() const
{
    nlVector3 direction = { 0.0f, 0.0f, 0.0f };
    if (mUnidentified40 != 0 && m_CurrentPathPoint > 0)
    {
        nlVec3Sub(direction, mUnidentified40[m_CurrentPathPoint], m_position);
        float scale = nlRecipSqrt(direction.GetLengthSq3D(), false);
        nlVec3Scale(direction, scale);
    }
    return direction;
}

void PhysicsPatch::fn_80173DA4(float dt)
{
    bool finished = false;
    float remaining = dt * (float)fabs(m_PathSpeed);
    nlVector3 position = GetPosition();
    nlVector3 delta;
    nlVector3 step;
    nlVec3Sub(delta, mUnidentified40[m_CurrentPathPoint], position);
    float distanceSquared = delta.GetLengthSq3D();
    if (distanceSquared > remaining * remaining)
    {
        float scale = nlRecipSqrt(distanceSquared, true);
        nlVec3Set(delta, scale * delta.x, scale * delta.y, scale * delta.z);
        nlVec3Scale(step, delta, remaining);
        nlVec3Add(position, position, step);
    }
    else
    {
        int previousPoint = GetCurrentPathPoint();
        if (m_PathSpeed > 0.0f)
        {
            m_CurrentPathPoint++;
        }
        else
        {
            m_CurrentPathPoint--;
        }

        if (m_CurrentPathPoint < m_PathPointCount && m_CurrentPathPoint >= 0)
        {
            remaining -= nlSqrt(delta.GetLengthSq3D(), true);
            if (remaining > 0.0f)
            {
                nlVector3 nextDelta;
                nlVector3 nextStep;
                nlVec3Sub(nextDelta, mUnidentified40[m_CurrentPathPoint], mUnidentified40[previousPoint]);
                float scale = nlRecipSqrt(nextDelta.GetLengthSq3D(), true);
                nlVec3Set(nextDelta, scale * nextDelta.x, scale * nextDelta.y, scale * nextDelta.z);
                nlVec3Scale(nextStep, nextDelta, remaining);
                nlVec3Add(position, position, nextStep);
            }
        }
        else
        {
            finished = true;
            position = mUnidentified40[previousPoint];
        }
    }

    if (__fpclassifyf(position.x) == 1
        || __fpclassifyf(position.y) == 1
        || __fpclassifyf(position.z) == 1)
    {
        nlVec3Set(position, 0.0f, 0.0f, 0.0f);
    }
    SetPosition(position, WORLD_COORDINATES);

    if (finished == true && mUnidentified38)
    {
        mUnidentified38(this);
    }
}

PhysicsPatchManager_801740D0::PhysicsPatchManager_801740D0()
{
    for (int i = 0; i < 60; ++i)
    {
        mUnidentified000[i] = 0;
    }

    Function<void*> callback(fn_8017472C);
    UnidentifiedEventBase** foundEvent;
    unsigned int hash = HashEventName("ResetEffects", -1);
    foundEvent = 0;
    g_pEventRegistry->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
    ((UnidentifiedTypedEvent<void>*)event)->Add(callback, (unsigned int)&mUnidentified0F0, -1);
}

PhysicsPatchManager_801740D0::~PhysicsPatchManager_801740D0()
{
    ResetEffects();
}

PhysicsPatch* PhysicsPatchManager_801740D0::fn_801743A8(
    int type, cPlayer* owner, const nlVector3& position,
    const nlVector3& velocity, float startRadius, float endRadius,
    float lifetime)
{
    DebugWriteCache* log = gNetworkSyncState->GetWriteCache();
    if (log != 0)
    {
        int ownerID = owner == 0 ? -1 : owner->mUnidentified120;
        char buffer[200];
        nlSNPrintf(buffer, sizeof(buffer), "Creating patch %d owner %d r1 %f r2 %f life %f at frame %d\n", type, ownerID, startRadius, endRadius, lifetime, GetFixedUpdateTask()->GetFrame());
        log->WriteText(buffer);
    }

    PhysicsPatch* patch = 0;
    for (int i = 0; i < 60; ++i)
    {
        if (mUnidentified000[i] == 0)
        {
            patch = new PhysicsPatch();
            mUnidentified000[i] = patch;
            patch->m_Index = i;
            break;
        }
    }

    if (patch != 0)
    {
        nlVector3 initialVelocity = velocity;
        patch->SetPosition(position, PhysicsObject::WORLD_COORDINATES);
        patch->mUnidentified9C = position;
        patch->fn_80172EE0(&type);
        patch->m_Velocity = initialVelocity;
        patch->m_pOwner = owner;
        patch->m_fStartRadius = startRadius;
        patch->m_fEndRadius = endRadius;
        patch->m_fLifetime = lifetime;
        patch->Update(0.0f);
    }
    return patch;
}

PhysicsPatch* PhysicsPatchManager_801740D0::fn_801745B8(int index)
{
    if (index >= 0 && index < 60)
    {
        return mUnidentified000[index];
    }
    return 0;
}

void PhysicsPatchManager_801740D0::ResetEffects()
{
    for (int i = 0; i < 60; ++i)
    {
        if (mUnidentified000[i] != 0)
        {
            mUnidentified000[i]->Unknown0();
            delete mUnidentified000[i];
            mUnidentified000[i] = 0;
        }
    }
    PhysicsPatch::lbl_805705D0.FreeBlocks();
}

void PhysicsPatchManager_801740D0::Update(float dt)
{
    for (int i = 0; i < 60; ++i)
    {
        PhysicsPatch* patch = mUnidentified000[i];
        if (patch != 0)
        {
            if (patch->m_bKillMe == true)
            {
                delete patch;
                mUnidentified000[i] = 0;
            }
            patch->Update(dt);
        }
    }
}

extern "C" void fn_8017472C(void*)
{
    for (int i = 0; i < 60; ++i)
    {
        PhysicsPatch* patch = lbl_806E12C8->fn_801745B8(i);
        if (patch != 0)
        {
            patch->UnidentifiedKillEffect();
        }
    }

    for (int j = 0; j < 13; ++j)
    {
        int type = j;
        PhysicsPatchInfo* info = GetPhysicsPatchInfo(type);
        if (info->mEffectName != 0 && nlStrLen(info->mEffectName) != 0)
        {
            EffectsGroup* effects = EmissionManager::Instance()->GetEffectsGroup(info->mEffectName);
            if (effects != 0)
            {
                EmissionManager::Instance()->Destroy(effects);
            }
        }
    }
}

void PhysicsPatchManager_801740D0::fn_801748A0(
    void* context, DebugWriteCache* cache)
{
    PhysicsPatch* patch;
    unsigned int offset;
    for (int i = 0; i < 60; ++i)
    {
        patch = mUnidentified000[i];
        if (patch == 0)
        {
            continue;
        }

        if (lbl_806DCAB8 == 0xFFFF)
        {
            patch->RegisterDebugFields(&lbl_806DCAB8, cache);
        }

        offset = (unsigned char*)&patch->m_Type - (unsigned char*)patch;
        void* data = cache->WriteData(lbl_806DCAB8, (unsigned char*)patch + offset, sizeof(PhysicsPatch) - offset);
        if (data != 0)
        {
            PhysicsPatch* copy = (PhysicsPatch*)((unsigned char*)data - offset);
            cPlayer* owner = patch->m_pOwner;
            copy->m_pOwner = (cPlayer*)(owner == 0 ? -1 : owner->mUnidentified120);
            cPlayer* target = patch->m_pTarget;
            copy->m_pTarget = (cPlayer*)(target == 0 ? -1 : target->mUnidentified120);
            cache->ChecksumData(lbl_806DCAB8, data, context);
        }
    }
}

static void fn_8017498C(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (controller.m_Replaying == 0)
    {
        PhysicsPatch* patch = (PhysicsPatch*)controller.m_uUserData;
        if (patch->m_bVisible == true)
        {
            controller.SetPosition(patch->mUnidentified9C);
            controller.SetVelocity(patch->m_Velocity);
        }
    }
}
