#include "Game/AI/Desire.h"
#include "Game/AI/FielderInput.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/CharacterTweaks.h"
#include "Game/CharacterTriggers.h"
#include "Game/DebugWriteCache.h"
#include "Game/Game.h"
#include "Game/SAnim/pnSAnimController.h"

extern "C" float fn_8002C7D0(PlayerTweaks*);
extern "C" float fn_8002CFD8(PlayerTweaks*);
extern "C" void fn_8002E340(cFielder*);
extern "C" bool fn_8003E73C(cFielder*);
extern "C" void fn_80038158(cFielder*, bool);
extern "C" void fn_801B8164(cFielder*);
extern "C" void fn_801B7F8C(cFielder*);
extern "C" void fn_801B865C(cFielder*);
extern "C" UnidentifiedFielderInput* fn_80316974(void*);
extern float lbl_806E0E40;
extern const nlVector3 lbl_804DC1A0;
extern UnidentifiedStateTransition lbl_806E20B8;

static unsigned short sDesireStarType = 0xFFFF;
static unsigned short sDesireMushroomType = 0xFFFF;
static unsigned short sDesireSlipperyType = 0xFFFF;
static unsigned short sDesireGooeyType = 0xFFFF;
static unsigned short sDesireShrinkType = 0xFFFF;
static unsigned short sDesireFrozenType = 0xFFFF;
static unsigned short sDesireConfusedType = 0xFFFF;

static float lbl_806DC160 = 2.0f;
static float lbl_806DC164 = 1.0f;
static float lbl_806DC168 = 20.0f;
static float lbl_806DC16C = 0.9f;
static float lbl_806DC170 = 0.75f;
static float lbl_806DC174 = 0.6f;
static float lbl_806DC178 = 0.15f;
static float lbl_806DC17C = 1.25f;
static float lbl_806DC180 = 2.5f;
static float lbl_806DC184 = 0.33f;
static nlVector2 lbl_806DC188 = { 0.1f, 0.0f };

/**
 * Offset/Address/Size: 0x0 | 0x800BC0C4 | size: 0x68
 */
bool DesireStar::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    mUnidentified078 = fn_8002CFD8(mUnidentifiedFielder->GetTweaks());
    mUnidentifiedFielder->muInvincibleStatus |= 0x1F;
    EmitStar(mUnidentifiedFielder, false);
    return result;
}

/**
 * Offset/Address/Size: 0x68 | 0x800BC12C | size: 0x74
 */
bool DesireStar::UnidentifiedReinitialize(void* context)
{
    mUnidentifiedTimer.m_unk0 = mUnidentifiedTimer.m_uPackedTime != 0;
    mUnidentifiedTimer.m_uPackedTime = 0;
    bool result = Desire::UnidentifiedInitialize(context);
    mUnidentified078 = fn_8002CFD8(mUnidentifiedFielder->GetTweaks());
    EmitStar(mUnidentifiedFielder, true);
    return result;
}

/**
 * Offset/Address/Size: 0x668 | 0x800BC72C | size: 0x3C
 */
void DesireStar::UnidentifiedCleanup()
{
    KillStar(mUnidentifiedFielder);
    fn_80038158(mUnidentifiedFielder, true);
}

/**
 * Offset/Address/Size: 0x6A4 | 0x800BC768 | size: 0x84
 */
bool DesireMushroom::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    mUnidentified078 = fn_8002C7D0(mUnidentifiedFielder->GetTweaks());
    fn_8002E340(mUnidentifiedFielder);
    if (!mUnidentifiedFielder->fn_8003E74C())
    {
        mUnidentifiedFielder->fn_8001EE74(lbl_806DC17C, 0.2f, -1.0f);
    }
    EmitMushroom(mUnidentifiedFielder, false);
    return result;
}

/**
 * Offset/Address/Size: 0x728 | 0x800BC7EC | size: 0x64
 */
bool DesireMushroom::UnidentifiedReinitialize(void* context)
{
    mUnidentifiedTimer.m_unk0 = mUnidentifiedTimer.m_uPackedTime != 0;
    mUnidentifiedTimer.m_uPackedTime = 0;
    bool result = Desire::UnidentifiedInitialize(context);
    EmitMushroom(mUnidentifiedFielder, true);
    return result;
}

/**
 * Offset/Address/Size: 0xC68 | 0x800BCD2C | size: 0x64
 */
void DesireMushroom::UnidentifiedCleanup()
{
    KillMushroom(mUnidentifiedFielder);
    if (!mUnidentifiedFielder->fn_8003E74C()
        && !fn_8003E73C(mUnidentifiedFielder))
    {
        mUnidentifiedFielder->fn_8001EE74(1.0f, 0.2f, -1.0f);
    }
}

/**
 * Offset/Address/Size: 0xCCC | 0x800BCD90 | size: 0x3C
 */
bool DesireSlippery::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    mUnidentified078 = lbl_806DC160;
    lbl_806E0E40 = lbl_806DC164;
    return result;
}

/**
 * Offset/Address/Size: 0xD08 | 0x800BCDCC | size: 0x20
 */
bool DesireSlippery::UnidentifiedReinitialize(void* context)
{
    mUnidentifiedTimer.m_unk0 = mUnidentifiedTimer.m_uPackedTime != 0;
    mUnidentifiedTimer.m_uPackedTime = 0;
    return Desire::UnidentifiedInitialize(context);
}

/**
 * Offset/Address/Size: 0xFB0 | 0x800BD074 | size: 0x4
 */
void DesireSlippery::UnidentifiedCleanup()
{
}

/**
 * Offset/Address/Size: 0xFB4 | 0x800BD078 | size: 0x78
 */
DesireGooey::DesireGooey()
    : Desire(27, UnidentifiedStateTransition(lbl_806E20B8))
    , mfGooPercentage(1.0f)
    , mfMaxGooEffect(1.0f)
    , mUnidentifiedAC(-1.0f)
    , mfGooTime(0.0f)
    , mf_NotRunning_SpeedScale(1.0f)
    , mf_NotRunning_MovementScale(1.0f)
{
}

/**
 * Offset/Address/Size: 0x102C | 0x800BD0F0 | size: 0xD4
 */
bool DesireGooey::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    UnidentifiedVariantCollection* params = (UnidentifiedVariantCollection*)context;
    float fGooEffect = params->Get(0)->mData.f;
    if (fGooEffect < mfMaxGooEffect)
    {
        mUnidentifiedAC = -1.0f;
        mfMaxGooEffect = params->Get(0)->mData.f;
        mfGooTime = params->Get(1)->mData.f;
        mf_NotRunning_SpeedScale = params->Get(2)->mData.f;
        mf_NotRunning_MovementScale = params->Get(3)->mData.f;
        mUnidentified078 = mfGooTime;
    }
    else
    {
        mUnidentifiedAC = fGooEffect;
    }
    mfGooPercentage = 1.0f;
    return result;
}

/**
 * Offset/Address/Size: 0x1100 | 0x800BD1C4 | size: 0x2C
 */
bool DesireGooey::UnidentifiedReinitialize(void* context)
{
    mUnidentifiedTimer.m_unk0 = mUnidentifiedTimer.m_uPackedTime != 0;
    mUnidentifiedTimer.m_uPackedTime = 0;
    return UnidentifiedInitialize(context);
}

/**
 * Offset/Address/Size: 0x112C | 0x800BD1F0 | size: 0x18
 */
float DesireGooey::fn_800BD1F0()
{
    return InterpolateRangeClamped(
        1.0f, mfMaxGooEffect, 0.0f, 1.0f, mfGooPercentage);
}

/**
 * Offset/Address/Size: 0x1144 | 0x800BD208 | size: 0x344
 */
void DesireGooey::Update(
    UnidentifiedDesireUpdate* update, float fDeltaT)
{
    if (mUnidentifiedAC != -1.0f)
    {
        mfMaxGooEffect += mUnidentifiedAC * (mfGooTime * fDeltaT);
    }

    mfGooPercentage = 1.0f
                    - (mUnidentifiedTimer.GetSeconds() / mUnidentified078);
    if (!mUnidentifiedFielder->IsRunning())
    {
        mUnidentifiedFielder->m_pCurrentAnimController
            ->m_fPlaybackSpeedScale = InterpolateRangeClamped(
            1.0f, mf_NotRunning_SpeedScale, 0.02f, 2.0f, mfGooPercentage);
        float movementScale = InterpolateRangeClamped(
            1.0f, mf_NotRunning_MovementScale, 0.02f, 2.0f, mfGooPercentage);
        mUnidentifiedFielder->fn_8001EF6C(movementScale);
    }
    else
    {
        float movementScale = InterpolateRangeClamped(
            1.0f, mfMaxGooEffect, 0.0f, 1.0f, mfGooPercentage);
        mUnidentifiedFielder->fn_8001EF6C(movementScale);
    }

    if (!g_pGame->IsGameplayOrOvertime())
    {
        {
            FuzzyVariant result(FT_INT, 1);
            *update = result;
        }
        update->mTemporary = false;
    }
}

/**
 * Offset/Address/Size: 0x1488 | 0x800BD54C | size: 0x48
 */
void DesireGooey::UnidentifiedCleanup()
{
    mfMaxGooEffect = 1.0f;
    mUnidentifiedFielder->fn_8001EF6C(1.0f);
    mUnidentifiedFielder->m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.0f;
}

/**
 * Offset/Address/Size: 0x1698 | 0x800BD75C | size: 0x8
 */
float DesireShrink::fn_800BD75C()
{
    return lbl_806DC16C;
}

/**
 * Offset/Address/Size: 0x1C40 | 0x800BDD04 | size: 0xAC
 */
bool DesireFrozen::UnidentifiedReinitialize(void* context)
{
    if (meFrozenState == 3)
    {
        return false;
    }

    mePrevFrozenState = meFrozenState;
    mfPrevFrozenTime = mUnidentified078 - mUnidentifiedTimer.GetSeconds();
    mUnidentifiedTimer.m_unk0 = mUnidentifiedTimer.m_uPackedTime != 0;
    mUnidentifiedTimer.m_uPackedTime = 0;

    UnidentifiedVariantCollection* params = (UnidentifiedVariantCollection*)context;
    fn_800BE1AC(params->Get(0)->mData.i);
    fn_801B865C(mUnidentifiedFielder);
    return Desire::UnidentifiedInitialize(context);
}

/**
 * Offset/Address/Size: 0x2234 | 0x800BE2F8 | size: 0x178
 */
bool DesireConfused::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    mfConfusedDirection = 16384.0f + nlRandomf(16384.0f);
    if (nlRandomf(1.0f) < 0.5f)
    {
        mfConfusedDirection = -mfConfusedDirection;
    }
    mfConfusedPercentage = 0.0f;
    fn_801B7F8C(mUnidentifiedFielder);

    if (mUnidentifiedFielder->m_pBall != 0)
    {
        if (mUnidentifiedFielder->fn_8002E060()
            == (eFielderDesireState)ACTION_UNKNOWN_32)
        {
            mUnidentifiedFielder->ReleaseBall(0);
            mUnidentifiedFielder->EndDesire();
            mUnidentifiedFielder->InitActionRunning();
        }
        else if (mUnidentifiedFielder->mUnidentified024.m_eCharacterClass
                     == (eCharacterClass)0x13
                 && mUnidentifiedFielder->m_eActionState
                        == ACTION_UNKNOWN_32)
        {
            unsigned short direction = mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection;
            bool hasGlobalPad = mUnidentifiedFielder->GetGlobalPad() != 0;
            if (hasGlobalPad)
            {
                direction += (int)(mfConfusedDirection * mfConfusedPercentage);
            }
            mUnidentifiedFielder->SetFacingDirection(direction, true);
        }
        else
        {
            mUnidentifiedFielder->ReleaseBall(0);
            mUnidentifiedFielder->ShootBallDueToContact(
                mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection);
        }
    }

    mvDesiredPosition = lbl_804DC1A0;
    mvDesiredPosition.x = 1.0f;
    fn_80316974(this)->fn_8030FA10(0xFF, 0.0f);
    return result;
}

/**
 * Offset/Address/Size: 0x23AC | 0x800BE470 | size: 0xB8
 */
bool DesireConfused::UnidentifiedReinitialize(void* context)
{
    mUnidentifiedTimer.m_unk0 = mUnidentifiedTimer.m_uPackedTime != 0;
    mUnidentifiedTimer.m_uPackedTime = 0;
    bool result = Desire::UnidentifiedInitialize(context);
    mfConfusedPercentage += lbl_806DC188.x;
    if (mfConfusedPercentage >= 1.0f)
    {
        mfConfusedPercentage = 1.0f;
    }

    if (mUnidentifiedFielder->m_pBall != 0
        && (mUnidentifiedFielder->mUnidentified024.m_eCharacterClass
                != (eCharacterClass)0x13
            || mUnidentifiedFielder->m_eActionState
                   != ACTION_UNKNOWN_32))
    {
        mUnidentifiedFielder->ReleaseBall(0);
        mUnidentifiedFielder->ShootBallDueToContact(
            mUnidentifiedFielder->mUnidentified024.m_aActualFacingDirection);
    }
    return result;
}

/**
 * Offset/Address/Size: 0x2C60 | 0x800BED24 | size: 0x70
 */
void DesireConfused::fn_800BED24(unsigned short* direction)
{
    bool hasGlobalPad = mUnidentifiedFielder->GetGlobalPad() != 0;
    if (hasGlobalPad)
    {
        *direction += (int)(mfConfusedDirection * mfConfusedPercentage);
    }
}

/**
 * Offset/Address/Size: 0x2CD0 | 0x800BED94 | size: 0x8
 */
void DesireConfused::UnidentifiedCleanup()
{
    fn_801B8164(mUnidentifiedFielder);
}

/**
 * Offset/Address/Size: 0x2CD8 | 0x800BED9C | size: 0x110
 */
void DesireConfused::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireConfused");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&mfConfusedPercentage - (u8*)&mvDesiredPosition, "mfConfusedPercentage");
    cache->AddField(8, gDebugFieldTypes[8].size, (u8*)&mfConfusedDirection - (u8*)&mvDesiredPosition, "mfConfusedDirection");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x2DE8 | 0x800BEEAC | size: 0x9C
 */
void DesireConfused::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireConfusedType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireConfusedType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireConfusedType, data, context);
    cache->WriteData(sDesireConfusedType, data, sizeof(DesireConfused) - offset);
}

/**
 * Offset/Address/Size: 0x2E84 | 0x800BEF48 | size: 0x17C
 */
void DesireFrozen::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireFrozen");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&meFrozenState - (u8*)&mvDesiredPosition, "meFrozenState");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mePrevFrozenState - (u8*)&mvDesiredPosition, "mePrevFrozenState");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mePrevActionState - (u8*)&mvDesiredPosition, "mePrevActionState");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&mfPrevFrozenTime - (u8*)&mvDesiredPosition, "mfPrevFrozenTime");
    cache->AddField(16, gDebugFieldTypes[16].size, (u8*)&mbWasDazed - (u8*)&mvDesiredPosition, "mbWasDazed");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x3000 | 0x800BF0C4 | size: 0x9C
 */
void DesireFrozen::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireFrozenType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireFrozenType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireFrozenType, data, context);
    cache->WriteData(sDesireFrozenType, data, sizeof(DesireFrozen) - offset);
}

/**
 * Offset/Address/Size: 0x309C | 0x800BF160 | size: 0xEC
 */
void DesireShrink::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireShrink");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&mfSlowPercentage - (u8*)&mvDesiredPosition, "mfSlowPercentage");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x3188 | 0x800BF24C | size: 0x9C
 */
void DesireShrink::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireShrinkType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireShrinkType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireShrinkType, data, context);
    cache->WriteData(sDesireShrinkType, data, sizeof(DesireShrink) - offset);
}

/**
 * Offset/Address/Size: 0x3224 | 0x800BF2E8 | size: 0x17C
 */
void DesireGooey::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireGooey");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&mfGooPercentage - (u8*)&mvDesiredPosition, "mfGooPercentage");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&mfMaxGooEffect - (u8*)&mvDesiredPosition, "mfMaxGooEffect");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&mfGooTime - (u8*)&mvDesiredPosition, "mfGooTime");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&mf_NotRunning_SpeedScale - (u8*)&mvDesiredPosition, "mf_NotRunning_SpeedScale");
    cache->AddField(17, gDebugFieldTypes[17].size, (u8*)&mf_NotRunning_MovementScale - (u8*)&mvDesiredPosition, "mf_NotRunning_MovementScale");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x33A0 | 0x800BF464 | size: 0x9C
 */
void DesireGooey::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireGooeyType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireGooeyType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireGooeyType, data, context);
    cache->WriteData(sDesireGooeyType, data, sizeof(DesireGooey) - offset);
}

/**
 * Offset/Address/Size: 0x343C | 0x800BF500 | size: 0xC8
 */
void DesireSlippery::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireSlippery");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x3504 | 0x800BF5C8 | size: 0x9C
 */
void DesireSlippery::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireSlipperyType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireSlipperyType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireSlipperyType, data, context);
    cache->WriteData(sDesireSlipperyType, data, sizeof(DesireSlippery) - offset);
}

/**
 * Offset/Address/Size: 0x35A0 | 0x800BF664 | size: 0xC8
 */
void DesireMushroom::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireMushroom");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x3668 | 0x800BF72C | size: 0x9C
 */
void DesireMushroom::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireMushroomType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireMushroomType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireMushroomType, data, context);
    cache->WriteData(sDesireMushroomType, data, sizeof(DesireMushroom) - offset);
}

/**
 * Offset/Address/Size: 0x3704 | 0x800BF7C8 | size: 0xC8
 */
void DesireStar::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireStar");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x37CC | 0x800BF890 | size: 0x9C
 */
void DesireStar::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireStarType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireStarType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireStarType, data, context);
    cache->WriteData(sDesireStarType, data, sizeof(DesireStar) - offset);
}

/**
 * Offset/Address/Size: 0x3868 | 0x800BF92C | size: 0x5C
 */
DesireStar::~DesireStar()
{
}

/**
 * Offset/Address/Size: 0x38C4 | 0x800BF988 | size: 0x5C
 */
DesireMushroom::~DesireMushroom()
{
}

/**
 * Offset/Address/Size: 0x3920 | 0x800BF9E4 | size: 0x5C
 */
DesireSlippery::~DesireSlippery()
{
}

/**
 * Offset/Address/Size: 0x397C | 0x800BFA40 | size: 0x5C
 */
DesireGooey::~DesireGooey()
{
}

/**
 * Offset/Address/Size: 0x39D8 | 0x800BFA9C | size: 0x5C
 */
DesireShrink::~DesireShrink()
{
}

/**
 * Offset/Address/Size: 0x3A34 | 0x800BFAF8 | size: 0x5C
 */
DesireFrozen::~DesireFrozen()
{
}

/**
 * Offset/Address/Size: 0x3A90 | 0x800BFB54 | size: 0x5C
 */
DesireConfused::~DesireConfused()
{
}
