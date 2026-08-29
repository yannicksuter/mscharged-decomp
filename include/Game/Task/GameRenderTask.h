#ifndef GAME_TASK_GAME_RENDER_TASK_H
#define GAME_TASK_GAME_RENDER_TASK_H

#include "NL/nlTask.h"

class GameRenderTask : public nlTask
{
public:
    virtual void Run(float fDeltaT);
    virtual const char* GetName() { return "Game Render"; }

private:
    float mValue_20;
};

extern bool g_bRenderWorld;

#endif // GAME_TASK_GAME_RENDER_TASK_H
