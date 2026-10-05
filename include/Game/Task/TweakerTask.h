#ifndef GAME_TWEAKER_TASK_H
#define GAME_TWEAKER_TASK_H

#include "NL/nlTask.h"

class TweakerTask : public nlTask
{
public:
    virtual void Run(float dt);
    virtual const char* GetName()
    {
        return "Tweaker";
    }
};

extern bool g_bTweaking;

void ToggleTweaking();

extern s32 gTweakerButton_806DF2E0;
extern s32 gTweakerButton_806DF2E4;
extern s32 gTweakerButton_806DF2E8;
extern s32 gTweakerButton_806DF2EC;
extern s32 gTweakerButton_806DF2F0;
extern s32 gTweakerButton_806DF2F4;
extern s32 gTweakerButton_806DF2F8;
extern s32 gTweakerButton_806DF2FC;

#endif // GAME_TWEAKER_TASK_H
