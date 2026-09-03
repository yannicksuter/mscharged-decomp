#include "Game/Physics/PhysicsPatch.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Powerups.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Event.h"
#include "Game/Field.h"
#include "Game/FixedUpdateTask.h"
#include "Game/Game.h"
#include "Game/Physics/CollisionSpace.h"
#include "Game/Player.h"
#include "NL/nlAVLTree.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"

#include <math.h>

class EffectsGroup;

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

extern CollisionSpace* g_CollisionSpace;
extern UnidentifiedEventRegistry* lbl_806E1D90;
extern "C" FixedUpdateTask* fn_8011166C();
extern "C" EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);
extern "C" EmissionController* fn_802E7FE4(
    EmissionManager*, EffectsGroup*, int, bool, bool);
extern "C" bool fn_800EBBFC(
    int, unsigned long, const char*, void*);
extern "C" void* fn_80338950(void*);
extern "C" void fn_8033919C(void*, const char*);
extern "C" void* lbl_806E2488;
extern "C" DebugFieldType lbl_80533C98[];
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void* fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);

extern "C" void fn_8017472C(void*);

class Unidentified_804F4780 : public unk_8000CD44
{
public:
    Unidentified_804F4780(PhysicsPatch* patch)
        : unk_8000CD44(0x20)
        , mUnidentified14(patch)
    {
    }
    virtual ~Unidentified_804F4780();

    /* 0x14 */ PhysicsPatch* mUnidentified14;
    /* 0x18 */ unsigned char mUnidentified18[0x0C];
}; // total size: 0x24

static const nlVector3 lbl_804DCCA0 = { 0.0f, 0.0f, 0.0f };
static const nlVector3 lbl_804DCCAC = { -2.0f, 0.0f, -5.0f };

unsigned short lbl_806DCAB8 = 0xFFFF;
float lbl_806DCABC = 0.25f;

SlotPool<PhysicsPatch> PhysicsPatch::lbl_805705D0(16, 16);

static void fn_8017498C(EmissionController& controller)
{
    if (g_pGame != 0)
    {
        if (g_pGame->m_eGameState == 4)
        {
            return;
        }
        if (controller.m_GlView == 0)
        {
            PhysicsPatch* patch
                = (PhysicsPatch*)controller.m_uUserData;
            if (patch->m_bVisible == true)
            {
                controller.SetPosition(patch->mUnidentified9C);
                controller.SetVelocity(patch->m_Velocity);
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
    m_Type = *type;
    m_fLifetime = 0.0f;
    m_fCurtime = 0.0f;
    m_bKillMe = false;
    m_bVisible = true;
    m_fEndRadiusTime = 1.0f;
    m_pTarget = 0;
    m_fStartRadiusTime = 0.0f;

    UnidentifiedPhysicsPatchInfo_80510BF0* info = fn_80174ED4(&m_Type);
    if (info == 0)
    {
        mUnidentified44 = 0;
        return;
    }

    SetCollide(info->mUnidentified0C);
    EnableCollisions();
    m_Gravity = info->mUnidentified14;
    fn_800EBBFC(10, info->mUnidentified10, 0, 0);

    if (info->mUnidentified08 != 0 && info->mUnidentified08[0] != '\0')
    {
        EffectsGroup* effects
            = fn_802E7CDC(EmissionManager::Instance(), info->mUnidentified08);
        if (effects != 0)
        {
            int view = m_Type < 8 ? 3 : 2;
            EmissionController* controller = fn_802E7FE4(
                EmissionManager::Instance(), effects, view, true, false);
            controller->SetPosition(GetPosition());
            controller->m_uUserData = (unsigned long)this;
            controller->SetUpdateCallback(
                Function1<void, EmissionController&>(fn_8017498C));
            controller->m_fGround = 2.25f;
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
            = new (nlMalloc(sizeof(Unidentified_804F4780), 8, false))
                Unidentified_804F4780(this);
        break;
    default:
        mUnidentified44 = 0;
        break;
    }
}

void PhysicsPatch::Unknown0()
{
    UnidentifiedPhysicsPatchInfo_80510BF0* info = fn_80174ED4(&m_Type);
    if (info != 0 && info->mUnidentified08 != 0
        && info->mUnidentified08[0] != '\0')
    {
        EffectsGroup* effects = fn_802E7CDC(
            EmissionManager::Instance(), info->mUnidentified08);
        if (effects != 0)
        {
            EmissionManager::Instance()->Kill((unsigned long)this, effects);
        }
    }

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
    m_Velocity = lbl_804DCCA0;
    mUnidentified40 = 0;
    m_PathSpeed = 0.0f;
    m_PathPointCount = 0;
    m_CurrentPathPoint = 0;
    mUnidentified38.Clear();
}

ContactType PhysicsPatch::Contact(
    PhysicsObject* other, dContact*, int)
{
    if (other == 0)
    {
        return NO_CONTACT;
    }

    switch (other->GetObjectType())
    {
    case 0x1D:
        return m_Type == 1 ? ONE_WAY_CONTACT_THIS : NO_CONTACT;
    default:
        return NO_CONTACT;
    }
}

void PhysicsPatch::Update(float dt)
{
    if (m_bVisible && !m_bFrozen)
    {
        UnidentifiedPhysicsPatchInfo_80510BF0* info
            = fn_80174ED4(&m_Type);
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
            if (radius < 0.00001f)
            {
                radius = 0.00001f;
            }
            if (radius != GetRadius())
            {
                SetRadius(radius);
            }

            if (mUnidentified40 != 0)
            {
                fn_80173DA4(dt);
                return;
            }

            if (info != 0)
            {
                float damping = 1.0f
                              - InterpolateRangeClamped(
                                  info->mUnidentified18, 2.25f, 0.0f, 0.5f, dt);
                m_Velocity.x *= damping;
                m_Velocity.y *= damping;
                m_Velocity.z *= damping;
            }
            if (m_pTarget != 0)
            {
                fn_80173B18();
            }
            m_Velocity.z -= m_Gravity * dt;

            nlVector3 position = GetPosition();
            position.x += m_Velocity.x * dt;
            position.y += m_Velocity.y * dt;
            position.z += m_Velocity.z * dt;
            SetPosition(position, WORLD_COORDINATES);
        }
        else
        {
            Unknown0();
            m_bKillMe = true;
        }
    }
    else if (m_bFrozen)
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

    UnidentifiedPhysicsPatchInfo_80510BF0* info = fn_80174ED4(&m_Type);
    contact->surface.bounce = info->mUnidentified1C;
    contact->surface.mu = info->mUnidentified18;
    contact->surface.bounce_vel = 0.0f;
    return true;
}

void PhysicsPatch::fn_80173A10(float)
{
    if (m_Type == 4)
    {
        UnidentifiedPhysicsPatchInfo_80510BF0* info = fn_80174ED4(&m_Type);
        if (info != 0 && info->mUnidentified08 != 0
            && info->mUnidentified08[0] != '\0')
        {
            EffectsGroup* effects = fn_802E7CDC(
                EmissionManager::Instance(), info->mUnidentified08);
            if (effects != 0)
            {
                EmissionManager::Instance()->Destroy(
                    (unsigned long)this, effects);
            }
        }
        Unknown0();
    }
}

void PhysicsPatch::fn_80173AF4()
{
    m_pTarget = 0;
    m_TargetSeekSpeed = 0.6f;
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
    nlVector3 delta;
    delta.x = m_pTarget->m_v3Position.x - GetPosition().x;
    delta.y = m_pTarget->m_v3Position.y - GetPosition().y;
    float length = nlSqrt(delta.x * delta.x + delta.y * delta.y, true);
    if (length == 0.0f)
    {
        return;
    }

    delta.x /= length;
    delta.y /= length;
    nlVector3 desired;
    desired.x = m_TargetSeekSpeed * delta.x;
    desired.y = m_TargetSeekSpeed * delta.y;
    float speed = nlSqrt(
        m_Velocity.x * m_Velocity.x + m_Velocity.y * m_Velocity.y, true);
    desired.x += m_Velocity.x;
    desired.y += m_Velocity.y;
    float desiredLength
        = nlSqrt(desired.x * desired.x + desired.y * desired.y, true);
    if (desiredLength != 0.0f)
    {
        m_Velocity.x = speed * desired.x / desiredLength;
        m_Velocity.y = speed * desired.y / desiredLength;
    }
}

void PhysicsPatch::fn_80173C9C(
    nlVector3* points, int pointCount, double speed)
{
    float fVar1 = (float)speed;
    mUnidentified40 = points;
    m_PathPointCount = pointCount;
    m_PathSpeed = speed;
    m_CurrentPathPoint = 0;
    if (fVar1 < 0.0f)
    {
        m_CurrentPathPoint = pointCount - 1;
    }
}

nlVector3 PhysicsPatch::fn_80173CCC()
{
    nlVector3 direction = { 0.0f, 0.0f, 0.0f };
    if (mUnidentified40 != 0 && m_CurrentPathPoint > 0)
    {
        const nlVector3& position = GetPosition();
        direction.x = mUnidentified40[m_CurrentPathPoint].x - position.x;
        direction.y = mUnidentified40[m_CurrentPathPoint].y - position.y;
        direction.z = mUnidentified40[m_CurrentPathPoint].z - position.z;
        float scale = nlRecipSqrt(direction.GetLengthSq3D(), true);
        direction.x *= scale;
        direction.y *= scale;
        direction.z *= scale;
    }
    return direction;
}

void PhysicsPatch::fn_80173DA4(float dt)
{
    float remaining = dt * nlAbs(m_PathSpeed);
    nlVector3 position = GetPosition();
    bool finished = false;

    while (!finished)
    {
        const nlVector3& target = mUnidentified40[m_CurrentPathPoint];
        nlVector3 delta;
        delta.x = target.x - position.x;
        delta.y = target.y - position.y;
        delta.z = target.z - position.z;
        float distanceSquared = delta.GetLengthSq3D();
        if (distanceSquared > remaining * remaining)
        {
            float scale = nlRecipSqrt(distanceSquared, true);
            position.x += remaining * delta.x * scale;
            position.y += remaining * delta.y * scale;
            position.z += remaining * delta.z * scale;
            break;
        }

        int previousPoint = m_CurrentPathPoint;
        if (m_PathSpeed > 0.0f)
        {
            ++m_CurrentPathPoint;
        }
        else
        {
            --m_CurrentPathPoint;
        }

        if (m_CurrentPathPoint >= m_PathPointCount
            || m_CurrentPathPoint < 0)
        {
            position = mUnidentified40[previousPoint];
            finished = true;
            break;
        }

        remaining -= nlSqrt(distanceSquared, true);
    }

    if (__fpclassifyf(position.x) == 1
        || __fpclassifyf(position.y) == 1
        || __fpclassifyf(position.z) == 1)
    {
        position = lbl_804DCCA0;
    }
    SetPosition(position, WORLD_COORDINATES);

    if (finished && mUnidentified38)
    {
        mUnidentified38(this);
    }
}

PhysicsPatchManager_801740D0::PhysicsPatchManager_801740D0()
    : mUnidentified0F0(0)
{
    for (int i = 0; i < 60; ++i)
    {
        mUnidentified000[i] = 0;
    }

    Function<void*> callback(fn_8017472C);
    UnidentifiedEventBase** foundEvent;
    unsigned int hash = fn_802B289C("ResetEffects", -1);
    foundEvent = 0;
    lbl_806E1D90->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
    ((UnidentifiedTypedEvent<void>*)event)->Add(callback, (unsigned int)&mUnidentified0F0, -1);
}

PhysicsPatchManager_801740D0::~PhysicsPatchManager_801740D0()
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

    UnidentifiedConnection* connection
        = (UnidentifiedConnection*)mUnidentified0F0;
    if (connection != 0 && ((connection->mFlags >> 30) & 1) != 0)
    {
        ((UnidentifiedEventBase*)connection->mTarget)
            ->Disconnect(&mUnidentified0F0);
    }
}

PhysicsPatch* PhysicsPatchManager_801740D0::fn_801743A8(
    int type, cPlayer* owner, const nlVector3& position,
    const nlVector3& velocity, float startRadius, float endRadius,
    float lifetime)
{
    void* log = fn_80338950(lbl_806E2488);
    if (log != 0)
    {
        int ownerID = owner != 0 ? owner->mUnidentified120 : -1;
        char buffer[200];
        nlSNPrintf(buffer, sizeof(buffer), "Creating patch %d owner %d r1 %f r2 %f life %f at frame %d\n", type, ownerID, startRadius, endRadius, lifetime, fn_8011166C()->GetFrame());
        fn_8033919C(log, buffer);
    }

    int index;
    for (index = 0; index < 60; ++index)
    {
        if (mUnidentified000[index] == 0)
        {
            break;
        }
    }
    if (index == 60)
    {
        return 0;
    }

    PhysicsPatch* patch = 0;
    PhysicsPatch::lbl_805705D0.Allocate(patch);
    patch = new (patch) PhysicsPatch();
    mUnidentified000[index] = patch;
    patch->m_Index = index;
    patch->SetPosition(position, PhysicsObject::WORLD_COORDINATES);
    patch->mUnidentified9C = position;
    patch->fn_80172EE0(&type);
    patch->m_Velocity = velocity;
    patch->m_pOwner = owner;
    patch->m_fStartRadius = startRadius;
    patch->m_fEndRadius = endRadius;
    patch->m_fLifetime = lifetime;
    patch->Update(0.0f);
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
    if (lbl_806E12C8 == 0)
    {
        return;
    }

    for (int i = 0; i < 60; ++i)
    {
        PhysicsPatch* patch = lbl_806E12C8->fn_801745B8(i);
        if (patch != 0)
        {
            UnidentifiedPhysicsPatchInfo_80510BF0* info
                = fn_80174ED4(&patch->m_Type);
            if (info != 0 && info->mUnidentified08 != 0
                && info->mUnidentified08[0] != '\0')
            {
                EffectsGroup* effects = fn_802E7CDC(
                    EmissionManager::Instance(), info->mUnidentified08);
                if (effects != 0)
                {
                    EmissionManager::Instance()->Kill(
                        (unsigned long)patch, effects);
                }
            }
        }
    }
}

void PhysicsPatchManager_801740D0::fn_801748A0(
    void* context, DebugWriteCache* cache)
{
    for (int i = 0; i < 60; ++i)
    {
        PhysicsPatch* patch = mUnidentified000[i];
        if (patch == 0)
        {
            continue;
        }

        if (lbl_806DCAB8 == 0xFFFF)
        {
            patch->SyncLog(&lbl_806DCAB8, cache);
        }
        void* copy = fn_8033930C(cache, lbl_806DCAB8, &patch->m_Type, sizeof(PhysicsPatch) - 0x48);
        if (copy != 0)
        {
            PhysicsPatch* copiedPatch
                = (PhysicsPatch*)((unsigned char*)copy - 0x48);
            copiedPatch->m_pOwner = patch->m_pOwner == 0
                                      ? (cPlayer*)-1
                                      : (cPlayer*)patch->m_pOwner->mUnidentified120;
            copiedPatch->m_pTarget = patch->m_pTarget == 0
                                       ? (cPlayer*)-1
                                       : (cPlayer*)patch->m_pTarget->mUnidentified120;
            fn_80339450(cache, lbl_806DCAB8, copy, context);
        }
    }
}

void PhysicsPatch::SyncLog(void* context, DebugWriteCache* cache)
{
    *(unsigned short*)context = fn_80338EBC(cache, "PhysicsPatch");

#define REGISTER_FIELD(kind, field) \
    fn_80338F88(cache, kind, lbl_80533C98[kind].size, (unsigned char*)&field - (unsigned char*)&m_Type, #field)

    REGISTER_FIELD(14, m_Type);
    REGISTER_FIELD(15, m_pOwner);
    REGISTER_FIELD(17, m_fStartRadius);
    REGISTER_FIELD(17, m_fEndRadius);
    REGISTER_FIELD(17, m_fLifetime);
    REGISTER_FIELD(17, m_fCurtime);
    REGISTER_FIELD(8, m_Index);
    REGISTER_FIELD(16, m_bVisible);
    REGISTER_FIELD(16, m_bKillMe);
    REGISTER_FIELD(22, m_Velocity);
    REGISTER_FIELD(17, m_Gravity);
    REGISTER_FIELD(15, m_pTarget);
    REGISTER_FIELD(17, m_TargetSeekSpeed);
    REGISTER_FIELD(17, m_fStartRadiusTime);
    REGISTER_FIELD(17, m_fEndRadiusTime);
    REGISTER_FIELD(16, m_bFrozen);
    REGISTER_FIELD(17, m_FreezeTimer);
    REGISTER_FIELD(17, m_PathSpeed);
    REGISTER_FIELD(8, m_CurrentPathPoint);
    REGISTER_FIELD(8, m_PathPointCount);

#undef REGISTER_FIELD

    fn_80338F78(cache);
}
