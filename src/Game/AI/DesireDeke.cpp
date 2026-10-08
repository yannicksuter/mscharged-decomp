#include "Game/AI/Desire.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/Field.h"
#include "Game/Goalie.h"
#include "Game/MathHelpers.h"
#include "Game/Team.h"
#include "NL/nlMath.inl"
#include "Game/DebugWriteCache.h"

static unsigned short sDesireDekeType = 0xFFFF;

bool DesireDeke::Initialize(void* context)
{
    mpTarget = 0;
    if (((UnidentifiedVariantCollection*)context)->IsSet(14))
    {
        mpTarget = (cFielder*)((UnidentifiedVariantCollection*)context)->Get(14)->mData.pointer;
    }
    return true;
}

void DesireDeke::Update(DesireUpdate* update, float)
{
    if (m_pFielder->m_eActionState == 1)
    {
        return;
    }

    bool avoidSideline = false;
    unsigned short direction = m_pFielder->mUnidentified024.m_aActualFacingDirection;
    switch (m_pFielder->mUnidentified024.m_eCharacterClass)
    {
    case MARIO:
    case LUIGI:
        avoidSideline = true;
        if (m_pFielder->IsSuperPowerActive() && mpTarget != 0)
        {
            cFielder* target = mpTarget;
            nlVector3 delta;
            nlVec3Sub(delta, target->mUnidentified024.m_v3Position,
                m_pFielder->mUnidentified024.m_v3Position);
            direction = nlATan2Angle(delta.y, delta.x);
            break;
        }
        // Without an explicit target, use the nearest opponent below.

    case YOSHI:
    case 14:
    case 16:
    case 19:
    {
        avoidSideline = true;
        cPlayer* opponent = m_pFielder->fn_800966AC(0, true);
        if (opponent == 0)
        {
            *update = DESIRE_FINISHED;
            return;
        }
        Goalie* goalie = m_pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
        nlVector3 opponentPosition = opponent->mUnidentified024.m_v3Position;
        int opponentDirection = opponent->mUnidentified024.m_aActualMovementDirection;
        if (nlVec3DistanceSquared2D(goalie->mUnidentified024.m_v3Position,
                m_pFielder->mUnidentified024.m_v3Position)
            < nlVec3DistanceSquared2D(opponent->mUnidentified024.m_v3Position,
                m_pFielder->mUnidentified024.m_v3Position))
        {
            opponentPosition = goalie->mUnidentified024.m_v3Position;
            opponentDirection = goalie->mUnidentified024.m_aActualFacingDirection;
        }
        nlVector3 delta;
        nlVec3Sub(delta, m_pFielder->mUnidentified024.m_v3Position, opponentPosition);
        unsigned short away = nlATan2Angle(delta.y, delta.x);
        unsigned short reverseDirection = (opponentDirection += 0x8000);
        direction = reverseDirection + (s16)(s32)(0.5f * (s16)(away - opponentDirection));
        break;
    }

    case PEACH:
    case DIDDYKONG:
    case 15:
    {
        avoidSideline = true;
        cPlayer* opponent = m_pFielder->fn_800966AC(0, true);
        if (opponent == 0)
        {
            *update = DESIRE_FINISHED;
            return;
        }
        nlVector3 opponentDelta;
        nlVec3Sub(opponentDelta, opponent->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        unsigned short opponentAngle = nlATan2Angle(opponentDelta.y, opponentDelta.x);
        Goalie* goalie = m_pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
        nlVector3 goalieDelta;
        nlVec3Sub(goalieDelta, goalie->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        unsigned short goalieAngle = nlATan2Angle(goalieDelta.y, goalieDelta.x);
        float blend = InterpolateRangeClamped(1.0f, 0.0f, 2.5f, 0.66f,
            nlSqrt(nlVec3DistanceSquared2D(goalie->mUnidentified024.m_v3Position,
                m_pFielder->mUnidentified024.m_v3Position), true));
        direction = goalieAngle + (s16)(s32)(blend * (s16)(opponentAngle - goalieAngle));
        break;
    }

    case DAISY:
    case WALUIGI:
    case 17:
    {
        float dekeDistance = m_pFielder->GetDekeDistance();
        cNet* net = m_pFielder->m_pTeam->GetOtherNet();
        cFielder* fielder = m_pFielder;
        nlVector3 goalieDelta;
        nlVec3Sub(goalieDelta,
            fielder->m_pTeam->GetOtherTeam()->GetGoalie()->mUnidentified024.m_v3Position,
            fielder->mUnidentified024.m_v3Position);
        unsigned short goalieAngle = nlATan2Angle(goalieDelta.y, goalieDelta.x);
        nlVector3 goalDelta;
        nlVector3 goalLine = m_pFielder->mUnidentified024.m_v3Position;
        goalLine.x = cField::GetGoalLineX(1U);
        goalLine.x *= AIsgn(net->m_v3NetLocation.x);
        nlVec3Sub(goalDelta, goalLine, m_pFielder->mUnidentified024.m_v3Position);
        unsigned short goalAngle = nlATan2Angle(goalDelta.y, goalDelta.x);
        AvoidableObject* playerObject = m_pFielder->mUnidentified320;
        float goalDistanceLength = nlSqrt(nlVec3DistanceSquared2D(goalLine,
            m_pFielder->mUnidentified024.m_v3Position), true);
        float goalRange = goalDistanceLength - playerObject->GetRadius();
        float blend = InterpolateRangeClamped(1.0f, 0.0f,
            0.8f * dekeDistance, 2.0f * dekeDistance, goalRange);
        Goalie* goalie = m_pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
        float goalieRange = nlSqrt(nlVec3DistanceSquared2D(
            m_pFielder->mUnidentified024.m_v3Position,
            goalie->mUnidentified024.m_v3Position), true);
        goalie = m_pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
        playerObject = m_pFielder->mUnidentified320;
        float goalieRadius = goalie->mUnidentified320->GetRadius();
        float playerRadius = playerObject->GetRadius();
        if (goalieRange + (playerRadius + goalieRadius) > dekeDistance)
        {
            blend = 0.0f;
        }
        if (goalRange < dekeDistance)
        {
            blend = 1.0f;
        }
        direction = goalAngle + (s16)(s32)(blend * (s16)(goalieAngle - goalAngle));
        break;
    }

    case BOWSER:
    case DONKEYKONG:
    case WARIO:
    case BOWSERJR:
    case PETEY:
    case BIRDO:
    case 13:
    case 18:
    {
        cPlayer* opponent = m_pFielder->fn_800966AC(0, true);
        if (opponent == 0)
        {
            *update = DESIRE_FINISHED;
            return;
        }
        nlVector3 delta;
        nlVec3Sub(delta, opponent->mUnidentified024.m_v3Position,
            m_pFielder->mUnidentified024.m_v3Position);
        direction = nlATan2Angle(delta.y, delta.x);
        break;
    }
    }

    if (avoidSideline)
    {
        nlVector2 sidelineDirection;
        if (CloseToSideline(m_pFielder->mUnidentified024.m_v3Position, 0, false, &sidelineDirection) > 0.2f)
        {
            unsigned short sidelineAngle = nlATan2Angle(sidelineDirection.y, sidelineDirection.x);
            short delta = sidelineAngle - direction;
            unsigned short angleDistance = delta < 0 ? -delta : delta;
            if (angleDistance < 0x2000)
            {
                direction += 0x8000;
            }
        }
    }
    m_pFielder->fn_800447C0(direction);
}

void DesireDeke::Cleanup()
{
    m_pFielder->mUnidentified1E4.m_eLastPadAction = 50;
}

DesireDeke::~DesireDeke()
{
}

inline void DesireDeke::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireDeke");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

inline void DesireDeke::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (sDesireDekeType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireDekeType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireDekeType, data, context);
    cache->WriteData(sDesireDekeType, data, sizeof(DesireDeke) - offset);
}
