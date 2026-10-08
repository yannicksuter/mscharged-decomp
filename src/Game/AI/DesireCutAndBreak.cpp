#include "Game/AI/Desire.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/SpaceSearch.h"
#include "Game/Team.h"
#include "NL/nlMemory.h"
#include "Game/DebugWriteCache.h"

float gCutAndBreakSearchRadius = 8.0f;
static unsigned short sDesireCutAndBreakType = 0xFFFF;
bool gCutAndBreakSearchDebug;

bool DesireCutAndBreak::Initialize(void* context)
{
    bool initialized = Desire::Initialize(context);
    nlVector3 searchCenter;
    if (m_pFielder->CalculateFormationPosition(searchCenter))
    {
        searchCenter = m_pFielder->m_DetChar.m_v3Position;
    }

    SSearchCutAndBreak* search = new (nlMalloc(sizeof(SSearchCutAndBreak), 8, false))
        SSearchCutAndBreak(m_pFielder);
    mpSpaceSearch = search;
    m_pFielder->SetSpaceSearch(search);
    m_pFielder->m_pSpaceSearch->m_bDebugOn = gCutAndBreakSearchDebug;
    m_pFielder->m_pSpaceSearch->FindBestPosition(
        mvDesiredPosition, searchCenter, DIR_NONE, 0, gCutAndBreakSearchRadius, 0x8000);
    return initialized;
}

void DesireCutAndBreak::Update(DesireUpdate* update, float)
{
    m_pFielder->AddDesiredPosition(mvDesiredPosition, 2.0f, 1.0f);
    if ((m_pFielder->GetDistanceToDesiredPos() < 0.5f
            || m_pFielder->m_pTeam->GetBestBallInterceptor() == m_pFielder)
        && update->mData.i == DESIRE_CONTINUE)
    {
        *update = 4;
    }
}

void DesireCutAndBreak::Cleanup()
{
    if (mpSpaceSearch == m_pFielder->m_pSpaceSearch)
    {
        m_pFielder->SetSpaceSearch(0);
    }
    mpSpaceSearch = 0;
}

DesireCutAndBreak::~DesireCutAndBreak()
{
}

inline void DesireCutAndBreak::RegisterDebugFields(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireCutAndBreak");
    Desire::RegisterDebugFields(field, cache);
    cache->EndType();
}

inline void DesireCutAndBreak::SyncLog(void* context, DebugWriteCache* cache)
{
    if (sDesireCutAndBreakType == 0xFFFF)
    {
        RegisterDebugFields(&sDesireCutAndBreakType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireCutAndBreakType, data, context);
    cache->WriteData(sDesireCutAndBreakType, data, sizeof(DesireCutAndBreak) - offset);
}
