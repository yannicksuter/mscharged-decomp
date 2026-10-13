#include "Game/AI/FielderDesireTypes.h"
#include "Game/AI/Desire.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidController.h"
#include "Game/AI/SkillTweaks.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Ball.h"
#include "Game/MathHelpers.h"
#include "Game/Team.h"
#include "NL/nlString.h"
#include "Game/DebugWriteCache.h"

#include "Game/SharedStaticStorage.h"

float gInterceptBallMaxPredictionTime = 3.0f;
static unsigned short sDesireInterceptBallType = DEBUG_TYPE_NONE;
int gInterceptBallSlideAttackState = FIELDER_DESIRE_SLIDE_ATTACK;

bool DesireInterceptBall::Initialize(void* context)
{
    bool initialized = Desire::Initialize(context);
    float blockPassChance = 0.0f;
    GetTeamSkillTweaks(g_pCurrentlyUpdatingTeam)->GetSkillValue(
        nlStringHash("Def/Block Pass Chance"), &blockPassChance, m_pFielder);
    meDesireSubState = 0;
    mbInterceptPass = IsPassInPlay() && nlRandomf(1.0f) < blockPassChance;
    return initialized;
}

void DesireInterceptBall::Update(DesireUpdate* update, float)
{
    if (update->mData.i == DESIRE_CHANGE)
    {
        switch (update->ExtraData.Get(8)->mData.i)
        {
        case FIELDER_DESIRE_SHOOT:
            m_pFielder->InitActionLooseBallShot(update->ExtraData.Get(16)->mData.b);
            *update = DESIRE_FINISHED;
            return;
        case FIELDER_DESIRE_PASS:
        {
            cFielder* target = static_cast<cFielder*>(update->ExtraData.Get(14)->mData.pPlayer);
            m_pFielder->InitActionLooseBallPass(target, OpenTo(m_pFielder, target) < 0.5f);
            *update = DESIRE_FINISHED;
            return;
        }
        }
    }

    if (Offensive(TeamOf(m_pFielder))
        || BallOffField(g_pBall) || BallOwner(m_pFielder))
    {
        *update = DESIRE_FINISHED;
        return;
    }

    nlVector3 position;
    cPlayer* passTarget = g_pBall->GetPassTarget();
    if (passTarget != NULL && passTarget->m_eClassType == FIELDER)
    {
        cFielder* target = static_cast<cFielder*>(passTarget);
        if (ReceivingVolleyPass(target))
        {
            if (g_pBall->GetPosition().z > m_pFielder->GetAirInterceptHeight(AIR_INTERCEPT_GROUND))
            {
                float interceptTime = m_pFielder->m_pTeam->mfBallInTimes[m_pFielder->m_DetPlayer.m_ID];
                float predictionTime = gInterceptBallMaxPredictionTime <= interceptTime ? gInterceptBallMaxPredictionTime : interceptTime;
                PredictBallPosition(g_pBall, &position, predictionTime);
            }
            else
            {
                target->GetApproachPosition(&position, &m_pFielder->m_DetChar.m_v3Position, 0.1f);
            }
        }
        else
        {
            position = GetClosestPointOnLineABFromPointC(g_pBall->m_v3Position,
                g_pBall->m_v3PassIntercept, m_pFielder->m_DetChar.m_v3Position);
            if (mbInterceptPass && AbleToSlideTackleBall(m_pFielder) >= 0.5f)
            {
                *update = DESIRE_CHANGE;
                update->SetParameter(8, FuzzyVariant(FT_INT, gInterceptBallSlideAttackState));
            }
        }
    }
    else if (g_pBall->GetOwnerFielder() != NULL)
    {
        g_pBall->GetOwnerFielder()->GetApproachPosition(&position,
            &m_pFielder->m_DetChar.m_v3Position, 0.25f);
    }
    else
    {
        float interceptTime = m_pFielder->m_pTeam->mfBallInTimes[m_pFielder->m_DetPlayer.m_ID];
        float predictionTime = gInterceptBallMaxPredictionTime <= interceptTime ? gInterceptBallMaxPredictionTime : interceptTime;
        PredictBallPosition(g_pBall, &position, predictionTime);
    }

    position.z = 0.0f;
    m_pFielder->AddDesiredPosition(position, 2.0f, 1.0f);
    AvoidController* avoidance = m_pFielder->GetAvoidController();
    avoidance->m_fRepulsionMult = 0.5f;
    if (g_pBall->m_pOwner != NULL && g_pBall->m_pOwner->m_eClassType == GOALIE)
    {
        *update = DESIRE_FINISHED;
    }
}

void DesireInterceptBall::Cleanup()
{
}

void DesireUpdate::SetParameter(int index, FuzzyVariant value)
{
    ExtraData.Set(index, value);
}

inline void DesireInterceptBall::RegisterDebugFields(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireInterceptBall");
    Desire::RegisterDebugFields(field, cache);
    cache->AddField(DEBUG_FIELD_ENUM, gDebugFieldTypes[DEBUG_FIELD_ENUM].size, (u8*)&meDesireSubState - (u8*)&mvDesiredPosition, "meDesireSubState");
    cache->AddField(DEBUG_FIELD_BOOL, gDebugFieldTypes[DEBUG_FIELD_BOOL].size, (u8*)&mbInterceptPass - (u8*)&mvDesiredPosition, "mbInterceptPass");
    cache->EndType();
}

inline void DesireInterceptBall::SyncLog(void* context, DebugWriteCache* cache)
{
    if (sDesireInterceptBallType == DEBUG_TYPE_NONE)
    {
        RegisterDebugFields(&sDesireInterceptBallType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireInterceptBallType, data, context);
    cache->WriteData(sDesireInterceptBallType, data, sizeof(DesireInterceptBall) - offset);
}

inline DesireInterceptBall::~DesireInterceptBall()
{
}
