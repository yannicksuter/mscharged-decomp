#include "Game/Task/LoadingTask.h"

#include "types.h"

// The asynchronous loading manager driven by this task is a singleton in the
// following, still unreconstructed translation unit. Only its address is known,
// so it stays opaque here and its methods are referenced by address.
class AsyncLoadingManager;

extern "C" {
AsyncLoadingManager* fn_80118A74();
void fn_80118B50(AsyncLoadingManager*);
u32 fn_80118B7C(AsyncLoadingManager*);
void fn_80119054(AsyncLoadingManager*);
void fn_801190A0(AsyncLoadingManager*);
void fn_801190EC(AsyncLoadingManager*);
void fn_80119138(AsyncLoadingManager*);
void fn_80119184(AsyncLoadingManager*);
void fn_801191D4(AsyncLoadingManager*);
void fn_80119220(AsyncLoadingManager*);

void fn_802C084C(void*, int);
void fn_802C07AC(void*, float);
void fn_8037537C(void*);
void fn_80137668();
}

extern void* lbl_806E1E28;
extern void* lbl_806E2478;
extern u8 lbl_806E1090;
extern u8 lbl_806E0FFC;

void LoadingTask::Start()
{
    mElapsed = 0.0f;
    fn_80118B50(fn_80118A74());
}

void LoadingTask::Run(float dt)
{
    mElapsed += dt;

    fn_802C084C(lbl_806E1E28, 0);
    fn_8037537C(lbl_806E2478);
    fn_802C07AC(lbl_806E1E28, dt);
    fn_80137668();

    switch (fn_80118B7C(fn_80118A74()))
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

void LoadingTask::StateTransition(u32 from, u32 to)
{
    if (to == 0x00100000 && from == 0x00010000)
    {
        fn_80119054(fn_80118A74());
    }

    if (to == 0x00080000)
    {
        fn_801190A0(fn_80118A74());
    }

    if (to == 0x00200000 && from == 0x00000004)
    {
        fn_801190EC(fn_80118A74());
    }

    if (to == 0x00800000 && from == 0x00010000)
    {
        fn_801191D4(fn_80118A74());
    }

    if (to == 0x00400000 && from != 0x02000000)
    {
        if (lbl_806E1090 && lbl_806E0FFC)
        {
            fn_80119184(fn_80118A74());
        }
        else
        {
            fn_80119138(fn_80118A74());
        }
    }

    if (to == 0x01000000 && from == 0x00010000)
    {
        fn_80119220(fn_80118A74());
    }
}

LoadingTask sLoadingTask;
