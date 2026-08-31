#include "NL/nlTask.h"

#include "NL/nlDLRing.h"
#include "NL/nlMemory.h"
#include "NL/nlTicker.h"

extern "C" void fn_80371264();

float g_fTaskTimeUpperBound = 0.1f;
nlTaskManager* nlTaskManager::m_pInstance;
float g_fTaskTimeLowerBound;

void nlTaskManager::Startup(u32 initialState)
{
    m_pInstance = new (nlMalloc(sizeof(nlTaskManager), 8, false)) nlTaskManager;
    m_pInstance->mPreviousState = initialState;
    m_pInstance->mCurrentState = initialState;
    m_pInstance->mPendingState = initialState;
    m_pInstance->mTaskList = 0;
    m_pInstance->mTimeDilation = 1.0f;
    m_pInstance->mLocked = false;
}

void nlTaskManager::AddTask(nlTask* task, u32 priority, u32 activeStates)
{
    task->mPriority = priority;
    task->mActiveStates = activeStates;
    task->mPreviousTicker = nlGetTicker();

    if (m_pInstance->mTaskList == 0)
    {
        nlDLRingAddStart<nlTask>(&m_pInstance->mTaskList, task);
        return;
    }

    nlTask* currentTask = nlDLRingGetStart<nlTask>(m_pInstance->mTaskList);
    while (currentTask != 0)
    {
        if (currentTask->mPriority >= priority)
        {
            currentTask = currentTask->m_prev;
            break;
        }
        else if (!nlDLRingIsEnd<nlTask>(m_pInstance->mTaskList, currentTask))
        {
            currentTask = currentTask->m_next;
        }
        else
        {
            break;
        }
    }

    nlDLRingInsert<nlTask>(&m_pInstance->mTaskList, currentTask, task);
}

void nlTaskManager::RunAllTasks()
{
    nlTask* currentTask = nlDLRingGetStart<nlTask>(m_pInstance->mTaskList);
    if (currentTask != 0)
    {
        if (m_pInstance->mCurrentState != m_pInstance->mPendingState)
        {
        transition_loop:
            currentTask->StateTransition(m_pInstance->mCurrentState, m_pInstance->mPendingState);
            if (!nlDLRingIsEnd<nlTask>(m_pInstance->mTaskList, currentTask))
            {
                currentTask = currentTask->m_next;
                goto transition_loop;
            }
            m_pInstance->mPreviousState = m_pInstance->mCurrentState;
            m_pInstance->mCurrentState = m_pInstance->mPendingState;
        }

        nlTask* taskIterator = nlDLRingGetStart<nlTask>(m_pInstance->mTaskList);
    task_loop:
        u32 currentTicker = nlGetTicker();
        float tickerDifference = nlGetTickerDifference(taskIterator->mPreviousTicker, currentTicker);
        taskIterator->mPreviousTicker = currentTicker;
        if (taskIterator->mActiveStates & m_pInstance->mCurrentState)
        {
            float deltaTime = tickerDifference / 1000.0f;
            m_pInstance->mRealTimeDelta = deltaTime;
            if (deltaTime < g_fTaskTimeLowerBound)
            {
                deltaTime = g_fTaskTimeLowerBound;
            }
            else if (deltaTime > g_fTaskTimeUpperBound)
            {
                deltaTime = g_fTaskTimeUpperBound;
            }
            if (taskIterator->mTimeDilated)
            {
                deltaTime *= m_pInstance->mTimeDilation;
            }
            m_pInstance->mCurrentTimeDelta = deltaTime;
            taskIterator->Run(deltaTime);
            taskIterator->mExecutionTime =
                nlGetTickerDifference(taskIterator->mPreviousTicker, nlGetTicker());
            fn_80371264();
        }
        if (taskIterator != m_pInstance->mTaskList)
        {
            taskIterator = taskIterator->m_next;
            goto task_loop;
        }
    }
}

void nlTaskManager::SetNextState(u32 nextState)
{
    m_pInstance->mPendingState = nextState;
}

void nlTaskManager::SetTimeDilation(float timeDilation)
{
    m_pInstance->mTimeDilation = timeDilation;
}
