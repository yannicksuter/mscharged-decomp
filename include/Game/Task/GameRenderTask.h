#ifndef GAME_TASK_GAME_RENDER_TASK_H
#define GAME_TASK_GAME_RENDER_TASK_H

#include "NL/nlTask.h"

class GameRenderTask : public nlTask
{
public:
    GameRenderTask()
        : mValue_20(0.0f)
    {
        sInstance = this;
    }

    ~GameRenderTask()
    {
        sInstance = 0;
    }

    virtual void Run(float fDeltaT);
    virtual const char* GetName() { return "Game Render"; }

    static GameRenderTask* sInstance;

private:
    float mValue_20;
};

extern bool g_bRenderWorld;

#endif // GAME_TASK_GAME_RENDER_TASK_H
