#include "Game/Task/GameTaskState.h"
#include "Game/Task/LoadingTask.h"

#include "NL/globalpad.h"
#include "NL/platpad.h"

#include "Game/AsyncLoading.h"
#include "Game/Pad/FlickDetection.h"
#include "types.h"
#include "Game/Task/FrontEndTask.h"
#include "Game/main.h"

void LoadingTask::Start()
{
    mElapsed = 0.0f;
    LoadAsyncLoadingByteCode(AsyncLoadingManager::Instance());
}

void LoadingTask::Run(float dt)
{
    mElapsed += dt;

    g_pPadManager->SetActivePadSet(0);
    UpdatePlatPad(g_pPlatPadManager);
    g_pPadManager->Update(dt);
    FlickDetection::Update();

    switch (UpdateAsyncLoadingSequence(AsyncLoadingManager::Instance()))
    {
    case ASYNC_LOADING_FE_READY:
        nlTaskManager::SetNextState(TASK_CLEAN_BOOT);
        break;
    case ASYNC_LOADING_CLEAN_BOOT_COMPLETE:
        nlTaskManager::SetNextState(TASK_FRONTEND);
        break;
    case ASYNC_LOADING_GAME_READY:
        nlTaskManager::SetNextState(TASK_GAMEPLAY);
        break;
    case ASYNC_LOADING_STADIUM_OR_GAME_READY:
        nlTaskManager::SetNextState(TASK_GAMEPLAY);
        break;
    case ASYNC_LOADING_RETURN_TO_FE:
        nlTaskManager::SetNextState(TASK_FRONTEND);
        break;
    }
}

void LoadingTask::StateTransition(unsigned int from, unsigned int to)
{
    if (to == TASK_BOOT_TO_FE && from == TASK_BOOT)
    {
        BeginBootToFrontEndLoading(AsyncLoadingManager::Instance());
    }

    if (to == TASK_CLEAN_BOOT)
    {
        BeginCleanBootShutdown(AsyncLoadingManager::Instance());
    }

    if (to == TASK_FE_TO_GAME && from == TASK_FRONTEND)
    {
        BeginFrontEndToGameLoading(AsyncLoadingManager::Instance());
    }

    if (to == TASK_BOOT_TO_GAME && from == TASK_BOOT)
    {
        BeginBootToGameLoading(AsyncLoadingManager::Instance());
    }

    if (to == TASK_GAME_TO_FE && from != TASK_HOME_BUTTON_MENU)
    {
        if (g_e3_Build && g_bE3IdleReset)
        {
            BeginGameToTitleLoading(AsyncLoadingManager::Instance());
        }
        else
        {
            BeginGameToFrontEndLoading(AsyncLoadingManager::Instance());
        }
    }

    if (to == TASK_STADIUM_VIEWER && from == TASK_BOOT)
    {
        BeginStadiumViewerLoading(AsyncLoadingManager::Instance());
    }
}

LoadingTask sLoadingTask;
