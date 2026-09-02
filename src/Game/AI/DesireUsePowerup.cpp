#include "Game/AI/DesireUsePowerup.h"

#include "Game/AI/Fielder.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/Team.h"
#include <stddef.h>

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

extern "C" DebugFieldType lbl_80533C98[];
extern "C" bool fn_8002EDC8(cFielder*, int);
extern "C" bool fn_800A65E8(cTeam*, bool);
extern "C" unsigned short fn_80338EBC(
    DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void* fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);

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
                fn_800A65E8(pTeam, false);
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
        = fn_80338EBC(cache, "DesireUsePowerup");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition,
        "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition,
        "mThinkTimer");
    fn_80338F88(cache, 15, lbl_80533C98[15].size,
        (u8*)&mpTarget - (u8*)&mvDesiredPosition, "mpTarget");
    fn_80338F88(cache, 16, lbl_80533C98[16].size,
        (u8*)&mbThrowingPowerup - (u8*)&mvDesiredPosition,
        "mbThrowingPowerup");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&mePowerup - (u8*)&mvDesiredPosition, "mePowerup");
    fn_80338F88(cache, 8, lbl_80533C98[8].size,
        (u8*)&mnNumPowerups - (u8*)&mvDesiredPosition,
        "mnNumPowerups");
    fn_80338F88(cache, 20, lbl_80533C98[20].size,
        (u8*)&mtPowerupEffectTime - (u8*)&mvDesiredPosition,
        "mtPowerupEffectTime");
    fn_80338F78(cache);
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
    void* data = fn_8033930C(cache, sDesireUsePowerupType,
        (u8*)this + offset, sizeof(DesireUsePowerup) - offset);
    if (data != NULL)
    {
        DesireUsePowerup* copy
            = (DesireUsePowerup*)((u8*)data - offset);
        *(int*)&copy->mpTarget
            = mpTarget == NULL ? -1 : mpTarget->mUnidentified120;
        fn_80339450(
            cache, sDesireUsePowerupType, data, context);
    }
}

/**
 * Offset/Address/Size: 0x239C | 0x800D4410 | size: 0x5C
 */
DesireUsePowerup::~DesireUsePowerup()
{
}
