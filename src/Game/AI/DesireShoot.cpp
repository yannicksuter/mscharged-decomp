#include "Game/AI/DesireShoot.h"

#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/ShotMeter.h"
#include "Game/Ball.h"
#include "Game/CharacterTweaks.h"
#include "Game/Player.h"
#include "NL/nlString.h"
#include <stddef.h>

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

struct UnidentifiedDesireMachine
{
    u8 mUnidentified000[0x18];
    void* mUnidentified018;
};

extern "C" DebugFieldType lbl_80533C98[];
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" unsigned short fn_80338EBC(
    DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void* fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);
extern "C" int fn_8002E9D0(cFielder*);
extern "C" bool fn_8002F858(cFielder*, bool);
extern "C" bool fn_8003C180(cFielder*);
extern "C" PlayerTweaks* fn_8003E6E4(cFielder*);
extern "C" float fn_8002C7E8(PlayerTweaks*);
extern "C" UnidentifiedDesireMachine* fn_80316974(void*);
extern "C" void fn_8031998C(
    void*, int, const UnidentifiedVariantCollection*);

static float lbl_806DC208 = 0.5f;
static unsigned short sDesireWindupShotType = 0xFFFF;
static unsigned short sDesireShootType = 0xFFFF;
static bool lbl_806DC210 = true;
static float lbl_806DC220 = 2.0f;

/**
 * Offset/Address/Size: 0x0 | 0x800C4198 | size: 0x7C
 */
bool DesireWindupShot::UnidentifiedInitialize(void*)
{
    bool result = true;
    if (mUnidentifiedFielder->m_pBall != NULL)
    {
        mUnidentifiedFielder->fn_8004B658();
        mUnidentified078 = lbl_806DC208
                         + mUnidentifiedFielder->m_pShotMeter->GetTotalDuration();
        mbShotMeterActivated = true;
        lbl_806DC210 = true;
    }
    else
    {
        result = false;
    }
    return result;
}

/**
 * Offset/Address/Size: 0xC28 | 0x800C4DC0 | size: 0x4
 */
void DesireWindupShot::UnidentifiedCleanup()
{
}

/**
 * Offset/Address/Size: 0xC2C | 0x800C4DC4 | size: 0x244
 */
bool DesireShoot::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    UnidentifiedVariantCollection* params = (UnidentifiedVariantCollection*)context;
    mbLobShot = params->Get(16)->mData.b;

    if (fn_8002E9D0(mUnidentifiedFielder) != 20
        && fn_8002E9D0(mUnidentifiedFielder) != 19)
    {
        mUnidentifiedFielder->DoResetShotMeter(0.0f);
    }

    if (fn_8003C180(mUnidentifiedFielder))
    {
        mUnidentifiedFielder->m_pShotMeter->m_fTime = 0.1f + (float)nlRandom((unsigned int)(fn_8002C7E8(fn_8003E6E4(mUnidentifiedFielder)) - 0.2f));
    }

    if (mUnidentifiedFielder->m_pBall != NULL)
    {
        mUnidentifiedFielder->fn_8004B86C(mbLobShot, false);
    }
    else if (fn_8002F858(mUnidentifiedFielder, false))
    {
        mUnidentifiedFielder->InitActionLooseBallShot(mbLobShot);
        result = mUnidentifiedFielder->m_eActionState
              == ACTION_LOOSE_BALL_SHOT;
    }

    if (mUnidentifiedFielder->m_pShotMeter->m_eShotMeterState
        == SHOT_METER_STS_RELEASED)
    {
        UnidentifiedVariantCollection transitionParams;
        transitionParams.Set(
            7, FuzzyVariant(FT_FLOAT, lbl_806DC220));
        transitionParams.Set(14, FuzzyVariant(g_pBall));
        transitionParams.Set(10,
            FuzzyVariant(FT_U32,
                nlStringHash("TransDesireWindupSkillshot")));
        fn_8031998C(fn_80316974(this)->mUnidentified018,
            13,
            &transitionParams);
    }

    return result;
}

/**
 * Offset/Address/Size: 0xE70 | 0x800C5008 | size: 0x4
 */
void DesireShoot::UnidentifiedUpdate(
    UnidentifiedDesireUpdate*, float)
{
}

/**
 * Offset/Address/Size: 0xE74 | 0x800C500C | size: 0xEC
 */
void DesireShoot::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireShoot");
    fn_80338F88(cache, 22, lbl_80533C98[22].size, 0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F88(cache, 16, lbl_80533C98[16].size, (u8*)&mbLobShot - (u8*)&mvDesiredPosition, "mbLobShot");
    fn_80338F78(cache);
}

/**
 * Offset/Address/Size: 0xF60 | 0x800C50F8 | size: 0x9C
 */
void DesireShoot::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireShootType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireShootType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    fn_80339450(cache, sDesireShootType, data, context);
    fn_8033930C(cache, sDesireShootType, data, sizeof(DesireShoot) - offset);
}

/**
 * Offset/Address/Size: 0xFFC | 0x800C5194 | size: 0xEC
 */
void DesireWindupShot::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireWindupShot");
    fn_80338F88(cache, 22, lbl_80533C98[22].size, 0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F88(cache, 16, lbl_80533C98[16].size, (u8*)&mbShotMeterActivated - (u8*)&mvDesiredPosition, "mbShotMeterActivated");
    fn_80338F78(cache);
}

/**
 * Offset/Address/Size: 0x10E8 | 0x800C5280 | size: 0x9C
 */
void DesireWindupShot::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireWindupShotType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireWindupShotType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    fn_80339450(cache, sDesireWindupShotType, data, context);
    fn_8033930C(cache, sDesireWindupShotType, data, sizeof(DesireWindupShot) - offset);
}

/**
 * Offset/Address/Size: 0x1184 | 0x800C531C | size: 0x5C
 */
DesireWindupShot::~DesireWindupShot()
{
}

/**
 * Offset/Address/Size: 0x11E0 | 0x800C5378 | size: 0x5C
 */
DesireShoot::~DesireShoot()
{
}
