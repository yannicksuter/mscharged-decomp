#include "Game/AI/DesireUsePowerup.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/DebugWriteCache.h"
#include "Game/Team.h"
#include <stddef.h>

extern "C" bool fn_8002EDC8(cFielder*, int);
static unsigned short sDesireUsePowerupType = 0xFFFF;

/**
 * Offset/Address/Size: 0xF48 | 0x800D2FBC | size: 0x128
 */
bool DesireUsePowerup::UnidentifiedInitialize(void* context)
{
    cFielder* pTarget;
    cTeam* pTeam;
    bool result;
    ePowerUpType ePowerup;
    UnidentifiedVariantCollection* params;

    result = Desire::UnidentifiedInitialize(context);
    params = (UnidentifiedVariantCollection*)context;

    mbThrowingPowerup = false;
    mePowerup = POWER_UP_NONE;
    mnNumPowerups = 0;
    mpTarget = NULL;
    mtPowerupEffectTime.m_unk0
        = mtPowerupEffectTime.m_uPackedTime != 0;
    mtPowerupEffectTime.m_uPackedTime = 0;
    mUnidentifiedFielder->m_nPowerupAnimID = -1;
    mUnidentified078 = -1.0f;

    if (params->IsSet(15))
    {
        ePowerup = (ePowerUpType)params->Get(15)->mData.i;
        pTarget = (cFielder*)params->Get(14)->mData.pPlayer;

        if (fn_8002EDC8(mUnidentifiedFielder, -1))
        {
            pTeam = mUnidentifiedFielder->m_pTeam;
            if (ePowerup != POWER_UP_NONE
                && ePowerup != pTeam->GetCurrentPowerUp().eType)
            {
                pTeam->TogglePowerup(false);
            }

            fn_800D3A50(
                pTeam->GetCurrentPowerUp().eType,
                pTeam->GetCurrentPowerUp().nnumOfPowerups,
                pTarget);
        }
    }

    return result;
}

/**
 * Offset/Address/Size: 0x1820 | 0x800D3894 | size: 0x3C
 */
void DesireUsePowerup::UnidentifiedCleanup()
{
    mbThrowingPowerup = false;
    mePowerup = POWER_UP_NONE;
    mnNumPowerups = 0;
    mpTarget = NULL;
    mtPowerupEffectTime.m_unk0
        = mtPowerupEffectTime.m_uPackedTime != 0;
    mtPowerupEffectTime.m_uPackedTime = 0;
    mUnidentifiedFielder->m_nPowerupAnimID = -1;
}

/**
 * Offset/Address/Size: 0x2160 | 0x800D41D4 | size: 0x17C
 */
void DesireUsePowerup::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field
        = cache->BeginType("DesireUsePowerup");
    cache->AddField(22, gDebugFieldTypes[22].size,
        0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition,
        "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition,
        "mThinkTimer");
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&mpTarget - (u8*)&mvDesiredPosition, "mpTarget");
    cache->AddField(16, gDebugFieldTypes[16].size,
        (u8*)&mbThrowingPowerup - (u8*)&mvDesiredPosition,
        "mbThrowingPowerup");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&mePowerup - (u8*)&mvDesiredPosition, "mePowerup");
    cache->AddField(8, gDebugFieldTypes[8].size,
        (u8*)&mnNumPowerups - (u8*)&mvDesiredPosition,
        "mnNumPowerups");
    cache->AddField(20, gDebugFieldTypes[20].size,
        (u8*)&mtPowerupEffectTime - (u8*)&mvDesiredPosition,
        "mtPowerupEffectTime");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x22DC | 0x800D4350 | size: 0xC0
 */
void DesireUsePowerup::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireUsePowerupType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireUsePowerupType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(sDesireUsePowerupType,
        (u8*)this + offset, sizeof(DesireUsePowerup) - offset);
    if (data != NULL)
    {
        DesireUsePowerup* copy
            = (DesireUsePowerup*)((u8*)data - offset);
        *(int*)&copy->mpTarget
            = mpTarget == NULL ? -1 : mpTarget->mUnidentified120;
        cache->ChecksumData(sDesireUsePowerupType, data, context);
    }
}

/**
 * Offset/Address/Size: 0x239C | 0x800D4410 | size: 0x5C
 */
DesireUsePowerup::~DesireUsePowerup()
{
}
