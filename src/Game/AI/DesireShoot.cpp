#include "Game/AI/DesireShoot.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/ShotMeter.h"
#include "Game/AI/SkillTweaks.h"
#include "Game/Ball.h"
#include "Game/CharacterTweaks.h"
#include "Game/DebugWriteCache.h"
#include "Game/Goalie.h"
#include "Game/Net.h"
#include "Game/Player.h"
#include "Game/Team.h"
#include "NL/nlString.h"
#include <math.h>
#include <stddef.h>

#include "Game/UnidentifiedStaticStorage.h"

struct UnidentifiedDesireMachine
{
    u8 mUnidentified000[0x18];
    void* mUnidentified018;
};

extern "C" bool fn_8003C180(cFielder*);
extern "C" float fn_8002C7E8(PlayerTweaks*);
extern "C" UnidentifiedDesireMachine* fn_80316974(void*);
extern "C" void fn_8031998C(
    void*, int, const UnidentifiedVariantCollection*);

extern "C" float fn_80039574(cFielder*);
extern cTeam* g_pCurrentlyUpdatingTeam;

static float lbl_806DC208 = 0.5f;
static unsigned short sDesireWindupShotType = 0xFFFF;
static unsigned short sDesireShootType = 0xFFFF;
static bool lbl_806DC210 = true;
int lbl_806DC214 = 32;
int lbl_806DC218 = 15;
int lbl_806DC21C = 3;
static float lbl_806DC220 = 2.0f;

/**
 * Offset/Address/Size: 0x0 | 0x800C4198 | size: 0x7C
 */
bool DesireWindupShot::UnidentifiedInitialize(void*)
{
    bool result = true;
    if (m_pFielder->m_pBall != NULL)
    {
        m_pFielder->fn_8004B658();
        mUnidentified078 = lbl_806DC208
                         + m_pFielder->m_pShotMeter->GetTotalDuration();
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
 * Offset/Address/Size: 0x7C | 0x800C4214 | size: 0xBAC
 */
void DesireWindupShot::Update(DesireUpdate* update, float fDeltaT)
{
    if (m_pFielder->m_pBall == NULL)
    {
        *update = 1;
        return;
    }

    bool bMeterTransition = false;
    unsigned char bSwitchToShootDesire = 0;
    ShotMeter* pShotMeter = m_pFielder->m_pShotMeter;
    if (pShotMeter->m_eShotMeterState == SHOT_METER_RELEASED
        || pShotMeter->m_eShotMeterState == SHOT_METER_STS_RELEASED)
    {
        bSwitchToShootDesire = 1;
    }
    else if (pShotMeter->m_eShotMeterState == SHOT_METER_STS_TRANSISTION)
    {
        bMeterTransition = true;
        bSwitchToShootDesire = 1;
    }

    if (bSwitchToShootDesire)
    {
        if (bMeterTransition)
        {
            *update = 3;
            update->SetParameter(8, FuzzyVariant(FT_INT, lbl_806DC214));
        }
        else
        {
            *update = 3;
            update->SetParameter(8, FuzzyVariant(FT_INT, lbl_806DC218));
        }
        return;
    }

    if (lbl_806DC210)
    {
        switch (m_pFielder->mUnidentified024.m_eCharacterClass)
        {
        case DONKEYKONG:
        case MARIO:
        case (eCharacterClass)17:
        {
            float fSign = AIsgn(m_pFielder->m_pTeam->GetOtherNet()->m_v3NetLocation.x);
            Goalie* pGoalie = m_pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
            float fGoalieX = fSign * pGoalie->mUnidentified024.m_v3Position.x;
            if (fSign * m_pFielder->mUnidentified024.m_v3Position.x < fGoalieX
                || (float)fabs(m_pFielder->mUnidentified024.m_v3Position.y) > 0.6f * cNet::GetNetWidth())
            {
                float fRange = fn_80039574(m_pFielder);
                float fDistance = nlSqrt(nlVec3DistanceSquared2D(
                    m_pFielder->mUnidentified024.m_v3Position,
                    m_pFielder->m_pTeam->GetOtherTeam()->GetGoalie()->mUnidentified024.m_v3Position), true);
                pGoalie = m_pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
                AvoidableObject* pAvoidable = m_pFielder->mUnidentified320;
                if (0.25f + (fDistance + (pAvoidable->GetRadius()
                        + pGoalie->mUnidentified320->GetRadius())) < fRange)
                {
                    float fSkillshotChance = fn_800A636C(g_pCurrentlyUpdatingTeam)->GetSkillValue(
                        nlStringLowerHash("Windup/Skillshot"), m_pFielder);
                    if (nlRandomf(1.0f) < fSkillshotChance)
                    {
                        *update = 3;
                        update->SetParameter(8, FuzzyVariant(FT_INT, lbl_806DC21C));
                    }
                    else
                    {
                        lbl_806DC210 = false;
                    }
                }
            }
            break;
        }
        }
    }
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

    if (m_pFielder->fn_8002E9D0() != 20
        && m_pFielder->fn_8002E9D0() != 19)
    {
        m_pFielder->DoResetShotMeter(0.0f);
    }

    if (fn_8003C180(m_pFielder))
    {
        m_pFielder->m_pShotMeter->m_fTime = 0.1f + (float)nlRandom((unsigned int)(fn_8002C7E8(m_pFielder->GetTweaks()) - 0.2f));
    }

    if (m_pFielder->m_pBall != NULL)
    {
        m_pFielder->fn_8004B86C(mbLobShot, false);
    }
    else if (m_pFielder->CanContactLooseBall(false))
    {
        m_pFielder->InitActionLooseBallShot(mbLobShot);
        result = m_pFielder->m_eActionState
              == ACTION_LOOSE_BALL_SHOT;
    }

    if (m_pFielder->m_pShotMeter->m_eShotMeterState
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
void DesireShoot::Update(
    DesireUpdate*, float)
{
}

/**
 * Offset/Address/Size: 0xE74 | 0x800C500C | size: 0xEC
 */
void DesireShoot::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireShoot");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->AddField(16, gDebugFieldTypes[16].size, (u8*)&mbLobShot - (u8*)&mvDesiredPosition, "mbLobShot");
    cache->EndType();
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
    cache->ChecksumData(sDesireShootType, data, context);
    cache->WriteData(sDesireShootType, data, sizeof(DesireShoot) - offset);
}

/**
 * Offset/Address/Size: 0xFFC | 0x800C5194 | size: 0xEC
 */
void DesireWindupShot::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireWindupShot");
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    cache->AddField(16, gDebugFieldTypes[16].size, (u8*)&mbShotMeterActivated - (u8*)&mvDesiredPosition, "mbShotMeterActivated");
    cache->EndType();
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
    cache->ChecksumData(sDesireWindupShotType, data, context);
    cache->WriteData(sDesireWindupShotType, data, sizeof(DesireWindupShot) - offset);
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
