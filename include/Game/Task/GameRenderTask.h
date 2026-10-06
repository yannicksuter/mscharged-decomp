#ifndef GAME_TASK_GAME_RENDER_TASK_H
#define GAME_TASK_GAME_RENDER_TASK_H

#include "NL/nlTask.h"

class GameRenderTask : public nlTask
{
public:
    GameRenderTask()
        : mElapsedTime(0.0f)
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
    void RenderFrame(float fDeltaT, bool bPictureInPicture);

    float mElapsedTime;
};

extern bool g_bRenderWorld;
extern u8 g_bRenderWorldEffects;

void SetRenderWorldEffects(unsigned int enabled);
bool IsWarbleParticleRenderingEnabled();
void InstallImageRenderCallback();

#endif // GAME_TASK_GAME_RENDER_TASK_H
