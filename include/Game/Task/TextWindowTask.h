#ifndef GAME_TEXT_WINDOW_TASK_H
#define GAME_TEXT_WINDOW_TASK_H

#include "NL/nlTask.h"

class TextWindowTask : public nlTask
{
public:
    TextWindowTask()
        : mActive(false)
        , mPrevious(0xFFFFFFFF)
        , mNext(0xFFFFFFFF)
        , mPreviousAlternate(0xFFFFFFFF)
        , mNextAlternate(0xFFFFFFFF)
        , mSelect(0xFFFFFFFF)
        , mHeld(0xFFFFFFFF)
    {
        sInstance = this;
    }

    virtual void Run(float dt);
    virtual const char* GetName() { return "TextWindow"; }

    static TextWindowTask* sInstance;

private:
    bool mActive;
    u32 mPrevious;
    u32 mNext;
    u32 mPreviousAlternate;
    u32 mNextAlternate;
    u32 mSelect;
    u32 mHeld;
};

extern "C" void fn_802BD820(const char* format, ...);

#endif // GAME_TEXT_WINDOW_TASK_H
