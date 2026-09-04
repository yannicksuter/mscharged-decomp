#include "Game/AI/Desire.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Fuzzy.h"
#include "Game/AI/FuzzyVariant.h"
#include <stddef.h>
#include "Game/AI/SpaceSearch.h"
#include "Game/Ball.h"
#include "Game/DebugWriteCache.h"
#include "Game/GameTweaks.h"
#include "Game/Player.h"
#include "Game/Team.h"

struct UnidentifiedPassCallbackValue
{
    FuzzyVariant mValue;
    float mConfidence;
    unsigned int mUnidentified018;
    UnidentifiedVariantCollection mExtraData;
};

extern "C" SkillTweaks* fn_800A636C(cTeam*);
extern "C" float fn_800D9EC4(cPlayer*);
extern "C" void fn_800401C0(
    cFielder*, const nlVector3&, float, float);
extern "C" float fn_8004028C(cFielder*);
extern "C" void* fn_80311734(void*);
extern "C" UnidentifiedVariant_80054AB8 fn_80041B0C(
    void*, cFielder*, const char*);
extern "C" float fn_800DCF18(cFielder*);
extern "C" void fn_800B6A1C(void*, int, const Variant&);
extern "C" PlayerTweaks* fn_8003E6E4(cFielder*);
extern "C" float fn_8002C328(PlayerTweaks*);
extern "C" bool fn_8002F858(cFielder*, bool);
extern "C" bool fn_80035F34(cFielder*);
extern cTeam* lbl_806E0E00;
extern bool lbl_806E0E38;
extern cFielder* g_pScriptCurrentFielder;
extern nlVector3 lbl_804DC190;

static float lbl_806DC130 = 4.5f;
static float lbl_806DC134 = 3.0f;
static float lbl_806DC138 = 0.85f;
static float lbl_806DC13C = 0.3f;
static float lbl_806DC140 = 1.0f;
static unsigned short lbl_806DC144 = 0xFFFF;
static unsigned short lbl_806DC146 = 0xFFFF;
static int lbl_806DC148 = 14;
static int lbl_806DC14C = 0;

/**
 * Offset/Address/Size: 0x0 | 0x800BA57C | size: 0x188
 */
bool DesirePreparePass::UnidentifiedInitialize(void* context)
{
    UnidentifiedVariantCollection* params =
        (UnidentifiedVariantCollection*)context;
    mpPassTarget = params->Get(14)->mData.pPlayer;
    mbVolleyPass = params->Get(16)->mData.b;
    if (mpPassTarget == 0)
    {
        return false;
    }

    float fDuration = lbl_806DC134;
    mUnidentified078 = fDuration + 0.2f;
    if (mbVolleyPass)
    {
        mThinkTimer.m_unk0 = mThinkTimer.m_uPackedTime != 0;
        mThinkTimer.m_uPackedTime = 0;
    }
    else
    {
        mThinkTimer.SetSeconds(fDuration);
    }

    SkillTweaks* pSkillTweaks = fn_800A636C(lbl_806E0E00);
    float fReactionTimeRange;
    float fReactionTime = 1.0f - pSkillTweaks->fn_800A3474(0);
    float fAbortThreshold = lbl_806DC138;
    fReactionTimeRange = lbl_806DC13C * fReactionTime;
    fReactionTimeRange = fAbortThreshold * fReactionTimeRange;
    mfAbortThreshold = fAbortThreshold
        + (nlRandomf(fReactionTimeRange)
            - (0.5f * fReactionTimeRange));

    m_pSpaceSearch = new (nlMalloc(sizeof(SSearchOpenLane), 8, false))
        SSearchOpenLane(mUnidentifiedFielder, mpPassTarget);
    mUnidentifiedFielder->SetSpaceSearch(m_pSpaceSearch);
    mUnidentifiedFielder->m_pSpaceSearch->m_bDebugOn = lbl_806E0E38;
    mUnidentifiedFielder->m_pSpaceSearch->FindBestPosition(
        mvDesiredPosition, mUnidentifiedFielder->m_v3Position,
        DIR_UPFIELD, &mpPassTarget->m_v3Position,
        lbl_806DC130, 0x8000);
    return true;
}

/**
 * Offset/Address/Size: 0x188 | 0x800BA704 | size: 0x6A0
 */
void DesirePreparePass::UnidentifiedUpdate(
    UnidentifiedDesireUpdate* update, float fDeltaT)
{
    if (fn_800D9EC4(mpPassTarget) != 0.0f)
    {
        *update = FuzzyVariant(FT_INT, 1);
        update->mTemporary = false;
        return;
    }

    mThinkTimer.Countdown(fDeltaT, 0.0f);
    fn_800401C0(
        mUnidentifiedFielder, mvDesiredPosition, 1.5f, 1.0f);

    bool bSwitchToPassDesire = false;
    if (mThinkTimer.m_uPackedTime != 0)
    {
        float fInDanger = fn_80041B0C(fn_80311734(this),
            mUnidentifiedFielder, "InDangerDelayed").mData.f;
        float fNotFarToTheirGoalie =
            FLESS(fn_800DCF18(g_pScriptCurrentFielder), 0.3f);
        float fDistanceToDesiredPos =
            fn_8004028C(mUnidentifiedFielder);
        float fClosingSpeedToDesiredPos = GetClosingSpeed2D(
            mvDesiredPosition, lbl_804DC190,
            mUnidentifiedFielder->m_v3Position,
            mUnidentifiedFielder->m_v3Velocity);
        if (fClosingSpeedToDesiredPos < 0.0f
            || fDistanceToDesiredPos <= lbl_806DC140
            || ((fInDanger >= fNotFarToTheirGoalie
                    ? fInDanger : fNotFarToTheirGoalie)
                >= mfAbortThreshold))
        {
            bSwitchToPassDesire = true;
        }
    }
    else
    {
        bSwitchToPassDesire = true;
    }

    if (bSwitchToPassDesire)
    {
        *update = FuzzyVariant(FT_INT, 3);
        update->mTemporary = false;
        fn_800B6A1C(
            update, 8, FuzzyVariant(FT_INT, lbl_806DC148));
        fn_800B6A1C(
            update, 14, FuzzyVariant(mpPassTarget));
        fn_800B6A1C(
            update, 16, FuzzyVariant(FT_BOOL, mbVolleyPass));
    }
}

/**
 * Offset/Address/Size: 0x828 | 0x800BADA4 | size: 0x50
 */
void DesirePreparePass::UnidentifiedCleanup()
{
    if (m_pSpaceSearch == mUnidentifiedFielder->m_pSpaceSearch)
    {
        mUnidentifiedFielder->SetSpaceSearch(0);
    }
    m_pSpaceSearch = 0;
}

/**
 * Offset/Address/Size: 0x878 | 0x800BADF4 | size: 0x164
 */
bool DesirePass::UnidentifiedInitialize(void* context)
{
    bool result = true;
    UnidentifiedVariantCollection* params =
        (UnidentifiedVariantCollection*)context;
    Variant* pParam;
    if (params->IsSet(14))
    {
        pParam = params->Get(14);
    }
    else
    {
        pParam = params->Get(0);
    }
    mpPassTarget = pParam->mData.pPlayer;
    if (params->IsSet(16))
    {
        pParam = params->Get(16);
    }
    else
    {
        pParam = params->Get(1);
    }
    mbVolleyPass = pParam->mData.b;
    if (mpPassTarget == 0)
    {
        return false;
    }

    PlayerTweaks* pTweaks = fn_8003E6E4(mUnidentifiedFielder);
    float fMaxSpeed = fn_8002C328(pTweaks);
    mUnidentifiedFielder->m_fDesiredSpeed =
        FMIN(mUnidentifiedFielder->m_fDesiredSpeed, fMaxSpeed);

    if (mUnidentifiedFielder->m_pBall != 0)
    {
        mUnidentifiedFielder->InitActionPass(
            mpPassTarget, mbVolleyPass, 0, false);
    }
    else if (fn_8002F858(mUnidentifiedFielder, false))
    {
        mUnidentifiedFielder->InitActionLooseBallPass(
            (cFielder*)mpPassTarget, mbVolleyPass);
        result = mUnidentifiedFielder->m_eActionState
                == ACTION_LOOSE_BALL_PASS
            || mUnidentifiedFielder->m_eActionState
                == ACTION_LOOSE_BALL_SHOT;
    }
    else
    {
        result = false;
    }
    return result;
}

/**
 * Offset/Address/Size: 0x9DC | 0x800BAF58 | size: 0x4
 */
void DesirePass::UnidentifiedUpdate(
    UnidentifiedDesireUpdate*, float)
{
}

/**
 * Offset/Address/Size: 0x9E0 | 0x800BAF5C | size: 0x4
 */
void DesirePass::UnidentifiedCleanup()
{
}

/**
 * Offset/Address/Size: 0x9E4 | 0x800BAF60 | size: 0xCC8
 */
extern "C" UnidentifiedVariant_80054AB8 fn_800BAF60(
    Variant* fielderValue, UnidentifiedPassCallbackValue* value)
{
    UnidentifiedVariant_80054AB8 result(FT_INT, lbl_806DC14C);
    if (g_pBall->m_pOwner != 0)
    {
        result = FuzzyVariant(FT_INT, 1);
    }
    else if (value->mValue.mType == (eVariantType)13)
    {
        cFielder* pFielder =
            (cFielder*)fielderValue->mData.pPlayer;
        if (fn_8002F858(pFielder, false))
        {
            bool bVolleyPass = fn_80035F34(pFielder);
            bool bActionInitialized;
            if (value->mExtraData.Get(1)->mData.b)
            {
                pFielder->InitActionLooseBallShot(bVolleyPass);
                bActionInitialized =
                    pFielder->m_eActionState == ACTION_LOOSE_BALL_SHOT;
            }
            else
            {
                cFielder* pPassTarget = (cFielder*)
                    value->mExtraData.Get(0)->mData.pPlayer;
                if (pPassTarget != 0 && pPassTarget->CanReceivePass())
                {
                    pFielder->InitActionLooseBallPass(
                        pPassTarget, bVolleyPass);
                    bActionInitialized =
                        pFielder->m_eActionState
                        == ACTION_LOOSE_BALL_PASS;
                }
                else
                {
                    pFielder->InitActionLooseBallShot(
                        fn_80035F34(pFielder));
                    bActionInitialized =
                        pFielder->m_eActionState
                        == ACTION_LOOSE_BALL_SHOT;
                }
            }
            if (bActionInitialized)
            {
                result = FuzzyVariant(FT_INT, 1);
            }
        }
    }
    else
    {
        result = FuzzyVariant(FT_INT, 1);
    }
    return result;
}

/**
 * Offset/Address/Size: 0x16AC | 0x800BBC28 | size: 0x110
 */
void DesirePass::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesirePass");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition,
        "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition,
        "mThinkTimer");
    fn_80338F88(cache, 15, lbl_80533C98[15].size,
        (u8*)&mpPassTarget - (u8*)&mvDesiredPosition,
        "mpPassTarget");
    fn_80338F88(cache, 16, lbl_80533C98[16].size,
        (u8*)&mbVolleyPass - (u8*)&mvDesiredPosition,
        "mbVolleyPass");
    fn_80338F78(cache);
}

/**
 * Offset/Address/Size: 0x17BC | 0x800BBD38 | size: 0xC0
 */
void DesirePass::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (lbl_806DC146 == 0xFFFF)
    {
        UnidentifiedVirtual8(&lbl_806DC146, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = fn_8033930C(cache, lbl_806DC146,
        (u8*)this + offset, sizeof(DesirePass) - offset);
    if (data != 0)
    {
        DesirePass* desire =
            (DesirePass*)((u8*)data - offset);
        desire->mpPassTarget = (cPlayer*)(mpPassTarget == 0
                ? -1 : mpPassTarget->mUnidentified120);
        fn_80339450(cache, lbl_806DC146, data, context);
    }
}

/**
 * Offset/Address/Size: 0x187C | 0x800BBDF8 | size: 0x134
 */
void DesirePreparePass::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field =
        fn_80338EBC(cache, "DesirePreparePass");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition,
        "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition,
        "mThinkTimer");
    fn_80338F88(cache, 15, lbl_80533C98[15].size,
        (u8*)&mpPassTarget - (u8*)&mvDesiredPosition,
        "mpPassTarget");
    fn_80338F88(cache, 16, lbl_80533C98[16].size,
        (u8*)&mbVolleyPass - (u8*)&mvDesiredPosition,
        "mbVolleyPass");
    fn_80338F88(cache, 17, lbl_80533C98[17].size,
        (u8*)&mfAbortThreshold - (u8*)&mvDesiredPosition,
        "mfAbortThreshold");
    fn_80338F78(cache);
}

/**
 * Offset/Address/Size: 0x19B0 | 0x800BBF2C | size: 0xC0
 */
void DesirePreparePass::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (lbl_806DC144 == 0xFFFF)
    {
        UnidentifiedVirtual8(&lbl_806DC144, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = fn_8033930C(cache, lbl_806DC144,
        (u8*)this + offset, sizeof(DesirePreparePass) - offset);
    if (data != 0)
    {
        DesirePreparePass* desire =
            (DesirePreparePass*)((u8*)data - offset);
        desire->mpPassTarget = (cPlayer*)(mpPassTarget == 0
                ? -1 : mpPassTarget->mUnidentified120);
        fn_80339450(
            cache, lbl_806DC144, data, context);
    }
}

/**
 * Offset/Address/Size: 0x1A70 | 0x800BBFEC | size: 0x5C
 */
DesirePreparePass::~DesirePreparePass()
{
}

/**
 * Offset/Address/Size: 0x1ACC | 0x800BC048 | size: 0x5C
 */
DesirePass::~DesirePass()
{
}
