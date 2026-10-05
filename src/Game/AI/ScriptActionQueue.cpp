#include "NL/nlPrint.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/TeamPlayMachine.h"

#include "Game/MathHelpers.h"
#include "NL/nlMath.h"
#include "NL/nlTicker.h"


static inline float GetActionFloatParameter(
    UnidentifiedVariant_80054AB8* pAction, int index,
    float defaultValue)
{
    if (pAction->ExtraData.IsSet(index))
    {
        return pAction->ExtraData.Get(index)->mData.f;
    }
    return defaultValue;
}

int CompareActionConfidence(
    UnidentifiedVariant_80054AB8*& first,
    UnidentifiedVariant_80054AB8*& second)
{
    if (first->UnidentifiedGetFloat(4)
        == second->UnidentifiedGetFloat(4))
    {
        return 0;
    }
    if (first->UnidentifiedGetFloat(4)
        > second->UnidentifiedGetFloat(4))
    {
        return -1;
    }
    return 1;
}

static inline void InsertNonHead(ScriptActionQueue* queue,
    UnidentifiedVariant_80054AB8* prev,
    UnidentifiedVariant_80054AB8* pInsertionNode)
{
    if (prev == queue->m_lQueuedActions.m_pEnd)
    {
        nlListAddEnd(&queue->m_lQueuedActions.m_pStart,
            &queue->m_lQueuedActions.m_pEnd, pInsertionNode);
    }
    else
    {
        UnidentifiedVariant_80054AB8* next = prev->next;
        prev->next = pInsertionNode;
        pInsertionNode->next = next;
    }
}

SlotPool<ScriptActionQueue> g_ScriptActionQueuePool(16, 16);

ScriptActionQueue::ScriptActionQueue()
{
    m_lQueuedActions.m_pEnd = 0;
    m_lQueuedActions.m_pStart = 0;
    m_pLastQueuedAction = 0;
    m_pSelectedAction = 0;
    mActionSelection = 1;
    m_pSelectionWeights = 0;
    mNumSelectionWeights = 0;
}

ScriptActionQueue::~ScriptActionQueue()
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

void ScriptActionQueue::ClearQueuedActions(bool preserveSelected)
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

void ScriptActionQueue::SetActionSelection(int actionSelection)
{
    mActionSelection = actionSelection;
}

void ScriptActionQueue::SetSelectionWeights(
    float* weights, int count)
{
    m_pSelectionWeights = weights;
    mNumSelectionWeights = count;
}

UnidentifiedVariant_80054AB8* ScriptActionQueue::QueueAction(
    UnidentifiedVariant_80054AB8* pNewAction)
{
    if (pNewAction->ExtraData.IsSet(5)
        && GetActionFloatParameter(pNewAction, 4, 0.0f)
               < pNewAction->ExtraData.Get(5)->mData.f)
    {
        delete pNewAction;
        return 0;
    }

    if (GetActionFloatParameter(pNewAction, 6, 1.0f) == 0.0f)
    {
        delete pNewAction;
        return 0;
    }

    if (GetActionFloatParameter(pNewAction, 4, 0.0f) == 0.0f)
    {
        nlPrintf("This should never happen!.\n");
    }

    UnidentifiedVariant_80054AB8* pAction = FindQueuedAction(pNewAction);
    if (pAction != 0)
    {
        float oldValue = GetActionFloatParameter(pAction, 4, 0.0f)
                       * GetActionFloatParameter(pAction, 6, 1.0f);
        float newValue = GetActionFloatParameter(pNewAction, 4, 0.0f)
                       * GetActionFloatParameter(pNewAction, 6, 1.0f);
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
        UnidentifiedVariant_80054AB8* pQueuedAction = pAction;
        UnidentifiedVariant_80054AB8* prev = 0;
        UnidentifiedVariant_80054AB8* cur
            = m_lQueuedActions.m_pStart;

        if (cur == 0)
        {
            nlListAddStart(&m_lQueuedActions.m_pStart, pQueuedAction,
                &m_lQueuedActions.m_pEnd);
        }
        else
        {
            for (; cur != 0; prev = cur, cur = cur->next)
            {
                if (CompareActionConfidence(cur, pQueuedAction) > 0)
                {
                    if (prev == 0)
                    {
                        nlListAddStart(&m_lQueuedActions.m_pStart,
                            pQueuedAction, &m_lQueuedActions.m_pEnd);
                    }
                    else
                    {
                        InsertNonHead(this, prev, pQueuedAction);
                    }
                    break;
                }
            }

            if (cur == 0)
            {
                nlListAddEnd(&m_lQueuedActions.m_pStart,
                    &m_lQueuedActions.m_pEnd, pQueuedAction);
            }
        }

        m_pLastQueuedAction = pAction;
    }

    return pAction;
}

static inline bool EqualParameterValue(const FuzzyVariant& value,
    const FuzzyVariant& other)
{
    return value == other;
}

UnidentifiedVariant_80054AB8* ScriptActionQueue::FindQueuedAction(
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
        if (*pAction == *pFind)
        {
            bool bEqual = true;
            for (int i = 0; i < 19; ++i)
            {
                if (i == 6 || i == 4)
                {
                    continue;
                }
                if (!EqualParameterValue(*pAction->ExtraData.Get(i), *pFind->ExtraData.Get(i)))
                {
                    bEqual = false;
                    break;
                }
            }

            if (bEqual)
            {
                return pAction;
            }
        }
        pAction = pAction->next;
    }
    return 0;
}

UnidentifiedVariant_80054AB8* ScriptActionQueue::SelectAction()
{
    UnidentifiedVariant_80054AB8* pSelectedAction;
    int count;
    UnidentifiedVariant_80054AB8* pAction
        = m_lQueuedActions.m_pStart;
    if (pAction == 0)
    {
        return &lbl_80584250;
    }

    pSelectedAction = 0;
    switch (mActionSelection)
    {
    case 2:
    {
        UnidentifiedVariant_80054AB8* actions[16];
        float chances[16];
        float total = 0.0f;
        count = 0;

        for (; pAction != 0; pAction = pAction->next, ++count)
        {
            actions[count] = pAction;
            float weight = 1.0f;
            if (m_pSelectionWeights != 0)
            {
                int weightIndex = nlMin(mNumSelectionWeights - 1, count);
                weight = m_pSelectionWeights[weightIndex];
            }

            float chance = GetActionFloatParameter(pAction, 6, 1.0f);
            chance = weight * chance;
            float confidence = GetActionFloatParameter(
                pAction, 4, 0.0f);
            chances[count] = chance * confidence;
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

    case 0:
        pSelectedAction = pAction;
        break;

    case 1:
    {
        int index = 0;
        for (; pAction != 0; pAction = pAction->next, ++index)
        {
            float chance = GetActionFloatParameter(pAction, 6, 1.0f);
            if (m_pSelectionWeights != 0)
            {
                int weightIndex = nlMin(mNumSelectionWeights - 1, index);
                chance *= m_pSelectionWeights[weightIndex];
            }

            bool selected = chance == 1.0f ? true : nlRandomf(1.0f) <= chance;
            if (selected)
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
                    || GetActionFloatParameter(pBest, 6, 1.0f)
                           > GetActionFloatParameter(
                               pSelectedAction, 6, 1.0f))
                {
                    pSelectedAction = pBest;
                }
            }
        }
        break;
    }
    }

    m_pSelectedAction = pSelectedAction;
    return pSelectedAction;
}
