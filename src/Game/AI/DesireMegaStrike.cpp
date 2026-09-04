#include "Game/AI/Desire.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/DebugWriteCache.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Player.h"
#include "Game/Team.h"
#include "NL/globalpad.h"
#include "unclassified/tu_80336B2C.h"
#include <stdlib.h>

extern "C" SkillTweaks* fn_800A636C(cTeam*);
extern "C" cTeam* fn_800D6670(cFielder*);
extern "C" float fn_800E02B8(cTeam*);
extern "C" void fn_8002E340(cFielder*);
extern "C" void fn_8002E39C(cFielder*);
extern "C" float fn_8002E058(cFielder*);
extern "C" bool fn_8003886C(cFielder*);
extern "C" void fn_8003A0E4(cFielder*);
extern "C" float fn_800499EC(cFielder*, int);
extern "C" float fn_80049CC0(cFielder*, int);
extern "C" void fn_8005FA2C(cGame*);
extern "C" void fn_80098098(cFielder*);
extern cTeam* lbl_806E0E00;
extern bool lbl_806E0E30;
extern bool lbl_806E0E31;

static float lbl_806DC118 = 10.0f;
static float lbl_806DC11C = 0.5f;
static float lbl_806DC120 = 0.9f;
static float lbl_806DC124 = 0.3f;
static unsigned short sDesireMegaStrikeType = 0xFFFF;

/**
 * Offset/Address/Size: 0x0 | 0x800B93C4 | size: 0x384
 */
bool DesireMegaStrike::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    DetInput* pGlobalPad = mUnidentifiedFielder->GetGlobalPad();
    if (pGlobalPad != 0)
    {
        fn_80336D90(
            (UnidentifiedNetworkPeerChannel*)pGlobalPad->m_pMyUser);
    }
    else
    {
        float fDelay = InterpolateClamped(lbl_806DC11C, lbl_806DC120,
            1.0f - fn_800E02B8(fn_800D6670(mUnidentifiedFielder)));
        mUnidentifiedB0 = fDelay;

        float fRange = fDelay * lbl_806DC124;
        mUnidentifiedB0 =
            fDelay + (nlRandomf(fRange) - (0.5f * fRange));
        mUnidentifiedB4 = 0;

        float probabilities[4];
        int i;
        for (i = 0; i < 4; ++i)
        {
            probabilities[i] = fn_800A636C(lbl_806E0E00)
                                   ->mUnidentified0A0[i]
                                   ->fn_800A0D6C();
        }

        float fRandom = nlRandomf(1.0f);
        float fTotal = 0.0f;
        int nRequestedBalls;
        if (fRandom < (fTotal += probabilities[0]))
        {
            nRequestedBalls = 0;
        }
        else if (fRandom < (fTotal += probabilities[1]))
        {
            nRequestedBalls = 1;
        }
        else if (fRandom < (fTotal += probabilities[2]))
        {
            nRequestedBalls = 2;
        }
        else if (fRandom < (fTotal += probabilities[3]))
        {
            nRequestedBalls = 3;
        }
        else
        {
            nRequestedBalls = 0;
        }
        mUnidentifiedA4 = nRequestedBalls;

        mUnidentifiedA4 = (int)(
            (float)mUnidentifiedA4 + mUnidentifiedFielder->fn_800489C4());

        if (g_pGame->GetNormalizedGameTime() > 0.75f)
        {
            cTeam* pOtherTeam =
                mUnidentifiedFielder->m_pTeam->GetOtherTeam();
            int nScoreDifference = mUnidentifiedFielder->m_pTeam->m_nScore
                - pOtherTeam->m_nScore;
            if (nScoreDifference < 0)
            {
                if ((unsigned int)mUnidentifiedA4
                        < (unsigned int)abs(nScoreDifference)
                    && (float)(unsigned int)abs(nScoreDifference)
                        < mUnidentifiedFielder->fn_80048A08())
                {
                    mUnidentifiedA4 = abs(nScoreDifference);
                    if (nlRandomf(1.0f) < 0.33f)
                    {
                        ++mUnidentifiedA4;
                    }
                }
            }
        }

        float fAccuracyRange = InterpolateRangeClamped(
            0.49f, 0.98f, 1.0f, 0.2f,
            fn_800E02B8(fn_800D6670(mUnidentifiedFielder)));
        mUnidentifiedA8 = nlRandomf(1.0f);

        int nSkillIndex = (int)(
            (float)mUnidentifiedA4 - mUnidentifiedFielder->fn_800489C4());
        float fAccuracy = fn_800A636C(lbl_806E0E00)
                              ->mUnidentified0B0[nSkillIndex]
                              ->fn_800A0D6C();
        if (mUnidentifiedA8 < fAccuracy)
        {
            mUnidentifiedA8 = 1.0f - nlRandomf(fAccuracyRange);
        }
        else
        {
            mUnidentifiedA8 = -0.01f - nlRandomf(fAccuracyRange);
        }

        mUnidentifiedAC = 0.0f;
        nlPrintf(
            "\nMegaStrike AI requested %d balls with accuracy score = %.2f.\n\n",
            mUnidentifiedA4, mUnidentifiedA8);
    }

    mUnidentifiedFielder->InitActionMegaStrikeMeter(true);
    fn_8002E340(mUnidentifiedFielder);
    mUnidentified078 = lbl_806DC118;

    if (lbl_806E0E31 || GameInfoManager::Instance()->IsRule0x8Equal2())
    {
        mUnidentifiedFielder->muInvincibleStatus |= 0x1F;
    }
    return result;
}

/**
 * Offset/Address/Size: 0x384 | 0x800B9748 | size: 0x63C
 */
void DesireMegaStrike::UnidentifiedUpdate(
    UnidentifiedDesireUpdate* update, float fDeltaT)
{
    if (!g_pGame->IsGameplayOrOvertime())
    {
        *update = FuzzyVariant(FT_INT, 1);
        update->mTemporary = false;
    }

    if (fn_8003886C(mUnidentifiedFielder))
    {
        *update = FuzzyVariant(FT_INT, 1);
        update->mTemporary = false;
    }

    if (update->mData.i == 1)
    {
        return;
    }

    if (lbl_806E0E31 || GameInfoManager::Instance()->IsRule0x8Equal2())
    {
        cFielder* pFielder = mUnidentifiedFielder;
        bool bAlreadySet = false;
        if (!fn_8003886C(pFielder)
            && (pFielder->muInvincibleStatus & 0x1F) == 0x1F)
        {
            bAlreadySet = true;
        }
        if (!bAlreadySet)
        {
            mUnidentifiedFielder->muInvincibleStatus |= 0x1F;
        }
    }

    if (mUnidentifiedFielder->m_eActionState == ACTION_SHOOT_TO_SCORE)
    {
        bool bButtonPressed = false;
        int nParam = 0;
        bool bHasGlobalPad = mUnidentifiedFielder->GetGlobalPad() != 0;
        if (bHasGlobalPad)
        {
            DetInput* pGlobalPad = mUnidentifiedFielder->GetGlobalPad();
            cGlobalPad* pInputPad = 0;
            if (pGlobalPad != 0)
            {
                pInputPad = fn_80336D90(
                    (UnidentifiedNetworkPeerChannel*)pGlobalPad->m_pMyUser);
            }
            if (pInputPad != 0)
            {
                if (lbl_806E0E30)
                {
                    bButtonPressed =
                        pInputPad->PlatJustPressed(0x1B, true);
                }
                else
                {
                    bButtonPressed =
                        pInputPad->PlatJustPressed(0x1C, true);
                }
                nParam = 1;
            }
        }
        else
        {
            nParam = 1;
            bButtonPressed = fn_800B9D84(update, fDeltaT);
        }
        mUnidentifiedFielder->fn_8004923C(
            fDeltaT, bButtonPressed, nParam);
        fn_80098098(mUnidentifiedFielder);
    }
    else if (mUnidentifiedFielder->m_eActionState == ACTION_SHOT)
    {
        fn_8002E39C(mUnidentifiedFielder);
        mUnidentifiedFielder->fn_800489C0();
    }
}

/**
 * Offset/Address/Size: 0x9C0 | 0x800B9D84 | size: 0x5E0
 */
bool DesireMegaStrike::fn_800B9D84(
    UnidentifiedDesireUpdate* update, float)
{
    bool bButtonPressed = false;
    float fMeterResult = fn_800499EC(mUnidentifiedFielder, 1);
    float fMeterPosition = fn_80049CC0(mUnidentifiedFielder, 1);

    if (update->mData.i == 3)
    {
        *update = FuzzyVariant(FT_INT, 0);
        update->mTemporary = false;
        if (mUnidentifiedTimer.GetSeconds() >= mUnidentifiedB0
            && mUnidentifiedB4 < 1)
        {
            mUnidentifiedB4 = 1;
            bButtonPressed = true;
        }
    }

    UnidentifiedVariantCollection* pValues =
        (UnidentifiedVariantCollection*)((u8*)this + 0x1C);
    pValues->Set(0, FuzzyVariant(FT_INT, mUnidentifiedB4));
    pValues->Set(1, FuzzyVariant(fMeterPosition));

    if (bButtonPressed)
    {
        return true;
    }

    int nMeterResult = (int)fMeterResult;
    switch (mUnidentifiedB4)
    {
    case 0:
    {
        bool bAtRequestedValue = false;
        if (fn_800E02B8(mUnidentifiedFielder->m_pTeam) < 0.25f
            && (float)mUnidentifiedA4
                == mUnidentifiedFielder->fn_800489C4())
        {
            bAtRequestedValue = true;
        }

        if (nMeterResult >= mUnidentifiedA4 && !bAtRequestedValue)
        {
            if (fn_8002E058(mUnidentifiedFielder) <= 0.225f)
            {
                break;
            }

            float fChance = InterpolateRangeClamped(
                0.65f, 0.8f, 1.0f, 0.2f,
                fn_800E02B8(mUnidentifiedFielder->m_pTeam));
            if (nlRandomf(1.0f) > fChance)
            {
                bButtonPressed = true;
                if (fn_8002E058(mUnidentifiedFielder) <= 0.225f)
                {
                    mUnidentifiedB4 = 2;
                }
                else if (nlRandomf(1.0f) < 0.5f)
                {
                    mUnidentifiedB4 = 1;
                }
                else
                {
                    mUnidentifiedB4 = 2;
                }
                break;
            }
        }

        if (nMeterResult > mUnidentifiedA4)
        {
            bButtonPressed = true;
            mUnidentifiedB4 = nlRandomf(1.0f) < 0.5f ? 1 : 2;
        }
        break;
    }
    case 1:
        if (mUnidentifiedAC > fMeterPosition)
        {
            if (mUnidentifiedA8 >= 0.0f)
            {
                bButtonPressed = true;
            }
            else
            {
                mUnidentifiedB4 = 2;
            }
        }
        else if ((mUnidentifiedA8 < 0.0f
                     && fMeterPosition < 0.0f
                     && fMeterPosition >= mUnidentifiedA8)
            || (mUnidentifiedA8 >= 0.0f
                && fMeterPosition >= 0.0f
                && fMeterPosition >= mUnidentifiedA8))
        {
            bButtonPressed = true;
        }
        break;
    case 2:
        if (fMeterPosition < mUnidentifiedAC
            && ((mUnidentifiedA8 < 0.0f
                    && fMeterPosition < 0.0f
                    && mUnidentifiedA8 >= fMeterPosition)
                || (mUnidentifiedA8 >= 0.0f
                    && fMeterPosition >= 0.0f
                    && mUnidentifiedA8 >= fMeterPosition)))
        {
            bButtonPressed = true;
        }
        break;
    }

    mUnidentifiedAC = fMeterPosition;
    return bButtonPressed;
}

/**
 * Offset/Address/Size: 0xFA0 | 0x800BA364 | size: 0x38
 */
void DesireMegaStrike::UnidentifiedCleanup()
{
    fn_8005FA2C(g_pGame);
    fn_8003A0E4(mUnidentifiedFielder);
}

/**
 * Offset/Address/Size: 0xFD8 | 0x800BA39C | size: 0xC8
 */
void DesireMegaStrike::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = fn_80338EBC(cache, "DesireMegaStrike");
    fn_80338F88(cache, 22, lbl_80533C98[22].size,
        0, "mvDesiredPosition");
    fn_80338F88(cache, 14, lbl_80533C98[14].size,
        (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    fn_80338F88(cache, 20, lbl_80533C98[20].size,
        (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
    fn_80338F78(cache);
}

/**
 * Offset/Address/Size: 0x10A0 | 0x800BA464 | size: 0x9C
 */
void DesireMegaStrike::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireMegaStrikeType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireMegaStrikeType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    fn_80339450(cache, sDesireMegaStrikeType, data, context);
    fn_8033930C(cache, sDesireMegaStrikeType, data,
        sizeof(DesireMegaStrike) - offset);
}

/**
 * Offset/Address/Size: 0x113C | 0x800BA500 | size: 0x5C
 */
DesireMegaStrike::~DesireMegaStrike()
{
}
