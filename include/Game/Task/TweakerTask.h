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

extern s32 gTweakerAccelButton;
extern s32 gTweakerBackButton;
extern s32 gTweakerToggleButton;
extern s32 gTweakerModifierButton;
extern s32 gTweakerLeftButton;
extern s32 gTweakerRightButton;
extern s32 gTweakerUpButton;
extern s32 gTweakerDownButton;

#endif // GAME_TWEAKER_TASK_H
