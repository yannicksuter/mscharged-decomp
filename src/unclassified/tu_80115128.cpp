#include "NL/nlTask.h"

#include "types.h"

class GameRenderTask : public nlTask
{
public:
    virtual void Run(float fDeltaT);
    virtual const char* GetName() { return "Game Render"; }

private:
    float mValue_20;
};

typedef void (*RenderModelCallback)();

extern bool g_bRenderWorld;
extern RenderModelCallback lbl_806E2044;
extern "C" void fn_8011530C();

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
