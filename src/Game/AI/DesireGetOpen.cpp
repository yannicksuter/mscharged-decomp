#include "Game/AI/Desire.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/SpaceSearch.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Ball.h"
#include "Game/Team.h"
#include "NL/nlMemory.h"
#include "Game/DebugWriteCache.h"

float gGetOpenSearchRadius = 6.0f;
static unsigned short sDesireGetOpenType = 0xFFFF;
// The default transition result resides in initialized small data.
#pragma explicit_zero_data on
int gTransDesireGetOpenContinue = DESIRE_CONTINUE;
#pragma explicit_zero_data off
bool gGetOpenSearchDebug;

bool DesireGetOpen::Initialize(void* context)
{
    bool initialized = Desire::Initialize(context);
    nlVector3 formationPosition;
    if (m_pFielder->CalculateFormationPosition(formationPosition))
    {
        formationPosition = m_pFielder->mUnidentified024.m_v3Position;
    }

    cFielder* ballCarrier = fn_800DF790(m_pFielder->m_pTeam);
    nlVector3 bestPosition = formationPosition;
    nlVector3 targetPosition = ballCarrier != NULL
        ? ballCarrier->mUnidentified024.m_v3Position
        : g_pBall->m_v3Position;
    targetPosition.z = 0.0f;
    if (ballCarrier == m_pFielder)
    {
        SSearchGetOpen* search = new (nlMalloc(sizeof(SSearchGetOpen), 8, false))
            SSearchGetOpen(m_pFielder);
        mpSpaceSearch = search;
        m_pFielder->SetSpaceSearch(search);
        m_pFielder->m_pSpaceSearch->m_bDebugOn = gGetOpenSearchDebug;
        m_pFielder->m_pSpaceSearch->FindBestPosition(
            bestPosition, formationPosition, DIR_NONE, 0, gGetOpenSearchRadius, 0x8000);
    }
    else
    {
        SSearchOpenLane* search = new (nlMalloc(sizeof(SSearchOpenLane), 8, false))
            SSearchOpenLane(ballCarrier, m_pFielder);
        mpSpaceSearch = search;
        m_pFielder->SetSpaceSearch(search);
        m_pFielder->m_pSpaceSearch->m_bDebugOn = gGetOpenSearchDebug;
        m_pFielder->m_pSpaceSearch->FindBestPosition(
            bestPosition, formationPosition, DIR_TOWARD_TARGET, &targetPosition, gGetOpenSearchRadius, 0x8000);
    }
    mvDesiredPosition = bestPosition;
    return initialized;
}

void DesireGetOpen::Update(DesireUpdate*, float)
{
    m_pFielder->AddDesiredPosition(mvDesiredPosition, 1.2f, 1.0f);
}

void DesireGetOpen::Cleanup()
{
    if (mpSpaceSearch == m_pFielder->m_pSpaceSearch)
    {
        m_pFielder->SetSpaceSearch(0);
    }
    mpSpaceSearch = 0;
}

DesireUpdate TransDesireGetOpen(AIContext* input)
{
    DesireUpdate result(FT_INT, gTransDesireGetOpenContinue);
    cFielder* fielder = (cFielder*)input->mData.pPlayer;
    cFielder* ballCarrier = fn_800DF790(fielder->m_pTeam);
    if (fielder->m_pBall != 0)
    {
        result = DESIRE_FINISHED;
    }
    else if (ballCarrier == 0 || fielder->m_pTeam->GetBestBallInterceptor() == fielder)
    {
        result = 4;
    }
    return result;
}

inline void DesireGetOpen::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireGetOpen");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

inline void DesireGetOpen::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (sDesireGetOpenType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireGetOpenType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireGetOpenType, data, context);
    cache->WriteData(sDesireGetOpenType, data, sizeof(DesireGetOpen) - offset);
}

inline DesireGetOpen::~DesireGetOpen()
{
}
