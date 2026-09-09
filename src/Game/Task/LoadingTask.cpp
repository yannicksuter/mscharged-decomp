#include "Game/Task/LoadingTask.h"

#include "NL/globalpad.h"
#include "NL/platpad.h"

#include "Game/AsyncLoading.h"
#include "Game/Pad/FlickDetection.h"
#include "types.h"

extern bool g_e3_Build;
extern bool g_bE3IdleReset;

void LoadingTask::Start()
{
    mElapsed = 0.0f;
    fn_80118B50(AsyncLoadingManager::Instance());
}

void LoadingTask::Run(float dt)
{
    mElapsed += dt;

    g_pPadManager->SetActivePadSet(0);
    UpdatePlatPad(g_pPlatPadManager);
    g_pPadManager->Update(dt);
    FlickDetection::Update();

    switch (fn_80118B7C(AsyncLoadingManager::Instance()))
    {
    case 3:
        nlTaskManager::SetNextState(0x00080000);
        break;
    case 4:
        nlTaskManager::SetNextState(0x00000004);
        break;
    case 5:
        nlTaskManager::SetNextState(0x00000002);
        break;
    case 7:
        nlTaskManager::SetNextState(0x00000002);
        break;
    case 6:
        nlTaskManager::SetNextState(0x00000004);
        break;
    }
}

void LoadingTask::StateTransition(unsigned int from, unsigned int to)
{
    if (to == 0x00100000 && from == 0x00010000)
    {
        fn_80119054(AsyncLoadingManager::Instance());
    }

    if (to == 0x00080000)
    {
        fn_801190A0(AsyncLoadingManager::Instance());
    }

    if (to == 0x00200000 && from == 0x00000004)
    {
        fn_801190EC(AsyncLoadingManager::Instance());
    }

    if (to == 0x00800000 && from == 0x00010000)
    {
        fn_801191D4(AsyncLoadingManager::Instance());
    }

    if (to == 0x00400000 && from != 0x02000000)
    {
        if (g_e3_Build && g_bE3IdleReset)
        {
            fn_80119184(AsyncLoadingManager::Instance());
        }
        else
        {
            fn_80119138(AsyncLoadingManager::Instance());
        }
    }

    if (to == 0x01000000 && from == 0x00010000)
    {
        fn_80119220(AsyncLoadingManager::Instance());
    }
}

LoadingTask sLoadingTask;
