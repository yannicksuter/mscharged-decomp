#include "Game/Task/GameRenderTask.h"

#include "types.h"

typedef void (*RenderModelCallback)();

extern RenderModelCallback lbl_806E2044;
extern "C" void fn_8011530C();

GameRenderTask* GameRenderTask::sInstance;

extern "C" void fn_80115504()
{
    lbl_806E2044 = fn_8011530C;
}

void GameRenderTask::Run(float fDeltaT)
{
    mValue_20 += fDeltaT;
}

extern "C" bool fn_80115EB0()
{
    return g_bRenderWorld;
}
