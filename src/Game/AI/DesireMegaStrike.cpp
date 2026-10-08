#include "NL/nlDLListContainer.inl"
#include "Game/AI/Desire.h"
#include "Game/AI/FielderActions.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/Fielder.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/DebugWriteCache.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Player.h"
#include "Game/Team.h"
#include "NL/globalpad.h"
#include "Game/NetworkInput.h"
#include <stdlib.h>

#include "Game/UnidentifiedStaticStorage.h"

bool gMegaStrikeUsePassButton;
bool gMegaStrikeInvincible;

static float sMegaStrikeMaxDuration = 10.0f;
static float sMegaStrikeFirstPressDelayMin = 0.5f;
static float sMegaStrikeFirstPressDelayMax = 0.9f;
static float sMegaStrikeFirstPressDelayJitter = 0.3f;
static unsigned short sDesireMegaStrikeType = 0xFFFF;

/**
 * Offset/Address/Size: 0x0 | 0x800B93C4 | size: 0x384
 */
bool DesireMegaStrike::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    DetInput* pGlobalPad = m_pFielder->GetGlobalPad();
    if (pGlobalPad != 0)
    {
        ((NetworkPeerChannel*)pGlobalPad->m_pMyUser)->GetLocalChannelPad();
    }
    else
    {
        mfFirstPressDelay = InterpolateClamped(sMegaStrikeFirstPressDelayMin, sMegaStrikeFirstPressDelayMax,
            1.0f - Difficult(fn_800D6670(m_pFielder)));
        float fDelay = mfFirstPressDelay;

        float fRange = fDelay * sMegaStrikeFirstPressDelayJitter;
        mfFirstPressDelay =
            fDelay + (nlRandomf(fRange) - (0.5f * fRange));
        mnPressStage = 0;

        float probabilities[4];
        int i;
        for (i = 0; i < 4; ++i)
        {
            probabilities[i] = fn_800A636C(g_pCurrentlyUpdatingTeam)
                                   ->MegaGoalChance[i]
                                   ->GetValue();
        }

        float fTotal = 0.0f;
        float fRandom = nlRandomf(1.0f);
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
        mnRequestedBalls = nRequestedBalls;

        mnRequestedBalls = (int)(
            (float)mnRequestedBalls + m_pFielder->fn_800489C4());

        if (g_pGame->GetNormalizedGameTime() > 0.75f)
        {
            cTeam* pOtherTeam =
                m_pFielder->m_pTeam->GetOtherTeam();
            int nScoreDifference = m_pFielder->m_pTeam->m_nScore
                - pOtherTeam->m_nScore;
            if (nScoreDifference < 0)
            {
                if ((unsigned int)mnRequestedBalls
                        < (unsigned int)_abs(nScoreDifference)
                    && (float)(unsigned int)_abs(nScoreDifference)
                        < m_pFielder->fn_80048A08())
                {
                    mnRequestedBalls = _abs(nScoreDifference);
                    if (nlRandomf(1.0f) < 0.33f)
                    {
                        ++mnRequestedBalls;
                    }
                }
            }
        }

        float fAccuracyRange = InterpolateRangeClamped(
            0.49f, 0.98f, 1.0f, 0.2f,
            Difficult(fn_800D6670(m_pFielder)));
        mfAccuracyScore = nlRandomf(1.0f);

        float fAccuracy = fn_800A636C(g_pCurrentlyUpdatingTeam)
                              ->MegaGoalAccuracy[(int)((float)mnRequestedBalls
                                  - m_pFielder->fn_800489C4())]
                              ->GetValue();
        if (mfAccuracyScore < fAccuracy)
        {
            mfAccuracyScore = 1.0f - nlRandomf(fAccuracyRange);
        }
        else
        {
            mfAccuracyScore = -0.01f - nlRandomf(fAccuracyRange);
        }

        mfPrevMeterPosition = 0.0f;
        nlPrintf(
            "\nMegaStrike AI requested %d balls with accuracy score = %.2f.\n\n",
            mnRequestedBalls, mfAccuracyScore);
    }

    m_pFielder->InitActionMegaStrikeMeter(true);
    m_pFielder->EndShrink();
    mMaxDuration = sMegaStrikeMaxDuration;

    if (gMegaStrikeInvincible || GameInfoManager::Instance()->IsRule0x8Equal2())
    {
        m_pFielder->muInvincibleStatus |= 0x1F;
    }
    return result;
}

/**
 * Offset/Address/Size: 0x384 | 0x800B9748 | size: 0x63C
 */
void DesireMegaStrike::Update(
    DesireUpdate* update, float fDeltaT)
{
    if (!g_pGame->IsGameplayOrOvertime())
    {
        *update = 1;
    }

    if (m_pFielder->IsStuck())
    {
        *update = 1;
    }

    if (update->mData.i == 1)
    {
        return;
    }

    if (gMegaStrikeInvincible || GameInfoManager::Instance()->IsRule0x8Equal2())
    {
        if (!m_pFielder->IsInvincible())
        {
            m_pFielder->muInvincibleStatus |= 0x1F;
        }
    }

    if (m_pFielder->m_eActionState == ACTION_SHOOT_TO_SCORE)
    {
        bool bButtonPressed = false;
        int nParam = 0;
        bool bHasGlobalPad = m_pFielder->GetGlobalPad() != 0;
        if (bHasGlobalPad)
        {
            DetInput* pGlobalPad = m_pFielder->GetGlobalPad();
            cGlobalPad* pInputPad = 0;
            if (pGlobalPad != 0)
            {
                pInputPad = ((NetworkPeerChannel*)pGlobalPad->m_pMyUser)->GetLocalChannelPad();
            }
            if (pInputPad != 0)
            {
                if (gMegaStrikeUsePassButton)
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
            bButtonPressed = UpdateAIButtonPress(update, fDeltaT);
        }
        m_pFielder->fn_8004923C(
            fDeltaT, bButtonPressed, nParam);
        fn_80098098(m_pFielder);
    }
    else if (m_pFielder->m_eActionState == ACTION_SHOT)
    {
        m_pFielder->EndStar();
        m_pFielder->fn_800489C0(fDeltaT);
    }
}

/**
 * Offset/Address/Size: 0x9C0 | 0x800B9D84 | size: 0x5E0
 */
bool DesireMegaStrike::UpdateAIButtonPress(
    DesireUpdate* update, float)
{
    bool bButtonPressed = false;
    float fMeterResult = GetMegaStrikeShotCount(m_pFielder, 1);
    float fMeterPosition = GetMegaStrikeAccuracy(m_pFielder, 1);

    if (update->mData.i == 3)
    {
        *update = 0;
        if (mAgeTimer.GetSeconds() >= mfFirstPressDelay
            && mnPressStage < 1)
        {
            mnPressStage = 1;
            bButtonPressed = true;
        }
    }

    mParameters.Set(0, FuzzyVariant(FT_INT, mnPressStage));
    mParameters.Set(1, FuzzyVariant(fMeterPosition));

    if (bButtonPressed)
    {
        return true;
    }

    int nMeterResult = (int)fMeterResult;
    switch (mnPressStage)
    {
    case 0:
    {
        bool bAtRequestedValue = false;
        if (Difficult(m_pFielder->m_pTeam) < 0.25f
            && (float)mnRequestedBalls
                == m_pFielder->fn_800489C4())
        {
            bAtRequestedValue = true;
        }

        if (nMeterResult >= mnRequestedBalls && !bAtRequestedValue
            && m_pFielder->GetMegaStrikeMeterPosition() > 0.225f)
        {
            float fChance = InterpolateRangeClamped(
                0.65f, 0.8f, 1.0f, 0.2f,
                Difficult(m_pFielder->m_pTeam));
            if (nlRandomf(1.0f) > fChance)
            {
                bButtonPressed = true;
                if (m_pFielder->GetMegaStrikeMeterPosition() <= 0.225f)
                {
                    mnPressStage = 2;
                }
                else if (nlRandomf(1.0f) < 0.5f)
                {
                    mnPressStage = 1;
                }
                else
                {
                    mnPressStage = 2;
                }
            }
            else if (nMeterResult > mnRequestedBalls)
            {
                bButtonPressed = true;
                if (nlRandomf(1.0f) < 0.5f)
                    mnPressStage = 1;
                else
                    mnPressStage = 2;
            }
        }
        break;
    }
    case 1:
        if (mfPrevMeterPosition > fMeterPosition)
        {
            if (mfAccuracyScore >= 0.0f)
                bButtonPressed = true;
            else
                mnPressStage = 2;
        }
        else if (mfAccuracyScore < 0.0f && fMeterPosition < 0.0f)
        {
            if (fMeterPosition >= mfAccuracyScore)
                bButtonPressed = true;
        }
        else if (mfAccuracyScore >= 0.0f && fMeterPosition >= 0.0f
            && fMeterPosition >= mfAccuracyScore)
        {
            bButtonPressed = true;
        }
        break;
    case 2:
        if (fMeterPosition < mfPrevMeterPosition)
        {
            if (mfAccuracyScore < 0.0f && fMeterPosition < 0.0f)
            {
                if (mfAccuracyScore >= fMeterPosition)
                    bButtonPressed = true;
            }
            else if (mfAccuracyScore >= 0.0f && fMeterPosition >= 0.0f
                && mfAccuracyScore >= fMeterPosition)
            {
                bButtonPressed = true;
            }
        }
        break;
    }

    mfPrevMeterPosition = fMeterPosition;
    return bButtonPressed;
}

/**
 * Offset/Address/Size: 0xFA0 | 0x800BA364 | size: 0x38
 */
void DesireMegaStrike::Cleanup()
{
    DeliverMegaStrikeMeterEndEvent(g_pGame);
    m_pFielder->CleanActionShootToScore();
}

/**
 * Offset/Address/Size: 0xFD8 | 0x800BA39C | size: 0xC8
 */
inline void DesireMegaStrike::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field = cache->BeginType("DesireMegaStrike");
    Desire::UnidentifiedVirtual8(field, cache);
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x10A0 | 0x800BA464 | size: 0x9C
 */
inline void DesireMegaStrike::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireMegaStrikeType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireMegaStrikeType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = (u8*)this + offset;
    cache->ChecksumData(sDesireMegaStrikeType, data, context);
    cache->WriteData(sDesireMegaStrikeType, data,
        sizeof(DesireMegaStrike) - offset);
}

/**
 * Offset/Address/Size: 0x113C | 0x800BA500 | size: 0x5C
 */
inline DesireMegaStrike::~DesireMegaStrike()
{
}
