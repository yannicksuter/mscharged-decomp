#include "Game/AI/Desire.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/AI/DesireSteering.h"
#include "Game/AI/SpaceSearch.h"
#include "Game/Ball.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/Team.h"
#include "Game/Task/FixedUpdateTask.h"
#include "NL/nlMath.inl"
#include "Game/DebugWriteCache.h"

static unsigned short sDesireGetInPositionType = 0xFFFF;
static unsigned short sDesireRunUpfieldType = 0xFFFF;
static unsigned short sDesireRunDownfieldType = 0xFFFF;
static unsigned short sDesireRunInDirectionType = 0xFFFF;
static unsigned short sDesireRunToTargetType = 0xFFFF;
nlVector2 gRunFieldInputRange = { 0.5f, 3.5f };
nlVector2 gRunFieldDistanceRange = { 4.0f, 1.0f };
// The default transition result resides in initialized small data.
#pragma explicit_zero_data on
int gTransDesireRunToTargetContinue = DESIRE_CONTINUE;
#pragma explicit_zero_data off

bool DesireGetInPosition::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    m_pFielder->StartRunning();
    return result;
}

void DesireGetInPosition::Update(DesireUpdate* update, float)
{
    nlVector3 position;
    if (m_pFielder->CalculateFormationPosition(position))
    {
        position = m_pFielder->mUnidentified024.m_v3Position;
    }
    m_pFielder->AddDesiredPosition(position, 0.8f, 1.0f);
    if (m_pFielder->m_pTeam->GetBestBallInterceptor() == m_pFielder
        && update->mData.i == DESIRE_CONTINUE)
    {
        *update = 4;
    }
}

bool DesireRunUpfield::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    m_pFielder->StartRunning();
    return result;
}

void DesireRunUpfield::Update(DesireUpdate* update, float)
{
    nlVector3 position;
    if (m_pFielder->CalculateFormationPosition(position) && g_pBall->GetOwnerGoalie() == 0)
    {
        position = m_pFielder->mUnidentified024.m_v3Position;
    }
    else
    {
        float distance = InterpolateRangeClamped(
            gRunFieldDistanceRange.x, gRunFieldDistanceRange.y, gRunFieldInputRange.x, gRunFieldInputRange.y,
            m_pFielder->mUnidentified1E4.m_v3AIPosition.x);
        position.x += distance * AIsgn(m_pFielder->m_pTeam->GetOtherNet()->m_v3NetLocation.x);
    }
    m_pFielder->AddDesiredPosition(position, 1.25f, 1.0f);
    if (m_pFielder->m_pTeam->GetBestBallInterceptor() == m_pFielder
        && update->mData.i == DESIRE_CONTINUE)
    {
        *update = 4;
    }
}

bool DesireRunDownfield::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    m_pFielder->StartRunning();
    return result;
}

void DesireRunDownfield::Update(DesireUpdate* update, float)
{
    nlVector3 position;
    if (m_pFielder->CalculateFormationPosition(position) && g_pBall->GetOwnerGoalie() == 0)
    {
        position = m_pFielder->mUnidentified024.m_v3Position;
    }
    else
    {
        float distance = InterpolateRangeClamped(
            gRunFieldDistanceRange.y, gRunFieldDistanceRange.x, gRunFieldInputRange.x, gRunFieldInputRange.y,
            m_pFielder->mUnidentified1E4.m_v3AIPosition.x);
        if (g_pBall->GetOwnerGoalie() != 0)
        {
            distance *= 2.0f;
        }
        position.x += distance * AIsgn(m_pFielder->m_pTeam->m_pNet->m_v3NetLocation.x);
    }
    m_pFielder->AddDesiredPosition(position, 1.25f, 1.0f);
    if (m_pFielder->m_pTeam->GetBestBallInterceptor() == m_pFielder
        && update->mData.i == DESIRE_CONTINUE)
    {
        *update = 4;
    }
}

bool DesireRunInDirection::Initialize(void* context)
{
    m_fSpeed = 1.0f;
    m_pTarget = 0;
    m_fDistTravelled = 0.0f;
    m_fMaxDistance = -1.0f;
    m_aDirection = 0;
    m_eFieldDirection = DIR_NONE;
    bool initialized = true;
    UnidentifiedVariantCollection* parameters = (UnidentifiedVariantCollection*)context;

    if (parameters->IsSet(14))
    {
        m_pTarget = (cFielder*)parameters->Get(14)->mData.pointer;
        if (m_pTarget == m_pFielder)
        {
            return false;
        }
    }
    if (parameters->IsSet(13))
    {
        m_fSpeed = parameters->Get(13)->mData.f;
    }
    if (parameters->IsSet(17))
    {
        if (m_pTarget != 0)
        {
            m_eFieldDirection = parameters->Get(17)->mData.i;
            m_aDirection = 0;
            if (m_eFieldDirection != DIR_TOWARD_TARGET && m_eFieldDirection != DIR_AWAYFROM_TARGET)
            {
                return false;
            }
        }
        else
        {
            m_aDirection = parameters->Get(17)->mData.i;
        }

        m_fMaxDistance = -1.0f;
        if (parameters->IsSet(18))
        {
            m_fMaxDistance = parameters->Get(18)->mData.f;
            mMaxDuration = 0.5f + m_fMaxDistance / m_pFielder->GetRunningSpeed();
        }
        m_pFielder->StartRunning();
    }
    else
    {
        initialized = false;
    }
    return initialized;
}

void DesireRunInDirection::Update(DesireUpdate* update, float deltaTime)
{
    if (m_pFielder->IsYoshiSuperPowerActive() && Incapacitated(m_pTarget))
    {
        *update = DESIRE_FINISHED;
        return;
    }
    if (update->mData.i != DESIRE_CONTINUE)
    {
        return;
    }
    if (m_fMaxDistance > 0.0f && m_fDistTravelled >= m_fMaxDistance)
    {
        *update = DESIRE_FINISHED;
        return;
    }

    if (m_pTarget != 0)
    {
        nlVector3 direction;
        nlVec3Sub(direction, m_pTarget->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        if (m_eFieldDirection == DIR_AWAYFROM_TARGET)
        {
            nlVec3Scale(direction, -1.0f);
        }
        m_aDirection = nlATan2Angle(direction.y, direction.x);
    }

    // Calculate the remaining distance; steering uses a fixed lookahead.
    float remainingDistance = nlMaxEquals(0.0f, m_fMaxDistance - m_fDistTravelled);

    nlVector3 direction;
    direction.z = 0.0f;
    nlPolar polar;
    polar.a = m_aDirection;
    polar.r = 1.0f;
    nlPolarToCartesian(direction, polar);
    nlVec3ScaleAdd(mvDesiredPosition, 5.0f, direction,
        m_pFielder->mUnidentified024.m_v3Position);
    m_pFielder->AddDesiredPosition(mvDesiredPosition, m_fSpeed, 1.0f);
    m_fDistTravelled += deltaTime * m_pFielder->mUnidentified024.m_fActualSpeed;
}

void DesireRunInDirection::Cleanup()
{
}

bool DesireRunToTarget::Initialize(void* context)
{
    bool initialized = true;
    m_pTargetFielder = 0;
    m_pTargetBall = 0;
    m_vTargetPos = v3Zero;
    m_eDirection = DIR_TOWARD_TARGET;
    m_fDistOffset = 0.0f;
    m_fSpeedCoeff = 1.0f;
    m_fUrgency = 2.0f;
    m_fAvoidanceCoeff = 1.0f;
    GetStateMachineAIContext(this)->SetTimer(1, 0.0f);
    UnidentifiedVariantCollection* parameters = (UnidentifiedVariantCollection*)context;

    if (parameters->IsSet(17))
    {
        m_eDirection = parameters->Get(17)->mData.i;
    }
    if (parameters->IsSet(13))
    {
        m_fSpeedCoeff = parameters->Get(13)->mData.f;
    }
    if (parameters->IsSet(18))
    {
        m_fDistOffset = parameters->Get(18)->mData.f;
    }
    if (parameters->IsSet(11))
    {
        m_fUrgency = parameters->Get(11)->mData.f;
    }
    if (parameters->IsSet(2))
    {
        m_fAvoidanceCoeff = parameters->Get(2)->mData.f;
    }

    if (parameters->IsSet(14) || parameters->IsSet(0))
    {
        FuzzyVariant* target = parameters->IsSet(14) ? parameters->Get(14) : parameters->Get(0);
        switch (target->GetType())
        {
        case FT_PLAYER:
            m_pTargetFielder = (cFielder*)target->mData.pointer;
            break;
        case FT_BALL:
            m_pTargetBall = (cBall*)target->mData.pointer;
            m_fUrgency = 4.0f;
            ResetSteeringHistory((DesireSteering*)GetFielderDesire(m_pFielder, 34));
            if (m_pFielder->m_pBall != 0)
            {
                initialized = false;
            }
            break;
        case FT_VECTOR:
            m_vTargetPos = target->mData.vector;
            break;
        default:
            initialized = false;
            break;
        }
    }
    else
    {
        initialized = false;
    }

    if (initialized)
    {
        DesireUpdate update;
        Update(&update, g_fSimulationTick);
    }
    return initialized;
}

static inline void PredictPosition(nlVector3& result, const cPlayer* player, float time)
{
    nlVec3ScaleAdd(result, time, player->mUnidentified024.m_v3Velocity,
        player->mUnidentified024.m_v3Position);
}

void DesireRunToTarget::Update(DesireUpdate* update, float)
{
    if (!GetStateMachineAIContext(this)->IsTimerRunning(1))
    {
        GetStateMachineAIContext(this)->SetTimer(1, 0.2f);
        mvDesiredPosition = m_vTargetPos;
        cPlayer* target = m_pTargetFielder;
        if (m_pTargetBall != 0)
        {
            cPlayer* owner = m_pTargetBall->GetOwner();
            if (owner == m_pFielder)
            {
                *update = DESIRE_FINISHED;
                return;
            }
            if (owner != 0)
            {
                target = owner;
            }
            else
            {
                target = 0;
                if (g_pBall->m_tShotTimer.m_uPackedTime == 0)
                {
                    mvDesiredPosition = m_pFielder->m_pTeam->GetBallInterceptPosition(
                        m_pFielder->mUnidentified1E4.m_ID);
                }
                else
                {
                    mvDesiredPosition = m_pFielder->mUnidentified024.m_v3Position;
                }
            }
        }

        if (target != 0)
        {
            AvoidableObject* targetObject = target->mUnidentified320;
            float distance = targetObject->GetRadius() + m_pFielder->mUnidentified320->GetRadius();
            distance += m_fDistOffset;
            if (target->m_eClassType == FIELDER
                && ((cFielder*)target)->IsInvincibleChars()
                && !m_pFielder->IsInvincibleChars()
                && !m_pFielder->IsSuperGrowActive())
            {
                distance += 0.75f * GetFielderHitReach(m_pFielder);
            }

            nlVector3 predicted;
            PredictPosition(predicted, target, 0.2f);
            nlVec3Sub(mvDesiredPosition, m_pFielder->mUnidentified024.m_v3Position, predicted);
            nlVec3Normalize(mvDesiredPosition, mvDesiredPosition);
            nlVec3ScaleAdd(mvDesiredPosition, distance, mvDesiredPosition, predicted);
        }

        mvDesiredPosition.z = 0.0f;
        if (m_eDirection == DIR_AWAYFROM_TARGET)
        {
            nlVector3 delta;
            nlVec3Sub(delta, mvDesiredPosition, m_pFielder->mUnidentified024.m_v3Position);
            nlVec3ScaleAdd(mvDesiredPosition, -1.0f, delta, m_pFielder->mUnidentified024.m_v3Position);
        }
    }

    m_pFielder->AddDesiredPosition(mvDesiredPosition, m_fUrgency * m_fSpeedCoeff, 1.0f);
    m_pFielder->SetAvoidanceMultiplier(m_fAvoidanceCoeff);
}

DesireUpdate TransDesireRunToTarget(AIContext* input, Desire* desire)
{
    DesireUpdate result(FT_INT, gTransDesireRunToTargetContinue);
    cFielder* fielder = (cFielder*)input->mData.pPlayer;
    nlVector2 delta = { fielder->mUnidentified024.m_v3Position.x - desire->GetDesiredPosition().x,
        fielder->mUnidentified024.m_v3Position.y - desire->GetDesiredPosition().y };
    if (nlVec2LengthSquared(delta) < 0.7f * 0.7f)
    {
        result = 4;
    }
    return result;
}

void DesireRunToTarget::Cleanup()
{
}

DesireGetInPosition::~DesireGetInPosition()
{
}

DesireRunUpfield::~DesireRunUpfield()
{
}

DesireRunDownfield::~DesireRunDownfield()
{
}

DesireRunInDirection::~DesireRunInDirection()
{
}

DesireRunToTarget::~DesireRunToTarget()
{
}

inline void DesireGetInPosition::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireGetInPosition");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

inline void DesireGetInPosition::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (sDesireGetInPositionType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireGetInPositionType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireGetInPositionType, data, context);
    cache->WriteData(sDesireGetInPositionType, data, sizeof(DesireGetInPosition) - offset);
}

inline void DesireRunUpfield::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireRunUpfield");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

inline void DesireRunUpfield::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (sDesireRunUpfieldType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireRunUpfieldType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireRunUpfieldType, data, context);
    cache->WriteData(sDesireRunUpfieldType, data, sizeof(DesireRunUpfield) - offset);
}

inline void DesireRunDownfield::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireRunDownfield");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

inline void DesireRunDownfield::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (sDesireRunDownfieldType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireRunDownfieldType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireRunDownfieldType, data, context);
    cache->WriteData(sDesireRunDownfieldType, data, sizeof(DesireRunDownfield) - offset);
}

inline void DesireRunInDirection::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireRunInDirection");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->AddField(19, gDebugFieldTypes[19].size, (u8*)&m_aDirection - (u8*)&mvDesiredPosition, "m_aDirection");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fMaxDistance - (u8*)&mvDesiredPosition, "m_fMaxDistance");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fDistTravelled - (u8*)&mvDesiredPosition, "m_fDistTravelled");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fSpeed - (u8*)&mvDesiredPosition, "m_fSpeed");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&m_eFieldDirection - (u8*)&mvDesiredPosition, "m_eFieldDirection");
    cache->AddField(15, gDebugFieldTypes[15].size, (u8*)&m_pTarget - (u8*)&mvDesiredPosition, "m_pTarget");
    cache->EndType();
}

inline void DesireRunInDirection::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (sDesireRunInDirectionType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireRunInDirectionType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(sDesireRunInDirectionType,
        (u8*)this + offset, sizeof(DesireRunInDirection) - offset);
    if (data != 0)
    {
        DesireRunInDirection* desire = (DesireRunInDirection*)((u8*)data - offset);
        desire->m_pTarget = (cFielder*)(m_pTarget == 0
                ? -1 : m_pTarget->mUnidentified120);
        cache->ChecksumData(sDesireRunInDirectionType, data, context);
    }
}

inline void DesireRunToTarget::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireRunToTarget");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->AddField(15, gDebugFieldTypes[15].size, (u8*)&m_pTargetFielder - (u8*)&mvDesiredPosition, "m_pTargetFielder");
    cache->AddField(22, gDebugFieldTypes[22].size, (u8*)&m_vTargetPos - (u8*)&mvDesiredPosition, "m_vTargetPos");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&m_eDirection - (u8*)&mvDesiredPosition, "m_eDirection");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fDistOffset - (u8*)&mvDesiredPosition, "m_fDistOffset");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fUrgency - (u8*)&mvDesiredPosition, "m_fUrgency");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fSpeedCoeff - (u8*)&mvDesiredPosition, "m_fSpeedCoeff");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&m_fAvoidanceCoeff - (u8*)&mvDesiredPosition, "m_fAvoidanceCoeff");
    cache->EndType();
}

inline void DesireRunToTarget::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (sDesireRunToTargetType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireRunToTargetType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(sDesireRunToTargetType,
        (u8*)this + offset, sizeof(DesireRunToTarget) - offset);
    if (data != 0)
    {
        DesireRunToTarget* desire = (DesireRunToTarget*)((u8*)data - offset);
        desire->m_pTargetFielder = (cFielder*)(m_pTargetFielder == 0
                ? -1 : m_pTargetFielder->mUnidentified120);
        cache->ChecksumData(sDesireRunToTargetType, data, context);
    }
}
