#include "Game/AI/Desire.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/Team.h"
#include "Game/DebugWriteCache.h"

static unsigned short sDesireHitType = 0xFFFF;

bool DesireHit::Initialize(void* context)
{
    bool initialized = Desire::Initialize(context);
    UserControlledT(m_pFielder->m_pTeam);
    m_pFielder->InitActionHit(
        (cFielder*)((UnidentifiedVariantCollection*)context)->Get(14)->mData.pPlayer,
        GetFielder()->GetActualFacing());
    return initialized;
}

void DesireHit::Update(DesireUpdate*, float)
{
}

DesireHit::~DesireHit()
{
}

inline void DesireHit::UnidentifiedVirtual8(void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireHit");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

inline void DesireHit::UnidentifiedVirtual7(void* context, DebugWriteCache* cache)
{
    if (sDesireHitType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireHitType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireHitType, data, context);
    cache->WriteData(sDesireHitType, data, sizeof(DesireHit) - offset);
}
