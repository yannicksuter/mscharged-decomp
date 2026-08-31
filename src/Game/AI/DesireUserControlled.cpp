#include "Game/AI/DesireUserControlled.h"

#include "Game/AI/DesireSteering.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/ShotMeter.h"
#include "Game/Ball.h"
#include "Game/Game.h"
#include <stddef.h>

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

extern "C" DebugFieldType lbl_80533C98[];
extern "C" unsigned short fn_80338EBC(
    DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void* fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);
extern "C" void fn_80098098(cFielder*);
extern "C" void fn_80040368(cFielder*);
extern "C" void fn_80095870(cFielder*);
extern "C" PlayerTweaks* fn_8003E6E4(cFielder*);
extern "C" float fn_8002C254(PlayerTweaks*);
extern "C" float fn_8002CE14(PlayerTweaks*);
extern "C" float fn_8002C328(PlayerTweaks*);
extern "C" void fn_8003C268(cFielder*, float, float);
extern "C" void fn_8003DA94(cFielder*, bool);
extern "C" bool fn_8003E948(cFielder*);
extern "C" void fn_800368E4(cFielder*);
extern "C" void fn_8003E0A8(cFielder*);
extern "C" bool fn_80035F34(cFielder*);
extern "C" bool fn_80331C04(cGlobalPad*, int, bool);
extern "C" void fn_800B6A1C(void*, int, const Variant&);
extern "C" void fn_8003E168(cFielder*, float);

extern float g_fSimulationTick;

static unsigned short sDesireUserControlledType = 0xFFFF;
static unsigned int lbl_806DC3AC = 0x20;

/**
 * Offset/Address/Size: 0x0 | 0x800D448C | size: 0x100
 */
bool DesireUserControlled::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);

    DesireSteering* desire = (DesireSteering*)fn_8002E08C(
        mUnidentifiedFielder, 34);
    fn_800C5784(desire);
    fn_800C574C(desire);
    fn_800C577C(desire);

    mUnidentified078 = -1.0f;

    UnidentifiedVariant_80054AB8 update;
    UnidentifiedUpdate(
        (UnidentifiedDesireUpdate*)&update, g_fSimulationTick);

    return result;
}

/**
 * Offset/Address/Size: 0x100 | 0x800D458C | size: 0x6BC
 */
void DesireUserControlled::UnidentifiedUpdate(
    UnidentifiedDesireUpdate* update, float fDeltaT)
{
    bool bHasPad = (bool)mUnidentifiedFielder->GetGlobalPad();
    if (!bHasPad)
    {
        *update = FuzzyVariant(FT_INT, 1);
        update->mTemporary = false;
        return;
    }

    if (g_pGame->m_eGameState == 1)
    {
        return;
    }
    else if (mUnidentifiedFielder->m_eActionState == ACTION_SHOOT_TO_SCORE)
    {
        return;
    }
    else if (mUnidentifiedFielder->m_eActionState == ACTION_SHOT)
    {
        return;
    }
    else
    {
        fn_80098098(mUnidentifiedFielder);
        if (mUnidentifiedFielder->m_eActionState == ACTION_NEED_ACTION
            || mUnidentifiedFielder->m_eActionState == ACTION_WAIT)
        {
            fn_80040368(mUnidentifiedFielder);
        }

        fn_80095870(mUnidentifiedFielder);
        if (mUnidentifiedFielder->m_eActionState == ACTION_RUNNING)
        {
            float fMaxSpeed = fn_8002C254(
                fn_8003E6E4(mUnidentifiedFielder));
            float fMinSpeed = fn_8002CE14(
                fn_8003E6E4(mUnidentifiedFielder));
            fn_8003C268(mUnidentifiedFielder, fMinSpeed, fMaxSpeed);
            fn_8003DA94(mUnidentifiedFielder, false);

            if (g_pBall->m_pOwner == NULL
                && (!fn_8003E948(mUnidentifiedFielder)
                    || !mUnidentifiedFielder->mUnidentified3DC))
            {
                fn_800368E4(mUnidentifiedFielder);
            }
            return;
        }

        if (mUnidentifiedFielder->m_eActionState == ACTION_UNKNOWN_30)
        {
            fn_8003E0A8(mUnidentifiedFielder);
            if (mUnidentifiedFielder->m_eActionState != ACTION_UNKNOWN_30)
            {
                return;
            }
            if (mUnidentifiedFielder->m_pBall == NULL)
            {
                return;
            }

            mUnidentifiedFielder->mActionShotVars.bIsChipShot
                = fn_80035F34(mUnidentifiedFielder);
            u8 bIsShotActive = true;
            ShotMeter* pShotMeter = mUnidentifiedFielder->m_pShotMeter;
            eShotMeterState state = pShotMeter->m_eShotMeterState;
            if (state != SHOT_METER_ACTIVE
                && state != SHOT_METER_STS_ACTIVE)
            {
                bIsShotActive = false;
            }
            if (bIsShotActive)
            {
                if (!fn_80331C04(
                        mUnidentifiedFielder->GetGlobalPad(), 0x1C, true))
                {
                    mUnidentifiedFielder->fn_8004B86C(
                        mUnidentifiedFielder->mActionShotVars.bIsChipShot,
                        false);
                }
                return;
            }

            if (pShotMeter->m_eShotMeterState == SHOT_METER_RELEASED
                || pShotMeter->m_eShotMeterState
                    == SHOT_METER_STS_RELEASED)
            {
                mUnidentifiedFielder->fn_8004B86C(
                    mUnidentifiedFielder->mActionShotVars.bIsChipShot,
                    false);
                return;
            }

            if (pShotMeter->m_eShotMeterState
                == SHOT_METER_STS_TRANSISTION)
            {
                *update = FuzzyVariant(FT_INT, 3);
                update->mTemporary = false;
                fn_800B6A1C(
                    update, 8, FuzzyVariant(FT_INT, lbl_806DC3AC));
            }
            return;
        }

        if (mUnidentifiedFielder->m_eActionState == ACTION_RUNNING_WB)
        {
            float fMaxSpeed = fn_8002C328(
                fn_8003E6E4(mUnidentifiedFielder));
            float fMinSpeed = fn_8002CE14(
                fn_8003E6E4(mUnidentifiedFielder));
            fn_8003C268(mUnidentifiedFielder, fMinSpeed, fMaxSpeed);
            fn_8003E168(mUnidentifiedFielder, fDeltaT);
        }
    }
}

/**
 * Offset/Address/Size: 0x7BC | 0x800D4C48 | size: 0x4
 */
void DesireUserControlled::UnidentifiedCleanup()
{
}

/**
 * Offset/Address/Size: 0x7C0 | 0x800D4C4C | size: 0xC8
 */
void DesireUserControlled::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field
        = fn_80338EBC(cache, "DesireUserControlled");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition,
        "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition,
        "mThinkTimer");
    fn_80338F78(cache);
}

/**
 * Offset/Address/Size: 0x888 | 0x800D4D14 | size: 0x9C
 */
void DesireUserControlled::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireUserControlledType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireUserControlledType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    fn_80339450(cache, sDesireUserControlledType, data, context);
    fn_8033930C(cache, sDesireUserControlledType, data,
        sizeof(DesireUserControlled) - offset);
}

/**
 * Offset/Address/Size: 0x924 | 0x800D4DB0 | size: 0x5C
 */
DesireUserControlled::~DesireUserControlled()
{
}
