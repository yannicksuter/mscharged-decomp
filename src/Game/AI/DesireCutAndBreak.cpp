#include "Game/AI/Desire.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/SpaceSearch.h"
#include "Game/Team.h"
#include "NL/nlMemory.h"
#include "Game/DebugWriteCache.h"

float lbl_806DC058 = 8.0f;
static unsigned short sDesireCutAndBreakType = 0xFFFF;
bool lbl_806E0E20;

bool DesireCutAndBreak::Initialize(void* context)
{
    bool initialized = Desire::Initialize(context);
    nlVector3 searchCenter;
    if (m_pFielder->CalculateFormationPosition(searchCenter))
    {
        searchCenter = m_pFielder->mUnidentified024.m_v3Position;
    }

    SSearchCutAndBreak* search = new (nlMalloc(sizeof(SSearchCutAndBreak), 8, false))
        SSearchCutAndBreak(m_pFielder);
    mUnidentifiedA4 = search;
    m_pFielder->SetSpaceSearch(search);
    m_pFielder->m_pSpaceSearch->m_bDebugOn = lbl_806E0E20;
    m_pFielder->m_pSpaceSearch->FindBestPosition(
        mvDesiredPosition, searchCenter, DIR_NONE, 0, lbl_806DC058, 0x8000);
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
    if (mUnidentifiedA4 == m_pFielder->m_pSpaceSearch)
    {
        m_pFielder->SetSpaceSearch(0);
    }
    mUnidentifiedA4 = 0;
}

DesireCutAndBreak::~DesireCutAndBreak()
{
}

inline void DesireCutAndBreak::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireCutAndBreak");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

inline void DesireCutAndBreak::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (sDesireCutAndBreakType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireCutAndBreakType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireCutAndBreakType, data, context);
    cache->WriteData(sDesireCutAndBreakType, data, sizeof(DesireCutAndBreak) - offset);
}
