#include "Game/AI/FuzzyVariant.h"

#include "Game/MathHelpers.h"
#include "NL/nlMath.h"
#include "NL/nlTicker.h"

extern void nlPrintf(const char*, ...);

extern UnidentifiedVariant_80054AB8 lbl_80584250;

class UnidentifiedTimerCountdown
{
public:
    UnidentifiedTimerCountdown(float dt)
        : mDeltaTime(dt)
    {
    }

    void fn_8030F9A4(const unsigned long&, Timer* timer)
    {
        timer->Countdown(mDeltaTime, 0.0f);
    }

    float mDeltaTime;
};

float fn_8030F5DC()
{
    return fn_802AAA28(nlGetTicker());
}

UnidentifiedFielderInput::~UnidentifiedFielderInput()
{
}

void UnidentifiedFielderInput::fn_8030F74C(
    bool deleteOwner, bool deleteController)
{
    if (deleteController && mUnidentified18 != 0)
    {
        mUnidentified18->UnidentifiedVirtual4(true);
        delete mUnidentified18;
        mUnidentified18 = 0;
    }

    if (deleteOwner)
    {
        delete mUnidentified14;
        mUnidentified14 = 0;
    }
}

void UnidentifiedFielderInput::fn_8030F800(
    bool updateController, float dt)
{
    UnidentifiedTimerCountdown callback(dt);
    mTimers.Walk(&callback, &UnidentifiedTimerCountdown::fn_8030F9A4);

    if (updateController && mUnidentified18 != 0)
    {
        mUnidentified18->UnidentifiedVirtual3(dt);
    }
}

unsigned long UnidentifiedFielderInput::fn_8030F9B4(
    unsigned long key, bool concurrent) const
{
    return concurrent * key;
}

Timer* UnidentifiedFielderInput::fn_8030F9BC(unsigned long key)
{
    Timer* timer = 0;
    mTimers.FindGet(key, &timer);
    return timer;
}

Timer* UnidentifiedFielderInput::fn_8030FA10(
    unsigned long key, float seconds)
{
    Timer* timer = fn_8030F9BC(key);
    if (timer == 0)
    {
        Timer newTimer;
        mTimers.Add(key, newTimer);
        timer = fn_8030F9BC(key);
    }

    if (seconds == 0.0f)
    {
        timer->m_unk0 = timer->m_uPackedTime != 0;
        timer->m_uPackedTime = 0;
    }
    else
    {
        float jitter = nlMinEquals(0.185f * seconds, 0.1f);
        timer->SetSeconds(
            seconds + (2.0f * jitter * nlRandomf(1.0f) - jitter));
    }

    return timer;
}

bool UnidentifiedFielderInput::fn_8030FB7C(unsigned long key)
{
    Timer* timer = fn_8030F9BC(key);
    return timer != 0 && timer->m_uPackedTime != 0;
}

static inline float UnidentifiedGetExtraFloat(
    UnidentifiedVariant_80054AB8* pAction, int index,
    float defaultValue)
{
    if (pAction->ExtraData.IsSet(index))
    {
        return pAction->ExtraData.Get(index)->mData.f;
    }
    return defaultValue;
}

int fn_8030FD40(
    UnidentifiedVariant_80054AB8* const& first,
    UnidentifiedVariant_80054AB8* const& second)
{
    if (UnidentifiedGetExtraFloat(first, 4, 0.0f)
        == UnidentifiedGetExtraFloat(second, 4, 0.0f))
    {
        return 0;
    }
    if (UnidentifiedGetExtraFloat(first, 4, 0.0f)
        > UnidentifiedGetExtraFloat(second, 4, 0.0f))
    {
        return -1;
    }
    return 1;
}

BasicSlotPool<UnidentifiedActionQueue> lbl_80584228(16, 16);

UnidentifiedActionQueue::UnidentifiedActionQueue()
{
    m_lQueuedActions.m_pEnd = 0;
    m_lQueuedActions.m_pStart = 0;
    m_pLastQueuedAction = 0;
    m_pSelectedAction = 0;
    mActionSelection = 1;
    m_pSelectionWeights = 0;
    mNumSelectionWeights = 0;
}

UnidentifiedActionQueue::~UnidentifiedActionQueue()
{
    UnidentifiedVariant_80054AB8* pNext;
    UnidentifiedVariant_80054AB8* pAction
        = m_lQueuedActions.m_pStart;
    while (pAction != 0)
    {
        pNext = pAction->next;
        delete pAction;
        pAction = pNext;
    }

    m_lQueuedActions.m_pEnd = 0;
    m_lQueuedActions.m_pStart = 0;
    m_pLastQueuedAction = 0;
    m_pSelectedAction = 0;
}

void UnidentifiedActionQueue::fn_8030FF6C(bool preserveSelected)
{
    UnidentifiedVariant_80054AB8* pAction
        = m_lQueuedActions.m_pStart;
    while (pAction != 0)
    {
        UnidentifiedVariant_80054AB8* pNext = pAction->next;
        if (!preserveSelected || pAction != m_pSelectedAction)
        {
            delete pAction;
        }
        pAction = pNext;
    }

    m_lQueuedActions.m_pEnd = 0;
    m_lQueuedActions.m_pStart = 0;
    m_pLastQueuedAction = 0;
    m_pSelectedAction = 0;
}

void UnidentifiedActionQueue::fn_8031002C(int actionSelection)
{
    mActionSelection = actionSelection;
}

void UnidentifiedActionQueue::fn_80310034(
    float* weights, int count)
{
    m_pSelectionWeights = weights;
    mNumSelectionWeights = count;
}

UnidentifiedVariant_80054AB8* UnidentifiedActionQueue::fn_80310040(
    UnidentifiedVariant_80054AB8* pNewAction)
{
    if (pNewAction->ExtraData.IsSet(5)
        && UnidentifiedGetExtraFloat(pNewAction, 4, 0.0f)
               < pNewAction->ExtraData.Get(5)->mData.f)
    {
        delete pNewAction;
        return 0;
    }

    if (UnidentifiedGetExtraFloat(pNewAction, 6, 1.0f) == 0.0f)
    {
        delete pNewAction;
        return 0;
    }

    if (UnidentifiedGetExtraFloat(pNewAction, 4, 0.0f) == 0.0f)
    {
        nlPrintf("This should never happen!.\n");
    }

    UnidentifiedVariant_80054AB8* pAction = fn_80310B80(pNewAction);
    if (pAction != 0)
    {
        float oldValue = UnidentifiedGetExtraFloat(pAction, 4, 0.0f)
                       * UnidentifiedGetExtraFloat(pAction, 6, 1.0f);
        float newValue = UnidentifiedGetExtraFloat(pNewAction, 4, 0.0f)
                       * UnidentifiedGetExtraFloat(pNewAction, 6, 1.0f);
        if (oldValue < newValue)
        {
            nlListRemoveElement(&m_lQueuedActions.m_pStart, pAction,
                &m_lQueuedActions.m_pEnd);
            *pAction = *pNewAction;
        }
        else
        {
            pAction = 0;
        }
        delete pNewAction;
    }
    else
    {
        pAction = pNewAction;
    }

    if (pAction != 0)
    {
        UnidentifiedVariant_80054AB8* prev = 0;
        UnidentifiedVariant_80054AB8* cur
            = m_lQueuedActions.m_pStart;

        if (cur == 0)
        {
            nlListAddStart(&m_lQueuedActions.m_pStart, pAction,
                &m_lQueuedActions.m_pEnd);
        }
        else
        {
            for (; cur != 0; prev = cur, cur = cur->next)
            {
                if (fn_8030FD40(cur, pAction) > 0)
                {
                    if (prev == 0)
                    {
                        nlListAddStart(&m_lQueuedActions.m_pStart,
                            pAction, &m_lQueuedActions.m_pEnd);
                    }
                    else if (prev == m_lQueuedActions.m_pEnd)
                    {
                        nlListAddEnd(&m_lQueuedActions.m_pStart,
                            &m_lQueuedActions.m_pEnd, pAction);
                    }
                    else
                    {
                        UnidentifiedVariant_80054AB8* next = prev->next;
                        prev->next = pAction;
                        pAction->next = next;
                    }
                    break;
                }
            }

            if (cur == 0)
            {
                nlListAddEnd(&m_lQueuedActions.m_pStart,
                    &m_lQueuedActions.m_pEnd, pAction);
            }
        }

        m_pLastQueuedAction = pAction;
    }

    return pAction;
}

UnidentifiedVariant_80054AB8* UnidentifiedActionQueue::fn_80310B80(
    UnidentifiedVariant_80054AB8* pFind)
{
    if (m_lQueuedActions.m_pStart == 0)
    {
        return 0;
    }

    UnidentifiedVariant_80054AB8* pAction
        = m_lQueuedActions.m_pStart;
    while (pAction != 0)
    {
        bool bEqual = *(const Variant*)pAction == *(const Variant*)pFind;
        if (bEqual)
        {
            for (int i = 0; i < 19; ++i)
            {
                if (i == 4 || i == 6)
                {
                    continue;
                }
                bEqual = *pAction->ExtraData.Get(i)
                      == *pFind->ExtraData.Get(i);
                if (!bEqual)
                {
                    break;
                }
            }
        }

        if (bEqual)
        {
            return pAction;
        }
        pAction = pAction->next;
    }
    return 0;
}

UnidentifiedVariant_80054AB8* UnidentifiedActionQueue::SelectAction()
{
    UnidentifiedVariant_80054AB8* pAction
        = m_lQueuedActions.m_pStart;
    if (pAction == 0)
    {
        return &lbl_80584250;
    }

    UnidentifiedVariant_80054AB8* pSelectedAction = 0;
    switch (mActionSelection)
    {
    case 0:
        pSelectedAction = pAction;
        break;

    case 1:
    {
        int index = 0;
        for (; pAction != 0; pAction = pAction->next, ++index)
        {
            float chance = UnidentifiedGetExtraFloat(pAction, 6, 1.0f);
            if (m_pSelectionWeights != 0)
            {
                int weightIndex = index;
                if (weightIndex >= mNumSelectionWeights)
                {
                    weightIndex = mNumSelectionWeights - 1;
                }
                chance *= m_pSelectionWeights[weightIndex];
            }

            if (chance == 1.0f || nlRandomf(1.0f) <= chance)
            {
                pSelectedAction = pAction;
                break;
            }
        }

        if (pSelectedAction == 0)
        {
            for (UnidentifiedVariant_80054AB8* pBest
                     = m_lQueuedActions.m_pStart;
                 pBest != 0; pBest = pBest->next)
            {
                if (pSelectedAction == 0
                    || UnidentifiedGetExtraFloat(pBest, 6, 1.0f)
                           > UnidentifiedGetExtraFloat(
                               pSelectedAction, 6, 1.0f))
                {
                    pSelectedAction = pBest;
                }
            }
        }
        break;
    }

    case 2:
    {
        float chances[16];
        UnidentifiedVariant_80054AB8* actions[16];
        float total = 0.0f;
        int count = 0;

        for (; pAction != 0; pAction = pAction->next, ++count)
        {
            actions[count] = pAction;
            float weight = 1.0f;
            if (m_pSelectionWeights != 0)
            {
                int weightIndex = count;
                if (weightIndex >= mNumSelectionWeights)
                {
                    weightIndex = mNumSelectionWeights - 1;
                }
                weight = m_pSelectionWeights[weightIndex];
            }

            float chance = UnidentifiedGetExtraFloat(pAction, 6, 1.0f);
            float confidence = UnidentifiedGetExtraFloat(
                pAction, 4, 0.0f);
            chances[count] = weight * chance * confidence;
            total += chances[count];
        }

        if (total == 0.0f)
        {
            pSelectedAction = m_lQueuedActions.m_pStart;
            break;
        }

        for (int i = 0; i < count; ++i)
        {
            chances[i] /= total;
        }

        float random = nlRandomf(1.0f);
        float cumulative = 0.0f;
        for (int i = 0; i < count; ++i)
        {
            cumulative += chances[i];
            if (random < cumulative)
            {
                pSelectedAction = actions[i];
                break;
            }
        }
        break;
    }
    }

    m_pSelectedAction = pSelectedAction;
    return pSelectedAction;
}
